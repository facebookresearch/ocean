/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_FRAME_H
#define META_OCEAN_BASE_FRAME_H

#include "ocean/base/Base.h"
#include "ocean/base/DataType.h"
#include "ocean/base/ObjectRef.h"
#include "ocean/base/StackHeapVector.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

/**
 * Definition of a frame type composed by the frame dimension, pixel format and pixel origin.
 * The frame dimension of a frame specifies the number of pixels in horizontal (width) and vertical (height) direction.<br>
 * The pixel format specifies which kind of image information is stored.<br>
 * The pixel origin defines whether the image data starts at the top left corner or at the bottom left corner.<br>
 * @ingroup base
 */
class OCEAN_BASE_EXPORT FrameType
{
	public:

		/**
		 * Definition of individual channel data type.
		 */
		enum DataType : uint8_t
		{
			/// Undefined data type.
			DT_UNDEFINED = 0u,
			/// Unsigned 8 bit integer data type (uint8_t).
			DT_UNSIGNED_INTEGER_8,
			/// Signed 8 bit integer data type (int8_t).
			DT_SIGNED_INTEGER_8,
			/// Unsigned 16 bit integer data type (uint16_t).
			DT_UNSIGNED_INTEGER_16,
			/// Signed 16 bit integer data type (int16_t).
			DT_SIGNED_INTEGER_16,
			/// Unsigned 32 bit integer data type (uint32_t).
			DT_UNSIGNED_INTEGER_32,
			/// Signed 232 bit integer data type (int32_t).
			DT_SIGNED_INTEGER_32,
			/// Unsigned 64 bit integer data type (uint64_t).
			DT_UNSIGNED_INTEGER_64,
			/// Signed 64 bit integer data type (int64_t).
			DT_SIGNED_INTEGER_64,
			/// Signed 16 bit float data type.
			DT_SIGNED_FLOAT_16,
			/// Signed 32 bit float data type (float).
			DT_SIGNED_FLOAT_32,
			/// Signed 64 bit float data type (double).
			DT_SIGNED_FLOAT_64,
			/// The helper data type which can be used to identify the last defined data type, DT_END is exclusive.
			DT_END
		};

		/**
		 * Definition of a vector holding data types.
		 */
		using DataTypes = std::vector<DataType>;

	protected:

		/// The number of bits the channel value is shifted within the PixelFormat value.
		static constexpr uint32_t pixelFormatBitOffsetChannels = 16u;

		/// The number of bits the data type value is shifted within the PixelFormat value.
		static constexpr uint32_t pixelFormatBitOffsetDatatype = pixelFormatBitOffsetChannels + 8u;

		/// The number of bits the planes value is shifted within the PixelFormat value.
		static constexpr uint32_t pixelFormatBitOffsetPlanes = pixelFormatBitOffsetDatatype + 8u;

		/// The number of bits the width-multiple value is shifted within the PixelFormat value.
		static constexpr uint32_t pixelFormatBitOffsetWidthMultiple = pixelFormatBitOffsetPlanes + 8u;

		/// The number of bits the height-multiple value is shifted within the PixelFormat value.
		static constexpr uint32_t pixelFormatBitOffsetHeightMultiple = pixelFormatBitOffsetWidthMultiple + 8u;

		/**
		 * This class implements a helper class allowing to create generic pixel formats.
		 * @tparam tDataType The data type of the generic pixel format
		 * @tparam tChannels The number of channels of the pixel format
		 * @tparam tPlanes The number of planes of the pixel format, a plane is a joined memory block
		 * @tparam tWidthMultiple The number of pixels the width of a frame must be a multiple of
		 * @tparam tHeightMultiple The number of pixels the height of a frame must be a multiple of
		 */
		template <DataType tDataType, uint32_t tChannels, uint32_t tPlanes, uint32_t tWidthMultiple, uint32_t tHeightMultiple>
		class GenericPixelFormat
		{
			public:

				/// The value of the generic pixel format.
				static constexpr uint64_t value = (uint64_t(tHeightMultiple) << pixelFormatBitOffsetHeightMultiple) | (uint64_t(tWidthMultiple) << pixelFormatBitOffsetWidthMultiple) | (uint64_t(tPlanes) << pixelFormatBitOffsetPlanes) |(uint64_t(tDataType) << pixelFormatBitOffsetDatatype) | (uint64_t(tChannels) << pixelFormatBitOffsetChannels);
		};

		/**
		 * Definition of a protected helper enum that simplifies to read the definition of a predefined pixel format.
		 */
		enum ChannelsValue : uint32_t
		{
			/// An invalid channel number, used for non-generic pixel formats.
			CV_CHANNELS_UNDEFINED = 0u,
			/// One channel.
			CV_CHANNELS_1 = 1u,
			/// Two channels.
			CV_CHANNELS_2 = 2u,
			/// Three channels.
			CV_CHANNELS_3 = 3u,
			/// Four channels.
			CV_CHANNELS_4 = 4u
		};

		/**
		 * Definition of a protected helper enum that simplifies to read the definition of a predefined pixel format.
		 */
		enum PlanesValue : uint32_t
		{
			/// One plane.
			PV_PLANES_1 = 1u,
			/// Two planes.
			PV_PLANES_2 = 2u,
			/// Three planes.
			PV_PLANES_3 = 3u,
			/// Four planes.
			PV_PLANES_4 = 4u
		};

		/**
		 * Definition of a protected helper enum that simplifies to read the definition of a predefined pixel format.
		 */
		enum MultipleValue : uint32_t
		{
			/// The size can have any value (as the value must be a multiple of 1).
			MV_MULTIPLE_1 = 1u,
			/// The size must have a multiple of 2.
			MV_MULTIPLE_2 = 2u,
			/// The size must have a multiple of 3.
			MV_MULTIPLE_3 = 3u,
			/// The size must have a multiple of 4.
			MV_MULTIPLE_4 = 4u
		};

	public:

		/**
		 * Definition of all pixel formats available in the Ocean framework.
		 * Several common pixel formats are predefined specifying a unique representation of the image information of a frame.<br>
		 * Further, generic pixel formats can be defined. Generic formats can have up to 31 zipped data channels and can be composed of any kind of data type.<br>
		 * The value of a pixel format can be separated into individual parts.<br>
		 * The lower two bytes can be used for predefined pixel formats.<br>
		 * The third byte define the number of data channels of all generic zipped pixel formats.<br>
		 * The fourth byte define the data type.<br>
		 * The fifth byte holds the number of planes.<br>
		 * The sixth byte holds the number of pixels the width of a frame must be a multiple of.<br>
		 * The seventh byte holds the number of pixels the height of a frame must be a multiple of:<br>
		 * <pre>
		 * Byte:   |     7      |         6         |         5        |     4      |       3      |       2        |      1      |      0      |
		 *         |   unused   |  height-multiple  |  width-multiple  |   planes   |   data type  | channel number |  predefined pixel format  |
		 * </pre>
		 * A generic zipped pixel format may have the same data layout compared to a predefined pixel format while the actual value of the pixel format may be different.
		 *
		 * The naming convention of predefined pixel formats is:<br>
		 * Left to right is equivalent from first to last bytes in memory.<br>
		 * For example RGB24 stored the red color value in the first byte and the blue color value in the last byte.<br>
		 * BGRA32 stores the blue color value in the first byte, green in the second bytes, red in the third byte and the alpha value in the last byte.
		 *
		 * The following code can be used to define a generic pixel format, in case the predefined pixel formats in 'PixelFormat' do not have the desired format:
		 * @code
		 * // define a used-defined pixel format with three double values per channel
		 * const FrameType::PixelFormat newPixelFormat = FrameType::genericPixelFormat<double, 3u>();
		 * @endcode
		 */
		enum PixelFormat : uint64_t
		{
			/**
			 * Undefined pixel format.
			 */
			FORMAT_UNDEFINED = 0ull,

			/**
			 * Pixel format with byte order ABGR and 32 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  AAAAAAAABBBBBBBBGGGGGGGGRRRRRRRR AAAAAAAA ........
			 * </pre>
			 */
			FORMAT_ABGR32 = 1ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order ARGB and 32 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB AAAAAAAA ........
			 * </pre>
			 */
			FORMAT_ARGB32 = 2ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order BGR and 24 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                        1                        2
			 *    Byte:  0       1        2       3       4       5        6
			 *     Bit:  0123456789ABCDEF01234567 89ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2        0       1       2        0
			 *   Color:  BBBBBBBBGGGGGGGGRRRRRRRR BBBBBBBBGGGGGGGGRRRRRRRR BBBBBBBB ........
			 * </pre>
			 */
			FORMAT_BGR24 = 3ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_3, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order BGR and 24 bits per pixel and 8 unused bits.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2                0
			 *   Color:  BBBBBBBBGGGGGGGGRRRRRRRR         BBBBBBBB ........
			 * </pre>
			 */
			FORMAT_BGR32 = 4ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 12 bits for BGR and 4 unused bits.
			 */
			FORMAT_BGR4444 = 5ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 15 bits for BGR and 1 unused bit.
			 */
			FORMAT_BGR5551 = 6ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 16 bits per pixel, 5 bits for blue, 6 bits for green and 5 bits for red.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0       1        2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01234
			 * Channel:  0    1     2     0    1     2     0
			 *   Color:  BBBBBGGGGGGRRRRR BBBBBGGGGGGRRRRR BBBBB ........
			 * </pre>
			 * This pixel format is equivalent to the following pixel formats on Android (note the inverse order or RGB):<br>
			 * Native code: ANDROID_BITMAP_FORMAT_RGB_565, Java: Bitmap.Config.RGB_565.
			 */
			FORMAT_BGR565 = 7ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order BGRA and 32 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  BBBBBBBBGGGGGGGGRRRRRRRRAAAAAAAA BBBBBBBB ........
			 * </pre>
			 */
			FORMAT_BGRA32 = 8ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 4 bits for each channel.
			 */
			FORMAT_BGRA4444 = 9ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * The packed pixel format representing a Bayer mosaic pattern for images with blue, green, and red channels with order BGGR for a 2x2 pixel block.
			 * The format has the byte order B G for the upper two pixels, and G R for the lower two pixels in a 2x2 pixel block.<br>
			 * Images with this pixel format have a resolution which is a multiple of 4x2 pixels.<br>
			 * The Pixel format stores 10 bits per pixel (and channel), packed so that four consecutive pixels fit into five bytes.<br>
			 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.<br>
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:        0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
			 *    Byte:        0        1        2        3        4          5        6        7        8        9
			 *     Bit:        01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
			 * Channel: row 0: 0        1        0        1        0 1 0 1    0        1        0        1        0 1 0 1
			 * Channel: row 1: 1        2        1        2        1 2 1 2    1        2        1        2        1 2 1 2
			 *   Color: row 1: BBBBBBBB GGGGGGGG BBBBBBBB GGGGGGGG BBGGBBGG   BBBBBBBB GGGGGGGG BBBBBBBB GGGGGGGG BBGGBBGG ........
			 *   Color: row 0: GGGGGGGG RRRRRRRR GGGGGGGG RRRRRRRR GGRRGGRR   GGGGGGGG RRRRRRRR GGGGGGGG RRRRRRRR GGRRGGRR ........
			 *   Color: row 2: BBBBBBBB GGGGGGGG ........
			 * </pre>
			 */
			FORMAT_BGGR10_PACKED = 10ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_4, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with byte order RGB and 24 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                        1                        2
			 *    Byte:  0       1        2       3       4       5        6
			 *     Bit:  0123456789ABCDEF01234567 89ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2        0       1       2        0
			 *   Color:  RRRRRRRRGGGGGGGGBBBBBBBB RRRRRRRRGGGGGGGGBBBBBBBB RRRRRRRR ........
			 * </pre>
			 */
			FORMAT_RGB24 = 11ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_3, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order RGB and 24 bits per pixel and 8 unused bits.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2                0
			 *   Color:  RRRRRRRRGGGGGGGGBBBBBBBB         RRRRRRRR ........
			 * </pre>
			 */
			FORMAT_RGB32 = 12ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 12 bits for RGB and 4 unused bits.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0       1        2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01234
			 * Channel:  0    1     2     0    1     2     0
			 *   Color:  RRRRGGGGBBBB     RRRRGGGGBBBB     RRRRR ........
			 * </pre>
			 */
			FORMAT_RGB4444 = 13ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 15 bits for RGB and 1 unused bit.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0       1        2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01234
			 * Channel:  0    1     2     0    1     2     0
			 *   Color:  RRRRRGGGGGBBBBB  RRRRRGGGGGBBBBB  RRRRR ........
			 * </pre>
			 */
			FORMAT_RGB5551 = 14ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 5 bits for red, 6 bits for green and 5 bits for blue.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0       1        2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01234
			 * Channel:  0    1     2     0    1     2     0
			 *   Color:  RRRRRGGGGGGBBBBB RRRRRGGGGGGBBBBB RRRRR ........
			 * </pre>
			 */
			FORMAT_RGB565 = 15ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order RGBA and 32 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA RRRRRRRR ........
			 * </pre>
			 * This pixel format is equivalent to the following pixel formats on Android (note the inverse order of ARGB):<br>
			 * Native code: ANDROID_BITMAP_FORMAT_RGBA_8888, Java: Bitmap.Config.ARGB_8888.
			 */
			FORMAT_RGBA32 = 16ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with entirely 16 bits per pixel, 4 bits for each channel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0       1        2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01234
			 * Channel:  0    1     2     0    1     2     0
			 *   Color:  RRRRGGGGBBBBAAAA RRRRGGGGBBBBAAAA RRRRR ........
			 * </pre>
			 */
			FORMAT_RGBA4444 = 17ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order RGBT and 24 bits for the RGB channels and 8 bits for an arbitrary texture channel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  RRRRRRRRGGGGGGGGBBBBBBBBTTTTTTTT RRRRRRRR ........
			 * </pre>
			 */
			FORMAT_RGBT32 = 18ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * The packed pixel format representing a Bayer mosaic pattern for images with red, green, and blue channels with order RGGB for a 2x2 pixel block.
			 * The format has the byte order R G for the upper two pixels, and G B for the lower two pixels in a 2x2 pixel block.<br>
			 * Images with this pixel format have a resolution which is a multiple of 4x2 pixels.<br>
			 * The Pixel format stores 10 bits per pixel (and channel), packed so that four consecutive pixels fit into five bytes.<br>
			 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.<br>
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:        0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
			 *    Byte:        0        1        2        3        4          5        6        7        8        9
			 *     Bit:        01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
			 * Channel: row 0: 0        1        0        1        0 1 0 1    0        1        0        1        0 1 0 1
			 * Channel: row 1: 1        2        1        2        1 2 1 2    1        2        1        2        1 2 1 2
			 *   Color: row 0: RRRRRRRR GGGGGGGG RRRRRRRR GGGGGGGG RRGGRRGG   RRRRRRRR GGGGGGGG RRRRRRRR GGGGGGGG RRGGRRGG ........
			 *   Color: row 1: GGGGGGGG BBBBBBBB GGGGGGGG BBBBBBBB GGBBGGBB   GGGGGGGG BBBBBBBB GGGGGGGG BBBBBBBB GGBBGGBB ........
			 *   Color: row 2: RRRRRRRR GGGGGGGG ........
			 * </pre>
			 */
			FORMAT_RGGB10_PACKED = 19ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_4, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled U frame and 8 bits 2x2 sub-sampled V frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'I420'.
			 *
			 * The memory layout of a Y_U_V12 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:      v-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_U_V12 = 20ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with byte order YUV and 24 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                        1                        2
			 *    Byte:  0       1       2        3       4       5        6
			 *     Bit:  0123456789ABCDEF01234567 89ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2        0       1       2        0
			 *   Color:  YYYYYYYYUUUUUUUUVVVVVVVV YYYYYYYYUUUUUUUUVVVVVVVV YYYYYYYY ........
			 * </pre>
			 */
			FORMAT_YUV24 = 21ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_3, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order YUVA and 32 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                1
			 *    Byte:  0       1       2       3        4
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2       3        0
			 *   Color:  YYYYYYYYUUUUUUUUVVVVVVVVAAAAAAAA YYYYYYYY ........
			 * </pre>
			 */
			FORMAT_YUVA32 = 22ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order YUVA and 24 bits for the YUV channels and 8 bit for an arbitrary texture channel.
			 */
			FORMAT_YUVT32 = 23ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled V frame and 8 bits 2x2 sub-sampled U frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'YV12'.
			 *
			 * The memory layout of a Y_V_U12 image looks like this:
			 * <pre>
			 *  y-plane:        v-plane:      u-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_V_U12 = 24ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with byte order YVU and 24-bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                        1                        2
			 *    Byte:  0       1       2        3       4       5        6
			 *     Bit:  0123456789ABCDEF01234567 89ABCDEF0123456789ABCDEF 01234567
			 * Channel:  0       1       2        0       1       2        0
			 *   Color:  YYYYYYYYVVVVVVVVUUUUUUUU YYYYYYYYVVVVVVVVUUUUUUUU YYYYYYYY ........
			 * </pre>
			 */
			FORMAT_YVU24 = 25ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_3, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * This pixel format is deprecated and is currently an alias for FORMAT_Y_UV12_LIMITED_RANGE.
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled U and V zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV12'.
			 *
			 * The memory layout of a Y_UV12 image looks like this:
			 * <pre>
			 *  y-plane:        u/v-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_UV12 = 26ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_2, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled V and U zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV21'.
			 *
			 * The memory layout of a Y_VU12 image looks like this:
			 * <pre>
			 *  y-plane:        v/u-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_VU12 = 27ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_2, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bit Y pixel values zipped (interleaved) with 8 bits 2x1 (horizontal) sub-sampled U and V pixel values respectively, resulting in 16 bits per pixel.
			 * Sometimes also denoted as 'YUY2'.
			 *
			 * The memory layout of a YUYV16 image looks like this:
			 * <pre>
			 *  y/u/v-plane:
			 *  -----------------
			 * | Y U Y V Y U Y V |
			 * | Y U Y V Y U Y V |
			 * | Y U Y V Y U Y V |
			 * | Y U Y V Y U Y V |
			 *  -----------------
			 * </pre>
			 * The width must be even (multiple of two).
			 */
			FORMAT_YUYV16 = 28ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_2, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 8 bit Y pixel values zipped (interleaved) with 8 bits 2x1 (horizontal) sub-sampled U and V pixel values respectively, resulting in 16 bits per pixel.
			 * Sometimes also denoted as 'UYVY'.
			 *
			 * The memory layout of a UYVY16 image looks like this:
			 * <pre>
			 *  y/u/v-plane:
			 *  -----------------
			 * | U Y V Y U Y V Y |
			 * | U Y V Y U Y V Y |
			 * | U Y V Y U Y V Y |
			 * | U Y V Y U Y V Y |
			 *  -----------------
			 * </pre>
			 * The width must be even (multiple of two).
			 */
			FORMAT_UYVY16 = 29ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_2, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format for grayscale images with byte order Y and 8 bits per pixel.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0        1
			 *    Byte:  0        1
			 *     Bit:  01234567 89ABCDEF
			 * Channel:  0        0
			 *   Color:  YYYYYYYY YYYYYYYY ........
			 * </pre>
			 */
			FORMAT_Y8 = 30ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order Y and 10 bits per pixel, the upper 6 bits are unused.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                 1
			 *    Byte:  0        1        2        3
			 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF
			 * Channel:  0                 0
			 *   Color:  YYYYYYYY YY       YYYYYYYY YY       ........
			 * </pre>
			 */
			FORMAT_Y10 = 31ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order Y and 10 bits per pixel, packed so that four consecutive pixels fit into five bytes.
			 * The higher 8 bits of each pixel are stored in the first four bytes, the lower 2 bits of all four pixels are stored in the fifth byte.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0        1        2        3        0 1 2 3    4        5        6        7        4 5 6 7
			 *    Byte:  0        1        2        3        4          5        6        7        8        9
			 *     Bit:  01234567 89ABCDEF 01234567 89ABCDEF 01234567   01234567 89ABCDEF 01234567 89ABCDEF 01234567
			 * Channel:  0        0        0        0        0 0 0 0    0        0        0        0        0 0 0 0
			 *   Color:  YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY   YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY YYYYYYYY ........
			 * </pre>
			 */
			FORMAT_Y10_PACKED = 32ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_1, MV_MULTIPLE_4, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 16 bits Y frame.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                1                2
			 *    Byte:  0        1       2       3        4
			 *     Bit:  0123456789ABCDEF 0123456789ABCDEF 01
			 * Channel:  0                0
			 *   Color:  YYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYY YY ........
			 * </pre>
			 */
			FORMAT_Y16 = 33ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 32 bits Y frame.
			 */
			FORMAT_Y32 = 34ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_32, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 64 bits Y frame.
			 */
			FORMAT_Y64 = 35ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_64, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order YA and 16 bits per pixel.
			 */
			FORMAT_YA16 = 36ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_2, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order RGB and 48 bits per pixel, with 16 bit per component.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                                1
			 *    Byte:  0       1       2       3       4       5        6       7
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF 0123456789ABCDEF
			 * Channel:  0               1               2                0
			 *   Color:  RRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBB RRRRRRRRRRRRRRRR ........
			 * </pre>
			 */
			FORMAT_RGB48 = 37ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_3, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with byte order RGBA and 64 bits per pixel, with 16 bit per component.
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0                                                                1
			 *    Byte:  0       1       2       3       4       5       6       7        8       9
			 *     Bit:  0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF 0123456789ABCDEF
			 * Channel:  0               1               2               3                0
			 *   Color:  RRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBAAAAAAAAAAAAAAAA RRRRRRRRRRRRRRRR ........
			 * </pre>
			 */
			FORMAT_RGBA64 = 38ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_16, CV_CHANNELS_4, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * This pixel format is deprecated and is currently an alias for FORMAT_Y_U_V24_LIMITED_RANGE.
			 * Pixel format with 8 bits Y frame as individual block, followed 8 bits U frame as individual block, followed by a V frame as individual block, resulting in 24 bits per pixel.
			 * Sometimes also denoted as 'I444'.
			 *
			 * The memory layout of a Y_U_V24 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:        v-plane:
			 *  ---------       ---------       ---------
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 *  ---------       ---------       ---------
			 * </pre>
			 */
			FORMAT_Y_U_V24 = 39ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed 8 bits U frame as individual block, followed by a V frame as individual block, resulting in 24 bits per pixel.
			 * Sometimes also denoted as 'I444'.
			 *
			 * The pixel format is using a limited value range for the individual channels:
			 * <pre>
			 * Y channel: [16, 235]
			 * U channel: [16, 240]
			 * V channel: [16, 240]
			 * </pre>
			 *
			 * The memory layout of a Y_U_V24 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:        v-plane:
			 *  ---------       ---------       ---------
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 *  ---------       ---------       ---------
			 * </pre>
			 * @see FORMAT_Y_U_V24_FULL_RANGE.
			 */
			FORMAT_Y_U_V24_LIMITED_RANGE = FORMAT_Y_U_V24,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed 8 bits U frame as individual block, followed by a V frame as individual block, resulting in 24 bits per pixel.
			 * Sometimes also denoted as 'I444'.
			 *
			 * The pixel format is using a full value range for all three channels:
			 * <pre>
			 * Y channel: [0, 255]
			 * U channel: [0, 255]
			 * V channel: [0, 255]
			 * </pre>
			 *
			 * The memory layout of a Y_U_V24 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:        v-plane:
			 *  ---------       ---------       ---------
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 * | Y Y Y Y |     | U U U U |     | V V V V |
			 *  ---------       ---------       ---------
			 * </pre>
			 * @see FORMAT_Y_U_V24_LIMITED_RANGE.
			 */
			FORMAT_Y_U_V24_FULL_RANGE = 40ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format for grayscale images with byte order Y and 8 bits per pixel (with limited range).
			 *
			 * The pixel format is using a limited value range:
			 * <pre>
			 * Y channel: [16, 235]
			 * </pre>
			 *
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0        1
			 *    Byte:  0        1
			 *     Bit:  01234567 89ABCDEF
			 * Channel:  0        0
			 *   Color:  YYYYYYYY YYYYYYYY ........
			 * </pre>
			 * @see FORMAT_Y8_FULL_RANGE.
			 */
			FORMAT_Y8_LIMITED_RANGE = 41ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format for grayscale images with byte order Y and 8 bits per pixel (with full range).
			 *
			 * The pixel format is using a full value range:
			 * <pre>
			 * Y channel: [0, 255]
			 * </pre>
			 *
			 * Here is the memory layout:
			 * <pre>
			 *   Pixel:  0        1
			 *    Byte:  0        1
			 *     Bit:  01234567 89ABCDEF
			 * Channel:  0        0
			 *   Color:  YYYYYYYY YYYYYYYY ........
			 * </pre>
			 * @see FORMAT_Y8_LIMITED_RANGE.
			 */
			FORMAT_Y8_FULL_RANGE = FORMAT_Y8,

			/**
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled U and V zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV12'.
			 *
			 * The pixel format is using a limited value range for all three channels:
			 * <pre>
			 * Y channel: [16, 235]
			 * U channel: [16, 240]
			 * V channel: [16, 240]
			 * </pre>
			 *
			 * The memory layout of a Y_UV12 image looks like this:
			 * <pre>
			 *  y-plane:        u/v-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_UV12_FULL_RANGE.
			 */
			FORMAT_Y_UV12_LIMITED_RANGE = FORMAT_Y_UV12,

			/**
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled U and V zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV12'.
			 *
			 * The pixel format is using a full value range for all three channels:
			 * <pre>
			 * Y channel: [0, 255]
			 * U channel: [0, 255]
			 * V channel: [0, 255]
			 * </pre>
			 *
			 * The memory layout of a Y_UV12 image looks like this:
			 * <pre>
			 *  y-plane:        u/v-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |     | U V U V |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_UV12_LIMITED_RANGE.
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_UV12_FULL_RANGE = 42ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_2, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled V and U zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV21'.
			 *
			 * The pixel format is using a limited value range for all three channels:
			 * <pre>
			 * Y channel: [16, 235]
			 * V channel: [16, 240]
			 * U channel: [16, 240]
			 * </pre>
			 *
			 * The memory layout of a Y_VU12 image looks like this:
			 * <pre>
			 *  y-plane:        u/v-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_VU12_FULL_RANGE.
			 */
			FORMAT_Y_VU12_LIMITED_RANGE = FORMAT_Y_VU12,

			/**
			 * Pixel format with 8 bits Y frame as entire block, followed by 8 bits 2x2 sub-sampled V and U zipped (interleaved) pixels, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'NV21'.
			 *
			 * The pixel format is using a full value range for all three channels:
			 * <pre>
			 * Y channel: [0, 255]
			 * V channel: [0, 255]
			 * U channel: [0, 255]
			 * </pre>
			 *
			 * The memory layout of a Y_VU12 image looks like this:
			 * <pre>
			 *  y-plane:        u/v-plane:
			 *  ---------       ---------
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |     | V U V U |
			 * | Y Y Y Y |      ---------
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_VU12_LIMITED_RANGE.
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_VU12_FULL_RANGE = 43ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_2, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled U frame and 8 bits 2x2 sub-sampled V frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'I420'.
			 *
			 * The pixel format is using a limited value range for all three channels:
			 * <pre>
			 * Y channel: [16, 235]
			 * V channel: [16, 240]
			 * U channel: [16, 240]
			 * </pre>
			 *
			 * The memory layout of a Y_U_V12 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:      v-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_U_V12_FULL_RANGE.
			 */
			FORMAT_Y_U_V12_LIMITED_RANGE = FORMAT_Y_U_V12,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled U frame and 8 bits 2x2 sub-sampled V frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'I420'.
			 *
			 * The pixel format is using a full value range for all three channels:
			 * <pre>
			 * Y channel: [0, 255]
			 * V channel: [0, 255]
			 * U channel: [0, 255]
			 * </pre>
			 *
			 * The memory layout of a Y_U_V12 image looks like this:
			 * <pre>
			 *  y-plane:        u-plane:      v-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |     | U U |       | V V |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_U_V12_LIMITED_RANGE.
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_U_V12_FULL_RANGE = 44ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled V frame and 8 bits 2x2 sub-sampled U frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'YV12'.
			 *
			 * The pixel format is using a limited value range for all three channels:
			 * <pre>
			 * Y channel: [16, 235]
			 * V channel: [16, 240]
			 * U channel: [16, 240]
			 * </pre>
			 *
			 * The memory layout of a Y_V_U12 image looks like this:
			 * <pre>
			 *  y-plane:        v-plane:      u-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_V_U12_FULL_RANGE.
			 */
			FORMAT_Y_V_U12_LIMITED_RANGE = FORMAT_Y_V_U12,

			/**
			 * Pixel format with 8 bits Y frame as individual block, followed by 8 bits 2x2 sub-sampled V frame and 8 bits 2x2 sub-sampled U frame, both as individual blocks, resulting in 12 bits per pixel.
			 * Sometimes also denoted as 'YV12'.
			 *
			 * The pixel format is using a full value range for all three channels:
			 * <pre>
			 * Y channel: [0, 255]
			 * V channel: [0, 255]
			 * U channel: [0, 255]
			 * </pre>
			 *
			 * The memory layout of a Y_V_U12 image looks like this:
			 * <pre>
			 *  y-plane:        v-plane:      u-plane:
			 *  ---------       -----         -----
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |     | V V |       | U U |
			 * | Y Y Y Y |      -----         -----
			 * | Y Y Y Y |
			 *  ---------
			 * </pre>
			 * @see FORMAT_Y_V_U12_LIMITED_RANGE.
			 * Width and height must be even (multiple of two).
			 */
			FORMAT_Y_V_U12_FULL_RANGE = 45ull | GenericPixelFormat<DT_UNSIGNED_INTEGER_8, CV_CHANNELS_UNDEFINED /* as non-generic */, PV_PLANES_3, MV_MULTIPLE_2, MV_MULTIPLE_2>::value,

			/**
			 * Pixel format for a frame with one channel and 32 bit floating point precision per element.
			 */
			FORMAT_F32 = 46ull | GenericPixelFormat<DT_SIGNED_FLOAT_32, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * Pixel format for a frame with one channel and 64 bit floating point precision per element.
			 */
			FORMAT_F64 = 47ull | GenericPixelFormat<DT_SIGNED_FLOAT_64, CV_CHANNELS_1, PV_PLANES_1, MV_MULTIPLE_1, MV_MULTIPLE_1>::value,

			/**
			 * The helper pixel format which can be used to identify the last defined pixel format, FORMAT_END is exclusive.
			 */
			FORMAT_END = 48ull
		};

		/**
		 * Definition of a vector holding pixel formats.
		 */
		using PixelFormats = std::vector<PixelFormat>;

		/**
		 * Defines different types of frame origin positions.
		 */
		enum PixelOrigin : uint32_t
		{
			/// Invalid origin type.
			ORIGIN_INVALID = 0u,
			/// The first pixel lies in the upper left corner, the last pixel in the lower right corner.
			ORIGIN_UPPER_LEFT,
			/// The first pixel lies in the lower left corner, the last pixel in the upper right corner.
			ORIGIN_LOWER_LEFT
		};

	private:

		/**
		 * Helper struct allowing to get access to the properties of a pixel format with a debugger.
		 */
		struct PixelFormatProperties
		{
			/// The value of the pixel format if predefined (if the pixel format is e.g., FORMAT_RGB24, FORMAT_Y8, FORMAT_Y_UV12, ...), 0 if the pixel format is pure generic.
			uint16_t predefinedPixelFormat_;

			/// The number of channels, the pixel format has.
			uint8_t channels_;

			/// The data type of each elements of the pixel format.
			DataType dataType_;

			/// The number of individual planes of the pixel format.
			uint8_t planes_;

			/// The number of pixels the width of a frame must be a multiple of
			uint8_t widthMultiple_;

			/// The number of pixels the height of a frame must be a multiple of
			uint8_t heightMultiple_;

			/// Currently unused.
			uint8_t unused_;
		};

		static_assert(sizeof(PixelFormatProperties) == sizeof(std::underlying_type<PixelFormat>::type), "Invalid helper struct!");

		/**
		 * This union mainly contains the pixel format as value.
		 * In addition, this union allows to access a struct (genericPixelFormatStruct_) to investigate the individual components of a pixel format during a debugging session.<br>
		 * However, overall this union is nothing else but a wrapper around 'PixelFormat'.
		 */
		union PixelFormatUnion
		{
			public:

				/**
				 * Creates a new union object based on a given pixel format.
				 * @param pixelFormat The pixel format to be stored in the new union
				 */
				explicit inline PixelFormatUnion(const PixelFormat& pixelFormat);

			public:

				/**
				 * The actual pixel format defining the layout of the color space, the number of channels and the data type.
				 * In case, the pixel format is pure generic, use 'genericPixelFormatStruct_' during a debugging session to lookup the data type and channel number.
				 */
				PixelFormat pixelFormat_;

			private:

				/// The properties of the pixel format.
				PixelFormatProperties properties_;
		};

	public:

		/**
		 * Creates a new frame type with invalid parameters.
		 */
		FrameType() = default;

		/**
		 * Creates a new frame type.
		 * @param width The width of the frame in pixel, must match with the pixel format condition, widthMultiple()
		 * @param height The height of the frame in pixel, must match with the pixel format condition, heightMultiple()
		 * @param pixelFormat Pixel format of the frame
		 * @param pixelOrigin Pixel origin of the frame
		 */
		inline FrameType(const unsigned int width, const unsigned int height, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin);

		/**
		 * Creates a new frame type.
		 * @param type Frame type to copy most properties from
		 * @param width The width of the frame in pixel to be used instead of the width defined in the given frame type, must match with the pixel format condition, widthMultiple()
		 * @param height The height of the frame in pixel to be used instead of the height defined in the given frame type, must match with the pixel format condition, heightMultiple()
		 */
		inline FrameType(const FrameType& type, const unsigned int width, const unsigned int height);

		/**
		 * Creates a new frame type.
		 * @param type Frame type to copy most properties from
		 * @param pixelFormat Pixel format to be used instead of the pixel format defined in the given frame type
		 */
		inline FrameType(const FrameType& type, const PixelFormat pixelFormat);

		/**
		 * Creates a new frame type.
		 * @param type Frame type to copy most properties from
		 * @param pixelOrigin Pixel origin to be used instead of the pixel origin defined in the given frame type
		 */
		inline FrameType(const FrameType& type, const PixelOrigin pixelOrigin);

		/**
		 * Creates a new frame type.
		 * @param type Frame type to copy most properties from
		 * @param pixelFormat Pixel format of the frame
		 * @param pixelOrigin Pixel origin to be used instead of the pixel origin defined in the given frame type
		 */
		inline FrameType(const FrameType& type, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin);

		/**
		 * Returns the width of the frame format in pixel.
		 * @return Width in pixel
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the frame in pixel.
		 * @return Height in pixel
		 */
		inline unsigned int height() const;

		/**
		 * Returns the pixel format of the frame.
		 * @return Pixel format
		 */
		inline PixelFormat pixelFormat() const;

		/**
		 * Explicitly changes the pixel format of this frame.
		 * Beware: Commonly there is no need to change the pixel format explicitly.
		 * @param pixelFormat The new pixel format to be set, can be invalid
		 */
		inline void setPixelFormat(const PixelFormat pixelFormat);

		/**
		 * Returns the data type of the pixel format of this frame.
		 * @return The frame's data type
		 */
		inline DataType dataType() const;

		/**
		 * Returns the number of bytes which are necessary to store the data type of this frame.
		 * @return The number of bytes of the frame's data type, with range [1, infinity)
		 */
		inline unsigned int bytesPerDataType() const;

		/**
		 * Returns the number of individual channels the frame has.
		 * An invalid frame or a frame with undefined pixel format has 0 channels.
		 * @return Number of channels, with range [0, infinity)
		 */
		inline unsigned int channels() const;

		/**
		 * Returns the number of planes of the pixel format of this frame.
		 * @return The number of planes, with range [0, infinity)
		 */
		inline uint32_t numberPlanes() const;

		/**
		 * Returns the pixel origin of the frame.
		 * @return Pixel origin
		 */
		inline PixelOrigin pixelOrigin() const;

		/**
		 * Returns the number of pixels for the frame.
		 * @return Number of frame pixels
		 */
		inline unsigned int pixels() const;

		/**
		 * Returns the number of bytes necessary for the frame type, without padding at the end of frame rows.
		 * In case the pixel format holds more than one plane, the resulting number of bytes is the sum of all planes.
		 * Beware: An actual frame may have a larger size if the frame comes with padding at end of rows.
		 * @return The size of the memory necessary for this frame type in bytes, with range [0, infinity)
		 * @see Frame::size().
		 */
		unsigned int frameTypeSize() const;

		/**
		 * Returns whether the pixel format of this frame type is compatible with a given pixel format.
		 * Two pixel formats are compatible if:
		 * - Both pixel formats are identical, or
		 * - Both pixel formats are pure generic pixel formats with identical data type and channel number, or
		 * - One pixel format is not pure generic (e.g., FORMAT_RGB24), while the other pixel format is pure generic but has the same data type and channel number
		 * @param pixelFormat The pixel format to be checked, must be valid
		 * @return True, if the given pixel format is compatible
		 * @see isFrameTypeCompatible().
		 */
		inline bool isPixelFormatCompatible(const PixelFormat pixelFormat) const;

		/**
		 * Returns whether this frame type is compatible with a given frame type.
		 * Two frame types are compatible if:
		 * - Both types are identical, or
		 * - Both types have the same dimension and compatible pixel formats
		 * @param frameType The first frame type to be checked, must be valid
		 * @param allowDifferentPixelOrigins True, to allow different pixel origins; False, so that both frame types must have the same pixel origin
		 * @return True, if the given frame type is compatible
		 * @see isPixelFormatCompatible().
		 */
		inline bool isFrameTypeCompatible(const FrameType& frameType, const bool allowDifferentPixelOrigins) const;

		/**
		 * Returns whether two frame types are equal.
		 * @param right The right frame type
		 * @return True, if so
		 */
		bool operator==(const FrameType& right) const;

		/**
		 * Returns whether two frame types are not equal.
		 * @param right The right frame type
		 * @return True, if so
		 */
		inline bool operator!=(const FrameType& right) const;

		/**
		 * Returns whether the left frame type is 'smaller' than the right one.
		 * The operator does not compare the area of both frames but considers 'width', 'height', pixel format, and pixel origin to create a unique order between frame types.
		 * @param right The right frame type
		 * @return True, if so
		 */
		bool operator<(const FrameType& right) const;

		/**
		 * Returns whether this frame type is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the number of individual channels of a given pixel format.
		 * @param pixelFormat Pixel format to be check
		 * @return Number of channels
		 */
		static unsigned int channels(const PixelFormat pixelFormat);

		/**
		 * Returns the number of planes of a pixel format.
		 * @param pixelFormat The pixel format for which the number of planes will be returned
		 * @return The number of planes, with range [0, infinity)
		 */
		static inline uint32_t numberPlanes(const PixelFormat pixelFormat);

		/**
		 * Returns the (pixel format) data type of a given C++ data type.
		 * @return The data type of the given template parameter, DT_UNDEFINED if the C++ data type is not supported
		 * @tparam T The C++ data type for which the (pixel format) data type is returned
		 */
		template <typename T>
		static constexpr DataType dataType();

		/**
		 * Returns the data type of a pixel format.
		 * @param pixelFormat Pixel format to be check
		 * @return The data type of the given pixel format, DT_UNDEFINED if the pixel format is not supported
		 */
		static inline DataType dataType(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bytes which are necessary to store a specified data type.
		 * @param dataType The data type for which the number of bytes is requested
		 * @return The number of bytes per data type, with range [1, infinity)
		 */
		static unsigned int bytesPerDataType(const DataType dataType);

		/**
		 * Returns a specific generic pixel format with a specified data type, channel number, and plane number.
		 * @param dataType The data type of the generic format
		 * @param channels The number of channels of the generic format, with range [1, 31]
		 * @param planes The number of planes of the generic pixel format, with range [1, 255]
		 * @param widthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @param heightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 * @return Pixel format the resulting pixel format
		 */
		static constexpr inline PixelFormat genericPixelFormat(const DataType dataType, const uint32_t channels, const uint32_t planes = 1u, const uint32_t widthMultiple = 1u, const uint32_t heightMultiple = 1u);

		/**
		 * Returns a specific generic pixel format with specified bit per pixel per channel, channel number, and plane number.
		 * The overall number of bits per pixel will be bitsPerPixelChannel * channels
		 * @param bitsPerPixelChannel The number of bits each pixel and channel of the pixel format will have, with values (4, 8, 16, 32, 64)
		 * @param channels The number of channels of the generic format, with range [1, 31]
		 * @param planes The number of planes of the generic pixel format, with range [1, 255]
		 * @param widthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @param heightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 * @return Pixel format the resulting pixel format
		 */
		static PixelFormat genericPixelFormat(const unsigned int bitsPerPixelChannel, const uint32_t channels, const uint32_t planes = 1u, const uint32_t widthMultiple = 1u, const uint32_t heightMultiple = 1u);

		/**
		 * Returns a specific generic pixel format with a specified data type and channel number.
		 * @return Pixel format the resulting pixel format
		 * @tparam tDataType The data type of the generic format
		 * @tparam tChannels The number of channels of the generic format, with range [1, 31]
		 * @tparam tPlanes The number of planes of the generic pixel format, with range [1, 255]
		 * @tparam tWidthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @tparam tHeightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 *
		 * @code
		 * // a pixel format with 3 channels storing 'unsigned char' values for each channel
		 * const FrameType::PixelFormat pixelFormat3Channels = FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>();
		 *
		 * // a pixel format with 1 channel composed of 'float' values
		 * const FrameType::PixelFormat pixelFormat1Channel = FrameType::genericPixelFormat<FrameType::DT_SIGNED_FLOAT_32, 1u>();
		 * @endcode
		 * @see genericPixelFormat<TDataType, tChannels>();
		 */
		template <DataType tDataType, uint32_t tChannels, uint32_t tPlanes = 1u, uint32_t tWidthMultiple = 1u, uint32_t tHeightMultiple = 1u>
		constexpr static PixelFormat genericPixelFormat();

		/**
		 * Returns a specific generic pixel format with a specified data type and channel number.
		 * @return Pixel format the resulting pixel format
		 * @param channels The number of channels of the generic format, with range [1, 31]
		 * @param planes The number of planes of the generic pixel format, with range [1, 255]
		 * @param widthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @param heightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 * @tparam tDataType The data type of the generic format
		 *
		 * @code
		 * // a pixel format with 3 channels storing 'unsigned char' values for each channel
		 * const FrameType::PixelFormat pixelFormat3Channels = FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8>(3u);
		 *
		 * // a pixel format with 1 channel composed of 'float' values
		 * const FrameType::PixelFormat pixelFormat1Channel = FrameType::genericPixelFormat<FrameType::DT_SIGNED_FLOAT_32>(1u);
		 * @endcode
		 */
		template <DataType tDataType>
		constexpr static PixelFormat genericPixelFormat(const uint32_t channels, const uint32_t planes = 1u, const uint32_t widthMultiple = 1u, const uint32_t heightMultiple = 1u);

		/**
		 * Returns a specific generic pixel format with a specified data type and channel number.
		 * @return Pixel format the resulting pixel format
		 * @tparam TDataType The C++ data type for which the (pixel format) data type is returned
		 * @tparam tChannels The number of channels of the generic format, with range [1, 31]
		 * @tparam tPlanes The number of planes of the generic pixel format, with range [1, 255]
		 * @tparam tWidthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @tparam tHeightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 *
		 * The following code snippet shows how to use this function:
		 * @code
		 * // a pixel format with 3 channels storing 'unsigned char' values for each channel
		 * const FrameType::PixelFormat pixelFormat3Channels = FrameType::genericPixelFormat<unsigned char, 3u>();
		 *
		 * // a pixel format with 1 channel composed of 'float' values
		 * const FrameType::PixelFormat pixelFormat1Channel = FrameType::genericPixelFormat<float, 1u>();
		 * @endcode
		 * @see genericPixelFormat<tDataType, tChannels>();
		 */
		template <typename TDataType, uint32_t tChannels, uint32_t tPlanes = 1u, uint32_t tWidthMultiple = 1u, uint32_t tHeightMultiple = 1u>
		constexpr static PixelFormat genericPixelFormat();

		/**
		 * Returns a specific generic pixel format with a specified data type and channel number.
		 * @return Pixel format the resulting pixel format
		 * @param channels The number of channels of the generic format, with range [1, 31]
		 * @param planes The number of planes of the generic pixel format, with range [1, 255]
		 * @param widthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @param heightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 * @tparam TDataType The C++ data type for which the (pixel format) data type is returned
		 *
		 * The following code snippet shows how to use this function:
		 * @code
		 * // a pixel format with 3 channels storing 'unsigned char' values for each channel
		 * const FrameType::PixelFormat pixelFormat3Channels = FrameType::genericPixelFormat<unsigned char>(3u);
		 *
		 * // a pixel format with 1 channel composed of 'float' values
		 * const FrameType::PixelFormat pixelFormat1Channel = FrameType::genericPixelFormat<float>(1u);
		 * @endcode
		 */
		template <typename TDataType>
		constexpr static PixelFormat genericPixelFormat(uint32_t channels, const uint32_t planes = 1u, const uint32_t widthMultiple = 1u, const uint32_t heightMultiple = 1u);

		/**
		 * Converts a any pixel format into a generic one
		 * This function has no effect for input pixel formats which are already generic
		 * @param pixelFormat A pixel format
		 * @return The generic pixel format; for generic pixel formats output will be identical to input
		 */
		static inline PixelFormat makeGenericPixelFormat(const PixelFormat pixelFormat);

		/**
		 * Checks whether a given pixel format is a specific layout regarding data channels and data type.
		 * @param pixelFormat The pixel format to be checked
		 * @param dataType The expected data type of the given pixel format
		 * @param channels The expected number of channels of the given pixel format, with range [0, 31]
		 * @param planes The number of planes of the generic pixel format, with range [0, 255]
		 * @param widthMultiple The number of pixels the width of a frame must be a multiple of, with range [1, 255]
		 * @param heightMultiple The number of pixels the height of a frame must be a multiple of, with range [1, 255]
		 * @return True, if succeeded
		 */
		static inline bool formatIsGeneric(const PixelFormat pixelFormat, const DataType dataType, const uint32_t channels, const uint32_t planes = 1u, const uint32_t widthMultiple = 1u, const uint32_t heightMultiple = 1u);

		/**
		 * Checks whether a given pixel format is a generic pixel format.
		 * @param pixelFormat The pixel format to be checked
		 * @return True, if succeeded
		 */
		static inline bool formatIsGeneric(const PixelFormat pixelFormat);

		/**
		 * Checks whether a given pixel format is a pure generic pixel format.
		 * @param pixelFormat The pixel format to be checked
		 * @return True, if succeeded
		 */
		static inline bool formatIsPureGeneric(const PixelFormat pixelFormat);

		/**
		 * Returns the number of individual channels of a given generic pixel format.
		 * @param pixelFormat Generic pixel format to be checked
		 * @return The number of channels, 0 if the pixel format is not generic (e.g., FORMAT_Y_UV12)
		 */
		static unsigned int formatGenericNumberChannels(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bits of one pixel for a given generic pixel format.
		 * @param pixelFormat Pixel format to check
		 * @return Number of bits per pixel
		 */
		static inline unsigned int formatGenericBitsPerPixel(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bits of one pixel for the red channel.
		 * @param pixelFormat Pixel format to check
		 * @return Number of bits per pixel
		 */
		static unsigned int formatBitsPerPixelRedChannel(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bits of one pixel for the green channel.
		 * @param pixelFormat Pixel format to check
		 * @return Number of bits per pixel
		 */
		static unsigned int formatBitsPerPixelGreenChannel(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bits of one pixel for the blue channel.
		 * @param pixelFormat Pixel format to check
		 * @return Number of bits per pixel
		 */
		static unsigned int formatBitsPerPixelBlueChannel(const PixelFormat pixelFormat);

		/**
		 * Returns the number of bits of one pixel for the alpha channel.
		 * @param pixelFormat Pixel format to check
		 * @return Number of bits per pixel
		 */
		static unsigned int formatBitsPerPixelAlphaChannel(const PixelFormat pixelFormat);

		/**
		 * Returns whether a given pixel format holds an alpha channel.
		 * @param pixelFormat Pixel format to check
		 * @param isLastChannel Optional returning whether the alpha channel is the last channel (true) or whether it is the first channel (false)
		 * @return True, if so
		 */
		static bool formatHasAlphaChannel(const PixelFormat pixelFormat, bool* isLastChannel = nullptr);

		/**
		 * Returns whether a given pixel format is a packed pixel format.
		 * Packed pixel formats like FORMAT_BGGR10_PACKED or FORMAT_Y10_PACKED contain bytes providing color information for several individual pixels.
		 * @return True, if so
		 */
		static bool formatIsPacked(const PixelFormat pixelFormat);

		/**
		 * Returns the most suitable 1-plane pixel format for a given pixel format which may be composed of several planes.
		 * If the given pixel format is a generic 1-plane pixel format already, the same pixel format will be returned.
		 * Here is a table with some examples:
		 * <pre>
		 * Output pixel format:       Input pixel format:
		 * \<generic pixel format>     \<generic pixel format> (e.g., FORMAT_RGB24)
		 * FORMAT_BGR24               FORMAT_BGR4444, FORMAT_BGR5551, FORMAT_BGR565
		 * FORMAT_BGRA32              FORMAT_BGRA4444
		 * FORMAT_RGB24               FORMAT_RGB4444, FORMAT_RGB5551, FORMAT_RGB565
		 * FORMAT_RGBA32              FORMAT_RGBA4444
		 * FORMAT_YUV24,              FORMAT_Y_UV12, FORMAT_UYVY16, FORMAT_Y_U_V12, FORMAT_YUYV16, FORMAT_Y_U_V24
		 * FORMAT_YVU24               FORMAT_Y_VU12, FORMAT_Y_V_U12
		 * </pre>
		 * @param pixelFormat Pixel format for which the 1-plane pixel format will be returned
		 * @return The resulting 1-plane pixel format, FORMAT_UNDEFINED if no matching pixel format could be found
		 */
		static PixelFormat genericSinglePlanePixelFormat(const PixelFormat pixelFormat);

		/**
		 * Adds an alpha channel to a given pixel format.
		 * @param pixelFormat Pixel format without alpha channel
		 * @param lastChannel True, to add the alpha channel at the end of the data channels, otherwise the alpha channel will be added in front of the data channels
		 * @return Pixel format with alpha channel, if existing
		 */
		static PixelFormat formatAddAlphaChannel(const PixelFormat pixelFormat, const bool lastChannel = true);

		/**
		 * Removes an alpha channel from a given pixel format.
		 * @param pixelFormat Pixel format with alpha channel
		 * @return Pixel format without alpha channel, if existing
		 */
		static PixelFormat formatRemoveAlphaChannel(const PixelFormat pixelFormat);

		/**
		 * Returns the number of pixels the width of a frame must be a multiple of.
		 * @param pixelFormat Pixel format to return the number of pixels for
		 * @return Number of pixels
		 */
		static inline unsigned int widthMultiple(const PixelFormat pixelFormat);

		/**
		 * Returns the number of pixels the height of a frame must be a multiple of.
		 * @param pixelFormat Pixel format to return the number of pixels for
		 * @return Number of pixels
		 */
		static inline unsigned int heightMultiple(const PixelFormat pixelFormat);

		/**
		 * Returns the channels of a plane for a pixel format.
		 * @param imagePixelFormat The pixel format of the entire frame, must be valid
		 * @param planeIndex The index of the plane for which the channels will be returned, with range [0, numberPlanes(imagePixelFormat))
		 * @return The plane's channels, with range [0, infinity)
		 */
		static unsigned int planeChannels(const PixelFormat& imagePixelFormat, const unsigned int planeIndex);

		/**
		 * Returns the number of bytes of one pixel of a plane for a pixel format.
		 * Beware: This function will return 0 if the pixel format is a special packed format (e.g., FORMAT_Y10_PACKED) which does not allow to calculate the number of bytes per pixel.
		 * @param imagePixelFormat The pixel format of the entire frame, must be valid
		 * @param planeIndex The index of the plane for which the bytes per pixel will be returned, with range [0, numberPlanes(imagePixelFormat))
		 * @return The plane's number of bytes per pixel, will be 0 for special packed pixel formats like FORMAT_Y10_PACKED
		 */
		static inline unsigned int planeBytesPerPixel(const PixelFormat& imagePixelFormat, const unsigned int planeIndex);

		/**
		 * Returns the plane layout of a given pixel format.
		 * @param imagePixelFormat The pixel format of the image for which the plane layout will be returned, must be valid
		 * @param imageWidth The width of the image, in (image) pixel, with range [1, infinity)
		 * @param imageHeight The height of the image, in (image) pixel, with range [1, infinity)
		 * @param planeIndex The index of the plane for which the layout will be returned, with range [0, numberPlanes(imagePixelFormat) - 1]
		 * @param planeWidth The resulting width of the specified plane, in (plane) pixel, with range [1, infinity)
		 * @param planeHeight The resulting height of the specified plane, in (plane) pixel, with range [1, infinity)
		 * @param planeChannels The resulting number of channels the plane has, with range [1, infinity)
		 * @param planeWidthElementsMultiple Optional the resulting number of (plane) elements the width of the plane must be a multiple of, in elements, with range [1, infinity)
		 * @param planeHeightElementsMultiple Optional the resulting number of (plane) elements the height of the plane must be a multiple of, in elements, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool planeLayout(const PixelFormat imagePixelFormat, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int planeIndex, unsigned int& planeWidth, unsigned int& planeHeight, unsigned int& planeChannels, unsigned int* planeWidthElementsMultiple = nullptr, unsigned int* planeHeightElementsMultiple = nullptr);

		/**
		 * Returns the plane layout of a given frame type.
		 * @param frameType The frame type for which the plane layout will be returned, must be valid
		 * @param planeIndex The index of the plane for which the layout will be returned, with range [0, numberPlanes(imagePixelFormat) - 1]
		 * @param planeWidth The resulting width of the specified plane, in (plane) pixel, with range [1, infinity)
		 * @param planeHeight The resulting height of the specified plane, in (plane) pixel, with range [1, infinity)
		 * @param planeChannels The resulting number of channels the plane has, with range [1, infinity)
		 * @param planeWidthElementsMultiple Optional the resulting number of (plane) elements the width of the plane must be a multiple of, in elements, with range [1, infinity)
		 * @param planeHeightElementsMultiple Optional the resulting number of (plane) elements the height of the plane must be a multiple of, in elements, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static inline bool planeLayout(const FrameType& frameType, const unsigned int planeIndex, unsigned int& planeWidth, unsigned int& planeHeight, unsigned int& planeChannels, unsigned int* planeWidthElementsMultiple = nullptr, unsigned int* planeHeightElementsMultiple = nullptr);

		/**
		 * Translates a string containing a data type into the data type.<br>
		 * For example 'UNSIGNED_INTEGER_8' will be translated into DT_UNSIGNED_INTEGER_8.
		 * @param dataType Data type as string
		 * @return Data type as value
		 */
		static DataType translateDataType(const std::string& dataType);

		/**
		 * Translates a string containing a pixel format into the pixel format.<br>
		 * For example 'BGR24' will be translated into FORMAT_BGR24.
		 * @param pixelFormat Pixel format as string
		 * @return Pixel format as value
		 */
		static PixelFormat translatePixelFormat(const std::string& pixelFormat);

		/**
		 * Translates a string containing the pixel origin into the pixel origin value.<br>
		 * For example 'UPPER_LEFT' will be translated into ORIGIN_UPPER_LEFT.
		 * @param pixelOrigin Pixel origin as string
		 * @return Pixel origin as value
		 */
		static PixelOrigin translatePixelOrigin(const std::string& pixelOrigin);

		/**
		 * Translates a data type value into a string containing the data type.<br>
		 * For example the DT_UNSIGNED_INTEGER_8 will be translated into 'UNSIGNED_INTEGER_8'.
		 * @param dataType the data type as value
		 * @return The data type as string, 'UNDEFINED' if the data type is invalid or cannot be translated
		 */
		static std::string translateDataType(const DataType dataType);

		/**
		 * Translates a pixel format value into a string containing the pixel format.<br>
		 * For example the FORMAT_BGR24 will be translated into 'BGR24'.
		 * @param pixelFormat Pixel format as value
		 * @return Pixel format as string, 'UNDEFINED' if the pixel format is invalid or cannot be translated
		 */
		static std::string translatePixelFormat(const PixelFormat pixelFormat);

		/**
		 * Translates a pixel origin value into a string containing the pixel origin.<br>
		 * For example the ORIGIN_UPPER_LEFT will be translated into 'UPPER_LEFT'.
		 * @param pixelOrigin Pixel origin as value
		 * @return Pixel origin as string, 'INVALID' if the pixel origin is invalid or cannot be translated
		 */
		static std::string translatePixelOrigin(const PixelOrigin pixelOrigin);

		/**
		 * Returns a best fitting pixel format having the given number of bits per pixels.
		 * @param bitsPerPixel Number of bits per pixel the resulting pixel format will have, with range [1, infinity)
		 * @return Resulting pixel format, FORMAT_UNDEFINED if no pixel format can be found
		 */
		static PixelFormat findPixelFormat(const unsigned int bitsPerPixel);

		/**
		 * Returns a best fitting pixel format having the given number of bits per pixels.
		 * The following mappings are defined:
		 * <pre>
		 * DataType:                 Channels:    PixelFormat:
		 * DT_UNSIGNED_INTEGER_8     1            FORMAT_Y8
		 * DT_UNSIGNED_INTEGER_8     2            FORMAT_YA16
		 * DT_UNSIGNED_INTEGER_8     3            FORMAT_RGB24
		 * DT_UNSIGNED_INTEGER_8     4            FORMAT_RGBA32
		 * </pre>
		 * @param dataType The data type of each pixel element for which a pixel type is determined, must be valid
		 * @param channels The number of channels for which a pixel format will be determined, with range [1, infinity)
		 * @return Resulting pixel format, FORMAT_UNDEFINED if no pixel format can be found
		 */
		static PixelFormat findPixelFormat(const DataType dataType, const unsigned int channels);

		/**
		 * Returns whether two given pixel formats are compatible.
		 * Two pixel formats are compatible if:
		 * - Both pixel formats are identical, or
		 * - Both pixel formats are pure generic pixel formats with identical data type and channel number, or
		 * - One pixel format is not pure generic (e.g., FORMAT_RGB24), while the other pixel format is pure generic but has the same data type and channel number
		 * @param pixelFormatA The first pixel format to be checked, must be valid
		 * @param pixelFormatB The second pixel format to be checked, must be valid
		 * @return True, if both pixel formats are compatible
		 * @see areFrameTypesCompatible().
		 */
		static bool arePixelFormatsCompatible(const PixelFormat pixelFormatA, const PixelFormat pixelFormatB);

		/**
		 * Returns whether two given frame types are compatible.
		 * Two frame types are compatible if:
		 * - Both types are identical, or
		 * - Both types have the same dimension and compatible pixel formats
		 * @param frameTypeA The first frame type to be checked, must be valid
		 * @param frameTypeB The second frame type to be checked, must be valid
		 * @param allowDifferentPixelOrigins True, to allow different pixel origins; False, so that both frame types must have the same pixel origin
		 * @return True, if both frame types are compatible
		 * @see arePixelFormatsCompatible().
		 */
		static bool areFrameTypesCompatible(const FrameType& frameTypeA, const FrameType& frameTypeB, const bool allowDifferentPixelOrigins);

		/**
		 * Returns whether a given pointer has the same byte alignment as the size of the data type the pointer is pointing to.
		 * Actually, this function returns whether the following condition is true:
		 * <pre>
		 * size_t(data) % sizeof(T) == 0
		 * </pre>
		 * @param data The pointer to the memory to be checked, must be valid
		 * @return True, if so
		 */
		template <typename T>
		static inline bool dataIsAligned(const void* data);

		/**
		 * Returns the size of a given data type in bytes.
		 * This function is a simple wrapper around sizeof until `if constexpr` can be used.
		 * @return The size of the specified data type in bytes, with range [0, infinity)
		 * @tparam T The data type for which the type will be returned, can be void
		 */
		template <typename T>
		static constexpr size_t sizeOfType();

		/**
		 * Returns all defined data types.
		 * @return Ocean's defined data types
		 */
		static const FrameType::DataTypes& definedDataTypes();

		/**
		 * Returns all defined pixel formats.
		 * @return Ocean's defined pixel formats
		 */
		static const FrameType::PixelFormats& definedPixelFormats();

	private:

		/// Frame width in pixel, with range [0, infinity)
		unsigned int width_ = 0u;

		/// Frame height in pixel, with range [0, infinity)
		unsigned int height_ = 0u;

		/// The pixel format of the frame encapsulated in a union (mainly holding PixelFormat).
		PixelFormatUnion pixelFormat_ = PixelFormatUnion(FORMAT_UNDEFINED);

		/// The origin of the pixel data, either the upper left corner or the bottom left corner (if valid).
		PixelOrigin pixelOrigin_ = ORIGIN_INVALID;
};

// Forward declaration.
class Frame;

/**
 * Definition of a vector holding padding frames.
 * @see Frame.
 * @ingroup base
 */
using Frames = std::vector<Frame>;

/**
 * Definition of an object reference for frame objects.
 * @ingroup base
 */
using FrameRef = ObjectRef<Frame>;

/**
 * Definition of a vector holding frame references.
 * @ingroup base
 */
using FrameRefs = std::vector<FrameRef>;

/**
 * This class implements Ocean's image class.
 * An image is composed of several planes, each plane can store image content with interleaved color channels.
 * <pre>
 * Plane 0:
 *  ---------------------------------- ----------------------------
 * |                                  |                            |
 * |                                  |                            |
 * |                                  |<--- paddingElements(0) --->|   plane height (0)
 * |                                  |                            |
 * |                                  |                            |
 *  ---------------------------------- ----------------------------
 *
 * Plane 1:
 *  -------------- ------------------------
 * |              |                        |
 * |              |<- paddingElements(1) ->|   plane height (1)
 * |              |                        |
 *  -------------- ------------------------
 * </pre>
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Frame : public FrameType
{
	public:

		/**
		 * Definition of individual copy modes.
		 */
		enum CopyMode : uint32_t
		{
			/// The source memory is used only, no copy is created, the padding layout is preserved.
			CM_USE_KEEP_LAYOUT = 1u << 0u,
			/// Makes a copy of the source memory, but the new plane will not contain padding elements.
			CM_COPY_REMOVE_PADDING_LAYOUT = 1u << 1u,
			/// Makes a copy of the source memory, the padding layout is preserved, but the padding data is not copied.
			CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA = 1u << 2u,
			/// Makes a copy of the source memory, the padding layout is preserved, the padding data is copied as well.
			CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA = 1u << 3u,
		};

		/**
		 * Definition of advanced copy modes containing all copy modes from `CopyMode` but also some additional.
		 */
		enum AdvancedCopyMode : std::underlying_type<CopyMode>::type
		{
			/// Same as CM_USE_KEEP_LAYOUT.
			ACM_USE_KEEP_LAYOUT = CM_USE_KEEP_LAYOUT,
			/// Same as CM_COPY_REMOVE_PADDING_LAYOUT.
			ACM_COPY_REMOVE_PADDING_LAYOUT = CM_COPY_REMOVE_PADDING_LAYOUT,
			/// Same as CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA.
			ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA = CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
			/// Same as CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA.
			ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA = CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA,

			/// The source memory is used if the source is not owner of the memory; The source memory is copied if the source is owner of the memory, padding layout will be removed.
			ACM_USE_OR_COPY = ACM_USE_KEEP_LAYOUT | ACM_COPY_REMOVE_PADDING_LAYOUT,
			/// The source memory is used if the source is not owner of the memory; The source memory is copied if the source is owner of the memory, padding layout is preserved, but padding data is not copied.
			ACM_USE_OR_COPY_KEEP_LAYOUT = ACM_USE_KEEP_LAYOUT | ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
		};

		/**
		 * Definition of an image plane, a block of memory storing pixel data with interleaved channels (or just one channel).
		 * The plane does not store the specific pixel format or the width of the plane, as this information is part of the frame which owns the plane.
		 * A plane has the following memory layout:
		 * <pre>
		 * |<-------- plane width ----------->|<-- paddingElements -->|
		 *
		 *  ---------------------------------- -----------------------       ---
		 * |A0 A1 An-1 B0 B1 Bn-1 ...         |                       |       ^
		 * |...                               |                       |       |
		 * |                                  |                       |  plane height
		 * |                                  |                       |       |
		 * |                                  |                       |       V
		 *  ---------------------------------- -----------------------       ---
		 *
		 * |<------------------- stride bytes ----------------------->|
		 *
		 * With A0 first channel (or element) of first pixel, A1 second channel of first pixel, ...
		 * And B0 first channel of second pixel, ...
		 * </pre>
		 * Note that: strideBytes == (planeWidth * channels + paddingElements) * bytesPerElement.<br>
		 * A plane can have a different number of channels than a frame's pixel format which is owning the plane.<br>
		 * The plane's channels are defined in relation to the data type of each pixel:<br>
		 * A frame with pixel format FORMAT_RGB24 has three channels, one plane, and the plane has three channels (as the data type of each pixel element is uint8_t).<br>
		 * However, a frame with pixel format FORMAT_RGB565 has three channels, one plane, but the plane has one channel only (as the data type of each pixel is uint16_t).
		 */
		class OCEAN_BASE_EXPORT Plane
		{
			friend class Frame;

			public:

				/**
				 * Creates a new invalid plane.
				 */
				Plane() = default;

				/**
				 * Move constructor.
				 * @param plane The plane to be moved
				 */
				inline Plane(Plane&& plane) noexcept;

				/**
				 * Copy constructor.
				 * @param plane The plane to be copied, can be invalid
				 * @param advancedCopyMode The copy mode specifying whether the source memory is used or copied
				 */
				Plane(const Plane& plane, const AdvancedCopyMode advancedCopyMode = ACM_USE_OR_COPY_KEEP_LAYOUT) noexcept;

				/**
				 * Creates a new plane object with own allocated memory.
				 * @param width The width of the plane in pixel, with range [1, infinity)
				 * @param height The height of the plane in pixel, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type `T`, with range [1, infinity)
				 * @param elementTypeSize The size of each element of the new plane, in bytes, with range [1, infinity)
				 * @param paddingElements The optional number of padding elements at the end of each row, in elements, with range [0, infinity)
				 */
				Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object which is not creating a copy of the given memory. Instead, the memory pointer is just used.
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixel, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type `T`, with range [1, infinity)
				 * @param dataToUse Memory pointer of the read-only memory which will not be copied, must be valid
				 * @param paddingElements The optional number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @tparam T The data type of each element
				 */
				template <typename T>
				inline Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const T* dataToUse, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object which is not creating a copy of the given memory. Instead, the memory pointer is just used.
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixel, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type `T`, with range [1, infinity)
				 * @param dataToUse Memory pointer of the writable memory which will not be copied, must be valid
				 * @param paddingElements The optional number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @tparam T The data type of each element
				 */
				template <typename T>
				inline Plane(const unsigned int width, const unsigned int height, const unsigned int channels, T* dataToUse, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object by making a copy of the given memory.
				 * @param sourceDataToCopy The source data to be copied, must be valid
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixels, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type `T`, with range [1, infinity)
				 * @param targetPaddingElements The optional number of padding elements at the end of each row this new plane will have, in elements, with range [0, infinity)
				 * @param sourcePaddingElements The number of padding elements at the end of each row the given source memory has, in elements, with range [0, infinity)
				 * @param makeCopyOfPaddingData True, to copy the entire padding data of the source plane (both planes must have the same padding layout: `targetPaddingElements == sourcePaddingElements`); False, to skip the padding data when copying the plane
				 * @tparam T The data type of each element
				 */
				template <typename T>
				inline Plane(const T* sourceDataToCopy, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData = false) noexcept;

				/**
				 * Creates a new plane object by making a copy of the given memory.
				 * @param sourceDataToCopy The source data to be copied, must be valid
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixels, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type `T`, with range [1, infinity)
				 * @param sourcePaddingElements The number of padding elements at the end of each row the given source memory has, in elements, with range [0, infinity)
				 * @param copyMode The copy mode to be applied
				 * @tparam T The data type of each element
				 */
				template <typename T>
				inline Plane(const T* sourceDataToCopy, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const CopyMode copyMode) noexcept;

				/**
				 * Destructs a Plane object.
				 */
				inline ~Plane();

				/**
				 * Returns the width of the plane in pixel.
				 * @return The plane's width, in pixel, with range [0, infinity)
				 */
				inline unsigned int width() const;

				/**
				 * Returns the height of the plane in pixel.
				 * @return The plane's height, in pixel, with range [0, infinity)
				 */
				inline unsigned int height() const;

				/**
				 * Returns the channels of the plane.
				 * @return The plane's channels, with range [0, infinity)
				 */
				inline unsigned int channels() const;

				/**
				 * Returns the read-only memory pointer to this plane with a specific data type compatible with elementTypeSize().
				 * @return The plane's read-only memory pointer, nullptr if this plane is invalid
				 * @tparam T the data type of the resulting memory pointer, with `sizeof(T) == elementTypeSize()`
				 */
				template <typename T>
				inline const T* constdata() const;

				/**
				 * Returns the writable memory pointer to this plane with a specific data type compatible with elementTypeSize().
				 * @return The plane's writable memory pointer, nullptr if this plane is not writable or invalid
				 * @tparam T the data type of the resulting memory pointer, with `sizeof(T) == elementTypeSize()`
				 */
				template <typename T>
				inline T* data();

				/**
				 * Returns the number of padding elements at the end of each plane row, in elements.
				 * @return The number of padding elements, with range [0, infinity)
				 * @see paddingBytes().
				 */
				inline unsigned int paddingElements() const;

				/**
				 * Returns the number of padding bytes at the end of each plane row, in bytes.
				 * This function actually returns `paddingElements() * elementTypeSize()`.
				 * @return The number of padding bytes, with range [0, infinity)
				 * @see paddingElements().
				 */
				inline unsigned int paddingBytes() const;

				/**
				 * Returns the size of each element of this plane.
				 * @return The element size, in bytes
				 */
				inline unsigned int elementTypeSize() const;

				/**
				 * Returns the width of the plane in elements, the width does not contain optional padding elements.
				 * This is the number of elements in which image data is stored (the number of elements between start of a row and start of the padding elements):
				 * <pre>
				 * widthElements == width * channels == strideElements() - paddingElements()
				 * </pre>
				 * @return The number of elements, with range [0, infinity)
				 */
				inline unsigned int widthElements() const;

				/**
				 * Returns the width of the plane in bytes, the width does not contain optional padding elements.
				 * This is the number of bytes in which image data is stored (the number of bytes between start of a row and start of the padding elements):
				 * <pre>
				 * widthBytes == width * channels * elementTypeSize() == strideBytes - elementTypeSize() * paddingElements()
				 * </pre>
				 * @return The number of bytes, with range [0, infinity)
				 */
				inline unsigned int widthBytes() const;

				/**
				 * Returns the number of elements between the start positions of two consecutive rows, in elements.
				 * This function actually returns `width * channels + paddingElements`.
				 * @return The number of elements, with range [width * elementsPerPixel, infinity)
				 */
				inline unsigned int strideElements() const;

				/**
				 * Returns the number of bytes between the start positions of two consecutive rows, in bytes.
				 * @return The number of bytes, with range [width * bytesPerPlanePixel, infinity)
				 */
				inline unsigned int strideBytes() const;

				/**
				 * Returns the number of bytes which is used for each pixel.
				 * @return The number of bytes, with range [1, infinity), 0 if unknown
				 */
				inline unsigned int bytesPerPixel() const;

				/**
				 * Releases this plane and all resources of this plane.
				 */
				void release();

				/**
				 * Returns whether this plane is compatible with a given element data type.
				 * @tparam T The data type to be checked
				 * @return True, if so
				 */
				template <typename T>
				inline bool isCompatibleWithDataType() const;

				/**
				 * Returns the number of bytes necessary for the entire plane data including optional padding elements at the end of each row.
				 * This function actually returns `strideBytes() * height()`.
				 * @return Plane  size in bytes, with range [0, infinity)
				 */
				inline unsigned int size() const;

				/**
				 * Returns whether this plane is based on continuous memory and thus does not have any padding at the end of rows.
				 * @return True, if so
				 */
				inline bool isContinuous() const;

				/**
				 * Returns whether this plane is the owner of the memory.
				 * @return True, if the plane is the owner; False, if someone else is the owner
				 */
				inline bool isOwner() const;

				/**
				 * Returns whether this plane holds read-only memory.
				 * @return True, if the memory of the plane is not writable
				 */
				inline bool isReadOnly() const;

				/**
				 * Returns whether this plane holds valid data.
				 * @return True, if so; False, if the plane is empty
				 */
				inline bool isValid() const;

				/**
				 * Copies data from another plane into this plane.
				 * If this plane does not have a compatible memory, or if this plane's memory is not writable, reallocation will be done if `reallocateIfNecessary == true`.
				 * @param sourcePlane The source plane from which the memory will be copied, an invalid plane to release this plane
				 * @param advancedCopyMode The copy mode specifying whether the source memory is used or copied
				 * @param reallocateIfNecessary True, to reallocate new memory if this plane is not compatible with the source plane; False, to prevent a reallocation and to skip to copy the source plane
				 * @return True, if succeeded
				 */
				bool copy(const Plane& sourcePlane, const AdvancedCopyMode advancedCopyMode = ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA, const bool reallocateIfNecessary = true);

				/**
				 * Move operator.
				 * @param plane The plane to be moved
				 * @return The reference to this object
				 */
				Plane& operator=(Plane&& plane) noexcept;

				/**
				 * Copy operator.
				 * This plane will have the same stride/padding layout. However, the padding memory will not be copied.
				 * If the source plane is owner of the memory, this plane will be owner of an own copy of the memory.
				 * If the source plane is not the owner of the memory, this plane will also not be the owner but will use the memory of the source plane as well.
				 * @param plane The plane to be copied
				 * @return Reference to this object
				 */
				Plane& operator=(const Plane& plane) noexcept;

				/**
				 * Allocates memory with specific byte alignment.
				 * @param size The size of the resulting buffer in bytes, with range [0, infinity)
				 * @param alignment The requested byte alignment, with range [1, infinity)
				 * @param alignedData the resulting pointer to the aligned memory
				 * @return The allocated memory with arbitrary alignment
				 */
				static void* alignedMemory(const size_t size, const size_t alignment, void*& alignedData);

			protected:

				/**
				 * Creates a new plane object which is not creating a copy of the given memory. Instead, the memory pointer is just used.
				 * @param width The width of the plane, in pixel, with range [0, infinity)
				 * @param height The height of the plane, in pixel, with range [0, infinity)
				 * @param channels The channels of the plane, with range [0, infinity)
				 * @param elementTypeSize The size of each element in bytes, which is `sizeof(T)`, with range [1, infinity)
				 * @param constData The value for `constData` to be set
				 * @param data The value for `data` to be set
				 * @param paddingElements The optional number of padding elements at the end of each row, in elements, with range [0, infinity)
				 */
				inline Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* constData, void* data, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object which is not creating a copy of the given memory. Instead, the memory pointer is just used.
				 * @param width The width of the plane, in pixel, with range [0, infinity)
				 * @param height The height of the plane, in pixel, with range [0, infinity)
				 * @param channels The channels of the plane, with range [0, infinity)
				 * @param elementTypeSize The size of each element in bytes, which is `sizeof(T)`, with range [1, infinity)
				 * @param dataToUse Memory pointer of the read-only memory which will not be copied, must be valid
				 * @param paddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 */
				Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* dataToUse, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object which is not creating a copy of the given memory. Instead, the memory pointer is just used.
				 * @param width The width of the plane, in pixel, with range [0, infinity)
				 * @param height The height of the plane, in pixel, with range [0, infinity)
				 * @param channels The channels of the plane, with range [0, infinity)
				 * @param elementTypeSize The size of each element in bytes, which is `sizeof(T)`, with range [1, infinity)
				 * @param dataToUse Memory pointer of the read-only memory which will not be copied, must be valid
				 * @param paddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 */
				Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, void* dataToUse, const unsigned int paddingElements) noexcept;

				/**
				 * Creates a new plane object by making a copy of the given memory.
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixels, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type, with range [1, infinity)
				 * @param elementTypeSize The size of each element in bytes, which is `sizeof(T)`, with range [1, infinity)
				 * @param sourceDataToCopy The source data to be copied, must be valid
				 * @param targetPaddingElements The number of padding elements at the end of each row this new plane will have, in elements, with range [0, infinity)
				 * @param sourcePaddingElements The number of padding elements at the end of each row the given source memory has, in elements, with range [0, infinity)
				 * @param makeCopyOfPaddingData True, to copy the entire padding data of the source plane (both planes must have the same padding layout: `targetPaddingElements == sourcePaddingElements`); False, to skip the padding data when copying the plane
				 */
				Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* sourceDataToCopy, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData = false) noexcept;

				/**
				 * Creates a new plane object by making a copy of the given memory.
				 * @param width The width of the plane in pixels, one pixel has size `sizeof(T) * channels`, with range [1, infinity)
				 * @param height The height of the plane in pixels, with range [1, infinity)
				 * @param channels The number of channels the plane has, with respect to the specified data type, with range [1, infinity)
				 * @param elementTypeSize The size of each element in bytes, which is `sizeof(T)`, with range [1, infinity)
				 * @param sourceDataToCopy The source data to be copied, must be valid
				 * @param sourcePaddingElements The number of padding elements at the end of each row the given source memory has, in elements, with range [0, infinity)
				 * @param copyMode The copy mode to be applied
				 */
				Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* sourceDataToCopy, const unsigned int sourcePaddingElements, const CopyMode copyMode) noexcept;

				/**
				 * Copies memory into this plane which has compatible memory already.
				 * @param sourceData The source data from a compatible plane, must be valid
				 * @param sourceStrideBytes The number of bytes between the start positions of two consecutive rows in the source plane, in bytes, with range [strideBytes(), infinity)
				 * @param sourcePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
				 * @param makeCopyOfPaddingData True, to copy the entire padding data of the source plane (this plane mast have the same padding layout); False, to skip the padding data when copying the plane
				 */
				void copy(const void* sourceData, const unsigned int sourceStrideBytes, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData = false);

				/**
				 * Calculates the number of bytes between the start positions of two consecutive rows, in bytes.
				 * @return The number of bytes, with range [width * bytesPerPlanePixel, infinity).
				 */
				inline unsigned int calculateStrideBytes() const;

				/**
				 * Calculates the number of bytes per pixel.
				 * @return The number of bytes, with range [1, infinity), 0 if unknown
				 */
				unsigned int calculateBytesPerPixel() const;

			protected:

				/// The pointer to the memory which this plane has allocated, this pointer is pointing to the memory which needs to be freed when disposing the plane object, nullptr if the plane is not owner of the memory.
				void* allocatedData_ = nullptr;

				/// The pointer to the read-only memory of the plane (not the pointer to the allocated memory), nullptr, if the plane is not read-only, or invalid.
				const void* constData_ = nullptr;

				/// The pointer to the writable memory of the plane (not the pointer to the allocated memory), nullptr if the plane is not writable.
				void* data_ = nullptr;

				/// The width of the plane in pixel, with range [0, infinity).
				unsigned int width_ = 0u;

				/// The height of the plane in pixel, with range [0, infinity).
				unsigned int height_ = 0u;

				/// The number of channels the plane has, with range [0, infinity).
				unsigned int channels_ = 0u;

				/// The size of each element of this plane, in bytes, with range [0, infinity).
				unsigned int elementTypeSize_ = 0u;

				/// The number of padding elements at the end of each plane row, in elements, with range [0, infinity).
				unsigned int paddingElements_ = 0u;

				/// The number of bytes between the start positions of two consecutive rows, in bytes, identical to '(width_ * channels_ + paddingElements_) * elementTypeSize_`
				unsigned int strideBytes_ = 0u;

				/// The number of bytes per pixel, with range [1, infinity), 0 if unknown.
				unsigned int bytesPerPixel_ = 0u;
		};

		/**
		 * Definition of a vector storing planes.
		 */
		using Planes = StackHeapVector<Plane, 4>;

		/**
		 * This class implements a helper class which can be used to initialize a multi-plane frame in the constructor.
		 * The class is mainly a temporary storage for memory pointers, copy modes, and number of padding elements.
		 * @tparam T The data type of the frame's element type, can be `void` if unknown
		 */
		template <typename T>
		class PlaneInitializer
		{
			friend class Frame;

			public:

				/**
				 * Creates a new initializer object for a read-only memory pointer.
				 * @param constdata The read-only memory pointer to the plane data, must be valid
				 * @param copyMode The copy mode to be applied when initializing the plane
				 * @param dataPaddingElements The number of padding elements at the end of each row of the given memory pointer, in elements, with range [0, infinity)
				 */
				inline PlaneInitializer(const T* constdata, const CopyMode copyMode, const unsigned int dataPaddingElements = 0u);

				/**
				 * Creates a new initializer object for a writable memory pointer.
				 * @param data The writable memory pointer to the plane data, must be valid
				 * @param copyMode The copy mode to be applied when initializing the plane
				 * @param dataPaddingElements The number of padding elements at the end of each row of the given memory pointer, in elements, with range [0, infinity)
				 */
				inline PlaneInitializer(T* data, const CopyMode copyMode, const unsigned int dataPaddingElements = 0u);

				/**
				 * Creates a new initializer object for a new plane for which the number of padding elements is known.
				 * @param planePaddingElements The number of padding elements at the end of each row of the resulting plane, in elements, with range [0, infinity)
				 */
				explicit inline PlaneInitializer(const unsigned int planePaddingElements = 0u);

			protected:

				/**
				 * Creates plane initializer objects with padding elements only.
				 * @param paddingElementsPerPlane The padding elements one value for each plane
				 * @return The resulting plane initializer objects
				 */
				static std::vector<PlaneInitializer<T>> createPlaneInitializersWithPaddingElements(const Indices32& paddingElementsPerPlane);

			protected:

				/// The pointer to the read-only source memory, can be nullptr.
				const T* constdata_ = nullptr;

				/// The pointer to the writable source memory, can be nullptr.
				T* data_ = nullptr;

				/// The copy mode to be applied, unused if `constdata_ == nullptr` and `data_ == nullptr`.
				CopyMode copyMode_ = CopyMode(0u);

				/// If a valid memory pointer is provided, the number of padding elements at the end of each source memory row; Otherwise, the number of padding elements at the end row of the new plane, with range [0, infinity)
				unsigned int paddingElements_ = 0u;
		};

		/**
		 * Definition of a vector holding plane initializer objects.
		 * @tparam T The data type of the frame's element type.
		 */
		template <typename T>
		using PlaneInitializers = std::vector<PlaneInitializer<T>>;

		/**
		 * Definition of a data type storing all channel values of one pixel in an array.
		 * @tparam T The data type of each pixel channel
		 * @tparam tChannels The number of channels the pixel has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		using PixelType = typename Ocean::DataType<T, tChannels>::Type;

	public:

		/**
		 * Creates an empty frame.
		 */
		inline Frame();

		/**
		 * Creates a second version of a given frame.
		 * If the given source frame is not owner of the frame data, this new frame will also not be owner of the frame data.<br>
		 * But, if the given source frame is the owner of the frame data, this new frame will also be the owner of new copy of the frame data.<br>
		 * Thus, the following two lines of code produce the same result:
		 * @code
		 * Frame newFrameA(frame);
		 * Frame newFrameB(frame, ACM_USE_OR_COPY);
		 * ocean_assert(newFrameB.isOwner() == false || newFrameB.isContinuous());
		 * @endcode
		 * This function behaves similar like the normal assign operator.
		 * Whenever a copy is created, the memory layout of the resulting frame will be continuous.
		 * @param frame The frame to copy
		 */
		Frame(const Frame& frame);

		/**
		 * Move constructor.
		 * @param frame The frame to be moved
		 */
		inline Frame(Frame&& frame) noexcept;

		/**
		 * Creates a second version of a given frame.
		 * Beware: The pixel memory will either be copied or used only, this depends on 'advancedCopyMode'.<br>
		 * @param frame The frame to copy, can be invalid
		 * @param advancedCopyMode The copy mode to be applied
		 */
		Frame(const Frame& frame, const AdvancedCopyMode advancedCopyMode) noexcept;

		/**
		 * Creates a frame with specified width, height, pixel format and frame origin and an optional padding.
		 * The necessary buffer is allocated but not initialized.
		 * @param frameType Type of the frame, must be valid
		 * @param paddingElementsPerPlane The padding elements at the end of each individual plane row, in elements of the pixel format, one for each plane, an empty vector to define a frame without padding
		 * @param timestamp The timestamp of the frame
		 */
		explicit inline Frame(const FrameType& frameType, const Indices32& paddingElementsPerPlane = Indices32(), const Timestamp& timestamp = Timestamp(false));

		/**
		 * Deprecated: Use Frame(const FrameType& frameType, const Indices32& paddingElements, const Timestamp& timestamp) instead.
		 *
		 * Creates a new one-plane frame by given width, height, pixel format and frame origin and an optional padding.
		 * The necessary buffer is allocated but not initialized.
		 * @param frameType Type of the frame, must be valid
		 * @param paddingElements Optional number of elements at the end of each row, one pixel has (1 * channels) elements, must be 0 for non-generic pixel formats (e.g., Y_UV12), with range [0, infinity)
		 * @param timestamp The timestamp of the frame
		 */
		explicit inline Frame(const FrameType& frameType, const unsigned int paddingElements, const Timestamp& timestamp = Timestamp(false));

		/**
		 * Creates a new one-plane frame with known frame type with read-only source memory.
		 * Beware: If this frame uses the pixel data only, the provided buffer must be valid as long as this new frame exists!
		 * @param frameType Type of the frame, must be valid
		 * @param data Frame data to copy or to use, depending on the data copy flag
		 * @param copyMode The copy mode to be applied
		 * @param paddingElements Optional number of elements at the end of each row, one pixel has (1 * channels) elements, must be 0 for non-generic pixel formats (e.g., Y_UV12), with range [0, infinity)
		 * @param timestamp The timestamp of the frame
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'uint16_t', or 'float', can be `void` to force the usage of the pixel element type as defined in `frameType.pixelFormat()`
		 */
		template <typename T>
		inline Frame(const FrameType& frameType, const T* data, const CopyMode copyMode, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false));

		/**
		 * Creates a new one-plane frame with known frame type with writable source memory.
		 * Beware: If this frame uses the pixel data only, the provided buffer must be valid as long as this new frame exists!
		 * @param frameType Type of the frame
		 * @param data Frame data to copy or to use, depending on the data copy flag
		 * @param copyMode The copy mode to be applied
		 * @param paddingElements Optional number of elements at the end of each row, one pixel has (1 * channels) elements, must be 0 for non-generic pixel formats (e.g., Y_UV12), with range [0, infinity)
		 * @param timestamp The timestamp of the frame
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'uint16_t', or 'float', can be `void` to force the usage of the pixel element type as defined in `frameType.pixelFormat()`
		 */
		template <typename T>
		inline Frame(const FrameType& frameType, T* data, const CopyMode copyMode, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false));

		/**
		 * Creates a new multi-plane frame with known frame type and given source memory for each individual plane.
		 * @param frameType The data type of the new frame, must be valid
		 * @param planeInitializers The initializers for the individual planes, one for each plane of the pixel format
		 * @param timestamp The timestamp of the frame
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'uint16_t', or 'float', can be `void` to force the usage of the pixel element type as defined in `frameType.pixelFormat()`
		 */
		template <typename T>
		inline Frame(const FrameType& frameType, const PlaneInitializers<T>& planeInitializers, const Timestamp& timestamp = Timestamp(false));

		/**
		 * Destructs a frame.
		 */
		~Frame();

		/**
		 * Returns the frame type of this frame.
		 * This return value is actually the cast of the base class of this frame.
		 * @return Frame type
		 */
		inline const FrameType& frameType() const;

		/**
		 * Returns the individual planes of this frame.
		 * @return The frame's plane
		 */
		inline const Planes& planes() const;

		/**
		 * Deprecated.
		 *
		 * Copies frame data from a source frame.
		 * When both frame types are not identical, the frame type of this frame is changed to the source frame type.<br>
		 * This frame will own the new frame data, thus a new frame buffer is allocated if necessary.<br>
		 * In case a new frame buffer needed to be allocated, the memory layout of the new frame buffer will be continuous.
		 * @param source The source frame to copy, must be valid and must not be this frame
		 * @param copyTimestamp True, to copy the frame's timestamp; False, to copy the image data only
		 * @return True, if the copy operation was successful; otherwise, the frame is not modified and false is returned.
		 */
		bool copy(const Frame& source, const bool copyTimestamp = true);

		/**
		 * Copies the entire image content of a source frame into this frame.
		 * Both frames must have a compatible pixel format and must have the same pixel origin.<br>
		 * Only the intersecting image content will be copied, padding data is not copied:
		 * <pre>
		 *                   Source frame
		 *                    -------------------------------
		 * This              |(targetLeft, targetTop)        |
		 * target frame      |                               |
		 *  -----------------|---------                      |
		 * |(0, 0)           |XXXXXXXXX|                     |
		 * |                 |XXXXXXXXX|                     |
		 * |                  -------------------------------
		 * |                           |
		 *  ---------------------------
		 * </pre>
		 * The intersecting image content is marked with an 'X'.
		 * @param targetLeft The horizontal position within this image to which the top-left corner of the source image will be copied, with range (-infinity, infinity)
		 * @param targetTop The vertical position within this image to which the top-left corner of the source image will be copied, with range (-infinity, infinity)
		 * @param source The source frame to be copied, must be valid
		 * @param copyTimestamp True, to copy the frame's timestamp; False, to copy the image data only
		 * @return False, if both frames are not compatible; True, if the input was valid, even if both images do not intersect
		 */
		bool copy(const int targetLeft, const int targetTop, const Frame& source, const bool copyTimestamp = true);

		/**
		 * Sets a new frame type for this frame.
		 * The frame data will be reallocated (re-initialized) if the specified frame types, or one of the property flags (forceOwner, forceWritable) do not fit with the current frame.
		 * @param frameType New frame type to set, can be invalid
		 * @param forceOwner If specified and the frame is not yet owner, then the frame will allocate its own frame buffer
		 * @param forceWritable If specified and the frame is read-only, then the frame will allocate its own frame buffer
		 * @param planePaddingElements The padding elements at the end of each individual plane row, in elements, one for each plane, an empty vector to use the existing padding layout or no padding if reallocation
		 * @param timestamp The timestamp to be set
		 * @param reallocated Optional resulting state whether the frame has been reallocated; nullptr otherwise
		 * @return True, if succeeded
		 */
		bool set(const FrameType& frameType, const bool forceOwner, const bool forceWritable = false, const Indices32& planePaddingElements = Indices32(), const Timestamp& timestamp = Timestamp(false), bool* reallocated = nullptr);

		/**
		 * Updates the memory pointer for a specific plane of the frame to a new read-only memory location.
 		 * This function should only be used if the specified plane does not own its memory to ensure that the frame's ownership behavior remains consistent.
		 * @param data The new read-only memory pointer to be set, must be valid
		 * @param planeIndex The index of the frame's plane for which the memory will be updated, with range [0, numberPlanes())
		 * @return True, if succeeded; False, if e.g., the plane to update owned the memory
		 * @see isPlaneOwner().
		 */
		template <typename T>
		bool updateMemory(const T* data, const unsigned int planeIndex = 0u);

		/**
		 * Updates the memory pointer for a specific plane of the frame to a new read-only or writable memory location.
		 * This function should only be used if the specified plane currently does not own its memory to ensure that the frame's ownership behavior remains consistent.
		 * For read-only memory, provide a const memory pointer; For writable memory, provide a non-const pointer.
		 * @param data The new writable memory pointer to be set, must be valid
		 * @param planeIndex The index of the frame's plane for which the memory will be updated, with range [0, numberPlanes())
		 * @return True, if succeeded; False, if e.g., the plane to update owned the memory
		 * @see isPlaneOwner().
		 */
		template <typename T>
		bool updateMemory(T* data, const unsigned int planeIndex = 0u);

		/**
		 * Updates the memory pointers for all or some of the planes of the frame to new writable memory locations.
 		 * This function should be used only when the planes do not own their memory, to maintain consistent ownership behavior across the frame.
		 * @param planeDatas The new writable memory pointers to be set, the number of pointers provided should be at least one and at most equal to numberPlanes().
		 * @return True, if succeeded; False, if e.g., the plane to update owned the memory
		 * @see isPlaneOwner().
		 */
		template <typename T>
		bool updateMemory(const std::initializer_list<T*>& planeDatas);

		/**
		 * Makes the memory of this frame continuous.
		 * If the memory is already continuous, nothing happens.<br>
		 * If the memory is not continuous, a new continuous memory block will be allocated and the memory is copied into the new memory block (for each plane individually), the frame will be owner of the memory.
		 */
		void makeContinuous();

		/**
		 * Makes this frame the owner of the memory.
		 * In case this frame does not own the memory, new memory will be allocated.
		 */
		void makeOwner();

		/**
		 * Returns a sub-frame of this frame.
		 * The copy mode defines whether the resulting sub-frame owns the memory or uses the memory.
		 * @param subFrameLeft Left start location of the resulting sub-frame, in pixels, defined within this frame, with range [0, width - 1], must be 0 if the pixel format is packed
		 * @param subFrameTop Top start location of the resulting sub-frame, in pixels, defined within this frame, with range [0, height - 1]
		 * @param subFrameWidth Width of the resulting sub-frame in pixels, with range [1, width() - subFrameLeft]
		 * @param subFrameHeight Height of the resulting sub-frame in pixels, with range [1, height() - subFrameTop]
		 * @param copyMode The copy mode to be applied, must not be CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA
		 * @return The requested sub-frame not owning the image data, an invalid frame if the defined sub-region does not fit into this frame
		 * @see FrameType::formatIsPacked().
		 */
		Frame subFrame(const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const CopyMode copyMode = CM_USE_KEEP_LAYOUT) const;

		/**
		 * Sets the memory of the frame to a specified byte value (the memory of one plane).
		 * Each byte of the frame's memory will be set to the same value.
		 *
		 * The following code snippet shows how this function may be used:
		 * @code
		 * Frame rgbFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 * rgbFrame.setValue(0x00u);
		 * @endcode
		 * @param value The 8 bit value to be set to each byte of the frame, with range [0, 255]
		 * @param planeIndex The index of the plane for which the memory will be set, with range [0, numberPlanes())
		 * @param skipPaddingData True, to do not set the memory value of the padding data; False, to write the memory value of the padding data as well
		 * @return True, if the image data was writable; False, if the image holds read-only memory
		 * @see isReadOnly().
		 */
		bool setValue(const uint8_t value, const unsigned int planeIndex = 0u, const bool skipPaddingData = true);

		/**
		 * Sets the memory of the frame to a specified pixel value (the memory of one plane).
		 * Each pixel will be set to the same values (each channel will be set to an own value).
		 *
		 * The following code snippet shows how this function may be used:
		 * @code
		 * Frame rgbFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 * const Frame::PixelType<uint8_t, 3u> yellow({0xFFu, 0xFFu, 0x00u});
		 * rgbFrame.setValue<uint8_t, 3u>(yellow);
		 *
		 * Frame tensorFrame(FrameType(1920u, 1080u, FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_UPPER_LEFT));
		 * const Frame::PixelType<float, 3u> value({0.0f, 1.0f, 2.0f});
		 * tensorFrame.setValue<float, 3u>(value);
		 * @endcode
		 * @param planePixelValue The pixel value to be set to each pixel
		 * @param planeIndex The index of the plane for which the memory will be set, with range [0, numberPlanes())
		 * @tparam T The data type of the given pixel value, must be identical to the data type of the frame
		 * @tparam tPlaneChannels The number of channels the plane has (not the number of channels the frame has), with range [1, channels()]
		 * @return True, if the image data was writable; False, if the image holds read-only memory
		 */
		template <typename T, const unsigned int tPlaneChannels>
		bool setValue(const PixelType<T, tPlaneChannels>& planePixelValue, const unsigned int planeIndex = 0u);

		/**
		 * Sets the memory of the frame to a specified pixel value (the memory of one plane).
		 * Each pixel will be set to the same values (each channel will be set to an own value).
		 *
		 * The following code snippet shows how this function may be used:
		 * @code
		 * Frame rgbFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 * rgbFrame.setValue<uint8_t>(CV::Canvas::yellow(), 3u);
		 * @endcode
		 * @param planePixelValue The pixel value to be set to each pixel, one value for each plane channel, must be valid
		 * @param planePixelValueSize The number of provided pixel values, with range [1, 4], must be `planes().channels()`
		 * @param planeIndex The index of the plane for which the memory will be set, with range [0, numberPlanes())
		 * @tparam T The data type of the given pixel value, must be identical to the data type of the frame
		 * @return True, if the image data was writable; False, if the image holds read-only memory
		 */
		template <typename T>
		bool setValue(const T* planePixelValue, const size_t planePixelValueSize, const unsigned int planeIndex = 0u);

		/**
		 * Sets the memory of the frame to a specified pixel value (the memory of one plane).
		 * Each pixel will be set to the same values (each channel will be set to an own value).
		 *
		 * The following code snippet shows how this function may be used:
		 * @code
		 * Frame rgbFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 * rgbFrame.setValue<uint8_t>({0xFFu, 0xFFu, 0x00u});
		 *
		 * Frame tensorFrame(FrameType(1920u, 1080u, FrameType::genericPixelFormat<float, 3u>(), FrameType::ORIGIN_UPPER_LEFT));
		 * tensorFrame.setValue<float>({0.0f, 1.0f, 2.0f});
		 * @endcode
		 * @param planePixelValues The pixel values to be set to each pixel, one value for each plane channel
		 * @param planeIndex The index of the plane for which the memory will be set, with range [0, numberPlanes())
		 * @tparam T The data type of the given pixel value, must be identical to the data type of the frame
		 * @return True, if the image data was writable; False, if the image holds read-only memory
		 */
		template <typename T>
		bool setValue(const std::initializer_list<T>& planePixelValues, const unsigned int planeIndex = 0u);

		/**
		 * Returns whether the frame (one plane) contains a specified pixel value.
		 * @param planePixelValue The pixel value to be checked
		 * @param planeIndex The index of the plane for which the memory will be set, with range [0, numberPlanes())
		 * @tparam T The data type of the given pixel value, must be identical to the data type of the frame
		 * @tparam tPlaneChannels The number of channels the plane has (not the number of channels the frame has), with range [1, channels()]
		 * @return True, if at least one pixel has the specified value
		 */
		template <typename T, const unsigned int tPlaneChannels>
		bool containsValue(const PixelType<T, tPlaneChannels>& planePixelValue, const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the number of bytes necessary for a specific plane including optional padding at the end of plane rows.
		 * @param planeIndex The index of the plane for which the check is done, with range [0, planes().size())
		 * @return Frame buffer size in bytes, with range [0, infinity)
		 */
		inline unsigned int size(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the optional number of padding elements at the end of each row for a specific plane.
		 * @param planeIndex The index of the plane for which the number of padding elements will be returned, with range [0, planes().size())
		 * @return The frame's number of padding elements, in elements, with range [0, infinity)
		 */
		inline unsigned int paddingElements(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the optional number of padding bytes at the end of each row for a specific plane.
		 * @param planeIndex The index of the plane for which the number of padding bytes will be returned, with range [0, planes().size())
		 * @return The frame's number of padding bytes, in bytes, with range [0, infinity)
		 */
		inline unsigned int paddingBytes(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the number of elements within one row, including optional padding at the end of a row for a specific plane.
		 * The number of elements per row is be determined by the plane's values: pixels * elementsPerPixel + paddingElements().
		 * @param planeIndex The index of the plane for which the number of stride elements will be returned, with range [0, planes().size())
		 * @return The frame's stride defined in elements, with range [width * elementsPerPixel, infinity)
		 */
		inline unsigned int strideElements(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the number of bytes within one row, including optional padding at the end of a row for a specific plane.
		 * @param planeIndex The index of the plane for which the number of stride bytes will be returned, with range [0, planes().size())
		 * @return The frame's stride defined in bytes, with range [pixels * elementsPerPixel * bitsPerDatatype() / 8, infinity)
		 */
		inline unsigned int strideBytes(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the width of a plane of this frame.
		 * @param planeIndex The index of the plane for which the width will be returned, with range [0, planes().size())
		 * @return The plane's width, in pixel, with range [0, infinity)
		 */
		inline unsigned int planeWidth(const unsigned int planeIndex) const;

		/**
		 * Returns the height of a plane of this frame.
		 * @param planeIndex The index of the plane for which the height will be returned, with range [0, planes().size())
		 * @return The plane's height, in pixel, with range [0, infinity)
		 */
		inline unsigned int planeHeight(const unsigned int planeIndex) const;

		/**
		 * Returns the channels of a plane of this frame.
		 * @param planeIndex The index of the plane for which the channels will be returned, with range [0, planes().size())
		 * @return The plane's channels, with range [0, infinity)
		 */
		inline unsigned int planeChannels(const unsigned int planeIndex) const;

		/**
		 * Returns the width of a plane of this frame, not in pixel, but in elements, not including padding at the end of each plane row.
		 * @param planeIndex The index of the plane for which the width will be returned, with range [0, planes().size())
		 * @return The plane's width, in elements, with is `planeWidth(planeIndex) * planeChannels(planeIndex)`, with range [0, infinity)
		 */
		inline unsigned int planeWidthElements(const unsigned int planeIndex) const;

		/**
		 * Returns the width of a plane of this frame, not in pixel, but in bytes, not including padding at the end of each plane row.
		 * @param planeIndex The index of the plane for which the width will be returned, with range [0, planes().size())
		 * @return The plane's width, in bytes, with range [0, infinity)
		 */
		inline unsigned int planeWidthBytes(const unsigned int planeIndex) const;

		/**
		 * Returns the number of bytes of one pixel of a plane for a pixel format.
		 * Beware: This function will return 0 if the pixel format is a special packed format (e.g., FORMAT_Y10_PACKED) which does not allow to calculate the number of bytes per pixel.
		 * @param planeIndex The index of the plane for which the bytes per pixel will be returned, with range [0, numberPlanes(imagePixelFormat))
		 * @return The plane's number of bytes per pixel, will be 0 for special packed pixel formats like FORMAT_Y10_PACKED
		 */
		inline unsigned int planeBytesPerPixel(const unsigned int planeIndex) const;

		/**
		 * Returns whether a specific plane of this frame is based on continuous memory and thus does not have any padding at the end of rows.
		 * @param planeIndex The index of the plane for which the check is done, with range [0, planes().size())
		 * @return True, if so
		 */
		inline bool isPlaneContinuous(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns whether a specific plane of this frame is the owner of the memory.
		 * @param planeIndex The index of the plane for which the check is done, with range [0, planes().size())
		 * @return True, if so
		 */
		inline bool isPlaneOwner(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the timestamp of this frame.
		 * @return Timestamp
		 */
		inline const Timestamp& timestamp() const;

		/**
		 * Returns the relative timestamp of this frame.
		 * @return Timestamp
		 */
		inline const Timestamp& relativeTimestamp() const;

		/**
		 * Sets the timestamp of this frame.
		 * @param timestamp Timestamp to be set
		 * @see setRelativeTimestamp().
		 */
		inline void setTimestamp(const Timestamp& timestamp);

		/**
		 * Sets the relative timestamp of this frame.
		 * In contrast to the standard timestamp of this frame, the relative timestamp provides the frame time in relation to a reference time.<br>
		 * @param relative The relative timestamp to be set
		 * @see setTimestamp().
		 */
		inline void setRelativeTimestamp(const Timestamp& relative);

		/**
		 * Releases this frame and the frame data if this frame is the owner.
		 */
		void release();

		/**
		 * Returns a pointer to the pixel data of a specific plane.
		 * Ensure that the frame holds writable pixel data before calling this function.
		 * @param planeIndex The index of the plane for which the data will be returned, with range [0, planes().size())
		 * @return The plane's writable pixel data
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see isValid(), isReadOnly().
		 */
		template <typename T>
		inline T* data(const unsigned int planeIndex = 0u);

		/**
		 * Returns a pointer to the read-only pixel data of a specific plane.
		 * @param planeIndex The index of the plane for which the data will be returned, with range [0, planes().size())
		 * @return The plane's read-only pixel data
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see isValid().
		 */
		template <typename T>
		inline const T* constdata(const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the pointer to the pixel data of a specific row.
		 * Ensure that the frame is valid and that the frame holds a valid frame buffer before this function is called.
		 *
		 * The index of the row is defined with respect to the origin of the frame's data.<br>
		 * Therefore, row<T>(0) will return the top row of an image if pixelOrigin() == ORIGIN_UPPER_LEFT,<br>
		 * and will return the bottom row of an image if pixelOrigin() == ORIGIN_LOWER_LEFT.<br>
		 * In any case, row<T>(0) is equivalent to data<T>().
		 *
		 * @param y The index of the row (the vertical location) to which the resulting pointer will point, with range [0, planeHeight(planeIndex) - 1]
		 * @param planeIndex The index of the plane for which the pixel will be returned, with range [0, planes().size())
		 * @return The pointer to the memory at which the row starts
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see data(), pixel(), constrow(), constdata(), constpixel().
		 */
		template <typename T>
		inline T* row(const unsigned int y, const unsigned int planeIndex = 0u);

		/**
		 * Returns the pointer to the constant data of a specific row.
		 * Ensure that the frame is valid and that the frame holds a valid frame buffer before this function is called.
		 *
		 * The index of the row is defined with respect to the origin of the frame's data.<br>
		 * Therefore, constrow<T>(0) will return the top row of an image if pixelOrigin() == ORIGIN_UPPER_LEFT,<br>
		 * and will return the bottom row of an image if pixelOrigin() == ORIGIN_LOWER_LEFT.<br>
		 * In any case, constrow<T>(0) is equivalent to constdata<T>().
		 *
		 * @param y The index of the row (the vertical location) to which the resulting pointer will point, with range [0, planeHeight(planeIndex) - 1]
		 * @param planeIndex The index of the plane for which the pixel will be returned, with range [0, planes().size())
		 * @return The pointer to the memory at which the row starts
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see data(), pixel(), constrow(), constdata(), constpixel().
		 */
		template <typename T>
		inline const T* constrow(const unsigned int y, const unsigned int planeIndex = 0u) const;

		/**
		 * Returns the pointer to the data of a specific pixel.
		 * Ensure that the frame is valid and that the frame holds a valid frame buffer before this function is called.
		 *
		 * In general, the usage of this function is recommended for prototyping only.<br>
		 * As the location of each pixel has to be calculated every time, this function is quite slow.<br>
		 * Production code should use the constdata(), data(), constrow(), and row() functions instead.
		 *
		 * The vertical location (the y coordinate) of the pixel is defined with respect to the origin of the frame's data.<br>
		 * Therefore, pixel(0, 0) will return the top left pixel of an image if pixelOrigin() == ORIGIN_UPPER_LEFT,<br>
		 * and will return the bottom left pixel of an image if pixelOrigin() == ORIGIN_LOWER_LEFT.<br>
		 * In any case, pixel(0, 0) is equivalent to data().
		 *
		 * This function most not be called for packed pixel formats.
		 *
		 * @param x The horizontal position of the requested pixel, with range [0, planeWidth(planeIndex) - 1]
		 * @param y The vertical position of the requested pixel, with range [0, planeHeight(planeIndex) - 1]
		 * @param planeIndex The index of the plane for which the pixel will be returned, with range [0, planes().size())
		 * @return The pointer to the memory at which the pixel starts
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see data(), row(), constpixel(), constdata(), constrow(). formatIsPacked().
		 *
		 *
		 * The following code snippet shows how this function may be used:
		 * @code
		 * // we create a RGB image with 24 bit per pixel (8 bit per channel)
		 * Frame rgbImage(FrameType(1280u, 720u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		 *
		 * const uint8_t redChannelValue = 0xFF; // 255
		 * const uint8_t greenChannelValue = 0x80; // 128
		 * const uint8_t blueChannelValue = 0x00; // 0
		 *
		 * // we iterate overall every pixel
		 * for (unsigned int y = 0u; y < rgbImage.height(); ++y)
		 * {
		 *     for (unsigned int x = 0u; x < rgbImage.width(); ++x)
		 *     {
		 *         // we store the pointer to the pixel
		 *         uint8_t* rgbPixel = rgbImage.pixel<uint8_t>(x, y);
		 *
		 *         // we set the color value of each red channel and green channel
		 *         rgbPixel[0] = redChannelValue;
		 *         rgbPixel[1] = greenChannelValue;
		 *
		 *         // we also can set the value of each channel directly
		 *         rgbImage.pixel<uint8_t>(x, y)[2] = blueChannelValue;
		 *     }
		 * }
		 *
		 * // now we have set the color of every pixel of the image
		 * @endcode
		 */
		template <typename T>
		inline T* pixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u);

		/**
		 * Returns the pointer to the constant data of a specific pixel.
		 * Ensure that the frame is valid and that the frame holds a valid frame buffer before this function is called.
		 *
		 * In general, the usage of this function is recommended for prototyping only.<br>
		 * As the location of each pixel has to be calculated every time, this function is quite slow.<br>
		 * Production code should use the constdata(), data(), constrow(), and row() functions instead.
		 *
		 * The vertical location (the y coordinate) of the pixel is defined with respect to the origin of the frame's data.<br>
		 * Therefore, pixel<T>(0, 0) will return the top left pixel of an image if pixelOrigin() == ORIGIN_UPPER_LEFT,<br>
		 * and will return the bottom left pixel of an image if pixelOrigin() == ORIGIN_LOWER_LEFT.<br>
		 * In any case, pixel<T>(0, 0) is equivalent to data<T>().
		 *
		 * This function most not be called for packed pixel formats.
		 *
		 * @param x The horizontal position of the requested pixel, with range [0, planeWidth(planeIndex) - 1]
		 * @param y The vertical position of the requested pixel, with range [0, planeHeight(planeIndex) - 1]
		 * @param planeIndex The index of the plane for which the pixel will be returned, with range [0, planes().size())
		 * @return The pointer to the memory at which the pixel starts
		 * @tparam T The explicit data type of the value of each pixel channel
		 * @see data(), row(), pixel(), constdata(), constrow(), formatIsPacked().
		 */
		template <typename T>
		inline const T* constpixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u) const;

		/**
		 * Returns whether all planes of this frame have continuous memory and thus do not contain any padding at the end of their rows.
		 * @return True, if so
		 */
		inline bool isContinuous() const;

		/**
		 * Returns whether the frame is the owner of the internal frame data.
		 * Otherwise the frame data is stored by e.g., a 3rd party and this frame holds a reference only.<br>
		 * The frame is not owner of the memory if at least one plane is not owner of the memory.
		 * @return True, if so
		 * @see Plane::isOwner().
		 */
		inline bool isOwner() const;

		/**
		 * Returns true, if the frame allows only read access (using constdata()). Otherwise, data() may be used to modify the frame data.
		 * Beware: Call this method only if the frame is valid.
		 * The frame is read-only if at least one plane is read-only.
		 * @return True, if the frame object allows read access only
		 * @see data(), constdata(), Plane::isReadOnly().
		 */
		inline bool isReadOnly() const;

		/**
		 * Returns whether the frame's pixel format contains an alpha channel.
		 * @return True, if so
		 */
		inline bool hasAlphaChannel() const;

		/**
		 * Returns whether the frame holds at least one pixel with an non opaque alpha value.
		 * The pixel format must be composed of one plane only.
		 * @return True, if so
		 * @tparam T The type of the frame's data type, either `uint8_t`, or `uint16_t`
		 */
		template <typename T>
		bool hasTransparentPixel(const T opaque) const;

		/**
		 * Returns whether this frame is valid.
		 * This function is mainly callying `FrameType::isValid()`, while in debug builds, addtional checks are performed.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two frame objects have any amount of intersecting memory.
		 * This frame and the given frame must both be valid.<br>
		 * Use this function to ensure that e.g., a source buffer and target buffer is completely independent.<br>
		 * This functions also considers memory intersections in the padding area as regular intersection.
		 * @param frame The second frame of which its memory will be compared to the memory of this frame, must be valid
		 * @return True, if so
		 */
		bool haveIntersectingMemory(const Frame& frame) const;

		/**
		 * Returns whether this frame object is valid and holds a frame.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Assign operator.
		 * Releases the current frame (and frees the memory if the frame is the owner) and creates a second version of a given frame.
		 * If the given source frame is not owner of the frame data, this frame will also not be owner of the frame data.<br>
		 * But, if the given source frame is the owner of the frame data, this frame will also be the owner of new copy of the frame data.<br>
		 * If the memory is actually copied, the memory layout of this new frame will be continuous.
		 * This function behaves similar like the normal copy constructor.
		 * @param right The right frame to assign
		 * @return Reference to this frame
		 */
		Frame& operator=(const Frame& right) noexcept;

		/**
		 * Move operator.
		 * @param right The right frame to moved
		 * @return Reference to this frame
		 */
		Frame& operator=(Frame&& right) noexcept;

		/**
		 * Determines the number of padding elements at the end of a row of a plane for which the pixel format, the image width and the plane's stride (in bytes) are known.
		 * @param pixelFormat The pixel format of the image, must be valid
		 * @param imageWidth The width of the image in pixels, with range [0, infinity)
		 * @param planeStrideBytes The number of bytes between to start points of successive rows (the stride of the row in bytes) for the specified image plane, with range [planeWidthBytes(planeIndex), infinity)
		 * @param planePaddingElements The resulting number of padding elements at the end of each row (at the actual end of the row's pixel data) for the specified image plane, in elements (not bytes), with range [0, infinity)
		 * @param planeIndex The index of the image plane for which the number of padding elements will be calculated, with range [0, numberPlanes() - 1]
		 * @return True, if succeeded; False, if the given plane configuration is invalid
		 */
		static bool strideBytes2paddingElements(const PixelFormat& pixelFormat, const unsigned int imageWidth, const unsigned int planeStrideBytes, unsigned int& planePaddingElements, const unsigned int planeIndex = 0u);

	protected:

		/**
		 * Creates a new multi-plane frame with known frame type and given source memory for each individual plane.
		 * @param frameType The data type of the new frame, must be valid
		 * @param planeInitializers The initializers for the individual planes, one for each plane of the pixel format, must be valid
		 * @param sizePlaneInitializers The number of specified initializers for the individual planes, must be frameType.numberPlanes()
		 * @param timestamp The timestamp of the frame
		 */
		Frame(const FrameType& frameType, const PlaneInitializer<void>* planeInitializers, size_t sizePlaneInitializers, const Timestamp& timestamp = Timestamp(false));

		/**
		 * Deleted constructor to prevent misuse.
		 * @param frame The frame to copy
		 * @param copyData Determines whether this new frame will make an own copy of the given frame data or whether the pixel data is used only
		 */
		Frame(const Frame& frame, const bool copyData) = delete;

		/**
		 * Deleted constructor to prevent misuse.
		 * @param frameType The frame type which would be used to create the object
		 * @param copyMode The copy mode which would be used to create the object
		 */
		Frame(const FrameType& frameType, const CopyMode copyMode) = delete;

		/**
		 * Deleted constructor to prevent misuse.
		 * @param frameType The frame type which would be used to create the object
		 * @param avancedCopyMode The advanced copy mode which would be used to create the object
		 */
		Frame(const FrameType& frameType, const AdvancedCopyMode avancedCopyMode) = delete;

		/**
		 * Deleted constructor to prevent misuse, use `AdvancedCopyMode` instead.
		 * @param frame The frame to be copied
		 * @param copyMode The copy mode which would be used to create the object
		 */
		Frame(const Frame& frame, const CopyMode copyMode) = delete;

		/**
		 * Deleted constructor to prevent misuse.
		 * @param frameType The frame type which would be used
		 * @param timestamp The timestamp which would be used to create the object
		 */
		Frame(const FrameType& frameType, const Timestamp& timestamp) = delete;

		/**
		 * Deleted constructor to prevent misuse.
		 * @param frame The frame to be copied
		 * @param timestamp The timestamp which would be used to create the object
		 */
		Frame(const Frame& frame, const Timestamp& timestamp) = delete;

		/**
		 * Deleted constructor to prevent misuse, use `Frame(const FrameType& frameType, const T* data, const CopyMode copyMode, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false));` instead.
		 * @param frameType Type of the frame, must be valid
		 * @param data Frame data to copy or to use, depending on the data copy flag
		 * @param copyData Determines whether the frame will make an own copy of the given frame data or whether the pixel data are used only
		 * @param paddingElements Optional number of elements at the end of each row, one pixel has (1 * channels) elements, must be 0 for non-generic pixel formats (e.g., Y_UV12), with range [0, infinity)
		 * @param timestamp The timestamp of the frame
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'uint16_t', or 'float', can be `void` to force the usage of the pixel element type as defined in `frameType.pixelFormat()`
		 */
		template <typename T>
		Frame(const FrameType& frameType, const T* data, const bool copyData, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false)) = delete;

		/**
		 * Deleted constructor to prevent misuse, use `Frame(const FrameType& frameType, T* data, const CopyMode copyMode, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false));` instead.
		 * @param frameType Type of the frame
		 * @param data Frame data to copy or to use, depending on the data copy flag
		 * @param copyData Determines whether the frame will make an own copy of the given frame data or whether the pixel data are used only
		 * @param paddingElements Optional number of elements at the end of each row, one pixel has (1 * channels) elements, must be 0 for non-generic pixel formats (e.g., Y_UV12), with range [0, infinity)
		 * @param timestamp The timestamp of the frame
		 * @tparam T The data type of each pixel element, e.g., 'uint8_t', 'uint16_t', or 'float', can be `void` to force the usage of the pixel element type as defined in `frameType.pixelFormat()`
		 */
		template <typename T>
		Frame(const FrameType& frameType, T* data, const bool copyData, const unsigned int paddingElements = 0u, const Timestamp& timestamp = Timestamp(false)) = delete;

	protected:

		/// The individual memory planes of this frame.
		Planes planes_;

		/// Timestamp of the frame.
		Timestamp timestamp_;

		/// Relative timestamp of this frame.
		Timestamp relativeTimestamp_;
};

inline FrameType::PixelFormatUnion::PixelFormatUnion(const PixelFormat& pixelFormat) :
	pixelFormat_(pixelFormat)
{
	// nothing to do here
}

inline FrameType::FrameType(const unsigned int width, const unsigned int height, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin) :
	width_(width),
	height_(height),
	pixelFormat_(pixelFormat),
	pixelOrigin_(pixelOrigin)
{
	if (isValid())
	{
		if (width_ % widthMultiple(pixelFormat_.pixelFormat_) != 0u || height_ % heightMultiple(pixelFormat_.pixelFormat_) != 0u)
		{
			ocean_assert(false && "The configuration of this frame type is invalid - this should never happen!");

			width_ = 0u;
			height_ = 0u;
			pixelFormat_ = PixelFormatUnion(FORMAT_UNDEFINED);
			pixelOrigin_ = ORIGIN_INVALID;

			ocean_assert(!isValid());
		}
	}
}

inline FrameType::FrameType(const FrameType& type, const unsigned int width, const unsigned int height) :
	width_(width),
	height_(height),
	pixelFormat_(type.pixelFormat_),
	pixelOrigin_(type.pixelOrigin_)
{
	if (isValid())
	{
		if (width_ % widthMultiple(pixelFormat_.pixelFormat_) != 0u || height_ % heightMultiple(pixelFormat_.pixelFormat_) != 0u)
		{
			ocean_assert(false && "The configuration of this frame type is invalid - this should never happen!");

			width_ = 0u;
			height_ = 0u;
			pixelFormat_ = PixelFormatUnion(FORMAT_UNDEFINED);
			pixelOrigin_ = ORIGIN_INVALID;

			ocean_assert(!isValid());
		}
	}
}

inline FrameType::FrameType(const FrameType& type, const PixelFormat pixelFormat) :
	width_(type.width_),
	height_(type.height_),
	pixelFormat_(pixelFormat),
	pixelOrigin_(type.pixelOrigin_)
{
	// nothing to do here
}

inline FrameType::FrameType(const FrameType& type, const PixelOrigin pixelOrigin) :
	width_(type.width_),
	height_(type.height_),
	pixelFormat_(type.pixelFormat_),
	pixelOrigin_(pixelOrigin)
{
	// nothing to do here
}

inline FrameType::FrameType(const FrameType& type, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin) :
	width_(type.width_),
	height_(type.height_),
	pixelFormat_(pixelFormat),
	pixelOrigin_(pixelOrigin)
{
	// nothing to do here
}

inline unsigned int FrameType::width() const
{
	return width_;
}

inline unsigned int FrameType::height() const
{
	return height_;
}

inline FrameType::PixelFormat FrameType::pixelFormat() const
{
	return pixelFormat_.pixelFormat_;
}

inline void FrameType::setPixelFormat(const PixelFormat pixelFormat)
{
	pixelFormat_.pixelFormat_ = pixelFormat;
}

inline FrameType::DataType FrameType::dataType() const
{
	return dataType(pixelFormat_.pixelFormat_);
}

inline unsigned int FrameType::bytesPerDataType() const
{
	return bytesPerDataType(dataType());
}

inline unsigned int FrameType::channels() const
{
	if (pixelFormat_.pixelFormat_ == FORMAT_UNDEFINED)
	{
		return 0u;
	}

	return channels(pixelFormat_.pixelFormat_);
}

inline uint32_t FrameType::numberPlanes() const
{
	return numberPlanes(pixelFormat_.pixelFormat_);
}

inline FrameType::PixelOrigin FrameType::pixelOrigin() const
{
	return pixelOrigin_;
}

inline unsigned int FrameType::pixels() const
{
	return width_ * height_;
}

inline bool FrameType::isPixelFormatCompatible(const PixelFormat pixelFormat) const
{
	return arePixelFormatsCompatible(this->pixelFormat(), pixelFormat);
}

inline bool FrameType::isFrameTypeCompatible(const FrameType& frameType, const bool allowDifferentPixelOrigins) const
{
	return areFrameTypesCompatible(*this, frameType, allowDifferentPixelOrigins);
}

inline bool FrameType::operator!=(const FrameType& right) const
{
	return !(*this == right);
}

inline bool FrameType::isValid() const
{
	return width_ != 0u && height_ != 0u && pixelFormat_.pixelFormat_ != FORMAT_UNDEFINED && pixelOrigin_ != ORIGIN_INVALID;
}

inline uint32_t FrameType::numberPlanes(const PixelFormat pixelFormat)
{
	return uint32_t((pixelFormat >> pixelFormatBitOffsetPlanes) & 0xFFull);
}

inline uint32_t FrameType::formatGenericNumberChannels(const PixelFormat pixelFormat)
{
	return uint32_t((pixelFormat >> pixelFormatBitOffsetChannels) & 0xFFull);
}

template <>
constexpr FrameType::DataType FrameType::dataType<char>()
{
	static_assert(sizeof(char) == 1, "Invalid data type!");

	return (std::is_signed<char>::value) ? DT_SIGNED_INTEGER_8 : DT_UNSIGNED_INTEGER_8;
}

template <>
constexpr FrameType::DataType FrameType::dataType<signed char>()
{
	static_assert(sizeof(signed char) == 1, "Invalid data type!");
	return DT_SIGNED_INTEGER_8;
}

template <>
constexpr FrameType::DataType FrameType::dataType<unsigned char>()
{
	static_assert(sizeof(unsigned char) == 1, "Invalid data type!");
	return DT_UNSIGNED_INTEGER_8;
}

template <>
constexpr FrameType::DataType FrameType::dataType<unsigned short>()
{
	static_assert(sizeof(unsigned short) == 2, "Invalid data type!");
	return DT_UNSIGNED_INTEGER_16;
}

template <>
constexpr FrameType::DataType FrameType::dataType<short>()
{
	static_assert(sizeof(short) == 2, "Invalid data type!");
	return DT_SIGNED_INTEGER_16;
}

template <>
constexpr FrameType::DataType FrameType::dataType<unsigned int>()
{
	static_assert(sizeof(unsigned int) == 4, "Invalid data type!");
	return DT_UNSIGNED_INTEGER_32;
}

template <>
constexpr FrameType::DataType FrameType::dataType<int>()
{
	static_assert(sizeof(int) == 4, "Invalid data type!");
	return DT_SIGNED_INTEGER_32;
}

template <>
constexpr FrameType::DataType FrameType::dataType<unsigned long>()
{
	static_assert(sizeof(unsigned long) == 4 || sizeof(unsigned long) == 8, "Invalid data type!");

	return (sizeof(unsigned long) == 4) ? DT_UNSIGNED_INTEGER_32 : DT_UNSIGNED_INTEGER_64;
}

template <>
constexpr FrameType::DataType FrameType::dataType<long>()
{
	static_assert(sizeof(unsigned long) == 4 || sizeof(unsigned long) == 8, "Invalid data type!");

	return (sizeof(long) == 4) ? DT_SIGNED_INTEGER_32 : DT_SIGNED_INTEGER_64;
}

template <>
constexpr FrameType::DataType FrameType::dataType<unsigned long long>()
{
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");
	return DT_UNSIGNED_INTEGER_64;
}

template <>
constexpr FrameType::DataType FrameType::dataType<long long>()
{
	static_assert(sizeof(long long) == 8, "Invalid data type!");
	return DT_SIGNED_INTEGER_64;
}

template <>
constexpr FrameType::DataType FrameType::dataType<float>()
{
	static_assert(sizeof(float) == 4, "Invalid data type!");
	return DT_SIGNED_FLOAT_32;
}

template <>
constexpr FrameType::DataType FrameType::dataType<double>()
{
	static_assert(sizeof(double) == 8, "Invalid data type!");
	return DT_SIGNED_FLOAT_64;
}

template <typename T>
constexpr FrameType::DataType FrameType::dataType()
{
	return DT_UNDEFINED;
}

inline FrameType::DataType FrameType::dataType(const PixelFormat pixelFormat)
{
	ocean_assert(((pixelFormat >> pixelFormatBitOffsetDatatype) & 0xFFull) <= DT_SIGNED_FLOAT_64);

	return DataType((pixelFormat >> pixelFormatBitOffsetDatatype) & 0xFFull);
}

inline uint32_t FrameType::formatGenericBitsPerPixel(const PixelFormat pixelFormat)
{
	return formatGenericNumberChannels(pixelFormat) * bytesPerDataType(dataType(pixelFormat)) * 8u;
}

constexpr inline FrameType::PixelFormat FrameType::genericPixelFormat(const DataType dataType, const uint32_t channels, const uint32_t planes, const uint32_t widthMultiple, const uint32_t heightMultiple)
{
	ocean_assert(uint8_t(dataType) > uint8_t(DT_UNDEFINED) && uint8_t(dataType) < DT_END);
	ocean_assert(channels >= 1u && channels <= 31u);
	ocean_assert(planes >= 1u && planes <= 255u);
	ocean_assert(widthMultiple >= 1u && widthMultiple <= 255u);
	ocean_assert(heightMultiple >= 1u && heightMultiple <= 255u);

	return PixelFormat((uint64_t(heightMultiple) << pixelFormatBitOffsetHeightMultiple) | (uint64_t(widthMultiple) << pixelFormatBitOffsetWidthMultiple) | (uint64_t(planes) << pixelFormatBitOffsetPlanes) | (uint64_t(dataType) << pixelFormatBitOffsetDatatype) | (uint64_t(channels) << pixelFormatBitOffsetChannels));
}

template <FrameType::DataType tDataType, uint32_t tChannels, uint32_t tPlanes, uint32_t tWidthMultiple, uint32_t tHeightMultiple>
constexpr FrameType::PixelFormat FrameType::genericPixelFormat()
{
	static_assert(uint8_t(tDataType) > uint8_t(DT_UNDEFINED) && uint8_t(tDataType) < DT_END, "Invalid data type!");
	static_assert(tChannels >= 1u && tChannels < 31u, "Invalid channel number!");
	static_assert(tPlanes >= 1u && tPlanes <= 255u, "Invalid plane number!");
	static_assert(tWidthMultiple >= 1u && tWidthMultiple <= 255u, "Invalid width-multiple!");
	static_assert(tHeightMultiple >= 1u && tHeightMultiple <= 255u, "Invalid height-multiple!");

	return genericPixelFormat(tDataType, tChannels, tPlanes, tWidthMultiple, tHeightMultiple);
}

template <FrameType::DataType tDataType>
constexpr FrameType::PixelFormat FrameType::genericPixelFormat(const uint32_t channels, const uint32_t planes, const uint32_t widthMultiple, const uint32_t heightMultiple)
{
	static_assert(uint8_t(tDataType) > uint8_t(DT_UNDEFINED) && uint8_t(tDataType) < DT_END, "Invalid data type!");

	return genericPixelFormat(tDataType, channels, planes, widthMultiple, heightMultiple);
}

template <typename TDataType, uint32_t tChannels, uint32_t tPlanes, uint32_t tWidthMultiple, uint32_t tHeightMultiple>
constexpr FrameType::PixelFormat FrameType::genericPixelFormat()
{
	static_assert(tChannels >= 1u && tChannels < 31u, "Invalid channel number!");
	static_assert(tPlanes >= 1u && tPlanes <= 255u, "Invalid plane number!");
	static_assert(tWidthMultiple >= 1u && tWidthMultiple <= 255u, "Invalid width-multiple!");
	static_assert(tHeightMultiple >= 1u && tHeightMultiple <= 255u, "Invalid height-multiple!");

	constexpr DataType pixelFormatDataType = dataType<TDataType>();
	static_assert(uint8_t(pixelFormatDataType) > uint8_t(DT_UNDEFINED) && uint8_t(pixelFormatDataType) < DT_END, "Invalid data type!");

	return genericPixelFormat(pixelFormatDataType, tChannels, tPlanes, tWidthMultiple, tHeightMultiple);
}

template <typename TDataType>
constexpr FrameType::PixelFormat FrameType::genericPixelFormat(const uint32_t channels, const uint32_t planes, const uint32_t widthMultiple, const uint32_t heightMultiple)
{
	constexpr DataType pixelFormatDataType = dataType<TDataType>();
	static_assert(uint8_t(pixelFormatDataType) > uint8_t(DT_UNDEFINED) && uint8_t(pixelFormatDataType) < DT_END, "Invalid data type!");

	ocean_assert(channels >= 1u && channels <= 31u);
	ocean_assert(planes >= 1u && planes <= 255u);
	ocean_assert(widthMultiple >= 1u && widthMultiple <= 255u);
	ocean_assert(heightMultiple >= 1u && heightMultiple <= 255u);

	return genericPixelFormat(pixelFormatDataType, channels, planes, widthMultiple, heightMultiple);
}

inline FrameType::PixelFormat FrameType::makeGenericPixelFormat(const PixelFormat pixelFormat)
{
	static_assert(std::is_same<std::underlying_type<PixelFormat>::type, uint64_t>::value, "Invalid pixel format data type!");

	return PixelFormat(pixelFormat & 0xFFFFFFFFFFFF0000ull); // Cf. documentation of enum PixelFormat
}

inline bool FrameType::formatIsGeneric(const PixelFormat pixelFormat, const DataType pixelFormatDataType, const uint32_t channels, const uint32_t planes, const uint32_t widthMultiple, const uint32_t heightMultiple)
{
	return dataType(pixelFormat) == pixelFormatDataType && formatGenericNumberChannels(pixelFormat) == channels && numberPlanes(pixelFormat) == planes && FrameType::widthMultiple(pixelFormat) == widthMultiple && FrameType::heightMultiple(pixelFormat) == heightMultiple;
}

inline bool FrameType::formatIsGeneric(const PixelFormat pixelFormat)
{
	return dataType(pixelFormat) != DT_UNDEFINED && formatGenericNumberChannels(pixelFormat) != 0u && numberPlanes(pixelFormat) != 0u;
}

inline bool FrameType::formatIsPureGeneric(const PixelFormat pixelFormat)
{
	static_assert(std::is_same<std::underlying_type<PixelFormat>::type, uint64_t>::value, "Invalid pixel format data type!");

	return (pixelFormat & 0x000000000000FFFFull) == 0u && formatIsGeneric(pixelFormat);
}

inline uint32_t FrameType::widthMultiple(const PixelFormat pixelFormat)
{
	return uint32_t((pixelFormat >> pixelFormatBitOffsetWidthMultiple) & 0xFFull);
}

inline uint32_t FrameType::heightMultiple(const PixelFormat pixelFormat)
{
	return uint32_t((pixelFormat >> pixelFormatBitOffsetHeightMultiple) & 0xFFull);
}

inline unsigned int FrameType::planeBytesPerPixel(const PixelFormat& imagePixelFormat, const unsigned int planeIndex)
{
	unsigned int planeWidthDummy;
	unsigned int planeHeightDummy;

	unsigned int planeChannels;

	unsigned int planeWidthElementsMultiple;
	unsigned int planeHeightElementsMultiple;

	if (planeLayout(imagePixelFormat, widthMultiple(imagePixelFormat), heightMultiple(imagePixelFormat), planeIndex, planeWidthDummy, planeHeightDummy, planeChannels, &planeWidthElementsMultiple, &planeHeightElementsMultiple))
	{
		ocean_assert(planeChannels >= 1u && planeWidthElementsMultiple >= 1u && planeHeightElementsMultiple >= 1u);

		if (planeWidthElementsMultiple != 1u || planeHeightElementsMultiple != 1u)
		{
			// we have a packed pixel format for which we cannot calculate the number of bytes per pixel
			return 0u;
		}

		return planeChannels * bytesPerDataType(dataType(imagePixelFormat));
	}
	else
	{
		ocean_assert(false && "Invalid input!");
		return 0u;
	}
}

inline bool FrameType::planeLayout(const FrameType& frameType, const unsigned int planeIndex, unsigned int& planeWidth, unsigned int& planeHeight, unsigned int& planeChannels, unsigned int* planeWidthElementsMultiple, unsigned int* planeHeightElementsMultiple)
{
	ocean_assert(frameType.isValid());

	return planeLayout(frameType.pixelFormat(), frameType.width(), frameType.height(), planeIndex, planeWidth, planeHeight, planeChannels, planeWidthElementsMultiple, planeHeightElementsMultiple);
}

template <typename T>
inline bool FrameType::dataIsAligned(const void* data)
{
	ocean_assert(data != nullptr);
	return size_t(data) % sizeof(T) == size_t(0);
}

template <>
constexpr size_t FrameType::sizeOfType<void>()
{
	return size_t(0);
}

template <typename T>
constexpr size_t FrameType::sizeOfType()
{
	return sizeof(T);
}

inline Frame::Plane::Plane(Plane&& plane) noexcept
{
	*this = std::move(plane);
}

template <typename T>
inline Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const T* dataToUse, const unsigned int paddingElements) noexcept :
	Plane(width, height, channels, sizeof(T), (const void*)dataToUse, paddingElements)
{
	// nothing to do here
}

template <typename T>
inline Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, T* dataToUse, const unsigned int paddingElements) noexcept :
	Plane(width, height, channels, sizeof(T), (void*)dataToUse, paddingElements)
{
	// nothing to do here
}

template <typename T>
inline Frame::Plane::Plane(const T* sourceDataToCopy, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements, const bool makeCopyOfPaddingData) noexcept :
	Plane(width, height, channels, sizeof(T), (const void*)sourceDataToCopy, targetPaddingElements, sourcePaddingElements, makeCopyOfPaddingData)
{
	// nothing to do here
}

template <typename T>
inline Frame::Plane::Plane(const T* sourceDataToCopy, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const CopyMode copyMode) noexcept :
	Plane(width, height, channels, sizeof(T), (const void*)sourceDataToCopy, sourcePaddingElements, copyMode)
{
	// nothing to do here
}

inline Frame::Plane::Plane(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int elementTypeSize, const void* constData, void* data, const unsigned int paddingElements) noexcept :
	allocatedData_(nullptr),
	constData_(constData),
	data_(data),
	width_(width),
	height_(height),
	channels_(channels),
	elementTypeSize_(elementTypeSize),
	paddingElements_(paddingElements)
{
	strideBytes_ = calculateStrideBytes();
	bytesPerPixel_ = calculateBytesPerPixel();
}

inline Frame::Plane::~Plane()
{
	release();
}

inline unsigned int Frame::Plane::width() const
{
	return width_;
}

inline unsigned int Frame::Plane::height() const
{
	return height_;
}

inline unsigned int Frame::Plane::channels() const
{
	return channels_;
}

template <typename T>
inline const T* Frame::Plane::constdata() const
{
	return reinterpret_cast<const T*>(constData_);
}

template <typename T>
inline T* Frame::Plane::data()
{
	return reinterpret_cast<T*>(data_);
}

inline unsigned int Frame::Plane::paddingElements() const
{
	return paddingElements_;
}

inline unsigned int Frame::Plane::paddingBytes() const
{
	return paddingElements_ * elementTypeSize_;
}

inline unsigned int Frame::Plane::elementTypeSize() const
{
	return elementTypeSize_;
}

inline unsigned int Frame::Plane::widthElements() const
{
	return width_ * channels_;
}

inline unsigned int Frame::Plane::widthBytes() const
{
	return widthElements() * elementTypeSize_;
}

inline unsigned int Frame::Plane::strideElements() const
{
	return width_ * channels_ + paddingElements_;
}

inline unsigned int Frame::Plane::strideBytes() const
{
	ocean_assert(width_ == 0u || strideBytes_ != 0u);
	ocean_assert(strideBytes_ == calculateStrideBytes()); // ensuring that stride bytes is actually correct

	return strideBytes_;
}

inline unsigned int Frame::Plane::bytesPerPixel() const
{
	ocean_assert(bytesPerPixel_ == calculateBytesPerPixel());

	return bytesPerPixel_;
}

template <typename T>
inline bool Frame::Plane::isCompatibleWithDataType() const
{
	return elementTypeSize_ == sizeof(T);
}

inline unsigned int Frame::Plane::size() const
{
	return strideBytes() * height_;
}

inline bool Frame::Plane::isContinuous() const
{
	return paddingElements_ == 0u;
}

inline bool Frame::Plane::isOwner() const
{
	return allocatedData_ != nullptr;
}

inline bool Frame::Plane::isReadOnly() const
{
	return data_ == nullptr;
}

inline bool Frame::Plane::isValid() const
{
	return width_ != 0u && height_ != 0u && channels_ != 0u;
}

inline unsigned int Frame::Plane::calculateStrideBytes() const
{
	return strideElements() * elementTypeSize_;
}

template <typename T>
inline Frame::PlaneInitializer<T>::PlaneInitializer(const T* constdata, const CopyMode copyMode, const unsigned int dataPaddingElements) :
	constdata_(constdata),
	data_(nullptr),
	copyMode_(copyMode),
	paddingElements_(dataPaddingElements)
{
	// nothing to do here
}

template <typename T>
inline Frame::PlaneInitializer<T>::PlaneInitializer(T* data, const CopyMode copyMode, const unsigned int dataPaddingElements) :
	constdata_(nullptr),
	data_(data),
	copyMode_(copyMode),
	paddingElements_(dataPaddingElements)
{
	// nothing to do here
}

template <typename T>
inline Frame::PlaneInitializer<T>::PlaneInitializer(const unsigned int planePaddingElements) :
	constdata_(nullptr),
	data_(nullptr),
	copyMode_(CM_USE_KEEP_LAYOUT),
	paddingElements_(planePaddingElements)
{
	// nothing to do here
}

template <typename T>
std::vector<Frame::PlaneInitializer<T>> Frame::PlaneInitializer<T>::createPlaneInitializersWithPaddingElements(const Indices32& paddingElementsPerPlane)
{
	PlaneInitializers<T> planeInitializers;
	planeInitializers.reserve(paddingElementsPerPlane.size());

	for (const Index32& paddingElements : paddingElementsPerPlane)
	{
		planeInitializers.emplace_back(paddingElements);
	}

	return planeInitializers;
}

inline Frame::Frame() :
	FrameType(),
	planes_(1, Plane())
{
	// nothing to do here
}

inline Frame::Frame(Frame&& frame) noexcept :
	FrameType()
{
	*this = std::move(frame);

	ocean_assert(planes_.size() >= 1);
	ocean_assert(frame.planes_.size() == 1);
}

inline Frame::Frame(const FrameType& frameType, const Indices32& planePaddingElements, const Timestamp& timestamp) :
	Frame(frameType, PlaneInitializer<void>::createPlaneInitializersWithPaddingElements(planePaddingElements), timestamp)
{
	ocean_assert(frameType.numberPlanes() == planePaddingElements.size() || planePaddingElements.empty());
	ocean_assert(planes_.size() == frameType.numberPlanes());
}

inline Frame::Frame(const FrameType& frameType, const unsigned int paddingElements, const Timestamp& timestamp) :
	Frame(frameType, PlaneInitializers<void>(1, PlaneInitializer<void>(paddingElements)), timestamp)
{
	ocean_assert(frameType.numberPlanes() == 1u);
	ocean_assert(planes_.size() == 1);
}

template <>
inline Frame::Frame(const FrameType& frameType, const void* data, const CopyMode copyMode, const unsigned int paddingElements, const Timestamp& timestamp) :
	Frame(frameType, PlaneInitializers<void>(1, PlaneInitializer<void>(data, copyMode, paddingElements)), timestamp)
{
	// this constructor is for 1-plane frames only

	ocean_assert(frameType.numberPlanes() == 1u);
	ocean_assert(planes_.size() == 1);
}

template <typename T>
inline Frame::Frame(const FrameType& frameType, const T* data, const CopyMode copyMode, const unsigned int paddingElements, const Timestamp& timestamp) :
	Frame(frameType, (const void*)(data), copyMode, paddingElements, timestamp)
{
#ifdef OCEAN_DEBUG
	const FrameType::DataType debugTemplateDataType = FrameType::dataType<T>();
	const FrameType::DataType debugFrameTypeDataType = frameType.dataType();

	// we ensure that the template data type matches with the data type of the pixel format (as padding is defined in elements)
	ocean_assert(debugTemplateDataType == FrameType::DT_UNDEFINED || debugTemplateDataType == debugFrameTypeDataType);
#endif

	ocean_assert(planes_.size() == 1);
}

template <>
inline Frame::Frame(const FrameType& frameType, void* data, const CopyMode copyMode, const unsigned int paddingElements, const Timestamp& timestamp) :
	Frame(frameType, PlaneInitializers<void>(1, PlaneInitializer<void>(data, copyMode, paddingElements)), timestamp)
{
	// this constructor is for 1-plane frames only

	ocean_assert(frameType.numberPlanes() == 1u);
	ocean_assert(planes_.size() == 1);
}

template <typename T>
inline Frame::Frame(const FrameType& frameType, T* data, const CopyMode copyMode, const unsigned int paddingElements, const Timestamp& timestamp) :
	Frame(frameType, (void*)(data), copyMode, paddingElements, timestamp)
{
#ifdef OCEAN_DEBUG
	const FrameType::DataType debugTemplateDataType = FrameType::dataType<T>();
	const FrameType::DataType debugFrameTypeDataType = frameType.dataType();

	// we ensure that the template data type matches with the data type of the pixel format (as padding is defined in elements)
	ocean_assert(debugTemplateDataType == FrameType::DT_UNDEFINED || debugTemplateDataType == debugFrameTypeDataType);
#endif

	ocean_assert(planes_.size() == 1);
}

template <typename T>
inline Frame::Frame(const FrameType& frameType, const PlaneInitializers<T>& planeInitializers, const Timestamp& timestamp) :
	Frame(frameType, (const PlaneInitializer<void>*)planeInitializers.data(), planeInitializers.size(), timestamp)
{
#ifdef OCEAN_DEBUG
	const FrameType::DataType debugTemplateDataType = FrameType::dataType<T>();
	const FrameType::DataType debugFrameTypeDataType = frameType.dataType();

	// we ensure that the template data type matches with the data type of the pixel format (as padding is defined in elements)
	ocean_assert(debugTemplateDataType == FrameType::DT_UNDEFINED || debugTemplateDataType == debugFrameTypeDataType);
#endif

	ocean_assert(planes_.size() == frameType.numberPlanes());
}

inline const FrameType& Frame::frameType() const
{
	return (const FrameType&)(*this);
}

inline const Frame::Planes& Frame::planes() const
{
	return planes_;
}

template <typename T>
bool Frame::updateMemory(const T* data, const unsigned int planeIndex)
{
	ocean_assert(data != nullptr);
	if (data != nullptr)
	{
		ocean_assert(planeIndex < planes_.size());
		if (planeIndex < planes_.size())
		{
			Plane& plane = planes_[planeIndex];

			ocean_assert((std::is_same<T, void>::value) || sizeOfType<T>() == plane.elementTypeSize());

			ocean_assert(plane.allocatedData_ == nullptr);
			if (plane.allocatedData_ == nullptr)
			{
				plane.constData_ = data;
				plane.data_ = nullptr;

				return true;
			}
		}
	}

	return false;
}

template <typename T>
bool Frame::updateMemory(T* data, const unsigned int planeIndex)
{
	ocean_assert(data != nullptr);
	if (data != nullptr)
	{
		ocean_assert(planeIndex < planes_.size());
		if (planeIndex < planes_.size())
		{
			Plane& plane = planes_[planeIndex];

			ocean_assert((std::is_same<T, void>::value) || sizeOfType<T>() == plane.elementTypeSize());

			ocean_assert(plane.allocatedData_ == nullptr);
			if (plane.allocatedData_ == nullptr)
			{
				plane.data_ = data;
				plane.constData_ = (const T*)(data);

				return true;
			}
		}
	}

	return false;
}

template <typename T>
bool Frame::updateMemory(const std::initializer_list<T*>& planeDatas)
{
	ocean_assert(planeDatas.size() != 0);
	ocean_assert(planeDatas.size() <= planes_.size());

	if (planeDatas.size() == 0 || planeDatas.size() > planes_.size())
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < planeDatas.size(); ++planeIndex)
	{
		if (!updateMemory(planeDatas.begin()[planeIndex], planeIndex))
		{
			return false;
		}
	}

	return true;
}

template <typename T, const unsigned int tPlaneChannels>
bool Frame::setValue(const PixelType<T, tPlaneChannels>& planePixelValue, const unsigned int planeIndex)
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	if (sizeOfType<T>() != plane.elementTypeSize())
	{
		ocean_assert(false && "The specified data type must fit to the frame's data type!");
		return false;
	}

	if (plane.channels() != tPlaneChannels)
	{
		ocean_assert(false && "The specified number of channels does not fit with the plane's actual channels!");
		return false;
	}

	if (plane.isReadOnly())
	{
		return false;
	}

	if (plane.paddingElements_ == 0u)
	{
		PixelType<T, tPlaneChannels>* const data = plane.data<PixelType<T, tPlaneChannels>>();

		for (unsigned int n = 0u; n < plane.width() * plane.height(); ++n)
		{
			data[n] = planePixelValue;
		}
	}
	else
	{
		const unsigned int planeStrideBytes = plane.strideBytes();

		for (unsigned int y = 0u; y < plane.height(); ++y)
		{
			PixelType<T, tPlaneChannels>* const data = (PixelType<T, tPlaneChannels>*)(plane.data<uint8_t>() + y * planeStrideBytes);

			for (unsigned int x = 0u; x < plane.width(); ++x)
			{
				data[x] = planePixelValue;
			}
		}
	}

	return true;
}

template <typename T, const unsigned int tPlaneChannels>
bool Frame::containsValue(const PixelType<T, tPlaneChannels>& planePixelValue, const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	const Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	if (sizeOfType<T>() != plane.elementTypeSize())
	{
		ocean_assert(false && "The specified data type must fit to the frame's data type!");
		return false;
	}

	if (plane.channels() != tPlaneChannels)
	{
		ocean_assert(false && "The specified number of channels does not fit with the plane's actual channels!");
		return false;
	}

	const unsigned int planeStrideBytes = plane.strideBytes();

	for (unsigned int y = 0u; y < plane.height(); ++y)
	{
		PixelType<T, tPlaneChannels>* const data = (PixelType<T, tPlaneChannels>*)(plane.constdata<uint8_t>() + y * planeStrideBytes);

		for (unsigned int x = 0u; x < plane.width(); ++x)
		{
			if (data[x] == planePixelValue)
			{
				return true;
			}
		}
	}

	return false;
}

template <typename T>
bool Frame::setValue(const T* planePixelValue, const size_t planePixelValueSize, const unsigned int planeIndex)
{
	ocean_assert(planePixelValue != nullptr);

	ocean_assert(planes_[planeIndex].elementTypeSize() == sizeOfType<T>());
	ocean_assert(planes_[planeIndex].channels() == planePixelValueSize);

	switch (planePixelValueSize)
	{
		case 1:
		{
			const PixelType<T, 1u> value =
			{{
				planePixelValue[0]
			}};

			return setValue<T, 1u>(value, planeIndex);
		}

		case 2:
		{
			const PixelType<T, 2u> value =
			{{
				planePixelValue[0],
				planePixelValue[1]
			}};

			return setValue<T, 2u>(value, planeIndex);
		}

		case 3:
		{
			const PixelType<T, 3u> value =
			{{
				planePixelValue[0],
				planePixelValue[1],
				planePixelValue[2]
			}};

			return setValue<T, 3u>(value, planeIndex);
		}

		case 4:
		{
			const PixelType<T, 4u> value =
			{{
				planePixelValue[0],
				planePixelValue[1],
				planePixelValue[2],
				planePixelValue[3]
			}};

			return setValue<T, 4u>(value, planeIndex);
		}
	}

	ocean_assert(false && "The number of channels is not supported");
	return false;
}

template <typename T>
bool Frame::setValue(const std::initializer_list<T>& planePixelValues, const unsigned int planeIndex)
{
	return setValue<T>(planePixelValues.begin(), planePixelValues.size(), planeIndex);
}

inline unsigned int Frame::size(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].size();
}

inline unsigned int Frame::paddingElements(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].paddingElements();
}

inline unsigned int Frame::paddingBytes(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].paddingBytes();
}

inline unsigned int Frame::strideElements(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].strideElements();
}

inline unsigned int Frame::strideBytes(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].strideBytes();
}

inline unsigned int Frame::planeWidth(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].width();
}

inline unsigned int Frame::planeHeight(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].height();
}

inline unsigned int Frame::planeChannels(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].channels();
}

inline unsigned int Frame::planeWidthElements(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].widthElements();
}

inline unsigned int Frame::planeWidthBytes(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].widthBytes();
}

inline unsigned int Frame::planeBytesPerPixel(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return FrameType::planeBytesPerPixel(pixelFormat(), planeIndex);
}

inline bool Frame::isPlaneContinuous(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].isContinuous();
}

inline bool Frame::isPlaneOwner(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].isOwner();
}

inline const Timestamp& Frame::timestamp() const
{
	return timestamp_;
}

inline const Timestamp& Frame::relativeTimestamp() const
{
	return relativeTimestamp_;
}

inline void Frame::setTimestamp(const Timestamp& timestamp)
{
	timestamp_ = timestamp;
}

inline void Frame::setRelativeTimestamp(const Timestamp& relativeTimestamp)
{
	relativeTimestamp_ = relativeTimestamp;
}

template <typename T>
inline T* Frame::data(const unsigned int planeIndex)
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].data<T>();
}

template <typename T>
inline const T* Frame::constdata(const unsigned int planeIndex) const
{
	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());

	return planes_[planeIndex].constdata<T>();
}

template <typename T>
inline T* Frame::row(const unsigned int y, const unsigned int planeIndex)
{
	ocean_assert(isValid());
	ocean_assert(y < height());

	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());
	Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	ocean_assert(y < plane.height());
	return reinterpret_cast<T*>(plane.data<uint8_t>() + y * plane.strideBytes());
}

template <typename T>
inline const T* Frame::constrow(const unsigned int y, const unsigned int planeIndex) const
{
	ocean_assert(isValid());
	ocean_assert(y < height());

	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());
	const Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	ocean_assert(y < plane.height());
	return reinterpret_cast<const T*>(plane.constdata<uint8_t>() + y * plane.strideBytes());
}

template <typename T>
inline T* Frame::pixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex)
{
	ocean_assert(isValid());
	ocean_assert(x < planeWidth(planeIndex));
	ocean_assert(y < planeHeight(planeIndex));

	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());
	Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	ocean_assert((std::is_same<T, void>::value) || sizeOfType<T>() == plane.elementTypeSize());

	/*
	 * how to determine pixel offsets within row:
	 *
	 * the pixel format RGB24 has data type`uint8_t` and 3 channels
	 * so the n-th pixel is reach by row<uint8_t>() + n * sizeof(uint8_t) * channels()
	 *
	 * RGB5551 has data type `uint16_t` and 3 channels
	 * so the n-th pixel is reach by row<uint8_t>() + n * sizeof(uint16_t) * channels() / 3  ==  row<uint8_t>() + n * sizeof(uint16_t)
	 *                            or row<uint16_t>() + n * channels() / 3  ==  row<uint16_t>() + n
	 *
	 * Therefore, the pixel offset cannot be determined via x * channels(),
	 * Instead, we determine the offset via bytes per pixel == planeWidthBytes() / planeWidth()
	 */

	ocean_assert(x == 0u || !formatIsPacked(pixelFormat()));

	ocean_assert(plane.bytesPerPixel() != 0u);
	const unsigned int xBytes = x * plane.bytesPerPixel();

	ocean_assert(y < plane.height());
	return reinterpret_cast<T*>(plane.data<uint8_t>() + y * plane.strideBytes() + xBytes);
}

template <typename T>
inline const T* Frame::constpixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex) const
{
	ocean_assert(isValid());
	ocean_assert(x < planeWidth(planeIndex));
	ocean_assert(y < planeHeight(planeIndex));

	ocean_assert(planes_.size() >= 1);
	ocean_assert(planeIndex < planes_.size());
	const Plane& plane = planes_[planeIndex];

	ocean_assert(plane.isValid());

	ocean_assert((std::is_same<T, void>::value) || sizeOfType<T>() == plane.elementTypeSize());

	ocean_assert(x == 0u || !formatIsPacked(pixelFormat()));

	ocean_assert(plane.bytesPerPixel() != 0u);
	const unsigned int xBytes = x * plane.bytesPerPixel();

	ocean_assert(y < plane.height());
	return reinterpret_cast<const T*>(plane.constdata<uint8_t>() + y * plane.strideBytes() + xBytes);
}

inline bool Frame::isContinuous() const
{
	ocean_assert(planes_.size() >= 1);

	for (const Plane& plane : planes_)
	{
		if (!plane.isContinuous())
		{
			return false;
		}
	}

	return true;
}

inline bool Frame::isOwner() const
{
	ocean_assert(planes_.size() >= 1);

	for (const Plane& plane : planes_)
	{
		if (!plane.isOwner())
		{
			return false;
		}
	}

	return true;
}

inline bool Frame::isReadOnly() const
{
	ocean_assert(planes_.size() >= 1);

	for (const Plane& plane : planes_)
	{
		if (plane.isReadOnly())
		{
			return true;
		}
	}

	return false;
}

inline bool Frame::hasAlphaChannel() const
{
	ocean_assert(isValid());

	return formatHasAlphaChannel(pixelFormat());
}

template <>
inline bool Frame::hasTransparentPixel(const uint8_t opaque) const
{
	if (!hasAlphaChannel())
	{
		return false;
	}

	ocean_assert(numberPlanes() == 1u);

	if (dataType() != dataType<uint8_t>())
	{
		ocean_assert(false && "Data type does not fit with the frame's data type!");
		return false;
	}

	if (pixelFormat() == FORMAT_YA16)
	{
		for (unsigned int y = 0u; y < height(); ++y)
		{
			const uint8_t* row = constrow<uint8_t>(y) + 1;

			for (unsigned int x = 0u; x < width(); ++x)
			{
				if (*row != opaque)
				{
					return true;
				}

				row += 2;
			}
		}
	}
	else
	{
		ocean_assert(pixelFormat() == FORMAT_ABGR32 || pixelFormat() == FORMAT_ARGB32 || pixelFormat() == FORMAT_RGBA32 || pixelFormat() == FORMAT_BGRA32 || pixelFormat() == FORMAT_YUVA32);

		const unsigned int offset = (pixelFormat() == FORMAT_ABGR32 || pixelFormat() == FORMAT_ARGB32) ? 0u : 3u;

		for (unsigned int y = 0u; y < height(); ++y)
		{
			const uint8_t* row = constrow<uint8_t>(y) + offset;

			for (unsigned int x = 0u; x < width(); ++x)
			{
				if (*row != opaque)
				{
					return true;
				}

				row += 4;
			}
		}
	}

	return false;
}

template <>
inline bool Frame::hasTransparentPixel(const uint16_t opaque) const
{
	if (!hasAlphaChannel())
	{
		return false;
	}

	ocean_assert(numberPlanes() == 1u);

	if (dataType() != dataType<uint16_t>())
	{
		ocean_assert(false && "Data type does not fit with the frame's data type!");
		return false;
	}

	if (pixelFormat() == FORMAT_RGBA64)
	{
		for (unsigned int y = 0u; y < height(); ++y)
		{
			const uint16_t* row = constrow<uint16_t>(y) + 3;

			for (unsigned int x = 0u; x < width(); ++x)
			{
				if (*row != opaque)
				{
					return true;
				}

				row += 4;
			}
		}
	}
	else
	{
		ocean_assert(pixelFormat() == FORMAT_RGBA4444 || pixelFormat() == FORMAT_BGRA4444);

		for (unsigned int y = 0u; y < height(); ++y)
		{
			const uint16_t* row = constrow<uint16_t>(y);

			for (unsigned int x = 0u; x < width(); ++x)
			{
				if ((*row & opaque) != opaque)
				{
					return true;
				}

				++row;
			}
		}
	}

	return false;
}

template <typename T>
bool Frame::hasTransparentPixel(const T /*opaque*/) const
{
	return false;
}

inline bool Frame::isValid() const
{
	ocean_assert(planes_.size() >= 1);

	const bool frameTypeIsValid = FrameType::isValid();

#ifdef OCEAN_DEBUG
	{
		// we ensure that the state of `planes_` is consistent with the state of `FrameType::isValid()`

		size_t debugValidPlanes = 0;

		for (const Plane& plane : planes_)
		{
			if (plane.isValid())
			{
				++debugValidPlanes;
			}
		}

		const bool debugIsValid = !planes_.isEmpty() && debugValidPlanes == planes_.size();

		ocean_assert(debugIsValid == frameTypeIsValid);
	}
#endif // OCEAN_DEBUG

	return frameTypeIsValid;
}

inline Frame::operator bool() const
{
	return isValid();
}

}

#endif // META_OCEAN_BASE_FRAME_H
