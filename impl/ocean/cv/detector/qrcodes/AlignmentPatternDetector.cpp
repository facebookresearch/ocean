/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/AlignmentPatternDetector.h"

#include "ocean/cv/detector/qrcodes/TransitionDetector.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

AlignmentPatterns AlignmentPatternDetector::detectAlignmentPatterns(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int searchX, const unsigned int searchY, const unsigned int searchWidth, const unsigned int searchHeight, const bool isNormalReflectance, const unsigned int grayThreshold)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert_and_suppress_unused(searchX + searchWidth <= width && searchY + searchHeight <= height, height);

	const unsigned int strideElements = width + paddingElements;

	const uint8_t* ySearchFrame = yFrame + searchY * strideElements + searchX;
	const unsigned int searchPaddingElements = strideElements - searchWidth;

	// Detect alignment patterns

	AlignmentPatterns alignmentPatterns;
	alignmentPatterns.reserve(4);

	for (unsigned int row = 0u; row < searchHeight; ++row)
	{
		detectAlignmentPatternsInRow(ySearchFrame, searchWidth, searchHeight, searchPaddingElements, row, searchX, searchY, isNormalReflectance, grayThreshold, alignmentPatterns);
	}

	// Filter out alignment patterns which are too close to eachother

	for (size_t outer = 0; outer < alignmentPatterns.size(); ++outer)
	{
		for (size_t inner = 0; inner < outer; ++inner)
		{
			// Each alignment pattern has 5 segments; the minimum distance is the sum of the halves of the two current patterns mutiplied with a factor, i.e. (2.5 * (segmentSizeA + segmentSizeB)) * 1.6
			const Scalar minimumSqrDistance = Numeric::sqr(Scalar((alignmentPatterns[outer].averageSegmentSize() + alignmentPatterns[inner].averageSegmentSize()) * 4u));
			const Scalar sqrDistance = alignmentPatterns[outer].center().sqrDistance(alignmentPatterns[inner].center());

			if (sqrDistance < minimumSqrDistance)
			{
				alignmentPatterns[outer] = alignmentPatterns.back();
				alignmentPatterns.pop_back();

				// Cancelling the next ++outer in the outer loop; this is required because the item at the current index has not been checked yet.
				--outer;

				break;
			}
		}
	}

	return alignmentPatterns;
}

void AlignmentPatternDetector::detectAlignmentPatternsInRow(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int row, const unsigned int xOffset, const unsigned int yOffset, const bool isNormalReflectance, const unsigned int grayThreshold, AlignmentPatterns& alignmentPatterns)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width != 0u);

	const unsigned int strideElements = width + paddingElements;
	const uint8_t* const yRow = yFrame + strideElements * row;

	TransitionDetector::PixelComparisonFunc isForegroundPixel = isNormalReflectance ? TransitionDetector::isLessOrEqual : TransitionDetector::isGreater;
	TransitionDetector::PixelComparisonFunc isBackgroundPixel = isNormalReflectance ? TransitionDetector::isGreater : TransitionDetector::isLessOrEqual;

	unsigned int x = 0u;

	while (x < width && isForegroundPixel(yRow + x, grayThreshold))
	{
		++x;
	}

	if (x >= width)
	{
		return;
	}

	unsigned int xStarts[4] = { x, 0u, 0u, 0u };

	while (x < width && isBackgroundPixel(yRow + x, grayThreshold))
	{
		++x;
	}

	if (x >= width)
	{
		return;
	}

	xStarts[1] = x;

	while (x < width)
	{
		while (x < width && isForegroundPixel(yRow + x, grayThreshold))
		{
			++x;
		}

		if (x >= width)
		{
			return;
		}

		xStarts[2] = x;

		while (x < width && isBackgroundPixel(yRow + x, grayThreshold))
		{
			++x;
		}

		if (x >= width)
		{
			return;
		}

		xStarts[3] = x;

		ocean_assert(xStarts[0] < xStarts[1]);
		ocean_assert(xStarts[1] < xStarts[2]);
		ocean_assert(xStarts[2] < xStarts[3]);

		const unsigned int lengths[3] =
		{
			xStarts[1] - xStarts[0],
			xStarts[2] - xStarts[1],
			xStarts[3] - xStarts[2],
		};

		const unsigned int minLength = (lengths[0] * 512u + 512u) / 1024u; // ~ length[0] * 0.5
		const unsigned int maxLength = (lengths[0] * 1536u + 512u) / 1024u; // ~ length[0] * 1.5

		if (lengths[1] >= minLength && lengths[1] <= maxLength &&
			lengths[2] >= minLength && lengths[2] <= maxLength)
		{
			const unsigned int xCenter = (xStarts[1] + xStarts[2] + 1u) / 2u;

			const unsigned int diameter = lengths[0] + lengths[1] + lengths[2];
			const unsigned int diameter3_4 = (diameter * 3u + 2u) / 4u;

			if (xCenter >= diameter3_4 && xCenter + diameter3_4 < width &&
				row >= diameter3_4 && row + diameter3_4 < height)
			{
				// Increase min-max-range in order to account for Pythagoras
				const unsigned int minCircularLength = minLength / 2u;
				const unsigned int maxCircularLength = maxLength * 2u;

				if (checkInCircle(yFrame, width, height, paddingElements, xCenter, row, minCircularLength, maxCircularLength, isNormalReflectance, grayThreshold))
				{
					Vector2 location;

					if (TransitionDetector::determineSubPixelLocation(yFrame, width, height, paddingElements, xCenter, row, isNormalReflectance, grayThreshold, location))
					{
						const unsigned int averageSegmentSize = ((lengths[0] + lengths[1] + lengths[2]) * 2u + 3u) / 6u;
						alignmentPatterns.emplace_back(location + Vector2(Scalar(xOffset), Scalar(yOffset)), averageSegmentSize);
					}
				}
			}
		}

		xStarts[0] = xStarts[2];
		xStarts[1] = xStarts[3];
		xStarts[2] = 0u;
		xStarts[3] = 0u;
	}
}

bool AlignmentPatternDetector::checkInCircle(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int minLength, const unsigned int maxLength, const bool isNormalReflectance, const unsigned int grayThreshold)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(xCenter < width && yCenter < height);

	constexpr Scalar anglesToCheckInRad[10] =
	{
		// Skipping 0 because that's the angle at which this candidate has been found initially.
		Numeric::deg2rad(Scalar(15)),
		Numeric::deg2rad(Scalar(30)),
		Numeric::deg2rad(Scalar(45)),
		Numeric::deg2rad(Scalar(60)),
		Numeric::deg2rad(Scalar(75)),
		Numeric::deg2rad(Scalar(90)),
		Numeric::deg2rad(Scalar(105)),
		Numeric::deg2rad(Scalar(120)),
		Numeric::deg2rad(Scalar(135)),
		Numeric::deg2rad(Scalar(150)),
	};

	for (size_t i = 0; i < 10; ++i)
	{
		if (!checkInDirection(yFrame, width, height, paddingElements, xCenter, yCenter, minLength, maxLength, isNormalReflectance, grayThreshold, anglesToCheckInRad[i]))
		{
			return false;
		}
	}

	return true;
}

bool AlignmentPatternDetector::checkInDirection(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int minLength, const unsigned int maxLength, const bool isNormalReflectance, const unsigned int grayThreshold, const Scalar angle)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(xCenter < width && yCenter < height);
	ocean_assert(minLength <= maxLength);
	ocean_assert(Numeric::isInsideRange(Scalar(0), angle, Numeric::pi() - Numeric::eps()));

	const Vector3 homogeneousScanlineDirection = Quaternion(Vector3(0, 0, 1), -angle) * Vector3(1, 0, 1);
	ocean_assert(Numeric::isNotEqualEps(homogeneousScanlineDirection.z()));
	const Vector2 scanlineDirection = Vector2(homogeneousScanlineDirection.x() / homogeneousScanlineDirection.z(), homogeneousScanlineDirection.y() / homogeneousScanlineDirection.z()) * Scalar(/* random but large value! */ 10000u);

	Bresenham bresenhamTop(int(xCenter), int(yCenter), Numeric::round32(Scalar(xCenter) + scanlineDirection.x()), Numeric::round32(Scalar(yCenter) + scanlineDirection.y()));
	Bresenham bresenhamBottom(int(xCenter), int(yCenter), Numeric::round32(Scalar(xCenter) - scanlineDirection.x()), Numeric::round32(Scalar(yCenter) - scanlineDirection.y()));

	if (bresenhamTop.isValid() == false || bresenhamBottom.isValid() == false)
	{
		return false;
	}

	const unsigned int maxLength_2 = (maxLength + 1u) / 2u;
	const unsigned int sqrMaxLength = maxLength * maxLength;
	const unsigned int sqrMinLength = minLength * minLength;

	TransitionDetector::FindNextPixelFunc findNextForegroundPixel = isNormalReflectance ? TransitionDetector::findNextPixel<true> : TransitionDetector::findNextPixel<false>;
	TransitionDetector::FindNextPixelFunc findNextBackgroundPixel = isNormalReflectance ? TransitionDetector::findNextPixel<false> : TransitionDetector::findNextPixel<true>;

	const unsigned int maxLengthWithSymmetricTolerance = max(1u, maxLength * 179u / 512u); // ~ 35%

	// The center square (which uses the foreground color)

	unsigned int topColumns = 0u;
	unsigned int topRows = 0u;
	unsigned int bottomColumns = 0u;
	unsigned int bottomRows = 0u;

	VectorT2<unsigned int> topIn;
	VectorT2<unsigned int> topOut;

	VectorT2<unsigned int> bottomIn;
	VectorT2<unsigned int> bottomOut;

	if (!findNextBackgroundPixel(yFrame, xCenter, yCenter, width, height, paddingElements, bresenhamTop, maxLength_2 + 1u, grayThreshold, topColumns, topRows, topIn, topOut) ||
		!findNextBackgroundPixel(yFrame, xCenter, yCenter, width, height, paddingElements, bresenhamBottom, maxLength_2 + 1u, grayThreshold, bottomColumns, bottomRows, bottomIn, bottomOut))
	{
		return false;
	}

	const unsigned int numberCenterColumns = topColumns + bottomColumns + 1u; // + 1u == xCenter
	const unsigned int numberCenterRows = topRows + bottomRows + 1u;
	const unsigned int sqrCenterLength = numberCenterColumns * numberCenterColumns + numberCenterRows * numberCenterRows;

	if (NumericT<unsigned int>::isNotEqual(topRows * topRows + topColumns * topColumns, bottomRows * bottomRows + bottomColumns * bottomColumns, maxLengthWithSymmetricTolerance * maxLengthWithSymmetricTolerance) ||
		!NumericT<unsigned int>::isInsideRange(sqrMinLength, sqrCenterLength, sqrMaxLength))
	{
		return false;
	}

	ocean_assert(topIn.x() < width && topIn.y() < height && topOut.x() < width && topOut.y() < height);
	ocean_assert(bottomIn.x() < width && bottomIn.y() < height && bottomOut.x() < width && bottomOut.y() < height);

	// Sandwich ring (which uses the background color)

	if (!findNextForegroundPixel(yFrame, topOut.x(), topOut.y(), width, height, paddingElements, bresenhamTop, maxLength + 1u, grayThreshold, topColumns, topRows, topIn, topOut) ||
		!findNextForegroundPixel(yFrame, bottomOut.x(), bottomOut.y(), width, height, paddingElements, bresenhamBottom, maxLength + 1u, grayThreshold, bottomColumns, bottomRows, bottomIn, bottomOut))
	{
		return false;
	}

	const unsigned int sqrTopLength = (topColumns * topColumns) + (topRows * topRows);
	const unsigned int sqrBottomLength = (bottomColumns * bottomColumns) + (bottomRows * bottomRows);

	if (!NumericT<unsigned int>::isInsideRange(sqrMinLength, sqrTopLength, sqrMaxLength) ||
		!NumericT<unsigned int>::isInsideRange(sqrMinLength, sqrBottomLength, sqrMaxLength))
	{
		return false;
	}

	ocean_assert(topIn.x() < width && topIn.y() < height && topOut.x() < width && topOut.y() < height);
	ocean_assert(bottomIn.x() < width && bottomIn.y() < height && bottomOut.x() < width && bottomOut.y() < height);

	// Not checking the outer ring because it may be directly adjacent to other foreground modules

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
