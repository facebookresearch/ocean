/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_R_G_B_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_R_G_B_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with R_G_B24 pixel format.
 * The R_G_B24 format holds the three planes/blocks of color channels.<br>
 * The first block covers the R channel and holds 8 bit per pixel.<br>
 * The second block covers the G channel and holds 8 bit per pixel.<br>
 * The third block covers the B channel and also holds 8 bit per pixel.
 * The layout of a R_G_B24 image looks like this:
 * <pre>
 *  r-plane:        g-plane:        b-plane:
 *  ---------       ---------       ---------
 * | R R R R |     | G G G G |     | B B B B |
 * | R R R R |     | G G G G |     | B B B B |
 * | R R R R |     | G G G G |     | B B B B |
 * | R R R R |     | G G G G |     | B B B B |
 *  ---------       ---------       ---------
 * </pre>
 * @ingroup cv
 */
class FrameConverterR_G_B24 : public FrameConverter
{
	public:

		/**
		 * Converts a R_G_B24 frame to a 24 bit RGB frame into a second image buffer.
		 * @param rSource The r source frame buffer, must be valid
		 * @param gSource The g source frame buffer, must be valid
		 * @param bSource The b source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param rSourcePaddingElements The number of padding elements at the end of each r-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param gSourcePaddingElements The number of padding elements at the end of each g-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param bSourcePaddingElements The number of padding elements at the end of each b-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertR_G_B24ToRGB24(const uint8_t* rSource, const uint8_t* gSource, const uint8_t* bSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int rSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int bSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a R_G_B24 frame to a 24 bit BGR frame into a second image buffer.
		 * @param rSource The r source frame buffer, must be valid
		 * @param gSource The g source frame buffer, must be valid
		 * @param bSource The b source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param rSourcePaddingElements The number of padding elements at the end of each r-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param gSourcePaddingElements The number of padding elements at the end of each g-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param bSourcePaddingElements The number of padding elements at the end of each b-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertR_G_B24ToBGR24(const uint8_t* rSource, const uint8_t* gSource, const uint8_t* bSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int rSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int bSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterR_G_B24::convertR_G_B24ToRGB24(const uint8_t* rSource, const uint8_t* gSource, const uint8_t* bSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int rSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int bSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(rSource != nullptr && gSource != nullptr && bSource != nullptr && target != nullptr);

	const int options[4] =
	{
		// padding parameters
		int(rSourcePaddingElements), int(gSourcePaddingElements), int(bSourcePaddingElements), int(targetPaddingElements)
	};

	const void* sources[3] =
	{
		rSource,
		gSource,
		bSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterR_G_B24::convertR_G_B24ToBGR24(const uint8_t* rSource, const uint8_t* gSource, const uint8_t* bSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int rSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int bSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(rSource != nullptr && gSource != nullptr && bSource != nullptr && target != nullptr);

	const int options[4] =
	{
		// padding parameters
		int(rSourcePaddingElements), int(gSourcePaddingElements), int(bSourcePaddingElements), int(targetPaddingElements)
	};

	const void* sources[3] =
	{
		rSource,
		gSource,
		bSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_R_G_B_24_H
