/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_INTEGRAL_IMAGE_H
#define META_OCEAN_CV_INTEGRAL_IMAGE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Numeric.h"

#include <limits>
#include <type_traits>

namespace Ocean
{

namespace CV
{

/**
 * This class provides functions to create an integral image from a gray scale image.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT IntegralImage
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Creates an integral image from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
				 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
				 * The resulting lined integral image has the following scheme:
				 * <pre>
				 *  ------------ ---------
				 * |000000000000|         |
				 * |0|----------|         |
				 * |0|          | padding |
				 * |0| Integral |         |
				 * |0|          |         |
				 *  ------------ ---------
				 * </pre>
				 * The resolution of the integral image is: (width + 1)x(height + 1).
				 * @param frame The frame for which the integral image will be returned, must be valid
				 * @return The resulting integral image, invalid if the pixel format of the given frame is not supported
				 */
				static Frame createLinedImage(const Frame& frame);

				/**
				 * Creates a bordered integral image from a given 1-plane image and adds an extra border to the resulting integral image.
				 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
				 * The resulting integral image has a (non interfering) border.<br>
				 * The bordered integral image has the following scheme:
				 * <pre>
				 *  ------------------------- ---------
				 * |0000000000000000000000000|         |
				 * |0|-----------------------|         |
				 * |0|     |           |     |         |
				 * |0|  0  |     0     |  0  |         |
				 * |0|     |           |     |         |
				 * |0|-----|-----------|-----|         |
				 * |0|     |           |  >  |         |
				 * |0|     |           |  >  |         |
				 * |0|  0  | Integral  |  >  | padding |
				 * |0|     |           |  >  |         |
				 * |0|     |           |  >  |         |
				 * |0|-----|-----------|-----|         |
				 * |0|     |           |     |         |
				 * |0|  0  |     V     |  V  |         |
				 * |0|     |           |     |         |
				 *  -----------------------------------
				 * </pre>
				 * Columns or rows with '0' receive a null value.<br>
				 * Rows with '>' receive the last valid integral value from the left.<br>
				 * Rows with 'v' receive the last valid integral value from the top.<br>
				 * Further, additionally to the border, the integral image contains one column at the left border and one row at the top border with zeros.<br>
				 * The resolution of the integral image is: (width + 1 + 2 * border)x(height + 1 + 2 * border).
				 * @param frame The image for which the integral image will be returned, with size width x height, must be valid
				 * @param border The thickness of the border in pixel, with range [1, infinity)
				 * @return The resulting integral image, invalid if the pixel format of the given frame is not supported
				 */
				static Frame createBorderedImage(const Frame& frame, const unsigned int border);

			protected:

				/**
				 * Creates an integral image from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
				 * @param frame The frame for which the integral image will be returned, must be valid
				 * @return The resulting integral image, invalid if the pixel format of the given frame is not supported
				 * @tparam T The data type of each frame element
				 * @tparam TIntegral The data type of each integral element
				 */
				template <typename T, typename TIntegral>
				static Frame createLinedImage(const Frame& frame);

				/**
				 * Creates a bordered integral image from a given 1-plane image and adds an extra border to the resulting integral image.
				 * @param frame The frame for which the integral image will be returned, must be valid
				 * @param border The thickness of the border in pixel, with range [1, infinity)
				 * @return The resulting integral image, invalid if the pixel format of the given frame is not supported
				 * @tparam T The data type of each frame element
				 * @tparam TIntegral The data type of each integral element
				 */
				template <typename T, typename TIntegral>
				static Frame createBorderedImage(const Frame& frame, const unsigned int border);
		};

	public:

		/**
		 * Creates an integral image from a given 1-plane image.
		 * The resulting integral image will have the same resolution as the given image (without any extra borders).<br>
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting integral image has the following scheme:
		 * <pre>
		 *  ---------- ---------
		 * |          |         |
		 * | Integral | padding |
		 * |          |         |
		 *  ---------- ---------
		 * </pre>
		 * The resolution of the integral image is: width x height.
		 * @param source The source frame for which the integral image will be created, must be valid
		 * @param integral The target integral image, must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImage().
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates an integral image from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting lined integral image has the following scheme:
		 * <pre>
		 *  ------------ ---------
		 * |000000000000|         |
		 * |0|----------|         |
		 * |0|          | padding |
		 * |0| Integral |         |
		 * |0|          |         |
		 *  ------------ ---------
		 * </pre>
		 * The resolution of the integral image is: (width + 1)x(height + 1).
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1)x(height + 1), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createImage(), createBorderedImage().
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createLinedImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates an integral image with squared pixel intensities from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting lined integral image has the following scheme:
		 * <pre>
		 *  -------------- ---------
		 * |00000000000000|         |
		 * |0|------------|         |
		 * |0|            | padding |
		 * |0| Integral^2 |         |
		 * |0|            |         |
		 *  -------------- ---------
		 * </pre>
		 * The resolution of the integral image is: (width + 1)x(height + 1).
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1)x(height + 1), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImageAndSquared(), createImage(), createBorderedImage().
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createLinedImageSquared(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates an integral image and squared integral image from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
		 * The resulting integral image provides access to pixel intensities and squared pixel intensities.<br>
		 * Beware: As the same data type is used for both integral data, the depth of the data type must be large enough, or the image resolution must be small enough to avoid overflows.<br>
		 * Pixel intensities and squared pixel intensities are interleaved so that both values can be looked-up at the same memory location.<br>
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting lined integral image has the following scheme:
		 * <pre>
		 *  ------------------------------------ ---------
		 * |000000000000000000000000000000000000|         |
		 * |00|---------------------------------|         |
		 * |00|                                 | padding |
		 * |00| I0 I1 I0^2 I1^2 I0 I1 I0^2 I1^2 |         |
		 * |00|                                 |         |
		 *  ------------------------------------ ---------
		 * With I0 integral value of the first channel,
		 * I1 integral value of the second channel,
		 * I0^2 integral squared value of the first channel,
		 * I1^2 integral squared value of the second channel.
		 * </pre>
		 * The resolution of the integral image is: ((width + 1) * 2)x(height + 1).
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integralAndSquared The resulting integral (and squared integral) image, with size ((width + 1) * 2)x(height + 1), must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralAndSquaredPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegralAndSquared The data type of each integral (and squared integral) pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImage(), createLinedImageSquared().
		 */
		template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
		static void createLinedImageAndSquared(const T* source, TIntegralAndSquared* integralAndSquared, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralAndSquaredPaddingElements);

		/**
		 * Creates an integral image and squared integral image from a given 1-plane image and adds an extra line (one column and one row) with zeros to the left and top image border.
		 * The result are two individual integral images, one for the pixel intensities and one for the squared pixel intensities.<br>
		 * The resulting lined integral images have the following scheme:
		 * <pre>
		 * Integral image:                     Integral squared image:
		 *  ---------------- ---------          ------------------------ ---------
		 * |0000000000000000|         |        |000000000000000000000000|         |
		 * |00|-------------|         |        |00|---------------------|         |
		 * |00|             | padding |        |00|                     | padding |
		 * |00| I0 I1 I0 I1 |         |        |00| I0^2 I1^2 I0^2 I1^2 |         |
		 * |00|             |         |        |00|                     |         |
		 *  ---------------- ---------          ------------------------ ---------
		 * With I0 integral value of the first channel,
		 * I1 integral value of the second channel,
		 * I0^2 integral squared value of the first channel,
		 * I1^2 integral squared value of the second channel.
		 * </pre>
		 * The resolution of the integral image are: (width + 1)x(height + 1).
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1)x(height + 1), must be valid
		 * @param integralSquared The resulting squared integral image, with size (width + 1)x(height + 1), must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @param integralSquaredPaddingElements The number of padding elements at the end of each row of the integral squared frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam TIntegralSquared The data type of each squared integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImage(), createLinedImageSquared().
		 */
		template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
		static void createLinedImageAndSquared(const T* source, TIntegral* integral, TIntegralSquared* integralSquared, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int integralSquaredPaddingElements);

		/**
		 * Creates a bordered integral image from a given 1-plane image and adds an extra border to the resulting integral image.
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting integral image has a (non interfering) border.<br>
		 * The bordered integral image has the following scheme:
		 * <pre>
		 *  ------------------------- ---------
		 * |0000000000000000000000000|         |
		 * |0|-----------------------|         |
		 * |0|     |           |     |         |
		 * |0|  0  |     0     |  0  |         |
		 * |0|     |           |     |         |
		 * |0|-----|-----------|-----|         |
		 * |0|     |           |  >  |         |
		 * |0|     |           |  >  |         |
		 * |0|  0  | Integral  |  >  | padding |
		 * |0|     |           |  >  |         |
		 * |0|     |           |  >  |         |
		 * |0|-----|-----------|-----|         |
		 * |0|     |           |     |         |
		 * |0|  0  |     V     |  V  |         |
		 * |0|     |           |     |         |
		 *  -----------------------------------
		 * </pre>
		 * Columns or rows with '0' receive a null value.<br>
		 * Rows with '>' receive the last valid integral value from the left.<br>
		 * Rows with 'v' receive the last valid integral value from the top.<br>
		 * Further, additionally to the border, the integral image contains one column at the left border and one row at the top border with zeros.<br>
		 * Therefore, the entire integral image width is: 1 + 2 * border + originalWidth,<br>
		 * and the entire height is: 1 + 2 * border + originalHeight.<br>
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1 + 2 * border)x(height + 1 + 2 * border), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param border The thickness of the border in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImage().
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createBorderedImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates a bordered squared integral image from a given 1-plane image and adds an extra border to the resulting integral image.
		 * In case, the given frame has more than one channel, the channels of the resulting integral image will be interleaved (not stored as individual planes).<br>
		 * The resulting integral image has a (non interfering) border.<br>
		 * The bordered integral image has the following scheme:
		 * <pre>
		 *  -------------------------- ---------
		 * |00000000000000000000000000|         |
		 * |0|------------------------|         |
		 * |0|     |            |     |         |
		 * |0|  0  |     0      |  0  |         |
		 * |0|     |            |     |         |
		 * |0|-----|------------|-----|         |
		 * |0|     |            |  >  |         |
		 * |0|     |            |  >  |         |
		 * |0|  0  | Integral^2 |  >  | padding |
		 * |0|     |            |  >  |         |
		 * |0|     |            |  >  |         |
		 * |0|-----|------------|-----|         |
		 * |0|     |            |     |         |
		 * |0|  0  |     V      |  V  |         |
		 * |0|     |            |     |         |
		 *  ------------------------------------
		 * </pre>
		 * Columns or rows with '0' receive a null value.<br>
		 * Rows with '>' receive the last valid integral value from the left.<br>
		 * Rows with 'v' receive the last valid integral value from the top.<br>
		 * Further, additionally to the border, the integral image contains one column at the left border and one row at the top border with zeros.<br>
		 * Therefore, the entire integral image width is: 1 + 2 * border + originalWidth,<br>
		 * and the entire height is: 1 + 2 * border + originalHeight.<br>
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1)x(height + 1), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param border The thickness of the border in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 * @see createLinedImage().
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createBorderedImageSquared(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates a bordered integral image from a given 1-plane image and adds an extra border with mirrored image content to the resulting integral image.
		 * The resulting integral image has an extra border created from the original frame with mirrored border pixels.<br>
		 * Additionally, independent from the border size, the integral image contains one column at the left border and one row at the top border with zeros.<br>
		 * Therefore, the entire integral image width is: 1 + 2 * border + originalWidth,<br>
		 * and the entire height is: 1 + 2 * border + originalHeight.<br>
		 * The bordered integral image has the following scheme:
		 * <pre>
		 *  ------------------------- ---------
		 * |0000000000000000000000000|         |
		 * |0|-----------------------|         |
		 * |0|     |           |     |         |
		 * |0|  m  |  mirrored |  m  |         |
		 * |0|     |           |     |         |
		 * |0|-----|-----------|-----|         |
		 * |0|     |           |     |         |
		 * |0|     |           |     |         |
		 * |0|  m  | Integral  |  m  | padding |
		 * |0|     |           |     |         |
		 * |0|     |           |     |         |
		 * |0|-----|-----------|-----|         |
		 * |0|     |           |     |         |
		 * |0|  m  |  mirrored |  m  |         |
		 * |0|     |           |     |         |
		 *  -----------------------------------
		 * </pre>
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (1 + border + width)x(1 + border + height), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param border The thickness of the border in pixel, with range [1, min(width, height)]
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createBorderedImageMirror(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Creates a bordered squared integral image from a given 1-plane image and adds an extra border with mirrored image content to the resulting integral image.
		 * The resulting integral image has an extra border created from the original frame with mirrored border pixels.<br>
		 * Additionally, independent from the border size, the integral image contains one column at the left border and one row at the top border with zeros.<br>
		 * Therefore, the entire integral image width is: 1 + 2 * border + originalWidth,<br>
		 * and the entire height is: 1 + 2 * border + originalHeight.<br>
		 * The bordered integral image has the following scheme:
		 * <pre>
		 *  ------------------------------- ---------
		 * |0000000000000000000000000000000|         |
		 * |0|-----------------------------|         |
		 * |0|       |             |       |         |
		 * |0|  m^2  |  mirrored^2 |  m^2  |         |
		 * |0|       |             |       |         |
		 * |0|-------|-------------|-------|         |
		 * |0|       |             |       |         |
		 * |0|       |             |       |         |
		 * |0|  m^2  | Integral^2  |  m^2  | padding |
		 * |0|       |             |       |         |
		 * |0|       |             |       |         |
		 * |0|-------|-------------|-------|         |
		 * |0|       |             |       |         |
		 * |0|  m^2  |  mirrored^2 |  m^2  |         |
		 * |0|       |             |       |         |
		 *  -----------------------------------------
		 * </pre>
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (1 + border + width)x(1 + border + height), must be valid
		 * @param width The width of the source frame in pixel, with range [0, infinity)
		 * @param height The height of the source frame in pixel, with range [0, infinity)
		 * @param border The thickness of the border in pixel, with range [1, min(width, height)]
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element of the source frame e.g., 'uint8_t' or 'float'
		 * @tparam TIntegral The data type of each integral pixel element, e.g., 'unsigned int' or 'double'
		 * @tparam tChannels The number of channels the source frame has, with range [1, infinity)
		 */
		template <typename T, typename TIntegral, unsigned int tChannels>
		static void createBorderedImageSquaredMirror(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

		/**
		 * Determines the sum of elements within a window from an integral image.
		 * The lined integral image must have the following scheme:
		 * <pre>
		 *    integralStrideElements
		 * |<------------------------>|
		 *
		 *  ---------------- ---------
		 * |0000000000000000|         |
		 * |0|--------------|         |
		 * |0|              | padding |
		 * |0|   Integral   |         |
		 * |0|              |         |
		 *  ----------------- ---------
		 *
		 *   |<------------>|
		 *    original width
		 *
		 * |<-------------->|
		 * lined integral width
		 * </pre>
		 * @param linedIntegral The lined integral image, must be valid
		 * @param linedIntegralStrideElements The number of elements between two row start positions in the lined integral image, in elements, may contain padding elements at the end of each row, with range [originalImageWidth + 1, infinity)
		 * @param windowLeft The left starting point of the window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowTop The top starting point of the window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowWidth The width of the window, in pixel, with range [1, originalImageWidth - windowLeft]
		 * @param windowHeight The height of the window, in pixel, with range [1, originalImageHeight - windowTop]
		 * @return The resulting sum of elements
		 * @tparam TIntegral The data type of the elements in the integral image
		 */
		template <typename TIntegral>
		static OCEAN_FORCE_INLINE  TIntegral linedIntegralSum(const TIntegral* const linedIntegral, const unsigned int linedIntegralStrideElements, const unsigned int windowLeft, const unsigned int windowTop, const unsigned int windowWidth, const unsigned int windowHeight);

		/**
		 * Determines the variance of elements within a window from two integral images.
		 * Variance is calculated based on the following equation:
		 * <pre>
		 * Var(x) = E[(x - E[x])^2] = E[x^2] - E[x]^2
		 * </pre>
		 * The variance is determined based on the standard lined integral image and the lined integral image for squared values.<br>
		 * The lined integral image must have the following scheme:
		 * <pre>
		 *    integralStrideElements                      integralSquaredStrideElements
		 * |<------------------------>|				   |<------------------------------>|
		 *
		 *  ---------------- ---------				    -------------------- -----------
		 * |0000000000000000|         |				   |00000000000000000000|           |
		 * |0|--------------|         |				   |0|------------------|           |
		 * |0|              | padding |				   |0|                  |  padding  |
		 * |0|   Integral   |         |				   |0|    Integral^2    |           |
		 * |0|              |         |				   |0|                  |           |
		 *  ----------------- ---------				    -------------------- -----------
		 *
		 *   |<------------>|						     |<---------------->|
		 *    original width						         original width
		 *
		 * |<-------------->|						   |<------------------>|
		 * lined integral width					     lined integral squared width
		 * </pre>
		 * @param linedIntegral The lined integral image, must be valid
		 * @param linedIntegralSquared The lined integral image with squared values, must be valid
		 * @param integralStrideElements The number of elements between two row starts in the lined integral image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param integralStrideSquaredElements The number of elements between two row starts in the lined integral squared image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param windowLeft The left starting point of the window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowTop The top starting point of the window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowWidth The width of the window, in pixel, with range [1, originalImageWidth - windowLeft]
		 * @param windowHeight The height of the window, in pixel, with range [1, originalImageHeight - windowTop]
		 * @param mean Optional resulting mean value and (`tReturnMean = true`), nullptr otherwise (and `tReturnMean = false`)
		 * @return The resulting variance, with range [0, infinity)
		 * @tparam TIntegral The data type of the elements in the integral image
		 * @tparam TIntegralSquared The data type of the elements in the integral squared image
		 * @tparam TVariance The data type of the resulting variance (also used for intermediate calculations)
		 * @tparam tReturnMean True, to return the mean value (`mean` must be defined); False, to skip calculating of mean (`mean` must be nullptr)
		 */
		template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean = false>
		static inline TVariance linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowLeft, const unsigned int windowTop, const unsigned int windowWidth, const unsigned int windowHeight, TVariance* mean = nullptr);

		/**
		 * Determines the variance of elements within two windows from two integral images.
		 * The two windows are treated as one joined area.<br>
		 * Variance is calculated based on the following equation:
		 * <pre>
		 * Var(x) = E[(x - E[x])^2] = E[x^2] - E[x]^2
		 * </pre>
		 * The variance is determined based on the standard lined integral image and the lined integral image for squared values.<br>
		 * The lined integral image must have the following scheme:
		 * <pre>
		 *    integralStrideElements                      integralSquaredStrideElements
		 * |<------------------------>|				   |<------------------------------>|
		 *
		 *  ---------------- ---------				    -------------------- -----------
		 * |0000000000000000|         |				   |00000000000000000000|           |
		 * |0|--------------|         |				   |0|------------------|           |
		 * |0|              | padding |				   |0|                  |  padding  |
		 * |0|   Integral   |         |				   |0|    Integral^2    |           |
		 * |0|              |         |				   |0|                  |           |
		 *  ----------------- ---------				    -------------------- -----------
		 *
		 *   |<------------>|						     |<---------------->|
		 *    original width						         original width
		 *
		 * |<-------------->|						   |<------------------>|
		 * lined integral width					     lined integral squared width
		 * </pre>
		 * @param linedIntegral The lined integral image, must be valid
		 * @param linedIntegralSquared The lined integral image with squared values, must be valid
		 * @param integralStrideElements The number of elements between two row starts in the lined integral image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param integralStrideSquaredElements The number of elements between two row starts in the lined integral squared image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param windowALeft The left starting point of the first window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowATop The top starting point of the first window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowAWidth The width of the first window, in pixel, with range [1, originalImageWidth - windowALeft]
		 * @param windowAHeight The height of the first window, in pixel, with range [1, originalImageHeight - windowATop]
		 * @param windowBLeft The left starting point of the second window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowBTop The top starting point of the second window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowBWidth The width of the second window, in pixel, with range [1, originalImageWidth - windowBLeft]
		 * @param windowBHeight The height of the second window, in pixel, with range [1, originalImageHeight - windowBTop]
		 * @param mean Optional resulting mean value and (`tReturnMean = true`), nullptr otherwise (and `tReturnMean = false`)
		 * @return The resulting variance, with range [0, infinity)
		 * @tparam TIntegral The data type of the elements in the integral image
		 * @tparam TIntegralSquared The data type of the elements in the integral squared image
		 * @tparam TVariance The data type of the resulting variance (also used for intermediate calculations)
		 * @tparam tReturnMean True, to return the mean value (`mean` must be defined); False, to skip calculating of mean (`mean` must be nullptr)
		 */
		template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean = false>
		static inline TVariance linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowALeft, const unsigned int windowATop, const unsigned int windowAWidth, const unsigned int windowAHeight, const unsigned int windowBLeft, const unsigned int windowBTop, const unsigned int windowBWidth, const unsigned int windowBHeight, TVariance* mean = nullptr);

		/**
		 * Determines the variance of elements within three windows from two integral images.
		 * The three windows are treated as one joined area.<br>
		 * @param linedIntegral The lined integral image, must be valid
		 * @param linedIntegralSquared The lined integral image with squared values, must be valid
		 * @param integralStrideElements The number of elements between two row starts in the lined integral image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param integralStrideSquaredElements The number of elements between two row starts in the lined integral squared image, in elements, may contain padding elements at the end of each row, with range [2, infinity)
		 * @param windowALeft The left starting point of the first window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowATop The top starting point of the first window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowAWidth The width of the first window, in pixel, with range [1, originalImageWidth - windowALeft]
		 * @param windowAHeight The height of the first window, in pixel, with range [1, originalImageHeight - windowATop]
		 * @param windowBLeft The left starting point of the second window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowBTop The top starting point of the second window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowBWidth The width of the second window, in pixel, with range [1, originalImageWidth - windowBLeft]
		 * @param windowBHeight The height of the second window, in pixel, with range [1, originalImageHeight - windowBTop]
		 * @param windowCLeft The left starting point of the third window, in pixel, with range [0, originalImageWidth - 1]
		 * @param windowCTop The top starting point of the third window, in pixel, with range [0, originalImageHeight - 1]
		 * @param windowCWidth The width of the third window, in pixel, with range [1, originalImageWidth - windowCLeft]
		 * @param windowCHeight The height of the third window, in pixel, with range [1, originalImageHeight - windowCTop]
		 * @param mean Optional resulting mean value and (`tReturnMean = true`), nullptr otherwise (and `tReturnMean = false`)
		 * @return The resulting variance, with range [0, infinity)
		 * @tparam TIntegral The data type of the elements in the integral image
		 * @tparam TIntegralSquared The data type of the elements in the integral squared image
		 * @tparam TVariance The data type of the resulting variance (also used for intermediate calculations)
		 * @tparam tReturnMean True, to return the mean value (`mean` must be defined); False, to skip calculating of mean (`mean` must be nullptr)
		 */
		template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean = false>
		static inline TVariance linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowALeft, const unsigned int windowATop, const unsigned int windowAWidth, const unsigned int windowAHeight, const unsigned int windowBLeft, const unsigned int windowBTop, const unsigned int windowBWidth, const unsigned int windowBHeight, const unsigned int windowCLeft, const unsigned int windowCTop, const unsigned int windowCWidth, const unsigned int windowCHeight, TVariance* mean = nullptr);

	private:

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(__aarch64__)

		/**
		 * Creates an integral image from 8 bit images with 1 channel and adds an extra line with zeros to the left and top image border and applies NEON instructions.
		 * @param source The image for which the integral image will be determined, with size width x height, must be valid
		 * @param integral The resulting integral image, with size (width + 1)x(height + 1), must be valid
		 * @param width The width of the given image in pixel, with range [8, 4096^2]
		 * @param height The height of the given image in pixel, with range [1, 4096^2 / width]
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param integralPaddingElements The number of padding elements at the end of each row of the integral frame, in elements, with range [0, infinity)
		 * @see createLinedImage8BitPerChannel<tChannels>().
		 */
		static void createLinedImage1Channel8BitNEON(const uint8_t* source, uint32_t* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements);

#endif // defined(__aarch64__)

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Returns the square value of the given parameter.
		 * @param value The value to be squared
		 * @return Square value
		 * @tparam T The data type of the value to be squared
		 * @tparam TSquared The data type of the squared value
		 */
		template <typename T, typename TSquared>
		static inline TSquared sqr(const T& value);
};

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(std::is_signed<T>::value == std::is_signed<TIntegral>::value, "The integral image must have the same sign-properties as the source image!");
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(sizeof(T) >=4 || double(NumericT<T>::maxValue()) * double(width * height) <= double(NumericT<TIntegral>::maxValue()));

	const T* const sourceFirstRowEnd = source + width * tChannels;
	const T* const sourceEnd = source + (width * tChannels + sourcePaddingElements) * height;
	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		previousIntegral[n] = TIntegral(0);
	}

	// the first row

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(*source++);
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	source += sourcePaddingElements;
	integral += integralPaddingElements;


	// the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(*source++);
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralPreviousRow += integralPaddingElements;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createLinedImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(std::is_signed<T>::value == std::is_signed<TIntegral>::value, "The integral image must have the same sign-properties as the source image!");
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert((std::is_floating_point<T>::value) || (double(NumericT<T>::maxValue()) * double(width * height) <= double(NumericT<TIntegral>::maxValue())));

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	if (std::is_same<T, uint8_t>::value && std::is_same<TIntegral, uint32_t>::value && tChannels == 1u && width >= 8u)
	{
		createLinedImage1Channel8BitNEON((const uint8_t*)source, (uint32_t*)integral, width, height, sourcePaddingElements, integralPaddingElements);
		return;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	/*
	 * This is the resulting lined integral image.
	 *  ------------
	 * |000000000000|
	 * |0|----------|
	 * |0|          |
	 * |0| Integral |
	 * |0|          |
	 * |------------
	 */

	// entire top line will be set to zero
	memset(integral, 0x00, (width + 1u) * tChannels * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < width + 1u; ++n)
	{
		ocean_assert(integral[n] == TIntegral(0));
	}
#endif

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width * tChannels;
	const T* const sourceEnd = source + (width * tChannels + sourcePaddingElements) * height;

	integral += (width + 1u) * tChannels + integralPaddingElements;
	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral[tChannels] = {TIntegral(0)};

	// left pixel
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		*integral++ = TIntegral(0);
	}

	// the remaining pixels of the first row

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source++;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	source += sourcePaddingElements;
	integral += integralPaddingElements;


	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		// left pixel
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = TIntegral(0);
		}

		integralPreviousRow += tChannels;

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source++;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralPreviousRow += integralPaddingElements;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createLinedImageSquared(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert((std::is_floating_point<T>::value) || (NumericD::sqr(double(NumericT<T>::maxValue())) * double(width * height) <= double(NumericT<TIntegral>::maxValue())));

	// entire top line will be set to zero
	memset(integral, 0x00, (width + 1u) * tChannels * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < width + 1u; ++n)
	{
		ocean_assert(integral[n] == TIntegral(0));
	}
#endif

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width * tChannels;
	const T* const sourceEnd = source + (width * tChannels + sourcePaddingElements) * height;

	integral += (width + 1u) * tChannels + integralPaddingElements;
	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral[tChannels] = {TIntegral(0)};

	// left pixel
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		*integral++ = TIntegral(0);
	}

	// the remaining pixels of the first row

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source * *source;
			++source;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	source += sourcePaddingElements;
	integral += integralPaddingElements;


	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		// left pixel
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = TIntegral(0);
		}

		integralPreviousRow += tChannels;

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source * *source;
				++source;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralPreviousRow += integralPaddingElements;
	}
}

template <typename T, typename TIntegralAndSquared, unsigned int tChannels>
void IntegralImage::createLinedImageAndSquared(const T* source, TIntegralAndSquared* integralAndSquared, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralAndSquaredPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegralAndSquared), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integralAndSquared != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert((std::is_floating_point<T>::value) || (NumericD::sqr(double(NumericT<T>::maxValue())) * double(width * height) <= double(NumericT<TIntegralAndSquared>::maxValue())));

	// entire top line will be set to zero
	memset(integralAndSquared, 0x00, (width + 1u) * tChannels * sizeof(TIntegralAndSquared) * 2u);

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < (width + 1u) * 2u; ++n)
	{
		ocean_assert(integralAndSquared[n] == TIntegralAndSquared(0));
	}
#endif

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width * tChannels;
	const T* const sourceEnd = source + (width * tChannels + sourcePaddingElements) * height;

	integralAndSquared += (width + 1u) * tChannels * 2u + integralAndSquaredPaddingElements;
	const TIntegralAndSquared* integralAndSquaredPreviousRow = integralAndSquared;

	TIntegralAndSquared previousIntegral[tChannels] = {TIntegralAndSquared(0)};
	TIntegralAndSquared previousIntegralSquared[tChannels] = {TIntegralAndSquared(0)};

	// left pixel integral and squared integral
	for (unsigned int n = 0u; n < tChannels * 2u; ++n)
	{
		*integralAndSquared++ = TIntegralAndSquared(0);
	}

	// the remaining pixels of the first row

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source;
			previousIntegralSquared[n] += *source * *source;
			++source;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integralAndSquared++ = previousIntegral[n];
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integralAndSquared++ = previousIntegralSquared[n];
		}
	}

	source += sourcePaddingElements;
	integralAndSquared += integralAndSquaredPaddingElements;

	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegralAndSquared(0);
		}
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegralSquared[n] = TIntegralAndSquared(0);
		}

		// left pixel integral and squared integral
		for (unsigned int n = 0u; n < tChannels * 2u; ++n)
		{
			*integralAndSquared++ = TIntegralAndSquared(0);
		}

		integralAndSquaredPreviousRow += tChannels * 2u;

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source;
				previousIntegralSquared[n] += *source * *source;
				++source;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integralAndSquared++ = previousIntegral[n] + *integralAndSquaredPreviousRow++;
			}
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integralAndSquared++ = previousIntegralSquared[n] + *integralAndSquaredPreviousRow++;
			}
		}

		source += sourcePaddingElements;
		integralAndSquared += integralAndSquaredPaddingElements;
		integralAndSquaredPreviousRow += integralAndSquaredPaddingElements;
	}
}

template <typename T, typename TIntegral, typename TIntegralSquared, unsigned int tChannels>
void IntegralImage::createLinedImageAndSquared(const T* source, TIntegral* integral, TIntegralSquared* integralSquared, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements, const unsigned int integralSquaredPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(sizeof(TIntegral) <= sizeof(TIntegralSquared), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr && integralSquared != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert((std::is_floating_point<T>::value) || (double(NumericT<T>::maxValue()) * double(width * height) <= double(NumericT<TIntegral>::maxValue()) && NumericD::sqr(double(NumericT<T>::maxValue())) * double(width * height) <= double(NumericT<TIntegralSquared>::maxValue())));

	// entire top line will be set to zero
	memset(integral, 0x00, (width + 1u) * tChannels * sizeof(TIntegral));
	memset(integralSquared, 0x00, (width + 1u) * tChannels * sizeof(TIntegralSquared));

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < width + 1u; ++n)
	{
		ocean_assert(integral[n] == TIntegral(0));
		ocean_assert(integralSquared[n] == TIntegralSquared(0));
	}
#endif

	// we calculate the first row of the integral image

	const T* const sourceFirstRowEnd = source + width * tChannels;
	const T* const sourceEnd = source + (width * tChannels + sourcePaddingElements) * height;

	integral += (width + 1u) * tChannels + integralPaddingElements;
	integralSquared += (width + 1u) * tChannels + integralSquaredPaddingElements;
	const TIntegral* integralPreviousRow = integral;
	const TIntegralSquared* integralSquaredPreviousRow = integralSquared;

	TIntegral previousIntegral[tChannels] = {TIntegral(0)};
	TIntegralSquared previousIntegralSquared[tChannels] = {TIntegralSquared(0)};

	// left pixel integral
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		*integral++ = TIntegral(0);
	}
	// left pixel squared integral
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		*integralSquared++ = TIntegralSquared(0);
	}

	// the remaining pixels of the first row

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source;
			previousIntegralSquared[n] += *source * *source;
			++source;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integralSquared++ = previousIntegralSquared[n];
		}
	}

	source += sourcePaddingElements;
	integral += integralPaddingElements;
	integralSquared += integralSquaredPaddingElements;

	// we calculate the remaining rows

	while (source != sourceEnd)
	{
		const T* const sourceRowEnd = source + width * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegralSquared[n] = TIntegralSquared(0);
		}

		// left pixel integral
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = TIntegral(0);
		}
		// left pixel squared integral
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integralSquared++ = TIntegralSquared(0);
		}

		integralPreviousRow += tChannels;
		integralSquaredPreviousRow += tChannels;

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source;
				previousIntegralSquared[n] += *source * *source;
				++source;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integralSquared++ = previousIntegralSquared[n] + *integralSquaredPreviousRow++;
			}
		}

		source += sourcePaddingElements;

		integral += integralPaddingElements;
		integralSquared += integralSquaredPaddingElements;

		integralPreviousRow += integralPaddingElements;
		integralSquaredPreviousRow += integralSquaredPaddingElements;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createBorderedImage(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(std::is_signed<T>::value == std::is_signed<TIntegral>::value, "The integral image must have the same sign-properties as the source image!");
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u);

	/**
	 * This is the resulting bordered integral image.
	 * Columns or rows with '0' receive a null value.
	 * Rows with '>' receive the last valid integral value from the left
	 * Rows with 'v' receive the last valid integral value from the top
	 *  -------------------------
	 * |0000000000000000000000000|
	 * |0|-----------------------|
	 * |0|     |           |     |
	 * |0|  0  |     0     |  0  |
	 * |0|     |           |     |
	 * |0|-----|-----------|-----|
	 * |0|     |           |  >  |
	 * |0|     |           |  >  |
	 * |0|  0  | Integral  |  >  |
	 * |0|     |           |  >  |
	 * |0|     |           |  >  |
	 * |0|-----|-----------|-----|
	 * |0|     |           |     |
	 * |0|  0  |     V     |  V  |
	 * |0|     |           |     |
	 *  -------------------------
	 */

	const unsigned int integralWidth = width + (border * 2u) + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = integralWidth * tChannels + integralPaddingElements;

	// entire top border (plus the extra zero-row) will be set to zero

	if (integralPaddingElements == 0u)
	{
		memset(integral, 0x00, integralWidth * tChannels * sizeof(TIntegral) * (border + 1u));

#ifdef OCEAN_DEBUG
		for (unsigned int n = 0u; n < integralWidth; ++n)
		{
			ocean_assert(integral[n] == TIntegral(0));
		}
#endif

		integral += integralStrideElements * (border + 1u);
	}
	else
	{
		for (unsigned int y = 0u; y < border + 1u; ++y)
		{
			memset(integral, 0x00, integralWidth * tChannels * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
			for (unsigned int n = 0u; n < integralWidth; ++n)
			{
				ocean_assert(integral[n] == TIntegral(0));
			}
#endif

			integral += integralStrideElements;
		}
	}

	// computing the first row of the integral image

	// setting left border (plus the extra zero-column) to zero
	memset(integral, 0x00, (border + 1u) * tChannels * sizeof(TIntegral));
	integral += (border + 1u) * tChannels;

	const T* const sourceFirstRowEnd = source + tChannels * width;
	const T* const sourceEnd = source + sourceStrideElements * height;
	const TIntegral* integralPreviousRow = integral;

	TIntegral previousIntegral[tChannels] = {0u};

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source++;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// setting right border to last value

	for (unsigned int n = 0u; n < border; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			*integral++ = previousIntegral[c];
		}
	}

	integral += integralPaddingElements;
	source += sourcePaddingElements;

	// computing the following rows of the integral image

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		// setting left border (plus the extra zero-column) to zero
		memset(integral, 0x00, (border + 1u) * tChannels * sizeof(TIntegral));
		integral += (border + 1u) * tChannels;

		const T* const sourceRowEnd = source + tChannels * width;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source++;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// setting right border to last value

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = *(integral - tChannels + n);
		}

		for (unsigned int n = 0u; n < border; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				*integral++ = previousIntegral[c];
			}
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralPreviousRow += (border * 2u + 1u) * tChannels + integralPaddingElements;
	}

	// bottom border will be set to the last column of the integral image

	integralPreviousRow -= (border + 1u) * tChannels;

	for (unsigned int n = 0u; n < border; ++n)
	{
		memcpy(integral, integralPreviousRow, integralWidth * tChannels * sizeof(TIntegral));
		integral += integralStrideElements;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createBorderedImageSquared(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u);

	/**
	 * This is the resulting bordered integral image.
	 * Columns or rows with '0' receive a null value.
	 * Rows with '>' receive the last valid integral value from the left
	 * Rows with 'v' receive the last valid integral value from the top
	 *  --------------------------
	 * |00000000000000000000000000|
	 * |0|------------------------|
	 * |0|     |            |     |
	 * |0|  0  |     0      |  0  |
	 * |0|     |            |     |
	 * |0|-----|------------|-----|
	 * |0|     |            |  >  |
	 * |0|     |            |  >  |
	 * |0|  0  | Integral^2 |  >  |
	 * |0|     |            |  >  |
	 * |0|     |            |  >  |
	 * |0|-----|------------|-----|
	 * |0|     |            |     |
	 * |0|  0  |     V      |  V  |
	 * |0|     |            |     |
	 *  --------------------------
	 */

	const unsigned int integralWidth = width + (border * 2u) + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = integralWidth * tChannels + integralPaddingElements;

	// entire top border (plus the extra zero-row) will be set to zero

	if (integralPaddingElements == 0u)
	{
		memset(integral, 0x00, integralWidth * tChannels * sizeof(TIntegral) * (border + 1u));

#ifdef OCEAN_DEBUG
		for (unsigned int n = 0u; n < integralWidth; ++n)
		{
			ocean_assert(integral[n] == TIntegral(0));
		}
#endif

		integral += integralStrideElements * (border + 1u);
	}
	else
	{
		for (unsigned int y = 0u; y < border + 1u; ++y)
		{
			memset(integral, 0x00, integralWidth * tChannels * sizeof(TIntegral));

#ifdef OCEAN_DEBUG
			for (unsigned int n = 0u; n < integralWidth; ++n)
			{
				ocean_assert(integral[n] == TIntegral(0));
			}
#endif

			integral += integralStrideElements;
		}
	}

	// computing the first row of the integral image

	// setting left border (plus the extra zero-column) to zero
	memset(integral, 0x00, (border + 1u) * tChannels * sizeof(TIntegral));
	integral += (border + 1u) * tChannels;

	const T * const sourceFirstRowEnd = source + tChannels * width;
	const T * const sourceEnd = source + sourceStrideElements * height;
	const TIntegral * integralPreviousRow = integral;

	TIntegral previousIntegral[tChannels] = {TIntegral(0)};

	while (source != sourceFirstRowEnd)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += *source * *source;
			source++;
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// setting right border to last value

	for (unsigned int n = 0u; n < border; ++n)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			*integral++ = previousIntegral[c];
		}
	}

	integral += integralPaddingElements;
	source += sourcePaddingElements;

	// computing the following rows of the integral image

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		// setting left border (plus the extra zero-column) to zero
		memset(integral, 0x00, (border + 1u) * tChannels * sizeof(TIntegral));
		integral += (border + 1u) * tChannels;

		const T * const sourceRowEnd = source + tChannels * width;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		while (source != sourceRowEnd)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += *source * *source;
				source++;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// setting right border to last value

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = *(integral - tChannels + n);
		}

		for (unsigned int n = 0u; n < border; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				*integral++ = previousIntegral[c];
			}
		}

		source += sourcePaddingElements;
		integral += integralPaddingElements;
		integralPreviousRow += (border * 2u + 1u) * tChannels + integralPaddingElements;
	}

	// bottom border will be set to the last column of the integral image

	integralPreviousRow -= (border + 1u) * tChannels;

	for (unsigned int n = 0u; n < border; ++n)
	{
		memcpy(integral, integralPreviousRow, integralWidth * tChannels * sizeof(TIntegral));
		integral += integralStrideElements;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createBorderedImageMirror(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u && border <= min(width, height));

	/**
	 * This is the resulting bordered integral image.
	 * Columns or rows with '0' receive a null value.
	 *  -------------------------
	 * |0000000000000000000000000|
	 * |0|-----------------------|
	 * |0|     |           |     |
	 * |0|  m  |  mirrored |  m  |
	 * |0|     |           |     |
	 * |0|-----|-----------|-----|
	 * |0|     |           |     |
	 * |0|     |           |     |
	 * |0|  m  | Integral  |  m  |
	 * |0|     |           |     |
	 * |0|     |           |     |
	 * |0|-----|-----------|-----|
	 * |0|     |           |     |
	 * |0|  m  |  mirrored |  m  |
	 * |0|     |           |     |
	 *  -------------------------
	 */

	const unsigned int integralWidth = width + border * 2u + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = integralWidth * tChannels + integralPaddingElements;

	// entire first row (plus the extra zero-column) will be set to zero
	memset(integral, 0, integralWidth * sizeof(TIntegral) * tChannels);
	integral += integralStrideElements;

	const TIntegral* integralPreviousRow = integral + tChannels;
	TIntegral previousIntegral[tChannels] = {TIntegral(0)};

	const T* sourcePtr = source + sourceStrideElements * (border - 1u);

	// setting first column to zero

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		integral[n] = TIntegral(0);
	}
	integral += tChannels;

	// first row left border

	for (unsigned int x = (border - 1u); x != (unsigned int)(-1); --x)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(sourcePtr[x * tChannels + n]);
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// first row center pixels

	const T* const sourceRowEnd0 = sourcePtr + width * tChannels;
	while (sourcePtr != sourceRowEnd0)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(*sourcePtr++);
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// first row right border

	for (unsigned int x = 0u; x < border; ++x)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(*(sourcePtr - int((x + 1u) * tChannels) + int(n)));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	integral += integralPaddingElements;

	// following rows

	int y = -int(border) + 1;

	while (y != int(height + border))
	{
		if (y < 0)
		{
			ocean_assert(-y - 1 >= 0 && -y - 1 < int(border));
			sourcePtr = source + int(sourceStrideElements) * (-y - 1);
		}
		else if (y < int(height))
		{
			sourcePtr = source + int(sourceStrideElements) * y;
		}
		else
		{
			sourcePtr = source + int(sourceStrideElements) * (2 * int(height) - y - 1);
		}

		// left column

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = TIntegral(0);
		}

		// left border

		for (unsigned int x = border - 1u; x != (unsigned int)(-1); --x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(sourcePtr[x * tChannels + n]);
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// center

		const T* const followingSourceRowEnd0 = sourcePtr + width * tChannels;
		while (sourcePtr != followingSourceRowEnd0)
		{
			ocean_assert(sourcePtr < followingSourceRowEnd0);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(*sourcePtr++);
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// right border

		for (unsigned int x = 0u; x < border; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(*(sourcePtr - int((x + 1u) * tChannels) + int(n)));
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		integral += integralPaddingElements;
		integralPreviousRow += tChannels + integralPaddingElements;

		++y;
	}
}

template <typename T, typename TIntegral, unsigned int tChannels>
void IntegralImage::createBorderedImageSquaredMirror(const T* source, TIntegral* integral, const unsigned int width, const unsigned int height, const unsigned int border, const unsigned int sourcePaddingElements, const unsigned int integralPaddingElements)
{
	static_assert(sizeof(T) <= sizeof(TIntegral), "Invalid integral elements!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && integral != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(border >= 1u && border <= min(width, height));

	/**
	 * This is the resulting bordered integral image.
	 * Columns or rows with '0' receive a null value.
	 *  -------------------------------
	 * |0000000000000000000000000000000|
	 * |0|-----------------------------|
	 * |0|       |             |       |
	 * |0|  m^2  |  mirrored^2 |  m^2  |
	 * |0|       |             |       |
	 * |0|-------|-------------|-------|
	 * |0|       |             |       |
	 * |0|       |             |       |
	 * |0|  m^2  | Integral^2  |  m^2  |
	 * |0|       |             |       |
	 * |0|       |             |       |
	 * |0|-------|-------------|-------|
	 * |0|       |             |       |
	 * |0|  m^2  |  mirrored^2 |  m^2  |
	 * |0|       |             |       |
	 *  -------------------------------
	 */

	const unsigned int integralWidth = width + border * 2u + 1u;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int integralStrideElements = integralWidth * tChannels + integralPaddingElements;

	// entire first row (plus the extra zero-column) will be set to zero
	memset(integral, 0, integralWidth * sizeof(TIntegral) * tChannels);
	integral += integralStrideElements;

	const TIntegral* integralPreviousRow = integral + tChannels;
	TIntegral previousIntegral[tChannels] = {TIntegral(0)};

	const T* sourcePtr = source + sourceStrideElements * (border - 1u);

	// setting first column to zero

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		integral[n] = TIntegral(0);
	}
	integral += tChannels;

	// first row left border

	for (unsigned int x = (border - 1u); x != (unsigned int)(-1); --x)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(sqr<T, TIntegral>(sourcePtr[x * tChannels + n]));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// first row center pixels

	const T* const sourceRowEnd0 = sourcePtr + width * tChannels;
	while (sourcePtr != sourceRowEnd0)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(sqr<T, TIntegral>(*sourcePtr++));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	// first row right border

	for (unsigned int x = 0u; x < border; ++x)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] += TIntegral(sqr<T, TIntegral>(*(sourcePtr - int((x + 1u) * tChannels) + int(n))));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = previousIntegral[n];
		}
	}

	integral += integralPaddingElements;

	// following rows

	int y = -int(border) + 1;

	while (y != int(height + border))
	{
		if (y < 0)
		{
			ocean_assert(-y - 1 >= 0 && -y - 1 < int(border));
			sourcePtr = source + int(sourceStrideElements) * (-y - 1);
		}
		else if (y < int(height))
		{
			sourcePtr = source + int(sourceStrideElements) * y;
		}
		else
		{
			sourcePtr = source + int(sourceStrideElements) * (2 * int(height) - y - 1);
		}

		// left column

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousIntegral[n] = TIntegral(0);
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			*integral++ = TIntegral(0);
		}

		// left border

		for (unsigned int x = border - 1u; x != (unsigned int)(-1); --x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(sqr<T, TIntegral>(sourcePtr[x * tChannels + n]));
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// center

		const T* const followingSourceRowEnd0 = sourcePtr + width * tChannels;
		while (sourcePtr != followingSourceRowEnd0)
		{
			ocean_assert(sourcePtr < followingSourceRowEnd0);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(sqr<T, TIntegral>(*sourcePtr++));
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		// right border

		for (unsigned int x = 0u; x < border; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				previousIntegral[n] += TIntegral(sqr<T, TIntegral>(*(sourcePtr - int((x + 1u) * tChannels) + int(n))));
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*integral++ = previousIntegral[n] + *integralPreviousRow++;
			}
		}

		integral += integralPaddingElements;
		integralPreviousRow += tChannels + integralPaddingElements;

		++y;
	}
}

template <typename TIntegral>
inline TIntegral IntegralImage::linedIntegralSum(const TIntegral* const linedIntegral, const unsigned int linedIntegralStrideElements, const unsigned int windowLeft, const unsigned int windowTop, const unsigned int windowWidth, const unsigned int windowHeight)
{
	ocean_assert(linedIntegral != nullptr);
	ocean_assert(windowLeft + windowWidth < linedIntegralStrideElements);
	ocean_assert(windowWidth != 0u);
	ocean_assert(windowHeight != 0u);

	const unsigned int windowRight = windowLeft + windowWidth;
	const unsigned int windowBottom = windowTop + windowHeight;

	return linedIntegral[windowTop * linedIntegralStrideElements + windowLeft] - linedIntegral[windowTop * linedIntegralStrideElements + windowRight]
				- linedIntegral[windowBottom * linedIntegralStrideElements + windowLeft] + linedIntegral[windowBottom * linedIntegralStrideElements + windowRight];
}

template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean>
inline TVariance IntegralImage::linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowLeft, const unsigned int windowTop, const unsigned int windowWidth, const unsigned int windowHeight, TVariance* mean)
{
	static_assert(std::is_floating_point<TVariance>::value, "Invalid TVariance must be a floating point data type!");

	ocean_assert(linedIntegral != nullptr);
	ocean_assert(windowLeft + windowWidth < integralStrideElements);

	ocean_assert(linedIntegralSquared != nullptr);
	ocean_assert(windowLeft + windowWidth < integralStrideSquaredElements);

	ocean_assert(windowWidth != 0u);
	ocean_assert(windowHeight != 0u);

	if (windowWidth == 1u && windowHeight == 1u)
	{
		return TVariance(0);
	}

	const TIntegral sum = linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowLeft, windowTop, windowWidth, windowHeight);
	const TIntegralSquared squaredSum = linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowLeft, windowTop, windowWidth, windowHeight);

	const TVariance size = TVariance(windowWidth * windowHeight);

	if constexpr (tReturnMean)
	{
		ocean_assert(mean != nullptr);
		*mean = TVariance(sum) / size;
	}
	else
	{
		ocean_assert(mean == nullptr);
	}

	const TVariance variance = (TVariance(squaredSum) - TVariance(TIntegralSquared(sum) * TIntegralSquared(sum)) / size) / size;

	return std::max(TVariance(0), variance); // due to floating point precision, always ensure that the variance is non-negative
}

template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean>
inline TVariance IntegralImage::linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowALeft, const unsigned int windowATop, const unsigned int windowAWidth, const unsigned int windowAHeight, const unsigned int windowBLeft, const unsigned int windowBTop, const unsigned int windowBWidth, const unsigned int windowBHeight, TVariance* mean)
{
	static_assert(std::is_floating_point<TVariance>::value, "Invalid TVariance must be a floating point data type!");

	ocean_assert(linedIntegral != nullptr);
	ocean_assert(windowALeft + windowAWidth < integralStrideElements);
	ocean_assert(windowBLeft + windowBWidth < integralStrideElements);

	ocean_assert(linedIntegralSquared != nullptr);
	ocean_assert(windowALeft + windowAWidth < integralStrideSquaredElements);
	ocean_assert(windowBLeft + windowBWidth < integralStrideSquaredElements);

	ocean_assert(windowAWidth != 0u && windowAHeight != 0u);
	ocean_assert(windowBWidth != 0u && windowBHeight != 0u);

	const TIntegral sumA = linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowALeft, windowATop, windowAWidth, windowAHeight);
	const TIntegral sumB = linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowBLeft, windowBTop, windowBWidth, windowBHeight);

	const TIntegralSquared squaredSumA = linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowALeft, windowATop, windowAWidth, windowAHeight);
	const TIntegralSquared squaredSumB = linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowBLeft, windowBTop, windowBWidth, windowBHeight);

	const unsigned int sizeA = windowAWidth * windowAHeight;
	const unsigned int sizeB = windowBWidth * windowBHeight;
	const TVariance size = TVariance(sizeA + sizeB);

	const TVariance squaredSum = TVariance(squaredSumA + squaredSumB);
	const TIntegralSquared sum = TIntegralSquared(sumA + sumB);

	if constexpr (tReturnMean)
	{
		ocean_assert(mean != nullptr);
		*mean = TVariance(sum) / size;
	}
	else
	{
		ocean_assert(mean == nullptr);
	}

	const TVariance variance = (squaredSum - TVariance(sum * sum) / size) / size;

	return std::max(TVariance(0), variance); // due to floating point precision, always ensure that the variance is non-negative
}

template <typename TIntegral, typename TIntegralSquared, typename TVariance, bool tReturnMean>
inline TVariance IntegralImage::linedIntegralVariance(const TIntegral* linedIntegral, const TIntegralSquared* linedIntegralSquared, const unsigned int integralStrideElements, const unsigned int integralStrideSquaredElements, const unsigned int windowALeft, const unsigned int windowATop, const unsigned int windowAWidth, const unsigned int windowAHeight, const unsigned int windowBLeft, const unsigned int windowBTop, const unsigned int windowBWidth, const unsigned int windowBHeight, const unsigned int windowCLeft, const unsigned int windowCTop, const unsigned int windowCWidth, const unsigned int windowCHeight, TVariance* mean)
{
	static_assert(std::is_floating_point<TVariance>::value, "Invalid TVariance must be a floating point data type!");

	ocean_assert(linedIntegral != nullptr);
	ocean_assert(windowALeft + windowAWidth < integralStrideElements);
	ocean_assert(windowBLeft + windowBWidth < integralStrideElements);
	ocean_assert(windowCLeft + windowCWidth < integralStrideElements);

	ocean_assert(linedIntegralSquared != nullptr);
	ocean_assert(windowALeft + windowAWidth < integralStrideSquaredElements);
	ocean_assert(windowBLeft + windowBWidth < integralStrideSquaredElements);
	ocean_assert(windowCLeft + windowCWidth < integralStrideSquaredElements);

	ocean_assert(windowAWidth != 0u && windowAHeight != 0u);
	ocean_assert(windowBWidth != 0u && windowBHeight != 0u);
	ocean_assert(windowCWidth != 0u && windowCHeight != 0u);

	const TIntegral sumA = CV::IntegralImage::linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowALeft, windowATop, windowAWidth, windowAHeight);
	const TIntegral sumB = CV::IntegralImage::linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowBLeft, windowBTop, windowBWidth, windowBHeight);
	const TIntegral sumC = CV::IntegralImage::linedIntegralSum<TIntegral>(linedIntegral, integralStrideElements, windowCLeft, windowCTop, windowCWidth, windowCHeight);

	const TIntegralSquared squaredSumA = CV::IntegralImage::linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowALeft, windowATop, windowAWidth, windowAHeight);
	const TIntegralSquared squaredSumB = CV::IntegralImage::linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowBLeft, windowBTop, windowBWidth, windowBHeight);
	const TIntegralSquared squaredSumC = CV::IntegralImage::linedIntegralSum<TIntegralSquared>(linedIntegralSquared, integralStrideSquaredElements, windowCLeft, windowCTop, windowCWidth, windowCHeight);

	const unsigned int sizeA = windowAWidth * windowAHeight;
	const unsigned int sizeB = windowBWidth * windowBHeight;
	const unsigned int sizeC = windowCWidth * windowCHeight;
	const TVariance size = TVariance(sizeA + sizeB + sizeC);

	const TVariance squaredSum = TVariance(squaredSumA + squaredSumB + squaredSumC);
	const TIntegralSquared sum = TIntegralSquared(sumA + sumB + sumC);

	if constexpr (tReturnMean)
	{
		ocean_assert(mean != nullptr);
		*mean = TVariance(sum) / size;
	}
	else
	{
		ocean_assert(mean == nullptr);
	}

	const TVariance variance = (squaredSum - TVariance(sum * sum) / size) / size;

	return std::max(TVariance(0), variance); // due to floating point precision, always ensure that the variance is non-negative
}

template <typename T, typename TSquared>
inline TSquared IntegralImage::sqr(const T& value)
{
	return TSquared(value * value);
}

}

}

#endif // META_OCEAN_CV_INTEGRAL_IMAGE_H
