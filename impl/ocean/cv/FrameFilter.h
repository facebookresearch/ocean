// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
