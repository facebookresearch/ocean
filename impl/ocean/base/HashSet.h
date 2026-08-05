/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_HASH_SET_H
#define META_OCEAN_BASE_HASH_SET_H

#include "ocean/base/Base.h"

#include <vector>

namespace Ocean
{

/**
 * This class implements a hash set.
 * @tparam T The data type that is stored by the hash set
 * @ingroup base
 */
template <typename T>
class HashSet
{
	protected:

		/**
		 * Definition of a pair combining a counter states and an object.
		 */
		using Element = typename std::pair<std::pair<size_t, size_t>, T>;

		/**
		 * Definition of a vector holding the set objects.
		 */
		using Elements = std::vector<Element>;

		/**
		 * Definition of a function pointer returning a hash set value.
		 */
		using ValueFunction = size_t (*)(const T& element);

	public:

		/**
		 * Copy constructor.
		 * @param hashSet The hash set to copy
		 */
		inline HashSet(const HashSet<T>& hashSet);

		/**
		 * Move constructor.
		 * @param hashSet The hash set to move
		 */
		inline HashSet(HashSet<T>&& hashSet) noexcept;

		/**
		 * Creates a new hash set object by a given capacity.
		 * @param capacity Maximal capacity the hash set will support
		 * @param function Hash function to be used
		 */
		explicit HashSet(const size_t capacity, const ValueFunction& function = defaultHashFunction);

		/**
		 * Adds a new element to this hash set.
		 * @param element Element to be added
		 * @param oneOnly Adds the element if it does not exist already
		 * @param extendCapacity True, to extend the capacity if necessary
		 * @return True, if the element has been added
		 */
		bool insert(const T& element, const bool oneOnly = true, const bool extendCapacity = true);

		/**
		 * Adds (moves) a new element to this hash set.
		 * @param element Element to be moved
		 * @param oneOnly Adds the element if it does not exist already
		 * @param extendCapacity True, to extend the capacity if necessary
		 * @return True, if the element has been added
		 */
		bool insert(T&& element, const bool oneOnly = true, const bool extendCapacity = true);

		/**
		 * Removes an element from this hash set.
		 * @param element Element to be removed
		 * @return True, if succeeded
		 */
		bool remove(const T& element);

		/**
		 * Returns whether this hash set holds a given element.
		 * @param element Element to be checked
		 * @return True, if succeeded
		 */
		bool find(const T& element) const;

		/**
		 * Removes all elements from this has set.
		 */
		void clear();

		/**
		 * Returns the number of elements this hash set currently holds.
		 * @return Number of elements
		 */
		inline size_t size() const;

		/**
		 * Returns the capacity of this hash set.
		 * @return Maximal capacity this hash set supports
		 */
		inline size_t capacity() const;

		/**
		 * Returns whether this hash set is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Assign operator.
		 * @param hashSet The hash set to assign
		 * @return The reference to this object
		 */
		inline HashSet<T>& operator=(const HashSet<T>& hashSet);

		/**
		 * Move operator.
		 * @param hashSet The hash set to move
		 * @return The reference to this object
		 */
		inline HashSet<T>& operator=(HashSet<T>&& hashSet) noexcept;

	protected:

		/**
		 * Creates a new hash set by a given hash set.
		 * @param capacity The capacity of the new has set, with range [hashSet.size(), infinity)
		 * @param hashSet The hash set which defines the initial values of this hash set, will be moved
		 */
		HashSet(const size_t capacity, HashSet<T>&& hashSet);

		/**
		 * Default hash function for elements supporting an cast size_t cast.
		 * @param element Element to return the hash value for
		 * @return Resulting hash value
		 */
		static inline size_t defaultHashFunction(const T& element);

		/**
		 * Returns whether this hash set is still consistent.
		 * @return True, if so
		 */
		bool isConsistent() const;

	protected:

		/// Hash set elements.
		Elements elements_;

		/// Number of elements this has set holds.
		size_t size_ = 0;

		/// Value function.
		ValueFunction function_ = nullptr;
};

template <typename T>
inline HashSet<T>::HashSet(const HashSet<T>& hashSet) :
	elements_(hashSet.elements_),
	size_(hashSet.size_),
	function_(hashSet.function_)
{
	// nothing to do here
}

template <typename T>
inline HashSet<T>::HashSet(HashSet<T>&& hashSet) noexcept :
	elements_(std::move(hashSet.elements_)),
	size_(hashSet.size_),
	function_(hashSet.function_)
{
	hashSet.size_ = 0;
}

template <typename T>
HashSet<T>::HashSet(const size_t capacity, const ValueFunction& function) :
	elements_(capacity),
	size_(0),
	function_(function)
{
	ocean_assert(isConsistent());
}

template <typename T>
HashSet<T>::HashSet(const size_t capacity, HashSet<T>&& hashSet) :
	elements_(capacity),
	size_(0),
	function_(hashSet.function_)
{
	ocean_assert(capacity >= hashSet.size());

	for (Element& hashSetElement : hashSet.elements_)
	{
		if (hashSetElement.first.first != 0)
		{
			T& element = hashSetElement.second;

			// duplicate elements are preserved, and this set is already large enough for all elements
			insert(std::move(element), false /*oneOnly*/, false /*extendCapacity*/);
		}
	}

	ocean_assert(size() == hashSet.size());
	ocean_assert(isConsistent());

	hashSet.clear();
}

template <typename T>
bool HashSet<T>::insert(const T& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash set (we extend the set if more than 80% is occupied)
	if (extendCapacity && size_ >= elements_.size() * 80 / 100)
	{
		*this = HashSet<T>(max(size_t(32), elements_.size() * 2), std::move(*this));
		ocean_assert(size_ < elements_.size() * 80 / 100);
	}

	if (size_ == elements_.size())
	{
		return false;
	}

	if (oneOnly)
	{
		// linear search
		for (size_t n = 0; n < elements_.size(); ++n)
		{
			const size_t value = (function_(element) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second = element;

				++size_;
				return true;
			}
			else if (elements_[value].second == element)
			{
				// undo the counts added while probing towards the already existing element
				const size_t startIndex = function_(element);

				for (size_t i = 0; i < n; ++i)
				{
					elements_[(startIndex + i) % elements_.size()].first.first--;
				}

				return false;
			}
			else
			{
				elements_[value].first.first++;
			}
		}
	}
	else
	{
		// linear search
		for (size_t n = 0; n < elements_.size(); ++n)
		{
			const size_t value = (function_(element) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second = element;

				++size_;
				return true;
			}
			else
			{
				elements_[value].first.first++;
			}
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename T>
bool HashSet<T>::insert(T&& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash set (we extend the set if more than 80% is occupied)
	if (extendCapacity && size_ >= elements_.size() * 80 / 100)
	{
		*this = HashSet<T>(max(size_t(32), elements_.size() * 2), std::move(*this));
		ocean_assert(size_ < elements_.size() * 80 / 100);
	}

	if (size_ == elements_.size())
	{
		return false;
	}

	if (oneOnly)
	{
		// linear search
		for (size_t n = 0; n < elements_.size(); ++n)
		{
			const size_t value = (function_(element) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second = std::move(element);

				++size_;
				return true;
			}
			else if (elements_[value].second == element)
			{
				// undo the counts added while probing towards the already existing element
				const size_t startIndex = function_(element);

				for (size_t i = 0; i < n; ++i)
				{
					elements_[(startIndex + i) % elements_.size()].first.first--;
				}

				return false;
			}
			else
			{
				elements_[value].first.first++;
			}
		}
	}
	else
	{
		// linear search
		for (size_t n = 0; n < elements_.size(); ++n)
		{
			const size_t value = (function_(element) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second = std::move(element);

				++size_;
				return true;
			}
			else
			{
				elements_[value].first.first++;
			}
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename T>
bool HashSet<T>::remove(const T& element)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(element) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this place has no shift problem
		if (elements_[value].first.first == 1)
		{
			if (elements_[value].second == element)
			{
				// the slots this element was probed past no longer carry it
				const size_t startIndex = function_(element);

				for (size_t i = 0; i < n; ++i)
				{
					elements_[(startIndex + i) % elements_.size()].first.first--;
				}

				elements_[value].first.first = 0;
				elements_[value].second = T();
				--size_;

				ocean_assert(isConsistent());

				return true;
			}

			// the element is not the element to be removed, but also there is no other position for this element
			return false;
		}

		ocean_assert(elements_[value].first.first > 1);

		size_t elementOffset = 0u;

		if (elements_[value].second == element)
		{
			// the element exists however, the following elements needs a special handling

			size_t localValue = value;
			size_t endLocation = elements_.size();

			while (true)
			{
				size_t lastOffset = 0;

				// find last element to swap
				for (size_t i = 1; i < endLocation; ++i)
				{
					const size_t testValue = (localValue + i) % elements_.size();

					if (elements_[testValue].first.first >= 1)
					{
						if (elements_[testValue].first.second >= i)
						{
							lastOffset = i;
						}
					}

					if (elements_[testValue].first.first <= 1)
					{
						break;
					}
				}

				if (lastOffset == 0)
				{
					break;
				}

				ocean_assert(endLocation >= lastOffset);
				endLocation -= lastOffset;

				elementOffset += lastOffset;

				const size_t lastValue = (localValue + lastOffset) % elements_.size();

				// move the found element

				// elements_[localValue].first.first stays constant
				elements_[localValue].first.second = elements_[lastValue].first.second - lastOffset;
				elements_[localValue].second = elements_[lastValue].second;

				localValue = lastValue;

				if (elements_[lastValue].first.first == 1)
				{
					break;
				}
			}

			// decrease the used counter
			const size_t startIndex = function_(element);

			for (size_t i = 0u; i < elementOffset + n; ++i)
				elements_[(startIndex + i) % elements_.size()].first.first--;

			elements_[(value + elementOffset) % elements_.size()].first.first = 0;
			elements_[(value + elementOffset) % elements_.size()].second = T();
			--size_;

			ocean_assert(isConsistent());

			return true;
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename T>
bool HashSet<T>::find(const T& element) const
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(element) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this element is equal to the given one
		if (elements_[value].second == element)
		{
			return true;
		}

		// check whether this place is not free but unique
		if (elements_[value].first.first == 1)
		{
			return false;
		}
	}

	return false;
}

template <typename T>
void HashSet<T>::clear()
{
	ocean_assert(isConsistent());

	for (Element& element : elements_)
	{
		element.first.first = 0;
	}

	size_ = 0;

	ocean_assert(isConsistent());
}

template <typename T>
inline size_t HashSet<T>::size() const
{
	return size_;
}

template <typename T>
inline size_t HashSet<T>::capacity() const
{
	return elements_.size();
}

template <typename T>
inline bool HashSet<T>::isEmpty() const
{
	return size_ == 0;
}

template <typename T>
inline HashSet<T>& HashSet<T>::operator=(const HashSet<T>& hashSet)
{
	if (this != &hashSet)
	{
		elements_ = hashSet.elements_;
		size_ = hashSet.size_;
		function_ = hashSet.function_;
	}

	return *this;
}

template <typename T>
inline HashSet<T>& HashSet<T>::operator=(HashSet<T>&& hashSet) noexcept
{
	if (this != &hashSet)
	{
		elements_ = std::move(hashSet.elements_);
		size_ = hashSet.size_;
		function_ = hashSet.function_;

		hashSet.size_ = 0;
	}

	return *this;
}

template <typename T>
inline size_t HashSet<T>::defaultHashFunction(const T& element)
{
	return size_t(element);
}

template <typename T>
bool HashSet<T>::isConsistent() const
{
	size_t count = 0;

	// every element contributes one count to each slot of its probe sequence, so both sums must match
	size_t slotCounters = 0;
	size_t probeSequenceLengths = 0;

	for (const Element& element : elements_)
	{
		slotCounters += element.first.first;

		if (element.first.first != 0)
		{
			++count;
			probeSequenceLengths += element.first.second + 1;
		}
	}

	return count == size_ && slotCounters == probeSequenceLengths;
}

}

#endif // META_OCEAN_BASE_HASH_SET_H
