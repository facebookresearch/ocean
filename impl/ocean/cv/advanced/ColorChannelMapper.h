/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_COLOR_CHANNEL_MAPPER_H
#define META_OCEAN_CV_ADVANCED_COLOR_CHANNEL_MAPPER_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * Provides methods for fast remapping of colors inside frames
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT ColorChannelMapper
{
	public:

		/**
		 * Callback function for color mapping
		 * First parameter: color channel index
		 * Second parameter: color channel input value
		 * Return value: color channel output value
		 */
		typedef Callback<uint8_t, unsigned int, uint8_t> ColorChannelMapFunction;

	public:

		/**
		 * Creates a look-up table using the specified color channel mapping function.
		 * The lookup data is a 2D image using 1 Byte per pixel.
		 * @param pixelFormat Pixel format
		 * @param lookupFrame Lookup table that is filled
		 * @param function The function that defines the color channel mapping
		 * @return True, if succeeded
		 */
		static bool createLookup8BitsPerChannel(FrameType::PixelFormat pixelFormat, Frame& lookupFrame, const ColorChannelMapFunction& function);

		/**
		 * Maps the colors of the specified frame to new values using the specified look-up table
		 * @param frame Image frame that is modified, must be valid
		 * @param lookupFrame Lookup table that is used, must be valid
		 * @param worker Optional worker object used for load distribution
		 */
		static void applyLookup8BitsPerChannel(Frame& frame, const Frame& lookupFrame, Worker* worker = nullptr);

		/**
		 * Creates a lookup table for  color scaling and offset operation.
		 * This operation is only supported for frames using 8 bit per R, G and B channel and a zipped pixel format.
		 * The values of an optional alpha channel are not modified.
		 * @param pixelFormat Pixel format of image frame
		 * @param lookupFrame Lookup table that is filled
		 * @param scale Color scale factor
		 * @param offset Color offset
		 * @param gamma Exponent for gamma correction
		 */
		static void createLookupScaleOffset(const FrameType::PixelFormat& pixelFormat, Frame& lookupFrame, const VectorF3& scale, const VectorF3& offset, const VectorF3& gamma);

		/**
		 * Performs a color scaling and offset operation on the specified image frame.
		 * This operation is only supported for frames using 8 bit per R, G and B channel and a zipped pixel format.
		 * The values of an optional alpha channel are not modified.
		 * @param frame Image frame that is modified
		 * @param scale Color scale factor
		 * @param offset Color offset
		 * @param gamma Exponent for gamma correction
		 * @param worker Optional worker object used for load distribution
		 */
		static void mapScaleOffset(Frame& frame, const VectorF3& scale, const VectorF3& offset, const VectorF3& gamma, Worker* worker = nullptr);

	protected:

		/**
		 * Maps the colors of the specified frame to new values using the specified look-up table for a specified subset of pixel rows.
		 * @param frameData Pointer to image frame data that is modified, must be valid
		 * @param frameWidth Width of frame in pixel, with range [1, infinity)
		 * @param frameHeight Height of frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param lookupData Pointer to data of lookup table, must be valid
		 * @param firstRow Index of first pixel row, with range [0, frameHeight - 1]
		 * @param numberRows Number of pixel rows that are modified, with range [1, frameHeight - firstRow]
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void applyLookup8BitsPerChannelSubset(uint8_t* frameData, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int framePaddingElements, const uint8_t* lookupData, const unsigned int firstRow, const unsigned int numberRows);
};

/**
 * Defines a scaling and offset color channel mapping
 * @tparam tChannels Number of color channels
 * @ingroup cvadvanced
 */
template <unsigned int tChannels>
class ScaleOffsetMapping
{

	public:

		/**
		 * Creates a new instance with an identity mapping
		 */
		ScaleOffsetMapping();

		/**
		 * Creates a new instance with the specified scaling factor and offset.
		 * @param scale Color scaling factor
		 * @param offset Color offset
		 * @param gamma Exponent for gamma correction, should be greater than zero
		 */
		ScaleOffsetMapping(const float* scale, const float* offset, const float* gamma = nullptr);

		/**
		 * Maps a color value for the specified intensity index
		 * @param channel Color channel index
		 * @param input Color input value in the range of [0;255]
		 * @return Color output value in the range of [0;255]
		 */
		uint8_t map(const unsigned int channel, const uint8_t input);

	protected:

		/// Specifies color scale factor for each color channel
		float gamma_[tChannels];

		/// Specifies color scale factor for each color channel
		float scale_[tChannels];

		/// Specifies color offset for each color channel
		float offset_[tChannels];
};

template <unsigned int tChannels>
Ocean::CV::Advanced::ScaleOffsetMapping<tChannels>::ScaleOffsetMapping()
{
	for (unsigned int c = 0u; c < tChannels; c++)
	{
		scale_[c] = 1.0f;
		offset_[c] = 0.0f;
		gamma_[c] = 1.0f;
	}
}

template <unsigned int tChannels>
ScaleOffsetMapping<tChannels>::ScaleOffsetMapping(const float* scale, const float* offset, const float* gamma)
{
	for (unsigned int c = 0; c < tChannels; c++)
	{
		ocean_assert(gamma == nullptr || NumericF::isAbove(gamma[c], 0.0f));

		scale_[c] = scale[c];
		offset_[c] = offset == nullptr ? 0.0f : offset[c];
		gamma_[c] = gamma == nullptr ? 1.0f : max(NumericF::eps(), gamma[c]);
	}
}

template <unsigned int tChannels>
uint8_t ScaleOffsetMapping<tChannels>::map(const unsigned int channel, const uint8_t input)
{
	if (channel >= tChannels)
	{
		return input;
	}

	constexpr float inv255 = 1.0f / 255.0f;

	return uint8_t(minmax<int>(0u, int(((NumericF::pow(input * inv255, gamma_[channel]) * scale_[channel]) + offset_[channel]) * 255.0f + 0.5f), 255u));
}

template <unsigned int tChannels>
void ColorChannelMapper::applyLookup8BitsPerChannelSubset(uint8_t* frameData, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int framePaddingElements, const uint8_t* lookupData, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frameData != nullptr && lookupData != nullptr);
	ocean_assert(frameWidth > 0u && frameHeight > 0u);
	ocean_assert_and_suppress_unused(numberRows <= frameHeight, frameHeight);

	const unsigned int frameStrideElements = frameWidth * tChannels + framePaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		uint8_t* data = frameData + y * frameStrideElements;

		for (unsigned int x = 0u; x < frameWidth; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; c++)
			{
				*data = lookupData[*data * tChannels + c];
				++data;
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_COLOR_CHANNEL_MAPPER_H
