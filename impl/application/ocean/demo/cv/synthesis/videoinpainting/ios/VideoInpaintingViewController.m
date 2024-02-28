// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/synthesis/videoinpainting/ios/VideoInpaintingViewController.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/HomographyTracker.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/cv/synthesis/CreatorInpaintingContentF1.h"
#include "ocean/cv/synthesis/InitializerHomographyMappingAdaptionF1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodReferenceFrameF1.h"
#include "ocean/cv/synthesis/SynthesisPyramidI1.h"
#include "ocean/cv/synthesis/SynthesisPyramidF1.h"

#include "ocean/geometry/Homography.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/AdapterFrameMedium.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "ocean/platform/osx/Resource.h"
#include "ocean/platform/osx/Utilities.h"

#include "ocean/system/Process.h"

VideoInpainting::VideoInpainting() :
	inpaintingRemoving(false),
	inpaintingTotalHomography(true)
{
	// nothing to do here
}

bool VideoInpainting::onFrame(const LegacyFrame& inputFrame, LegacyFrame& outputFrame)
{
	ocean_assert(inputFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	static HighPerformanceStatistic performance;

	if (performance.measurements() % 20u == 0u)
	{
		Log::info() << performance.averageMseconds() << "ms";
		performance.reset();
	}

	ocean_assert(Thread::threadPriority() == Thread::PRIORTY_REALTIME);

	const HighPerformanceStatistic::ScopedStatistic scope(performance);

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	LegacyFrame yFrame;
	if (!CV::FrameConverter::Comfort::convert(inputFrame, FrameType(inputFrame, FrameType::FORMAT_Y8), yFrame, false, scopedWorker()))
		return false;

	LegacyFrame yuvFrame;
	if (!CV::FrameConverter::Comfort::convert(inputFrame, FrameType(inputFrame, FrameType::FORMAT_YUV24), yuvFrame, true, scopedWorker()))
		return false;

	TemporaryScopedLock scopedLockMaskPoint(inpaintingMaskPointLock);
		const CV::PixelPosition newMaskPoint(inpaintingNewMaskPoint);
		inpaintingNewMaskPoint = CV::PixelPosition();
	scopedLockMaskPoint.release();

	const ScopedLock scopedLock(inpaintingLock);

	if (inpaintingRemoving)
		inpaint(yFrame, yuvFrame, newMaskPoint, scopedWorker());
	else
		defineMask(yFrame, yuvFrame, newMaskPoint, scopedWorker());

	outputFrame = std::move(yuvFrame);

	outputFrame.setTimestamp(inputFrame.timestamp());
	outputFrame.setRelativeTimestamp(inputFrame.relativeTimestamp());

	return true;
}

void VideoInpainting::inpaint(const LegacyFrame& yFrame, LegacyFrame& yuvFrame, const CV::PixelPosition newMaskPoint, Worker* worker)
{
	ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	ocean_assert(yuvFrame.pixelFormat() == FrameType::FORMAT_YUV24);
	ocean_assert(yuvFrame.pixelOrigin() == yFrame.pixelOrigin());
	ocean_assert(yuvFrame.width() == yFrame.width() && yuvFrame.height() == yFrame.height());

	if (inpaintingHomographyContourPoints.empty())
		return;

	const HighPerformanceTimer timer;

	SquareMatrix3 homography(true);
	if (inpaintingHomographyTracker.trackPoints(yFrame, yFrame, inpaintingRandomGenerator, inpaintingHomographyContourPoints, homography, worker))
		inpaintingHomographyContourPoints = CV::Advanced::HomographyTracker::transformPoints(inpaintingHomographyContourPoints, homography);


	for (unsigned int n = 0; n < inpaintingHomographyContourPoints.size(); ++n)
		if ((unsigned int)inpaintingHomographyContourPoints[n].x() >= yFrame.width() || (unsigned int)inpaintingHomographyContourPoints[n].y() >= yFrame.height())
		{
			inpaintingTotalHomography.toIdentity();
			inpaintingInpaintFrame.release();
			return;
		}

	// determine homography
	inpaintingTotalHomography = inpaintingTotalHomography * homography.inverted();
	Geometry::Homography::normalizeHomography(inpaintingTotalHomography);


	// create the tracking mask according to the tracked object contour
	inpaintingTrackingMask.set(FrameType(yuvFrame, FrameType::FORMAT_Y8));
	memset(inpaintingTrackingMask.data(), 0xFF, inpaintingTrackingMask.size());


	CV::Segmentation::PixelContour contour(CV::PixelPosition::vectors2pixelPositions(inpaintingHomographyContourPoints, yFrame.width(), yFrame.height()));
	contour.makeDistinct();
	contour.makeDense();

	CV::Segmentation::MaskCreator::contour2inclusiveMask(inpaintingTrackingMask.data(), inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), contour, 0x00);

	// if this is the first frame to be inpainted
	if (!inpaintingInpaintFrame)
	{
		//performanceInitialInpainting.start();

		CV::Synthesis::SynthesisPyramidI1 newSynthesisPixel;
		newSynthesisPixel.oldInitialize(yuvFrame, inpaintingTrackingMask, worker);
		//newSynthesisPixel._initialize(topLeft, trackingMask, &worker, false, false);
		//newSynthesisPixel.applyInpainting(randomGenerator, CV::Synthesis::SynthesisPyramidI1::IT_PATCH_RANDOM_1, 10u, 25u, 50 * 50, &worker); // **TODO** faster

		HighPerformanceTimer timer;
		newSynthesisPixel.applyInpainting(inpaintingRandomGenerator, CV::Synthesis::SynthesisPyramidI1::IT_PATCH_REGION_2, 5u, 26u, 0xFFFFFFFF, worker); // **TODO** faster



		inpaintingInpaintFrame = yuvFrame; // **TODO** move operator

		newSynthesisPixel.createInpaintingResult(inpaintingInpaintFrame, worker);

		CV::FrameShrinker::divideByTwo(inpaintingInpaintFrame, inpaintingInpaintFrame_4, worker);
		CV::FrameShrinker::divideByTwo(inpaintingInpaintFrame_4, worker);

		//previousSynthesisLayer = newSynthesisPixel.layers().back(); // **TODO** move operator
		inpaintingPreviousMapping = std::move(newSynthesisPixel.layers().back().mapping()); // **TODO** swap

		//performanceInitialInpainting.stop();

		//setFrame(inpaintFrame);
	}
	else
	{
		const CV::PixelBoundingBox trackingMaskBoundingBox(contour.boundingBox());


		// determine the inner distance between the mask border
		CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(inpaintingTrackingMask.data(), inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), 6u, false, trackingMaskBoundingBox, worker);


		// prepare reference frame
		createReferenceFrame(yuvFrame, trackingMaskBoundingBox, contour.pixels(), worker);


		// initialize synthesis layer
		//CV::Synthesis::LayerF1 newSynthesisPixelLayer(LegacyFrame(topLeft, true), LegacyFrame(trackingMask, true), previousSynthesisLayer, homography.inverted(), trackingMaskBoundingBox); // remove frame copy

		LegacyFrame tempYUVFrame(yuvFrame, true);
		LegacyFrame tempMaskFrame(inpaintingTrackingMask, true);
		CV::Synthesis::LayerF1 newSynthesisPixelLayer(tempYUVFrame, tempMaskFrame, trackingMaskBoundingBox); // remove frame copy
		CV::Synthesis::InitializerHomographyMappingAdaptionF1(newSynthesisPixelLayer, inpaintingPreviousMapping, inpaintingRandomGenerator, homography.inverted()).invoke(worker);


		//performanceInnerSynthesis.start();
		//CV::Synthesis::LayerEngineF::optimize(CV::Synthesis::OptimizerReferenceFrame4NeighborhoodF1<5u, 25u>(randomGenerator, referenceFrame), newSynthesisPixelLayer, 5u, 1u, 0xFFFFFFFF, &worker);
		CV::Synthesis::Optimizer4NeighborhoodReferenceFrameF1<5u, 25u, true>(newSynthesisPixelLayer, inpaintingRandomGenerator, inpaintingReferenceFrame).invoke(5u, 1u, 0xFFFFFFFF, worker, true);
		//newSynthesisPixelLayer.applyLayerReferenceIteration(referenceFrame, randomGenerator, 1u, 5u, 25u, &worker);
		//performanceInnerSynthesis.stop();


		LegacyFrame resultFrame = yuvFrame;
		//CV::Synthesis::LayerEngineF::create(CV::Synthesis::CreatorInpaintingContentF1(), newSynthesisPixelLayer, resultFrame, &worker);
		CV::Synthesis::CreatorInpaintingContentF1(newSynthesisPixelLayer, resultFrame).invoke(worker);
		//newSynthesisPixelLayer.createHQFrame3Channel24Bit(resultFrame, trackingMask);


		// create the final image
		const CV::PixelPosition boundingBoxTopLeft(trackingMaskBoundingBox.topLeft());
		const uint8_t* mask = inpaintingTrackingMask.constdata<uint8_t>() + boundingBoxTopLeft.index(inpaintingTrackingMask.width());
		const uint8_t* original = yuvFrame.constdata<uint8_t>() + boundingBoxTopLeft.index(yuvFrame.width()) * 3u;
		uint8_t* result = resultFrame.data<uint8_t>() + boundingBoxTopLeft.index(resultFrame.width()) * 3u;

		for (unsigned int y = trackingMaskBoundingBox.top(); y < trackingMaskBoundingBox.bottomEnd(); ++y)
		{
			for (unsigned int x = trackingMaskBoundingBox.left(); x < trackingMaskBoundingBox.rightEnd(); ++x)
			{
				if (*mask > 0u && *mask <= 5u)
				{
					const unsigned int factor = *mask * 1024u / 6u;
					const unsigned int factor_ = 1024u - factor;

					*(result + 0) = (*(original + 0) * factor_ + *(result + 0) * factor) >> 10u;
					*(result + 1) = (*(original + 1) * factor_ + *(result + 1) * factor) >> 10u;
					*(result + 2) = (*(original + 2) * factor_ + *(result + 2) * factor) >> 10u;
				}

				++mask;
				original += 3;
				result += 3;
			}

			mask += inpaintingTrackingMask.width() - trackingMaskBoundingBox.width();
			original += (inpaintingTrackingMask.width() - trackingMaskBoundingBox.width()) * 3u;
			result += (inpaintingTrackingMask.width() - trackingMaskBoundingBox.width()) * 3u;
		}

		//previousSynthesisLayer = newSynthesisPixelLayer; // **TODO** move operator
		inpaintingPreviousMapping = std::move(newSynthesisPixelLayer.mapping()); // **TODO** move operator
		//previousMapping = newSynthesisPixelLayer.mapping(); // **TODO** move operator


		yuvFrame = std::move(resultFrame); // **TODO** avoid copy
	}
}

void VideoInpainting::createReferenceFrame(const LegacyFrame& yuvFrame, const Ocean::CV::PixelBoundingBox& trackingMaskBoundingBox, const CV::PixelPositions& contourPoints, Worker* worker)
{
	// create the reference frame for the current image
	inpaintingReferenceFrame = yuvFrame;

	if (trackingMaskBoundingBox && (trackingMaskBoundingBox.width() == 0u || trackingMaskBoundingBox.height() == 0u))
	{
		ocean_assert(false && "**TODO**");
		return;
	}

	CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolateHomography(inpaintingInpaintFrame, inpaintingReferenceFrame, inpaintingTrackingMask, inpaintingTotalHomography, trackingMaskBoundingBox, worker);

	CV::FrameShrinker::divideByTwo(yuvFrame, inpaintingYUVFrame_4, worker); // **TODO** masked shrinking
	CV::FrameShrinker::divideByTwo(inpaintingYUVFrame_4, worker);

	// gather position deltas
	PositionDeltas positionDeltas;
	CV::PixelPosition last;
	const uint8_t* const dataInpaint_4 = inpaintingInpaintFrame_4.constdata<uint8_t>();
	const uint8_t* const dataTopLeft_4 = inpaintingYUVFrame_4.constdata<uint8_t>();
	for (CV::PixelPositions::const_iterator i = contourPoints.begin(); i != contourPoints.end(); ++i)
	{
		CV::PixelPosition position((i->x() + 2u) / 4u, (i->y() + 2u) / 4u);

		if (position != last)
		{
			const Vector2 previousPoint((inpaintingTotalHomography * i->vector()) * 0.25);
			const Vector2 clippedPoint(minmax(Scalar(0), previousPoint.x(), Scalar(inpaintingYUVFrame_4.width() - 1u)),
									   minmax(Scalar(0), previousPoint.y(), Scalar(inpaintingYUVFrame_4.height() - 1u)));

			const CV::PixelPosition inpaintPoint(CV::PixelPosition::vector2pixelPosition(clippedPoint));

			const unsigned int inpaintIndex3 = inpaintPoint.index(inpaintingYUVFrame_4.width()) * 3u;
			const unsigned int topLeftIndex3 = position.index(inpaintingYUVFrame_4.width()) * 3u;

			positionDeltas.push_back(PositionDelta(position.vector(), dataInpaint_4 + inpaintIndex3, dataTopLeft_4 + topLeftIndex3));
			last = position;
		}
	}

	// shrink the gathered position deltas
	while (positionDeltas.size() / 2u > 30)
	{
		PositionDeltas newPositionDeltas;
		for (unsigned int n = 1u; n < positionDeltas.size(); n += 2)
			newPositionDeltas.push_back(PositionDelta(positionDeltas[n - 1u], positionDeltas[n]));
		positionDeltas = std::move(newPositionDeltas);
	}




	ocean_assert(trackingMaskBoundingBox.width() >= 10u);
	ocean_assert(trackingMaskBoundingBox.height() >= 10u);

	Vectors3 interpolatedDeltas;
	interpolatedDeltas.reserve(121);

	const Scalar left = Scalar(trackingMaskBoundingBox.left());
	const Scalar top = Scalar(trackingMaskBoundingBox.top());
	const Scalar xFactor = Scalar((trackingMaskBoundingBox.width()) * Scalar(0.1));
	const Scalar yFactor = Scalar((trackingMaskBoundingBox.height()) * Scalar(0.1));
	const Scalar diagonal_4 = Numeric::sqrt(Numeric::sqr(Scalar(trackingMaskBoundingBox.width())) + Numeric::sqr(Scalar(trackingMaskBoundingBox.height()))) * Scalar(0.25);

	for (unsigned int y = 0u; y <= 10u; ++y)
		for (unsigned int x = 0u; x <= 10u; ++x)
		{
			const Scalar positionX = left + Scalar(x) * xFactor;
			const Scalar positionY = top + Scalar(y) * yFactor;
			ocean_assert(positionX >= Scalar(trackingMaskBoundingBox.left()) && positionX <= Scalar(trackingMaskBoundingBox.rightEnd()));
			ocean_assert(positionY >= Scalar(trackingMaskBoundingBox.top()) && positionY <= Scalar(trackingMaskBoundingBox.bottomEnd()));

			const Vector2 position(positionX, positionY);
			const Vector2 position_4(position * Scalar(0.25));

			Scalar deltas0 = 0;
			Scalar deltas1 = 0;
			Scalar deltas2 = 0;
			Scalar weights = 0;

			for (PositionDeltas::const_iterator i = positionDeltas.begin(); i != positionDeltas.end(); ++i)
			{
				//const Scalar w = Numeric::exp(-Numeric::sqrt(Numeric::sqrt((unsigned int)(p.sqrDistance(pos))))); // <- more ideal but slow
				//const Scalar w = Numeric::exp(-Numeric::sqrt(p.sqrDistance(pos)));

				const Scalar sqrDistance = position_4.sqrDistance(*i);
				const Scalar weight = sqrDistance > 0 ? diagonal_4 / Numeric::sqrt(sqrDistance) : diagonal_4;

				weights += weight;
				deltas0 += i->delta0 * weight;
				deltas1 += i->delta1 * weight;
				deltas2 += i->delta2 * weight;
			}

			interpolatedDeltas.push_back(Vector3(deltas0 / weights, deltas1 / weights, deltas2 / weights));
		}
	ocean_assert(interpolatedDeltas.size() == 121);





	//Frame referenceFrame2(referenceFrame, true);
	uint8_t* const dataReference = inpaintingReferenceFrame.data<uint8_t>();

	const CV::PixelPosition boundingBoxTopLeft(trackingMaskBoundingBox.topLeft());
	const uint8_t* mask = inpaintingTrackingMask.constdata<uint8_t>() + int(boundingBoxTopLeft.index(inpaintingTrackingMask.width())) - 1;

	for (unsigned int y = trackingMaskBoundingBox.top(); y < trackingMaskBoundingBox.bottomEnd(); ++y)
	{
		for (unsigned int x = trackingMaskBoundingBox.left(); x < trackingMaskBoundingBox.rightEnd(); ++x)
			if (*++mask != 0xFF)
			{
				ocean_assert(x >= trackingMaskBoundingBox.left());
				ocean_assert(y >= trackingMaskBoundingBox.top());

				const unsigned int leftBin = (x - trackingMaskBoundingBox.left()) * 10u / trackingMaskBoundingBox.width();
				const unsigned int topBin = (y - trackingMaskBoundingBox.top()) * 10u / trackingMaskBoundingBox.height();
				ocean_assert(leftBin < 10u);
				ocean_assert(topBin < 10u);

				const Vector3& pTopLeft = interpolatedDeltas[topBin * 11u + leftBin + 0u];
				const Vector3& pTopRight = interpolatedDeltas[topBin * 11u + leftBin + 1u];

				const Vector3& pBottomLeft = interpolatedDeltas[topBin * 11u + leftBin + 11u + 0u];
				const Vector3& pBottomRight = interpolatedDeltas[topBin * 11u + leftBin + 11u + 1u];

				const Scalar tx = (Scalar(x) - (left + Scalar(leftBin) * xFactor)) / xFactor;
				const Scalar ty = (Scalar(y) - (top + Scalar(topBin) * yFactor)) / yFactor;
				ocean_assert(tx >= 0 && tx <= 1);
				ocean_assert(ty >= 0 && ty <= 1);

				const Scalar tx_ = 1 - tx;
				const Scalar ty_ = 1 - ty;

				const Vector3 pTop(pTopLeft * tx_ + pTopRight * tx);
				const Vector3 pBottom(pBottomLeft * tx_ + pBottomRight * tx);
				const Vector3 pFinal(pTop * ty_ + pBottom * ty);

				const int delta0 = Numeric::round32(pFinal[0]);
				const int delta1 = Numeric::round32(pFinal[1]);
				const int delta2 = Numeric::round32(pFinal[2]);

				unsigned char* const reference = dataReference + 3u * (y * yuvFrame.width() + x);
				*(reference + 0) = minmax(0, *(reference + 0) + delta0, 255);
				*(reference + 1) = minmax(0, *(reference + 1) + delta1, 255);
				*(reference + 2) = minmax(0, *(reference + 2) + delta2, 255);
			}

		mask += inpaintingTrackingMask.width() - trackingMaskBoundingBox.width();
	}

	mask = inpaintingTrackingMask.constdata<uint8_t>() + boundingBoxTopLeft.index(inpaintingTrackingMask.width());
	const uint8_t* original = yuvFrame.constdata<uint8_t>() + boundingBoxTopLeft.index(yuvFrame.width()) * 3u;
	uint8_t* reference = inpaintingReferenceFrame.data<uint8_t>() + boundingBoxTopLeft.index(inpaintingReferenceFrame.width()) * 3u;

	for (unsigned int y = trackingMaskBoundingBox.top(); y < trackingMaskBoundingBox.bottomEnd(); ++y)
	{
		for (unsigned int x = trackingMaskBoundingBox.left(); x < trackingMaskBoundingBox.rightEnd(); ++x)
		{
			if (*mask > 0u && *mask <= 5u)
			{
				const unsigned int factor = *mask * 1024u / 6u;
				const unsigned int factor_ = 1024u - factor;

				*(reference + 0) = (*(original + 0) * factor_ + *(reference + 0) * factor) >> 10u;
				*(reference + 1) = (*(original + 1) * factor_ + *(reference + 1) * factor) >> 10u;
				*(reference + 2) = (*(original + 2) * factor_ + *(reference + 2) * factor) >> 10u;
			}

			++mask;
			original += 3;
			reference += 3;
		}

		mask += inpaintingTrackingMask.width() - trackingMaskBoundingBox.width();
		original += (inpaintingTrackingMask.width() - trackingMaskBoundingBox.width()) * 3u;
		reference += (inpaintingTrackingMask.width() - trackingMaskBoundingBox.width()) * 3u;
	}
}

void VideoInpainting::defineMask(const LegacyFrame& yFrame, LegacyFrame& yuvFrame, const CV::PixelPosition newMaskPoint, Worker* worker)
{
	ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	ocean_assert(yuvFrame.pixelFormat() == FrameType::FORMAT_YUV24);
	ocean_assert(yuvFrame.pixelOrigin() == yFrame.pixelOrigin());
	ocean_assert(yuvFrame.width() == yFrame.width() && yuvFrame.height() == yFrame.height());

	SquareMatrix3 homography;
	if (inpaintingHomographyTracker.trackPoints(yFrame, yFrame, inpaintingRandomGenerator, inpaintingHomographyContourPoints, homography, worker))
		inpaintingHomographyContourPoints = CV::Advanced::HomographyTracker::transformPoints(inpaintingHomographyContourPoints, homography);

	inpaintingTrackingMask.set(yFrame.frameType());
	memset(inpaintingTrackingMask.data(), 0xFF, inpaintingTrackingMask.size());

	CV::Segmentation::PixelContour contour(CV::PixelPosition::vectors2pixelPositions(inpaintingHomographyContourPoints, yFrame.width(), yFrame.height()));
	contour.makeDistinct();
	contour.makeDense();

	CV::Segmentation::MaskCreator::contour2inclusiveMask(inpaintingTrackingMask.data(), inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), contour, 0x00);

	if (newMaskPoint.isValid())
	{
		const unsigned char value = 0x00;
		CV::Canvas::ellipse8BitPerChannel<1u>(inpaintingTrackingMask.data(), inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), newMaskPoint, 51u, 51u, &value);

		CV::PixelPositions outlinePixels4;
		CV::Segmentation::MaskAnalyzer::findBorderPixels4(inpaintingTrackingMask.constdata<uint8_t>(), inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), outlinePixels4);

		CV::PixelPositions pixelContour;
		pixelContour.reserve(outlinePixels4.size());
		CV::Segmentation::MaskAnalyzer::outlinePixels2contour2(outlinePixels4, inpaintingTrackingMask.width(), inpaintingTrackingMask.height(), pixelContour);

		inpaintingHomographyContourPoints = CV::PixelPosition::pixelPositions2vectors(pixelContour);
		contour = CV::Segmentation::PixelContour(pixelContour);
		contour.makeDistinct();
		contour.makeDense();
	}

	const uint8_t* maskData = inpaintingTrackingMask.constdata<uint8_t>();
	uint8_t* yuvFrameData = yuvFrame.data<uint8_t>();

	for (unsigned int n = 0; n < inpaintingTrackingMask.pixels(); ++n)
		if (maskData[n] == 0x00)
		{
			yuvFrameData[3 * n + 0u];
			yuvFrameData[3 * n + 1u] = 0xFF;
			yuvFrameData[3 * n + 2u] >>= 1u;
		}

	for (unsigned int n = 0; n < contour.pixels().size(); ++n)
	{
		const CV::PixelPosition& pos = contour.pixels()[n];

		yuvFrameData[3u * (pos.y() * yuvFrame.width() + pos.x()) + 0u] = 0x00;
		yuvFrameData[3u * (pos.y() * yuvFrame.width() + pos.x()) + 1u] = 0x00;
		yuvFrameData[3u * (pos.y() * yuvFrame.width() + pos.x()) + 2u] = 0xFF;
	}
}

void VideoInpainting::addMaskPoint(const Vector2& point)
{
	ocean_assert(point.x() >= 0 && point.y() >= 0);

	const CV::PixelPosition pixelPosition(Numeric::round32(point.x()), Numeric::round32(point.y()));

	const ScopedLock scopedLock(inpaintingMaskPointLock);
	inpaintingNewMaskPoint = pixelPosition;
}

void VideoInpainting::reset()
{
	const ScopedLock scopedLock(inpaintingLock);

	inpaintingHomographyContourPoints.clear();
	inpaintingRemoving = false;
	inpaintingTrackingMask.release();
	inpaintingTotalHomography.toIdentity();

	inpaintingInpaintFrame.release();
	inpaintingInpaintFrame_4.release();
	inpaintingReferenceFrame.release();
	inpaintingPreviousMapping.reset();
	inpaintingYUVFrame_4.release();


	const ScopedLock scopedLockMaskPoint(inpaintingMaskPointLock);
	inpaintingNewMaskPoint = CV::PixelPosition();
}

void VideoInpainting::startRemove()
{
	if (inpaintingTrackingMask.isNull())
		return;

	inpaintingRemoving = true;
}

Vector2 VideoInpainting::view2video(const double viewWidth, const double viewHeight, const Scalar viewFovX, const PinholeCamera& videoCamera, const Vector2& point)
{
	const PinholeCamera viewCamera((unsigned int)viewWidth, (unsigned int)viewHeight, viewFovX);

	const Vector3 objectPoint = viewCamera.ray(point, HomogenousMatrix4(true)).point(1);
	const Vector2 projectedObjectPoint = videoCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false);

	return projectedObjectPoint;
}

@interface VideoInpaintingViewController ()
{
	/// The abstract rendering engine.
	Ocean::Rendering::EngineRef renderingEngine;

	/// The abstract framebuffer in which the result will be rendered.
	Ocean::Rendering::FramebufferRef renderingFramebuffer;

	/// The rendering view.
	Ocean::Rendering::PerspectiveViewRef renderingView;

	/// The undistorted background.
	Ocean::Rendering::UndistortedBackgroundRef renderingUndistortedBackground;
}

@end

@implementation VideoInpaintingViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

	// we aquire any existing rendering engine using OpenGL ES (use individual param for other graphics API)
	renderingEngine = Ocean::Rendering::Manager::get().engine("", Ocean::Rendering::Engine::API_OPENGLES);
	// TODO: renderingEngine = Ocean::Rendering::Manager::get().engine("", Ocean::Rendering::Engine::API_METAL);

	// we ensure that we have a valid rendering engine before we proceed
	if (renderingEngine.isNull())
		return;

	// we create a framebuffer in which we will draw the content
	renderingFramebuffer = renderingEngine->createFramebuffer();

	// we create a view with perspective projection model
	renderingView = renderingEngine->factory().createPerspectiveView();

	if (renderingView.isNull())
		return;

	// we set the initial horizontal viewing angle of the view (however, this fov will be adjusted later so that it perfectly fits with the video-background)
	renderingView->setFovX(Ocean::Numeric::deg2rad(35));
	// we define the background as black (however, due to the video background its color will not be visible)
	renderingView->setBackgroundColor(Ocean::RGBAColor(0, 0, 0));

	// we connect the view with the framebuffer
	renderingFramebuffer->setView(renderingView);

	Ocean::Media::AdapterFrameMediumRef adapterVideo = Ocean::Media::Manager::get().newMedium("AdapterLiveVideo", Ocean::Media::Medium::ADAPTER_FRAME_MEDIUM);

	// we create a live video medium for our background (and e.g., for the tracker aquired by "BACKGROUND" in the TrackerTransform node)
	// as the name of the camera device may change we take the first one
	Ocean::Media::FrameMediumRef liveVideo = Ocean::Media::Manager::get().newMedium("LiveVideoId:0", Ocean::Media::Medium::LIVE_VIDEO);

	if (liveVideo)
	{
		// we define a preferred frame dimension (however, the camera device may provide a different dimension if this dimension is not suitable/available)
		liveVideo->setPreferredFrameDimension(640u, 480u);

		// we start the camera device
		liveVideo->start();
	}

	if (adapterVideo)
	{
		// as we have a valid video object we now can create the corresponding background, which will be added to the view
		renderingUndistortedBackground = renderingEngine->factory().createUndistortedBackground();

		if (renderingUndistortedBackground.isNull())
			return;

		adapterVideo->setInputMedium(liveVideo);
		adapterVideo->setFrameCallback(Media::AdapterFrameMedium::FrameCallback::create(VideoInpainting::get(), &VideoInpainting::onFrame));

		// we start the camera device
		adapterVideo->start();

		const Timestamp startTimestamp(true);
		while (startTimestamp + 2 > Timestamp(true))
		{
			const FrameRef frame = liveVideo->frame();
			if (frame)
			{
				IO::CameraCalibrationManager::get().registerCalibration(adapterVideo->url(), IO::CameraCalibrationManager::get().camera(liveVideo->url(), frame->width(), frame->height()));
				break;
			}
		}

		// we connect the background to the live video
		renderingUndistortedBackground->setMedium(adapterVideo);

		// we connect the background to the view
		renderingView->addBackground(renderingUndistortedBackground);
	}

	Thread::setThreadPriority(Thread::PRIORITY_IDLE);
}

- (void)update
{
	if (renderingFramebuffer && renderingEngine)
	{
		renderingEngine->update(Ocean::Timestamp(true));
	}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	if (renderingFramebuffer.isNull() || renderingView.isNull())
		return;

	ocean_assert(Thread::threadPriority() == Thread::PRIORITY_IDLE);

	const double width = self.view.bounds.size.width;
	const double height = self.view.bounds.size.height;

	if (NumericD::isEqualEps(height))
		return;

	/*const int intWidth = NumericD::round32(width);
	const int intHeight = NumericD::round32(height);

	ocean_assert(double(intWidth) == width);
	ocean_assert(double(intHeight) == height);
	ocean_assert(intWidth > 0 && intHeight > 0);

	renderingFramebuffer->setViewport(0u, 0u, (unsigned int)intWidth, (unsigned int)intHeight);*/

	const double aspect = fabs(width / height);
	renderingView->setAspectRatio(aspect);

	// now we determine the ideal field of view for our device (the field of view that will be rendered should be slightly smaller than the field of view of the camera)
	const Ocean::Scalar idealFovX = renderingView->idealFovX();

	if (idealFovX != renderingView->fovX())
	{
		renderingView->setFovX(idealFovX);
		Ocean::Log::info() << "The field of view has been adjusted to " << Ocean::Numeric::rad2deg(idealFovX) << " degree.";
	}

	// we simply invoke the rendering of the framebuffer
	renderingFramebuffer->render();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (!renderingUndistortedBackground || !renderingView)
		return;

	if ([touches count] != 1)
		return;

	CGPoint viewPoint = [[touches anyObject] locationInView:[self view]];
	CGSize viewSize = [[self view] frame].size;

	const Vector2 videoPoint = VideoInpainting::view2video(viewSize.width, viewSize.height, renderingView->fovX(), renderingUndistortedBackground->camera(), Vector2(viewPoint.x, viewPoint.y));

	VideoInpainting::get().reset();
	VideoInpainting::get().addMaskPoint(videoPoint);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (!renderingUndistortedBackground || !renderingView)
		return;

	if ([touches count] != 1)
		return;

	CGPoint viewPoint = [[touches anyObject] locationInView:[self view]];
	CGSize viewSize = [[self view] frame].size;

	const Vector2 videoPoint = VideoInpainting::view2video(viewSize.width, viewSize.height, renderingView->fovX(), renderingUndistortedBackground->camera(), Vector2(viewPoint.x, viewPoint.y));

	VideoInpainting::get().addMaskPoint(videoPoint);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (!renderingUndistortedBackground || !renderingView)
		return;

	if ([touches count] != 1)
		return;

	CGPoint viewPoint = [[touches anyObject] locationInView:[self view]];
	CGSize viewSize = [[self view] frame].size;

	const Vector2 videoPoint = VideoInpainting::view2video(viewSize.width, viewSize.height, renderingView->fovX(), renderingUndistortedBackground->camera(), Vector2(viewPoint.x, viewPoint.y));

	VideoInpainting::get().startRemove();
}

@end
