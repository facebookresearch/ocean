/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_POSE_ESTIMATION_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_POSE_ESTIMATION_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a brute-force pose estimation for unified data types.
 * @ingroup trackingmapbuilding
 */
class UnifiedBruteForcePoseEstimation
{
	public:

		/**
		 * Definition of an unordered map mapping object point ids to 3D object point locations.
		 */
		using ObjectPointMap = std::unordered_map<unsigned int, Vector3>;

	public:

		/**
		 * Creates a new pose estimation object
		 * @param objectPointMap The map containing all 3D object points used during pose estimation
		 * @param unifiedDescriptorMap The unified descriptor map holding the descriptors of the 3D object points
		 */
		UnifiedBruteForcePoseEstimation(const ObjectPointMap& objectPointMap, const UnifiedDescriptorMap& unifiedDescriptorMap);

		/**
		 * Returns all 3D object points which are used for pose estimation.
		 * @return The 3D object points
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the ids of all 3D object points
		 * @return The object point ids, one for each 3D object point, with same order
		 */
		inline const Indices32& objectPointIds() const;

		/**
		 * Determines the camera pose based on several image points and their descriptors.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePointDescriptors The descriptors of the image points which will be used for pose estimation, at least 4
		 * @param imagePoints The 2D image points to be used for pose estimation, one for each image point descriptor, defined in the domain of the camera, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting transformation between camera and world, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of feature correspondences for a valid pose, with range [4, infinity)
		 * @param maximalDescriptorDistance The maximal distance between feature descriptors to count as a valid descriptor match, must be valid
		 * @param maximalProjectionError The maximal projection error between 3D object points and their 2D observations, in pixels, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param usedObjectPointIndices Optional resulting indices of all 3D object points which were used during pose estimation, nullptr if not of interest
		 * @param usedImagePointIndices Optional resulting indices of all 2D image points which were used during pose estimation, nullptr if not of interest
		 * @param world_T_roughCamera Optional rough camera pose to speedup the pose estimation, if known, invalid otherwise
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		bool determinePose(const AnyCamera& camera, const UnifiedDescriptors& imagePointDescriptors, const Vector2* imagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIndices = nullptr, Indices32* usedImagePointIndices = nullptr, const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false), Worker* worker = nullptr) const;

		/**
		 * Determines the brute-force matching between two sets of feature descriptors.
		 * @param descriptorsA The first set of feature descriptors
		 * @param descriptorsB The second set of feature descriptors, must be compatible with the first set of descriptors
		 * @param maximalDescriptorDistance The maximal distance between feature descriptors to count as a valid descriptor match, must be valid
		 * @param indicesA The resulting indices of descriptors from the first set which could be matched to descriptors from the second set
		 * @param indicesB The resulting indices of descriptors from the second set which could be matched to descriptors from the first set
		 * @param distances Optional resulting distances between the individually matched descriptors, nullptr if not of interest
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determineBruteForceMatchings(const UnifiedDescriptors& descriptorsA, const UnifiedDescriptors& descriptorsB, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances = nullptr, Worker* worker = nullptr);

		template <typename TDescriptorA, typename TDescriptorB, typename TDescriptorDistance, TDescriptorDistance(*tDescriptorDistanceFunction)(const TDescriptorA&, const TDescriptorB&)>
		static void determineBruteForceMatchings(const TDescriptorA* descriptorsA, const size_t numberDescriptorsA, const TDescriptorB* descriptorsB, const size_t numberDescriptorsB, const TDescriptorDistance maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances = nullptr, Worker* worker = nullptr);

	protected:

		template <typename TUnifiedDescriptorsA, typename TUnifiedDescriptorsB, typename TDescriptorDistance>
		static bool determineBruteForceMatchings(const UnifiedDescriptors& descriptorsA, const UnifiedDescriptors& descriptorsB, const TDescriptorDistance maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances = nullptr, Worker* worker = nullptr);

		/**
		 * Extracts serialized descriptors from a descriptor map.
		 * @param unifiedDescriptorMap The map from which the descriptors will be extracted
		 * @param objectPointIds The ids of the object points for which descriptors will be extracted
		 * @return The resulting serialized descriptors
		 */
		static SharedUnifiedDescriptors extractObjectPointDescriptors(const UnifiedDescriptorMap& unifiedDescriptorMap, const Indices32& objectPointIds);

		/**
		 * Extracts serialized descriptors from a descriptor map.
		 * @param unifiedDescriptorMap The map from which the descriptors will be extracted
		 * @param objectPointIds The ids of the object points for which descriptors will be extracted
		 * @return The resulting serialized descriptors
		 * @tparam T The data type of the descriptors
		 */
		template <typename T>
		static SharedUnifiedDescriptors extractObjectPointDescriptors(const T& unifiedDescriptorMap, const Indices32& objectPointIds);

	protected:

		/// The 3D object points which will be used for pose estimation.
		Vectors3 objectPoints_;

		/// The ids of all 3D object points, one for each object point.
		Indices32 objectPointIds_;

		/// The descriptors of the 3D object point in sequential order (same order as the 3D object points), one for each object point.
		SharedUnifiedDescriptors objectPointDescriptors_;
};

inline const Vectors3& UnifiedBruteForcePoseEstimation::objectPoints() const
{
	ocean_assert(objectPoints_.size() == objectPointIds_.size());

	return objectPoints_;
}

inline const Indices32& UnifiedBruteForcePoseEstimation::objectPointIds() const
{
	ocean_assert(objectPoints_.size() == objectPointIds_.size());

	return objectPointIds_;
}

template <typename TDescriptorA, typename TDescriptorB, typename TDescriptorDistance, TDescriptorDistance(*tDescriptorDistanceFunction)(const TDescriptorA&, const TDescriptorB&)>
void UnifiedBruteForcePoseEstimation::determineBruteForceMatchings(const TDescriptorA* descriptorsA, const size_t numberDescriptorsA, const TDescriptorB* descriptorsB, const size_t numberDescriptorsB, const TDescriptorDistance maximalDescriptorDistance, Indices32& indicesA, Indices32& indicesB, std::vector<double>* distances, Worker* worker)
{
	ocean_assert(descriptorsA != nullptr);
	ocean_assert(numberDescriptorsA != 0);

	ocean_assert(descriptorsB != nullptr);
	ocean_assert(numberDescriptorsB != 0);

	Indices32 indicesB2A(numberDescriptorsB);
	Indices32 floatDistances(numberDescriptorsB);

	PoseEstimationT::determineUnguidedBruteForceMatchings<TDescriptorA, TDescriptorB, TDescriptorDistance, tDescriptorDistanceFunction>(descriptorsA, numberDescriptorsA, descriptorsB, numberDescriptorsB, maximalDescriptorDistance, indicesB2A.data(), worker);

	for (size_t n = 0; n < indicesB2A.size(); ++n)
	{
		const Index32& indexA = indicesB2A[n];

		if (indexA != Index32(-1))
		{
			indicesA.emplace_back(indexA);
			indicesB.emplace_back(Index32(n));

			if (distances != nullptr)
			{
				distances->emplace_back(double(floatDistances[n]));
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_POSE_ESTIMATION_H
