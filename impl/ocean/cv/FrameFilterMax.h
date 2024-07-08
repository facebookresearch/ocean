/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_MAX_H
#define META_OCEAN_CV_FRAME_FILTER_MAX_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameFilterSorted.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements filters based on the max function.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterMax : protected FrameFilterSorted
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
				 * Filters a frame with a max filter with arbitrary size (a square patch).
				 * @param source The source image to be filtered, must be valid
				 * @param target The target frame with same size and pixel format receiving the filtered result, must be valid
				 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool filter(const Frame& source, Frame& target, const unsigned int filterSize, Worker* worker);

				/**
				 * Filters a frame with a max filter with arbitrary size (a square patch).
				 * @param frame The image to be filtered, must be valid
				 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool filter(Frame& frame, const unsigned int filterSize, Worker* worker);
		};

	public:

		/**
		 * Filters a frame with a max filter with arbitrary size (a square patch).
		 * @param source The source image to be filtered, must be valid
		 * @param target The target frame with same size and pixel format receiving the filtered result, must be valid
		 * @param width The width of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param height The height of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of the data elements
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, Worker* worker = nullptr);

		/**
		 * Filters a frame with a max filter with arbitrary size (a square patch).
		 * @param frame The image to be filtered, must be valid
		 * @param width The width of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param height The height of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of the data elements
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void filter(T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int filterSize, Worker* worker = nullptr);

	protected:

		/**
		 * Filters a subset of an integer frame with a max filter with arbitrary size.
		 * @param source The source image to be filtered, must be valid
		 * @param target The target frame with same size and pixel format receiving the filtered result, must be valid
		 * @param width The width of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param height The height of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of the data elements, must be an integer data type
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels, typename THistogram>
		static void filterHistogramSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Filters a subset of a floating point frame with a max filter with arbitrary size.
		 * @param source The source image to be filtered, must be valid
		 * @param target The target frame with same size and pixel format receiving the filtered result, must be valid
		 * @param width The width of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param height The height of the input frame in pixel, with range [filterSize / 2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param filterSize Size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam T Data type of the data elements, must be a valid data type
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void filterSequentialSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T, unsigned int tChannels>
void FrameFilterMax::filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr && source != target);
	ocean_assert(filterSize / 2u <= width && filterSize / 2u <= height);

	if constexpr (std::is_floating_point<T>::value || sizeof(T) > sizeof(uint16_t))
	{
		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&filterSequentialSubset<T, tChannels>, source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, 0u), 0u, height, 7u, 8u, 20u);
		}
		else
		{
			filterSequentialSubset<T, tChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, height);
		}
	}
	else if (filterSize < 5u)
	{
		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&filterSequentialSubset<T, tChannels>, source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, 0u), 0u, height, 7u, 8u, 20u);
		}
		else
		{
			filterSequentialSubset<T, tChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, height);
		}
	}
	else
	{
		ocean_assert(!std::is_floating_point<T>::value);

		ocean_assert(uint64_t(filterSize * filterSize) < uint64_t(NumericT<uint16_t>::maxValue()));

		constexpr size_t histogramElements = 1 << sizeof(T) * 8;

		ocean_assert(sizeof(T) != sizeof(uint8_t) || histogramElements == 256);
		ocean_assert(sizeof(T) != sizeof(uint16_t) || histogramElements == 65536);

		typedef HistogramInteger<T, uint16_t, histogramElements> Histogram;

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&filterHistogramSubset<T, tChannels, Histogram>, source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, 0u), 0u, height, 7u, 8u, 20u);
		}
		else
		{
			filterHistogramSubset<T, tChannels, Histogram>(source, target, width, height, sourcePaddingElements, targetPaddingElements, filterSize, 0u, height);
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameFilterMax::filter(T* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int filterSize, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(filterSize / 2u <= width && filterSize / 2u <= height);

	Memory memory(width * height * sizeof(T) * tChannels);

	constexpr unsigned int memoryPaddingElements = 0u;
	filter<T, tChannels>(frame, memory.data<T>(), width, height, framePaddingElements, memoryPaddingElements, filterSize, worker);

	if (framePaddingElements == 0u)
	{
		memcpy(frame, memory.data(), memory.size());
	}
	else
	{
		const T* memoryData = memory.data<T>();

		for (unsigned int y = 0u; y < height; ++y)
		{
			memcpy(frame, memoryData, width * tChannels * sizeof(T));

			frame += width * tChannels + framePaddingElements;
			memoryData += width * tChannels + memoryPaddingElements;
		}
	}
}

template <typename T, unsigned int tChannels, typename THistogram>
void FrameFilterMax::filterHistogramSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");
	static_assert(tChannels != 0u, "Invalid channel number");

	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(filterSize_2 <= width && filterSize_2 <= height);

	const unsigned int endRow = firstRow + numberRows;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	THistogram histogram[tChannels];

	// fill initial histogram

	for (unsigned int y = clampLower(firstRow, filterSize_2); y <= clampUpper(firstRow, filterSize_2, height); ++y)
	{
		const T* sourceRow = source + y * sourceStrideElements;

		for (unsigned int x = 0u; x <= filterSize_2; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				histogram[n].pushValue(*sourceRow++);
			}
		}
	}

	ocean_assert(histogram[0].values() >= (filterSize_2 + 1u) * (filterSize_2 + 1u));
	ocean_assert(histogram[0].values() <= (filterSize_2 + 1u) * filterSize);

	THistogram previousHistograms[tChannels];

	for (unsigned int y = firstRow; y < endRow; ++y)
	{
		T* targetRow = target + y * targetStrideElements;

		// making a copy which we can use when we start with the next row
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			previousHistograms[n] = histogram[n];
		}

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				ocean_assert(histogram[n]);
				*targetRow++ = histogram[n].maxValue();
			}

			if (x != width - 1u)
			{
				ocean_assert(x + 1u < width);

				// horizontal histogram update

				for (unsigned int yy = clampLower(y, filterSize_2); yy <= clampUpper(y, filterSize_2, height); ++yy)
				{
					const T* sourceRow = source + yy * sourceStrideElements;

					const unsigned int xxLeft = x - filterSize_2;
					const unsigned int xxRight = x + filterSize_2 + 1u;

					if (xxLeft < width) // handling negative cases: int(xxLeft) < 0
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const T popValue = sourceRow[xxLeft * tChannels + n];

							ocean_assert(histogram[n].hasValue(popValue));
							histogram[n].popValue(popValue);
						}
					}

					if (xxRight < width)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							const T pushValue = sourceRow[xxRight * tChannels + n];

							histogram[n].pushValue(pushValue);
						}
					}
				}
			}

			ocean_assert(histogram[0].values() <= filterSize * filterSize);
		}

		if (y != endRow - 1u)
		{
			ocean_assert(y + 1u < endRow);

			// vertical histogram update at the beginning of the row

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				histogram[n] = previousHistograms[n];
			}

			const unsigned int yyTop = y - filterSize_2;
			const unsigned int yyBottom = y + filterSize_2 + 1u;

			if (yyTop < height) // handling negative cases: int(yyTop) < 0
			{
				const T* sourceRow = source + yyTop * sourceStrideElements;

				for (unsigned int x = 0u; x <= filterSize_2; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						const T popValue = sourceRow[x * tChannels + n];

						ocean_assert(histogram[n].hasValue(popValue));
						histogram[n].popValue(popValue);
					}
				}
			}

			if (yyBottom < height)
			{
				const T* sourceRow = source + yyBottom * sourceStrideElements;

				for (unsigned int x = 0u; x <= filterSize_2; ++x)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						const T pushValue = sourceRow[x * tChannels + n];

						histogram[n].pushValue(pushValue);
					}
				}
			}
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameFilterMax::filterSequentialSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int filterSize, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	const unsigned int filterSize_2 = filterSize / 2u;

	T maxValues[tChannels];

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		T* targetRow = target + y * targetStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				maxValues[n] = NumericT<T>::minValue();
			}

			for (unsigned int xx = max(0, int(x) - int(filterSize_2)); xx < min(x + filterSize_2 + 1u, width); ++xx)
			{
				for (unsigned int yy = max(0, int(y) - int(filterSize_2)); yy < min(y + filterSize_2 + 1u, height); ++yy)
				{
					ocean_assert(xx < width && yy < height);

					const T* const sourcePixel = source + yy * sourceStrideElements + xx * tChannels;

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						if (sourcePixel[n] > maxValues[n])
						{
							maxValues[n] = sourcePixel[n];
						}
					}
				}
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				targetRow[n] = maxValues[n];
			}

			targetRow += tChannels;
		}
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_MAX_H
