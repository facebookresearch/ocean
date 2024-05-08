/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_SORTED_H
#define META_OCEAN_CV_FRAME_FILTER_SORTED_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Median.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements the base class for all filters relying on sorted filter values.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterSorted
{
	protected:

		/**
		 * This class implements a histogram for integer values.
		 * @tparam T The data type of the values for which the median needs to be determined
		 * @tparam TBin The internal data type for the histogram bins
		 * @tparam tSize The size of the histogram, with range [1, infinity)
		 */
		template <typename T, typename TBin, unsigned int tSize>
		class HistogramInteger
		{
			public:

				/// The data type of the histogram values.
				typedef T Type;

			public:

				/**
				 * Returns the current number of values in the histogram.
				 * @return The histogram's values
				 */
				inline size_t values() const;

				/**
				 * Pushes a new value to the histogram.
				 * @param value The value to be pushed
				 */
				inline void pushValue(const T& value);

				/**
				 * Pops a value from the histogram.
				 * @param value The value to pop
				 * @see hasValue().
				 */
				inline void popValue(const T& value);

				/**
				 * Returns the minimal value of this histogram.
				 * @return The histogram's min value
				 */
				inline T minValue() const;

				/**
				 * Returns the median value of this histogram.
				 * @return The histogram's median value
				 */
				inline T medianValue() const;

				/**
				 * Returns the maximal value of this histogram.
				 * @return The histogram's max value
				 */
				inline T maxValue() const;

				/**
				 * Returns whether this histogram contains a specific value.
				 * @param value The value to check
				 * @return True, if so
				 */
				inline bool hasValue(const T& value) const;

				/**
				 * Returns whether this histogram holds at least one value.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// The histogram's bins.
				TBin bins_[tSize] = {0u};

				/// The number of values in the histogram.
				size_t values_ = 0;
		};

		/**
		 * This class implements a container holding sorted elements.
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		class SortedElements
		{
			protected:

				/**
				 * Definition of a vector holding the elements.
				 */
				typedef std::vector<T> Elements;

			public:

				/**
				 * Creates a new object.
				 * @param capacity The expected number of elements this object will hold at the same time, with range [0, infinity)
				 */
				inline explicit SortedElements(const size_t capacity = 64);

				/**
				 * Pushes a new value into the container.
				 * @param value The value to be pushed
				 */
				inline void pushValue(const T& value);

				/**
				 * Pops a value from the container.
				 * @param value The value to pop
				 */
				inline void popValue(const T& value);

				/**
				 * Exchanges a value with another value.
				 * @param pushValue The value to be pushed
				 * @param popValue The value to be popped
				 */
				inline void exchange(const T& pushValue, const T& popValue);

				/**
				 * Returns the minimal value of this histogram.
				 * @return The histogram's min value
				 */
				inline T minValue() const;

				/**
				 * Returns the median value of this histogram.
				 * @return The histogram's median value
				 */
				inline T medianValue() const;

				/**
				 * Returns the maximal value of this histogram.
				 * @return The histogram's max value
				 */
				inline T maxValue() const;

				/**
				 * Returns the number of elements this container currently holds.
				 * @return The number of elements, with range [0, infinity)
				 */
				inline size_t size() const;

			protected:

				/// The elements, stored in ascending order.
				Elements elements_;
		};

	protected:

		/**
		 * Returns the lower clamped offset to an index.
		 * @param index The index from which the offset will be subtracted, with range [0, infinity)
		 * @param lowerOffset The lower offset to be applied, with range [0, infinity)
		 * @return The clamped offset: max(0, index - lowerOffset)
		 */
		static inline unsigned int clampLower(const unsigned int index, const unsigned int lowerOffset);

		/**
		 * Returns the upper clamped offset to an index
		 * @param index The index to which the offset will be added, with range [0, infinity)
		 * @param upperOffset The upper offset to be applied, with range [0, infinity)
		 * @param size The number of maximal elements, with range [1, infinity)
		 * @return The clamped offset: min(index + upperOffset, size - 1)
		 */
		static inline unsigned int clampUpper(const unsigned int index, const unsigned int upperOffset, const unsigned int size);
};

template <typename T, typename TBin, unsigned int tSize>
inline size_t FrameFilterSorted::HistogramInteger<T, TBin, tSize>::values() const
{
	return values_;
}

template <typename T, typename TBin, unsigned int tSize>
inline void FrameFilterSorted::HistogramInteger<T, TBin, tSize>::pushValue(const T& value)
{
	ocean_assert((unsigned int)(value) < tSize);

	ocean_assert(uint64_t(bins_[value]) < uint64_t(NumericT<TBin>::maxValue()));
	++bins_[value];

	++values_;
}

template <typename T, typename TBin, unsigned int tSize>
inline void FrameFilterSorted::HistogramInteger<T, TBin, tSize>::popValue(const T& value)
{
	ocean_assert((unsigned int)(value) < tSize);

	ocean_assert(bins_[value] != 0u);
	--bins_[value];

	ocean_assert(values_ != 0);
	--values_;
}

template <typename T, typename TBin, unsigned int tSize>
inline T FrameFilterSorted::HistogramInteger<T, TBin, tSize>::minValue() const
{
	ocean_assert(values_ != 0);

	for (unsigned int nBin = 0u; nBin < tSize; ++nBin)
	{
		if (bins_[nBin] != TBin(0))
		{
			return T(nBin);
		}
	}

	ocean_assert(false && "This should never happen!");
	return T(0);
}

template <typename T, typename TBin, unsigned int tSize>
inline T FrameFilterSorted::HistogramInteger<T, TBin, tSize>::medianValue() const
{
	constexpr unsigned int blockSize = 8u;

	static_assert(tSize % blockSize == 0u, "Invalid size");

	ocean_assert(values_ != 0);
	const unsigned int values_2 = (unsigned int)(values_ - 1) / 2u; // -1 in case histogram holds even number of elements

	unsigned int counter = 0u;
	unsigned int binIndex = 0u;

	for (unsigned int nBlock = 0u; nBlock < tSize / blockSize; ++nBlock)
	{
		unsigned int blockCounter = 0u;

		for (unsigned int n = 0u; n < blockSize; ++n)
		{
			blockCounter += bins_[nBlock * blockSize + n];
		}

		if (counter + blockCounter < values_2)
		{
			counter += blockCounter;
			binIndex += blockSize;
			continue;
		}

		while (binIndex < tSize && counter <= values_2)
		{
			counter += bins_[binIndex];
			++binIndex;
		}

		break;
	}

	ocean_assert(NumericT<T>::isInsideValueRange(binIndex - 1u));

	return T(binIndex - 1u);
}

template <typename T, typename TBin, unsigned int tSize>
inline T FrameFilterSorted::HistogramInteger<T, TBin, tSize>::maxValue() const
{
	ocean_assert(values_ != 0);

	for (unsigned int nBin = tSize - 1u; nBin < tSize; --nBin)
	{
		if (bins_[nBin] != TBin(0))
		{
			return T(nBin);
		}
	}

	ocean_assert(false && "This should never happen!");
	return T(0);
}

template <typename T, typename TBin, unsigned int tSize>
inline bool FrameFilterSorted::HistogramInteger<T, TBin, tSize>::hasValue(const T& value) const
{
	ocean_assert((unsigned int)(value) < tSize);

	return bins_[value] != 0u;
}

template <typename T, typename TBin, unsigned int tSize>
inline FrameFilterSorted::HistogramInteger<T, TBin, tSize>::operator bool() const
{
	return values_ != 0;
}

template <typename T>
inline FrameFilterSorted::SortedElements<T>::SortedElements(const size_t capacity)
{
	elements_.reserve(capacity);
}

template <typename T>
inline void FrameFilterSorted::SortedElements<T>::pushValue(const T& value)
{
	const typename Elements::const_iterator iElement = std::lower_bound(elements_.cbegin(), elements_.cend(), value);
	ocean_assert(iElement == elements_.cend() || value <= *iElement);

	elements_.insert(iElement, value);
}

template <typename T>
inline void FrameFilterSorted::SortedElements<T>::popValue(const T& value)
{
	const typename Elements::const_iterator iElement = std::lower_bound(elements_.cbegin(), elements_.cend(), value);
	ocean_assert(value == *iElement);

	elements_.erase(iElement);
}

template <typename T>
inline void FrameFilterSorted::SortedElements<T>::exchange(const T& pushValue, const T& popValue)
{
	if (pushValue == popValue)
	{
		return;
	}

	const typename Elements::const_iterator iPushElement = std::lower_bound(elements_.cbegin(), elements_.cend(), pushValue);
	ocean_assert(iPushElement == elements_.cend() || pushValue <= *iPushElement);

	if (pushValue < popValue)
	{
		const typename Elements::const_iterator iPopElement = std::lower_bound(iPushElement, elements_.cend(), popValue);
		ocean_assert(popValue == *iPopElement);

		const size_t pushIndex = iPushElement - elements_.cbegin();
		const size_t popIndex = iPopElement - elements_.cbegin();

		ocean_assert(pushIndex <= popIndex && popIndex < elements_.size());

		T previousTmp = elements_[pushIndex];
		elements_[pushIndex] = pushValue;

		for (size_t n = pushIndex + 1; n <= popIndex; ++n)
		{
			T currentTmp = elements_[n];
			elements_[n] = previousTmp;

			previousTmp = currentTmp;
		}
	}
	else
	{
		// pushValue >= popValue

		const typename Elements::const_iterator iPopElement = std::lower_bound(elements_.cbegin(), iPushElement, popValue);
		ocean_assert(popValue == *iPopElement);

		const size_t popIndex = iPopElement - elements_.cbegin();
		const size_t pushEnd = iPushElement - elements_.cbegin();

		ocean_assert(popIndex < pushEnd && pushEnd <= elements_.size());

		for (size_t n = popIndex + 1; n < pushEnd; ++n)
		{
			elements_[n - 1] = elements_[n];
		}

		elements_[pushEnd - 1] = pushValue;
	}
}

template <typename T>
inline T FrameFilterSorted::SortedElements<T>::minValue() const
{
	ocean_assert(!elements_.empty());

	return elements_.front();
}

template <typename T>
inline T FrameFilterSorted::SortedElements<T>::medianValue() const
{
	ocean_assert(!elements_.empty());
	const size_t index = size_t(elements_.size() - 1) / size_t(2); // -1 in case histogram holds even number of elements

	return elements_[index];
}

template <typename T>
inline T FrameFilterSorted::SortedElements<T>::maxValue() const
{
	ocean_assert(!elements_.empty());

	return elements_.back();
}

template <typename T>
inline size_t FrameFilterSorted::SortedElements<T>::size() const
{
	return elements_.size();
}

inline unsigned int FrameFilterSorted::clampLower(const unsigned int index, const unsigned int lowerOffset)
{
	return (unsigned int)(std::max(0, int(index) - int(lowerOffset)));
}

inline unsigned int FrameFilterSorted::clampUpper(const unsigned int index, const unsigned int upperOffset, const unsigned int size)
{
	ocean_assert(size >= 1u);

	return std::min(index + upperOffset, size - 1u);
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_SORTED_H
