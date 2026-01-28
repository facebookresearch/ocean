/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/BullseyeDetectorMono.h"
#include "ocean/cv/detector/bullseyes/BullseyesDebugElements.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/FramePyramid.h"

#include <ocean/base/Messenger.h>

#define OCEAN_USE_OLD_RADIAL_CHECK 1


namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

bool BullseyeDetectorMono::Parameters::isValid() const noexcept
{
	return framePyramidLayers_ >= 1u;
}

unsigned int BullseyeDetectorMono::Parameters::framePyramidPixelThreshold() const noexcept
{
	return framePyramidPixelThreshold_;
}

void BullseyeDetectorMono::Parameters::setFramePyramidPixelThreshold(unsigned int framePyramidPixelThreshold) noexcept
{
	framePyramidPixelThreshold_ = framePyramidPixelThreshold;
}

unsigned int BullseyeDetectorMono::Parameters::framePyramidLayers() const noexcept
{
	return framePyramidLayers_;
}

void BullseyeDetectorMono::Parameters::setFramePyramidLayers(unsigned int framePyramidLayers) noexcept
{
	framePyramidLayers_ = framePyramidLayers;
}

bool BullseyeDetectorMono::Parameters::useAdaptiveRowSpacing() const noexcept
{
	return useAdaptiveRowSpacing_;
}

void BullseyeDetectorMono::Parameters::setUseAdaptiveRowSpacing(bool useAdaptiveRowSpacing) noexcept
{
	useAdaptiveRowSpacing_ = useAdaptiveRowSpacing;
}

unsigned int BullseyeDetectorMono::Parameters::minimumSegmentSize() const noexcept
{
	return minimumSegmentSize_;
}

void BullseyeDetectorMono::Parameters::setMinimumSegmentSize(unsigned int minimumSegmentSize) noexcept
{
	ocean_assert(minimumSegmentSize >= 1u);
	minimumSegmentSize_ = minimumSegmentSize;
}

BullseyeDetectorMono::Parameters BullseyeDetectorMono::Parameters::defaultParameters() noexcept
{
	return Parameters();
}

bool BullseyeDetectorMono::detectBullseyes(const Frame& yFrame, Bullseyes& bullseyes, const Parameters& parameters, Worker* worker)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	if (!yFrame.isValid() || yFrame.pixelFormat() != FrameType::FORMAT_Y8)
	{
		return false;
	}

	ocean_assert(yFrame.width() >= 21u && yFrame.height() >= 21u);
	if (yFrame.width() < 21u || yFrame.height() < 21u)
	{
		return false;
	}

	ocean_assert(parameters.isValid());
	if (!parameters.isValid())
	{
		return false;
	}

	unsigned int framePyramidLayers = 1u;
	if (yFrame.width() * yFrame.height() >= parameters.framePyramidPixelThreshold())
	{
		framePyramidLayers = parameters.framePyramidLayers();
	}

	constexpr bool copyFirstLayer = false;
	const CV::FramePyramid yFramePyramid(yFrame, CV::FramePyramid::DM_FILTER_14641, framePyramidLayers, copyFirstLayer, worker);

	ocean_assert(yFramePyramid.isValid());
	if (!yFramePyramid.isValid())
	{
		return false;
	}

	Bullseyes localBullseyes;
	localBullseyes.reserve(16);

	for (unsigned int layer = 0u; layer < yFramePyramid.layers(); ++layer)
	{
		const Frame& yFrameLayer = yFramePyramid.layer(layer);

		if (yFrameLayer.height() < 21u || yFrameLayer.width() < 21u)
		{
			continue;
		}

		Bullseyes newBullseyes;
		newBullseyes.reserve(4);

		if (worker && yFrameLayer.height() >= 600u)
		{
			Lock multiThreadLock;
			worker->executeFunction(Worker::Function::createStatic(&BullseyeDetectorMono::detectBullseyesSubset, &yFrameLayer, &newBullseyes, &multiThreadLock, parameters.useAdaptiveRowSpacing(), parameters.minimumSegmentSize(), layer, 0u, 0u), 10u, yFrameLayer.height() - 20u);
		}
		else
		{
			detectBullseyesSubset(&yFrameLayer, &newBullseyes, nullptr, parameters.useAdaptiveRowSpacing(), parameters.minimumSegmentSize(), layer, 10u, yFrameLayer.height() - 20u);
		}

		for (const Bullseye& newBullseye : newBullseyes)
		{
			// Bullseyes are already upscaled to original image coordinates in detectBullseyesInRow()
			ocean_assert(newBullseye.isValid());
			if (!newBullseye.isValid())
			{
				continue;
			}

			// Non-maximum suppression (kind of): add new bullseye if no other bullseye is too close
			bool addNewBullseye = true;

			for (const Bullseye& bullseye : localBullseyes)
			{
				const Scalar sqrDistance = bullseye.position().sqrDistance(newBullseye.position());

				if (sqrDistance < (newBullseye.radius() * newBullseye.radius()) || sqrDistance < (bullseye.radius() * bullseye.radius()))
				{
					addNewBullseye = false;
					break;
				}
			}

			if (addNewBullseye)
			{
				localBullseyes.emplace_back(newBullseye);
			}
		}
	}

	bullseyes = std::move(localBullseyes);

	return true;
}

void BullseyeDetectorMono::detectBullseyesSubset(const Frame* yFrame, Bullseyes* bullseyes, Lock* multiThreadLock, const bool useAdaptiveRowSpacing, const unsigned int minimumSegmentSize, const unsigned int pyramidLayer, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr && yFrame->isValid() && yFrame->pixelFormat() == FrameType::FORMAT_Y8);

	// Adaptive row spacing
	//
	// If a candidate is found in a row, it is likely to be found in the next few rows as well. These candidates will be
	// merged later using something like non-maximum suppression. Instead of merging them later, adaptive row spacing
	// only processes every n-th image row, essentially skipping some of the candidates that would be merged later
	// anyway. Skipping some rows doesn't affect the overall detection performance (especially when used in conjunction
	// with a frame pyramd) while it drastically reduces the overall computation time.
	//
	// Image height : row spacing
	//        < 300 : 1
	//        < 450 : 2
	//        < 600 : 3
	//        < 750 : 4
	//        < 900 : 5
	//        >= 900 : 6
	const unsigned int rowSpacing = useAdaptiveRowSpacing ? std::min(6u, std::max(1u, yFrame->height() / 150u)) : 1u;
	ocean_assert(rowSpacing >= 1u);

	Bullseyes localBullseyes;
	for (unsigned int y = firstRow; y < firstRow + numberRows; y += rowSpacing)
	{
		detectBullseyesInRow(*yFrame, y, localBullseyes, minimumSegmentSize, pyramidLayer);
	}

	const OptionalScopedLock scopedLock(multiThreadLock);

	bullseyes->insert(bullseyes->end(), localBullseyes.cbegin(), localBullseyes.cend());
}

void BullseyeDetectorMono::detectBullseyesInRow(const Frame& yFrame, const unsigned int y, Bullseyes& bullseyes, const unsigned int minimumSegmentSize, const unsigned int pyramidLayer)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(y < yFrame.height());
	ocean_assert(y >= 10u && y < yFrame.height() - 10u);
	ocean_assert(yFrame.width() >= 21u && yFrame.height() >= 21u);

	// Scale factor for this pyramid layer: 2^pyramidLayer
	const Scalar scale = Scalar(1u << pyramidLayer);

	const unsigned int height = yFrame.height();
	const unsigned int width = yFrame.width();

	const uint8_t* const yRow = yFrame.constrow<uint8_t>(y);

	// we are looking for the following pattern: W B W B W B W with ratio N:1:1:1:1:1:M (with N, M >= 1)

	// the first segment must be black

	unsigned int segment_1_start_black = (unsigned int)(-1);
	unsigned int segment_2_start_white = (unsigned int)(-1);

	unsigned int x = 1u;

	// start segment 1: we search for the start of the first black segment (with white pixel to the left)

	TransitionHistory history;
	while (x < width && !TransitionHistory::isTransitionToBlack(yRow + x, history))
	{
		++x;
	}

	if (x == width)
	{
		return;
	}

	ocean_assert(segment_1_start_black == (unsigned int)(-1));
	segment_1_start_black = x;

	while (x < width)
	{
		// start segment 2: we search for the start of the first white segment
		if (segment_2_start_white == (unsigned int)(-1))
		{
			history.reset();
			while (x < width && !TransitionHistory::isTransitionToWhite(yRow + x, history))
			{
				++x;
			}

			if (x == width)
			{
				break;
			}

			ocean_assert(segment_2_start_white == (unsigned int)(-1));
			segment_2_start_white = x;
		}

		ocean_assert(segment_1_start_black < segment_2_start_white);
		const unsigned int segment_1_size = segment_2_start_white - segment_1_start_black;

		const unsigned int blackRingSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int blackRingSegmentMax = max(segment_1_size + 3u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		// start segment 3: we search for the start of the second black segment (the center dot)

		history.reset();
		while (x < width && !TransitionHistory::isTransitionToBlack(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_2_start_white < x);
		const unsigned int segment_2_size = x - segment_2_start_white;

		// now we can check whether the first black and first white segment had almost identical sizes

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < x);

		const unsigned int whiteRingSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int whiteRingSegmentMax = max(segment_1_size + 6u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		if (segment_2_size < whiteRingSegmentMin || segment_2_size > whiteRingSegmentMax)
		{
			// the first two segments are too different, so we discard the first two segments
			// and we can use the current third segment (black) as new first segment

			segment_1_start_black = x;
			segment_2_start_white = (unsigned int)(-1);

			// x stays untouched

			continue;
		}

		const unsigned int segment_3_start_black = x;

		// start segment 4: we search for the start of the second white segment

		history.reset();
		while (x < width && !TransitionHistory::isTransitionToWhite(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_3_start_black < x);
		const unsigned int segment_3_size = x - segment_3_start_black;

		const unsigned int segment_4_start_white = x;

		// now we can check whether the third segment has the same size as the first two segments (or is slightly smaller)

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < segment_3_start_black);
		ocean_assert(segment_3_start_black < segment_4_start_white);

		const unsigned int dotSegmentMin = max((unsigned int)max(1, int(blackRingSegmentMin) - 3), blackRingSegmentMin * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int dotSegmentMax = blackRingSegmentMax;

		if (segment_3_size < dotSegmentMin || segment_3_size > dotSegmentMax)
		{
			// the size of the center block does not match with the size of the first two blocks
			// so we discard the first two segments and we use the center segment as new first black segment
			// and the start of the forth segment as start of the second segment

			segment_1_start_black = segment_3_start_black;
			segment_2_start_white = segment_4_start_white;

			// we also have to reset x
			x = segment_2_start_white;

			continue;
		}

		// start segment 5: we search for the start of the last black segment

		history.reset();
		while (x < width && !TransitionHistory::isTransitionToBlack(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_4_start_white < x);
		const unsigned int segment_4_size = x - segment_4_start_white;

		if (segment_4_size < whiteRingSegmentMin || segment_4_size > whiteRingSegmentMax)
		{
			// the forth segment (the last white segment) does not have the correct size (not 1/3 of the third size)

			// in case the forth segment has same size as the third segment, we take the third and forth segment as new first and second
			// otherwise, we take the start of the fifth segment as very first black segment

			if (segment_4_size < segment_3_size * 384u / 512u || segment_4_size > segment_3_size * 640u / 512u) // 0.75 ~ 384/512,   1.25 ~ 640/512
			{
				// third and forth segment don't match

				segment_1_start_black = x;
				segment_2_start_white = (unsigned int)(-1);

				// x stays untouched
			}
			else
			{
				// third and forth segment match

				segment_1_start_black = segment_3_start_black;
				segment_2_start_white = segment_4_start_white;

				// we also have to reset x
				x = segment_2_start_white;
			}

			continue;
		}

		const unsigned int segment_5_start_black = x;

		// start 'segment 6': we search for the start of the next white segment (the end of the last black segment + 1 pixel)

		history.reset();
		while (x < width && !TransitionHistory::isTransitionToWhite(yRow + x, history))
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_5_start_black < x);
		const unsigned int segment_5_size = x - segment_5_start_black;

		// finally we can check whether also the last black segment has the correct size

		if (segment_5_size >= blackRingSegmentMin && segment_5_size <= blackRingSegmentMax)
		{
			// we have a valid combination of segments

			// Early filtering: reject segments that are too small for reliable threshold computation
			if (segment_1_size < minimumSegmentSize || segment_2_size < minimumSegmentSize || segment_3_size < minimumSegmentSize || segment_4_size < minimumSegmentSize || segment_5_size < minimumSegmentSize)
			{
				// One or more segments are too small - skip this candidate

				segment_1_start_black = segment_3_start_black;
				segment_2_start_white = segment_4_start_white;
				x = segment_2_start_white;

				continue;
			}

			BullseyesDebugElements::get().drawBullseyeCandidateInRow(y, segment_1_start_black, segment_1_size, segment_2_size, segment_3_size, segment_4_size, segment_5_size, scale);

			const unsigned int xCenter = (segment_3_start_black + segment_4_start_white + 1u) / 2u;

			const unsigned int grayThreshold = determineThreshold(yRow + segment_1_start_black, segment_1_size, segment_2_size, segment_3_size, segment_4_size, segment_5_size);

			// let's ensure that the center pixel actually fits with the determined gray threshold
			if (grayThreshold < 255u && *yFrame.constpixel<uint8_t>(xCenter, y) <= grayThreshold)
			{
				// the diameter of the bullseye including the left and right black segment
				const unsigned int diameter = x - segment_1_start_black;
				ocean_assert(diameter >= 5u);

				const unsigned int diameter3_4 = (diameter * 3u + 2u) / 4u;

				// now we check whether we can find the same bullseye pattern in vertical direction

				if (xCenter >= diameter3_4 && y >= diameter3_4 && xCenter < width - diameter3_4 && y < height - diameter3_4
						&& checkBullseyeInColumn(yFrame, xCenter, y, grayThreshold, blackRingSegmentMin, blackRingSegmentMax, whiteRingSegmentMin, whiteRingSegmentMax, dotSegmentMin, dotSegmentMax))
				{
					const float whiteRingRadius = float(segment_3_size) * 0.5f + float(segment_2_size + segment_4_size) * 0.25f;
					const float blackRingRadius = whiteRingRadius + float(segment_2_size + segment_4_size) * 0.25f + float(segment_1_size + segment_5_size) * 0.25f;
					const float whiteBorderRadius = blackRingRadius * 1.5f;

					if (checkBullseyeInNeighborhood(yFrame, xCenter, y, grayThreshold, whiteRingRadius, blackRingRadius, whiteBorderRadius))
					{
						BullseyesDebugElements::get().drawCheckBullseyeInNeighborhood(y, xCenter, scale, diameter);

						Vector2 location;
						if (determineAccurateBullseyeLocation(yFrame, xCenter, y, grayThreshold, location))
						{
							const float radius = float(diameter) * 0.5f;

							ocean_assert(location.x() >= Scalar(radius) && location.y() >= Scalar(radius));
							ocean_assert(location.x() < Scalar(width) - Scalar(radius) && location.y() < Scalar(height) - Scalar(radius));

							bullseyes.emplace_back(location * scale, Scalar(radius) * scale, grayThreshold, pyramidLayer);
						}
					}
				}
			}
		}

		// in any case (either if the last segment does not have the correct size, or if we found a valid segment combination)
		// we use the last black segment as new first black segment

		segment_1_start_black = segment_3_start_black;
		segment_2_start_white = segment_4_start_white;

		// we also have to reset x
		x = segment_2_start_white;
	}
}

template <bool tFindBlackPixel>
bool BullseyeDetectorMono::findNextUpperPixel(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int maximalRows, const unsigned int threshold, unsigned int& rows)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(x < yFrame.width() && y < yFrame.height());
	ocean_assert(maximalRows != 0u);

	if (y == 0u)
	{
		return false;
	}

	rows = 0u;
	unsigned int currentY = y;

	while (int(--currentY) >= 0 && ++rows <= maximalRows)
	{
		const uint8_t* pixel = yFrame.constpixel<uint8_t>(x, currentY);
		const bool condition = tFindBlackPixel ? (int(*pixel) > int(threshold)) : (int(*pixel) < int(threshold));

		if (!condition)
		{
			break;
		}
	}

	return int(currentY) >= 0 && rows <= maximalRows;
}

template <bool tFindBlackPixel>
bool BullseyeDetectorMono::findNextLowerPixel(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int maximalRows, const unsigned int threshold, unsigned int& rows)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(x < yFrame.width() && y < yFrame.height());
	ocean_assert(maximalRows != 0u);

	if (y >= yFrame.height() - 1u)
	{
		return false;
	}

	rows = 0u;
	unsigned int currentY = y;

	while (++currentY < yFrame.height() && ++rows <= maximalRows)
	{
		const uint8_t* pixel = yFrame.constpixel<uint8_t>(x, currentY);
		const bool condition = tFindBlackPixel ? (int(*pixel) > int(threshold)) : (int(*pixel) < int(threshold));

		if (!condition)
		{
			break;
		}
	}

	return currentY < yFrame.height() && rows <= maximalRows;
}

unsigned int BullseyeDetectorMono::determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5)
{
	unsigned int sumBlack = 0u;
	unsigned int sumWhite = 0u;

	sumWhite += *(yPosition - 1);

	for (unsigned int n = 0u; n < segmentSize1; ++n)
	{
		sumBlack += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize2; ++n)
	{
		sumWhite += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize3; ++n)
	{
		sumBlack += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize4; ++n)
	{
		sumWhite += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize5; ++n)
	{
		sumBlack += *yPosition++;
	}

	sumWhite += *yPosition;

	const unsigned int averageBlack = sumBlack / (segmentSize1 + segmentSize3 + segmentSize5);
	const unsigned int averageWhite = sumWhite / (segmentSize2 + segmentSize4 + 2u);

	if (averageBlack + 2u >= averageWhite)
	{
		// the separate between bright and dark pixels is not strong enough
		return (unsigned int)(-1);
	}

	return (averageBlack + averageWhite + 1u) / 2u;
}

bool BullseyeDetectorMono::checkBullseyeInColumn(const Frame& yFrame, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackRingSegmentMin, const unsigned int blackRingSegmentMax, const unsigned int whiteRingSegmentMin, const unsigned int whiteRingSegmentMax, const unsigned int dotSegmentMin, const unsigned int dotSegmentMax)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(xCenter < yFrame.width() && yCenter < yFrame.height());

	// the black center dot

	const unsigned int dotSegmentMaxHalf = (dotSegmentMax + 1u) / 2u;
	const unsigned int dotSegmentSymmetricTolerance = max(1u, dotSegmentMax * 179u / 512u); // ~ 35%

	unsigned int topRows;
	unsigned int bottomRows;
	if (!findNextUpperPixel<false>(yFrame, xCenter, yCenter, dotSegmentMaxHalf + 1u, threshold, topRows)
		|| !findNextLowerPixel<false>(yFrame, xCenter, yCenter, dotSegmentMaxHalf + 1u, threshold, bottomRows)
		|| NumericT<unsigned int>::isNotEqual(topRows, bottomRows, dotSegmentSymmetricTolerance)
		|| topRows + bottomRows - 1u < dotSegmentMin // (topRows + bottomRows - 1): as top and bottom include the center pixel
		|| topRows + bottomRows - 1u > dotSegmentMax)
	{
		return false;
	}

	// the inner white ring

	unsigned int upperStart = yCenter - topRows;
	unsigned int lowerStart = yCenter + bottomRows;

	if (!findNextUpperPixel<true>(yFrame, xCenter, upperStart, whiteRingSegmentMax + 1u, threshold, topRows)
		|| !findNextLowerPixel<true>(yFrame, xCenter, lowerStart, whiteRingSegmentMax + 1u, threshold, bottomRows)
		|| topRows < whiteRingSegmentMin || bottomRows < whiteRingSegmentMin)
	{
		return false;
	}

	ocean_assert(topRows >= whiteRingSegmentMin && topRows <= whiteRingSegmentMax + 1u);
	ocean_assert(bottomRows >= whiteRingSegmentMin && bottomRows <= whiteRingSegmentMax + 1u);

	// the outer black ring

	upperStart -= topRows;
	lowerStart += bottomRows;

	if (!findNextUpperPixel<false>(yFrame, xCenter, upperStart, blackRingSegmentMax + 1u, threshold, topRows)
		|| !findNextLowerPixel<false>(yFrame, xCenter, lowerStart, blackRingSegmentMax + 1u, threshold, bottomRows)
		|| topRows < blackRingSegmentMin || bottomRows < blackRingSegmentMin)
	{
		return false;
	}

	ocean_assert(topRows >= blackRingSegmentMin && topRows <= blackRingSegmentMax + 1u);
	ocean_assert(bottomRows >= blackRingSegmentMin && bottomRows <= blackRingSegmentMax + 1u);

	return true;
}

bool BullseyeDetectorMono::checkBullseyeInNeighborhood(const Frame& yFrame, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const float whiteRingRadius, const float blackRingRadius, const float whiteBorderRadius)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame.width() >= 21u && yFrame.height() >= 21u);

	ocean_assert(float(xCenter) + whiteBorderRadius + 0.5f < float(yFrame.width()));
	ocean_assert(float(xCenter) - whiteBorderRadius + 0.5f > 0.0f);

	ocean_assert(float(yCenter) + whiteBorderRadius + 0.5f < float(yFrame.height()));
	ocean_assert(float(yCenter) - whiteBorderRadius + 0.5f > 0.0f);

	ocean_assert(whiteRingRadius >= 1.0f);
	ocean_assert(whiteRingRadius + 1.0f <= blackRingRadius);
	ocean_assert(blackRingRadius + 1.0f <= whiteBorderRadius);

	constexpr unsigned int numberLookupLocations = 3u;

	const VectorT2<unsigned int> offsets1024[numberLookupLocations] =
	{
		VectorT2<unsigned int>(392u, 946u), // = (0.3826834323, 0.9238795325) * 1024, 22.5 deg
		VectorT2<unsigned int>(724u, 724u), // = (0.7071067811, 0.7071067811) * 1024, 45.0 deg
		VectorT2<unsigned int>(946u, 392u), // = (0.9238795325, 0.3826834323) * 1024, 67.5 deg
	};

	const unsigned int whiteRingRadius1024 = (unsigned int)(whiteRingRadius * 1024.0f + 0.5f);
	const unsigned int blackRingRadius1024 = (unsigned int)(blackRingRadius * 1024.0f + 0.5f);
	const unsigned int whiteBorderRadius1024 = (unsigned int)(whiteBorderRadius * 1024.0f + 0.5f);

	// inner white ring
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		const unsigned int testY1 = yCenter - (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX1 = xCenter - (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX1, testY1) < threshold)
		{
			return false;
		}

		const unsigned int testY2 = yCenter + (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX2 = xCenter - (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX2, testY2) < threshold)
		{
			return false;
		}

		const unsigned int testY3 = yCenter - (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX3 = xCenter + (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX3, testY3) < threshold)
		{
			return false;
		}

		const unsigned int testY4 = yCenter + (whiteRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX4 = xCenter + (whiteRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX4, testY4) < threshold)
		{
			return false;
		}
	}

	// black ring
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		const unsigned int testY1 = yCenter - (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX1 = xCenter - (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX1, testY1) > threshold)
		{
			return false;
		}

		const unsigned int testY2 = yCenter + (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX2 = xCenter - (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX2, testY2) > threshold)
		{
			return false;
		}

		const unsigned int testY3 = yCenter - (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX3 = xCenter + (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX3, testY3) > threshold)
		{
			return false;
		}

		const unsigned int testY4 = yCenter + (blackRingRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX4 = xCenter + (blackRingRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX4, testY4) > threshold)
		{
			return false;
		}
	}

	// outer white border
	for (unsigned int n = 0u; n < numberLookupLocations; ++n)
	{
		const unsigned int testY1 = yCenter - (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX1 = xCenter - (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX1, testY1) < threshold)
		{
			return false;
		}

		const unsigned int testY2 = yCenter + (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX2 = xCenter - (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX2, testY2) < threshold)
		{
			return false;
		}

		const unsigned int testY3 = yCenter - (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX3 = xCenter + (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX3, testY3) < threshold)
		{
			return false;
		}

		const unsigned int testY4 = yCenter + (whiteBorderRadius1024 * offsets1024[n].y() + 524288u) / 1048576u;
		const unsigned int testX4 = xCenter + (whiteBorderRadius1024 * offsets1024[n].x() + 524288u) / 1048576u;
		if (*yFrame.constpixel<uint8_t>(testX4, testY4) < threshold)
		{
			return false;
		}
	}

	return true;
}

bool BullseyeDetectorMono::determineAccurateBullseyeLocation(const Frame& yFrame, const unsigned int xBullseye, const unsigned int yBullseye, const unsigned int threshold, Vector2& location)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame.width() >= 21u && yFrame.height() >= 21u);
	ocean_assert(xBullseye < yFrame.width() && yBullseye < yFrame.height());

	ocean_assert(*yFrame.constpixel<uint8_t>(xBullseye, yBullseye) <= threshold);

	// first, we identify the first left, right, top, and bottom pixel that do not match with the threshold anymore

	unsigned int bullseyeLeft = xBullseye - 1u; // exclusive location

	while (bullseyeLeft < yFrame.width() && *yFrame.constpixel<uint8_t>(bullseyeLeft, yBullseye) <= threshold)
	{
		--bullseyeLeft;
	}

	if (bullseyeLeft >= yFrame.width())
	{
		return false;
	}

	unsigned int bullseyeRight = xBullseye + 1u; // exclusive location

	while (bullseyeRight < yFrame.width() && *yFrame.constpixel<uint8_t>(bullseyeRight, yBullseye) <= threshold)
	{
		++bullseyeRight;
	}

	if (bullseyeRight >= yFrame.width())
	{
		return false;
	}

	unsigned int bullseyeTop = yBullseye - 1u; // exclusive location

	while (bullseyeTop < yFrame.height() && *yFrame.constpixel<uint8_t>(xBullseye, bullseyeTop) <= threshold)
	{
		--bullseyeTop;
	}

	if (bullseyeTop >= yFrame.height())
	{
		return false;
	}

	unsigned int bullseyeBottom = yBullseye + 1u; // exclusive location

	while (bullseyeBottom < yFrame.height() && *yFrame.constpixel<uint8_t>(xBullseye, bullseyeBottom) <= threshold)
	{
		++bullseyeBottom;
	}

	if (bullseyeBottom >= yFrame.height())
	{
		return false;
	}

	// now, we determine the sub-pixel borders:
	// (out - in) / 1 == (out - threshold) / x
	// x = (out - threshold) / (out - in)

	// left border
	const float leftIn = float(*yFrame.constpixel<uint8_t>(bullseyeLeft + 1u, yBullseye));
	const float leftOut = float(*yFrame.constpixel<uint8_t>(bullseyeLeft + 0u, yBullseye));
	ocean_assert(leftIn <= float(threshold) && leftOut > float(threshold));

	ocean_assert(leftOut - leftIn >= NumericF::eps());
	const float leftBorder = float(bullseyeLeft) + (leftOut - float(threshold)) / (leftOut - leftIn);


	// right border
	const float rightIn = float(*yFrame.constpixel<uint8_t>(bullseyeRight - 1u, yBullseye));
	const float rightOut = float(*yFrame.constpixel<uint8_t>(bullseyeRight + 0u, yBullseye));
	ocean_assert(rightIn <= float(threshold) && rightOut > float(threshold));

	ocean_assert(rightOut - rightIn >= NumericF::eps());
	const float rightBorder = float(bullseyeRight) - (rightOut - float(threshold)) / (rightOut - rightIn);


	// top border
	const float topIn = float(*yFrame.constpixel<uint8_t>(xBullseye, bullseyeTop + 1u));
	const float topOut = float(*yFrame.constpixel<uint8_t>(xBullseye, bullseyeTop + 0u));
	ocean_assert(topIn <= float(threshold) && topOut > float(threshold));

	ocean_assert(topOut - topIn >= NumericF::eps());
	const float topBorder = float(bullseyeTop) + (topOut - float(threshold)) / (topOut - topIn);


	// bottom border
	const float bottomIn = float(*yFrame.constpixel<uint8_t>(xBullseye, bullseyeBottom - 1u));
	const float bottomOut = float(*yFrame.constpixel<uint8_t>(xBullseye, bullseyeBottom + 0u));
	ocean_assert(bottomIn <= float(threshold) && bottomOut > float(threshold));

	ocean_assert(bottomOut - bottomIn >= NumericF::eps());
	const float bottomBorder = float(bullseyeBottom) - (bottomOut - float(threshold)) / (bottomOut - bottomIn);

	ocean_assert(leftBorder <= float(xBullseye) && float(xBullseye) <= rightBorder);
	ocean_assert(topBorder <= float(yBullseye) && float(yBullseye) <= bottomBorder);

	location = Vector2(Scalar(leftBorder + rightBorder) * Scalar(0.5), Scalar(topBorder + bottomBorder) * Scalar(0.5));

	return true;
}

bool BullseyeDetectorMono::isBlackPixel(const uint8_t* pixel, const uint8_t threshold)
{
	ocean_assert(pixel != nullptr);
	return *pixel < threshold;
};

bool BullseyeDetectorMono::isWhitePixel(const uint8_t* pixel, const uint8_t threshold)
{
	ocean_assert(pixel != nullptr);
	return *pixel >= threshold;
}

Vector2 BullseyeDetectorMono::computeSubpixelTransition(const VectorT2<unsigned int>& lastPointInside, const VectorT2<unsigned int>& firstPointOutside, uint8_t insideIntensity, uint8_t outsideIntensity, const unsigned int threshold)
{
	ocean_assert(threshold < 256u);

	const int intensityDifference = int(outsideIntensity) - int(insideIntensity);
	if (intensityDifference == 0)
	{
		return Vector2((Scalar(lastPointInside.x()) + Scalar(firstPointOutside.x())) * Scalar(0.5), (Scalar(lastPointInside.y()) + Scalar(firstPointOutside.y())) * Scalar(0.5));
	}

	const Scalar interpolationScale = Scalar(int(outsideIntensity) - int(threshold)) / Scalar(intensityDifference);
	ocean_assert(Numeric::isInsideRange(Scalar(0), interpolationScale, Scalar(1)));

	const Scalar x = Scalar(firstPointOutside.x()) + (Scalar(lastPointInside.x()) - Scalar(firstPointOutside.x())) * interpolationScale;
	const Scalar y = Scalar(firstPointOutside.y()) + (Scalar(lastPointInside.y()) - Scalar(firstPointOutside.y())) * interpolationScale;
	ocean_assert(x > Numeric::minValue() && x < Numeric::maxValue());
	ocean_assert(y > Numeric::minValue() && y < Numeric::maxValue());

	ocean_assert(!Numeric::isInf(x));
	ocean_assert(!Numeric::isNan(x));

	ocean_assert(!Numeric::isInf(y));
	ocean_assert(!Numeric::isNan(y));

	return Vector2(x, y);
}

Scalar BullseyeDetectorMono::computeIntensityInterpolationFactor(const uint8_t insideIntensity, const uint8_t outsideIntensity, const unsigned int threshold)
{
	ocean_assert(threshold < 256u);

	const int intensityDifference = int(outsideIntensity) - int(insideIntensity);

	if (intensityDifference == 0)
	{
		return Scalar(0.5);
	}

	const Scalar t = Scalar(int(threshold) - int(insideIntensity)) / Scalar(intensityDifference);

	// Clamp to [0, 1] for safety
	return std::max(Scalar(0), std::min(Scalar(1), t));
}

Vector2 BullseyeDetectorMono::computeTransitionPointOnRay(const VectorT2<unsigned int>& insidePoint, const VectorT2<unsigned int>& outsidePoint, const uint8_t insideIntensity, const uint8_t outsideIntensity, const unsigned int threshold, const Vector2& center, const Vector2& rayDirection)
{
	ocean_assert(rayDirection.isUnit());

	// Compute the parametric distance along the ray for each Bresenham pixel
	const Vector2 insidePointF(Scalar(insidePoint.x()), Scalar(insidePoint.y()));
	const Vector2 outsidePointF(Scalar(outsidePoint.x()), Scalar(outsidePoint.y()));

	const Scalar distanceInside = (insidePointF - center) * rayDirection;  // dot product
	const Scalar distanceOutside = (outsidePointF - center) * rayDirection;

	// Interpolate the distance based on intensity
	const Scalar t = computeIntensityInterpolationFactor(insideIntensity, outsideIntensity, threshold);
	const Scalar interpolatedDistance = distanceInside + (distanceOutside - distanceInside) * t;

	// Reconstruct the 2D point exactly on the ray
	return center + rayDirection * interpolatedDistance;
}

bool BullseyeDetectorMono::castHalfRay(const uint8_t* yFrameData, const unsigned int yFrameWidth, const unsigned int yFrameHeight, const unsigned int yFrameStrideElements, const unsigned int xCenter, const unsigned int yCenter, const Scalar angle, const Scalar maxSearchRadius, const uint8_t centerIntensity, const uint8_t grayThreshold, HalfRay& ray)
{
	// TODO ocean_assert(...)

	const Scalar maxSqrSearchRadius = maxSearchRadius * maxSearchRadius;

	ray = HalfRay();
	ray.angle = angle;

	const Scalar cosAngle = Numeric::cos(angle);
	const Scalar sinAngle = Numeric::sin(angle);

	// Center and ray direction for direct distance interpolation.
	//
	// Problem: Bresenham's line algorithm steps along the "major axis" (the axis with
	// greater displacement) one pixel at a time. For near-vertical rays (e.g., 67.5°,
	// 90°, 112.5°), the Y-axis is the major axis, so the first step is always in the
	// Y direction. When the bullseye center is at or near integer pixel coordinates,
	// multiple rays at different angles will step to the same first pixel, causing
	// their first transition points (r0) to be incorrectly computed at identical
	// locations despite having different angular directions.
	//
	// Solution: Instead of computing a 2D subpixel point via linear interpolation
	// between Bresenham pixels and then projecting onto the ray, we directly interpolate
	// the distance along the ray. For each Bresenham pixel, we compute its parametric
	// distance along the ray (via dot product with the ray direction), then interpolate
	// between these distances based on intensity. This ensures the transition point
	// lies exactly on the ray at the correct distance from center.
	const Vector2 center{Scalar(xCenter), Scalar(yCenter)};
	const Vector2 rayDirection{cosAngle, sinAngle};

	const int xStart = int(xCenter);
	const int yStart = int(yCenter);
	const int xEnd = int(Scalar(xCenter) + cosAngle * maxSearchRadius + Scalar(0.5));
	const int yEnd = int(Scalar(yCenter) + sinAngle * maxSearchRadius + Scalar(0.5));

	CV::Bresenham bresenham(xStart, yStart, xEnd, yEnd);
	if (!bresenham.isValid())
	{
		return false;
	}

	enum class State : unsigned int
	{
		Center = 0u,
		Ring0,
		Ring1,
		Done
	};

	State state = State::Center;
	int xCurrent = xStart;
	int yCurrent = yStart;

	VectorT2<unsigned int> lastPointInside((unsigned int)xStart, (unsigned int)yStart);
	uint8_t lastIntensityInside = centerIntensity;

	while (state < State::Done)
	{
		int xNext = xCurrent;
		int yNext = yCurrent;

		bresenham.findNext(xNext, yNext);

		if (xNext < 0 || xNext >= int(yFrameWidth) || yNext < 0 || yNext >= int(yFrameHeight))
		{
			break;
		}

		const Scalar dx = Scalar(xNext) - Scalar(xCenter);
		const Scalar dy = Scalar(yNext) - Scalar(yCenter);
		if (dx * dx + dy * dy > maxSqrSearchRadius)
		{
			break;
		}

		const VectorT2<unsigned int> nextPoint((unsigned int)xNext, (unsigned int)yNext);
		const uint8_t* nextPixel = yFrameData + yNext * yFrameStrideElements + xNext;
		const uint8_t nextIntensity = *nextPixel;
		const bool nextIsBlack = isBlackPixel(nextPixel, grayThreshold);

		if (state == State::Center)
		{
			if (!nextIsBlack)
			{
				ray.transitionPoints[0] = computeTransitionPointOnRay(lastPointInside, nextPoint, lastIntensityInside, nextIntensity, grayThreshold, center, rayDirection);
				state = State::Ring0;
			}

			lastPointInside = nextPoint;
			lastIntensityInside = nextIntensity;
		}
		else if (state == State::Ring0)
		{
			if (nextIsBlack)
			{
				ray.transitionPoints[1] = computeTransitionPointOnRay(lastPointInside, nextPoint, lastIntensityInside, nextIntensity, grayThreshold, center, rayDirection);
				state = State::Ring1;
			}

			lastPointInside = nextPoint;
			lastIntensityInside = nextIntensity;
		}
		else if (state == State::Ring1)
		{
			if (!nextIsBlack)
			{
				ray.transitionPoints[2] = computeTransitionPointOnRay(lastPointInside, nextPoint, lastIntensityInside, nextIntensity, grayThreshold, center, rayDirection);
				state = State::Done;
			}
			else
			{
				lastPointInside = nextPoint;
				lastIntensityInside = nextIntensity;
			}
		}

		xCurrent = xNext;
		yCurrent = yNext;
	}

	return state == State::Done;
};

bool BullseyeDetectorMono::checkRadialConsistencyPhase1CastRays(const uint8_t* yData, const unsigned int width, const unsigned int height, const unsigned int strideElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const float maxSearchRadius, const uint8_t centerIntensity, const unsigned int numberDiameters, const Scalar minValidRayFraction, const Scalar scale, Diameters& diameters)
{
	ocean_assert(yData != nullptr);
	ocean_assert(xCenter < width && yCenter < height);
	ocean_assert(numberDiameters >= 4u);
	ocean_assert(minValidRayFraction > Scalar(0) && minValidRayFraction <= Scalar(1));

	const Scalar angleStep = Numeric::pi() / Scalar(numberDiameters);

	unsigned int validDiameterCount = 0u;

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		Diameter& diameter = diameters[i];
		const Scalar angle = Scalar(i) * angleStep;

		// Cast positive direction
		if (!castHalfRay(yData, width, height, strideElements, xCenter, yCenter, angle, Scalar(maxSearchRadius), centerIntensity, threshold, diameter.halfRayPositive) || !diameter.halfRayPositive.isValid())
		{
			continue;
		}

		// Cast negative direction (angle + PI)
		castHalfRay(yData, width, height, strideElements, xCenter, yCenter, angle + Numeric::pi(), Scalar(maxSearchRadius), centerIntensity, threshold, diameter.halfRayNegative);
		if (!diameter.halfRayNegative.isValid())
		{
			continue;
		}

		++validDiameterCount;
	}

	const bool passed = Scalar(validDiameterCount) >= Scalar(numberDiameters) * minValidRayFraction;

	BullseyesDebugElements::get().drawRadialConsistencyPhase1(yCenter, xCenter, scale, diameters, passed);

	return passed;
}

bool BullseyeDetectorMono::checkRadialConsistencyPhase2SymmetryValidation(const unsigned int xCenter, const unsigned int yCenter, const unsigned int numberDiameters, const Scalar minValidRayFraction, const Scalar scale, Diameters& diameters)
{
	ocean_assert(numberDiameters >= 4u);
	ocean_assert(minValidRayFraction > Scalar(0) && minValidRayFraction <= Scalar(1));

	constexpr Scalar symmetryTolerance = Scalar(0.25);
	constexpr Scalar minTolerance = Scalar(1) - symmetryTolerance;
	constexpr Scalar maxTolerance = Scalar(1) + symmetryTolerance;

	unsigned int symmetricCount = 0u;
	const Vector2 center{Scalar(xCenter), Scalar(yCenter)};

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		Diameter& diameter = diameters[i];

		if (!diameter.areHalfRaysValid())
		{
			continue;
		}

		const Scalar distancePositive = (diameter.halfRayPositive.transitionPoints[2] - center).length();
		const Scalar distanceNegative = (diameter.halfRayNegative.transitionPoints[2] - center).length();

		const bool inRange = ((minTolerance * distancePositive <= distanceNegative) && (maxTolerance * distancePositive >= distanceNegative)) ||
		                     ((minTolerance * distanceNegative <= distancePositive) && (maxTolerance * distanceNegative >= distancePositive));

		if (inRange)
		{
			diameter.isSymmetryValid = true;
			++symmetricCount;
		}
		else
		{
			diameter.isSymmetryValid = false;
		}
	}

	const bool passed = Scalar(symmetricCount) >= Scalar(numberDiameters) * minValidRayFraction;

	BullseyesDebugElements::get().drawRadialConsistencyPhase2(yCenter, xCenter, scale, diameters, passed);

	return passed;
}

bool BullseyeDetectorMono::checkRadialConsistencyPhase3IntensityValidation(const Frame& yFrame, const unsigned int threshold, const unsigned int numberDiameters, const Scalar backgroundExtensionFactor, const Scalar scale, const unsigned int xCenter, const unsigned int yCenter, Diameters& diameters)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(numberDiameters >= 4u);

	const unsigned int width = yFrame.width();
	const unsigned int height = yFrame.height();

	unsigned int intensityPassCount = 0u;
	unsigned int intensityTotalCount = 0u;

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		Diameter& diameter = diameters[i];

		for (HalfRay* ray : {&diameter.halfRayPositive, &diameter.halfRayNegative})
		{
			const Vector2 midPoint0 = (ray->transitionPoints[0] + ray->transitionPoints[1]) * Scalar(0.5);
			if (midPoint0.x() >= Scalar(1) && midPoint0.x() < Scalar(width - 1) && midPoint0.y() >= Scalar(1) && midPoint0.y() < Scalar(height - 1))
			{
				if (isWhitePixel(yFrame.constpixel<uint8_t>((unsigned int)midPoint0.x(), (unsigned int)midPoint0.y()), threshold))
				{
					ray->isIntensityValid[0] = true;
					++intensityPassCount;
				}
				else
				{
					ray->isIntensityValid[0] = false;
				}

				++intensityTotalCount;
			}

			ray->intensityCheckPoints[0] = midPoint0;

			const Vector2 midPoint1 = (ray->transitionPoints[1] + ray->transitionPoints[2]) * Scalar(0.5);
			if (midPoint1.x() >= Scalar(1) && midPoint1.x() < Scalar(width - 1) && midPoint1.y() >= Scalar(1) && midPoint1.y() < Scalar(height - 1))
			{
				if (isBlackPixel(yFrame.constpixel<uint8_t>((unsigned int)midPoint1.x(), (unsigned int)midPoint1.y()), threshold))
				{
					ray->isIntensityValid[1] = true;
					++intensityPassCount;
				}
				else
				{
					ray->isIntensityValid[1] = false;
				}

				ray->intensityCheckPoints[1] = midPoint1;

				++intensityTotalCount;
			}

			if (ray->transitionPoints.size() >= 2 && ray->transitionPoints[1] != HalfRay::invalidTransitionPoint() && ray->transitionPoints[2] != HalfRay::invalidTransitionPoint())
			{
				const Scalar ringWidth = ray->transitionPoints[2].distance(ray->transitionPoints[1]);
				const Vector2 direction = (ray->transitionPoints[2] - ray->transitionPoints[1]).normalizedOrZero();
				ocean_assert(direction.isUnit());

				const Scalar extensionWidth = ringWidth * backgroundExtensionFactor;
				const Vector2 outsidePoint = ray->transitionPoints[2] + direction * extensionWidth;

				if (outsidePoint.x() >= Scalar(1) && outsidePoint.x() < Scalar(width - 1) && outsidePoint.y() >= Scalar(1) && outsidePoint.y() < Scalar(height - 1))
				{
					if (isWhitePixel(yFrame.constpixel<uint8_t>((unsigned int)outsidePoint.x(), (unsigned int)outsidePoint.y()), threshold))
					{
						ray->isIntensityValid[2] = true;
						++intensityPassCount;
					}
					else
					{
						ray->isIntensityValid[2] = false;
					}

					ray->intensityCheckPoints[2] = outsidePoint;
				}

				++intensityTotalCount;
			}
		}
	}

	constexpr Scalar minPassFraction = Scalar(0.75);
	ocean_assert(intensityTotalCount != 0u);
	const Scalar passFraction = intensityTotalCount == 0u ? Scalar(0) : (Scalar(intensityPassCount) / Scalar(intensityTotalCount));

	const bool passed = passFraction >= minPassFraction;

	BullseyesDebugElements::get().drawRadialConsistencyPhase3(yCenter, xCenter, scale, diameters, passed);

	return passed;
}

bool BullseyeDetectorMono::checkRadialConsistencyPhase4RadialProfileValidation(const unsigned int xCenter, const unsigned int yCenter, const unsigned int numberDiameters, const Diameters& diameters)
{
	ocean_assert(numberDiameters >= 4u);

	// Step 1: Compute centroid of outer ring (r2) points as more accurate center
	Vector2 centroid(Scalar(0), Scalar(0));
	unsigned int numberCentroidPoints = 0u;

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		const Diameter& diameter = diameters[i];

		if (!diameter.isSymmetryValid)
		{
			continue;
		}

		centroid += diameter.halfRayPositive.transitionPoints[2];
		centroid += diameter.halfRayNegative.transitionPoints[2];

		numberCentroidPoints += 2u;
	}

	if (numberCentroidPoints < 4u)
	{
		// Not enough symmetric diameters to validate radial profile - reject
		return false;
	}

	centroid /= Scalar(numberCentroidPoints);

	// Step 2: Collect squared distances from centroid, sorted by angle
	using AngleDistanceSqrPair = std::pair<Scalar, Scalar>;
	using AngleDistanceSqrPairs = std::vector<AngleDistanceSqrPair>;

	auto angleDistanceSqrPairLess = [](const AngleDistanceSqrPair& a, const AngleDistanceSqrPair& b)
	{
		return a.first < b.first;
	};

	AngleDistanceSqrPairs angleDistanceSqrPairs;
	angleDistanceSqrPairs.reserve(numberCentroidPoints);

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		const Diameter& diameter = diameters[i];

		if (!diameter.isSymmetryValid)
		{
			continue;
		}

		const Scalar distancePositiveSqr = (diameter.halfRayPositive.transitionPoints[2] - centroid).sqr();
		const Scalar distanceNegativeSqr = (diameter.halfRayNegative.transitionPoints[2] - centroid).sqr();

		angleDistanceSqrPairs.emplace_back(diameter.halfRayPositive.angle, distancePositiveSqr);
		angleDistanceSqrPairs.emplace_back(diameter.halfRayNegative.angle, distanceNegativeSqr);
	}

	// Sort by angle for circular traversal
	std::sort(angleDistanceSqrPairs.begin(), angleDistanceSqrPairs.end(), angleDistanceSqrPairLess);

	// Step 3: Count local extrema using multiplicative distance threshold
	constexpr Scalar distanceThreshold = Scalar(0.05);
	static_assert(distanceThreshold >= Scalar(0) && distanceThreshold <= Scalar(1), "Invalid distance threshold");
	constexpr Scalar maxDistanceThresholdSqr = (Scalar(1) + distanceThreshold) * (Scalar(1) + distanceThreshold);
	constexpr Scalar minDistanceThresholdSqr = (Scalar(1) - distanceThreshold) * (Scalar(1) - distanceThreshold);

	const size_t n = angleDistanceSqrPairs.size();
	unsigned int extremaCount = 0u;

	for (size_t i = 0; i < n; ++i)
	{
		const Scalar prevSqr = angleDistanceSqrPairs[(i + n - 1) % n].second;
		const Scalar currSqr = angleDistanceSqrPairs[i].second;
		const Scalar nextSqr = angleDistanceSqrPairs[(i + 1) % n].second;

		// Local maximum: significantly higher than both neighbors
		if (currSqr > prevSqr * maxDistanceThresholdSqr && currSqr > nextSqr * maxDistanceThresholdSqr)
		{
			++extremaCount;
		}
		// Local minimum: significantly lower than both neighbors
		else if (currSqr < prevSqr * minDistanceThresholdSqr && currSqr < nextSqr * minDistanceThresholdSqr)
		{
			++extremaCount;
		}
	}

	// Step 4: Reject if too many extrema (ellipse should have ~4, allow up to 6 for noise)
	constexpr unsigned int maxExtrema = 6u;

	return extremaCount <= maxExtrema;
}

bool BullseyeDetectorMono::checkRadialConsistencyPhase5RingProportionValidation(const unsigned int xCenter, const unsigned int yCenter, const unsigned int numberDiameters, const Diameters& diameters)
{
	ocean_assert(numberDiameters >= 4u);

	constexpr Scalar maxCoeffOfVariation = Scalar(0.25);
	constexpr Scalar minRingWidth = Scalar(1.0);

	const Vector2 center{Scalar(xCenter), Scalar(yCenter)};

	Scalars widthsRing0;
	Scalars widthsRing1;

	for (unsigned int i = 0u; i < numberDiameters; ++i)
	{
		const Diameter& diameter = diameters[i];

		if (!diameter.isSymmetryValid)
		{
			continue;
		}

		// Collect from positive ray
		{
			const HalfRay& ray = diameter.halfRayPositive;
			const Scalar distance0 = ray.transitionPoints[0].distance(center);
			const Scalar distance1 = ray.transitionPoints[1].distance(center);
			const Scalar distance2 = ray.transitionPoints[2].distance(center);
			const Scalar widthRing0 = distance1 - distance0;
			const Scalar widthRing1 = distance2 - distance1;

			widthsRing0.push_back(widthRing0);
			widthsRing1.push_back(widthRing1);
		}

		// Collect from negative ray
		{
			const HalfRay& ray = diameter.halfRayNegative;
			const Scalar distance0 = ray.transitionPoints[0].distance(center);
			const Scalar distance1 = ray.transitionPoints[1].distance(center);
			const Scalar distance2 = ray.transitionPoints[2].distance(center);
			const Scalar widthRing0 = distance1 - distance0;
			const Scalar widthRing1 = distance2 - distance1;

			widthsRing0.push_back(widthRing0);
			widthsRing1.push_back(widthRing1);
		}
	}

	if (widthsRing0.size() < 4u)
	{
		// Not enough data to validate ring proportions
		return true;
	}

	const Scalar meanRing0 = computeMean(widthsRing0);
	const Scalar meanRing1 = computeMean(widthsRing1);

	const Scalar stddevRing0 = computeStddev(widthsRing0, meanRing0);
	const Scalar stddevRing1 = computeStddev(widthsRing1, meanRing1);

	const Scalar cvRing0 = meanRing0 > Numeric::eps() ? stddevRing0 / meanRing0 : Scalar(0);
	const Scalar cvRing1 = meanRing1 > Numeric::eps() ? stddevRing1 / meanRing1 : Scalar(0);

	const Scalar minWidthRing1 = findMin(widthsRing1);

	const bool failedCV = cvRing0 > maxCoeffOfVariation || cvRing1 > maxCoeffOfVariation;
	const bool failedMinWidth = minWidthRing1 < minRingWidth;

	return !failedCV && !failedMinWidth;
}

Scalar BullseyeDetectorMono::computeMean(const Scalars& values)
{
	ocean_assert(!values.empty());

	Scalar sum = Scalar(0);

	for (const Scalar& value : values)
	{
		sum += value;
	}

	return sum / Scalar(values.size());
}

Scalar BullseyeDetectorMono::computeStddev(const Scalars& values, const Scalar mean)
{
	ocean_assert(!values.empty());

	Scalar sumSqDiff = Scalar(0);

	for (const Scalar& value : values)
	{
		const Scalar diff = value - mean;
		sumSqDiff += diff * diff;
	}

	return Numeric::sqrt(sumSqDiff / Scalar(values.size()));
}

Scalar BullseyeDetectorMono::findMin(const Scalars& values)
{
	ocean_assert(!values.empty());

	Scalar minValue = values[0];

	for (size_t i = 1; i < values.size(); ++i)
	{
		if (values[i] < minValue)
		{
			minValue = values[i];
		}
	}

	return minValue;
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
