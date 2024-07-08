/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_PLANE_FINDER_H
#define META_OCEAN_TRACKING_PLANE_FINDER_H

#include "ocean/tracking/Tracking.h"
#include "ocean/tracking/CorrespondenceSet.h"

#include "ocean/geometry/Error.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a 3D plane finder without any previous knowledge about the plane or the camera poses.
 * The class takes several correspondences of image points that capture corresponding 3D object points lying on the same 3D plane.<br>
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT PlaneFinder
{
	protected:

		/**
		 * Definition of a correspondence set object with handling 2D vectors.
		 */
		typedef CorrespondenceSet<Vector2> ImagePointCorrespondenceSet;

	public:

		/**
		 * Returns the number of stored correspondence sets.
		 * @return Correspondence set size
		 */
		inline size_t size() const;

		/**
		 * Returns the number of image points within each individual correspondence set.
		 * @return Number of image points
		 */
		inline size_t imagePoints() const;

		/**
		 * Returns the first set of image points.
		 * Beware: Check whether this object holds at least on set of image points before you call this function.
		 * @return Image points
		 */
		const Vectors2& firstImagePoints() const;

		/**
		 * Returns the last set of image points.
		 * Beware: Check whether this object holds at least on set of image points before you call this function.
		 * @return Image points
		 */
		inline const Vectors2& lastImagePoints() const;

		/**
		 * Calculates the summed square distance between the first image points and the corresponding image points in the most recent set of image points.
		 * @return Resulting summed square distance
		 */
		inline Scalar sqrDistance() const;

		/**
		 * Adds new image points as new set of correspondences.
		 * Beware: The number of provided image points must match with the number of already stored image points within each individual set of correspondences.
		 * @param imagePoints Image points to be added
		 * @return True, if succeeded
		 */
		virtual bool addImagePoint(const Vectors2& imagePoints);

		/**
		 * Adds new image points as new set of correspondences.
		 * Beware: The number of provided image points must match with the number of already stored image points within each individual set of correspondences.
		 * @param imagePoints Image points to be added (and moved)
		 * @return True, if succeeded
		 */
		virtual bool addImagePoint(Vectors2&& imagePoints);

		/**
		 * Adds a new subset of image points that corresponds to a subset of the stored sets of image points.
		 * This function takes a set of indices which define the valid subset of the given image points.<br>
		 * Only valid image points will be added while also the already stored sets of image points will be reduced so that only valid elements are stored finally.<br>
		 * @param imagePoints Large set of image points to be added
		 * @param validIndices Indices that define a valid subset of the given elements, each index must exist at most once and must lie inside the range [0, elements.size())
		 * @return True, if succeeded
		 */
		virtual bool addImagePoint(const Vectors2& imagePoints, const Indices32& validIndices);

		/**
		 * Reduces the image points within each set of corresponding image points.
		 * The remaining elements are defined by a set of indices.<br>
		 * @param validIndices Indices that define a valid subset of the already stored elements, each index must exist at most once and must lie inside the range [0, elements())
		 * @return True, if succeeded
		 */
		inline bool reduce(const Indices32& validIndices);

	protected:

		/**
		 * Creates a new plane finder object.
		 */
		inline PlaneFinder();

		/**
		 * Destructs this object.
		 */
		virtual ~PlaneFinder() = default;

	protected:

		/// The set of image point correspondences.
		ImagePointCorrespondenceSet imagePointCorrespondences;
};

inline PlaneFinder::PlaneFinder()
{
	// nothing to do here
}

inline size_t PlaneFinder::size() const
{
	return imagePointCorrespondences.size();
}

inline size_t PlaneFinder::imagePoints() const
{
	return imagePointCorrespondences.elements();
}

inline const Vectors2& PlaneFinder::firstImagePoints() const
{
	ocean_assert(!imagePointCorrespondences.correspondences().empty());
	return imagePointCorrespondences.correspondences().front();
}

inline const Vectors2& PlaneFinder::lastImagePoints() const
{
	ocean_assert(!imagePointCorrespondences.correspondences().empty());
	return imagePointCorrespondences.correspondences().back();
}

inline Scalar PlaneFinder::sqrDistance() const
{
	const ImagePointCorrespondenceSet::ElementsVector& correspondences = imagePointCorrespondences.correspondences();

	if (correspondences.size() <= 1)
		return 0;

	return Geometry::Error::determineAverageError(correspondences.front(), correspondences.back());
}

inline bool PlaneFinder::reduce(const Indices32& validIndices)
{
	return imagePointCorrespondences.reduce(validIndices);
}

}

}

#endif // META_OCEAN_TRACKING_PLANE_FINDER_H
