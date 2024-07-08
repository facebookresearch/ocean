/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_RGGB_10_PACKED_H
#define META_OCEAN_CV_FRAME_CONVERTER_RGGB_10_PACKED_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY10_Packed.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with RGGB10_PACKED pixel format.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterRGGB10_Packed : public FrameConverter
{
	public:

		/**
		 * Converts a RGGB10_PACKED frame to a BGR24 frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGGB10_PackedToBGR24(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGGB10_PACKED frame to a RGB24 frame.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGGB10_PackedToRGB24(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a RGGB10_PACKED frame to a RGB24 frame with black level subtraction, white balance, and gamma encoding
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [4, infinity), must be a multiple of 4
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param blackLevel The black level value that is subtracted from each element of the raw image before any other operation, range: [0, 1024)
		 * @param whiteBalance The white balancing scalars of the red, green, and blue channel (in that order), range: [0, infinity), must be valid, will be ignored channel-wise for values < 0
		 * @param gamma The gamma value that each pixel will be encoded with, range: [0, infinity), will be ignored if value is < 0
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 */
		static inline void convertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const uint16_t blackLevel, const float* whiteBalance, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterRGGB10_Packed::convertRGGB10_PackedToBGR24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	const int options[2] =
	{
		// padding parameters
		int(sourcePaddingElements), int(targetPaddingElements)
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)&source, (void**)&target, width, height, flag, 2u, FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit<2u, 1u, 0u>, options, worker);
}

inline void FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);

	const int options[2] =
	{
		// padding parameters
		int(sourcePaddingElements), int(targetPaddingElements)
	};

	FrameConverter::convertArbitraryPixelFormat((const void**)&source, (void**)&target, width, height, flag, 2u, FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const uint16_t blackLevel, const float* whiteBalance, const float gamma, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 4u && height >= 1u);
	ocean_assert(width % 4u == 0u);
	ocean_assert(whiteBalance != nullptr && whiteBalance[0] >= 0.0f && whiteBalance[1] >= 0.0f && whiteBalance[2] >= 0.0f);
	ocean_assert(gamma > 0.0f);

	// White balance as fixed-point numbers with 7 bit precision
	const unsigned int whiteBalance7[3] =
	{
		(unsigned int)(whiteBalance[0] * 128.0f + 0.5f),
		(unsigned int)(whiteBalance[1] * 128.0f + 0.5f),
		(unsigned int)(whiteBalance[2] * 128.0f + 0.5f),
	};

	const uint8_t* gammaLookupValues = FrameConverterY10_Packed::LookupTableManager::get().lookupTable(gamma);

	const FrameConverter::RGGB10ToRGB24AdvancedOptions options{blackLevel, {whiteBalance7[0], whiteBalance7[1], whiteBalance7[2]}, gammaLookupValues, sourcePaddingElements, targetPaddingElements};

	FrameConverter::convertArbitraryPixelFormat((const void**)(&source), (void**)(&target), width, height, flag, 2u, FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8BitAdvanced<0u, 1u, 2u>, &options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_RGGB_10_PACKED_H
