/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_Y16_H
#define META_OCEAN_CV_FRAME_CONVERTER_Y16_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with Y16 pixel format.
 * @ingroup cv
 */
class FrameConverterY16 : public FrameConverter
{
	public:

		/**
		 * Converts a Y16 bit frame linearly to a Y8 bit frame.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertY16ToY8(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterY16::convertY16ToY8(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	FrameChannels::narrow16BitPerChannelTo8BitPerChannel</* tChannels */ 1u>(source, target, width, height, conversionFlag, sourcePaddingElements, targetPaddingElements, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_Y16_H
