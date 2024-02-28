// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		inline StaticVector();

		/**
		 * Creates a new vector object.
		 * @param value Value that will be set as first element
		 */
		inline explicit StaticVector(const T& value);

		/**
		 * Creates a new vector object.
		 * @param number Number of elements to be created
		 * @param value Value that will be created in the first 'number' elements of this vector
		 */
		inline StaticVector(const size_t number, const T& value);

		/**
		 * Creates a new vector object.
		 * This constructor converts a stl vector object to a static vector object.<br>
		 * Only the first tCapacity elements of the given vector are copied.<br>
		 * @param values Values that will be used as first elements
		 */
		inline explicit StaticVector(const std::vector<T>& values);

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
		 * @param value Value to be added
		 * @see securePushBack().
		 */
		inline void pushBack(const T& value);

		/**
		 * Adds a new element to this vector if this vector has free elements left, otherwise nothing happens
		 * @param value Value to be added
		 * @return True, if succeeded
		 * @see pushBack().
		 */
		inline bool securePushBack(const T& value);

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
		 * @param index Index of the element that will be removed, with range [0, size())
		 * @see unstableErase().
		 */
		inline void erase(const size_t index);

		/**
		 * Erases one element of this vector.
		 * The free element is replace by the last element in the vector, thus the order of the elements inside this vector is lost.<br>
		 * This erase function is faster than the standard erase function.<br>
		 * @param index Index of the element that will be removed, with range [0, size())
		 * @see erase().
		 */
		inline void unstableErase(const size_t index);

		/**
		 * Resizes this vector.
		 * @param size Size to be applied, with range [0, tCapacity]
		 * @see weakResize().
		 */
		inline void resize(const size_t size);

		/**
		 * Resizes this vector.
		 * This function simply sets the element counter.<br>
		 * @param size Size to be applied, with range [0, tCapacity]
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
		 * @param index Index of the element that will be returned, with range [0, size())
		 * @return Vector element
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Returns one element of this vector.
		 * Beware: No range check is done.
		 * @param index Index of the element that will be returned, with range [0, size())
		 * @return Vector element
		 */
		inline T& operator[](const size_t index);

		/**
		 * Returns whether two vectors are identical.
		 * @param second Second vector object
		 * @return True, if so
		 */
		inline bool operator==(const StaticVector<T, tCapacity>& second) const;

		/**
		 * Returns whether two vectors are not identical.
		 * @param second Second vector object
		 * @return True, if so
		 */
		inline bool operator!=(const StaticVector<T, tCapacity>& second) const;

		/**
		 * Returns whether this vector holds at least one element.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// Current number of stored elements.
		size_t vectorSize;
};

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector() :
	vectorSize(0)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const T& value) :
	StaticBuffer<T, tCapacity>(value),
	vectorSize(1)
{
	static_assert(tCapacity > 0, "Invalid vector capacity!");
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const size_t number, const T& value) :
	StaticBuffer<T, tCapacity>(number, value),
	vectorSize(number)
{
	ocean_assert(number <= tCapacity);
}

template <typename T, size_t tCapacity>
inline StaticVector<T, tCapacity>::StaticVector(const std::vector<T>& values) :
	StaticBuffer<T, tCapacity>(values),
	vectorSize(min(tCapacity, values.size()))
{
	// nothing to do here
}

template <typename T, size_t tCapacity>
inline size_t StaticVector<T, tCapacity>::size() const
{
	return vectorSize;
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::occupied() const
{
	return vectorSize == tCapacity;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::pushBack(const T& value)
{
	ocean_assert(vectorSize < tCapacity);
	this->bufferElements[vectorSize++] = value;
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::securePushBack(const T& value)
{
	if (vectorSize >= tCapacity)
		return false;

	this->bufferElements[vectorSize++] = value;
	return true;
}

template <typename T, size_t tCapacity>
template <size_t tCapacity2>
inline void StaticVector<T, tCapacity>::pushBack(const StaticVector<T, tCapacity2>& value)
{
	size_t elements = min(value.size(), tCapacity - vectorSize);

	for (size_t n = 0; n < elements; ++n)
		this->bufferElements[n + vectorSize] = value.bufferElements[n];

	vectorSize += elements;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::pushBack(const std::vector<T>& value)
{
	size_t elements = min(value.size(), tCapacity - vectorSize);

	for (size_t n = 0; n < elements; ++n)
		this->bufferElements[n + vectorSize] = value.bufferElements[n];

	vectorSize += elements;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::popBack()
{
	ocean_assert(vectorSize > 0);
	this->bufferElements[--vectorSize] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::securePopBack()
{
	if (vectorSize > 0)
		this->bufferElements[--vectorSize] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakPopBack()
{
	ocean_assert(vectorSize > 0);
	--vectorSize;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::secureWeakPopBack()
{
	if (vectorSize > 0)
		vectorSize--;
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::front() const
{
	ocean_assert(!empty());
	return this->bufferElements[0];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::front()
{
	ocean_assert(!empty());
	return this->bufferElements[0];
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::back() const
{
	ocean_assert(!empty());
	return this->bufferElements[vectorSize - 1];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::back()
{
	ocean_assert(!empty());
	return this->bufferElements[vectorSize - 1];
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::empty() const
{
	return vectorSize == 0;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::erase(const size_t index)
{
	ocean_assert(index < vectorSize);
	for (size_t n = index; n + 1 < vectorSize; ++n)
		this->bufferElements[n] = std::move(this->bufferElements[n + 1]);

	this->bufferElements[--vectorSize] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::unstableErase(const size_t index)
{
	ocean_assert(index < vectorSize);

	vectorSize--;

	if (index < vectorSize)
		this->bufferElements[index] = this->bufferElements[vectorSize];

	this->bufferElements[vectorSize] = T();
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::resize(const size_t size)
{
	ocean_assert(size <= tCapacity);

	for (size_t n = size; n < vectorSize; ++n)
		this->bufferElements[n] = T();

	vectorSize = size;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakResize(const size_t size)
{
	ocean_assert(size <= tCapacity);
	vectorSize = size;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::clear()
{
	for (size_t n = 0; n < vectorSize; ++n)
		this->bufferElements[n] = T();

	vectorSize = 0;
}

template <typename T, size_t tCapacity>
inline void StaticVector<T, tCapacity>::weakClear()
{
	vectorSize = 0;
}

template <typename T, size_t tCapacity>
inline const T& StaticVector<T, tCapacity>::operator[](const size_t index) const
{
	ocean_assert(index < vectorSize);
	return this->bufferElements[index];
}

template <typename T, size_t tCapacity>
inline T& StaticVector<T, tCapacity>::operator[](const size_t index)
{
	ocean_assert(index < vectorSize);
	return this->bufferElements[index];
}

template <typename T, size_t tCapacity>
inline bool StaticVector<T, tCapacity>::operator==(const StaticVector<T, tCapacity>& second) const
{
	if (vectorSize != second.vectorSize)
		return false;

	for (size_t n = 0; n < vectorSize; ++n)
		if (this->bufferElements[n] != second.bufferElements[n])
			return false;

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
	return vectorSize > 0;
}

}

#endif // META_OCEAN_BASE_STATIC_VECTOR_H
