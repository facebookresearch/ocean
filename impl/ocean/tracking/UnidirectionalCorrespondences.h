/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_UNIDIRECTIONAL_CORRESPONDENCES_H
#define META_OCEAN_TRACKING_UNIDIRECTIONAL_CORRESPONDENCES_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class provides unidirectional feature correspondences.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT UnidirectionalCorrespondences
{
	public:

		/**
		 * Definition of a pair holding the indices of two corresponding features or descriptors.
		 */
		using CorrespondencePair = std::pair<unsigned int, unsigned int>;

		/**
		 * Definition of a vector holding correspondence pairs.
		 */
		using CorrespondencePairs = std::vector<CorrespondencePair>;

	protected:

		/// Definition of a match count value for zero matches.
		static constexpr Index32 matchCountZero_ = Index32(-1);

		/// Definition of a match count value for two+ matches.
		static constexpr Index32 matchCounterTwo_ = Index32(-2);

	public:

		/**
		 * Creates a new unidirectional correspondences object.
		 * @param numberForward The number of forward features, with range [1, infinity)
		 * @param numberBackward The number of backward features, with range [1, infinity)
		 */
		UnidirectionalCorrespondences(const size_t numberForward, const size_t numberBackward);

		/**
		 * Adds a new feature correspondence candidate.
		 * @param forwardIndex Index of the forward feature or descriptor, with range [0, sizeForward-1]
		 * @param backwardIndex Index of the corresponding backward feature or descriptor, with range [0, sizeBackward-1]
		 * @tparam tThreadSafe True, in case the call needs to be thread-safe; False, if no other thread is currently using this correspondence object
		 */
		template <bool tThreadSafe>
		void addCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex);

		/**
		 * Returns all unidirectional feature correspondences.
		 * @return The resulting unique feature correspondences
		 */
		CorrespondencePairs correspondences() const;

		/**
		 * Determines unidirectional correspondences pairs between two sets of descriptors.
		 * @param forwardDescriptors The first (forward) set of descriptors, can be invalid if 'numberForwardDescriptors == 0'
		 * @param numberForwardDescriptors The number of descriptors in the first set, with range [0, infinity)
		 * @param backwardDescriptors The second (backward) set of descriptors, can be invalid if 'numberBackwardDescriptors == 0'
		 * @param numberBackwardDescriptors The number of descriptors in the second set, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two descriptors to count as match, with range [0, infinity)
		 * @parma worker Optional worker to distribute the computation
		 * @return The resulting pairs with unidirectional correspondences
		 * @tparam TDescriptor The data type of the descriptors
		 * @tparam TDistance The data type of the distance between two descriptors
		 * @tparam tDistanceFunction The function returning the distance between two descriptors
		 */
		template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
		static CorrespondencePairs determineCorrespondingDescriptors(const TDescriptor* forwardDescriptors, const size_t numberForwardDescriptors, const TDescriptor* backwardDescriptors, const size_t numberBackwardDescriptors, const TDistance maximalDistance, Worker* worker);

		/**
		 * Determines guided unidirectional correspondence pairs between image features and object features.
		 * The resulting matches are guided based on a given rough 6-DOF camera pose.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_camera The rough 6-DOF pose of the camera, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param objectPoints The 3D locations of the 3D object features, one for each object feature, must be valid
		 * @param objectPointDescriptors The descriptors of the 3D object features, one for each object feature, must be valid
		 * @param numberObjectFeatures The number of 3D object features, with range [1, infinity)
		 * @param imagePoints The 2D location of the 2D image features, one for each image feature, must be valid
		 * @param imagePointDescriptors The descriptors of the 2D image features, one for each image feature, must be valid
		 * @param numberImageFeatures The number of 2D image features, with range [1, infinity)
		 * @param maximalDistance The maximal distance between two descriptors (an object descriptor and an image descriptor) to count as match, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error between image features and projected object features to count as candidate (allows to balance the number of candidates based on the precision of the given camera pose), in pixels, with range [0, infinity)
		 * @tparam TDescriptor The data type of the descriptors
		 * @tparam TDistance The data type of the distance between two descriptors
		 * @tparam tDistanceFunction The function returning the distance between two descriptors
		 */
		template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
		static CorrespondencePairs determineCorrespondingFeatures(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3* objectPoints, const TDescriptor* objectPointDescriptors, const size_t numberObjectFeatures, const Vector2* imagePoints, const TDescriptor* imagePointDescriptors, const size_t numberImageFeatures, const TDistance maximalDistance, const Scalar maximalProjectionError);

		/**
		 * Extracts corresponding elements based on correspondence pairs.
		 * @param correspondencePairs The correspondence pairs providing the individual indices of the elements to be extracted
		 * @param firstElements The entire set of first elements, the first index of each correspondence pair belongs to these elements, must be valid
		 * @param sizeFirstElements The size of the entire set of first elements, with range [1, infinity)
		 * @param secondElements The entire set of second elements, the second index of each correspondence pair belongs to these elements, must be valid
		 * @param sizeSecondElements The size of the entire set of second elements, with range [1, infinity)
		 * @param correspondenceFirstElements The resulting elements from the first set which were defined in the provided correspondence pairs
		 * @param correspondenceSecondElements The resulting elements from the second set which were define i the provided correspondence pairs, one for each elements in correspondenceFirstElements
		 * @tparam TFirst The data type of the first elements
		 * @tparam TSecond The data type of the second elements
		 */
		template <typename TFirst, typename TSecond>
		static void extractCorrespondenceElements(const CorrespondencePairs& correspondencePairs, const TFirst* firstElements, const size_t sizeFirstElements, const TSecond* secondElements, const size_t sizeSecondElements, std::vector<TFirst>& correspondenceFirstElements, std::vector<TSecond>& correspondenceSecondElements);

	protected:

		/**
		 * Determines a subset of all unidirectional correspondences pairs between two sets of descriptors.
		 * @param forwardDescriptors The first (forward) set of descriptors, must be valid
		 * @param numberForwardDescriptors The number of descriptors in the first set, with range [1, infinity)
		 * @param backwardDescriptors The second (backward) set of descriptors, must be valid
		 * @param numberBackwardDescriptors The number of descriptors in the second set, with range [1, infinity)
		 * @param maximalDistance The maximal distance between two descriptors to count as match, with range [0, infinity)
		 * @param forwardIndicesForBackwardDescriptors The individual indices of matching forward descriptors, one for each backward descriptor
		 * @param locks The 'tLocks' locks elements, must be valid if 'tLocks >= 1'
		 * @param subsetFirstForwardDescriptor The first forward descriptor to be handled, with range [0, numberForwardDescriptors-1]
		 * @param subsetNumberForwardDescriptors The number of forward descriptors to be handled, with range [1, numberForwardDescriptors-subsetFirstForwardDescriptor]
		 * @return The resulting pairs with unidirectional correspondences
		 * @tparam TDescriptor The data type of the descriptors
		 * @tparam TDistance The data type of the distance between two descriptors
		 * @tparam tDistanceFunction The function returning the distance between two descriptors
		 * @tparam tLocks The number of provided lock objects, with range [0, infinity)
		 */
		template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&), unsigned int tLocks>
		static void determineCorrespondingDescriptorsSubset(const TDescriptor* forwardDescriptors, const size_t numberForwardDescriptors, const TDescriptor* backwardDescriptors, const size_t numberBackwardDescriptors, const TDistance maximalDistance, Index32* forwardIndicesForBackwardDescriptors, Lock* locks, const unsigned int subsetFirstForwardDescriptor, const unsigned int subsetNumberForwardDescriptors);

	protected:

		/// Correspondence counter for forward features (or for the first features).
		Indices32 forwardCounters_;

		/// Correspondence counter for backward features (or for the second features).
		Indices32 backwardCounters_;

		/// Correspondence candidates.
		CorrespondencePairs candidates_;

		/// The object's lock.
		mutable Lock lock_;
};

template <bool tThreadSafe>
void UnidirectionalCorrespondences::addCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	candidates_.emplace_back(forwardIndex, backwardIndex);

	ocean_assert(forwardIndex < forwardCounters_.size());
	++forwardCounters_[forwardIndex];

	ocean_assert(backwardIndex < backwardCounters_.size());
	++backwardCounters_[backwardIndex];
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineCorrespondingDescriptors(const TDescriptor* forwardDescriptors, const size_t numberForwardDescriptors, const TDescriptor* backwardDescriptors, const size_t numberBackwardDescriptors, const TDistance maximalDistance, Worker* worker)
{
	if (numberForwardDescriptors == 0 || numberBackwardDescriptors == 0)
	{
		return CorrespondencePairs();
	}

	ocean_assert(forwardDescriptors != nullptr && backwardDescriptors != nullptr);

	Indices32 forwardIndicesForBackwardDescriptors(numberBackwardDescriptors, matchCountZero_); // in the beginning no backward descriptor has a matching forward descriptor

	if (worker != nullptr)
	{
		Lock locks[8];

		worker->executeFunction(Worker::Function::createStatic(&UnidirectionalCorrespondences::determineCorrespondingDescriptorsSubset<TDescriptor, TDistance, tDistanceFunction, 8u>, forwardDescriptors, numberForwardDescriptors, backwardDescriptors, numberBackwardDescriptors, maximalDistance, forwardIndicesForBackwardDescriptors.data(), locks, 0u, 0u), 0u, (unsigned int)(numberForwardDescriptors));
	}
	else
	{
		determineCorrespondingDescriptorsSubset<TDescriptor, TDistance, tDistanceFunction, 0u>(forwardDescriptors, numberForwardDescriptors, backwardDescriptors, numberBackwardDescriptors, maximalDistance, forwardIndicesForBackwardDescriptors.data(), nullptr, 0u, (unsigned int)(numberForwardDescriptors));
	}

	CorrespondencePairs result;
	result.reserve(min(numberForwardDescriptors, numberBackwardDescriptors));

	for (size_t indexBackward = 0; indexBackward < numberBackwardDescriptors; ++indexBackward)
	{
		if (forwardIndicesForBackwardDescriptors[indexBackward] < Index32(numberForwardDescriptors))
		{
			// we only accept a pair of forward/backward descriptors if both descriptors have been assigned exactly for one match

			result.emplace_back(forwardIndicesForBackwardDescriptors[indexBackward], Index32(indexBackward));
		}
	}

	return result;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::determineCorrespondingFeatures(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3* objectPoints, const TDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TDescriptor* imagePointDescriptors, const size_t numberImagePoints, const TDistance maximalDistance, const Scalar maximalProjectionError)
{
	ocean_assert(camera.isValid() && world_T_camera.isValid());
	ocean_assert(maximalProjectionError >= 0.0);

	ocean_assert(numberObjectPoints != 0 && numberImagePoints != 0);

	if (numberObjectPoints == 0 || numberImagePoints == 0 || maximalProjectionError < 0.0 || !camera.isValid() || !world_T_camera.isValid())
	{
		return CorrespondencePairs();
	}

	ocean_assert(objectPoints != nullptr && objectPointDescriptors != nullptr);
	ocean_assert(imagePoints != nullptr && imagePointDescriptors != nullptr);

	UnidirectionalCorrespondences candidates(numberImagePoints, numberObjectPoints);

	// first, we project the 3D feature points of the feature map into the camera image and distribute all (visible) features into a 2D grid

	const unsigned int horizontalBins = std::max(1u, (unsigned int)(Scalar(camera.width()) / maximalProjectionError + Scalar(0.5)));
	const unsigned int verticalBins = std::max(1u, (unsigned int)(Scalar(camera.height()) / maximalProjectionError + Scalar(0.5)));

	Geometry::SpatialDistribution::DistributionArray projectedObjectPointsDistributionArray(Scalar(0), Scalar(0), Scalar(camera.width()), Scalar(camera.height()), horizontalBins, verticalBins);

	const HomogenousMatrix4 cameraFlipped_T_world = AnyCamera::standard2InvertedFlipped(world_T_camera);

	Vectors2 projectedObjectPoints(numberObjectPoints);
	camera.projectToImageIF(cameraFlipped_T_world, objectPoints, numberObjectPoints, projectedObjectPoints.data());

	for (size_t n = 0; n < projectedObjectPoints.size(); ++n)
	{
		const Vector2& projectedObjectPoint = projectedObjectPoints[n];

		const int binX = projectedObjectPointsDistributionArray.horizontalBin(projectedObjectPoint.x());
		const int binY = projectedObjectPointsDistributionArray.horizontalBin(projectedObjectPoint.y());

		if ((unsigned int)(binX) < horizontalBins && (unsigned int)(binY) < verticalBins)
		{
			ocean_assert(camera.isInside(projectedObjectPoint));

			projectedObjectPointsDistributionArray(binX, binY).emplace_back(Index32(n));
		}
	}

	// now, we apply a guided matching based on the 9-neighborhood within the 2D grid for each live feature point

	const Scalar sqrMaximalProjectionError = Numeric::sqr(maximalProjectionError);

	for (size_t indexImagePoint = 0; indexImagePoint < numberImagePoints; ++indexImagePoint)
	{
		const Vector2& imagePoint = imagePoints[indexImagePoint];
		const TDescriptor& imagePointDescriptor = imagePointDescriptors[indexImagePoint];

		const int binX = projectedObjectPointsDistributionArray.horizontalBin(imagePoint.x());
		const int binY = projectedObjectPointsDistributionArray.horizontalBin(imagePoint.y());
		ocean_assert((unsigned int)(binX) < horizontalBins && (unsigned int)(binY) < verticalBins);

		TDistance bestDistance = NumericT<TDistance>::maxValue();
		Index32 bestObjectPointIndex = Index32(-1);

		// we iterate over the 9-neighborhood in the distribution array

		for (unsigned int bY = (unsigned int)(std::max(0, binY - 1)); bY < std::min((unsigned int)(binY) + 2u, verticalBins); ++bY)
		{
			for (unsigned int bX = (unsigned int)std::max(0, binX - 1); bX < std::min((unsigned int)(binX) + 2u, horizontalBins); ++bX)
			{
				const Indices32& objectPointIndices = projectedObjectPointsDistributionArray(bX, bY);

				for (const Index32& objectPointIndex : objectPointIndices)
				{
					ocean_assert(size_t(objectPointIndex) < numberObjectPoints);
					ocean_assert(size_t(objectPointIndex) < projectedObjectPoints.size());

					if (projectedObjectPoints[objectPointIndex].sqrDistance(imagePoint) <= sqrMaximalProjectionError)
					{
						const TDistance distance = tDistanceFunction(objectPointDescriptors[objectPointIndex], imagePointDescriptor);

						if (distance < bestDistance)
						{
							bestDistance = distance;
							bestObjectPointIndex = objectPointIndex;
						}
					}
				}
			}
		}

		if (bestObjectPointIndex != Index32(-1) && bestDistance <= maximalDistance)
		{
			candidates.addCandidate<false>(Index32(indexImagePoint), bestObjectPointIndex);
		}
	}

	return candidates.correspondences();
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&), unsigned int tLocks>
void UnidirectionalCorrespondences::determineCorrespondingDescriptorsSubset(const TDescriptor* forwardDescriptors, const size_t numberForwardDescriptors, const TDescriptor* backwardDescriptors, const size_t numberBackwardDescriptors, const TDistance maximalDistance, Index32* forwardIndicesForBackwardDescriptors, Lock* locks, const unsigned int subsetFirstForwardDescriptor, const unsigned int subsetNumberForwardDescriptors)
{
	ocean_assert(forwardDescriptors != nullptr && backwardDescriptors != nullptr && forwardIndicesForBackwardDescriptors != nullptr);

	for (unsigned int indexForward = subsetFirstForwardDescriptor; indexForward < subsetFirstForwardDescriptor + subsetNumberForwardDescriptors; ++indexForward)
	{
		ocean_assert_and_suppress_unused(indexForward < numberForwardDescriptors, numberForwardDescriptors);

		TDistance bestDistance = NumericT<TDistance>::maxValue();
		Index32 bestBackwardIndex = Index32(-1);

		const TDescriptor& forwardDescriptor = forwardDescriptors[indexForward];

		for (unsigned int indexBackward = 0u; indexBackward < Index32(numberBackwardDescriptors); ++indexBackward)
		{
			const TDistance distance = tDistanceFunction(forwardDescriptor, backwardDescriptors[indexBackward]);

			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestBackwardIndex = indexBackward;
			}
		}

		if (bestDistance >= maximalDistance)
		{
			continue;
		}

		// if we found a valid correspondence
		if (bestBackwardIndex != Index32(-1))
		{
			Index32& forwardIndexForBackwardDescriptor = forwardIndicesForBackwardDescriptors[bestBackwardIndex];

			if constexpr (tLocks != 0u)
			{
				ocean_assert(locks != nullptr);
				const ScopedLock scopedLock(locks[bestBackwardIndex % tLocks]);

				switch (forwardIndexForBackwardDescriptor)
				{
					// we have more than two matching feature point
					case matchCounterTwo_:
						break;

					// we have one feature point
					case matchCountZero_:
						forwardIndexForBackwardDescriptor = indexForward;
						break;

					// we have more than one matching feature point
					default:
						forwardIndexForBackwardDescriptor = matchCounterTwo_;
						break;
				}
			}
			else
			{
				switch (forwardIndexForBackwardDescriptor)
				{
					// currently, no matching descriptor defined
					case matchCountZero_:
						forwardIndexForBackwardDescriptor = indexForward;
						break;

					// this descriptor already had more than two matches
					case matchCounterTwo_:
						break;

					// currently, one matching descriptor is already defined, so we invalidate the match
					default:
						forwardIndexForBackwardDescriptor = matchCounterTwo_;
						break;
				}
			}
		}
	}
}

template <typename TFirst, typename TSecond>
void UnidirectionalCorrespondences::extractCorrespondenceElements(const CorrespondencePairs& correspondencePairs, const TFirst* firstElements, const size_t sizeFirstElements, const TSecond* secondElements, const size_t sizeSecondElements, std::vector<TFirst>& correspondenceFirstElements, std::vector<TSecond>& correspondenceSecondElements)
{
	correspondenceFirstElements.clear();
	correspondenceSecondElements.clear();

	correspondenceFirstElements.reserve(correspondencePairs.size());
	correspondenceSecondElements.reserve(correspondencePairs.size());

	for (const CorrespondencePair& correspondencePair : correspondencePairs)
	{
		ocean_assert_and_suppress_unused(correspondencePair.first < sizeFirstElements, sizeFirstElements);
		ocean_assert_and_suppress_unused(correspondencePair.second < sizeSecondElements, sizeSecondElements);

		correspondenceFirstElements.emplace_back(firstElements[correspondencePair.first]);
		correspondenceSecondElements.emplace_back(secondElements[correspondencePair.second]);
	}
}


}

}

#endif // META_OCEAN_TRACKING_UNIDIRECTIONAL_CORRESPONDENCES_H
