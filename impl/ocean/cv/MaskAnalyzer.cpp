/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/MaskAnalyzer.h"

namespace Ocean
{

namespace CV
{

CV::PixelBoundingBox MaskAnalyzer::detectBoundingBox(const uint8_t* mask, const unsigned int width, const unsigned int height, const uint8_t nonMaskPixel, const unsigned int maskPaddingElements)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	unsigned int boxLeft = (unsigned int)(-1);
	unsigned int boxRight = 0u;
	unsigned int boxTop = (unsigned int)(-1);
	unsigned int boxBottom = 0u;

	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* const maskRow = mask + y * maskStrideElements;

		for (unsigned int xLeft = 0u; xLeft < width; ++xLeft)
		{
			if (xLeft >= boxLeft || maskRow[xLeft] != nonMaskPixel)
			{
				// we found the start point of the mask

				if (xLeft < boxLeft)
				{
					boxLeft = xLeft;
				}

				// now let's find the end point of the mask

				unsigned int xRight = width - 1u;

				while (xLeft < xRight && xRight > boxRight)
				{
					if (maskRow[xRight] != nonMaskPixel)
					{
						break;
					}

					ocean_assert(xRight >= 1u);
					--xRight;
				}

				if (xRight > boxRight)
				{
					boxRight = xRight;
				}

				if (maskRow[xLeft] != nonMaskPixel || maskRow[xRight] != nonMaskPixel)
				{
					if (y < boxTop)
					{
						boxTop = y;
					}

					if (y > boxBottom)
					{
						boxBottom = y;
					}
				}
				else
				{
					// neither the left starting point nor the right end point is part of the mask, so we need to check the entire area between start and end point

					for (unsigned int x = xLeft + 1u; x < xRight; ++x)
					{
						if (maskRow[x] != nonMaskPixel)
						{
							if (y < boxTop)
							{
								boxTop = y;
							}

							if (y > boxBottom)
							{
								boxBottom = y;
							}

							break;
						}
					}
				}

				break;
			}
		}
	}

	return PixelBoundingBox(boxLeft, boxTop, boxRight, boxBottom);
}

PixelBoundingBox MaskAnalyzer::detectBoundingBox(const uint8_t* const mask, const unsigned int width, const unsigned int height, const PixelBoundingBox& rough, const unsigned int maxUncertainty, const uint8_t nonMaskPixel, const unsigned int maskPaddingElements)
{
	ocean_assert(mask != nullptr);
	ocean_assert(maxUncertainty > 0u);

	if (!rough.isValid() || width == 0u || height == 0u)
	{
		return rough;
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	unsigned int exactLeft = (unsigned int)(-1);
	unsigned int exactTop = (unsigned int)(-1);
	unsigned int exactRight = 0u;
	unsigned int exactBottom = 0u;

	const unsigned int leftBorderStart = max(0, int(rough.left()) - int(maxUncertainty));
	const unsigned int leftBorderEnd = min(rough.left() + maxUncertainty + 1u, width); // exclusive

	const unsigned int topBorderStart = max(0, int(rough.top()) - int(maxUncertainty));
	const unsigned int topBorderEnd = min(rough.top() + maxUncertainty + 1u, height); // exclusive

	const unsigned int rightBorderStart = max(0, int(rough.right()) - int(maxUncertainty));
	const unsigned int rightBorderEnd = min(rough.right() + maxUncertainty + 1u, width); // exclusive

	const unsigned int bottomBorderStart = max(0, int(rough.bottom()) - int(maxUncertainty));
	const unsigned int bottomBorderEnd = min(rough.bottom() + maxUncertainty + 1u, height); // exclusive

	// top border

	for (unsigned int y = topBorderStart; exactTop == (unsigned int)(-1) && y < topBorderEnd; ++y)
	{
		const uint8_t* const maskRow = mask + y * maskStrideElements;

		for (unsigned int x = leftBorderStart; x < rightBorderEnd; ++x) // range [leftStart, rightEnd)
		{
			if (maskRow[x] != nonMaskPixel)
			{
				exactTop = y;
				break;
			}
		}
	}

	ocean_assert(exactTop != (unsigned int)(-1));

	// left border

	for (unsigned int x = leftBorderStart; exactLeft == (unsigned int)(-1) && x < leftBorderEnd; ++x)
	{
		const uint8_t* const maskTop = mask + x;

		for (unsigned int y = topBorderStart; y < bottomBorderEnd; ++y) // range [topStart, bottomEnd)
		{
			if (maskTop[y * maskStrideElements] != nonMaskPixel)
			{
				exactLeft = x;
				break;
			}
		}
	}

	ocean_assert(exactLeft != (unsigned int)(-1));

	// right border

	for (int x = int(rightBorderEnd) - 1; exactRight == (unsigned int)(0u) && x >= int(rightBorderStart); --x)
	{
		const uint8_t* const maskTop = mask + x;

		for (unsigned int y = topBorderStart; y < bottomBorderEnd; ++y) // range [topStart, bottomEnd)
		{
			if (maskTop[y * maskStrideElements] != nonMaskPixel)
			{
				exactRight = (unsigned int)(x);
				break;
			}
		}
	}

	ocean_assert(exactRight != 0u || exactRight == exactLeft);

	// bottom border

	for (int y = int(bottomBorderEnd) - 1; exactBottom == (unsigned int)(0u) && y >= int(bottomBorderStart); --y)
	{
		const uint8_t* const maskRow = mask + y * maskStrideElements;

		for (unsigned int x = leftBorderStart; x < rightBorderEnd; ++x) // range [leftStart, rightEnd)
		{
			if (maskRow[x] != nonMaskPixel)
			{
				exactBottom = (unsigned int)(y);
				break;
			}
		}
	}

	ocean_assert(exactBottom != 0u || exactBottom == exactTop);

	return PixelBoundingBox(exactLeft, exactTop, exactRight, exactBottom);
}

PixelBoundingBox MaskAnalyzer::detectOpaqueBoundingBox(const Frame& frame, const bool transparentIs0xFF, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u);

	if (frame.numberPlanes() != 1u)
	{
		return PixelBoundingBox();
	}

	bool alphaIsLast;
	const bool hasAlpha = FrameType::formatHasAlphaChannel(frame.pixelFormat(), &alphaIsLast);

	if (!hasAlpha)
	{
		ocean_assert(false && "The frame has no alpha channel!");
		return PixelBoundingBox();
	}

	const unsigned int channels = frame.channels();

	switch (frame.dataType())
	{
		case FrameType::dataType<uint8_t>():
			return detectOpaqueBoundingBox<uint8_t>(frame.constdata<uint8_t>(), frame.width(), frame.height(), alphaIsLast ? channels - 1u : 0u, channels, transparentIs0xFF ? 0xFFu : 0x00u, frame.paddingElements(), worker);

		case FrameType::dataType<uint16_t>():
			return detectOpaqueBoundingBox<uint16_t>(frame.constdata<uint16_t>(), frame.width(), frame.height(), alphaIsLast ? channels - 1u : 0u, channels, transparentIs0xFF ? 0xFFFFu : 0x0000u, frame.paddingElements(), worker);

		case FrameType::dataType<uint32_t>():
			return detectOpaqueBoundingBox<uint32_t>(frame.constdata<uint32_t>(), frame.width(), frame.height(), alphaIsLast ? channels - 1u : 0u, channels, transparentIs0xFF ? 0xFFFFFFFFu : 0x000000u, frame.paddingElements(), worker);

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return PixelBoundingBox();
}

}

}
