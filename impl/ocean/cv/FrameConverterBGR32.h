/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_BGR_32_H
#define META_OCEAN_CV_FRAME_CONVERTER_BGR_32_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert or to change frames with RGB pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterBGR32 : public FrameConverter
{
	public:

		/**
		 * Converts a BGR 32 bit frame to a RGB 24 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR32ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a BGR 32 bit frame to a RGBA 32 bit frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param alphaValue The value of the alpha channel to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertBGR32ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue = 0xFF, Worker* worker = nullptr);
};

inline void FrameConverterBGR32:: convertBGR32ToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame B G R -
	//              0 1 2 3
	// target frame R G B
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannels<uint8_t, 4u, 3u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

inline void FrameConverterBGR32::convertBGR32ToRGBA32(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame B G R -
	//              0 1 2 3
	// target frame R G B A
	// pattern      2 1 0
	constexpr unsigned int shufflePattern = 0x012u;

	FrameChannels::shuffleChannelsAndSetLastChannelValue<uint8_t, 4u, 4u, shufflePattern>(source, alphaValue, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

}

}


#endif // META_OCEAN_CV_FRAME_CONVERTER_BGR_32_H
