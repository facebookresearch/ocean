/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
		StaticBuffer() = default;

		/**
		 * Creates a new buffer object.
		 * @param value The value that will be set for the first element of this buffer
		 */
		explicit inline StaticBuffer(const T& value);

		/**
		 * Creates a new buffer object.
		 * @param value The value that will be set for the first element of this buffer
		 */
		explicit inline StaticBuffer(T&& value);

		/**
		 * Creates a new buffer object.
		 * @param buffer A buffer with at least as much elements as this static buffer has, all 'tCapacity' elements are copied, must be valid
		 */
		explicit inline StaticBuffer(const T* buffer);

		/**
		 * Creates a new buffer object.
		 * @param number The number of elements to be created, with range [0, tCapacity]
		 * @param value The value that will be created in the first 'number' elements of this buffer
		 */
		inline StaticBuffer(const size_t number, const T& value);

		/**
		 * Creates a new buffer object.
		 * This constructor converts a stl vector object to a static buffer object.<br>
		 * Only the first tCapacity elements of the given vector are copied.
		 * @param values The values that will be used as first elements
		 */
		explicit inline StaticBuffer(const std::vector<T>& values);

		/**
		 * Creates a new buffer object.
		 * This constructor converts a stl vector object to a static buffer object.<br>
		 * Only the first tCapacity elements of the given vector are copied.
		 * @param values The values that will be used as first elements
		 */
		explicit inline StaticBuffer(std::vector<T>&& values);

		/**
		 * Returns the capacity of this buffer.
		 * @return Buffer capacity
		 */
		static constexpr size_t capacity();

		/**
		 * Clears all elements of this buffer.
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
		 * @param index The index of the element that will be returned, with range [0, tCapacity)
		 * @return Buffer element
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Returns one element of this buffer.
		 * Beware: No range check is done.
		 * @param index The index of the element that will be returned, with range [0, tCapacity)
		 * @return Buffer element
		 */
		inline T& operator[](const size_t index);

		/**
		 * Returns whether two buffers are identical.
		 * @param second The second buffer object
		 * @return True, if so
		 */
		inline bool operator==(const StaticBuffer<T, tCapacity>& second) const;

		/**
		 * Returns whether two buffers are not identical.
		 * @param second The second buffer object
		 * @return True, if so
		 */
		inline bool operator!=(const StaticBuffer<T, tCapacity>& second) const;

	protected:

		/// Elements of this buffer (with at least one entry).
		T elements_[tCapacity > size_t(0) ? tCapacity : size_t(1)];
};

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const T& value)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	elements_[0] = value;
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(T&& value)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	elements_[0] = std::move(value);
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const T* buffer)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	ocean_assert(buffer != nullptr);

	for (size_t n = 0u; n < tCapacity; ++n)
	{
		elements_[n] = buffer[n];
	}
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const size_t number, const T& value)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	ocean_assert(number <= tCapacity);

	for (size_t n = 0; n < number; ++n)
	{
		elements_[n] = value;
	}
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(const std::vector<T>& values)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	const size_t size = min(tCapacity, values.size());

	for (size_t n = 0; n < size; ++n)
	{
		elements_[n] = values[n];
	}
}

template <typename T, size_t tCapacity>
inline StaticBuffer<T, tCapacity>::StaticBuffer(std::vector<T>&& values)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	const size_t size = min(tCapacity, values.size());

	for (size_t n = 0; n < size; ++n)
	{
		elements_[n] = std::move(values[n]);
	}
}

template <typename T, size_t tCapacity>
constexpr size_t StaticBuffer<T, tCapacity>::capacity()
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	return tCapacity;
}

template <typename T, size_t tCapacity>
inline const T* StaticBuffer<T, tCapacity>::data() const
{
	return elements_;
}

template <typename T, size_t tCapacity>
inline T* StaticBuffer<T, tCapacity>::data()
{
	return elements_;
}

template <typename T, size_t tCapacity>
inline void StaticBuffer<T, tCapacity>::clear()
{
	for (size_t n = 0; n < tCapacity; ++n)
	{
		elements_[n] = T();
	}
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::front() const
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	return elements_[0];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::front()
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	return elements_[0];
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::back() const
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	return elements_[tCapacity - 1];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::back()
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	return elements_[tCapacity - 1];
}

template <typename T, size_t tCapacity>
inline const T& StaticBuffer<T, tCapacity>::operator[](const size_t index) const
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	ocean_assert(index < tCapacity);

	return elements_[index];
}

template <typename T, size_t tCapacity>
inline T& StaticBuffer<T, tCapacity>::operator[](const size_t index)
{
	static_assert(tCapacity > 0, "Invalid buffer capacity!");

	ocean_assert(index < tCapacity);

	return elements_[index];
}

template <typename T, size_t tCapacity>
inline bool StaticBuffer<T, tCapacity>::operator==(const StaticBuffer<T, tCapacity>& second) const
{
	for (size_t n = 0; n < tCapacity; ++n)
	{
		if (elements_[n] != second.elements_[n])
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
inline bool StaticBuffer<T, tCapacity>::operator!=(const StaticBuffer<T, tCapacity>& second) const
{
	return !(*this == second);
}

}

#endif // META_OCEAN_BASE_STATIC_BUFFER_H
