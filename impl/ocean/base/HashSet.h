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
		typedef typename std::pair<std::pair<size_t, size_t>, T> Element;

		/**
		 * Definition of a vector holding the set objects.
		 */
		typedef std::vector<Element> Elements;

		/**
		 * Definition of a function pointer returning a hash set value.
		 */
		typedef size_t (*ValueFunction)(const T& element);

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
		HashSet(size_t capacity, HashSet<T>&& hashSet);

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
		Elements setElements;

		/// Number of elements this has set holds.
		size_t setSize;

		/// Value function.
		ValueFunction setFunction;
};

template <typename T>
inline HashSet<T>::HashSet(const HashSet<T>& hashSet) :
	setElements(hashSet.setElements),
	setSize(hashSet.setSize),
	setFunction(hashSet.setFunction)
{
	// nothing to do here
}

template <typename T>
inline HashSet<T>::HashSet(HashSet<T>&& hashSet) noexcept :
	setElements(std::move(hashSet.setElements)),
	setSize(hashSet.setSize),
	setFunction(hashSet.setFunction)
{
	hashSet.setSize = 0;
}

template <typename T>
HashSet<T>::HashSet(const size_t capacity, const ValueFunction& function) :
	setElements(capacity),
	setSize(0),
	setFunction(function)
{
	ocean_assert(isConsistent());
}

template <typename T>
HashSet<T>::HashSet(size_t capacity, HashSet<T>&& hashSet) :
	setElements(capacity),
	setSize(0),
	setFunction(hashSet.setFunction)
{
	ocean_assert(capacity >= hashSet.size());

	for (typename Elements::iterator i = hashSet.setElements.begin(); i != hashSet.setElements.end(); ++i)
		if (i->first.first != 0)
		{
			T& element = i->second;
			insert(std::move(element));
		}

	ocean_assert(size() == hashSet.size());
	ocean_assert(isConsistent());

	hashSet.clear();
}

template <typename T>
bool HashSet<T>::insert(const T& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(setSize <= setElements.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash set (we extend the set if more than 80% is occupied)
	if (extendCapacity && setSize >= setElements.size() * 80 / 100)
	{
		*this = HashSet<T>(max(size_t(32), setElements.size() * 2), std::move(*this));
		ocean_assert(setSize < setElements.size() * 80 / 100);
	}

	if (setSize == setElements.size())
		return false;

	if (oneOnly)
	{
		// linear search
		for (size_t n = 0; n < setElements.size(); ++n)
		{
			const size_t value = (setFunction(element) + n) % setElements.size();

			// check whether the place is free
			if (setElements[value].first.first == 0)
			{
				setElements[value].first.first = 1;
				setElements[value].first.second = n;
				setElements[value].second = element;

				++setSize;
				return true;
			}
			else if (setElements[value].second == element)
				return false;
			else
				setElements[value].first.first++;
		}
	}
	else
	{
		// linear search
		for (size_t n = 0; n < setElements.size(); ++n)
		{
			const size_t value = (setFunction(element) + n) % setElements.size();

			// check whether the place is free
			if (setElements[value].first.first == 0)
			{
				setElements[value].first.first = 1;
				setElements[value].first.second = n;
				setElements[value].second = element;

				++setSize;
				return true;
			}
			else
				setElements[value].first.first++;
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename T>
bool HashSet<T>::insert(T&& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(setSize <= setElements.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash set (we extend the set if more than 80% is occupied)
	if (extendCapacity && setSize >= setElements.size() * 80 / 100)
	{
		*this = HashSet<T>(max(size_t(32), setElements.size() * 2), std::move(*this));
		ocean_assert(setSize < setElements.size() * 80 / 100);
	}

	if (setSize == setElements.size())
		return false;

	if (oneOnly)
	{
		// linear search
		for (size_t n = 0; n < setElements.size(); ++n)
		{
			const size_t value = (setFunction(element) + n) % setElements.size();

			// check whether the place is free
			if (setElements[value].first.first == 0)
			{
				setElements[value].first.first = 1;
				setElements[value].first.second = n;
				setElements[value].second = std::move(element);

				++setSize;
				return true;
			}
			else if (setElements[value].second == element)
				return false;
			else
				setElements[value].first.first++;
		}
	}
	else
	{
		// linear search
		for (size_t n = 0; n < setElements.size(); ++n)
		{
			const size_t value = (setFunction(element) + n) % setElements.size();

			// check whether the place is free
			if (setElements[value].first.first == 0)
			{
				setElements[value].first.first = 1;
				setElements[value].first.second = n;
				setElements[value].second = std::move(element);

				++setSize;
				return true;
			}
			else
				setElements[value].first.first++;
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename T>
bool HashSet<T>::remove(const T& element)
{
	ocean_assert(setSize <= setElements.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < setElements.size(); ++n)
	{
		const size_t value = (setFunction(element) + n) % setElements.size();

		// check whether this place is free
		if (setElements[value].first.first == 0)
			return false;

		// check whether this place has no shift problem
		if (setElements[value].first.first == 1)
		{
			if (setElements[value].second == element)
			{
				setElements[value].first.first = 0;
				setElements[value].second = T();
				--setSize;

				ocean_assert(isConsistent());

				return true;
			}

			// the element is not the element to be removed, but also there is no other position for this element
			return false;
		}

		ocean_assert(setElements[value].first.first > 1);

		size_t elementOffset = 0u;

		if (setElements[value].second == element)
		{
			// the element exists however, the following elements needs a special handling

			size_t localValue = value;
			size_t endLocation = setElements.size();

			while (true)
			{
				size_t lastOffset = 0;

				// find last element to swap
				for (size_t i = 1; i < endLocation; ++i)
				{
					const size_t testValue = (localValue + i) % setElements.size();

					if (setElements[testValue].first.first >= 1)
					{
						if (setElements[testValue].first.second >= i)
							lastOffset = i;
					}

					if (setElements[testValue].first.first <= 1)
						break;
				}

				if (lastOffset == 0)
					break;

				ocean_assert(endLocation >= lastOffset);
				endLocation -= lastOffset;

				elementOffset += lastOffset;

				const size_t lastValue = (localValue + lastOffset) % setElements.size();

				// move the found element

				// setElements[localValue].first.first stays constant
				setElements[localValue].first.second = setElements[lastValue].first.second - lastOffset;
				setElements[localValue].second = setElements[lastValue].second;

				localValue = lastValue;

				if (setElements[lastValue].first.first == 1)
					break;
			}

			// decrease the used counter
			const size_t startIndex = setFunction(element);

			for (size_t i = 0u; i < elementOffset + n; ++i)
				setElements[(startIndex + i) % setElements.size()].first.first--;

			setElements[(value + elementOffset) % setElements.size()].first.first = 0;
			setElements[(value + elementOffset) % setElements.size()].second = T();
			--setSize;

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
	ocean_assert(setSize <= setElements.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < setElements.size(); ++n)
	{
		const size_t value = (setFunction(element) + n) % setElements.size();

		// check whether this place is free
		if (setElements[value].first.first == 0)
			return false;

		// check whether this element is equal to the given one
		if (setElements[value].second == element)
			return true;

		// check whether this place is not free but unique
		if (setElements[value].first.first == 1)
			return false;
	}

	return false;
}

template <typename T>
void HashSet<T>::clear()
{
	ocean_assert(isConsistent());

	for (typename Elements::iterator i = setElements.begin(); i != setElements.end(); ++i)
		i->first.first = 0;

	setSize = 0;

	ocean_assert(isConsistent());
}

template <typename T>
inline size_t HashSet<T>::size() const
{
	return setSize;
}

template <typename T>
inline size_t HashSet<T>::capacity() const
{
	return setElements.size();
}

template <typename T>
inline bool HashSet<T>::isEmpty() const
{
	return setSize == 0;
}

template <typename T>
inline HashSet<T>& HashSet<T>::operator=(const HashSet<T>& hashSet)
{
	if (this != &hashSet)
	{
		setElements = hashSet.setElements;
		setSize = hashSet.setSize;
		setFunction = hashSet.setFunction;
	}

	return *this;
}

template <typename T>
inline HashSet<T>& HashSet<T>::operator=(HashSet<T>&& hashSet) noexcept
{
	if (this != &hashSet)
	{
		setElements = std::move(hashSet.setElements);
		setSize = hashSet.setSize;
		setFunction = hashSet.setFunction;

		hashSet.setSize = 0;
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

	for (typename Elements::const_iterator i = setElements.begin(); i != setElements.end(); ++i)
		if (i->first.first != 0)
			++count;

	return count == setSize;
}

}

#endif // META_OCEAN_BASE_HASH_SET_H
