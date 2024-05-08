/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_HOMOGRAPHY_PLANE_FINDER_H
#define META_OCEAN_TRACKING_HOMOGRAPHY_PLANE_FINDER_H

#include "ocean/tracking/Tracking.h"
#include "ocean/tracking/PlaneFinder.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a 3D plane finder that determines the plane by calculation of a homography between two frames taken from two individual camera positions.
 * The resulting plane quality can be improved by using more than two frames.<br>
 * Two possible plane solutions will be provided.<br>
 * **NOTE** This class is almost obsolete we keep it for demonstration purpose only.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT HomographyPlaneFinder : public PlaneFinder
{
	public:

		/**
		 * Definition of a pair storing two corresponding normals.
		 */
		typedef std::pair<Vector3, Vector3> NormalPair;

	protected:

		/**
		 * Definition of a vector holding pairs of normals.
		 */
		typedef std::vector<NormalPair> NormalPairs;

	public:

		/**
		 * Creates a new plane finder object.
		 */
		inline HomographyPlaneFinder();

		/**
		 * Returns the first point set that is stored.
		 * @return First set of points
		 */
		inline const Vectors2& initialImagePoints() const;

		/**
		 * Returns the current point set that is stored.
		 * @return current set of points
		 */
		inline const Vectors2& currentImagePoints() const;

		/**
		 * Adds new image points as new set of correspondences.
		 * After new points have added the homography determination needs to be invoked.<br>
		 * @see PlaneFinder::addImagePoint(), applyHomographyDetermination().
		 */
		virtual bool addImagePoint(const Vectors2& imagePoints);

		/**
		 * Adds new image points as new set of correspondences.
		 * After new points have added the homography determination needs to be invoked.<br>
		 * @see PlaneFinder::addImagePoint(), applyHomographyDetermination().
		 */
		virtual bool addImagePoint(Vectors2&& imagePoints);

		/**
		 * Adds a new subset of image points that corresponds to a subset of the stored sets of image points.
		 * After new points have added the homography determination needs to be invoked.<br>
		 * @see PlaneFinder::addImagePoint(), applyHomographyDetermination().
		 */
		virtual bool addImagePoint(const Vectors2& imagePoints, const Indices32& validIndices);

		/**
		 * Determines a new set of plane candidates matching with the currently stored image point correspondences.
		 * The new candidates will be added to the large set of possible plane candidates finally allowing to determine the most reliable/accurate plane(s).
		 * @param pinhole The pinhole camera profile defining the projection, must be valid
		 * @return True, if succeeded
		 * @see determineMostAccuratePlanes().
		 */
		bool addPlaneCandidates(const PinholeCamera& pinhole);

		/**
		 * Checks whether one of the last three successive frames provided almost identical plane normals.
		 * @param maxAngle The maximal angle between two plane normals, in radian, with range [0, PI/2)
		 */
		bool hasAccuratePlane(const Scalar maxAngle);

		/**
		 * Determines the pair of planes which have been determined in the previous calls of addPlaneCandidates().
		 * @param planes The two resulting planes
		 * @return True, if succeeded
		 */
		bool determineMostAccuratePlanes(Plane3 planes[2]);

	protected:

		/**
		 * Determines the maximal cosine value between three pairs of plane normals.
		 * @param minus The previous pairs of plane normals
		 * @param center The current pairs of plane normals
		 * @param plus The next pairs of plane normals
		 * @return The maximal cosine value, with range [-1, 1]
		 */
		static Scalar maximalCosBetweenNormalPairs(const NormalPair& minus, const NormalPair& center, const NormalPair& plus);

		/**
		 * Determines the pair of normals with smallest angle.
		 * @param normalPairs The pairs of normals, at least 3
		 * @param index The resulting index of the best pairs, with range [0, normalPairs.size())
		 * @param angle The resulting angle between the best pairs of normals
		 * @return True, if succeeded
		 */
		static bool determineBestNormalPair(const NormalPairs& normalPairs, size_t& index, Scalar& angle);

	protected:

		/// Successive plane normal sets.
		NormalPairs planeFinderNormalPairs;

		/// Random number generator.
		RandomGenerator randomGenerator;

		/// Number of image points sets that have been added.
		size_t planeImagePointsSets;
};

inline HomographyPlaneFinder::HomographyPlaneFinder() :
	planeImagePointsSets(0)
{
	// nothing to do here
}

inline const Vectors2& HomographyPlaneFinder::initialImagePoints() const
{
	ocean_assert(!imagePointCorrespondences.isEmpty());
	return imagePointCorrespondences.correspondences().front();
}

inline const Vectors2& HomographyPlaneFinder::currentImagePoints() const
{
	ocean_assert(!imagePointCorrespondences.isEmpty());
	return imagePointCorrespondences.correspondences().back();
}

}

}

#endif // META_OCEAN_TRACKING_HOMOGRAPHY_PLANE_FINDER_H
