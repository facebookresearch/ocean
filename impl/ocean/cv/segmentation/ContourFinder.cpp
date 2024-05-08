/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/ContourFinder.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameEnlarger.h"
#include "ocean/cv/FrameFilterDilation.h"
#include "ocean/cv/FrameFilterMean.h"
#include "ocean/cv/FrameFilterMorphology.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/cv/segmentation/ContourAnalyzer.h"
#include "ocean/cv/segmentation/Clustering.h"
#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Variance.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

PixelContour ContourFinder::similarityContour(const Frame& frame, const unsigned int window, const PixelContour& roughContour, const unsigned int extraOffset, RandomGenerator& randomGenerator, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(window >= 1u && (window % 2u) == 1u);
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.numberPlanes() == 1u);

	ocean_assert(!roughContour.isEmpty());
	ocean_assert(roughContour.isDistinct());

	switch (frame.channels())
	{
		case 3u:
			return similarityContour8BitPerChannel<3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), window, roughContour, extraOffset, randomGenerator, worker);
	}

	ocean_assert(false && "Invalid frame type!");
	return PixelContour();
}

template <unsigned int tChannels>
PixelContour ContourFinder::similarityContour8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int window, const PixelContour& roughContour, const unsigned int extraOffset, RandomGenerator& randomGenerator, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && width != 0u && height != 0u);
	ocean_assert(window >= 1u && (window % 2u) == 1u);

	ocean_assert(!roughContour.isEmpty());
	ocean_assert(roughContour.isDistinct());

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	// first we apply the mean filter for each frame channel

	Frame meanFrame(FrameType(width, height, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
	FrameFilterMean::filter8BitPerChannel<tChannels>(frame, meanFrame.data<uint8_t>(), width, height, window, framePaddingElements, meanFrame.paddingElements(), worker);

	// now we determine the maximal variances of all fingerprints distributed at the rough contour

	typename Fingerprint<tChannels>::Fingerprints fingerprints;
	fingerprints.reserve(roughContour.pixels().size());

	for (PixelPositions::const_iterator i = roughContour.pixels().begin(); i != roughContour.pixels().end(); ++i)
	{
		fingerprints.emplace_back(frame, frameStrideElements, *i);
	}

	static_assert(sizeof(Fingerprint<tChannels>) == tChannels, "Invalid data type!");

	typename Clustering<tChannels>::Datas clusteringDatas;
	clusteringDatas.reserve(fingerprints.size());

	for (size_t fingerprintIndex = 0; fingerprintIndex < fingerprints.size(); ++fingerprintIndex)
	{
		clusteringDatas.emplace_back((const uint8_t*)(&fingerprints[fingerprintIndex].data()), (unsigned int)(fingerprintIndex));
	}

	typename Clustering<tChannels>::Clusters clusters(Clustering<tChannels>::findOptimalRandomClustering(clusteringDatas, 30u, randomGenerator, 5u).clusters());

	unsigned int maxVariances[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		maxVariances[n] = 100u;
	}

	for (typename Clustering<tChannels>::Clusters::iterator i = clusters.begin(); i != clusters.end(); ++i)
	{
		i->calculateVariance();

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			maxVariances[n] = max(maxVariances[n], i->variance(n));
		}
	}

	if (clusters.size() > 1u)
	{
		if (clusters.size() < 8)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				maxVariances[n] *= 2u;
			}
		}
		else if (clusters.size() < 16)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				maxVariances[n] *= 4u;
			}
		}
		else
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				maxVariances[n] *= 8u;
			}
		}
	}

	// now we create the initial rough mask defined by the rough contour

	Frame mask(FrameType(meanFrame, FrameType::FORMAT_Y8));
	mask.setValue(0xFF);
	CV::Segmentation::MaskCreator::contour2inclusiveMaskByTriangulation(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), roughContour.simplified(), 0x00, worker);

	// we improve the rough contour due to a pyramid-based similarity/fingerprint approach

	Frame fineMask(mask.frameType());
	similarityMaskPyramid8BitPerChannel<tChannels>(meanFrame, mask, roughContour, maxVariances, true, 2u, fineMask, worker);

	// we improve the quality of the mask by several dilation iterations

	const PixelBoundingBox extendedBoundingBox(roughContour.boundingBox().extended(4u, 0u, 0u, width - 1u, height - 1u));

	Frame fineMaskSubFrame = fineMask.subFrame(extendedBoundingBox.left(), extendedBoundingBox.top(), extendedBoundingBox.width(), extendedBoundingBox.height(), Frame::CM_USE_KEEP_LAYOUT);
	FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterDilation::MF_SQUARE_3>(fineMaskSubFrame.data<uint8_t>(), fineMaskSubFrame.width(), fineMaskSubFrame.height(), 4u, 0x00, fineMaskSubFrame.paddingElements(), worker);

	// now we determine the contour of the resulting mask

	CV::PixelPositions outlinePixels4;
	outlinePixels4.reserve(50000);
	CV::Segmentation::MaskAnalyzer::findOutline4(fineMask.data<uint8_t>(), fineMask.width(), fineMask.height(), fineMask.paddingElements(), outlinePixels4, extendedBoundingBox);
	if (outlinePixels4.empty())
	{
		return PixelContour();
	}

	CV::PixelPositions contourPixels;
	if (!CV::Segmentation::MaskAnalyzer::pixels2contour(outlinePixels4, fineMask.width(), fineMask.height(), contourPixels))
	{
		ocean_assert(false && "This must never happen!");
		return PixelContour();
	}

	// finally we improve the contour by adjusting the shaping and location due to individual landmarks around the object

	PixelContour denseContour(contourPixels);
	ocean_assert(denseContour.isDense());

	Vectors2 landmarks;
	Vectors2 adjustedContourSubpixels;

	const unsigned int iterations = 3u;

	for (unsigned int n = 0; n < iterations; ++n)
	{
		landmarks.clear();
		findBorderLandmarks8BitPerChannel<tChannels>(meanFrame.constdata<uint8_t>(), fineMask.constdata<uint8_t>(), width, height, meanFrame.paddingElements(), fineMask.paddingElements(), denseContour, extraOffset, landmarks);

		adjustedContourSubpixels.clear();
		adjustContourWithLandmarks(PixelPosition::pixelPositions2vectors(denseContour.pixels()), landmarks, adjustedContourSubpixels, n < iterations - 1u);

		const Vectors2 equalizedContourSubpixels(ContourAnalyzer::equalizeContourDensity(adjustedContourSubpixels));
		const PixelPositions equalizedContourPixels(PixelPosition::vectors2pixelPositions(equalizedContourSubpixels));

		denseContour = CV::Segmentation::ContourAnalyzer::createDenseContour(equalizedContourPixels);
		ocean_assert(denseContour.isDense());
		ocean_assert(denseContour.isDistinct());

		if (n == iterations - 1u)
		{
			break;
		}

		if (denseContour.simplified().isEmpty())
		{
			return PixelContour();
		}

		fineMask.setValue(0xFF);
		MaskCreator::denseContour2inclusiveMask(fineMask.data<uint8_t>(), fineMask.width(), fineMask.height(), fineMask.paddingElements(), denseContour, 0x00);
	}

	ocean_assert(denseContour.isDense());
	ocean_assert(denseContour.isDistinct());

	return denseContour;
}

bool ContourFinder::findBorderLandmarks(const Frame& frame, const Frame& roughMask, const PixelContour& roughContour, const unsigned int extraOffset, Vectors2& landmarks)
{
	ocean_assert(frame.isValid() && roughMask.isValid());
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u);

	ocean_assert(roughMask.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(frame.isFrameTypeCompatible(FrameType(roughMask, frame.pixelFormat()), true));

	ocean_assert(!roughContour.isEmpty());

	switch (frame.channels())
	{
		case 3u:
			return findBorderLandmarks8BitPerChannel<3u>(frame.constdata<uint8_t>(), roughMask.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), roughMask.paddingElements(), roughContour, extraOffset, landmarks);
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <unsigned int tChannels>
bool ContourFinder::findBorderLandmarks8BitPerChannel(const uint8_t* frame, const uint8_t* roughMask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int roughMaskPaddingElements, const PixelContour& roughContour, const unsigned int extraOffset, Vectors2& landmarks)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int roughMaskStrideElements = width + roughMaskPaddingElements;

	typename Fingerprint<tChannels>::Fingerprints fingerprints;
	fingerprints.reserve(roughContour.size());

	std::vector< std::pair<Vector2, Vector2> > ribs;
	ribs.reserve(roughContour.size());

	for (size_t n = 0; n < roughContour.size(); ++n)
	{
		const unsigned int index = modulo(int(n), int(roughContour.size()));
		const unsigned int indexMinus = modulo(int(index) - 30, int(roughContour.size()));
		const unsigned int indexPlus = modulo(int(index + 30u), int(roughContour.size()));

		const Vector2 position = roughContour[index].vector();
		const Vector2 positionMinus = roughContour[indexMinus].vector();
		const Vector2 positionPlus = roughContour[indexPlus].vector();

		const Vector2 direction(positionPlus - positionMinus);
		Vector2 perpendicularDirection(direction.perpendicular());

		if (!perpendicularDirection.normalize())
		{
			perpendicularDirection = direction.perpendicular();

			if (!perpendicularDirection.normalize())
			{
				ocean_assert(false && "This should never happen!");

				return false;
			}
		}

		const Vector2 perpendicularInwards(roughContour.isCounterClockwise() ? -perpendicularDirection : perpendicularDirection);

		const Vector2 fingerprintPositionOut(position + perpendicularInwards * -20);
		const Vector2 fingerprintPositionIn(position + perpendicularInwards * 30);

		const unsigned int xOut = int(fingerprintPositionOut.x() + Scalar(0.5));
		const unsigned int yOut = int(fingerprintPositionOut.y() + Scalar(0.5));
		const unsigned int xIn = int(fingerprintPositionIn.x() + Scalar(0.5));
		const unsigned int yIn = int(fingerprintPositionIn.y() + Scalar(0.5));

		if (xOut < width && xIn < width && yOut < height && yIn < height
				&& roughMask[yOut * roughMaskStrideElements + xOut] == 0xFF && roughMask[yIn * roughMaskStrideElements + xIn] != 0xFF)
		{
			ribs.emplace_back(fingerprintPositionOut, perpendicularInwards);

			fingerprints.emplace_back(frame, frameStrideElements, PixelPosition(xOut, yOut));
		}
	}

	ocean_assert(ribs.size() == fingerprints.size());

	if (ribs.empty())
	{
		return false;
	}

	ocean_assert(landmarks.empty());
	landmarks.reserve(roughContour.size() / 10);

	const unsigned int randomOffset = RandomI::random(9u);

	// find the fine adjusted position for each 10th position
	for (unsigned int n = 0; n < ribs.size(); n += 10u)
	{
		const unsigned int index = modulo(int(n + randomOffset), int(ribs.size()));
		const Vector2& positionOut(ribs[index].first);
		const Vector2& positionDirection(ribs[index].second);

		// determine the variance for the fingerprints in the direct neighborhood
		VarianceT<unsigned int> varianceObject[tChannels];

		for (int f = -20; f <= 20; ++f)
		{
			// take each fourth fingerprint in the direct neighborhood
			const unsigned int fpIndex = modulo(int(index) + f * 4, int(ribs.size()));

			const Fingerprint<tChannels>& fingerprint = fingerprints[fpIndex];

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				ocean_assert(fingerprint[i] == frame[(unsigned int)(ribs[fpIndex].first.y() + Scalar(0.5)) * frameStrideElements + ((unsigned int)(ribs[fpIndex].first.x() + Scalar(0.5))) * tChannels + i]);

				varianceObject[i].add(fingerprint[i]);
			}
		}

		unsigned int variances[tChannels];
		for (unsigned int i = 0u; i < tChannels; ++i)
		{
			variances[i] = max(10u * 10u, varianceObject[i].variance() * 2u);
		}


		// find the object's border by starting for outside and going inwards along the perpendicular contour direction

		PixelPosition lastTestPosition;
		unsigned int validIterations = 0u;

		for (Scalar t = 0; t < 50; ++t)
		{
			const Vector2 testPosition(positionOut + positionDirection * t);

			const unsigned int x = (unsigned int)(testPosition.x() + Scalar(0.5));
			const unsigned int y = (unsigned int)(testPosition.y() + Scalar(0.5));

			// avoid testing of the same position due to rounding inaccuracies
			if (lastTestPosition == PixelPosition(x, y))
			{
				continue;
			}

			lastTestPosition = PixelPosition(x, y);

			const uint8_t* const testFingerprint = frame + y * frameStrideElements + x * tChannels;

			unsigned int dissimilarityCounter = 0u;
			for (int f = -20; f <= 20; ++f)
			{
				const unsigned int fpIndex = modulo(int(index) + f * 4, int(ribs.size()));

				const Fingerprint<tChannels>& fingerprint = fingerprints[fpIndex];

				for (unsigned int i = 0u; i < tChannels; ++i)
				{
					ocean_assert(fingerprint[i] == frame[(unsigned int)(ribs[fpIndex].first.y() + Scalar(0.5)) * frameStrideElements + ((unsigned int)(ribs[fpIndex].first.x() + Scalar(0.5))) * tChannels + i]);
				}

				if (!fingerprint.isSimilar(testFingerprint, variances))
				{
					++dissimilarityCounter;
				}
			}

			if (dissimilarityCounter >= 38u) // **TODO** why 38?
			{
				++validIterations;

				// only if the this is the third successive iteration we accept this point
				if (validIterations >= 3u)
				{
					landmarks.push_back(positionOut + positionDirection * (t - 3 - Scalar(extraOffset)));
					break;
				}
			}
			else
			{
				// reset the number of valid iterations
				validIterations = 0u;
			}
		}
	}

	return true;
}

bool ContourFinder::adjustContourWithLandmarks(const Vectors2& contour, const Vectors2& landmarks, Vectors2& adjustedContour, const bool aggressive)
{
	if (landmarks.size() < 2)
		return false;

	ocean_assert(adjustedContour.empty());
	adjustedContour.reserve(contour.size());

	for (unsigned int n = 0; n < contour.size(); ++n)
	{
		const Vector2& point = contour[n];

		// find the two nearest points in the landmark set
		Vector2 near0(0, 0);
		Vector2 near1(0, 0);
		Scalar sqrDistance0 = Numeric::maxValue();
		Scalar sqrDistance1 = Numeric::maxValue();

		for (Vectors2::const_iterator i = landmarks.begin(); i != landmarks.end(); ++i)
		{
			const Scalar sqrDistance = point.sqrDistance(*i);

			if (sqrDistance < sqrDistance0)
			{
				sqrDistance1 = sqrDistance0;
				near1 = near0;

				sqrDistance0 = sqrDistance;
				near0 = *i;
			}
			else if (sqrDistance < sqrDistance1)
			{
				sqrDistance1 = sqrDistance;
				near1 = *i;
			}
		}

		// as we have at least 2 landmarks this must never happen
		ocean_assert(sqrDistance0 != Numeric::maxValue() && sqrDistance1 != Numeric::maxValue());

		if (near0 == near1)
		{
			const Vector2 centerPoint((point + near0) * Scalar(0.5));

			const Vector2& pointMinus = contour[modulo(int(n) - 1, int(contour.size()))];
			const Vector2& pointPlus = contour[modulo(int(n) + 1, int(contour.size()))];

			adjustedContour.push_back((centerPoint * 6 + pointMinus * 2 + pointPlus * 2) * Scalar(0.1));
		}
		else
		{
			const Line2 line(near0, (near1 - near0).normalized());
			const Vector2 pointOnLine(line.nearestPoint(point));

			if (aggressive)
			{
				const Vector2& centerPoint = pointOnLine;

				const Vector2& pointMinus = contour[modulo(int(n) - 1, int(contour.size()))];
				const Vector2& pointPlus = contour[modulo(int(n) + 1, int(contour.size()))];

				adjustedContour.push_back((centerPoint * 8 + pointMinus * 1 + pointPlus * 1) * Scalar(0.1));
			}
			else
			{
				const Vector2 centerPoint((pointOnLine + point) * Scalar(0.5));

				const Vector2& pointMinus = contour[modulo(int(n) - 1, int(contour.size()))];
				const Vector2& pointPlus = contour[modulo(int(n) + 1, int(contour.size()))];

				adjustedContour.push_back((centerPoint * 1 + pointMinus * 2 + pointPlus * 2) * Scalar(0.2));
			}
		}
	}

	ocean_assert(adjustedContour.size() == contour.size());
	return true;
}

template <unsigned int tChannels>
void ContourFinder::similarityMaskPyramid8BitPerChannel(const Frame& frame, const Frame& roughMask, const PixelContour& roughContour, const unsigned int variances[tChannels], const bool oneObject, const unsigned int layers, Frame& fineMaskResult, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame.isValid() && roughMask.isValid() && fineMaskResult.isValid());
	ocean_assert(frame.channels() == tChannels && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(frame.width() == roughMask.width() && frame.height() == roughMask.height());
	ocean_assert(frame.width() == fineMaskResult.width() && frame.height() == fineMaskResult.height());
	ocean_assert(frame.pixelOrigin() == roughMask.pixelOrigin() && frame.pixelOrigin() == fineMaskResult.pixelOrigin());

	ocean_assert(roughContour.isDistinct());

	// first we down sample the finest frame until we reach the coarsest pyramid layer
	// on the coarsest pyramid layer we determine the corresponding contour/mask and up-sample the information back to the next finer pyramid layer

	if (layers != 0u)
	{
		// we have not reached the final pyramid layer yet
		// therefore, we create the next pyramid layer from the current frame, mask and contour by halving the sizes

		Frame smallFrame;
		if (!FrameShrinker::downsampleByTwo11(frame, smallFrame, worker))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		Frame smallRoughMask;
		if (!FrameShrinker::downsampleBinayMaskByTwo11(roughMask, smallRoughMask, 766u, worker))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		PixelPositions smallRoughPositions;
		smallRoughPositions.reserve(roughContour.size());
		for (PixelPositions::const_iterator i = roughContour.pixels().begin(); i != roughContour.pixels().end(); ++i)
		{
			smallRoughPositions.emplace_back(i->half());
		}

		// now we ensure that the new (downsampled) contour is distinct again

		PixelContour smallRoughContour(smallRoughPositions);
		smallRoughContour.makeDistinct();

		// and we proceed with the next coarser pyramid layer

		Frame smallFineMaskFrame(smallRoughMask.frameType());
		similarityMaskPyramid8BitPerChannel<tChannels>(smallFrame, smallRoughMask, smallRoughContour, variances, oneObject, layers - 1u, smallFineMaskFrame, worker);

		// now we have the mask result for the current pyramid layer
		// we need to up-sample the result of this pyramid layer for the next finer pyramid layer
		// therefore, we will gather all pixel locations at the border of the mask (inside or outside the mask)

		PixelPositions smallNonUniquePixels;
		MaskAnalyzer::findNonUniquePixels4(smallFineMaskFrame.constdata<uint8_t>(), smallFineMaskFrame.width(), smallFineMaskFrame.height(), smallFineMaskFrame.paddingElements(), smallNonUniquePixels); /// **TODO** use bounding box from contour to improve the computation

		PixelPositions nonUniquePixels;
		nonUniquePixels.reserve(smallNonUniquePixels.size() * 4);

		// we up-sample the locations for the next finder layer (by multiplying the locations by 2 and by filling the gaps on the finer layer)

		for (PixelPositions::const_iterator i = smallNonUniquePixels.begin(); i != smallNonUniquePixels.end(); ++i)
		{
			const PixelPosition twice(i->twice());

			nonUniquePixels.emplace_back(twice);
			nonUniquePixels.emplace_back(twice.east());
			nonUniquePixels.emplace_back(twice.south());
			nonUniquePixels.emplace_back(twice.southEast());
		}

		// we up-sample the mask by a nearest pixel sampling
		CV::FrameEnlarger::multiplyByTwo<uint8_t, 1u>(smallFineMaskFrame.constdata<uint8_t>(), fineMaskResult.data<uint8_t>(), fineMaskResult.width(), fineMaskResult.height(), smallFineMaskFrame.paddingElements(), fineMaskResult.paddingElements(), worker);

		// and we finally fine tune the border of the mask (for all pixels we have up-sampled)
		finetuneSimilarityMask8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), fineMaskResult.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), fineMaskResult.paddingElements(), roughContour, nonUniquePixels, variances, worker);
	}
	else
	{
		// we have reached the coarsest pyramid layer

		fineMaskResult.setValue(0xFFu);

		// apply the fingerprint similarity mask creation
		similarityMask8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), roughMask.constdata<uint8_t>(), fineMaskResult.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), roughMask.paddingElements(), fineMaskResult.paddingElements(), roughContour, variances, worker);

		// remove small paths, bridges and single pixels (on this smallest frame layer only)
		FrameFilterMorphology::openMask<FrameFilterMorphology::MF_SQUARE_3>(fineMaskResult.data<uint8_t>(), fineMaskResult.width(), fineMaskResult.height(), fineMaskResult.paddingElements(), 0x00u);

		Frame separationFrame(FrameType(fineMaskResult, FrameType::FORMAT_Y32));

		if (oneObject)
		{
			// we are interested in one (the largest joined) mask object only

			MaskAnalyzer::MaskBlocks maskBlocks;
			MaskAnalyzer::analyzeMaskSeparation8Bit(fineMaskResult.constdata<uint8_t>(), fineMaskResult.width(), fineMaskResult.height(), fineMaskResult.paddingElements(), separationFrame.data<uint32_t>(), separationFrame.paddingElements(), maskBlocks);
			std::sort(maskBlocks.begin(), maskBlocks.end());

			if (!maskBlocks.empty())
			{
				// we keep that largest mask block

				fineMaskResult.setValue(0xFF);
				MaskCreator::separation2mask(separationFrame.constdata<uint32_t>(), fineMaskResult.width(), fineMaskResult.height(), separationFrame.paddingElements(), maskBlocks.back().id(), fineMaskResult.data<uint8_t>(), fineMaskResult.paddingElements(), 0x00);
			}
		}

		// now our determined mask block(s) may still have holes inside them
		// therefore, we fill all holes inside the mask block(s) by analyzing the non-mask areas and check whether they reach the frame border or not

		MaskAnalyzer::MaskBlocks nonMaskBlocks;
		MaskAnalyzer::analyzeNonMaskSeparation8Bit(fineMaskResult.constdata<uint8_t>(), fineMaskResult.width(), fineMaskResult.height(), fineMaskResult.paddingElements(), separationFrame.data<uint32_t>(), separationFrame.paddingElements(), nonMaskBlocks);

		if (!nonMaskBlocks.empty())
		{
			std::vector<uint8_t> ids(nonMaskBlocks.size() + 1, 0);

			for (MaskAnalyzer::MaskBlocks::const_iterator i = nonMaskBlocks.begin(); i != nonMaskBlocks.end(); ++i)
			{
				if (!i->border())
				{
					ocean_assert(size_t(i->id()) < ids.size());
					ids[i->id()] = 1u;
				}
			}

			CV::Segmentation::MaskCreator::separations2mask(separationFrame.constdata<uint32_t>(), fineMaskResult.width(), fineMaskResult.height(), separationFrame.paddingElements(), ids.data(), ids.size(), 0x00, fineMaskResult.data<uint8_t>(), fineMaskResult.paddingElements());
		}
	}
}

template <unsigned int tChannels>
void ContourFinder::similarityMask8BitPerChannel(const uint8_t* frame, const uint8_t* roughMask, uint8_t* fineMask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int roughMaskPaddingElements, const unsigned int fineMaskPaddingElements, const PixelContour& roughContour, const unsigned int variances[tChannels], Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && roughMask != nullptr && fineMask != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(roughContour.isDistinct());

	ocean_assert(!roughContour.isEmpty());
	if (roughContour.isEmpty())
	{
		return;
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int roughMaskStrideElements = width + roughMaskPaddingElements;
	const unsigned int fineMaskStrideElements = width + fineMaskPaddingElements;

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0u; y <height; ++y)
	{
		const uint8_t* fineMaskRow = fineMask + y * fineMaskStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			ocean_assert(fineMaskRow[x] == 0xFF);
		}
	}
#endif

	const PixelBoundingBox& boundingBox = roughContour.boundingBox();
	const PixelPositions& fingerprintPositions = roughContour.pixels();

	typename Fingerprint<tChannels>::Fingerprints fingerprints;
	fingerprints.reserve(fingerprintPositions.size());

	for (size_t n = 0; n < fingerprintPositions.size(); ++n)
	{
		const PixelPosition& fingerprintPosition = fingerprintPositions[n];

		ocean_assert(fingerprintPosition.x() < width && fingerprintPosition.y() < height);
		fingerprints.emplace_back(frame, frameStrideElements, fingerprintPosition);
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&ContourFinder::similarityMask8BitPerChannelSubset<tChannels>, frame, roughMask, fineMask, width, height, frameStrideElements, roughMaskStrideElements, fineMaskStrideElements, (const Fingerprint<tChannels>*)(fingerprints.data()), fingerprints.size(), variances, boundingBox.left(), boundingBox.width(), 0u, 0u), boundingBox.top(), boundingBox.height());
	}
	else
	{
		similarityMask8BitPerChannelSubset<tChannels>(frame, roughMask, fineMask, width, height, frameStrideElements, roughMaskStrideElements, fineMaskStrideElements, fingerprints.data(), fingerprints.size(), variances, boundingBox.left(), boundingBox.width(), boundingBox.top(), boundingBox.height());
	}
}

template <unsigned int tChannels>
void ContourFinder::finetuneSimilarityMask8BitPerChannel(const uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelContour& roughContour, const CV::PixelPositions& positions, const unsigned int variances[tChannels], Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert_and_suppress_unused(width != 0u && height != 0u, height);

	ocean_assert(roughContour.isDistinct());

	ocean_assert(!roughContour.isEmpty() && !positions.empty());
	if (roughContour.isEmpty() || positions.empty())
	{
		return;
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

#ifdef OCEAN_DEBUG
	for (PixelPositions::const_iterator i = positions.begin(); i != positions.end(); ++i)
	{
		ocean_assert(i->x() < width && i->y() < height);
	}
#endif

	const PixelPositions& fingerprintPositions = roughContour.pixels();

	typename Fingerprint<tChannels>::Fingerprints fingerprints;
	fingerprints.reserve(fingerprintPositions.size());

	for (size_t n = 0; n < fingerprintPositions.size(); ++n)
	{
		const PixelPosition& fingerprintPosition = fingerprintPositions[n];

		ocean_assert(fingerprintPosition.x() < width && fingerprintPosition.y() < height);
		fingerprints.emplace_back(frame, frameStrideElements, fingerprintPosition);
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&ContourFinder::finetuneSimilarityMask8BitPerChannelSubset<tChannels>, frame, mask, frameStrideElements, maskStrideElements, (const Fingerprint<tChannels>*)(fingerprints.data()), fingerprints.size(), variances, positions.data(), 0u, 0u), 0u, (unsigned int)(positions.size()));
	}
	else
	{
		finetuneSimilarityMask8BitPerChannelSubset<tChannels>(frame, mask, frameStrideElements, maskStrideElements, fingerprints.data(), fingerprints.size(), variances, positions.data(), 0u, (unsigned int)positions.size());
	}
}

template <unsigned int tChannels>
void ContourFinder::similarityMask8BitPerChannelSubset(const uint8_t* frame, const uint8_t* roughMask, uint8_t* fineMask, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int roughMaskStrideElements, const unsigned int fineMaskStrideElements, const Fingerprint<tChannels>* fingerprints, const size_t numberFingerprints, const unsigned int variances[tChannels], const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr && roughMask != nullptr && fineMask != nullptr);
	ocean_assert(fingerprints != nullptr);

	ocean_assert_and_suppress_unused(firstColumn + numberColumns <= width, width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* frameRow = frame + y * frameStrideElements + firstColumn * tChannels;

		const uint8_t* roughMaskRow = roughMask + y * roughMaskStrideElements + firstColumn;
		uint8_t* fineMaskRow = fineMask + y * fineMaskStrideElements + firstColumn;

		for (unsigned int n = 0u; n < numberColumns; ++n)
		{
			// check if the rough mask pixel is a mask pixel
			if (*roughMaskRow != 0xFF)
			{
				unsigned int numberSimilar = 0u;

				for (size_t f = 0; f < numberFingerprints && numberSimilar < 7u; ++f) // **TODO** 7 -> function parameter
				{
					if (fingerprints[f].isSimilar(frameRow, variances))
					{
						++numberSimilar;
					}
				}

				if (numberSimilar < 7u)  // **TODO** 7 -> function parameter
				{
					*fineMaskRow = 0x00;
				}
			}

			++roughMaskRow;
			++fineMaskRow;
			frameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void ContourFinder::finetuneSimilarityMask8BitPerChannelSubset(const uint8_t* frame, uint8_t* mask, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const Fingerprint<tChannels>* fingerprints, const size_t numberFingerprints, const unsigned int variances[tChannels], const CV::PixelPosition* positions, const unsigned int firstPosition, const unsigned int numberPositions)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(fingerprints != nullptr && variances != nullptr && positions != nullptr);

	for (unsigned int n = firstPosition; n < firstPosition + numberPositions; ++n)
	{
		const CV::PixelPosition& position = positions[n];

		ocean_assert(frameStrideElements >= position.x() * tChannels);
		ocean_assert(maskStrideElements >= position.x());

		const unsigned int maskIndex = position.y() * maskStrideElements + position.x();

		if (mask[maskIndex] != 0xFF)
		{
			unsigned int numberSimilar = 0u;

			for (size_t f = 0; f < numberFingerprints && numberSimilar < 7u; ++f)
			{
				if (fingerprints[f].isSimilar(frame, frameStrideElements, position, variances)) // **TODO** 7 -> function parameter
				{
					++numberSimilar;
				}
			}

			if (numberSimilar < 7u)  // **TODO** 7 -> function parameter
			{
				mask[maskIndex] = 0x00;
			}
			else
			{
				mask[maskIndex] = 0xFF;
			}
		}
	}
}

}

}

}
