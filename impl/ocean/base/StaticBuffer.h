// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_BASE_STATIC_BUFFER_H
#define META_OCEAN_BASE_STATIC_BUFFER_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class implements a static buffer that has a fixed capacity.
 * @tparam T Data type of the elements that will be stored
 * @tparam tCapacity Number of elements that can be stored, with range [1, infinity)
 * @ingroup base
 */
template <typename T, size_t tCapacity>
class StaticBuffer
{
	public:

		/**
		 * Definition of the data type of each individual buffer element.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new buffer object.
		 */
		inline StaticBuffer();

		/**
		 * Creates a new buffer object.
		 * @param value Value that will be set for the first element of this buffer
		 */
		inline explicit StaticBuffer(const T& value);

		/**
		 * Creates a new buffer object.
		 * @param buffer A buffer with at least as much elements as this static buffer has, all elements are copied
		 */
		inline explicit StaticBuffer(const T* buffer);

		/**
		 * Creates a new buffer object.
		 * @param number Number of elements to be created
		 * @param value Value that will be created in the first 'number' elements of this buffer
		 */
		inline StaticBuffer(const size_t number, const T& value);

		/**
		 * Creates a new buffer object.
		 * This constructor converts a stl vector object to a static buffer object.<br>
		 * Only the first tCapacity elements of the given vector are copied.<br>
		 * @param values Values that will be used as first elements
		 */
		inline explicit StaticBuffer(const std::vector<T>& values);

		/**
		 * Returns the capacity of this buffer.
		 * @return Buffer capacity
		 */
		inline static constexpr size_t capacity();

		/**
		 * Clears all elements of this buffer.
		 * @see weakClear().
		 */
		inline void clear();

		/**
		 * Returns the first elements of this buffer.
		 * @return First element
		 */
		inline const T& front() const;

		/**
		 * Returns the first elements of this buffer.
		 * @return First element
		 */
		inline T& front();

		/**
		 * Returns the last elements of this buffer.
		 * @return Last element
		 */
		inline const T& back() const;

		/**
		 * Returns the last elements of this buffer.
		 * @return Last element
		 */
		inline T& back();

		/**
		 * Returns the buffer data pointer.
		 * @return Data pointer
		 */
		inline const T* data() const;

		/**
		 * Returns the buffer data pointer.
		 * @return Data pointer
		 */
		inline T* data();

		/**
		 * Returns one element of this buffer.
		 * Beware: No range check is done.
		 * @param index Index of the element that will be returned, with range [0, tCapacity)
		 * @return Buffer element
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Returns one element of this buffer.
		 * Beware: No range check is done.
		 * @param index Index of the element that will be returned, with range [0, tCapacity)
		 * @return Buffer element
		 */
		inline T& operator[](const size_t index);

		/**
		 * Returns whether two buffers are identical.
		 * @param second Second buffer object
		 * @return True, if so
		 */
		inline bool operator==(const StaticBuffer<T, tCapacity>& second) const;

		/**
		 * Returns whether two buffers are not identical.
		 * @param second Second buffer object
		 * @return True, if so
		 */
		inline bool operator!=(const StaticBuffer<T, tCapacity>& second) const;

	protected:

		/// Elements of this buffer (with at least one entry).
		T bufferElements[tCapacity > size_t(0) ? tCapacity : size_t(1)];
};

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer()
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const T& value)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	bufferElements[0] = value;
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const T* buffer)
{
	ocean_assert(buffer != NULL);
	memcpy(bufferElements, buffer, tCapacity * sizeof(T));
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const size_t number, const T& value)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	ocean_assert(number <= tCapacity);

	for (size_t n = 0; n < number; ++n)
		bufferElements[n] = value;
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const std::vector<T>& values)
{
	const size_t size = min(tCapacity, values.size());

	for (size_t n = 0; n < size; ++n)
		bufferElements[n] = values[n];
}

template <typename T, size_t tCapacity>
inline constexpr size_t StaticBuffer<T, tCapacity>::capacity()
{
	return tCapacity;
}

template <typename T, size_t tCapacity>
inline const T* StaticBuffer<T, tCapacity>::data() const
{
	return bufferElements;
}

template <typename T, size_t tCapacity>
inline T* StaticBuffer<T, tCapacity>::data()
{
	return bufferElements;
}

template <typename T, size_t tCapacity>
inline void StaticBuffer<T, tCapacity>::clear()
{
	for (size_t n = 0; n < tCapacity; ++n)
		bufferElements[n] = T();
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::front() const
{
	return bufferElements[0];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::front()
{
	return bufferElements[0];
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::back() const
{
	return bufferElements[tCapacity - 1];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::back()
{
	return bufferElements[tCapacity - 1];
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::operator[](const size_t index) const
{
	ocean_assert(index < tCapacity);
	return bufferElements[index];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::operator[](const size_t index)
{
	ocean_assert(index < tCapacity);
	return bufferElements[index];
}

template <typename T, size_t tCapacity>
inline bool StaticBuffer<T, tCapacity>::operator==(const StaticBuffer<T, tCapacity>& second) const
{
	for (size_t n = 0; n < tCapacity; ++n)
		if (bufferElements[n] != second.bufferElements[n])
			return false;

	return true;
}

template <typename T, size_t tCapacity>
inline bool StaticBuffer<T, tCapacity>::operator!=(const StaticBuffer<T, tCapacity>& second) const
{
	return !(*this == second);
}

}

#endif // META_OCEAN_BASE_STATIC_BUFFER_H
