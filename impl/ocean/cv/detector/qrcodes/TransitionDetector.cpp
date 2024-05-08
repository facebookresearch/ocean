/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/TransitionDetector.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

template <bool tFindBlackPixel>
bool TransitionDetector::findNextPixel(const uint8_t* const yPointer, const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const unsigned int yPointerPaddingElements, CV::Bresenham& bresenham, const unsigned int maximumDistance, const unsigned int threshold, unsigned int& columns, unsigned int& rows, VectorT2<unsigned int>& lastPointInside, VectorT2<unsigned int>& firstPointOutside)
{
	ocean_assert(yPointer != nullptr);
	ocean_assert(maximumDistance != 0u);
	ocean_assert(x < width && y < height);

	const unsigned int yPointerStrideElements = width + yPointerPaddingElements;

	rows = 0u;
	columns = 0u;

	if (tFindBlackPixel ? ((unsigned int)(yPointer[y * yPointerStrideElements + x]) < threshold) : ((unsigned int)(yPointer[y * yPointerStrideElements + x]) >= threshold))
	{
		return false;
	}

	if (bresenham.isValid() == false)
	{
		ocean_assert(false && "This should never happen");
		return false;
	}

	int currentX = int(x);
	int currentY = int(y);
	int nextX = int(x);
	int nextY = int(y);

	ocean_assert(nextX >= 0 && nextX < int(width) && nextY >= 0 && nextY < int(height));
	const uint8_t* nextPixel = yPointer + (unsigned int)nextY * yPointerStrideElements + (unsigned int)nextX;

	while (nextX >= 0 && nextX < int(width) && nextY >= 0 && nextY < int(height) && columns <= maximumDistance && rows <= maximumDistance && (tFindBlackPixel ? (int(*nextPixel) >= int(threshold)) : (int(*nextPixel) < int(threshold))))
	{
		ocean_assert(currentX >= 0 && currentX < int(width) && currentY >= 0 && currentY < int(height));
		ocean_assert(std::abs(nextX - currentX) <= 1 && std::abs(nextY - currentY) <= 1);
		columns += (unsigned int)((nextX - currentX) & 0b00000001); // ~ columns += nextX != x ? 1 : 0
		rows += (unsigned int)((nextY - currentY) & 0b00000001); // ~ rows += nextY != y ? 1 : 0

		currentX = nextX;
		currentY = nextY;

		bresenham.findNext(nextX, nextY);
		ocean_assert(nextX != currentX || nextY != currentY);

		nextPixel = yPointer + (unsigned int)nextY * yPointerStrideElements + (unsigned int)nextX;
	}

	lastPointInside = VectorT2<unsigned int>((unsigned int)currentX, (unsigned int)currentY);
	firstPointOutside = VectorT2<unsigned int>((unsigned int)nextX, (unsigned int)nextY);

	if (columns == 0u && currentX != nextX)
	{
		columns = (unsigned int)(std::abs(nextX - currentX));
	}

	if (rows == 0u && currentY != nextY)
	{
		rows = (unsigned int)(std::abs(nextY - currentY));
	}

	return currentX >=0 && currentX < int(width) && currentY >= 0 && currentY < int(height)
		&& (currentX != nextX || currentY != nextY)
		&& nextX >= 0 && nextX < int(width) && nextY >= 0 && nextY < int(height)
		&& columns <= maximumDistance && rows <= maximumDistance
		&& (tFindBlackPixel ? ((unsigned int)(yPointer[currentY * yPointerStrideElements + currentX]) >= threshold) : ((unsigned int)(yPointer[currentY * yPointerStrideElements + currentX]) < threshold))
		&& (tFindBlackPixel ? ((unsigned int)(yPointer[nextY * yPointerStrideElements + nextX]) < threshold) : ((unsigned int)(yPointer[nextY * yPointerStrideElements + nextX]) >= threshold));
}

// Explicit instantiations
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextPixel<true>(const uint8_t* const, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, CV::Bresenham&, const unsigned int, const unsigned int, unsigned int&, unsigned int&, VectorT2<unsigned int>&, VectorT2<unsigned int>&);
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextPixel<false>(const uint8_t* const, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, CV::Bresenham&, const unsigned int, const unsigned int, unsigned int&, unsigned int&, VectorT2<unsigned int>&, VectorT2<unsigned int>&);

template <bool tFindBlackPixel>
bool TransitionDetector::findNextUpperPixel(const uint8_t* yPointer, unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows)
{
	ocean_assert(yPointer != nullptr);
	ocean_assert(maximalRows != 0u);
	ocean_assert_and_suppress_unused(maximalRows <= height - y, height);
	ocean_assert(frameStrideElements != 0u);

	if (y == 0u)
	{
		return false;
	}

	rows = 0u;

	while (int(--y) >= 0 && ++rows <= maximalRows && (tFindBlackPixel ? (int(*(yPointer - frameStrideElements)) > int(threshold)) : (int(*(yPointer - frameStrideElements)) < int(threshold))))
	{
		yPointer -= frameStrideElements;
	}

	return int(y) >= 0 && rows <= maximalRows;
}

// Explicit instantiations
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextUpperPixel<true>(const uint8_t*, unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, unsigned int&);
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextUpperPixel<false>(const uint8_t*, unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, unsigned int&);

template <bool tFindBlackPixel>
bool TransitionDetector::findNextLowerPixel(const uint8_t* yPointer, unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows)
{
	ocean_assert(yPointer != nullptr);
	ocean_assert(maximalRows != 0u);
	ocean_assert(y < height);
	ocean_assert(frameStrideElements != 0u);

	if (y >= height - 1u)
	{
		return false;
	}

	rows = 0u;

	while (++y < height && ++rows <= maximalRows && (tFindBlackPixel ? (int(*(yPointer + frameStrideElements)) > int(threshold)) : (int(*(yPointer + frameStrideElements)) < int(threshold))))
	{
		yPointer += frameStrideElements;
	}

	return y < height && rows <= maximalRows;
}

// Explicit instantiations
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextLowerPixel<true>(const uint8_t*, unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, unsigned int&);
template OCEAN_CV_DETECTOR_QRCODES_EXPORT bool TransitionDetector::findNextLowerPixel<false>(const uint8_t*, unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, unsigned int&);

bool TransitionDetector::determineSubPixelLocation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const bool isNormalReflectance, const unsigned int grayThreshold, Vector2& location)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(xCenter < width && yCenter < height);

	const unsigned int strideElements = width + paddingElements;

	TransitionDetector::PixelComparisonFunc isForegroundPixel = isNormalReflectance ? TransitionDetector::isLessOrEqual : TransitionDetector::isGreater;
#if defined(OCEAN_DEBUG)
	TransitionDetector::PixelComparisonFunc isBackgroundPixel = isNormalReflectance ? TransitionDetector::isGreater : TransitionDetector::isLessOrEqual;
#endif

	ocean_assert(isForegroundPixel(yFrame + yCenter * strideElements + xCenter, grayThreshold));

	// First, we identify the first left, right, top, and bottom pixel that do not match with the threshold anymore

	unsigned int xLeft = xCenter - 1u; // exclusive location

	while (xLeft < width && isForegroundPixel(yFrame + (yCenter * strideElements + xLeft), grayThreshold))
	{
		--xLeft;
	}

	if (xLeft >= width)
	{
		return false;
	}

	unsigned int xRight = xCenter + 1u; // exclusive location

	while (xRight < width && isForegroundPixel(yFrame + (yCenter * strideElements + xRight), grayThreshold))
	{
		++xRight;
	}

	if (xRight >= width)
	{
		return false;
	}

	unsigned int yTop = yCenter - 1u; // exclusive location

	while (yTop < height && isForegroundPixel(yFrame + (yTop * strideElements + xCenter), grayThreshold))
	{
		--yTop;
	}

	if (yTop >= height)
	{
		return false;
	}

	unsigned int yBottom = yCenter + 1u; // exclusive location

	while (yBottom < height && isForegroundPixel(yFrame + (yBottom * strideElements + xCenter), grayThreshold))
	{
		++yBottom;
	}

	if (yBottom >= height)
	{
		return false;
	}

	// now, we determine the sub-pixel borders:
	// (out - in) / 1 == (out - threshold) / x
	// x = (out - threshold) / (out - in)

	// left border
	ocean_assert(isForegroundPixel(yFrame + yCenter * strideElements + xLeft + 1u, grayThreshold));
	ocean_assert(isBackgroundPixel(yFrame + yCenter * strideElements + xLeft + 0u, grayThreshold));
	const float leftIn = float(yFrame[yCenter * strideElements + xLeft + 1u]);
	const float leftOut = float(yFrame[yCenter * strideElements + xLeft + 0u]);

	ocean_assert(Numeric::isNotEqualEps(leftOut - leftIn));
	const float leftBorder = float(xLeft) + (leftOut - float(grayThreshold)) / (leftOut - leftIn);

	// right border
	ocean_assert(isForegroundPixel(yFrame + yCenter * strideElements + xRight - 1u, grayThreshold));
	ocean_assert(isBackgroundPixel(yFrame + yCenter * strideElements + xRight + 0u, grayThreshold));
	const float rightIn = float(yFrame[yCenter * strideElements + xRight - 1u]);
	const float rightOut = float(yFrame[yCenter * strideElements + xRight + 0u]);

	ocean_assert(Numeric::isNotEqualEps(rightOut - rightIn));
	const float rightBorder = float(xRight) - (rightOut - float(grayThreshold)) / (rightOut - rightIn);

	// top border
	ocean_assert(isForegroundPixel(yFrame + (yTop + 1u) * strideElements + xCenter, grayThreshold));
	ocean_assert(isBackgroundPixel(yFrame + (yTop + 0u) * strideElements + xCenter, grayThreshold));
	const float topIn = float(yFrame[(yTop + 1u) * strideElements + xCenter]);
	const float topOut = float(yFrame[(yTop + 0u) * strideElements + xCenter]);

	ocean_assert(Numeric::isNotEqualEps(topOut - topIn));
	const float topBorder = float(yTop) + (topOut - float(grayThreshold)) / (topOut - topIn);

	// bottom border
	ocean_assert(isForegroundPixel(yFrame + (yBottom - 1u) * strideElements + xCenter, grayThreshold));
	ocean_assert(isBackgroundPixel(yFrame + (yBottom + 0u) * strideElements + xCenter, grayThreshold));
	const float bottomIn = float(yFrame[(yBottom - 1u) * strideElements + xCenter]);
	const float bottomOut = float(yFrame[(yBottom + 0u) * strideElements + xCenter]);

	ocean_assert(Numeric::isNotEqualEps(bottomOut - bottomIn));
	const float bottomBorder = float(yBottom) - (bottomOut - float(grayThreshold)) / (bottomOut - bottomIn);

	ocean_assert(leftBorder <= float(xCenter) && float(xCenter) <= rightBorder);
	ocean_assert(topBorder <= float(yCenter) && float(yCenter) <= bottomBorder);

	location = Vector2(Scalar(leftBorder + rightBorder) * Scalar(0.5), Scalar(topBorder + bottomBorder) * Scalar(0.5));

	return true;
}

Vector2 TransitionDetector::computeTransitionPointSubpixelAccuracy(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const VectorT2<unsigned int>& lastPointInside, const VectorT2<unsigned int>& firstPointOutside, const unsigned int grayTreshold)
{
	ocean_assert_and_suppress_unused(yFrame != nullptr && width != 0u && height != 0u, height);
	ocean_assert(std::abs(int(lastPointInside.x()) - int(firstPointOutside.x())) <= 1 || std::abs(int(lastPointInside.y()) - int(firstPointOutside.y())) <= 1);
	ocean_assert(lastPointInside.x() < width && lastPointInside.y() < height);
	ocean_assert(firstPointOutside.x() < width && firstPointOutside.y() < height);

	const unsigned int frameStrideElements = width + yFramePaddingElements;

	const Scalar lastPointInsideValue = Scalar(yFrame[(unsigned int)lastPointInside.y() * frameStrideElements + (unsigned int)lastPointInside.x()]);
	const Scalar firstPointOutsideValue = Scalar(yFrame[(unsigned int)firstPointOutside.y() * frameStrideElements + (unsigned int)firstPointOutside.x()]);
	ocean_assert(lastPointInsideValue < Scalar(grayTreshold) && firstPointOutsideValue >= Scalar(grayTreshold));

	const Vector2 transitionPoint(Vector2(firstPointOutside) + (Vector2(lastPointInside) - Vector2(firstPointOutside)) * ((firstPointOutsideValue - Scalar(grayTreshold)) / (firstPointOutsideValue - lastPointInsideValue)));
	ocean_assert(transitionPoint.x() >= 0 && transitionPoint.x() < Scalar(width) && transitionPoint.y() >= 0 && transitionPoint.y() < Scalar(height));

	return transitionPoint;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
