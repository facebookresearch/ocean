/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SUBSET_H
#define META_OCEAN_BASE_SUBSET_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class implements subset functions.
 * @ingroup base
 */
class Subset
{
	private:

		/**
		 * This helper class implements a subset functions.
		 * @tparam TIndex Data type of the index elements
		 */
		template <typename TIndex>
		class InternalSubset
		{
			public:

				/**
				 * Extracts a subset of a given set of objects by usage of an index vector holding the indices of all objects to be used.
				 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param indices Indices defining the subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> subset(const std::vector<T>& objects, const std::vector<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param indices Indices defining the subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> subset(const std::vector<T>& objects, const std::set<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param indices Indices defining the inverted subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> invertedSubset(const std::vector<T>& objects, const std::unordered_set<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param indices Indices defining the inverted subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> invertedSubset(const std::vector<T>& objects, const std::set<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
				 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param numberObjects Number of given objects
				 * @param indices Indices defining the subset to be extracted
				 * @param numberIndices Number of given indices
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const TIndex* indices, const size_t numberIndices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
				 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param numberObjects Number of given objects
				 * @param indices Indices defining the subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const std::vector<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param numberObjects Number of given objects
				 * @param indices Indices defining the subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param numberObjects Number of given objects
				 * @param indices Indices defining the inverted subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> invertedSubset(const T* objects, const size_t numberObjects, const std::unordered_set<TIndex>& indices);

				/**
				 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
				 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
				 * @param objects Entire set of objects from that a subset will be extracted
				 * @param numberObjects Number of given objects
				 * @param indices Indices defining the inverted subset to be extracted
				 * @return Resulting subset of the given objects
				 * @tparam T Data type of the objects
				 */
				template <typename T>
				static inline std::vector<T> invertedSubset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices);

				/**
				 * Converts object indices to an uint8_t vector holding statements for each object.
				 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
				 * @param numberObjects Number of objects that can be addressed by the indices
				 * @return Resulting (boolean != 0u) statements
				 * @tparam TContainer The container to be used, e.g., std::vector, std::set, std::unordred_set
				 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
				 */
				template <typename TContainer, uint8_t tValue>
				static inline std::vector<uint8_t> indices2statements(const TContainer& indices, const size_t numberObjects);

				/**
				 * Converts object indices to an uint8_t vector holding statements for each object.
				 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
				 * @param numberIndices Number of provided indices
				 * @param numberObjects Number of objects that can be addressed by the indices
				 * @return Resulting boolean statements
				 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
				 */
				template <uint8_t tValue>
				static inline std::vector<uint8_t> indices2statements(const TIndex* indices, const size_t numberIndices, const size_t numberObjects);

				/**
				 * Converts an uint8_t vector holding statements for each object into object indices.
				 * @param statements Boolean statement for each object
				 * @return Resulting object indices
				 * @tparam tValue Value for objects that will be defined in the indices
				 */
				template <uint8_t tValue>
				static inline std::vector<TIndex> statements2indices(const std::vector<uint8_t>& statements);

				/**
				 * Converts an uint8_t vector holding statements for each object into object indices.
				 * @param statements Boolean statement for each object
				 * @param numberStatements Number of statements
				 * @return Resulting object indices
				 * @tparam tValue Value for objects that will be defined in the indices
				 */
				template <uint8_t tValue>
				static inline std::vector<TIndex> statements2indices(const uint8_t* statements, const size_t numberStatements);
			};

	public:

		/**
		 * Extracts a subset of a given set of objects by usage of an index vector holding the indices of all objects to be used.
		 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param indices Indices defining the subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> subset(const std::vector<T>& objects, const std::vector<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param indices Indices defining the subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> subset(const std::vector<T>& objects, const std::set<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param indices Indices defining the inverted subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> invertedSubset(const std::vector<T>& objects, const std::unordered_set<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param indices Indices defining the inverted subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> invertedSubset(const std::vector<T>& objects, const std::set<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
		 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param numberObjects Number of given objects
		 * @param indices Indices defining the subset to be extracted
		 * @param numberIndices Number of given indices
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const TIndex* indices, const size_t numberIndices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
		 * If the index data type is an uint8_t, than the (boolean != 0u) statement is used to use the corresponding object or not.<br>
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param numberObjects Number of given objects
		 * @param indices Indices defining the subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const std::vector<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param numberObjects Number of given objects
		 * @param indices Indices defining the subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> subset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices);

		/**
		 * Extracts the indices that are not given within a set indices.
		 * @param indices The set of given indices, the resulting indices will not contain any of these indices, can contain indices with range [0, maximalIndex]
		 * @param numberElements The number of possible elements defining the entire range of possible indices: [0, numberElements)
		 * @tparam TIndex Data type of the index elements
		 */
		template <typename TIndex>
		static std::vector<TIndex> invertedIndices(const std::vector<TIndex>& indices, const size_t numberElements);

		/**
		 * Extracts the indices that are not given within a set indices.
		 * @param indices The set of given indices, the resulting indices will not contain any of these indices, can contain indices with range [0, maximalIndex]
		 * @param numberElements The number of possible elements defining the entire range of possible indices: [0, numberElements)
		 * @tparam TIndex Data type of the index elements
		 */
		template <typename TIndex>
		static std::unordered_set<TIndex> invertedIndices(const std::unordered_set<TIndex>& indices, const size_t numberElements);

		/**
		 * Extracts the indices that are not given within a set indices.
		 * @param indices The set of given indices, the resulting indices will not contain any of these indices, can contain indices with range [0, maximalIndex]
		 * @param numberElements The number of possible elements defining the entire range of possible indices: [0, numberElements)
		 * @tparam TIndex Data type of the index elements
		 */
		template <typename TIndex>
		static std::set<TIndex> invertedIndices(const std::set<TIndex>& indices, const size_t numberElements);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param numberObjects Number of given objects
		 * @param indices Indices defining the inverted subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> invertedSubset(const T* objects, const size_t numberObjects, const std::unordered_set<TIndex>& indices);

		/**
		 * Extracts a subset of a given set of objects by usage of a set of indices of all objects to be not used.
		 * Beware: No range check is done! Thus, each index must not exceed the number of given objects.<br>
		 * @param objects Entire set of objects from that a subset will be extracted
		 * @param numberObjects Number of given objects
		 * @param indices Indices defining the inverted subset to be extracted
		 * @return Resulting subset of the given objects
		 * @tparam TIndex Data type of the index elements
		 * @tparam T Data type of the objects
		 */
		template <typename TIndex, typename T>
		static inline std::vector<T> invertedSubset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices);

		/**
		 * Converts object indices to an uint8_t vector holding statements for each object.
		 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
		 * @param numberObjects Number of objects that can be addressed by the indices
		 * @return Resulting boolean statements
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<uint8_t> indices2statements(const std::vector<TIndex>& indices, const size_t numberObjects);

		/**
		 * Converts object indices to an uint8_t vector holding statements for each object.
		 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
		 * @param numberObjects Number of objects that can be addressed by the indices
		 * @return Resulting boolean statements
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<uint8_t> indices2statements(const std::set<TIndex>& indices, const size_t numberObjects);

		/**
		 * Converts object indices to an uint8_t vector holding statements for each object.
		 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
		 * @param numberObjects Number of objects that can be addressed by the indices
		 * @return Resulting boolean statements
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<uint8_t> indices2statements(const std::unordered_set<TIndex>& indices, const size_t numberObjects);

		/**
		 * Converts object indices to an uint8_t vector holding statements for each object.
		 * @param indices Indices defining the subset of the objects, with range [0, numberObjects)
		 * @param numberIndices Number of provided indices
		 * @param numberObjects Number of objects that can be addressed by the indices
		 * @return Resulting boolean statements
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that are defined in the indices, the inverse value is applied otherwise
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<uint8_t> indices2statements(const TIndex* indices, const size_t numberIndices, const size_t numberObjects);

		/**
		 * Converts an uint8_t vector holding statements for each object into object indices.
		 * @param statements Boolean statement for each object
		 * @return Resulting object indices
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that will be defined in the indices
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<TIndex> statements2indices(const std::vector<uint8_t>& statements);

		/**
		 * Converts an uint8_t vector holding statements for each object into object indices.
		 * @param statements Boolean statement for each object
		 * @param numberStatements Number of statements
		 * @return Resulting object indices
		 * @tparam TIndex Data type of the index elements
		 * @tparam tValue Value for objects that will be defined in the indices
		 */
		template <typename TIndex, uint8_t tValue>
		static inline std::vector<TIndex> statements2indices(const uint8_t* statements, const size_t numberStatements);

		/**
		 * Determines corresponding element pairs from two sets of element maps.
		 * Two elements correspond with each other if they have the same key.
		 * @param elementMapA The first map of elements
		 * @param elementMapB The second map of elements
		 * @param elementsA The resulting elements from the first map which have a corresponding element in the second map
		 * @param elementsB The resulting elements from the second map which have a corresponding element in the first map
		 * @tparam TKey The data type of each key
		 * @tparam TElement The data type of each element
		 */
		template <typename TKey, typename TElement>
		static void correspondingElements(const std::map<TKey, TElement>& elementMapA, const std::map<TKey, TElement>& elementMapB, std::vector<TElement>& elementsA, std::vector<TElement>& elementsB);

		/**
		 * Determines whether two (ordered) sets have at least one intersecting element.
		 * Both input sets must be in ascending order.
		 * @param firstA The iterator to the first element in the first set
		 * @param endA The iterator to the (exclusive) element after the last element in the first set
		 * @param firstB The iterator to the first element in the second set
		 * @param endB The iterator to the (exclusive) element after the last element in the second set
		 * @return True, if so
		 */
		template <typename TIterator>
		static bool hasIntersectingElement(const TIterator& firstA, const TIterator& endA, const TIterator& firstB, const TIterator& endB);

		/**
		 * Determines whether two (ordered) sets have at least one intersecting element.
		 * @param setA The first set to check, can be empty
		 * @param setB The second set to check, can be empty
		 * @return True, if so
		 */
		template <typename T>
		static inline bool hasIntersectingElement(const std::set<T>& setA, const std::set<T>& setB);

		/**
		 * Determines whether two ordered vectors have at least one intersecting element.
		 * @param sortedA The first vector to check, can be empty
		 * @param sortedB The second vector to check, can be empty
		 * @return True, if so
		 */
		template <typename T>
		static inline bool hasIntersectingElement(const std::vector<T>& sortedA, const std::vector<T>& sortedB);
};

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::subset(const std::vector<T>& objects, const std::vector<TIndex>& indices)
{
	std::vector<T> result;
	result.reserve(indices.size());

	for (const TIndex& index : indices)
	{
		ocean_assert(size_t(index) < objects.size());
		result.push_back(objects[size_t(index)]);
	}

	return result;
}

template <>
template <typename T>
inline std::vector<T> Subset::InternalSubset<uint8_t>::subset(const std::vector<T>& objects, const std::vector<uint8_t>& indices)
{
	std::vector<T> result;
	result.reserve(indices.size());

	typename std::vector<T>::const_iterator iObject = objects.begin();

	for (const uint8_t index : indices)
	{
		if (index != 0u)
		{
			result.push_back(*iObject);
		}

		++iObject;
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::subset(const std::vector<T>& objects, const std::set<TIndex>& indices)
{
	std::vector<T> result;
	result.reserve(indices.size());

	for (typename std::set<TIndex>::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		ocean_assert(*i < objects.size());
		result.push_back(objects[size_t(*i)]);
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::invertedSubset(const std::vector<T>& objects, const std::unordered_set<TIndex>& indices)
{
	ocean_assert(objects.size() >= indices.size());
	ocean_assert((unsigned long long)(objects.size()) < (unsigned long long)(std::numeric_limits<TIndex>::max()));

	if (objects.size() == indices.size())
	{
#ifdef OCEAN_DEBUG
		for (const TIndex& index : indices)
		{
			ocean_assert(index < objects.size());
		}
#endif

		return std::vector<T>();
	}

	std::vector<T> result;
	result.reserve(objects.size() - indices.size());

	for (size_t n = 0u; n < objects.size(); ++n)
	{
		if (indices.find(TIndex(n)) == indices.cend())
		{
			result.push_back(objects[n]);
		}
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::invertedSubset(const std::vector<T>& objects, const std::set<TIndex>& indices)
{
	ocean_assert(objects.size() >= indices.size());
	ocean_assert((unsigned long long)(objects.size()) < (unsigned long long)(std::numeric_limits<TIndex>::max()));

	if (objects.size() == indices.size())
	{
#ifdef OCEAN_DEBUG
		for (const TIndex& index : indices)
		{
			ocean_assert(index < objects.size());
		}
#endif

		return std::vector<T>();
	}

	std::vector<T> result;
	result.reserve(objects.size() - indices.size());

	for (size_t n = 0u; n < objects.size(); ++n)
	{
		if (indices.find(TIndex(n)) == indices.cend())
		{
			result.push_back(objects[n]);
		}
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::subset(const T* objects, const size_t numberObjects, const TIndex* indices, const size_t numberIndices)
{
	std::vector<T> result;
	result.reserve(numberIndices);

	for (size_t n = 0; n < numberIndices; ++n)
	{
		ocean_assert_and_suppress_unused(indices[n] < numberObjects, numberObjects);

		result.push_back(objects[indices[n]]);
	}

	return result;
}

template <>
template <typename T>
inline std::vector<T> Subset::InternalSubset<uint8_t>::subset(const T* objects, const size_t numberObjects, const uint8_t* indices, const size_t numberIndices)
{
	ocean_assert_and_suppress_unused(numberIndices <= numberObjects, numberObjects);

	std::vector<T> result;
	result.reserve(numberIndices);

	for (size_t n = 0; n < numberIndices; ++n)
	{
		if (indices[n] != 0u)
		{
			result.push_back(objects[n]);
		}
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::subset(const T* objects, const size_t numberObjects, const std::vector<TIndex>& indices)
{
	return subset(objects, numberObjects, indices.data(), indices.size());
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::subset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices)
{
	std::vector<T> result;
	result.reserve(indices.size());

	for (const TIndex& index : indices)
	{
		ocean_assert_and_suppress_unused(index < numberObjects, numberObjects);

		result.push_back(objects[index]);
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::invertedSubset(const T* objects, const size_t numberObjects, const std::unordered_set<TIndex>& indices)
{
	ocean_assert(numberObjects >= indices.size());
	ocean_assert((unsigned long long)(numberObjects) < (unsigned long long)(std::numeric_limits<TIndex>::max()));

	if (numberObjects == indices.size())
	{
#ifdef OCEAN_DEBUG
		for (const TIndex& index : indices)
		{
			ocean_assert(indices < numberObjects);
		}
#endif

		return std::vector<T>();
	}

	std::vector<T> result;
	result.reserve(numberObjects - indices.size());

	for (size_t n = 0; n < numberObjects; ++n)
	{
		if (indices.find(TIndex(n)) == indices.cend())
		{
			result.push_back(objects[n]);
		}
	}

	return result;
}

template <typename TIndex>
template <typename T>
inline std::vector<T> Subset::InternalSubset<TIndex>::invertedSubset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices)
{
	ocean_assert(numberObjects >= indices.size());
	ocean_assert((unsigned long long)(numberObjects) < (unsigned long long)(std::numeric_limits<TIndex>::max()));

	if (numberObjects == indices.size())
	{
#ifdef OCEAN_DEBUG
		for (const TIndex& index : indices)
		{
			ocean_assert(index < numberObjects);
		}
#endif

		return std::vector<T>();
	}

	std::vector<T> result;
	result.reserve(numberObjects - indices.size());

	for (size_t n = 0; n < numberObjects; ++n)
	{
		if (indices.find(TIndex(n)) == indices.cend())
		{
			result.push_back(objects[n]);
		}
	}

	return result;
}

template <typename TIndex>
template <typename TContainer, uint8_t tValue>
inline std::vector<uint8_t> Subset::InternalSubset<TIndex>::indices2statements(const TContainer& indices, const size_t numberObjects)
{
	std::vector<uint8_t> result(numberObjects, !tValue);

	for (const TIndex& index : indices)
	{
		ocean_assert(index < result.size());
		result[index] = tValue;
	}

	return result;
}

template <typename TIndex>
template <uint8_t tValue>
inline std::vector<uint8_t> Subset::InternalSubset<TIndex>::indices2statements(const TIndex* indices, const size_t numberIndices, const size_t numberObjects)
{
	std::vector<uint8_t> result(numberObjects, !tValue);

	for (size_t n = 0; n < numberIndices; ++n)
	{
		ocean_assert(indices[n] < result.size());
		result[indices[n]] = tValue;
	}

	return result;
}

template <typename TIndex>
template <uint8_t tValue>
inline std::vector<TIndex> Subset::InternalSubset<TIndex>::statements2indices(const std::vector<uint8_t>& statements)
{
	std::vector<TIndex> result;

	for (size_t n = 0; n < statements.size(); ++n)
	{
		if (statements[n] == tValue)
		{
			result.push_back(TIndex(n));
		}
	}

	return result;
}

template <typename TIndex>
template <uint8_t tValue>
inline std::vector<TIndex> Subset::InternalSubset<TIndex>::statements2indices(const uint8_t* statements, const size_t numberStatements)
{
	std::vector<TIndex> result;

	for (size_t n = 0; n < numberStatements; ++n)
	{
		if (statements[n] == tValue)
		{
			result.push_back(TIndex(n));
		}
	}

	return result;
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::subset(const std::vector<T>& objects, const std::vector<TIndex>& indices)
{
	return InternalSubset<TIndex>::template subset<T>(objects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::subset(const std::vector<T>& objects, const std::set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template subset<T>(objects, indices);
}

template <typename TIndex>
std::vector<TIndex> Subset::invertedIndices(const std::vector<TIndex>& indices, const size_t numberElements)
{
	ocean_assert(indices.size() <= TIndex(numberElements));

	if (indices.size() == numberElements)
	{
#ifdef OCEAN_DEBUG
		const std::unordered_set<TIndex> indexSet(indices.begin(), indices.end());
		ocean_assert(indexSet.size() == indices.size());

		for (size_t n = 0; n < numberElements; ++n)
		{
			ocean_assert(indexSet.find(TIndex(n)) != indexSet.cend());
		}
#endif

		return std::vector<TIndex>();
	}

	const std::unordered_set<TIndex> indexSet(indices.begin(), indices.end());
	ocean_assert(indexSet.size() == indices.size());

	std::vector<TIndex> result;
	result.reserve(numberElements - indices.size());

	for (TIndex i = TIndex(0); i < TIndex(numberElements); ++i)
	{
		if (indexSet.find(i) == indexSet.cend())
		{
			result.push_back(i);
		}
	}

	return result;
}

template <typename TIndex>
std::unordered_set<TIndex> Subset::invertedIndices(const std::unordered_set<TIndex>& indices, const size_t numberElements)
{
	ocean_assert(indices.size() <= TIndex(numberElements));

	if (indices.size() == numberElements)
	{
#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < numberElements; ++n)
		{
			ocean_assert(indices.find(TIndex(n)) != indices.end());
		}
#endif

		return std::unordered_set<TIndex>();
	}

	std::unordered_set<TIndex> result;
	result.reserve(numberElements - indices.size());

	for (TIndex i = TIndex(0); i < TIndex(numberElements); ++i)
	{
		if (indices.find(i) == indices.cend())
		{
			result.emplace(i);
		}
	}

	return result;
}

template <typename TIndex>
std::set<TIndex> Subset::invertedIndices(const std::set<TIndex>& indices, const size_t numberElements)
{
	ocean_assert(indices.size() <= TIndex(numberElements));

	if (indices.size() == numberElements)
	{
#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < numberElements; ++n)
		{
			ocean_assert(indices.find(TIndex(n)) != indices.end());
		}
#endif

		return std::set<TIndex>();
	}

	std::set<TIndex> result;

	for (TIndex i = TIndex(0); i < TIndex(numberElements); ++i)
	{
		if (indices.find(i) == indices.cend())
		{
			result.insert(i);
		}
	}

	return result;
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::invertedSubset(const std::vector<T>& objects, const std::unordered_set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template invertedSubset<T>(objects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::invertedSubset(const std::vector<T>& objects, const std::set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template invertedSubset<T>(objects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::subset(const T* objects, const size_t numberObjects, const TIndex* indices, const size_t numberIndices)
{
	return InternalSubset<TIndex>::template subset<T>(objects, numberObjects, indices, numberIndices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::subset(const T* objects, const size_t numberObjects, const std::vector<TIndex>& indices)
{
	return InternalSubset<TIndex>::template subset<T>(objects, numberObjects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::subset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template subset<T>(objects, numberObjects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::invertedSubset(const T* objects, const size_t numberObjects, const std::unordered_set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template invertedSubset<T>(objects, numberObjects, indices);
}

template <typename TIndex, typename T>
inline std::vector<T> Subset::invertedSubset(const T* objects, const size_t numberObjects, const std::set<TIndex>& indices)
{
	return InternalSubset<TIndex>::template invertedSubset<T>(objects, numberObjects, indices);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<uint8_t> Subset::indices2statements(const std::vector<TIndex>& indices, const size_t numberObjects)
{
	return InternalSubset<TIndex>::template indices2statements<std::vector<TIndex>, tValue>(indices, numberObjects);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<uint8_t> Subset::indices2statements(const std::unordered_set<TIndex>& indices, const size_t numberObjects)
{
	return InternalSubset<TIndex>::template indices2statements<std::unordered_set<TIndex>, tValue>(indices, numberObjects);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<uint8_t> Subset::indices2statements(const std::set<TIndex>& indices, const size_t numberObjects)
{
	return InternalSubset<TIndex>::template indices2statements<std::set<TIndex>, tValue>(indices, numberObjects);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<uint8_t> Subset::indices2statements(const TIndex* indices, const size_t numberIndices, const size_t numberObjects)
{
	return InternalSubset<TIndex>::template indices2statements<tValue>(indices, numberIndices, numberObjects);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<TIndex> Subset::statements2indices(const std::vector<uint8_t>& statements)
{
	return InternalSubset<TIndex>::template statements2indices<tValue>(statements);
}

template <typename TIndex, uint8_t tValue>
inline std::vector<TIndex> Subset::statements2indices(const uint8_t* statements, const size_t numberStatements)
{
	return InternalSubset<TIndex>::template statements2indices<tValue>(statements, numberStatements);
}

template <typename TKey, typename TElement>
void Subset::correspondingElements(const std::map<TKey, TElement>& elementMapA, const std::map<TKey, TElement>& elementMapB, std::vector<TElement>& elementsA, std::vector<TElement>& elementsB)
{
	elementsA.clear();
	elementsB.clear();

	// we expect that the map entries are sorted based on their keys
	ocean_assert(elementMapA.size() < 2 || elementMapA.cbegin()->first < elementMapA.crbegin()->first);
	ocean_assert(elementMapB.size() < 2 || elementMapB.cbegin()->first < elementMapB.crbegin()->first);

	typename std::map<TKey, TElement>::const_iterator iA = elementMapA.cbegin();
	typename std::map<TKey, TElement>::const_iterator iB = elementMapB.cbegin();

	while (iA != elementMapA.cend() && iB != elementMapB.cend())
	{
		// let's find elements with identical keys

		if (iA->first > iB->first)
		{
			++iB;
		}
		else if (iA->first < iB->first)
		{
			++iA;
		}
		else
		{
			ocean_assert(iA->first == iB->first);

			elementsA.emplace_back(iA->second);
			elementsB.emplace_back(iB->second);

			++iA;
			++iB;
		}
	}
}

template <typename TIterator>
bool Subset::hasIntersectingElement(const TIterator& firstA, const TIterator& endA, const TIterator& firstB, const TIterator& endB)
{
	if (firstA == endA || firstB == endB)
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	TIterator debugLeftA = firstA;
	TIterator debugRightA = firstA;
	++debugRightA;

	TIterator debugLeftB = firstB;
	TIterator debugRightB = firstB;
	++debugRightB;

	while (debugRightA != endA && debugRightB != endB)
	{
		ocean_assert(*debugLeftA++ <= *debugRightA++);
		ocean_assert(*debugLeftB++ <= *debugRightB++);
	}
#endif

	TIterator lastA = endA;
	TIterator lastB = endB;

	if (*(--lastA) < *firstB || *(--lastB) < *firstA)
	{
		return false;
	}

	TIterator iA = firstA;
	TIterator iB = firstB;

	while (iA != endA && iB != endB)
	{
		if (*iA == *iB)
		{
			return true;
		}

		if (*iA < *iB)
		{
			++iA;
		}
		else
		{
			ocean_assert(*iA > *iB);
			++iB;
		}
	}

	return false;
}

template <typename T>
bool Subset::hasIntersectingElement(const std::set<T>& setA, const std::set<T>& setB)
{
	return hasIntersectingElement(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend());
}

template <typename T>
bool Subset::hasIntersectingElement(const std::vector<T>& sortedA, const std::vector<T>& sortedB)
{
	return hasIntersectingElement(sortedA.cbegin(), sortedA.cend(), sortedB.cbegin(), sortedB.cend());
}

}

#endif // META_OCEAN_BASE_SUBSET_H
