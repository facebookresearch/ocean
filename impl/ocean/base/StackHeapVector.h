/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_STACK_HEAP_VECTOR_H
#define META_OCEAN_BASE_STACK_HEAP_VECTOR_H

#include "ocean/base/Base.h"
#include "ocean/base/StaticBuffer.h"

namespace Ocean
{

/**
 * Vector like data structure combining stack and heap memory.
 * This class implements a vector-like data structure which stores the first `tStackCapacity` elements on the stack, and any additional elements on the heap.<br>
 * This approach can optimize performance and memory usage when the number of elements is often within the `tStackCapacity` but can occasionally exceed it.
 * @tparam T Data type of the elements that will be stored
 * @tparam tStackCapacity Number of elements that can be stored on the stack, with range [1, infinity)
 * @ingroup base
 */
template <typename T, size_t tStackCapacity>
class StackHeapVector
{
	static_assert(tStackCapacity >= 1, "Invalid stack capacity!");

	public:

		/**
		 * Definition of an iterator allowing to iterate through the vector.
		 * The iterator allows to modified the element to which the iterator is pointing.
		 */
		class Iterator
		{
			friend class StackHeapVector;

			public:

				/**
				 * (Pre-) increments this iterator.
				 * @return Reference to the incremented iterator
				 */
				Iterator& operator++();

				/**
				 * (Post-) increments this iterator.
				 * @return The current (not yet) incremented iterator.
				 */
				Iterator operator++(int);

				/**
				 * De-references the iterator and provides access to the underlying element in the vector.
				 * @return The vector element to which this iterator points
				 */
				T& operator*();

				/**
				 * Compares two iterators and returns whether both iterators point to the same vector element.
				 * @return True, if so
				 */
				bool operator==(const Iterator& iterator) const;

				/**
				 * Compares two iterators and returns whether both iterators do not point to the same vector element.
				 * @return True, if so
				 */
				bool operator!=(const Iterator& iterator) const;

			protected:

				/**
				 * Creates a new iterator pointing to a specified elements in the vector.
				 * @param vector The vector owning this iterator
				 * @param index The index of the element to which the iterator points
				 */
				Iterator(StackHeapVector& vector, const size_t index);

			protected:

				/// The vector owning this iterator.
				StackHeapVector& vector_;

				/// The index of the element within the vector to which the iterator points.
				size_t index_ = size_t(-1);
		};

		/**
		 * Definition of an iterator allowing to iterate through the vector.
		 * The iterator does ont allow to modified the element to which the iterator is pointing.
		 */
		class ConstIterator
		{
			friend class StackHeapVector;

			public:

				/**
				 * (Pre-) increments this iterator.
				 * @return Reference to the incremented iterator
				 */
				ConstIterator& operator++();

				/**
				 * (Post-) increments this iterator.
				 * @return The current (not yet) incremented iterator.
				 */
				ConstIterator operator++(int);

				/**
				 * De-references the iterator and provides access to the underlying element in the vector.
				 * @return The vector element to which this iterator points
				 */
				const T& operator*() const;

				/**
				 * Compares two iterators and returns whether both iterators point to the same vector element.
				 * @return True, if so
				 */
				bool operator==(const ConstIterator& iterator) const;

				/**
				 * Compares two iterators and returns whether both iterators do not point to the same vector element.
				 * @return True, if so
				 */
				bool operator!=(const ConstIterator& iterator) const;

			protected:

				/**
				 * Creates a new iterator pointing to a specified elements in the vector.
				 * @param vector The vector owning this iterator
				 * @param index The index of the element to which the iterator points
				 */
				ConstIterator(const StackHeapVector& vector, const size_t index);

			protected:

				/// The vector owning this iterator.
				const StackHeapVector& vector_;

				/// The index of the element within the vector to which the iterator points.
				size_t index_ = size_t(-1);
		};

	public:

		/**
		 * Creates a new vector object.
		 */
		StackHeapVector();

		/**
		 * Creates a new vector object.
		 * @param size The number of elements to be created
		 * @param element The value that will be created in the first 'number' elements of this vector
		 */
		StackHeapVector(const size_t size, const T& element);

		/**
		 * Pushes a new element to the end of this vector.
		 * @param element The new element to be pushed
		 */
		void pushBack(T&& element);

		/**
		 * Pushes a new element to the end of this vector.
		 * @param element The new element to be pushed
		 */
		void pushBack(const T& element);

		/**
		 * Emplaces a new element to the end of this vector.
		 * @param args The arguments of the new element
		 * @tparam TArgs The data types of the arguments
		 */
		template <typename... TArgs>
		T& emplaceBack(TArgs&&... args);

		/**
		 * Removed the last elements from the vector.
		 * The vector must not be empty.
		 */
		void popBack();

		/**
		 * Resizes the vector.
		 * @param size The new size of the vector
		 */
		void resize(const size_t size);

		/**
		 * Replaces the content of the vector with 'size' copies of the provided element.
		 * @param size The new size of the vector, with range [0, infinity)
		 * @param element The element which will be copied into all places of the resized vector
		 */
		void assign(const size_t size, const T& element);

		/**
		 * Returns the number of elements of this vector.
		 * @return The vector's number of elements, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Returns the overall capacity of this vector (including the capacity on the stack and on the heap).
		 * @return The vector's capacity, with range [size(), infinity)
		 */
		inline size_t capacity() const;

		/**
		 * Returns whether this vector is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Clears this vector.
		 * All stack elements will be overwritten with default values.
		 */
		void clear();

		/**
		 * Sets the capacity of this vector to a specified number of elements.
		 * In case the specified capacity is smaller than the current capacity or is smaller than the number of elements in this vector, nothing happens.
		 * @param capacity The capacity to set, with range [0, infinity)
		 */
		void setCapacity(const size_t capacity);

		/**
		 * Returns the first element of this vector.
		 * Ensure that the vector is not empty before calling this function.
		 * @return The vector's first element
		 */
		const T& front() const;

		/**
		 * Returns the first element of this vector.
		 * Ensure that the vector is not empty before calling this function.
		 * @return The vector's first element
		 */
		T& front();

		/**
		 * Returns the last element of this vector.
		 * Ensure that the vector is not empty before calling this function.
		 * @return The vector's last element
		 */
		const T& back() const;

		/**
		 * Returns the last element of this vector.
		 * Ensure that the vector is not empty before calling this function.
		 * @return The vector's last element
		 */
		T& back();

		/**
		 * Returns an iterator to the first element in this vector.
		 * @return The vector's iterator to the first element
		 */
		Iterator begin();

		/**
		 * Returns an iterator to the element following the last element in this vector.
		 * @return The vector's iterator to the element following the last element
		 */
		Iterator end();

		/**
		 * Returns a const iterator to the first element in this vector.
		 * @return The vector's iterator to the first element
		 */
		ConstIterator begin() const;

		/**
		 * Returns an iterator to the element following the last element in this vector.
		 * @return The vector's iterator to the element following the last element
		 */
		ConstIterator end() const;

		/**
		 * Elements access operator.
		 * @param index The index of the element to access, with range [0, size() - 1]
		 * @return The element with specified index
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Elements access operator.
		 * @param index The index of the element to access, with range [0, size() - 1]
		 * @return The element with specified index
		 */
		inline T& operator[](const size_t index);

	protected:

		/// The elements located on the stack.
		T stackElements_[tStackCapacity];

		/// The remaining elements located on the heap.
		std::vector<T> heapElements_;

		/// The number of elements in this vector.
		size_t size_ = 0;
};

template <typename T, size_t tStackCapacity>
StackHeapVector<T, tStackCapacity>::Iterator::Iterator(StackHeapVector& vector, const size_t index) :
	vector_(vector),
	index_(index)
{
	ocean_assert(index <= vector_.size_);
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::Iterator& StackHeapVector<T, tStackCapacity>::Iterator::operator++()
{
	++index_;

	return *this;
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::Iterator StackHeapVector<T, tStackCapacity>::Iterator::operator++(int)
{
	Iterator iterator(*this);

	++index_;

	return iterator;
}

template <typename T, size_t tStackCapacity>
T& StackHeapVector<T, tStackCapacity>::Iterator::operator*()
{
	return vector_[index_];
}

template <typename T, size_t tStackCapacity>
bool StackHeapVector<T, tStackCapacity>::Iterator::operator==(const Iterator& iterator) const
{
	ocean_assert(&vector_ == &iterator.vector_);

	return &vector_ == &iterator.vector_ && index_ == iterator.index_;
}

template <typename T, size_t tStackCapacity>
bool StackHeapVector<T, tStackCapacity>::Iterator::operator!=(const Iterator& iterator) const
{
	return !(*this == iterator);
}

template <typename T, size_t tStackCapacity>
StackHeapVector<T, tStackCapacity>::ConstIterator::ConstIterator(const StackHeapVector& vector, const size_t index) :
	vector_(vector),
	index_(index)
{
	ocean_assert(index <= vector_.size_);
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::ConstIterator& StackHeapVector<T, tStackCapacity>::ConstIterator::operator++()
{
	++index_;

	return *this;
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::ConstIterator StackHeapVector<T, tStackCapacity>::ConstIterator::operator++(int)
{
	Iterator iterator(*this);

	++index_;

	return iterator;
}

template <typename T, size_t tStackCapacity>
const T& StackHeapVector<T, tStackCapacity>::ConstIterator::operator*() const
{
	return vector_[index_];
}

template <typename T, size_t tStackCapacity>
bool StackHeapVector<T, tStackCapacity>::ConstIterator::operator==(const ConstIterator& iterator) const
{
	ocean_assert(&vector_ == &iterator.vector_);

	return &vector_ == &iterator.vector_ && index_ == iterator.index_;
}

template <typename T, size_t tStackCapacity>
bool StackHeapVector<T, tStackCapacity>::ConstIterator::operator!=(const ConstIterator& iterator) const
{
	return !(*this == iterator);
}

template <typename T, size_t tStackCapacity>
StackHeapVector<T, tStackCapacity>::StackHeapVector()
{
	// nothing to do here
}

template <typename T, size_t tStackCapacity>
StackHeapVector<T, tStackCapacity>::StackHeapVector(const size_t size, const T& element)
{
	setCapacity(size);

	for (size_t n = 0; n < size; ++n)
	{
		pushBack(element);
	}
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::pushBack(T&& element)
{
	if (size_ < tStackCapacity)
	{
		stackElements_[size_] = std::move(element);
	}
	else
	{
		heapElements_.emplace_back(std::move(element));
	}

	++size_;
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::pushBack(const T& element)
{
	if (size_ < tStackCapacity)
	{
		stackElements_[size_] = element;
	}
	else
	{
		heapElements_.push_back(element);
	}

	++size_;
}

template <typename T, size_t tStackCapacity>
template <typename... TArgs>
T& StackHeapVector<T, tStackCapacity>::emplaceBack(TArgs&&... args)
{
	const size_t index = size_;

	++size_;

	if (index < tStackCapacity)
	{
		stackElements_[index] = T(std::forward<TArgs>(args)...);

		return stackElements_[index];
	}
	else
	{
		return heapElements_.emplace_back(std::forward<TArgs>(args)...);
	}
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::popBack()
{
	ocean_assert(size_ >= 1);

	--size_;

	if (size_ >= tStackCapacity)
	{
		heapElements_.pop_back();
	}
	else
	{
		stackElements_[size_] = T();
	}
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::resize(const size_t size)
{
	if (size == size_)
	{
		return;
	}

	if (size < size_)
	{
		// we have to remove elements

		for (size_t n = size; n < tStackCapacity; ++n) // in case size >= tStackCapacity, nothing happens
		{
			stackElements_[n] = T();
		}

		if (size_ > tStackCapacity)
		{
			if (size < tStackCapacity)
			{
				heapElements_.clear();
			}
			else
			{
				heapElements_.resize(size - tStackCapacity);
			}
		}
	}
	else
	{
		ocean_assert(size > size_);

		if (size > tStackCapacity)
		{
			heapElements_.resize(size - tStackCapacity);
		}
	}

	size_ = size;
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::assign(const size_t size, const T& element)
{
	for (size_t n = 0; n < std::min(size, tStackCapacity); ++n) // we assign as many elements in the stack
	{
		stackElements_[n] = element;
	}

	for (size_t n = size; n < std::min(tStackCapacity, size_); ++n) // if necessary (if the new size is smaller than the previous size), we overwrite stack elements with default values
	{
		stackElements_[n] = T();
	}

	if (size < tStackCapacity)
	{
		heapElements_.clear();
	}
	else
	{
		const size_t newHeapSize = size - tStackCapacity;

		heapElements_.assign(newHeapSize, element);
	}

	size_ = size;
}

template <typename T, size_t tStackCapacity>
inline size_t StackHeapVector<T, tStackCapacity>::size() const
{
	ocean_assert(size_ <= tStackCapacity || size_ == tStackCapacity + heapElements_.size());

	return size_;
}

template <typename T, size_t tStackCapacity>
inline size_t StackHeapVector<T, tStackCapacity>::capacity() const
{
	return tStackCapacity + heapElements_.capacity();
}

template <typename T, size_t tStackCapacity>
inline bool StackHeapVector<T, tStackCapacity>::isEmpty() const
{
	return size() == 0;
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::clear()
{
	for (size_t nStack = 0; nStack < std::min(size_, tStackCapacity); ++nStack)
	{
		stackElements_[nStack] = T();
	}

	heapElements_.clear();

	size_ = 0;
}

template <typename T, size_t tStackCapacity>
void StackHeapVector<T, tStackCapacity>::setCapacity(const size_t capacity)
{
	if (capacity > size_)
	{
		if (capacity > tStackCapacity)
		{
			heapElements_.reserve(capacity - tStackCapacity);
		}
	}
}

template <typename T, size_t tStackCapacity>
const T& StackHeapVector<T, tStackCapacity>::front() const
{
	ocean_assert(!isEmpty());

	return stackElements_[0];
}

template <typename T, size_t tStackCapacity>
T& StackHeapVector<T, tStackCapacity>::front()
{
	ocean_assert(!isEmpty());

	return stackElements_[0];
}

template <typename T, size_t tStackCapacity>
const T& StackHeapVector<T, tStackCapacity>::back() const
{
	ocean_assert(!isEmpty());

	if (size_ <= tStackCapacity)
	{
		return stackElements_[size_ - 1];
	}
	else
	{
		return heapElements_.back();
	}
}

template <typename T, size_t tStackCapacity>
T& StackHeapVector<T, tStackCapacity>::back()
{
	ocean_assert(!isEmpty());

	if (size_ <= tStackCapacity)
	{
		return stackElements_[size_ - 1];
	}
	else
	{
		return heapElements_.back();
	}
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::Iterator StackHeapVector<T, tStackCapacity>::begin()
{
	return Iterator(*this, 0);
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::Iterator StackHeapVector<T, tStackCapacity>::end()
{
	return Iterator(*this, size_);
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::ConstIterator StackHeapVector<T, tStackCapacity>::begin() const
{
	return ConstIterator(*this, 0);
}

template <typename T, size_t tStackCapacity>
typename StackHeapVector<T, tStackCapacity>::ConstIterator StackHeapVector<T, tStackCapacity>::end() const
{
	return ConstIterator(*this, size_);
}

template <typename T, size_t tStackCapacity>
inline const T& StackHeapVector<T, tStackCapacity>::operator[](const size_t index) const
{
	ocean_assert(index < size());

	if (index < tStackCapacity)
	{
		return stackElements_[index];
	}

	return heapElements_[index - tStackCapacity];
}

template <typename T, size_t tStackCapacity>
inline T& StackHeapVector<T, tStackCapacity>::operator[](const size_t index)
{
	ocean_assert(index < size());

	if (index < tStackCapacity)
	{
		return stackElements_[index];
	}

	return heapElements_[index - tStackCapacity];
}

}

#endif // META_OCEAN_BASE_STACK_HEAP_VECTOR_H
