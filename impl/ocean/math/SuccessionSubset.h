/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SUCCESSION_SUBSET_H
#define META_OCEAN_MATH_SUCCESSION_SUBSET_H

#include "ocean/math/Math.h"
#include "ocean/math/Variance.h"

#include "ocean/base/StaticBuffer.h"

#include <limits>

namespace Ocean
{

/**
 * This class implements a data container for abstract data objects with several dimensions.
 * The container allows to extract a subset of the data objects with largest distance to the remaining objects in the container.<br>
 * The distance between two objects is determined by the Euclidean distance while each dimension is normalized by the object's standard deviation of all data objects.<br>
 * Each object has the same dimension and each element of the objects has the same data type.<br>
 * Use this class to find a subset of e.g. camera poses, matrices or vectors so that each object of the subset has the largest distance to all objects of the entire set.<br>
 * The set of objects should be set during the construction of a SuccessionSubset object or by application of SuccessionSubset::setObjects().<br>
 * Beware: Set the entire set of objects before determine the subset.<br>
 * @tparam T Data type of the individual objects of each object
 * @tparam tDimensions Number of dimension that each object has
 * @ingroup math
 */
template <typename T, size_t tDimensions>
class SuccessionSubset
{
	public:

		/**
		 * The definition of an abstract object of this constainer.
		 */
		typedef StaticBuffer<T, tDimensions> Object;

		/**
		 * Definition of a vector holding indices.
		 */
		typedef std::vector<size_t> Indices;

	protected:

		/**
		 * Definition of a vector holding abstract objects.
		 */
		typedef std::vector<Object> Objects;

		/**
		 * Definition of a vector holding flags.
		 */
		typedef std::vector<unsigned char> Flags;

	public:

		/**
		 * Creates an empty container object.
		 */
		SuccessionSubset();

		/**
		 * Creates a new container and provides a set of objects that will be managed by this container.
		 * @param objects Objects that will be copied
		 * @param size Number of given objects
		 */
		SuccessionSubset(const Object* objects, const size_t size);

		/**
		 * Returns the dimension of each object of this container.
		 * @return Dimensions of each object, tDimension is returned
		 */
		static inline size_t dimensions();

		/**
		 * Returns the number of objects that are managed by this container
		 * @return Object number
		 */
		inline size_t size() const;

		/**
		 * Determines the next object of this container that has the largest distance to all remaining objects that are stored by this container.
		 * The determined object is added to the internal subset, further the index of the object will be returned.<br>
		 * @return Index of the object with largest distance, returns -1 if the subset holds already all objects
		 */
		size_t incrementSubset();

		/**
		 * Explicitly selects one object of this container so that is will be added to the internal subset.
		 * Use this function only if one specific object should be part of the final subset.<br>
		 * The object must not be part of the current subset.<br>
		 * @param object Index of the object that is selected explicitly, with range [0, size())
		 * @return True, if succeeded
		 */
		bool incrementSubset(const size_t object);

		/**
		 * Returns one object of this container.
		 * @param index The index of the requested object, with range [0, size())
		 * @return The requested object
		 */
		inline const Object& object(const size_t index);

		/**
		 * Returns the current object subset of this container.
		 * @return Indices of the objects that belong to the subset of this container
		 */
		inline const Indices& subset() const;

		/**
		 * Returns a subset of the stored elements with specified size.
		 * If the internal subset is larger than the requested size, the entire subset is returned.<br>
		 * If the internal subset is smaller than the requested size, the internal subset will be incremented until the requested size is reached.<br>
		 * @param size Size of the requested subset, with range [1, size())
		 * @return Indices of the elements of the requested subset
		 */
		const Indices& subset(const size_t size);

		/**
		 * Overwrites all objects of this container and resets the current subset of the previous object to zero.
		 * @param objects New object to be copied into this container
		 * @param size Number of given objects
		 */
		void setObjects(const Object* objects, const size_t size);

		/**
		 * Returns whether this container is empty and thus does not store any object.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this container holds at least one object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Converts the indices of this object to 32 bit indices.
		 * @param indices Indices to convert
		 * @return The converted indices
		 */
		static inline Indices32 indices2indices32(const Indices& indices);

	protected:

		/**
		 * Returns the elements of this container that is not part of the subset and that has the smallest maximal distance to all remaining objects in the container.
		 * @return Index of the resulting object
		 */
		size_t smallestMaximalDistance() const;

		/**
		 * Returns the elements of this container that is not part of the subset and that has the largest minimal distance to all subset objects in the container.
		 * @return Index of the resulting object
		 */
		size_t largestMinimalDistanceWithSubset() const;

		/**
		 * Determines the distance between two objects of this container.
		 * @param a Index of the first object, with range [0, size())
		 * @param b Index of the second object
		 * @return Resulting distance between both objects
		 */
		T distance(const size_t a, const size_t b) const;

		/**
		 * Returns the maximal distance between a specified object of this container and all remaining objects of this container.
		 * @param index Index of the object for that the minimal distance will be determined, with range [0, size())
		 * @return Resulting smallest distance
		 */
		T maximalDistance(const size_t index) const;

		/**
		 * Returns the smallest distance between a specified object of this container and all subset objects of this container.
		 * @param index Index of the object for that the minimal distance will be determined, with range [0, size())
		 * @return Resulting smallest distance
		 */
		T minimalDistanceWithSubset(const size_t index) const;

	protected:

		/// All objects of this container.
		Objects successionObjects;

		/// Objects flags, for a fast check whether an objects is part of the internal subset.
		Flags successionFlags;

		/// The indices of all objects inside the subset.
		Indices successionSubset;
};

template <typename T, size_t tDimensions>
SuccessionSubset<T, tDimensions>::SuccessionSubset()
{
	// nothing to do here
}

template <typename T, size_t tDimensions>
SuccessionSubset<T, tDimensions>::SuccessionSubset(const Object* objects, const size_t size)
{
	setObjects(objects, size);
}

template <typename T, size_t tDimensions>
inline size_t SuccessionSubset<T, tDimensions>::dimensions()
{
	return tDimensions;
}

template <typename T, size_t tDimensions>
inline size_t SuccessionSubset<T, tDimensions>::size() const
{
	return successionObjects.size();
}

template <typename T, size_t tDimensions>
size_t SuccessionSubset<T, tDimensions>::incrementSubset()
{
	ocean_assert(successionObjects.size() == successionFlags.size());

	size_t index = size_t(-1);

	if (successionSubset.size() == successionObjects.size())
	{
		return size_t(-1);
	}

	ocean_assert(successionSubset.size() < successionObjects.size());

	if (successionSubset.size() == 0)
	{
		index = smallestMaximalDistance();
	}
	else
	{
		index = largestMinimalDistanceWithSubset();
	}

	if (index == size_t(-1))
	{
		return size_t(-1);
	}

	ocean_assert(index < successionFlags.size());
	successionFlags[index] = 1;
	successionSubset.push_back(index);

	return index;
}

template <typename T, size_t tDimensions>
bool SuccessionSubset<T, tDimensions>::incrementSubset(const size_t object)
{
	ocean_assert(successionObjects.size() == successionFlags.size());

	if (object >= successionObjects.size() || successionFlags[object] != 0)
	{
		return false;
	}

	successionFlags[object] = 1;
	successionSubset.push_back(object);

	return true;
}

template <typename T, size_t tDimensions>
inline const typename SuccessionSubset<T, tDimensions>::Object& SuccessionSubset<T, tDimensions>::object(const size_t index)
{
	ocean_assert(index < successionObjects.size());
	return successionObjects[index];
}

template <typename T, size_t tDimensions>
inline const typename SuccessionSubset<T, tDimensions>::Indices& SuccessionSubset<T, tDimensions>::subset() const
{
	return successionSubset;
}

template <typename T, size_t tDimensions>
const typename SuccessionSubset<T, tDimensions>::Indices& SuccessionSubset<T, tDimensions>::subset(const size_t objects)
{
	while (successionSubset.size() < objects && successionSubset.size() < successionObjects.size())
	{
		const size_t index = incrementSubset();
		ocean_assert_and_suppress_unused(index < successionObjects.size(), index);
	}

	return successionSubset;
}

template <typename T, size_t tDimensions>
void SuccessionSubset<T, tDimensions>::setObjects(const Object* objects, const size_t size)
{
	StaticBuffer<VarianceT<T>, tDimensions> variances;

	for (size_t n = 0; n < size; ++n)
	{
		for (size_t i = 0; i < tDimensions; ++i)
		{
			variances[i].add(objects[n][i]);
		}
	}

	// copy normalized objects
	successionObjects.resize(size);
	successionFlags = Flags(size, 0u);
	successionSubset.clear();

	if (size == 0)
	{
		return;
	}

	StaticBuffer<T, tDimensions> normalizationFactors;
	for (size_t i = 0; i < tDimensions; ++i)
	{
		normalizationFactors[i] = variances[i].deviation() > 0 ? (T(1) / variances[i].deviation()) : T(1);
	}

	for (size_t n = 0; n < size; ++n)
	{
		for (size_t i = 0; i < tDimensions; ++i)
		{
			successionObjects[n][i] = objects[n][i] * normalizationFactors[i];
		}
	}
}

template <typename T, size_t tDimensions>
inline bool SuccessionSubset<T, tDimensions>::isEmpty() const
{
	return successionObjects.empty();
}

template <typename T, size_t tDimensions>
inline SuccessionSubset<T, tDimensions>::operator bool() const
{
	ocean_assert(successionObjects.size() == successionFlags.size());
	return successionSubset.size() < successionObjects.size();
}

template <typename T, size_t tDimensions>
inline Indices32 SuccessionSubset<T, tDimensions>::indices2indices32(const Indices& indices)
{
	Indices32 result(indices.size());

	for (size_t n = 0; n < indices.size(); ++n)
	{
		ocean_assert(indices[n] <= NumericT<Index32>::maxValue());
		result[n] = Index32(indices[n]);
	}

	return result;
}

template <typename T, size_t tDimensions>
size_t SuccessionSubset<T, tDimensions>::smallestMaximalDistance() const
{
	ocean_assert(successionSubset.size() < successionObjects.size());

	size_t smallestIndex = size_t(-1);

	for (size_t n = 0; n < successionObjects.size(); ++n)
	{
		if (successionFlags[n] == 0)
		{
			smallestIndex = n;
			break;
		}
	}

	// determine the distance for the selected object
	T smallestDistance = maximalDistance(smallestIndex);

	// now check the distance for all remaining objects
	for (size_t n = smallestIndex + 1; n < successionObjects.size(); ++n)
	{
		if (successionFlags[n] == 0)
		{
			const T localDistance = maximalDistance(n);

			if (localDistance < smallestDistance)
			{
				smallestDistance = localDistance;
				smallestIndex = n;
			}
		}
	}

	return smallestIndex;
}

template <typename T, size_t tDimensions>
size_t SuccessionSubset<T, tDimensions>::largestMinimalDistanceWithSubset() const
{
	ocean_assert(successionSubset.size() < successionObjects.size());

	size_t largestIndex = size_t(-1);

	for (size_t n = 0; n < successionObjects.size(); ++n)
	{
		if (successionFlags[n] == 0)
		{
			largestIndex = n;
			break;
		}
	}

	// determine the distance for the selected object
	T largestDistance = minimalDistanceWithSubset(largestIndex);

	// now check the distance for all remaining objects
	for (size_t n = largestIndex + 1; n < successionObjects.size(); ++n)
	{
		if (successionFlags[n] == 0)
		{
			const T localDistance = minimalDistanceWithSubset(n);

			if (localDistance > largestDistance)
			{
				largestDistance = localDistance;
				largestIndex = n;
			}
		}
	}

	return largestIndex;
}

template <typename T, size_t tDimensions>
T SuccessionSubset<T, tDimensions>::distance(const size_t a, const size_t b) const
{
	ocean_assert(a < successionObjects.size());
	ocean_assert(b < successionObjects.size());

	T result = 0;

	for (size_t i = 0; i < tDimensions; ++i)
	{
		result += NumericT<T>::sqr(successionObjects[a][i] - successionObjects[b][i]);
	}

	return result;
}

template <typename T, size_t tDimensions>
T SuccessionSubset<T, tDimensions>::maximalDistance(const size_t index) const
{
	ocean_assert(index < successionObjects.size());
	ocean_assert(successionObjects.size() >= 2);

	T maxDistance = NumericT<T>::minValue();

	// determine the distance for the selected object
	for (size_t n = 0; n < successionObjects.size(); ++n)
	{
		if (n != index)
		{
			const T localDistance = distance(n, index);

			if (localDistance > maxDistance)
				maxDistance = localDistance;
		}
	}

	ocean_assert(maxDistance != NumericT<T>::minValue());
	return maxDistance;
}

template <typename T, size_t tDimensions>
T SuccessionSubset<T, tDimensions>::minimalDistanceWithSubset(const size_t index) const
{
	ocean_assert(index < successionObjects.size());
	ocean_assert(successionSubset.size() >= 1);

	T minDistance = NumericT<T>::maxValue();

	// determine the distance for the selected object
	for (size_t n = 0; n < successionSubset.size(); ++n)
	{
		ocean_assert(index != successionSubset[n]);
		const T localDistance = distance(successionSubset[n], index);

		if (localDistance < minDistance)
		{
			minDistance = localDistance;
		}
	}

	ocean_assert(minDistance != NumericT<T>::maxValue());
	return minDistance;
}

}

#endif // META_OCEAN_MATH_SUCCESSION_SUBSET_H
