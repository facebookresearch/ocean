/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_B_G_R_24_H
#define META_OCEAN_CV_FRAME_CONVERTER_B_G_R_24_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to convert frames with B_G_R24 pixel format.
 * The B_G_R24 format holds the three planes/blocks of color channels.<br>
 * The first block covers the B channel and holds 8 bit per pixel.<br>
 * The second block covers the G channel and holds 8 bit per pixel.<br>
 * The third block covers the R channel and also holds 8 bit per pixel.
 * The layout of a B_G_R24 image looks like this:
 * <pre>
 *  b-plane:        g-plane:        r-plane:
 *  ---------       ---------       ---------
 * | B B B B |     | G G G G |     | R R R R |
 * | B B B B |     | G G G G |     | R R R R |
 * | B B B B |     | G G G G |     | R R R R |
 * | B B B B |     | G G G G |     | R R R R |
 *  ---------       ---------       ---------
 * </pre>
 * @ingroup cv
 */
class FrameConverterB_G_R24 : public FrameConverter
{
	public:

		/**
		 * Converts a B_G_R24 frame to a 24 bit BGR frame into a second image buffer.
		 * @param bSource The b source frame buffer, must be valid
		 * @param gSource The g source frame buffer, must be valid
		 * @param rSource The r source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param bSourcePaddingElements The number of padding elements at the end of each b-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param gSourcePaddingElements The number of padding elements at the end of each g-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param rSourcePaddingElements The number of padding elements at the end of each r-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertB_G_R24ToBGR24(const uint8_t* bSource, const uint8_t* gSource, const uint8_t* rSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int bSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int rSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts a B_G_R24 frame to a 24 bit RGB frame into a second image buffer.
		 * @param bSource The b source frame buffer, must be valid
		 * @param gSource The g source frame buffer, must be valid
		 * @param rSource The r source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param flag Determining the type of conversion
		 * @param bSourcePaddingElements The number of padding elements at the end of each b-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param gSourcePaddingElements The number of padding elements at the end of each g-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param rSourcePaddingElements The number of padding elements at the end of each r-source row, in (uint8_t) elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in (uint8_t) elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational to several CPU cores
		 */
		static inline void convertB_G_R24ToRGB24(const uint8_t* bSource, const uint8_t* gSource, const uint8_t* rSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int bSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int rSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);
};

inline void FrameConverterB_G_R24::convertB_G_R24ToBGR24(const uint8_t* bSource, const uint8_t* gSource, const uint8_t* rSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int bSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int rSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(bSource != nullptr && gSource != nullptr && rSource != nullptr && target != nullptr);

	const int options[4] =
	{
		// padding parameters
		int(bSourcePaddingElements), int(gSourcePaddingElements), int(rSourcePaddingElements), int(targetPaddingElements)
	};

	const void* sources[3] =
	{
		bSource,
		gSource,
		rSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>, options, worker);
}

inline void FrameConverterB_G_R24::convertB_G_R24ToRGB24(const uint8_t* bSource, const uint8_t* gSource, const uint8_t* rSource, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int bSourcePaddingElements, const unsigned int gSourcePaddingElements, const unsigned int rSourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(bSource != nullptr && gSource != nullptr && rSource != nullptr && target != nullptr);

	const int options[4] =
	{
		// padding parameters
		int(bSourcePaddingElements), int(gSourcePaddingElements), int(rSourcePaddingElements), int(targetPaddingElements)
	};

	const void* sources[3] =
	{
		bSource,
		gSource,
		rSource
	};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)(&target), width, height, flag, 1u, FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>, options, worker);
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_B_G_R_24_H
