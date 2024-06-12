/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_RING_MAP_H
#define META_OCEAN_BASE_RING_MAP_H

#include "ocean/base/Base.h"
#include "ocean/base/DataType.h"
#include "ocean/base/Lock.h"

#include <list>

namespace Ocean
{

/**
 * This class implements a data storage map that stores the data elements in a ring manner.
 * The map can hold a maximal number of elements and exchanges the oldest object by a new object if this map is full.<br>
 * Each stored object is connected with a key so that the object can be addressed.<br>
 * @tparam TKey Data type of the map keys
 * @tparam T Data type of the map elements
 * @tparam tThreadsafe True, to create a thread-safe object
 * @tparam tOrderedKeys True, to allow accessing the keys in order; False, if the order of the keys is not of interest
 * @ingroup base
 */
template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys = false>
class RingMapT
{
	template <typename TKey2, typename T2, bool tThreadsafe2, bool tOrderedKeys2> friend class RingMapT;

	public:

		/**
		 * The data type of the objects that are stored in this container.
		 */
		typedef T Type;

		/**
		 * The data type of the keys that are used to address the data objects.
		 */
		typedef TKey TypeKey;

		/**
		 * Definition of individual element access modes.
		 */
		enum AccessMode : uint32_t
		{
			/// The element's key must be a perfect match.
			AM_MATCH = 0u,
			/// The element with highest key is returned if no perfect match can be found, only if 'tOrderedKeys == true'.
			AM_MATCH_OR_HIGHEST,
			/// The element with lowest key is returned if no perfect match can be found, only if 'tOrderedKeys == true'.
			AM_MATCH_OR_LOWEST
		};

	protected:

		/**
		 * Definition of a double-linked list holding the keys in order how they have been inserted.
		 */
		using KeyList = std::list<TKey>;

		/**
		 * Definition of a pair combining a value with a list iterator.
		 */
		using ValuePair = std::pair<T, typename KeyList::iterator>;

		/**
		 * Definition of a map that maps keys to value pairs.
		 */
		typedef typename MapTyper<tOrderedKeys>::template TMap<TKey, ValuePair> KeyMap;

	public:

		/**
		 * Creates a new ring storage object with no capacity.
		 */
		RingMapT() = default;

		/**
		 * Move constructor.
		 * @param ringMap Object to be moved
		 */
		inline RingMapT(RingMapT<TKey, T, tThreadsafe, tOrderedKeys>&& ringMap) noexcept;

		/**
		 * Copy constructor.
		 * @param ringMap Object to be copied
		 */
		inline RingMapT(const RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& ringMap);

		/**
		 * Creates a new ring storage object with a specified capacity.
		 * @param capacity The capacity of the storage container, with range [0, infinity)
		 */
		explicit inline RingMapT(const size_t capacity);

		/**
		 * Returns the capacity of this storage container.
		 * @return Capacity of this storage container
		 */
		inline size_t capacity() const;

		/**
		 * Returns the number of elements that are currently stored in this container.
		 * @return Number of elements, with range [0, capacity()]
		 */
		inline size_t size() const;

		/**
		 * Sets or changes the capacity of this storage container.
		 * @param capacity The capacity to be set, with range [0, infinity)
		 */
		void setCapacity(const size_t capacity);

		/**
		 * Inserts a new element into this storage container.
		 * @param key The key of the new element
		 * @param element The element that will be inserted
		 * @param forceOverwrite True, to overwrite an existing element with some key, False to avoid that an element is inserted if an element with same key exists already
		 * @return True, if the element has been inserted
		 */
		bool insertElement(const TKey& key, const T& element, const bool forceOverwrite = false);

		/**
		 * Inserts a new element into this storage container.
		 * This function moves the new element.
		 * @param key The key of the new element
		 * @param element The element that will be inserted
		 * @param forceOverwrite True, to overwrite an existing element with some key, False to avoid that an element is inserted if an element with same key exists already
		 * @return True, if the element has been inserted
		 */
		bool insertElement(const TKey& key, T&& element, const bool forceOverwrite = false);

		/**
		 * Returns an element of this storage container.
		 * @param key The key of the element to be returned
		 * @param element Resulting element
		 * @return True, if the requested element exists
		 * @tparam tAccessMode The access mode to be used
		 * @see checkoutElement().
		 */
		template <AccessMode tAccessMode = AM_MATCH>
		bool element(const TKey& key, T& element) const;

		/**
		 * Returns the element with highest key.
		 * The map must be created with 'tOrderedKeys == true'.
		 * @param element Resulting element
		 * @return True, if the requested element exists
		 * @see element().
		 */
		bool highestElement(T& element) const;

		/**
		 * Returns the element with lowest key.
		 * The map must be created with 'tOrderedKeys == true'.
		 * @param element Resulting element
		 * @return True, if the requested element exists
		 * @see element().
		 */
		bool lowestElement(T& element) const;

		/**
		 * Returns an element of this storage container and removes the element from the container.
		 * @param key The key of the element to be returned
		 * @param element Resulting element
		 * @return True, if the requested element exists
		 * @tparam tAccessMode The access mode to be used
		 * @see element().
		 */
		template <AccessMode tAccessMode = AM_MATCH>
		bool checkoutElement(const TKey& key, T& element);

		/**
		 * Returns whether this storage container holds a specific element.
		 * @param key The key of the element that is checked
		 * @return True, if so
		 */
		bool hasElement(const TKey& key) const;

		/**
		 * Checks whether a specified element exists and changes the age of this element.
		 * If the specified element exists, the age of the element will be changed so that the element is the newest element in the database.<br>
		 * @param key The key of the element that will be refreshed
		 * @return True, if the element exists
		 */
		bool refreshElement(const TKey& key);

		/**
		 * Returns all elements of this map as a vector.
		 * In case 'tOrderedKeys == true', the resulting elements will be in order based on their corresponding keys.
		 * @return The map's element
		 */
		std::vector<T> elements() const;

		/**
		 * Clears all elements of this storage container.
		 */
		void clear();

		/**
		 * Returns whether this ring map holds at least one element.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Move operator.
		 * @param ringMap The ring map to be moved
		 * @return Reference to this object
		 */
		RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& operator=(RingMapT<TKey, T, tThreadsafe, tOrderedKeys>&& ringMap) noexcept;

		/**
		 * Move operator.
		 * @param ringMap The ring map to be moved
		 * @return Reference to this object
		 * @tparam tThreadSafeSecond True, if the map to be moved is thread-safe
		 */
		template <bool tThreadSafeSecond>
		RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& operator=(RingMapT<TKey, T, tThreadSafeSecond, tOrderedKeys>&& ringMap) noexcept;

		/**
		 * Copy operator.
		 * @param ringMap The ring map to be moved
		 * @return Reference to this object
		 */
		RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& operator=(const RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& ringMap);

		/**
		 * Copy operator.
		 * @param ringMap The ring map to be moved
		 * @return Reference to this object
		 * @tparam tThreadSafeSecond True, if the map to be moved is thread-safe
		 */
		template <bool tThreadSafeSecond>
		RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& operator=(const RingMapT<TKey, T, tThreadSafeSecond, tOrderedKeys>& ringMap);

	protected:

		/**
		 * Returns whether the internal states of this storage container is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// The map mapping keys to value pairs.
		KeyMap keyMap_;

		/// The list holding the keys in order how they have been added, oldest keys first.
		KeyList keyList_;

		/// The capacity of this storage container.
		size_t storageCapacity_ = 0;

		/// The container lock.
		mutable Lock lock_;
};

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::RingMapT(RingMapT<TKey, T, tThreadsafe, tOrderedKeys>&& ringMap) noexcept
{
	*this = std::move(ringMap);
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::RingMapT(const RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& ringMap)
{
	*this = ringMap;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::RingMapT(const size_t capacity) :
	storageCapacity_(capacity)
{
	// nothing to do here
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline size_t RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::capacity() const
{
	return storageCapacity_;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline size_t RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::size() const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	return keyMap_.size();
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
void RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::setCapacity(const size_t capacity)
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (capacity == storageCapacity_)
	{
		return;
	}

	if (capacity == 0)
	{
		keyMap_.clear();
		keyList_.clear();

	}
	else if (capacity < storageCapacity_)
	{
		while (keyMap_.size() > capacity)
		{
			ocean_assert(keyMap_.find(keyList_.front()) != keyMap_.cend());
			keyMap_.erase(keyList_.front());

			keyList_.pop_front();
		}
	}

	storageCapacity_ = capacity;

	ocean_assert(isValid());
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::insertElement(const TKey& key, const T& element, const bool forceOverwrite)
{
	T copyElement(element);

	return insertElement(key, std::move(copyElement), forceOverwrite);
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::insertElement(const TKey& key, T&& element, const bool forceOverwrite)
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (storageCapacity_ == 0)
	{
		return false;
	}

	// check whether the key exist already
	const typename KeyMap::iterator iMap = keyMap_.find(key);
	if (iMap != keyMap_.cend())
	{
		if (!forceOverwrite)
		{
			return false;
		}

		iMap->second.first = std::move(element);

		// moving the list entry to the end (making it the youngest entry), note: a list's iterator is still valid after moving an element
		keyList_.splice(keyList_.cend(), keyList_, iMap->second.second);

		return true;
	}

	// the key does not exist

	ocean_assert(keyMap_.size() <= storageCapacity_);

	if (keyMap_.size() >= storageCapacity_)
	{
		// the map is too big, we remove the oldest entry
		const TKey oldKey(keyList_.front());
		keyList_.pop_front();

		ocean_assert(keyMap_.find(oldKey) != keyMap_.end());
		keyMap_.erase(oldKey);
	}

	ocean_assert(keyMap_.size() < storageCapacity_);

	keyList_.emplace_back(key);

	keyMap_[key] = std::make_pair(std::move(element), --keyList_.end());

	ocean_assert(isValid());
	return true;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
template <typename RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::AccessMode tAccessMode>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::element(const TKey& key, T& element) const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (storageCapacity_ == 0 || keyMap_.empty())
	{
		return false;
	}

	typename KeyMap::const_iterator iMap = keyMap_.find(key);
	if (iMap == keyMap_.end())
	{
		if constexpr (tOrderedKeys)
		{
			if constexpr (tAccessMode == AM_MATCH_OR_HIGHEST)
			{
				iMap = keyMap_.rbegin().base();
				--iMap;
			}
			else if constexpr (tAccessMode == AM_MATCH_OR_LOWEST)
			{
				iMap = keyMap_.begin();
			}
			else
			{
				ocean_assert(tAccessMode == AM_MATCH);
				return false;
			}
		}
		else
		{
			ocean_assert(tAccessMode == AM_MATCH);
			return false;
		}
	}

	element = iMap->second.first;

	ocean_assert(isValid());
	return true;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::highestElement(T& element) const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (keyMap_.empty())
	{
		return false;
	}

	if constexpr (tOrderedKeys)
	{
		element = keyMap_.rbegin()->second.first;

		return true;
	}

	return false;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::lowestElement(T& element) const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (keyMap_.empty())
	{
		return false;
	}

	if constexpr (tOrderedKeys)
	{
		element = keyMap_.begin()->second;

		return true;
	}

	return false;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
template <typename RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::AccessMode tAccessMode>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::checkoutElement(const TKey& key, T& element)
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	if (storageCapacity_ == 0 || keyMap_.empty())
	{
		return false;
	}

	typename KeyMap::iterator iMap = keyMap_.find(key);
	if (iMap == keyMap_.end())
	{
		if constexpr (tOrderedKeys)
		{
			if constexpr (tAccessMode == AM_MATCH_OR_HIGHEST)
			{
				iMap = keyMap_.rbegin().base();
				--iMap;
			}
			else if constexpr (tAccessMode == AM_MATCH_OR_LOWEST)
			{
				iMap = keyMap_.begin();
			}
			else
			{
				ocean_assert(tAccessMode == AM_MATCH);
				return false;
			}
		}
		else
		{
			ocean_assert(tAccessMode == AM_MATCH);
			return false;
		}
	}

	keyList_.erase(iMap->second.second);

	element = std::move(iMap->second.first);

	keyMap_.erase(iMap);

	ocean_assert(isValid());
	return true;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::hasElement(const TKey& key) const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	return keyMap_.find(key) != keyMap_.end();
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
std::vector<T> RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::elements() const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	std::vector<T> result;
	result.reserve(keyMap_.size());

	for (typename KeyMap::const_iterator iMap = keyMap_.cbegin(); iMap != keyMap_.cend(); ++iMap)
	{
		result.emplace_back(iMap->second.first);
	}

	return result;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::refreshElement(const TKey& key)
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	const typename KeyMap::const_iterator iMap = keyMap_.find(key);

	if (iMap == keyMap_.cend())
	{
		return false;
	}

	// moving the list entry to the end (making it the youngest entry), note: a list's iterator is still valid after moving an element
	keyList_.splice(keyList_.cend(), keyList_, iMap->second.second);

	ocean_assert(isValid());
	return true;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
void RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::clear()
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	keyMap_.clear();
	keyList_.clear();

	ocean_assert(isValid());
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::isValid() const
{
	ocean_assert(keyMap_.size() == keyList_.size());

	return keyMap_.size() <= storageCapacity_ && keyMap_.size() == keyList_.size();
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
inline bool RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::isEmpty() const
{
	const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
	ocean_assert(isValid());

	return keyMap_.empty();
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::operator=(RingMapT<TKey, T, tThreadsafe, tOrderedKeys>&& ringMap) noexcept
{
	if (this != &ringMap)
	{
		const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
		const TemplatedScopedLock<tThreadsafe> scopedLockSecond(ringMap.lock_); // will not create a dead-lock unless both maps depend on each other

		keyMap_ = std::move(ringMap.keyMap_);
		keyList_ = std::move(ringMap.keyList_);
		storageCapacity_ = ringMap.storageCapacity_;
		ringMap.storageCapacity_ = 0;
	}

	return *this;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
template <bool tThreadSafeSecond>
RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::operator=(RingMapT<TKey, T, tThreadSafeSecond, tOrderedKeys>&& ringMap) noexcept
{
	if ((void*)(this) != (void*)(&ringMap))
	{
		const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
		const TemplatedScopedLock<tThreadSafeSecond> scopedLockSecond(ringMap.lock_); // will not create a dead-lock unless both maps depend on each other

		keyMap_ = std::move(ringMap.keyMap_);
		keyList_ = std::move(ringMap.keyList_);
		storageCapacity_ = ringMap.storageCapacity_;
		ringMap.storageCapacity_ = 0;
	}

	return *this;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::operator=(const RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& ringMap)
{
	if (this != &ringMap)
	{
		const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
		const TemplatedScopedLock<tThreadsafe> scopedLockSecond(ringMap.lock_); // will not create a dead-lock unless both maps depend on each other

		keyMap_ = ringMap.keyMap_;
		keyList_ = ringMap.keyList_;
		storageCapacity_ = ringMap.storageCapacity_;
	}

	return *this;
}

template <typename TKey, typename T, bool tThreadsafe, bool tOrderedKeys>
template <bool tThreadSafeSecond>
RingMapT<TKey, T, tThreadsafe, tOrderedKeys>& RingMapT<TKey, T, tThreadsafe, tOrderedKeys>::operator=(const RingMapT<TKey, T, tThreadSafeSecond, tOrderedKeys>& ringMap)
{
	if ((void*)(this) != (void*)(&ringMap))
	{
		const TemplatedScopedLock<tThreadsafe> scopedLock(lock_);
		const TemplatedScopedLock<tThreadSafeSecond> scopedLockSecond(ringMap.lock_); // will not create a dead-lock unless both maps depend on each other

		keyMap_ = ringMap.keyMap_;
		keyList_ = ringMap.keyList_;
		storageCapacity_ = ringMap.storageCapacity_;
	}

	return *this;
}

}

#endif // META_OCEAN_BASE_RING_MAP_H
