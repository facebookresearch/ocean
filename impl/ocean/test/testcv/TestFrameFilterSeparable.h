/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SEPARABLE_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SEPARABLE_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the separable filter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterSeparable
{
	friend class TestFrameFilterGaussian;

	public:

		/**
		 * Tests all separable filter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests extreme (small) frame dimensions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testExtremeDimensions(const double testDuration, Worker& worker);

		/**
		 * Tests extreme (small) frame dimensions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testExtremeDimensions(const double testDuration, Worker& worker);

		/**
		 * Tests the performance of the frame filter for normal frame dimensions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testNormalDimensions(const double testDuration, Worker& worker);

		/**
		 * Tests the performance of the frame filter when using a reusable memory object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testReusableMemory(const double testDuration);

		/**
		 * Tests the performance of the frame filter when using a reusable memory object.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'uint8_t', or 'float'
		 */
		template <typename T>
		static bool testReusableMemoryComfort(const double testDuration);

		/**
		 * Tests the filter for frame with 8 bit per channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [tFilterSize, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char', or 'float'
		 * @tparam TFilter The data type of each filter value, e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker);

		/**
		 * Tests the universal separable filter function supporting arbitrary data types for extreme image resolutions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testSeparableFilterUniversalExtremeResolutions(const double testDuration, Worker& worker);

		/**
		 * Tests the universal separable filter function supporting arbitrary data types.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testSeparableFilterUniversal(const double testDuration, Worker& worker);

		/**
		 * Tests the universal separable filter function supporting arbitrary data types.
		 * @param width The width of the test frame, with range [1, infinity)
		 * @param height The height of the test frame, with range [1, infinity)
		 * @param channels The number of channels the test frame has, with range [1, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static bool testSeparableFilterUniversal(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const double testDuration, Worker& worker);

	protected:

		/**
		 * Creates a randomized 1D filer with specified size.
		 * Filters with floating point precision will be normalized so that the sum of all elements is equal 1.<br>
		 * Filters with integer precision will not be normalized.
		 * @param randomGenerator The random number generator to be used
		 * @param size The size of the filter (the number of elements), with range [1, infinity), must be odd
		 * @tparam T The data type of each filter element
		 */
		template <typename T>
		static std::vector<T> randomFilter(RandomGenerator& randomGenerator, const unsigned int size);

		/**
		 * Returns a normalized filter for a given filter.
		 * @param filter The filter to normalize
		 * @return The normalized filter, with sum of all filter elements equal 1
		 * @tparam T The data type of all filter elements of the given filter
		 */
		template <typename T>
		static std::vector<float> normalizedFilter(const std::vector<T>& filter);

		/**
		 * Validates the separable filter function for arbitrary data types.
		 * @param source The source frame that has been filtered, must be valid
		 * @param target The target frame that holds the result of the filter, must be valid
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of both frames in pixel, with range [1, infinity)
		 * @param channels Number of the data channels of the source (and target) frame, with range [1, 4]
		 * @param horizontalFilter The (separable) horizontal filter to be applied, the number of filter elements must be odd, at least one element, not more elements than 'width'
		 * @param verticalFilter The (separable) vertical filter to be applied, the number of filter elements must be odd, at least one element, not more elements than 'height'
		 * @param averageAbsError Optional resulting average absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param maximalAbsError Optional resulting maximal absolute error between the converted result and the ground truth result, with range (-infinity, infinity)
		 * @param groundTruth Optional resulting ground truth data (the resized image content determined with floating point accuracy), nullptr otherwise
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param groundTruthPaddingElements Optional number of padding elements at the end of each ground truth row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel channel, e.g., 'float', 'double', 'int', ...
		 */
		template <typename T>
		static void validateFilter(const T* source, const T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const std::vector<float>& horizontalFilter, const std::vector<float>& verticalFilter, double* averageAbsError, double* maximalAbsError, T* groundTruth, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int groundTruthPaddingElements);
};

template <typename T>
std::vector<T> TestFrameFilterSeparable::randomFilter(RandomGenerator& randomGenerator, const unsigned int size)
{
	ocean_assert(size % 2u == 1u);

	std::vector<T> filter(size);

	T filterSum = T(0);
	for (size_t n = 0; n < filter.size(); ++n)
	{
		if constexpr (std::is_same<float, T>::value || std::is_same<double, T>::value)
		{
			filter[n] = T(RandomF::scalar(randomGenerator, 0.001f, 10.0f));
		}
		else
		{
			filter[n] = T(RandomI::random(randomGenerator, 1, 20));
		}

		filterSum += filter[n];
	}

	if constexpr (std::is_same<float, T>::value || std::is_same<double, T>::value)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(filterSum));
		for (size_t n = 0; n < filter.size(); ++n)
		{
			filter[n] /= filterSum;
		}
	}

	return filter;
}

template <typename T>
std::vector<float> TestFrameFilterSeparable::normalizedFilter(const std::vector<T>& filter)
{
	T filterSum = T(0);
	for (size_t n = 0; n < filter.size(); ++n)
	{
		filterSum += filter[n];
	}

	std::vector<float> floatFilter(filter.size());

	ocean_assert(NumericT<T>::isNotEqualEps(filterSum));
	for (size_t n = 0; n < filter.size(); ++n)
	{
		floatFilter[n] = float(filter[n]) / float(filterSum);
	}

	return floatFilter;
}

template <typename T>
void TestFrameFilterSeparable::validateFilter(const T* source, const T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const std::vector<float>& horizontalFilter, const std::vector<float>& verticalFilter, double* averageAbsError, double* maximalAbsError, T* groundTruth, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int groundTruthPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	ocean_assert(horizontalFilter.size() % 2 == 1 && horizontalFilter.size() <= width);
	ocean_assert(verticalFilter.size() % 2 == 1 && verticalFilter.size() <= height);

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;
	const unsigned int groundTruthStrideElements = width * channels + groundTruthPaddingElements;

	if (averageAbsError)
	{
		*averageAbsError = NumericD::maxValue();
	}

	if (maximalAbsError)
	{
		*maximalAbsError = NumericD::maxValue();
	}

	double sumAbsError = 0.0;
	double maxAbsError = 0.0;

	typedef typename FloatTyper<T>::Type TIntermediate;

	Frame intermediateFrame(FrameType(width, height, FrameType::genericPixelFormat<TIntermediate>(channels), FrameType::ORIGIN_UPPER_LEFT));

	// horizontal pass

	const unsigned int horizontalFilter_2 = (unsigned int)horizontalFilter.size() / 2u;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				TIntermediate response = TIntermediate(0);

				for (int xx = -int(horizontalFilter_2); xx <= int(horizontalFilter_2); ++xx)
				{
					const unsigned int filterIndex = xx + int(horizontalFilter_2);
					const unsigned int xSource = mirrorValue(xx + int(x), width);

					const T* sourcePixel = source + y * sourceStrideElements + xSource * channels;

					response += TIntermediate(sourcePixel[c]) * TIntermediate(horizontalFilter[filterIndex]);
				}

				intermediateFrame.pixel<TIntermediate>(x, y)[c] = TIntermediate(response);
			}
		}
	}


	// vertical pass

	std::vector<T> groundTruthPixel(channels);

	const unsigned int verticalFilter_2 = (unsigned int)verticalFilter.size() / 2u;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				TIntermediate response = TIntermediate(0);

				for (int yy = -int(verticalFilter_2); yy <= int(verticalFilter_2); ++yy)
				{
					const unsigned int filterIndex = yy + int(verticalFilter_2);
					const unsigned int ySource = mirrorValue(yy + int(y), height);

					response += intermediateFrame.constpixel<TIntermediate>(x, ySource)[c] * TIntermediate(verticalFilter[filterIndex]);
				}

				groundTruthPixel[c] = T(response);
			}

			const T* const targetPixel = target + y * targetStrideElements + x * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				const double absError = NumericD::abs(double(groundTruthPixel[n]) - double(targetPixel[n]));

				sumAbsError += absError;
				maxAbsError = max(maxAbsError, absError);
			}

			if (groundTruth)
			{
				memcpy(groundTruth + y * groundTruthStrideElements + x * channels, groundTruthPixel.data(), sizeof(T) * channels);
			}
		}
	}

	if (averageAbsError)
	{
		*averageAbsError = sumAbsError / double(width * height * channels);
	}

	if (maximalAbsError)
	{
		*maximalAbsError = maxAbsError;
	}
}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_SEPARABLE_H
