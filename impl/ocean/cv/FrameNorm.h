/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_NORM_H
#define META_OCEAN_CV_FRAME_NORM_H

#include "ocean/cv/CV.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions allowing to determine norms of images e.g, a L2 norm.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameNorm
{
	public:

		/**
		 * Determines the L2 norm (square root of summed squares) of given data.
		 * This function is actually a wrapper around normL2() - and applies the sqrt calculation in addition.
		 * @param data The data for which the norm value will be determined, must be valid
		 * @param size The size of the given data in elements, with range [1, infinity)
		 * @return The resulting L2 norm
		 * @tparam T Data type of each value, e.g., 'uint8_t' or 'float'
		 * @tparam TNorm The data type of the resulting norm (and the intermediate sum), e.g., 'uint32_t' or 'double'
		 * @see squaredNormL2().
		 */
		template <typename T, typename TNorm>
		static TNorm normL2(const T* data, const size_t size);

		/**
		 * Determines the L2 norm (square root of summed squares) of a given frame allowing to specify a padding to enable the application of e.g., sub-frames.
		 * This function is actually a wrapper around normL2() - and applies the sqrt calculation in addition.
		 * @param frame The data of the frame for which the norm value will be determined, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @return The resulting L2 norm
		 * @tparam T Data type of each value, e.g., 'unsigned char' or 'float'
		 * @tparam TNorm The data type of the resulting norm (and the intermediate sum), e.g., 'unsigned int' or 'double'
		 * @see squaredNormL2().
		 */
		template <typename T, typename TNorm>
		static TNorm normL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements);

		/**
		 * Determines the squared L2 norm (summed squares) of given data.
		 * @param data The data for which the norm value will be determined, must be valid
		 * @param size The size of the given data in elements, with range [1, infinity)
		 * @return The resulting L2 norm
		 * @tparam T Data type of each value, e.g., 'uint8_t' or 'float'
		 * @tparam TNorm The data type of the resulting norm (and the intermediate sum), e.g., 'uint32_t' or 'double'
		 * @see normL2().
		 */
		template <typename T, typename TNorm>
		static TNorm squaredNormL2(const T* data, const size_t size);

		/**
		 * Determines the squared L2 norm (summed squares) of a given frame allowing to specify a padding to enable the application of e.g., sub-frames.
		 * @param frame The data of the frame for which the norm value will be determined, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @return The resulting L2 norm
		 * @tparam T Data type of each value, e.g., 'unsigned char' or 'float'
		 * @tparam TNorm The data type of the resulting norm (and the intermediate sum), e.g., 'unsigned int' or 'double'
		 * @see normL2().
		 */
		template <typename T, typename TNorm>
		static TNorm squaredNormL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements);
};

template <typename T, typename TNorm>
TNorm FrameNorm::normL2(const T* data, const size_t size)
{
	ocean_assert(data != nullptr);
	ocean_assert(size != 0);

	return NumericT<TNorm>::sqrt(squaredNormL2<T, TNorm>(data, size));
}

template <typename T, typename TNorm>
TNorm FrameNorm::normL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	return NumericT<TNorm>::sqrt(squaredNormL2<T, TNorm>(frame, width, height, framePaddingElements));
}

template <typename T, typename TNorm>
TNorm FrameNorm::squaredNormL2(const T* data, const size_t size)
{
	ocean_assert(data != nullptr);
	ocean_assert(size != 0);

	// we will calculate four values in parallel
	// thus, the compiler will have a direct hint to use SSE/NEON
	// so that we can avoid to make individual implementations...

	TNorm result0 = TNorm(0);
	TNorm result1 = TNorm(0);
	TNorm result2 = TNorm(0);
	TNorm result3 = TNorm(0);

	ocean_assert(NumericT<ptrdiff_t>::isInsideValueRange(size));

	ptrdiff_t n = 0;

	for (n = 0; n < ptrdiff_t(size) - 3; n += 4)
	{
		result0 += TNorm(data[n + 0] * data[n + 0]);
		result1 += TNorm(data[n + 1] * data[n + 1]);
		result2 += TNorm(data[n + 2] * data[n + 2]);
		result3 += TNorm(data[n + 3] * data[n + 3]);
	}

	TNorm result = result0 + result1 + result2 + result3;

	// now we have to handle the last (at most) three elements

	while (n < ptrdiff_t(size))
	{
		result += TNorm(data[n] * data[n]);

		++n;
	}

	return result;
}

template <typename T, typename TNorm>
TNorm FrameNorm::squaredNormL2(const T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (framePaddingElements == 0u)
	{
		return squaredNormL2<T, TNorm>(frame, width * height);
	}

	// we will calculate four values in parallel
	// thus, the compiler will have a direct hint to use SSE/NEON
	// so that we can avoid to make individual implementations...

	TNorm result0 = TNorm(0);
	TNorm result1 = TNorm(0);
	TNorm result2 = TNorm(0);
	TNorm result3 = TNorm(0);

	ptrdiff_t x;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (x = 0; x < ptrdiff_t(width) - 3; x += 4)
		{
			result0 += TNorm(frame[x + 0] * frame[x + 0]);
			result1 += TNorm(frame[x + 1] * frame[x + 1]);
			result2 += TNorm(frame[x + 2] * frame[x + 2]);
			result3 += TNorm(frame[x + 3] * frame[x + 3]);
		}

		if (x + 0 < ptrdiff_t(width))
		{
			result0 += TNorm(frame[x + 0] * frame[x + 0]);
		}

		if (x + 1 < ptrdiff_t(width))
		{
			result1 += TNorm(frame[x + 1] * frame[x + 1]);
		}

		if (x + 2 < ptrdiff_t(width))
		{
			result2 += TNorm(frame[x + 2] * frame[x + 2]);
		}

		frame += width + framePaddingElements;
	}

	return result0 + result1 + result2 + result3;
}

}

}

#endif // META_OCEAN_CV_FRAME_NORM_H
