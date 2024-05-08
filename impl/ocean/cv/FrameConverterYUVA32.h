/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_YUVA_32_H
#define META_OCEAN_CV_FRAME_CONVERTER_YUVA_32_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with YUVA32 pixel format to other pixel formats.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterYUVA32 : public FrameConverter
{
	public:

		/**
		 * Converts a YUVA 32 bit frame to a Y 8 bit frame using the exact conversion.
		* @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void convertYUVA32ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterYUVA32::convertYUVA32ToY8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	// source frame Y U V A
	//              0 1 2 3
	// target frame Y
	// pattern      0

	constexpr unsigned int shufflePattern = 0x0u;

	FrameChannels::shuffleChannels<uint8_t, 4u, 1u, shufflePattern>(source, target, width, height, flag, sourcePaddingElements, targetPaddingElements, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_YUVA_32_H
