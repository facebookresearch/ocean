/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_FRAME_FILTER_BLUR_H
#define META_OCEAN_CV_SEGMENTATION_FRAME_FILTER_BLUR_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/FrameFilterGaussian.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions allowing to blur image content.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT FrameFilterBlur
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.
		 */
		class OCEAN_CV_SEGMENTATION_EXPORT Comfort
		{
			public:

				/**
				 * Blurs several masked regions in an image.
				 * The resulting image will be a blurred version of the original image, with the masked regions blurred and the remaining regions untouched.
				 * The image color of blurred regions is defined by the average color of the mask region.
				 * @param image The image to be blurred, must be valid
				 * @param mask The mask defining the regions to be blurred, with pixel format FORMAT_Y8, mask values 0x00 define regions to be blurred, 0xFF define regions to remain untouched, must be valid
				 * @param blurBorder Optional border in pixel defining a smooth transition between blurred and non-blurred regions (outside of the masked region), with range [0, infinity), must be odd, 0 to avoid a smooth transition
				 * @param randomGenerator Optional random generator object to add tiny random nose to each blurred region, nullptr to avoid random noise
				 * @return True, if succeeded
				 */
				static bool blurMaskRegions(Frame& image, const Frame& mask, const unsigned int blurBorder = 5u, RandomGenerator* randomGenerator = nullptr);
		};

	public:

		/**
		 * Blurs several masked regions in an image.
		 * The resulting image will be a blurred version of the original image, with the masked regions blurred and the remaining regions untouched.
		 * The image color of blurred regions is defined by the average color of the mask region.
		 * @param image The image to be blurred, must be valid
		 * @param mask The mask defining the regions to be blurred, mask values 0x00 define regions to be blurred, 0xFF define regions to remain untouched, must be valid
		 * @param width The width of the image and mask frame in pixel, with range [1, infinity)
		 * @param height The height of the image and mask frame in pixel, with range [1, infinity)
		 * @param imagePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param blurBorder Optional border in pixel defining a smooth transition between blurred and non-blurred regions (outside of the masked region), with range [0, infinity), must be odd, 0 to avoid a smooth transition
		 * @param randomGenerator Optional random generator object to add tiny random nose to each blurred region, nullptr to avoid random noise
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels the image has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool blurMaskRegions8BitPerChannel(uint8_t* image, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int imagePaddingElements, const unsigned int maskPaddingElements, const unsigned int blurBorder = 5u, RandomGenerator* randomGenerator = nullptr);
};

template <unsigned int tChannels>
bool FrameFilterBlur::blurMaskRegions8BitPerChannel(uint8_t* image, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int imagePaddingElements, const unsigned int maskPaddingElements, const unsigned int blurBorder, RandomGenerator* randomGenerator)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(image != nullptr);
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(blurBorder == 0u || blurBorder % 2u == 1u);

	Frame imageFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), image, Frame::CM_USE_KEEP_LAYOUT, imagePaddingElements);

	const Frame imageMask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_USE_KEEP_LAYOUT, maskPaddingElements);

	// first, we determine the individual joined masks

	Frame separation(FrameType(width, height, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

	CV::Segmentation::MaskAnalyzer::MaskBlocks maskBlocks;
	CV::Segmentation::MaskAnalyzer::analyzeMaskSeparation8Bit(mask, width, height, maskPaddingElements, separation.data<uint32_t>(), separation.paddingElements(), maskBlocks);

	// now, we determine the average color value of each individual block and update the image content

	CV::PixelBoundingBoxes pixelBoundingBoxes;

	for (const CV::Segmentation::MaskAnalyzer::MaskBlock& maskBlock : maskBlocks)
	{
		ocean_assert(maskBlock.size() > 0 && maskBlock.size() <= width * height);

		CV::PixelBoundingBox pixelBoundingBox;

		uint64_t sumColors[tChannels] = {};

		size_t pixels = 0;

		for (unsigned int y = 0u; pixels != maskBlock.size() && y < separation.height(); ++y)
		{
			const uint32_t* const rowSepartion = separation.constrow<uint32_t>(y);
			const uint8_t* const rowImage = imageFrame.constrow<uint8_t>(y);

			for (unsigned int x = 0u; pixels != maskBlock.size() && x < separation.width(); ++x)
			{
				if (rowSepartion[x] == maskBlock.id())
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						sumColors[n] += rowImage[x * tChannels + n];
					}

					++pixels;

					pixelBoundingBox += CV::PixelPosition(x, y);
				}
			}
		}

		uint8_t averageColors[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			averageColors[n] = uint8_t((sumColors[n] + (pixels / 2)) / pixels);

			if (randomGenerator != nullptr)
			{
				averageColors[n] = uint8_t(minmax(0, int(averageColors[n]) + RandomI::random(*randomGenerator, -10, 10), 255));
			}
		}

		pixels = 0u;

		for (unsigned int y = pixelBoundingBox.top(); y < pixelBoundingBox.bottomEnd(); ++y)
		{
			const uint32_t* const rowSepartion = separation.constrow<uint32_t>(y);
			uint8_t* const rowImage = imageFrame.row<uint8_t>(y);

			for (unsigned int x = pixelBoundingBox.left(); x < pixelBoundingBox.rightEnd(); ++x)
			{
				if (rowSepartion[x] == maskBlock.id())
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						rowImage[x * tChannels + n] = averageColors[n];
					}

					++pixels;
				}
			}
		}

		pixelBoundingBoxes.push_back(pixelBoundingBox);
	}

	if (blurBorder != 0u && blurBorder % 2u != 0u)
	{
		// in case the user wants to apply a smooth border between the mask content and the surrounding (remaining image content)
		// we blend the results with a Gaussian blur

		ocean_assert(maskBlocks.size() == pixelBoundingBoxes.size());

		for (size_t n = 0; n < maskBlocks.size(); ++n)
		{
			const CV::PixelBoundingBox& pixelBoundingBox = pixelBoundingBoxes[n];

			const CV::PixelBoundingBox extendedBoundingBox = pixelBoundingBox.extended(blurBorder, 0u, 0u, width - 1u, height - 1u);

			Frame sourceSubFrame = imageFrame.subFrame(extendedBoundingBox.left(), extendedBoundingBox.top(), extendedBoundingBox.width(), extendedBoundingBox.height(), Frame::CM_USE_KEEP_LAYOUT);

			Frame blurredSubFrame;
			if (!CV::FrameFilterGaussian::filter(sourceSubFrame, blurredSubFrame, blurBorder))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			Frame blendMask = imageMask.subFrame(extendedBoundingBox.left(), extendedBoundingBox.top(), extendedBoundingBox.width(), extendedBoundingBox.height(), Frame::CM_COPY_REMOVE_PADDING_LAYOUT);

			CV::PixelPositions borderPixels;

			for (unsigned int iteration = 1u; iteration < blurBorder; ++iteration)
			{
				borderPixels.clear();

				for (unsigned int y = 0u; y < blendMask.height(); ++y)
				{
					const uint8_t* row = blendMask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < blendMask.width(); ++x)
					{
						if (*row == 0xFFu)
						{
							if ((x > 0u && *(row - 1) != 0xFFu) || (x < blendMask.width() - 1u && *(row + 1) != 0xFFu) || (y > 0u && *(row - blendMask.width()) != 0xFFu) || (y < blendMask.height() - 1u && *(row + blendMask.width()) != 0xFFu))
							{
								borderPixels.emplace_back(x, y);
							}
						}

						++row;
					}
				}

				const uint8_t targetColor = uint8_t((iteration * 255u) / blurBorder);

				uint8_t* blendMaskData = blendMask.data<uint8_t>();

				for (const CV::PixelPosition& borderPixel : borderPixels)
				{
					blendMaskData[borderPixel.index(blendMask.width())] = targetColor;
				}
			}

			CV::FrameBlender::blend8BitPerChannel<tChannels, true>(blurredSubFrame.constdata<uint8_t>(), blendMask.constdata<uint8_t>(), sourceSubFrame.data<uint8_t>(), sourceSubFrame.width(), sourceSubFrame.height(), blurredSubFrame.paddingElements(), blendMask.paddingElements(), sourceSubFrame.paddingElements());
		}
	}

	return true;
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_FRAME_FILTER_BLUR_H
