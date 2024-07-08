/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_CORRESPONDENCE_SET_H
#define META_OCEAN_TRACKING_CORRESPONDENCE_SET_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Subset.h"

#include <set>

namespace Ocean
{

namespace Tracking
{

/**
 * This class manages a set of element correspondences.
 * @tparam T Data type of the individual elements for that the set of correspondences will be stored
 * @ingroup tracking
 */
template <typename T>
class CorrespondenceSet
{
	public:

		/**
		 * Defines the data type of the individual elements.
		 */
		typedef T Type;

		/**
		 * This class defines a vector storing individual elements.
		 */
		typedef std::vector<T> Elements;

		/**
		 * This class defines a vector storing vectors of elements.
		 */
		typedef std::vector<Elements> ElementsVector;

	public:

		/**
		 * Creates an empty object.
		 */
		inline CorrespondenceSet();

		/**
		 * Creates new object by adding the first elements.
		 * @param elements First elements that will be stored as first reference elements
		 */
		inline explicit CorrespondenceSet(const Elements& elements);

		/**
		 * Creates new object by adding the first elements.
		 * @param elements First elements that will be stored as first reference elements
		 */
		inline explicit CorrespondenceSet(Elements&& elements);

		/**
		 * Move constructor.
		 * @param correspondenceSet Correspondence object to be moved
		 */
		inline CorrespondenceSet(CorrespondenceSet<T>&& correspondenceSet) noexcept;

		/**
		 * Returns the number of correspondences.
		 * @return Correspondence number
		 */
		inline size_t size() const;

		/**
		 * Returns the number of corresponding elements.
		 * @return Number of elements
		 */
		inline size_t elements() const;

		/**
		 * Returns the set of stored correspondences.
		 * @return Set of correspondences
		 */
		inline const ElementsVector& correspondences() const;

		/**
		 * Changes (or sets) the first set of elements.
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object, if elements have been set already.<br>
		 * @param elements Elements that replace the existing ones
		 * @return True, if succeeded
		 */
		inline bool setFirstElements(const Elements& elements);

		/**
		 * Changes (or sets) the first set of elements.
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object, if elements have been set already.<br>
		 * @param elements Elements that replace the existing ones
		 * @return True, if succeeded
		 */
		inline bool setFirstElements(Elements&& elements);

		/**
		 * Changes (or sets) the first set of elements.
		 * This function takes a set of indices which define the valid subset of the given elements.<br>
		 * Only valid elements will be added while also the already stored sets of elements will be reduced so that only valid elements are stored finally.<br>
		 * @param elements Elements that replace the existing ones
		 * @param validIndices Indices that define a valid subset of the given elements, each index must exist at most once and must lie inside the range [0, elements.size())
		 * @return True, if succeeded
		 */
		bool setFirstElements(const Elements& elements, const Indices32& validIndices);

		/**
		 * Changes (or sets) the last set of elements.
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object, if elements have been set already.<br>
		 * @param elements Elements that replace the existing ones
		 * @return True, if succeeded
		 */
		inline bool setLastElements(const Elements& elements);

		/**
		 * Changes (or sets) the last set of elements.
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object, if elements have been set already.<br>
		 * @param elements Elements that replace the existing ones
		 * @return True, if succeeded
		 */
		inline bool setLastElements(Elements&& elements);

		/**
		 * Changes (or sets) the last set of elements.
		 * This function takes a set of indices which define the valid subset of the given elements.<br>
		 * Only valid elements will be added while also the already stored sets of elements will be reduced so that only valid elements are stored finally.<br>
		 * @param elements Elements that replace the existing ones
		 * @param validIndices Indices that define a valid subset of the given elements, each index must exist at most once and must lie inside the range [0, elements.size())
		 * @return True, if succeeded
		 */
		bool setLastElements(const Elements& elements, const Indices32& validIndices);

		/**
		 * Adds a new set of elements that corresponds to the stored sets of elements.
		 * Element correspondence is defined by the index of the given elements.<br>
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object.<br>
		 * @param elements Elements to be added
		 * @return True, if succeeded
		 */
		inline bool addElements(const Elements& elements);

		/**
		 * Adds a new set of elements that corresponds to the stored sets of elements.
		 * Element correspondence is defined by the index of the given elements.<br>
		 * Beware: The number of provided elements must match to the number of stored elements in each set of this object.<br>
		 * @param elements Elements to be added
		 * @return True, if succeeded
		 */
		inline bool addElements(Elements&& elements);

		/**
		 * Adds a new subset of elements that corresponds to a subset of the stored sets of elements.
		 * Element correspondence is defined by the index of the given elements.<br>
		 * This function takes a set of indices which define the valid subset of the given elements.<br>
		 * Only valid elements will be added while also the already stored sets of elements will be reduced so that only valid elements are stored finally.<br>
		 * @param elements Elements to be added
		 * @param validIndices Indices that define a valid subset of the given elements, each index must exist at most once and must lie inside the range [0, elements.size())
		 * @return True, if succeeded
		 */
		bool addElements(const Elements& elements, const Indices32& validIndices);

		/**
		 * Reduces the elements within each set of corresponding elements.
		 * The remaining elements are defined by a set of indices.<br>
		 * @param validIndices Indices that define a valid subset of the already stored elements, each index must exist at most once and must lie inside the range [0, elements())
		 * @return True, if succeeded
		 */
		bool reduce(const Indices32& validIndices);

		/**
		 * Returns whether this object does not hold any set of elements.
		 * Beware: This state does not say anything about the number of elements that are stored in the individual sets.<br>
		 * To ensure that this object holds a valid number of sets and a valid number of elements check also elements().<br>
		 * @return True, if so
		 * @see elements().
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this object holds at least one set of elements.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param right Second object to be moved
		 * @return Reference to this object
		 */
		inline CorrespondenceSet<T>& operator=(CorrespondenceSet<T>&& right) noexcept;

	protected:

		/**
		 * Returns whether this object stores sets of elements with same number of elements.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// The set of corresponding elements.
		ElementsVector correspondenceElementsSet;
};

template <typename T>
inline CorrespondenceSet<T>::CorrespondenceSet()
{
	// nothing to do here
}

template <typename T>
inline CorrespondenceSet<T>::CorrespondenceSet(const Elements& elements) :
	correspondenceElementsSet(1, elements)
{
	// nothing to do here
}

template <typename T>
inline CorrespondenceSet<T>::CorrespondenceSet(Elements&& elements) :
	correspondenceElementsSet(1, elements)
{
	// nothing to do here
}

template <typename T>
inline CorrespondenceSet<T>::CorrespondenceSet(CorrespondenceSet<T>&& correspondenceSet) noexcept :
	correspondenceElementsSet(std::move(correspondenceSet.correspondenceElementsSet))
{
	// nothing to do here
}

template <typename T>
inline size_t CorrespondenceSet<T>::size() const
{
	ocean_assert(isValid());
	return correspondenceElementsSet.size();
}

template <typename T>
inline size_t CorrespondenceSet<T>::elements() const
{
	ocean_assert(isValid());
	return correspondenceElementsSet.empty() ? 0 : correspondenceElementsSet.front().size();
}

template <typename T>
inline const typename CorrespondenceSet<T>::ElementsVector& CorrespondenceSet<T>::correspondences() const
{
	ocean_assert(isValid());
	return correspondenceElementsSet;
}

template <typename T>
inline bool CorrespondenceSet<T>::setFirstElements(const Elements& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	if (correspondenceElementsSet.empty())
		correspondenceElementsSet.push_back(elements);
	else
		correspondenceElementsSet.front() = elements;

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::setFirstElements(Elements&& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	if (correspondenceElementsSet.empty())
		correspondenceElementsSet.push_back(elements);
	else
		correspondenceElementsSet.front() = elements;

	return true;
}

template <typename T>
bool CorrespondenceSet<T>::setFirstElements(const Elements& elements, const Indices32& validIndices)
{
	ocean_assert(isValid() && !elements.empty());

	ocean_assert(elements.size() >= validIndices.size());
	ocean_assert(CorrespondenceSet<T>::elements() == 0 || elements.size() == CorrespondenceSet<T>::elements());

	if (elements.size() < validIndices.size() || (CorrespondenceSet<T>::elements() != 0 && elements.size() != CorrespondenceSet<T>::elements()))
		return false;

#ifdef OCEAN_DEBUG

	const IndexSet32 indexSet(validIndices.begin(), validIndices.end());

	// ensure that each index exists only once
	ocean_assert(indexSet.size() == validIndices.size());

	if (elements.size() == validIndices.size())
		ocean_assert(*indexSet.rbegin() == validIndices.size() - 1);
	else
	{
		for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
			ocean_assert(*i < elements.size());
	}

#endif // OCEAN_DEBUG

	if (elements.size() == validIndices.size())
	{
		if (correspondenceElementsSet.empty())
			correspondenceElementsSet.push_back(elements);
		else
			correspondenceElementsSet.front() = elements;
	}
	else
	{
		for (typename ElementsVector::iterator i = correspondenceElementsSet.begin(); i != correspondenceElementsSet.end(); ++i)
			*i = std::move(Subset::subset(*i, validIndices));

		if (correspondenceElementsSet.empty())
			correspondenceElementsSet.push_back(std::move(Subset::subset(elements, validIndices)));
		else
			correspondenceElementsSet.front() = std::move(Subset::subset(elements, validIndices));
	}

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::setLastElements(const Elements& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	if (correspondenceElementsSet.empty())
		correspondenceElementsSet.push_back(elements);
	else
		correspondenceElementsSet.back() = elements;

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::setLastElements(Elements&& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	if (correspondenceElementsSet.empty())
		correspondenceElementsSet.push_back(elements);
	else
		correspondenceElementsSet.back() = elements;

	return true;
}

template <typename T>
bool CorrespondenceSet<T>::setLastElements(const Elements& elements, const Indices32& validIndices)
{
	ocean_assert(isValid() && !elements.empty());

	ocean_assert(elements.size() >= validIndices.size());
	ocean_assert(CorrespondenceSet<T>::elements() == 0 || elements.size() == CorrespondenceSet<T>::elements());

	if (elements.size() < validIndices.size() || (CorrespondenceSet<T>::elements() != 0 && elements.size() != CorrespondenceSet<T>::elements()))
		return false;

#ifdef OCEAN_DEBUG

	const IndexSet32 indexSet(validIndices.begin(), validIndices.end());

	// ensure that each index exists only once
	ocean_assert(indexSet.size() == validIndices.size());

	if (elements.size() == validIndices.size())
		ocean_assert(*indexSet.rbegin() == validIndices.size() - 1);
	else
	{
		for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
			ocean_assert(*i < elements.size());
	}

#endif // OCEAN_DEBUG

	if (elements.size() == validIndices.size())
	{
		if (correspondenceElementsSet.empty())
			correspondenceElementsSet.push_back(elements);
		else
			correspondenceElementsSet.back() = elements;
	}
	else
	{
		for (typename ElementsVector::iterator i = correspondenceElementsSet.begin(); i != correspondenceElementsSet.end(); ++i)
			*i = std::move(Subset::subset(*i, validIndices));

		if (correspondenceElementsSet.empty())
			correspondenceElementsSet.push_back(std::move(Subset::subset(elements, validIndices)));
		else
			correspondenceElementsSet.back() = std::move(Subset::subset(elements, validIndices));
	}

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::addElements(const Elements& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	correspondenceElementsSet.push_back(elements);

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::addElements(Elements&& elements)
{
	ocean_assert(isValid());
	ocean_assert(correspondenceElementsSet.empty() || correspondenceElementsSet.front().size() == elements.size());

	if (!correspondenceElementsSet.empty() && correspondenceElementsSet.front().size() != elements.size())
		return false;

	correspondenceElementsSet.push_back(elements);

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::addElements(const Elements& elements, const Indices32& validIndices)
{
	ocean_assert(isValid() && !elements.empty());

	ocean_assert(elements.size() >= validIndices.size());
	ocean_assert(CorrespondenceSet<T>::elements() == 0 || elements.size() == CorrespondenceSet<T>::elements());

	if (elements.size() < validIndices.size() || (CorrespondenceSet<T>::elements() != 0 && elements.size() != CorrespondenceSet<T>::elements()))
		return false;

#ifdef OCEAN_DEBUG

	const IndexSet32 indexSet(validIndices.begin(), validIndices.end());

	// ensure that each index exists only once
	ocean_assert(indexSet.size() == validIndices.size());

	if (elements.size() == validIndices.size())
		ocean_assert(*indexSet.rbegin() == validIndices.size() - 1);
	else
	{
		for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
			ocean_assert(*i < elements.size());
	}

#endif // OCEAN_DEBUG

	if (elements.size() == validIndices.size())
		correspondenceElementsSet.push_back(elements);
	else
	{
		for (typename ElementsVector::iterator i = correspondenceElementsSet.begin(); i != correspondenceElementsSet.end(); ++i)
			*i = std::move(Subset::subset(*i, validIndices));

		correspondenceElementsSet.push_back(std::move(Subset::subset(elements, validIndices)));
	}

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::reduce(const Indices32& validIndices)
{
	ocean_assert(isValid());

	ocean_assert(elements() >= validIndices.size());

	if (elements() < validIndices.size())
		return false;

#ifdef OCEAN_DEBUG

	const IndexSet32 indexSet(validIndices.begin(), validIndices.end());

	// ensure that each index exists only once
	ocean_assert(indexSet.size() == validIndices.size());

	if (elements() == validIndices.size())
		ocean_assert(*indexSet.rbegin() == validIndices.size() - 1);
	else
	{
		for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
			ocean_assert(*i < elements());
	}

#endif // OCEAN_DEBUG

	if (elements() != validIndices.size())
	{
		for (typename ElementsVector::iterator i = correspondenceElementsSet.begin(); i != correspondenceElementsSet.end(); ++i)
			*i = std::move(Subset::subset(*i, validIndices));
	}

	return true;
}

template <typename T>
inline bool CorrespondenceSet<T>::isEmpty() const
{
	ocean_assert(isValid());
	return correspondenceElementsSet.empty();
}

template <typename T>
inline CorrespondenceSet<T>::operator bool() const
{
	ocean_assert(isValid());
	return !correspondenceElementsSet.empty();
}

template <typename T>
inline bool CorrespondenceSet<T>::isValid() const
{
	for (size_t n = 1; n < correspondenceElementsSet.size(); ++n)
		if (correspondenceElementsSet[0].size() != correspondenceElementsSet[n].size())
			return false;

	return true;
}

template <typename T>
inline CorrespondenceSet<T>& CorrespondenceSet<T>::operator=(CorrespondenceSet<T>&& right) noexcept
{
	ocean_assert(isValid() && right.isValid());

	if (this != &right)
		correspondenceElementsSet = std::move(right.correspondenceElementsSet);

	return *this;
}

}

}

#endif // META_OCEAN_TRACKING_CORRESPONDENCE_SET_H
