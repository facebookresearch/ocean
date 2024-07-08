/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_POINT_CORRESPONDENCES_H

#include "ocean/tracking/Tracking.h"

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements functions determining point correspondences or validates their accuracy.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT PointCorrespondences
{
	public:

		/**
		 * This class defines a correspondence object holding at most one correspondence candidate.
		 */
		class OCEAN_TRACKING_EXPORT Correspondence
		{
			public:

				/**
				 * Creates an empty correspondence object.
				 */
				inline Correspondence();

				/**
				 * Creates a new correspondence object.
				 * @param index Index of the interest point the candidate belongs to
				 * @param candidateIndex Index of the candidate point
				 * @param candidateSqrDistance Square distance of the candidate point
				 */
				inline Correspondence(const unsigned int index, const unsigned int candidateIndex, const Scalar candidateSqrDistance);

				/**
				 * Returns the index of the interest point.
				 * @return Interest point index
				 */
				inline unsigned int index() const;

				/**
				 * Returns the index of the candidate point.
				 * @return Candidate point index.
				 */
				inline unsigned int candidateIndex() const;

				/**
				 * Returns the square distance of the correspondence point.
				 * @return Correspondence point square distance
				 */
				inline Scalar candidateSqrDistance() const;

				/**
				 * Returns whether this object holds a valid candidate.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// Index of the interest point.
				unsigned int correspondenceIndex;

				/// Candidate index.
				unsigned int correspondenceCandidateIndex;

				/// Candidate square distance.
				Scalar correspondenceCandidateSqrDistance;
		};

		/**
		 * Definition of a vector holding correspondence objects.
		 */
		typedef std::vector<Correspondence> Correspondences;

		/**
		 * This class extends the correspondence object to allow at most two correspondence candidates.
		 */
		class OCEAN_TRACKING_EXPORT RedundantCorrespondence : public Correspondence
		{
			public:

				/**
				 * Creates an empty redundant correspondence object.
				 */
				inline RedundantCorrespondence();

				/**
				 * Creates a redundant correspondence object with two correspondence candidates.
				 * @param index Index of the interest point the candidate belongs to
				 * @param firstCandidateIndex Index of the first candidate point
				 * @param firstCandidateSqrDistance Square distance of the first candidate point
				 * @param secondCandidateIndex Index of the second candidate point
				 * @param secondCandidateSqrDistance Square distance of the second candidate point
				 */
				inline RedundantCorrespondence(const unsigned int index, const unsigned int firstCandidateIndex, const Scalar firstCandidateSqrDistance, const unsigned int secondCandidateIndex, const Scalar secondCandidateSqrDistance);

				/**
				 * Returns the index of the second candidate point.
				 * @return Second candidate index
				 */
				inline unsigned int secondCandidateIndex();

				/**
				 * Returns the square distance of the second candidate point.
				 * @return Second candidate square distance
				 */
				inline Scalar secondCandidateSqrDistance();

				/**
				 * Returns whether this redundant correspondence object holds at least one valid correspondence candidate and whether the square distance uniqueness between the first and second candidate is above a given threshold.
				 * The uniqueness is determined by a simple (squared) distance factor.<br>
				 * A (squared) distance factor of e.g., sqr(2) means that the distance between a target point and the second nearest candidate must be twice as large as the distance between the target point and the nearest candidate so that the nearest counts as unique.
				 * @param uniquenessSqrFactor The squared factor to distinguish between a unique and non-unique correspondence, with range (0, infinity)
				 * @return True, if the product between first correspondence square distance and the uniqueness factor is higher than the second correspondence square distance
				 */
				inline bool isUnique(const Scalar uniquenessSqrFactor) const;

				/**
				 * Returns whether this redundant correspondence objects holds a unique and also accurate correspondence candidate.
				 * First the candidate must be unique as determined by isUnique() and further the square distance must be below a given threshold.<br>
				 * The uniqueness is determined by a simple (squared) distance factor.<br>
				 * A (squared) distance factor of e.g., sqr(2) means that the distance between a target point and the second nearest candidate must be twice as large as the distance between the target point and the nearest candidate so that the nearest counts as unique.
				 * @param uniquenessSqrFactor Factor to distinguish between a unique and non-unique correspondence, with range (0, infinity)
				 * @param maxSqrDistance Maximal square distance allowed, with range [0, infinity)
				 * @return True, if so
				 */
				inline bool isUniqueAndAccurate(const Scalar uniquenessSqrFactor, const Scalar maxSqrDistance) const;

			protected:

				/// Second candidate index.
				unsigned int correspondenceSecondCandidateIndex;

				/// Second candidate square distance.
				Scalar correspondenceSecondCandidateSqrDistance;
		};

		/**
		 * Definition of a vector holding redundant correspondence sets.
		 */
		typedef std::vector<RedundantCorrespondence> RedundantCorrespondences;

	public:

		/**
		 * Determines valid correspondences for a set of given object and corresponding image points combined with an extrinsic and intrinsic camera matrix.
		 * @param extrinsic Extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPoints 3D Object points corresponding to the given pose
		 * @param imagePoints 2D Image points corresponding to the image points
		 * @param correspondences Number of point correspondences to be used
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param sqrPixelError Maximal allowed squared pixel error for a correspondence to count as valid
		 * @param validCorrespondences Optional resulting valid correspondence indices
		 * @return Number of valid correspondences
		 * @see determineValidCorrespondencesIF().
		 */
		static inline unsigned int determineValidCorrespondences(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const Geometry::ImagePoint* imagePoints, const size_t correspondences, const bool distortImagePoints, const Scalar sqrPixelError = Scalar(1.5 * 1.5), Indices32* validCorrespondences = nullptr);

		/**
		 * Determines valid correspondences for a set of given object and corresponding image points combined with an extrinsic and intrinsic camera matrix.
		 * @param invertedFlippedExtrinsic Inverted and flipped extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPoints 3D Object points corresponding to the given pose
		 * @param imagePoints 2D Image points corresponding to the image points
		 * @param correspondences Number of point correspondences to be used
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param sqrPixelError Maximal allowed squared pixel error for a correspondence to count as valid
		 * @param validCorrespondences Optional resulting valid correspondence indices
		 * @return Number of valid correspondences
		 * @see determineValidCorrespondences().
		 */
		static unsigned int determineValidCorrespondencesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const Geometry::ImagePoint* imagePoints, const size_t correspondences, const bool distortImagePoints, const Scalar sqrPixelError = Scalar(1.5 * 1.5), Indices32* validCorrespondences = nullptr);

		/**
		 * Determines valid correspondences in a set of given object and corresponding image points.
		 * Invalid correspondences will be removed from the given point set. Thus, this function detects outliers.<br>
		 * @param extrinsic Extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPoints 3D Object points corresponding to the given pose, each 3D point matches to a 2D point with the same index
		 * @param imagePoints 2D Image points corresponding to the image points, each 2D point matches to a 3D point with the same index
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param sqrPixelError Maximal allowed squared pixel error for a correspondence to count as valid
		 */
		static inline void removeInvalidCorrespondences(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints, const bool distortImagePoints, const Scalar sqrPixelError = Scalar(1.5 * 1.5));

		/**
		 * Determines valid correspondences in a set of given object and corresponding image points.
		 * Invalid correspondences will be removed from the given point set. Thus, this function detects outliers.<br>
		 * @param invertedFlippedExtrinsic Inverted and flipped extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPoints 3D Object points corresponding to the given pose, each 3D point matches to a 2D point with the same index
		 * @param imagePoints 2D Image points corresponding to the image points, each 2D point matches to a 3D point with the same index
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param sqrPixelError Maximal allowed squared pixel error for a correspondence to count as valid
		 */
		static void removeInvalidCorrespondencesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints, const bool distortImagePoints, const Scalar sqrPixelError = Scalar(1.5 * 1.5));

		/**
		 * Determines the nearest candidates for all given image points from an extra set of candidate image points.<br>
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points to found the nearest correspondences from
		 * @param numberCandidatePoints Number of given candidate points
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar searchWindowRadius, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from an extra set of candidate image points.<br>
		 * This function first distributes all candidate points into an array to speed up the search process.<br>
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points to found the nearest correspondences from
		 * @param numberCandidatePoints Number of given candidate points
		 * @param width The width of the image area in pixel
		 * @param height The height of the image area in pixel
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar searchWindowRadius, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from a set of candidate projected object points (or simply a second set of image points).<br>
		 * All object points will be projected into the image plane to find the neighbors for the image points.<br>
		 * @param extrinsic Extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points to found the nearest correspondences from
		 * @param numberCandidatePoints Number of given candidate points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static inline RedundantCorrespondences determineNearestCandidates(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ObjectPoint* candidatePoints, const size_t numberCandidatePoints, const bool distortImagePoints, const Scalar searchWindowRadius, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from a set of candidate object points.<br>
		 * All object points will be projected into the image plane to find the neighbors for the image points.<br>
		 * @param invertedFlippedExtrinsic Inverted and flipped extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points to found the nearest correspondences from
		 * @param numberCandidatePoints Number of given candidate points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidatesIF(const HomogenousMatrix4& invertedFlippedExtrinsic, const PinholeCamera& pinholeCamera, const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ObjectPoint* candidatePoints, const size_t numberCandidatePoints, const bool distortImagePoints, const Scalar searchWindowRadius, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from a set of candidate image points.<br>
		 * The spatial distribution of the candidate points must be provided explicitly.<br>
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points to found the nearest correspondences from
		 * @param numberCandidatePoints Number of given candidate points
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param distributionCandidatePoints Distribution of the candidate points (may contain more points as defined by the number of candidate points)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const size_t numberCandidatePoints, const Scalar searchWindowRadius, const Geometry::SpatialDistribution::DistributionArray& distributionCandidatePoints, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from a set of candidate image points.<br>
		 * This function first distributes all candidate points into an array to speed up the search process.<br>
		 * Further, for each candidate point a given 2D line is provided that predicts the position of a corresponding image points (they must lie on these lines).<br>
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points from those the nearest correspondences has to be found
		 * @param candidateLines Lines for each candidate point shrinking the search space to an almost 1D problem
		 * @param numberCandidatePoints Number of given candidate points
		 * @param width The width of the image area in pixel
		 * @param height The height of the image area in pixel
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param maximalLineSqrDistance Maximal square distance between point and candidate line
		 * @param candidateUseCounter Optional used counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const Line2* candidateLines, const size_t numberCandidatePoints, const unsigned int width, const unsigned int height, const Scalar searchWindowRadius, const Scalar maximalLineSqrDistance, Indices32* candidateUseCounter = nullptr);

		/**
		 * Determines the nearest candidates for all given image points from a set of candidate image points.<br>
		 * The spatial distribution of the candidate points must be provided explicitly.<br>
		 * Further, for each candidate point a given 2D line is provided that predicts the position of a corresponding image points (they must lie on these lines).<br>
		 * @param imagePoints Image points to find the candidates for
		 * @param numberImagePoints Number of image points
		 * @param candidatePoints Candidate points from those the nearest correspondences has to be found
		 * @param candidateLines Lines for each candidate point shrinking the search space to an almost 1D problem
		 * @param numberCandidatePoints Number of given candidate points
		 * @param searchWindowRadius Size of the search window (as 'radius') to accept a candidate, with range (0, infinity)
		 * @param maximalLineSqrDistance Maximal square distance between point and candidate line
		 * @param distributionCandidatePoints Distribution of the candidate points (may contain more points as defined by the number of candidate points)
		 * @param candidateUseCounter Optional used-counter of the candidate points
		 * @return Resulting redundant correspondences
		 */
		static RedundantCorrespondences determineNearestCandidates(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ImagePoint* candidatePoints, const Line2* candidateLines, const size_t numberCandidatePoints, const Scalar searchWindowRadius, const Scalar maximalLineSqrDistance, const Geometry::SpatialDistribution::DistributionArray& distributionCandidatePoints, Indices32* candidateUseCounter = nullptr);

		/**
		 * Finds the valid correspondences of a set of given 2D point correspondences according to the median distance of the entire set.
		 * @param firstPoints Points of the first set
		 * @param secondPoints Points of the second set, each point corresponds to one in the first set (with same index)
		 * @param numberPoints Number of provided points in each set
		 * @param thresholdFactor Factor that is applied to the median distance to filter valid correspondences
		 * @return Indices of all valid correspondences
		 */
		static Indices32 filterValidCorrespondences(const Vector2* firstPoints, const Vector2* secondPoints, const size_t numberPoints, const Scalar thresholdFactor);

		/**
		 * Finds the valid correspondences of a set of given 2D point correspondences according to the median distance of a subset of the entire set.
		 * This function needs a set of indices determining the subset of the given points that are investigated.
		 * @param firstPoints Points of the first set
		 * @param secondPoints Points of the second set, each point corresponds to one in the first set (with same index)
		 * @param subsetIndices Subset of the given two point sets that are investigated for correspondence determination
		 * @param thresholdFactor Factor that is applied to the median distance to filter valid correspondences
		 * @return Indices of all valid correspondences
		 */
		static Indices32 filterValidCorrespondences(const Vectors2& firstPoints, const Vectors2& secondPoints, const Indices32& subsetIndices, const Scalar thresholdFactor);
};

inline PointCorrespondences::Correspondence::Correspondence() :
	correspondenceIndex((unsigned int)(-1)),
	correspondenceCandidateIndex((unsigned int)(-1)),
	correspondenceCandidateSqrDistance(Numeric::maxValue())
{
	// nothing to do here
}

inline PointCorrespondences::Correspondence::Correspondence(const unsigned int index, const unsigned int candidateIndex, const Scalar sqrDistance) :
	correspondenceIndex(index),
	correspondenceCandidateIndex(candidateIndex),
	correspondenceCandidateSqrDistance(sqrDistance)
{
	// nothing to do here
}

inline unsigned int PointCorrespondences::Correspondence::index() const
{
	return correspondenceIndex;
}

inline unsigned int PointCorrespondences::Correspondence::candidateIndex() const
{
	return correspondenceCandidateIndex;
}

inline Scalar PointCorrespondences::Correspondence::candidateSqrDistance() const
{
	return correspondenceCandidateSqrDistance;
}

inline PointCorrespondences::Correspondence::operator bool() const
{
	return correspondenceCandidateIndex != (unsigned int)(-1);
}

inline PointCorrespondences::RedundantCorrespondence::RedundantCorrespondence() :
	Correspondence(),
	correspondenceSecondCandidateIndex((unsigned int)(-1)),
	correspondenceSecondCandidateSqrDistance(Numeric::maxValue())
{
	// nothing to do here
}

inline PointCorrespondences::RedundantCorrespondence::RedundantCorrespondence(const unsigned int index, const unsigned int firstCandidateIndex, const Scalar firstCandidateSqrDistance, const unsigned int secondCandidateIndex, const Scalar secondCandidateSqrDistance) :
	Correspondence(index, firstCandidateIndex, firstCandidateSqrDistance),
	correspondenceSecondCandidateIndex(secondCandidateIndex),
	correspondenceSecondCandidateSqrDistance(secondCandidateSqrDistance)
{
	// nothing to do here
}

inline unsigned int PointCorrespondences::RedundantCorrespondence::secondCandidateIndex()
{
	return correspondenceSecondCandidateIndex;
}

inline Scalar PointCorrespondences::RedundantCorrespondence::secondCandidateSqrDistance()
{
	return correspondenceSecondCandidateSqrDistance;
}

inline bool PointCorrespondences::RedundantCorrespondence::isUnique(const Scalar uniquenessSqrFactor) const
{
	return correspondenceCandidateIndex != (unsigned int)(-1) && correspondenceSecondCandidateSqrDistance >= uniquenessSqrFactor * correspondenceCandidateSqrDistance;
}

inline bool PointCorrespondences::RedundantCorrespondence::isUniqueAndAccurate(const Scalar uniquenessSqrFactor, const Scalar maxSqrDistance) const
{
	return correspondenceCandidateSqrDistance <= maxSqrDistance && isUnique(uniquenessSqrFactor);
}

inline unsigned int PointCorrespondences::determineValidCorrespondences(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const Geometry::ImagePoint* imagePoints, const size_t correspondences, const bool distortImagePoints, const Scalar sqrPixelError, Indices32* validCorrespondences)
{
	return determineValidCorrespondencesIF(PinholeCamera::standard2InvertedFlipped(extrinsic), pinholeCamera, objectPoints, imagePoints, correspondences, distortImagePoints, sqrPixelError, validCorrespondences);
}

inline void PointCorrespondences::removeInvalidCorrespondences(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints, const bool distortImagePoints, const Scalar sqrPixelError)
{
	removeInvalidCorrespondencesIF(PinholeCamera::standard2InvertedFlipped(extrinsic), pinholeCamera, objectPoints, imagePoints, distortImagePoints, sqrPixelError);
}

inline PointCorrespondences::RedundantCorrespondences PointCorrespondences::determineNearestCandidates(const HomogenousMatrix4& extrinsic, const PinholeCamera& pinholeCamera, const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const Geometry::ObjectPoint* candidatePoints, const size_t numberCandidatePoints, const bool distortImagePoints, const Scalar searchWindowRadius, Indices32* candidateUseIndices)
{
	return determineNearestCandidatesIF(PinholeCamera::standard2InvertedFlipped(extrinsic), pinholeCamera, imagePoints, numberImagePoints, candidatePoints, numberCandidatePoints, distortImagePoints, searchWindowRadius, candidateUseIndices);
}

}

}

#endif // META_OCEAN_TRACKING_POINT_CORRESPONDENCES_H
