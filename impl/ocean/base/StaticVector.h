/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_STATIC_VECTOR_H
#define META_OCEAN_BASE_STATIC_VECTOR_H

#include "ocean/base/Base.h"
#include "ocean/base/StaticBuffer.h"

namespace Ocean
{

/**
 * This class implements a static vector that has a fixed capacity.
 * @tparam T Data type of the elements that will be stored
 * @tparam tCapacity Number of elements that can be stored, with range [1, infinity)
 * @ingroup base
 */
template <typename T, size_t tCapacity>
class StaticVector : public StaticBuffer<T, tCapacity>
{
	public:

		/**
		 * Creates a new vector object.
		 */
		StaticVector() = default;

		/**
		 * Creates a new vector object.
		 * @param value The value that will be set as first element
		 */
		explicit inline StaticVector(const T& value);

		/**
		 * Creates a new vector object.
		 * @param value The value that will be set as first element
		 */
		explicit inline StaticVector(T&& value);

		/**
		 * Creates a new vector object.
		 * @param number The number of elements to be created
		 * @param value The value that will be created in the first 'number' elements of this vector
		 */
		inline StaticVector(const size_t number, const T& value);

		/**
		 * Creates a new vector object.
		 * @param values The values to be copied into this vector object, can be nullptr if 'size == 0'
		 * @param size The number of values to copy, with range [0, tCapacity]
		 */
		inline StaticVector(const T* values, const size_t size);

		/**
		 * Creates a new vector object.
		 * This constructor converts a stl vector object to a static vector object.<br>
		 * Only the first tCapacity elements of the given vector are copied.
		 * @param values The values that will be used as first elements
		 */
		explicit inline StaticVector(const std::vector<T>& values);

		/**
		 * Creates a new vector object.
		 * This constructor converts a stl vector object to a static vector object.<br>
		 * Only the first tCapacity elements of the given vector are copied.
		 * @param values The values that will be used as first elements
		 */
		explicit inline StaticVector(std::vector<T>&& values);

		/**
		 * Returns the size of this vector.
		 * @return Vector size
		 */
		inline size_t size() const;

		/**
		 * Returns whether no free space is left.
		 * @return True, if capacity() - size() == 0
		 */
		inline bool occupied() const;

		/**
		 * Adds a new element to this vector.
		 * Beware: No range check is applied.
		 * @param value The value to be added
		 * @see securePushBack().
		 */
		inline void pushBack(const T& value);

		/**
		 * Adds a new element to this vector.
		 * Beware: No range check is applied.
		 * @param value The value to be added
		 * @see securePushBack().
		 */
		inline void pushBack(T&& value);

		/**
		 * Adds a new element to this vector if this vector has free elements left, otherwise nothing happens
		 * @param value The value to be added
		 * @return True, if succeeded
		 * @see pushBack().
		 */
		inline bool securePushBack(const T& value);

		/**
		 * Adds a new element to this vector if this vector has free elements left, otherwise nothing happens
		 * @param value The value to be added
		 * @return True, if succeeded
		 * @see pushBack().
		 */
		inline bool securePushBack(T&& value);

		/**
		 * Adds a new elements to this vector.
		 * This function avoids a memory overflow.<br>
		 * @param value Values to be added
		 * @tparam tCapacity2 Capacity of the second vector
		 */
		template <size_t tCapacity2>
		inline void pushBack(const StaticVector<T, tCapacity2>& value);

		/**
		 * Adds a new elements to this vector.
		 * This function avoids a memory overflow.<br>
		 * @param value Values to be added
		 */
		inline void pushBack(const std::vector<T>& value);

		/**
		 * Removes the last element from this vector.
		 * Beware: No range check is applied.
		 * Thus: Check that this vector holds at least one element!<br>
		 * @see weakPopBack(), pushBack().
		 */
		inline void popBack();

		/**
		 * Removes the last element from this vector.
		 * If this vector holds no element, nothing is happen.<br>
		 * @see weakPopBack(), pushBack().
		 */
		inline void securePopBack();

		/**
		 * Removes the last element from this vector.
		 * This function simply decreases the element counter, the last element is untouched.<br>
		 * Beware: No range check is applied.
		 * Thus: Check that this vector holds at least one element!<br>
		 * @see popBack().
		 */
		inline void weakPopBack();

		/**
		 * Removes the last element from this vector.
		 * This function simply decreases the element counter, the last element is untouched.<br>
		 * If this vector holds no element, nothing is happen.<br>
		 * @see popBack().
		 */
		inline void secureWeakPopBack();

		/**
		 * Erases one element of this vector.
		 * @param index The index of the element that will be removed, with range [0, size())
		 * @see unstableErase().
		 */
		inline void erase(const size_t index);

		/**
		 * Erases one element from this vector.
		 * The free element is replace by the last element in the vector, thus the previous order of the elements inside this vector is lost.<br>
		 * This erase function is faster than the standard erase function.
		 * @param index The index of the element that will be removed, with range [0, size())
		 * @see erase().
		 */
		inline void unstableErase(const size_t index);

		/**
		 * Resizes this vector.
		 * @param size The size to be applied, with range [0, tCapacity]
		 * @see weakResize().
		 */
		inline void resize(const size_t size);

		/**
		 * Resizes this vector.
		 * This function simply sets the element counter.<br>
		 * @param size The size to be applied, with range [0, tCapacity]
		 * @see resize().
		 */
		inline void weakResize(const size_t size);

		/**
		 * Clears all elements of this vector.
		 * @see weakClear().
		 */
		inline void clear();

		/**
		 * Clears all elements of this vector by setting the internal index to zero (all stored elements are untouched).
		 * @see clear().
		 */
		inline void weakClear();

		/**
		 * Returns the first elements of this vector.
		 * @return First element
		 */
		inline const T& front() const;

		/**
		 * Returns the first elements of this vector.
		 * @return First element
		 */
		inline T& front();

		/**
		 * Returns the last elements of this vector.
		 * @return Last element
		 */
		inline const T& back() const;

		/**
		 * Returns the last elements of this vector.
		 * @return Last element
		 */
		inline T& back();

		/**
		 * Returns whether this vector hold no element.
		 * @return True, if so
		 */
		inline bool empty() const;

		/**
		 * Returns one element of this vector.
		 * Beware: No range check is done.
		 * @param index The index of the element that will be returned, with range [0, size())
		 * @return Vector element
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Returns one element of this vector.
		 * Beware: No range check is done.
		 * @param index The index of the element that will be returned, with range [0, size())
		 * @return Vector element
		 */
		inline T& operator[](const size_t index);

		/**
		 * Returns whether two vectors are identical.
		 * @param second The second vector object
		 * @return True, if so
		 */
		inline bool operator==(const StaticVector<T, tCapacity>& second) const;

		/**
		 * Returns whether two vectors are not identical.
		 * @param second The second vector object
		 * @return True, if so
		 */
		inline bool operator!=(const StaticVector<T, tCapacity>& second) const;

		/**
		 * Returns whether this vector holds at least one element.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The current number of stored elements, with range [0, tCapacity]
		size_t size_ = 0;
};

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const T& value) :
	StaticBuffer<T, tCapacity>(value),
	size_(1)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(T&& value) :
	StaticBuffer<T, tCapacity>(std::move(value)),
	size_(1)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const size_t number, const T& value) :
	StaticBuffer<T, tCapacity>(number, value),
	size_(number)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");

	ocean_assert(number <= tCapacity);
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const T* values, const size_t size)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");

	ocean_assert(size <= tCapacity);

	for (size_t n = 0; n < std::min(size, tCapacity); ++n)
	{
		this->elements_[n] = values[n];
	}

	size_ = size;
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const std::vector<T>& values) :
	StaticBuffer<T, tCapacity>(values),
	size_(min(tCapacity, values.size()))
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(std::vector<T>&& values) :
	StaticBuffer<T, tCapacity>(std::move(values)),
	size_(min(tCapacity, values.size()))
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline size_t StaticVector<T, tCapacity>::size() const
{
	return size_;
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::occupied() const
{
	return size_ == tCapacity;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::pushBack(const T& value)
{
	ocean_assert(size_ < tCapacity);

	this->elements_[size_++] = value;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::pushBack(T&& value)
{
	ocean_assert(size_ < tCapacity);

	this->elements_[size_++] = std::move(value);
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::securePushBack(const T& value)
{
	if (size_ >= tCapacity)
	{
		return false;
	}

	this->elements_[size_++] = value;

	return true;
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::securePushBack(T&& value)
{
	if (size_ >= tCapacity)
	{
		return false;
	}

	this->elements_[size_++] = std::move(value);

	return true;
}

template <typename T, size_t tCapacity>
template <size_t tCapacity2>
inline void StaticVector<T, tCapacity>::pushBack(const StaticVector<T, tCapacity2>& value)
{
	size_t elements = min(value.size(), tCapacity - size_);

	for (size_t n = 0; n < elements; ++n)
	{
		this->elements_[n + size_] = value.elements_[n];
	}

	size_ += elements;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::pushBack(const std::vector<T>& value)
{
	size_t elements = min(value.size(), tCapacity - size_);

	for (size_t n = 0; n < elements; ++n)
	{
		this->elements_[n + size_] = value.elements_[n];
	}

	size_ += elements;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::popBack()
{
	ocean_assert(size_ > 0);

	this->elements_[--size_] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::securePopBack()
{
	if (size_ > 0)
	{
		this->elements_[--size_] = T();
	}
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakPopBack()
{
	ocean_assert(size_ > 0);

	--size_;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::secureWeakPopBack()
{
	if (size_ > 0)
	{
		--size_;
	}
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::front() const
{
	ocean_assert(!empty());

	return this->elements_[0];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::front()
{
	ocean_assert(!empty());

	return this->elements_[0];
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::back() const
{
	ocean_assert(!empty());

	return this->elements_[size_ - 1];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::back()
{
	ocean_assert(!empty());

	return this->elements_[size_ - 1];
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::empty() const
{
	return size_ == 0;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::erase(const size_t index)
{
	ocean_assert(index < size_);

	for (size_t n = index + 1; n < size_; ++n)
	{
		this->elements_[n - 1] = std::move(this->elements_[n]);
	}

	this->elements_[--size_] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::unstableErase(const size_t index)
{
	ocean_assert(index < size_);

	--size_;

	if (index < size_)
	{
		this->elements_[index] = std::move(this->elements_[size_]);
	}

	this->elements_[size_] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::resize(const size_t size)
{
	ocean_assert(size <= tCapacity);

	for (size_t n = size; n < size_; ++n)
	{
		this->elements_[n] = T();
	}

	size_ = size;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakResize(const size_t size)
{
	ocean_assert(size <= tCapacity);

	size_ = size;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::clear()
{
	for (size_t n = 0; n < size_; ++n)
	{
		this->elements_[n] = T();
	}

	size_ = 0;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakClear()
{
	size_ = 0;
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::operator[](const size_t index) const
{
	ocean_assert(index < size_);

	return this->elements_[index];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::operator[](const size_t index)
{
	ocean_assert(index < size_);

	return this->elements_[index];
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::operator==(const StaticVector<T, tCapacity>& second) const
{
	if (size_ != second.size_)
	{
		return false;
	}

	for (size_t n = 0; n < size_; ++n)
	{
		if (this->elements_[n] != second.elements_[n])
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::operator!=(const StaticVector<T, tCapacity>& second) const
{
	return !(*this == second);
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::operator bool() const
{
	return size_ > 0;
}

}

#endif // META_OCEAN_BASE_STATIC_VECTOR_H
