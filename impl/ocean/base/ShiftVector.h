/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SHIFT_VECTOR_H
#define META_OCEAN_BASE_SHIFT_VECTOR_H

#include "ocean/base/Base.h"

#include <deque>

namespace Ocean
{

/**
 * This class implements a vector with shifted elements.
 * The elements are shifted by a shift offset index so that the elements can be accessed by adding this shift offset to the normal element index.<br>
 * The shift offset can either be positive or negative.
 * @tparam T Data type of the elements that are stored
 * @ingroup base
 */
template <typename T>
class ShiftVector
{
	public:

		/**
		 * Definition of the data type of each element of this object.
		 */
		typedef T Type;

		/**
		 * Definition of an element index.
		 */
		typedef std::ptrdiff_t Index;

		/**
		 * Definition of a data iterator.
		 */
		typedef typename std::deque<T>::iterator Iterator;

		/**
		 * Definition of a const data iterator.
		 */
		typedef typename std::deque<T>::const_iterator ConstIterator;

	public:

		/**
		 * Creates a new shift vector object.
		 */
		ShiftVector() = default;

		/**
		 * Copy constructor.
		 * @param object Vector object to be copied
		 */
		ShiftVector(const ShiftVector<T>& object);

		/**
		 * Move constructor.
		 * @param object Vector object to be moved
		 */
		ShiftVector(ShiftVector<T>&& object) noexcept;

		/**
		 * Creates a new shift vector object.
		 * @param firstIndex The index of the first element of this vector
		 */
		explicit ShiftVector(const Index firstIndex);

		/**
		 * Creates a new shift vector object.
		 * @param firstIndex The index of the first element of this vector
		 * @param size Number of elements to be created
		 */
		ShiftVector(const Index firstIndex, const size_t size);

		/**
		 * Creates a new shift vector object.
		 * @param firstIndex The index of the first element of this vector
		 * @param size Number of elements to be created
		 * @param element Pattern element that will be copied as often as requested
		 */
		ShiftVector(const Index firstIndex, const size_t size, const T& element);

		/**
		 * Creates a new shift vector object and copies a specified number of elements.
		 * @param firstIndex The index of the first element of this vector
		 * @param elements The elements to be copied
		 * @param size Number of elements to be copied
		 */
		ShiftVector(const Index firstIndex, const T* elements, const size_t size);

		/**
		 * Returns the index of the first element of this object.
		 * @return Index of first element
		 */
		inline Index firstIndex() const;

		/**
		 * Returns the index of the last (including) element of this object.
		 * @return Index of last element
		 * @see endIndex().
		 */
		inline Index lastIndex() const;

		/**
		 * Returns the index of the element behind the last (excluding) element of this object.
		 * Thus, there does not exist an element with the returning index.<br>
		 * @return Index of ending element
		 * @see lastIndex()
		 */
		inline Index endIndex() const;

		/**
		 * Sets the index of the first element of this vector.
		 * The elements of this vector will be untouched, however the individual elements receive a new index due to the new shift offset.
		 * @param index The index of the first element
		 */
		inline void setFirstIndex(const Index index);

		/**
		 * Returns the element located at the first index.
		 * Beware: Ensure that this object holds at least one element before accessing the element.
		 * @return The first element
		 * @see firstIndex().
		 */
		inline const T& front() const;

		/**
		 * Returns the element located at the first index.
		 * Beware: Ensure that this object holds at least one element before accessing the element.
		 * @return The first element
		 * @see firstIndex().
		 */
		inline T& front();

		/**
		 * Returns the element located at the last (including) index.
		 * Beware: Ensure that this object holds at least one element before accessing the element.
		 * @return The last element
		 * @see lastIndex().
		 */
		inline const T& back() const;

		/**
		 * Returns the element located at the last (including) index.
		 * Beware: Ensure that this object holds at least one element before accessing the element.
		 * @return The first element
		 * @see lastIndex().
		 */
		inline T& back();

		/**
		 * Returns the number of elements that are stored by this object.
		 * @return Number of elements
		 */
		inline size_t size() const;

		/**
		 * Changes the number of elements of this vector.
		 * If the new size is greater than the current size, than new elements are inserted with default initialization of the data type handled by this vector.
		 * @param size New element number
		 */
		inline void resize(const size_t size);

		/**
		 * Changes the number of elements of this vector.
		 * If the new size is greater than the current size, than new elements are inserted and initialized as copies of the given pattern element.<br>
		 * @param size New element number
		 * @param element The pattern element that is copied to each new internal element if necessary
		 */
		inline void resize(const size_t size, const T& element);

		/**
		 * Adds a new element to the end of this vector.
		 * The internal shift offset is untouched.<br>
		 * @param element New element to be added
		 */
		inline void pushBack(const T& element);

		/**
		 * Adds a new element to the end of this vector.
		 * The internal shift offset is untouched.<br>
		 * @param element New element to be added
		 */
		inline void pushBack(T&& element);

		/**
		 * Adds a new element to the front of this vector.
		 * The internal shift offset will be decremented by 1 so that the indices of the existing elements remain unchanged.<br>
		 * @param element New element to be added
		 */
		inline void pushFront(const T& element);

		/**
		 * Adds a new element to the front of this vector.
		 * The internal shift offset will be decremented by 1 so that the indices of the existing elements remain unchanged.<br>
		 * @param element New element to be added
		 */
		inline void pushFront(T&& element);

		/**
		 * Removes an element from the end of this vector.
		 * The internal shift offset is untouched.<br>
		 */
		inline void popBack();

		/**
		 * Removes an element form the front of this vector.
		 * The internal shift offset will be incremented by 1 so that the indices of the remaining elements remain unchanged.<br>
		 */
		inline void popFront();

		/**
		 * Inserts (or overwrites) an element at a specific position of this vector.
		 * If the position is outside the current range of the vector (in negative or positive direction) than this vector will be extended accordingly.<br>
		 * Whenever the vector will be extended, all intermediate elements (elements at new indices not equal to the given index) are initialized with the default constructor of the data type of this vector.<br>
		 * Beware: If elements have to be added at the front, than the index of the first element will also be adjusted.<br>
		 * @param index The index of the element
		 * @param element The element to be inserted at the given index
		 */
		inline void insert(const Index index, const T& element);

		/**
		 * Inserts (or overwrites) an element at a specific position of this vector.
		 * If the position is outside the current range of the vector (in negative or positive direction) than this vector will be extended accordingly.<br>
		 * Whenever the vector will be extended, all intermediate elements (elements at new indices not equal to the given index) are initialized with the given intermediate element instance.<br>
		 * Beware: If elements have to be added at the front, than the index of the first element will also be adjusted.<br>
		 * @param index The index of the element
		 * @param element The element to be inserted at the given index
		 * @param intermediateElement The element that is copied to all intermediate elements (elements at new indices not equal to the given index)
		 */
		inline void insert(const Index index, const T& element, const T& intermediateElement);

		/**
		 * Inserts (or overwrites) an element at a specific position of this vector.
		 * If the position is outside the current range of the vector (in negative or positive direction) than this vector will be extended accordingly.<br>
		 * Whenever the vector will be extended, all intermediate elements (elements at new indices not equal to the given index) are initialized with the default constructor of the data type of this vector.<br>
		 * Beware: If elements have to be added at the front, than the index of the first element will also be adjusted.<br>
		 * @param index The index of the element
		 * @param element The element to be inserted at the given index
		 */
		inline void insert(const Index index, T&& element);

		/**
		 * Inserts (or overwrites) an element at a specific position of this vector.
		 * If the position is outside the current range of the vector (in negative or positive direction) than this vector will be extended accordingly.<br>
		 * Whenever the vector will be extended, all intermediate elements (elements at new indices not equal to the given index) are initialized with the given intermediate element instance.<br>
		 * Beware: If elements have to be added at the front, than the index of the first element will also be adjusted.<br>
		 * @param index The index of the element
		 * @param element The element to be inserted at the given index
		 * @param intermediateElement The element that is copied to all intermediate elements (elements at new indices not equal to the given index)
		 */
		inline void insert(const Index index, T&& element, const T& intermediateElement);

		/**
		 * Returns whether a specific index is valid for this vector and matches to the current offset layout.
		 * @param index The index to be checked
		 * @return True, if succeeded
		 */
		inline bool isValidIndex(const Index index) const;

		/**
		 * Returns whether this object holds no element.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Clears this object, the specified index shift will be untouched.
		 */
		inline void clear();

		/**
		 * Returns a vector storing the elements.
		 * @return Vector holding the elements
		 */
		std::vector<T> data() const;

		/**
		 * Returns the iterator for the first data element.
		 * @return Iterator for the first data element
		 */
		inline Iterator begin();

		/**
		 * Returns the iterator for the first data element.
		 * @return Iterator for the first data element
		 */
		inline ConstIterator begin() const;

		/**
		 * Returns the end iterator.
		 * @return Iterator for the end
		 */
		inline Iterator end();

		/**
		 * Returns the end iterator.
		 * @return Iterator for the end
		 */
		inline ConstIterator end() const;

		/**
		 * Assign operator.
		 * @param object Vector object to be copied
		 * @return Reference to this object
		 */
		ShiftVector<T>& operator=(const ShiftVector<T>& object);

		/**
		 * Move operator.
		 * @param object Vector object to be moved
		 * @return Reference to this object
		 */
		ShiftVector<T>& operator=(ShiftVector<T>&& object) noexcept;

		/**
		 * Returns one element of this object.
		 * @param index The index of the element to be returned, with range [firstIndex(), lastIndex()]
		 * @return Requested element
		 */
		inline const T& operator[](const Index index) const;

		/**
		 * Returns one element of this object.
		 * @param index The index of the element to be returned, with range [firstIndex(), lastIndex()]
		 * @return Requested element
		 */
		inline T& operator[](const Index index);

		/**
		 * Returns whether this object holds at least one element.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Compares two shift vector objects whether they are equal.
		 * Two shift vector object are equal if they have the same firstIndex() parameter, if they have the same size and if all elements are equal.<br>
		 * @param object Second shift vector object to be compared
		 * @return True, if so
		 */
		bool operator==(const ShiftVector<T>& object) const;

		/**
		 * Compares two shift vector object whether they are not equal.
		 * @param object Second shift vector object to be compared
		 * @return True, if so
		 */
		inline bool operator!=(const ShiftVector<T>& object) const;

	protected:

		/// The index of the first element.
		Index firstIndex_ = Index(0);

		/// Elements of this object.
		std::deque<T> elements_;
};

template <typename T>
ShiftVector<T>::ShiftVector(const ShiftVector& object) :
	firstIndex_(object.firstIndex_),
	elements_(object.elements_)
{
	// nothing to do here
}

template <typename T>
ShiftVector<T>::ShiftVector(ShiftVector&& object) noexcept
{
	*this = std::move(object);
}

template <typename T>
ShiftVector<T>::ShiftVector(const Index firstIndex) :
	firstIndex_(firstIndex)
{
	// nothing to do here
}

template <typename T>
ShiftVector<T>::ShiftVector(const Index firstIndex, const size_t size) :
	firstIndex_(firstIndex),
	elements_(size)
{
	// nothing to do here
}

template <typename T>
ShiftVector<T>::ShiftVector(const Index firstIndex, const size_t size, const T& element) :
	firstIndex_(firstIndex),
	elements_(size, element)
{
	// nothing to do here
}

template <typename T>
ShiftVector<T>::ShiftVector(const Index firstIndex, const T* elements, const size_t size) :
	firstIndex_(firstIndex),
	elements_(size)
{
	for (size_t n = 0; n < size; ++n)
	{
		elements_[n] = elements[n];
	}
}

template <typename T>
inline typename ShiftVector<T>::Index ShiftVector<T>::firstIndex() const
{
	return firstIndex_;
}

template <typename T>
inline typename ShiftVector<T>::Index ShiftVector<T>::lastIndex() const
{
	ocean_assert(!elements_.empty());

	if (elements_.empty())
	{
		return firstIndex_ - 1;
	}

	return firstIndex_ + Index(elements_.size()) - 1;
}

template <typename T>
inline typename ShiftVector<T>::Index ShiftVector<T>::endIndex() const
{
	ocean_assert(!elements_.empty());

	if (elements_.empty())
	{
		return firstIndex_;
	}

	return firstIndex_ + Index(elements_.size());
}

template <typename T>
inline void ShiftVector<T>::setFirstIndex(const Index index)
{
	firstIndex_ = index;
}

template <typename T>
inline const T& ShiftVector<T>::front() const
{
	ocean_assert(!elements_.empty());
	ocean_assert((*this)[firstIndex()] == elements_.front());

	return elements_.front();
}

template <typename T>
inline T& ShiftVector<T>::front()
{
	ocean_assert(!elements_.empty());
	ocean_assert((*this)[firstIndex()] == elements_.front());

	return elements_.front();
}

template <typename T>
inline const T& ShiftVector<T>::back() const
{
	ocean_assert(!elements_.empty());
	ocean_assert((*this)[lastIndex()] == elements_.back());

	return elements_.back();
}

template <typename T>
inline T& ShiftVector<T>::back()
{
	ocean_assert(!elements_.empty());
	ocean_assert((*this)[lastIndex()] == elements_.back());

	return elements_.back();
}

template <typename T>
inline size_t ShiftVector<T>::size() const
{
	return elements_.size();
}

template <typename T>
inline void ShiftVector<T>::resize(const size_t size)
{
	elements_.resize(size);
}

template <typename T>
inline void ShiftVector<T>::resize(const size_t size, const T& element)
{
	elements_.resize(size, element);
}

template <typename T>
inline void ShiftVector<T>::pushBack(const T& element)
{
	elements_.push_back(element);
}

template <typename T>
inline void ShiftVector<T>::pushBack(T&& element)
{
	elements_.push_back(element);
}

template <typename T>
inline void ShiftVector<T>::pushFront(const T& element)
{
	elements_.push_front(element);
	firstIndex_--;
}

template <typename T>
inline void ShiftVector<T>::pushFront(T&& element)
{
	elements_.push_front(element);
	firstIndex_--;
}

template <typename T>
inline void ShiftVector<T>::popBack()
{
	ocean_assert(!elements_.empty());
	elements_.pop_back();
}

template <typename T>
inline void ShiftVector<T>::popFront()
{
	ocean_assert(!elements_.empty());
	elements_.pop_front();
	firstIndex_++;
}

template <typename T>
inline void ShiftVector<T>::insert(const Index index, const T& element)
{
	if (index < firstIndex_)
	{
		// add default objects
		while (index + 1 < firstIndex_)
		{
			pushFront(T());
		}

		pushFront(element);
	}
	else
	{
		if (index >= (Index)(firstIndex_ + elements_.size()))
		{
			elements_.resize(index - firstIndex_ + 1);
		}

		elements_[index - firstIndex_] = element;
	}
}

template <typename T>
inline void ShiftVector<T>::insert(const Index index, const T& element, const T& intermediateElement)
{
	if (index < firstIndex_)
	{
		// add default objects
		while (index + 1 < firstIndex_)
		{
			pushFront(intermediateElement);
		}

		pushFront(element);
	}
	else
	{
		if (index >= (Index)(firstIndex_ + elements_.size()))
		{
			elements_.resize(index - firstIndex_ + 1, intermediateElement);
		}

		elements_[index - firstIndex_] = element;
	}
}


template <typename T>
inline void ShiftVector<T>::insert(const Index index, T&& element)
{
	if (index < firstIndex_)
	{
		// add default objects
		while (index + 1 < firstIndex_)
		{
			pushFront(T());
		}

		pushFront(element);
	}
	else
	{
		if (index >= (Index)(firstIndex_ + elements_.size()))
		{
			elements_.resize(index - firstIndex_ + 1);
		}

		elements_[index - firstIndex_] = std::move(element);
	}
}

template <typename T>
inline void ShiftVector<T>::insert(const Index index, T&& element, const T& intermediateElement)
{
	if (index < firstIndex_)
	{
		// add default objects
		while (index + 1 < firstIndex_)
		{
			pushFront(intermediateElement);
		}

		pushFront(element);
	}
	else
	{
		if (index >= (Index)(firstIndex_ + elements_.size()))
		{
			elements_.resize(index - firstIndex_ + 1, intermediateElement);
		}

		elements_[index - firstIndex_] = element;
	}
}

template <typename T>
inline bool ShiftVector<T>::isValidIndex(const Index index) const
{
	return index >= firstIndex_  && index < Index(firstIndex_ + elements_.size());
}

template <typename T>
inline bool ShiftVector<T>::isEmpty() const
{
	return elements_.empty();
}

template <typename T>
inline void ShiftVector<T>::clear()
{
	elements_.clear();
}

template <typename T>
std::vector<T> ShiftVector<T>::data() const
{
	return std::vector<T>(elements_.begin(), elements_.end());
}

template <typename T>
inline typename ShiftVector<T>::Iterator ShiftVector<T>::begin()
{
	return elements_.begin();
}

template <typename T>
inline typename ShiftVector<T>::ConstIterator ShiftVector<T>::begin() const
{
	return elements_.begin();
}

template <typename T>
inline typename ShiftVector<T>::Iterator ShiftVector<T>::end()
{
	return elements_.end();
}

template <typename T>
inline typename ShiftVector<T>::ConstIterator ShiftVector<T>::end() const
{
	return elements_.end();
}

template <typename T>
ShiftVector<T>& ShiftVector<T>::operator=(const ShiftVector<T>& object)
{
	firstIndex_ = object.firstIndex_;
	elements_ = object.elements_;

	return *this;
}

template <typename T>
ShiftVector<T>& ShiftVector<T>::operator=(ShiftVector<T>&& object) noexcept
{
	if (this != &object)
	{
		firstIndex_ = object.firstIndex_;
		object.firstIndex_ = 0;

		elements_ = std::move(object.elements_);
	}

	return *this;
}

template <typename T>
inline const T& ShiftVector<T>::operator[](const Index index) const
{
	ocean_assert(!isEmpty());
	ocean_assert(isValidIndex(index));
	ocean_assert(index >= firstIndex() && index <= lastIndex());

	return elements_[index - firstIndex_];
}

template <typename T>
inline T& ShiftVector<T>::operator[](const Index index)
{
	ocean_assert(!isEmpty());
	ocean_assert(isValidIndex(index));
	ocean_assert(index >= firstIndex() && index <= lastIndex());

	return elements_[index - firstIndex_];
}

template <typename T>
inline ShiftVector<T>::operator bool() const
{
	return !elements_.empty();
}

template <typename T>
bool ShiftVector<T>::operator==(const ShiftVector<T>& object) const
{
	return firstIndex_ == object.firstIndex_ && elements_ == object.elements_;
}

template <typename T>
inline bool ShiftVector<T>::operator!=(const ShiftVector<T>& object) const
{
	return !(*this == object);
}

}

#endif // META_OCEAN_BASE_SHIFT_VECTOR_H
