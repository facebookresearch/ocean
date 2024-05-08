/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerShrinkingPatchMatchingI1.h"

#include "ocean/cv/FrameFilterSobel.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

VectorI2 InitializerShrinkingPatchMatchingI1::InpaintingPixel::determineBorderDirection5x5(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position)
{
	ocean_assert(mask != nullptr);
	ocean_assert(position.x() < width && position.y() < height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// the given position must be a mask-border pixel
	ocean_assert_and_suppress_unused(mask[position.y() * maskStrideElements + position.x()] != 0xFF, maskStrideElements);

	VectorI2 direction(0, 0);

	for (unsigned int y = max(0, int(position.y()) - 2); y < min(position.y() + 3u, height); ++y)
	{
		for (unsigned int x = max(0, int(position.x()) - 2); x < min(position.x() + 3u, width); ++x)
		{
			constexpr uint8_t nonMaskPixelValue = 0xFF;

			if (!Segmentation::MaskAnalyzer::hasMaskNeighbor9<false>(mask, width, height, maskPaddingElements, CV::PixelPosition(x, y), nonMaskPixelValue))
			{
				ocean_assert(y != position.y() || x != position.x());

				direction += VectorI2(int(x) - int(position.x()), int(y) - int(position.y()));
			}
		}
	}

	return direction;
}

bool InitializerShrinkingPatchMatchingI1::invoke(Worker* worker) const
{
	ocean_assert(iterations_ >= 1);

	const Frame& frame = layer_.frame();

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		layerI_.mapping().reset();

		switch (frame.channels())
		{
			case 1u:
				return invoke<1u>(worker);

			case 2u:
				return invoke<2u>(worker);

			case 3u:
				return invoke<3u>(worker);

			case 4u:
				return invoke<4u>(worker);

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <unsigned int tChannels>
bool InitializerShrinkingPatchMatchingI1::invoke(Worker* worker) const
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	const Frame& frame = layer_.frame();

	Frame sobelResponse(FrameType(frame.width(), frame.height(), FrameType::genericPixelFormat<int16_t, tChannels * 2u>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<int16_t, tChannels>(frame.constdata<uint8_t>(), sobelResponse.data<int16_t>(), frame.width(), frame.height(), frame.paddingElements(), sobelResponse.paddingElements(), worker);

	RandomGenerator randomGenerator(randomGenerator_);

	if (!patchInitializationIteration5x5<tChannels, SSDWrapperMask>(sobelResponse, layer_.mask(), randomGenerator))
	{
		return false;
	}

	ocean_assert(iterations_ >= 1);
	for (unsigned int i = 1u; i < iterations_; ++i)
	{
		if (!patchInitializationIteration5x5<tChannels, SSDWrapper>(sobelResponse, layer_.mask(), randomGenerator))
		{
			return false;
		}
	}

	return true;
}

}

}

}
