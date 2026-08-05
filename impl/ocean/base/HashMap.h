/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_HASH_MAP_H
#define META_OCEAN_BASE_HASH_MAP_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class implements a hash map.
 * @tparam TKey The data type of the key that is associated with the data element
 * @tparam T The data type of the data elements that are stored in the hash map
 * @ingroup base
 */
template <typename TKey, typename T>
class HashMap
{
	protected:

		/**
		 * Definition of a pair combining a counter states and an object.
		 */
		using Element = typename std::pair<std::pair<size_t, size_t>, std::pair<TKey, T>>;

		/**
		 * Definition of a vector holding the map objects.
		 */
		using Elements = std::vector<Element>;

		/**
		 * Definition of a function pointer returning a hash map value.
		 */
		using ValueFunction = size_t (*)(const TKey& key);

	public:

		/**
		 * Copy constructor.
		 * @param hashMap The hash map to copy
		 */
		inline HashMap(const HashMap<TKey, T>& hashMap);

		/**
		 * Move constructor.
		 * @param hashMap The hash map to move
		 */
		inline HashMap(HashMap<TKey, T>&& hashMap) noexcept;

		/**
		 * Creates a new hash map object by a given capacity.
		 * @param capacity Maximal capacity the hash map will support
		 * @param function Hash function to be used
		 */
		explicit HashMap(const size_t capacity, const ValueFunction& function = defaultHashFunction);

		/**
		 * Adds a new element to this hash map.
		 * @param key Key to be added
		 * @param element Element to be added
		 * @param oneOnly True, to add the element only if it does not exist already
		 * @param extendCapacity True, to extend the capacity if necessary
		 * @return True, if the element has been added
		 */
		bool insert(const TKey& key, const T& element, const bool oneOnly = true, const bool extendCapacity = true);

		/**
		 * Adds (moves) a new element to this hash map.
		 * @param key Key to be moved
		 * @param element Element to be moved
		 * @param oneOnly True, to add the element only if it does not exist already
		 * @param extendCapacity True, to extend the capacity if necessary
		 * @return True, if the element has been added
		 */
		bool insert(TKey&& key, T&& element, const bool oneOnly = true, const bool extendCapacity = true);

		/**
		 * Removes an element from this hash map.
		 * @param key Key to be removed
		 * @return True, if succeeded
		 */
		bool remove(const TKey& key);

		/**
		 * Returns whether this hash map holds a given element.
		 * @param key Key to be found
		 * @return True, if succeeded
		 */
		bool find(const TKey& key) const;

		/**
		 * Returns whether this hash map holds a given element.
		 * @param key Key to be found
		 * @param element Resulting element corresponding to the requested key
		 * @return True, if succeeded
		 */
		bool find(const TKey& key, const T*& element) const;

		/**
		 * Returns whether this hash map holds a given element.
		 * @param key Key to be found
		 * @param element Resulting element corresponding to the requested key
		 * @return True, if succeeded
		 */
		bool find(const TKey& key, T*& element);

		/**
		 * Returns a specific element of this map.
		 * Beware: The element must exist!
		 * @param key The key which belongs to the element
		 * @return The element of the defined key
		 */
		const T& element(const TKey& key) const;

		/**
		 * Removes all elements from this has map.
		 */
		void clear();

		/**
		 * Returns the number of elements this hash map currently holds.
		 * @return Number of elements
		 */
		inline size_t size() const;

		/**
		 * Returns the capacity of this hash map.
		 * @return Maximal capacity this hash map supports
		 */
		inline size_t capacity() const;

		/**
		 * Returns whether this hash map is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Assign operator.
		 * @param hashMap The hash map to assign
		 * @return The reference to this object
		 */
		inline HashMap<TKey, T>& operator=(const HashMap<TKey, T>& hashMap);

		/**
		 * Move operator.
		 * @param hashMap The hash map to move
		 * @return The reference to this object
		 */
		inline HashMap<TKey, T>& operator=(HashMap<TKey, T>&& hashMap) noexcept;

	protected:

		/**
		 * Creates a new hash map by a given hash map.
		 * @param capacity The capacity of the new has map, with range [hashMap.size(), infinity)
		 * @param hashMap The hash map which defines the initial values of this hash map, will be moved
		 */
		HashMap(const size_t capacity, HashMap<TKey, T>&& hashMap);

		/**
		 * Default hash function for elements supporting an cast size_t cast.
		 * @param key Key to return the hash value for
		 * @return Resulting hash value
		 */
		static inline size_t defaultHashFunction(const TKey& key);

		/**
		 * Returns whether this hash map is still consistent.
		 * @return True, if so
		 */
		bool isConsistent() const;

	protected:

		/// Hash map elements.
		Elements elements_;

		/// Number of elements this has map holds.
		size_t size_ = 0;

		/// Value function.
		ValueFunction function_ = nullptr;
};

template <typename TKey, typename T>
inline HashMap<TKey, T>::HashMap(const HashMap<TKey, T>& hashMap) :
	elements_(hashMap.elements_),
	size_(hashMap.size_),
	function_(hashMap.function_)
{
	// nothing to do here
}

template <typename TKey, typename T>
inline HashMap<TKey, T>::HashMap(HashMap<TKey, T>&& hashMap) noexcept :
	elements_(std::move(hashMap.elements_)),
	size_(hashMap.size_),
	function_(hashMap.function_)
{
	hashMap.size_ = 0;
}

template <typename TKey, typename T>
HashMap<TKey, T>::HashMap(const size_t capacity, const ValueFunction& function) :
	elements_(capacity),
	size_(0),
	function_(function)
{
	ocean_assert(isConsistent());
}

template <typename TKey, typename T>
HashMap<TKey, T>::HashMap(const size_t capacity, HashMap<TKey, T>&& hashMap) :
	elements_(capacity),
	size_(0),
	function_(hashMap.function_)
{
	ocean_assert(capacity >= hashMap.size());

	for (Element& hashMapElement : hashMap.elements_)
	{
		if (hashMapElement.first.first != 0)
		{
			TKey& key = hashMapElement.second.first;
			T& element = hashMapElement.second.second;

			// duplicate keys are preserved, and this map is already large enough for all elements
			insert(std::move(key), std::move(element), false /*oneOnly*/, false /*extendCapacity*/);
		}
	}

	ocean_assert(size() == hashMap.size());
	ocean_assert(isConsistent());

	hashMap.clear();
}

template <typename TKey, typename T>
bool HashMap<TKey, T>::insert(const TKey& key, const T& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash map (we extend the map if more than 80% is occupied)
	if (extendCapacity && size_ >= elements_.size() * 80 / 100)
	{
		*this = HashMap<TKey, T>(max(size_t(32), elements_.size() * 2), std::move(*this));
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
			const size_t value = (function_(key) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second.first = key;
				elements_[value].second.second = element;

				++size_;
				return true;
			}
			else if (elements_[value].second.first == key)
			{
				// undo the counts added while probing towards the already existing key
				const size_t startIndex = function_(key);

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
			const size_t value = (function_(key) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second.first = key;
				elements_[value].second.second = element;

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

template <typename TKey, typename T>
bool HashMap<TKey, T>::insert(TKey&& key, T&& element, const bool oneOnly, const bool extendCapacity)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// check whether we have to extend the capacity of this hash map (we extend the map if more than 80% is occupied)
	if (extendCapacity && size_ >= elements_.size() * 80 / 100)
	{
		*this = HashMap<TKey, T>(max(size_t(32), elements_.size() * 2), std::move(*this));
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
			const size_t value = (function_(key) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second.first = std::move(key);
				elements_[value].second.second = std::move(element);

				++size_;
				return true;
			}
			else if (elements_[value].second.first == key)
			{
				// undo the counts added while probing towards the already existing key
				const size_t startIndex = function_(key);

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
			const size_t value = (function_(key) + n) % elements_.size();

			// check whether the place is free
			if (elements_[value].first.first == 0)
			{
				elements_[value].first.first = 1;
				elements_[value].first.second = n;
				elements_[value].second.first = std::move(key);
				elements_[value].second.second = std::move(element);

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

template <typename TKey, typename T>
bool HashMap<TKey, T>::remove(const TKey& key)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(key) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this place has no shift problem
		if (elements_[value].first.first == 1)
		{
			if (elements_[value].second.first == key)
			{
				// the slots this element was probed past no longer carry it
				const size_t startIndex = function_(key);

				for (size_t i = 0; i < n; ++i)
				{
					elements_[(startIndex + i) % elements_.size()].first.first--;
				}

				elements_[value].first.first = 0;
				elements_[value].second.first = TKey();
				elements_[value].second.second = T();
				--size_;

				ocean_assert(isConsistent());

				return true;
			}

			// the element is not the element to be removed, but also there is no other position for this element
			return false;
		}

		ocean_assert(elements_[value].first.first > 1);

		size_t elementOffset = 0u;

		if (elements_[value].second.first == key)
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
			const size_t startIndex = function_(key);

			for (size_t i = 0u; i < elementOffset + n; ++i)
			{
				elements_[(startIndex + i) % elements_.size()].first.first--;
			}

			elements_[(value + elementOffset) % elements_.size()].first.first = 0;
			elements_[(value + elementOffset) % elements_.size()].second.first = TKey();
			elements_[(value + elementOffset) % elements_.size()].second.second = T();
			--size_;

			ocean_assert(isConsistent());

			return true;
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

template <typename TKey, typename T>
bool HashMap<TKey, T>::find(const TKey& key) const
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(key) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this element is equal to the given one
		if (elements_[value].second.first == key)
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

template <typename TKey, typename T>
bool HashMap<TKey, T>::find(const TKey& key, const T*& element) const
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(key) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this element is equal to the given one
		if (elements_[value].second.first == key)
		{
			element = &elements_[value].second.second;
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

template <typename TKey, typename T>
bool HashMap<TKey, T>::find(const TKey& key, T*& element)
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(key) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			return false;
		}

		// check whether this element is equal to the given one
		if (elements_[value].second.first == key)
		{
			element = &elements_[value].second.second;
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

template <typename TKey, typename T>
const T& HashMap<TKey, T>::element(const TKey& key) const
{
	ocean_assert(size_ <= elements_.size());
	ocean_assert(isConsistent());

	// linear search
	for (size_t n = 0; n < elements_.size(); ++n)
	{
		const size_t value = (function_(key) + n) % elements_.size();

		// check whether this place is free
		if (elements_[value].first.first == 0)
		{
			break;
		}

		// check whether this element is equal to the given one
		if (elements_[value].second.first == key)
		{
			return elements_[value].second.second;
		}

		// check whether this place is not free but unique
		if (elements_[value].first.first == 1)
		{
			break;
		}
	}

	ocean_assert(false && "Invalid key!");
	return elements_.cbegin()->second.second;
}

template <typename TKey, typename T>
void HashMap<TKey, T>::clear()
{
	ocean_assert(isConsistent());

	for (Element& element : elements_)
	{
		element.first.first = 0;
	}

	size_ = 0;

	ocean_assert(isConsistent());
}

template <typename TKey, typename T>
inline size_t HashMap<TKey, T>::size() const
{
	return size_;
}

template <typename TKey, typename T>
inline size_t HashMap<TKey, T>::capacity() const
{
	return elements_.size();
}

template <typename TKey, typename T>
inline bool HashMap<TKey, T>::isEmpty() const
{
	return size_ == 0;
}

template <typename TKey, typename T>
inline HashMap<TKey, T>& HashMap<TKey, T>::operator=(const HashMap<TKey, T>& hashMap)
{
	if (this != &hashMap)
	{
		elements_ = hashMap.elements_;
		size_ = hashMap.size_;
		function_ = hashMap.function_;
	}

	return *this;
}

template <typename TKey, typename T>
inline HashMap<TKey, T>& HashMap<TKey, T>::operator=(HashMap<TKey, T>&& hashMap) noexcept
{
	if (this != &hashMap)
	{
		elements_ = std::move(hashMap.elements_);
		size_ = hashMap.size_;
		function_ = hashMap.function_;

		hashMap.size_ = 0;
	}

	return *this;
}

template <typename TKey, typename T>
inline size_t HashMap<TKey, T>::defaultHashFunction(const TKey& key)
{
	return size_t(key);
}

template <typename TKey, typename T>
bool HashMap<TKey, T>::isConsistent() const
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

#endif // META_OCEAN_BASE_HASH_MAP_H
