/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_H
#define META_OCEAN_CV_FRAME_FILTER_H

#include "ocean/cv/CV.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements the base class for all filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilter
{
	public:

		/**
		 * Determines the per-pixel magnitude of a frame.
		 * @param frame The input frame for which the magnitude will be determined, must be valid
		 * @param magnitude The resulting magnitude frame, must be valid
		 * @param width The width of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param height The height of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param magnitudePaddingElements The number of padding elements at the end of each magnitude row, in elements, with range [0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @tparam T The data type of the frame pixel channel
		 * @tparam TMagnitude The data type of the magnitude, ensure that the magnitude of 'T' (over all channels) fits into 'TMagnitude'
		 */
		template <typename T, typename TMagnitude>
		static void magnitude(const T* frame, TMagnitude* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes a given value with a template-based normalization factor.
		 * This function will create a wrong rounded normalization result for extremely large 32bit and 64bit integers if the value is within tNormalizationDenominator/2 to the value range.<br>
		 * Thus, for integers and if 'tRoundedNormalization == true', the value range for valid rounded normalization results is:
		 * <pre>
		 *  int32_t: [-2147483648 + tNormalizationDenominator/2, 2147483647 - tNormalizationDenominator/2]
		 * uint32_t: [0, 4294967295 - tNormalizationDenominator/2]

		 *  int64_t: [-9223372036854775808 + tNormalizationDenominator/2, 9223372036854775807 - tNormalizationDenominator/2]
		 * uint64_t: [0, 18446744073709551615 - tNormalizationDenominator/2]
		 * </pre>
		 * @param value The value to be normalized
		 * @return The normalized value 'value / tNormalization'
		 * @tparam T The data type of the value to normalize, either an integer value or a floating point value
		 * @tparam tNormalizationDenominator The normalization factor to be applied, with range [1, infinity)
		 * @tparam tRoundedNormalization True, to apply a rounded normalization; False, to apply a normalization without rounding (ignored for floating point values)
		 * @see normalizeValueSlow().
		 */
		template <typename T, T tNormalizationDenominator, bool tRoundedNormalization>
		static inline T normalizeValue(const T& value);

		/**
		 * Normalizes a given value with a template-based normalization factor.
		 * This function does not provide wrong normalization results for extremely large 32bit and 64bit integers if 'tRoundedNormalization == true', but is also slower.
		 * @param value The value to be normalized
		 * @return The normalized value 'value / tNormalization'
		 * @tparam T The data type of the value to normalize, either an integer value or a floating point value
		 * @tparam tNormalizationDenominator The normalization factor to be applied, with range [1, infinity)
		 * @tparam tRoundedNormalization True, to apply a rounded normalization; False, to apply a normalization without rounding (ignored for floating point values)
		 * @see normalizeValue().
		 */
		template <typename T, T tNormalizationDenominator, bool tRoundedNormalization>
		static inline T normalizeValueSlow(const T& value);

	protected:

		/**
		 * Determines the per-pixel magnitude for a subset of a frame.
		 * @param frame The input frame for which the magnitude will be determined, must be valid
		 * @param magnitude The resulting magnitude frame, must be valid
		 * @param width The width of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param height The height of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param magnitudePaddingElements The number of padding elements at the end of each magnitude row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T The data type of the frame pixel channel
		 * @tparam TMagnitude The data type of the magnitude, ensure that the magnitude of 'T' (over all channels) fits into 'TMagnitude'
		 */
		template <typename T, typename TMagnitude>
		static void magnitudeSubset(const T* frame, TMagnitude* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Determines the per-pixel magnitude for a subset of a frame.
		 * This function applies a lookup-based sqrt calculation.
		 * @param frame The input frame for which the magnitude will be determined, must be valid
		 * @param magnitude The resulting magnitude frame, must be valid
		 * @param width The width of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param height The height of the input (and magnitude) frame, in pixels, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param magnitudePaddingElements The number of padding elements at the end of each magnitude row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		static void magnitude2Channels8BitPerChannelSubset(const uint8_t* frame, uint16_t* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T, typename TMagnitude>
void FrameFilter::magnitude(const T* frame, TMagnitude* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, Worker* worker)
{
	static_assert(sizeof(T) <= sizeof(TMagnitude), "Invalid data type!");

	ocean_assert(frame != nullptr);
	ocean_assert(magnitude != nullptr);
	ocean_assert(channels >= 2u);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&magnitudeSubset<T, TMagnitude>, frame, magnitude, channels, width, height, framePaddingElements, magnitudePaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		magnitudeSubset<T, TMagnitude>(frame, magnitude, channels, width, height, framePaddingElements, magnitudePaddingElements, 0u, height);
	}
}

template <typename T, T tNormalizationDenominator, bool tRoundedNormalization>
inline T FrameFilter::normalizeValue(const T& value)
{
	static_assert(tNormalizationDenominator > T(0), "Invalid normalization!");

	if constexpr (tNormalizationDenominator == T(1))
	{
		return value;
	}

	if constexpr (std::is_floating_point<T>::value)
	{
		return value / tNormalizationDenominator;
	}
	else
	{
		constexpr T tNormalizationDenominator_2 = tNormalizationDenominator / T(2);

#ifdef OCEAN_DEBUG
		ocean_assert((std::is_integral<T>::value));

		if constexpr (tRoundedNormalization)
		{
			if constexpr (std::is_same<T, int32_t>::value)
			{
				ocean_assert(value >= NumericT<int32_t>::minValue() + tNormalizationDenominator_2);
				ocean_assert(value <= NumericT<int32_t>::maxValue() - tNormalizationDenominator_2);
			}

			if constexpr (std::is_same<T, uint32_t>::value)
			{
				ocean_assert(value <= NumericT<uint32_t>::maxValue() - tNormalizationDenominator_2);
			}

			if constexpr (std::is_same<T, int64_t>::value)
			{
				ocean_assert(value >= NumericT<int64_t>::minValue() + tNormalizationDenominator_2);
				ocean_assert(value <= NumericT<int64_t>::maxValue() - tNormalizationDenominator_2);
			}

			if constexpr (std::is_same<T, uint64_t>::value)
			{
				ocean_assert(value <= NumericT<uint64_t>::maxValue() - tNormalizationDenominator_2);
			}
		}
#endif // OCEAN_DEBUG

		if constexpr (tRoundedNormalization && std::is_signed<T>::value)
		{
			if (value >= T(0))
			{
				return (value + tNormalizationDenominator_2) / tNormalizationDenominator;
			}
			else
			{
				return (value - tNormalizationDenominator_2) / tNormalizationDenominator;
			}
		}
		else
		{
			if constexpr (tRoundedNormalization)
			{
				return (value + tNormalizationDenominator_2) / tNormalizationDenominator;
			}
			else
			{
				return value / tNormalizationDenominator;
			}
		}
	}
}

template <typename T, T tNormalizationDenominator, bool tRoundedNormalization>
inline T FrameFilter::normalizeValueSlow(const T& value)
{
	if constexpr (std::is_integral<T>::value && tNormalizationDenominator != T(1) && tRoundedNormalization && sizeof(T) >= 4)
	{
		constexpr T tNormalizationDenominator_2 = (tNormalizationDenominator + 1) / T(2);

		if constexpr (std::is_signed<T>::value)
		{
			if (value < T(0))
			{
				const T remainder = value % tNormalizationDenominator;

				if (remainder <= -tNormalizationDenominator_2)
				{
					return value / tNormalizationDenominator - T(1);
				}
				else
				{
					return value / tNormalizationDenominator;
				}
			}

		}

		const T remainder = value % tNormalizationDenominator;

		if (remainder >= tNormalizationDenominator_2)
		{
			return value / tNormalizationDenominator + T(1);
		}
		else
		{
			return value / tNormalizationDenominator;
		}
	}
	else
	{
		return normalizeValue<T, tNormalizationDenominator, tRoundedNormalization>(value);
	}
}

template <typename T, typename TMagnitude>
void FrameFilter::magnitudeSubset(const T* frame, TMagnitude* magnitude, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int magnitudePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(sizeof(T) <= sizeof(TMagnitude), "Invalid data type!");

	ocean_assert(frame != nullptr);
	ocean_assert(magnitude != nullptr);
	ocean_assert(channels >= 2u);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	if constexpr (std::is_same<T, uint8_t>::value && std::is_same<TMagnitude, uint16_t>::value)
	{
		if (channels == 2u)
		{
			magnitude2Channels8BitPerChannelSubset(frame, magnitude, channels, width, height, framePaddingElements, magnitudePaddingElements, firstRow, numberRows);
			return;
		}
	}

	using TSqrMagnitude = typename SquareValueTyper<TMagnitude>::Type;
	using TIntermediateFloat = typename FloatTyper<TMagnitude>::Type;

	const unsigned int frameStrideElements = width * channels + framePaddingElements;
	const unsigned int magnitudeStrideElements = width + magnitudePaddingElements;

	frame += firstRow * frameStrideElements;
	magnitude += firstRow * magnitudeStrideElements;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			TSqrMagnitude sqrMagnitude = TSqrMagnitude(0);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				sqrMagnitude += TSqrMagnitude(frame[n]) * TSqrMagnitude(frame[n]);
			}

			const TIntermediateFloat floatMagnitude = NumericT<TIntermediateFloat>::sqrt(TIntermediateFloat(sqrMagnitude));

			if constexpr (std::is_floating_point<TMagnitude>::value)
			{
				ocean_assert(NumericT<TMagnitude>::isInsideValueRange(floatMagnitude));
			}
			else
			{
				ocean_assert(TIntermediateFloat(NumericT<TMagnitude>::minValue()) <= floatMagnitude);
				ocean_assert(floatMagnitude < TIntermediateFloat(NumericT<TMagnitude>::maxValue()));
			}

			*magnitude = TMagnitude(floatMagnitude);

			frame += channels;
			++magnitude;
		}

		frame += framePaddingElements;
		magnitude += magnitudePaddingElements;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_H
