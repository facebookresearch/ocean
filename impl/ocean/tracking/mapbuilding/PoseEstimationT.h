/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_T_H
#define META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_T_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"

#include "ocean/base/Worker.h"

#include "ocean/geometry/Octree.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements functions to estimate camera poses using template-based data types.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT PoseEstimationT
{
	public:

		/**
		 * Definition of an unordered map mapping object point ids to descriptors.
		 */
		template <typename TDescriptor>
		using UnorderedDescriptorMap = std::unordered_map<Index32, TDescriptor>;

	public:

		/**
		 * Determines the 6-DOF pose for 2D/3D correspondences applying a brute-force search.
		 * The function can be used to verify the performance/correctness of pose estimation functions avoiding a brute-force search.
		 * @param camera The camera profile to be used, must be valid
		 * @param objectPoints The 3D object points of all possible 3D features, must be valid
		 * @param objectPointDescriptors The descriptors for all 3D object points, one descriptor for each 3D object point (however, TObjectPointDescriptor allows to e.g., define a array/vector of several descriptors per object point), must be valid
		 * @param numberObjectPoints The number of given 3D object points (and object point descriptors), with range [4, infinity)
		 * @param imagePoints The 2D image points of all possible 2D features, must be valid
		 * @param imagePointDescriptors The descriptors for all 2D image points, one for each image points, must be valid
		 * @param numberImagePoints The number of 2D image points, with range [4, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting camera pose transforming camera to world, with default camera looking into the negative z-space an y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error between a projected 3D object point and a 2D image point so that both points count as corresponding, in pixel, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param usedObjectPointIndices Optional resulting indices of the 3D object points which have been used to determine the camera pose, nullptr if not of interest
		 * @param usedImagePointIndices Optional resulting indices of the 2D image points which have been used to determine the camera pose, nullptr if not of interest
		 * @param world_T_roughCamera Optional known rough camera pose allowing to skip the unguided matching, invalid if unknown
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static bool determinePoseBruteForce(const AnyCamera& camera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), Indices32* usedObjectPointIndices = nullptr, Indices32* usedImagePointIndices = nullptr, const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false), Worker* worker = nullptr);

		/**
		 * Determines the 6-DOF pose for 2D/3D correspondences applying a brute-force search.
		 * The function can be used to verify the performance/correctness of pose estimation functions avoiding a brute-force search.
		 * This function uses descriptors for 3D object points which can be stored in an arbitrary order (even multiple individual descriptors can exist for one 3D object point).
		 * The relationship between object point descriptors and their corresponding 3D object point locations is defined via 'objectPointIndices'.
		 * @param camera The camera profile to be used, must be valid
		 * @param objectPoints The 3D object points of all possible 3D features, must be valid
		 * @param objectPointDescriptors The descriptors for all 3D object points, some descriptors may describe the same 3D object points (e.g., because of different viewing locations), must be valid
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param numberObjectDescriptors The number of given descriptors for the 3D object points, with range [4, infinity)
		 * @param imagePoints The 2D image points of all possible 2D features, must be valid
		 * @param imagePointDescriptors The descriptors for all 2D image points, one for each image points, must be valid
		 * @param numberImagePoints The number of 2D image points, with range [4, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting camera pose transforming camera to world, with default camera looking into the negative z-space an y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error between a projected 3D object point and a 2D image point so that both points count as corresponding, in pixel, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param usedObjectPointIndices Optional resulting indices of the 3D object points which have been used to determine the camera pose, nullptr if not of interest
		 * @param usedImagePointIndices Optional resulting indices of the 2D image points which have been used to determine the camera pose, nullptr if not of interest
		 * @param world_T_roughCamera Optional known rough camera pose allowing to skip the unguided matching, invalid if unknown
		 * @return True, if succeeded
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static bool determinePoseBruteForceWithArbitraryDescriptorOrder(const AnyCamera& camera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const Index32* objectPointIndices, const size_t numberObjectDescriptors, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), Indices32* usedObjectPointIndices = nullptr, Indices32* usedImagePointIndices = nullptr, const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false));

		/**
		 * Determines the unguided matching (without apriori information) between image point and object point features applying a brute-force search.
		 * @param objectPointDescriptors The object point descriptors to be used, must be valid
		 * @param numberObjectPointDescriptors The number of given object point descriptors, with range [1, infinity)
		 * @param imagePointDescriptors The image point descriptors to be used, must be valid
		 * @param numberImagePointDescriptors The number of given image point descriptors, with range [1, infinity)
		 * @param maximalDescriptorDistance The maximal distance between two corresponding descriptors, with range [0, infinity)
		 * @param objectPointDescriptorIndices The resulting indices of object point descriptors matching to the individual image point descriptors, one object point descriptor index of each given image point descriptor, -1 for image point descriptors for which no matching object point descriptor could be found
		 * @param worker Optional worker to distribute the computation
		 * @param distances Optional resulting distances between the individual matched descriptors, one for each image point descriptor, undefined if no matching exists, nullptr if not of interest
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineUnguidedBruteForceMatchings(const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePointDescriptors, TDescriptorDistance maximalDescriptorDistance, Index32* objectPointDescriptorIndices, Worker* worker, TDescriptorDistance* distances = nullptr);

		/**
		 * Determines the guided matching (without known rough camera pose) between image point and object point features applying a brute-force search.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_roughCamera The known rough camera pose transforming camera to world, must be valid
		 * @param objectPoints The 3D object points to be used, must be valid
		 * @param objectPointDescriptors The object point descriptors, one for each 3D object point, must be valid
		 * @param numberObjectPoints The number of given object points (and object point descriptors), with range [1, infinity)
		 * @param imagePoints The 2D image points to be used, must be valid
		 * @param imagePointDescriptors The image point descriptors, one for each 2D image point, must be valid
		 * @param numberImagePoints The number of given image points (and image point descriptors), with range [1, infinity)
		 * @param maximalDescriptorDistance The maximal distance between two corresponding descriptors, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error of valid 2D/3D feature correspondences, in pixel, with range [0, infinity)
		 * @param objectPointDescriptorIndices The resulting indices of object point descriptors matching to the individual image point descriptors, one object point descriptor index of each given image point descriptor, -1 for image point descriptors for which no matching object point descriptor could be found
		 * @param worker Optional worker to distribute the computation
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineGuidedBruteForceMatchings(const AnyCamera& camera, const HomogenousMatrix4& world_T_roughCamera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, Index32* objectPointDescriptorIndices, Worker* worker);

		/**
		 * Determines the guided matching between 2D/3D correspondences.
		 * @param camera The camera profile to be used, must be valid
		 * @param world_T_camera The known (rough) camera pose transforming camera to world, with default camera looking into the negative z-space an y-axis upwards, must be valid
		 * @param imagePoints The 2D image points of all possible 2D features, must be valid
		 * @param imagePointDescriptors The descriptors for all 2D image points, one for each image points, must be valid
		 * @param numberImagePoints The number of 2D image points, with range [4, infinity)
		 * @param objectPoints The 3D object points of all possible 3D features, must be valid
		 * @param objectPointOctree The octree with all 3D object points
		 * @param objectPointIds The ids of all 3D object points, one for each 3D object point, must be valid
		 * @param objectPointDescriptorMap The map mapping 3D object point ids to their descriptors
		 * @param matchedImagePoints The resulting 2D image points which have been determined during the matching
		 * @param matchedObjectPoints The resulting 3D object points which have been determined during the matching
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param matchedImagePointIndices Optional resulting indices of the image points which have been matched, one for each resulting 2D image points, nullptr if not of interest
		 * @param matchedObjectPointIds Optional resulting object point ids of the object points which have been matched, one for each resulting 3D image points, nullptr if not of interest
		 * @param worker Optional worker to distribute the computation
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineGuidedMatchings(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<TObjectPointDescriptor>& objectPointDescriptorMap, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const TDescriptorDistance maximalDescriptorDistance, Indices32* matchedImagePointIndices = nullptr, Indices32* matchedObjectPointIds = nullptr, Worker* worker = nullptr);

		/**
		 * Returns the individual descriptor distances for a given set of corresponding feature descriptor pairs.
		 * This function can be used to e.g., analyze the distribution of descriptor distances and e.g., to determine an ideal threshold for valid descriptor correspondences.
		 * @param objectPointDescriptors The descriptors of the object points, must be valid
		 * @param imagePointDescriptors The descriptors of the image points, must be valid
		 * @param objectPointDescriptorIndices The indices of the object point descriptors for which a corresponding image point descriptor exists, must be valid
		 * @param imagePointDescriptorIndices The indices of the image point descriptors for which a corresponding object point descriptor exists, one for each object point descriptor, with the same order as the object point descriptors, must be valid
		 * @param numberCorrespondences The number of specified descriptor correspondences, with range [1, infinity)
		 * @return The descriptor distances, one of each corresponding descriptor pair, in the same order as the provided correspondences
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static std::vector<TDescriptorDistance> determineDescriptorDistances(const TObjectPointDescriptor* objectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, const Index32* objectPointDescriptorIndices, const Index32* imagePointDescriptorIndices, const size_t numberCorrespondences);

	protected:

		/**
		 * Determines the unguided matching between a subset of image point and object point features applying a brute-force search.
		 * @param objectPointDescriptors The object point descriptors to be used, must be valid
		 * @param numberObjectPointDescriptors The number of given object point descriptors, with range [1, infinity)
		 * @param imagePointDescriptors The image point descriptors to be used, must be valid
		 * @param maximalDescriptorDistance The maximal distance between two corresponding descriptors, with range [0, infinity)
		 * @param objectPointDescriptorIndices The resulting indices of object point descriptors matching to the individual image point descriptors, one object point descriptor index of each given image point descriptor, -1 for image point descriptors for which no matching object point descriptor could be found
		 * @param distances Optional resulting distances between the individual matched descriptors, one for each image point descriptor, nullptr if not of interest
		 * @param firstImagePointDescriptor The first image point descriptor to be handled, with range [0, numberImagePointDescriptors - 1]
		 * @param numberImagePointDescriptors The number of image point descriptors to be handled, with range [1, numberImagePointDescriptors - firstImagePointDescriptor]
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineUnguidedBruteForceMatchingsSubset(const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, TDescriptorDistance maximalDescriptorDistance, Index32* objectPointDescriptorIndices, TDescriptorDistance* distances, const unsigned int firstImagePointDescriptor, const unsigned int numberImagePointDescriptors);

		/**
		 * Determines the guided matching (without known rough camera pose) between a subset of image point and object point features applying a brute-force search.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_roughCamera The known rough camera pose transforming camera to world, must be valid
		 * @param objectPoints The 3D object points to be used, must be valid
		 * @param objectPointDescriptors The object point descriptors, one for each 3D object point, must be valid
		 * @param numberObjectPoints The number of given object points (and object point descriptors), with range [1, infinity)
		 * @param imagePoints The 2D image points to be used, must be valid
		 * @param imagePointDescriptors The image point descriptors, one for each 2D image point, must be valid
		 * @param maximalDescriptorDistance The maximal distance between two corresponding descriptors, with range [0, infinity)
		 * @param maximalProjectionError The maximal projection error of valid 2D/3D feature correspondences, in pixel, with range [0, infinity)
		 * @param objectPointDescriptorIndices The resulting indices of object point descriptors matching to the individual image point descriptors, one object point descriptor index of each given image point descriptor, -1 for image point descriptors for which no matching object point descriptor could be found
		 * @param firstImagePoint The first image point to be handled, with range [0, numberImagePoints - 1]
		 * @param numberImagePoints The number of image points to be handled, with range [1, numberImagePoints - firstImagePoint]
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineGuidedBruteForceMatchingsSubset(const AnyCamera* camera, const HomogenousMatrix4* world_T_roughCamera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, Index32* objectPointDescriptorIndices, const unsigned int firstImagePoint, const unsigned int numberImagePoints);

		/**
		 * Determines the guided matching between 2D/3D correspondences for a subset of the given 2D image points.
		 * @param camera The camera profile to be used, must be valid
		 * @param world_T_camera The known (rough) camera pose transforming camera to world, with default camera looking into the negative z-space an y-axis upwards, must be valid
		 * @param imagePoints The 2D image points of all possible 2D features, must be valid
		 * @param imagePointDescriptors The descriptors for all 2D image points, one for each image points, must be valid
		 * @param objectPoints The 3D object points of all possible 3D features, must be valid
		 * @param objectPointOctree The octree with all 3D object points
		 * @param objectPointIds The ids of all 3D object points, one for each 3D object point, must be valid
		 * @param objectPointDescriptorMap The map mapping 3D object point ids to their descriptors
		 * @param matchedImagePoints The resulting 2D image points which have been determined during the matching
		 * @param matchedObjectPoints The resulting 3D object points which have been determined during the matching
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param matchedImagePointIndices The resulting indices of the image points which have been matched, one for each resulting 2D image points, nullptr if not of interest
		 * @param matchedObjectPointIds The resulting object point ids of the object points which have been matched, one for each resulting 3D image points, nullptr if not of interest
		 * @param lock The lock object in case this function is executed across multiple thread, nullptr if executed in one thread
		 * @param firstImagePoint The index of the first image point to be handled, with range [0, numberImagePoints - 1]
		 * @param numberImagePoints The number of image points to be handled, with range [1, numberImagePoints - firstImagePoint]
		 * @tparam TImagePointDescriptor The data type of the descriptor for the 2D image points
		 * @tparam TObjectPointDescriptor The data type of the descriptor for the 3D object points
		 * @tparam TDescriptorDistance The data type of the distance between image point and object point descriptors, e.g., 'uint32_t' or 'float'
		 * @tparam tImageObjectDistanceFunction The function pointer to a function allowing to determine the descriptor distance between an image point feature descriptor and an object point feature descriptor, must be valid
		 */
		template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
		static void determineGuidedMatchingsSubset(const AnyCamera* camera, const HomogenousMatrix4* world_T_camera, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const Vector3* objectPoints, const Geometry::Octree* objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<TObjectPointDescriptor>* objectPointDescriptorMap, Vectors2* matchedImagePoints, Vectors3* matchedObjectPoints, Indices32* matchedImagePointIndices, Indices32* matchedObjectPointIds, const TDescriptorDistance maximalDescriptorDistance, Lock* lock, const unsigned int firstImagePoint, const unsigned int numberImagePoints);
};

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
bool PoseEstimationT::determinePoseBruteForce(const AnyCamera& camera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor
* imagePointDescriptors, const size_t numberImagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIndices, Indices32* usedImagePointIndices, const HomogenousMatrix4& world_T_roughCamera, Worker* worker)
{
	ocean_assert(camera.isValid());

	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	if (numberImagePoints < size_t(minimalNumberCorrespondences))
	{
		return false;
	}

	const Scalar maximalSqrProjectionError = maximalProjectionError * maximalProjectionError;

	Indices32 matchedObjectPointIds(numberImagePoints);

	Vectors2 matchedImagePoints;
	Vectors3 matchedObjectPoints;

	matchedImagePoints.reserve(numberImagePoints);
	matchedObjectPoints.reserve(numberImagePoints);

	if (!world_T_roughCamera.isValid())
	{
		determineUnguidedBruteForceMatchings<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>(objectPointDescriptors, numberObjectPoints, imagePointDescriptors, numberImagePoints, maximalDescriptorDistance, matchedObjectPointIds.data(), worker);

		for (size_t imagePointIndex = 0; imagePointIndex < numberImagePoints; ++imagePointIndex)
		{
			const Index32& matchedObjectPointIndex = matchedObjectPointIds[imagePointIndex];

			if (matchedObjectPointIndex != Index32(-1))
			{
				matchedImagePoints.emplace_back(imagePoints[imagePointIndex]);
				matchedObjectPoints.emplace_back(objectPoints[matchedObjectPointIndex]);
			}
		}

		if (matchedImagePoints.size() < size_t(minimalNumberCorrespondences))
		{
			world_T_camera.toNull();
			return false;
		}
	}

	Indices32 internalUsedObjectPointIndices;
	Indices32 internalUsedImagePointIndices;

	const bool useInternalIndices = usedObjectPointIndices != nullptr || usedImagePointIndices != nullptr;

	if (useInternalIndices)
	{
		internalUsedObjectPointIndices.reserve(numberImagePoints);
		internalUsedImagePointIndices.reserve(numberImagePoints);
	}

	const Scalar faultyRate = Scalar(1) - inlierRate;
	ocean_assert(faultyRate >= Scalar(0) && faultyRate < Scalar(1));

	const unsigned int unguidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), faultyRate);

	Indices32 validIndices;
	if (world_T_roughCamera.isValid() || Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, 20u, true, unguidedIterations, maximalSqrProjectionError, &validIndices))
	{
		Log::info() << "finished RANSAC";

		// now applying guided matching

		if (world_T_roughCamera.isValid())
		{
			world_T_camera = world_T_roughCamera;
		}

		matchedImagePoints.clear();
		matchedObjectPoints.clear();

		determineGuidedBruteForceMatchings<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>(camera, world_T_camera, objectPoints, objectPointDescriptors, numberObjectPoints, imagePoints, imagePointDescriptors, numberImagePoints, maximalDescriptorDistance, maximalProjectionError, matchedObjectPointIds.data(), worker);

		for (size_t imagePointIndex = 0; imagePointIndex < numberImagePoints; ++imagePointIndex)
		{
			const Index32& matchedObjectPointIndex = matchedObjectPointIds[imagePointIndex];

			if (matchedObjectPointIndex != Index32(-1))
			{
				matchedImagePoints.emplace_back(imagePoints[imagePointIndex]);
				matchedObjectPoints.emplace_back(objectPoints[matchedObjectPointIndex]);

				if (useInternalIndices)
				{
					internalUsedImagePointIndices.emplace_back(Index32(imagePointIndex));
					internalUsedObjectPointIndices.emplace_back(matchedObjectPointIndex);
				}
			}
		}

		if (matchedImagePoints.size() < size_t(minimalNumberCorrespondences))
		{
			world_T_camera.toNull();
			return false;
		}

		constexpr unsigned int guidedIterations = 40u;

		world_T_camera.toNull();
		validIndices.clear();
		if (Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, 20u, true, guidedIterations, maximalSqrProjectionError, &validIndices))
		{
			if (usedObjectPointIndices != nullptr)
			{
				ocean_assert(useInternalIndices);

				usedObjectPointIndices->clear();
				usedObjectPointIndices->reserve(validIndices.size());

				for (const Index32 validIndex : validIndices)
				{
					usedObjectPointIndices->emplace_back(internalUsedObjectPointIndices[validIndex]);
				}
			}

			if (usedImagePointIndices != nullptr)
			{
				ocean_assert(useInternalIndices);

				usedImagePointIndices->clear();
				usedImagePointIndices->reserve(validIndices.size());

				for (const Index32 validIndex : validIndices)
				{
					usedImagePointIndices->emplace_back(internalUsedImagePointIndices[validIndex]);
				}
			}

			return true;
		}
	}

	world_T_camera.toNull();

	return false;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
bool PoseEstimationT::determinePoseBruteForceWithArbitraryDescriptorOrder(const AnyCamera& camera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const Index32* objectPointIndices, const size_t numberObjectDescriptors, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIndices, Indices32* usedImagePointIndices, const HomogenousMatrix4& world_T_roughCamera)
{
	ocean_assert(camera.isValid());

	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	if (numberImagePoints < size_t(minimalNumberCorrespondences))
	{
		return false;
	}

	const Scalar maximalSqrProjectionError = maximalProjectionError * maximalProjectionError;

	Vectors2 matchedImagePoints;
	Vectors3 matchedObjectPoints;

	matchedImagePoints.reserve(numberImagePoints);
	matchedObjectPoints.reserve(numberImagePoints);

	if (!world_T_roughCamera.isValid())
	{
		// unguided brute force matching

		for (size_t imagePointIndex = 0; imagePointIndex < numberImagePoints; ++imagePointIndex)
		{
			const TImagePointDescriptor& imagePointDescriptor = imagePointDescriptors[imagePointIndex];

			TDescriptorDistance bestDistance = NumericT<TDescriptorDistance>::maxValue();
			Index32 bestObjectPointDescriptorIndex = Index32(-1);

			for (size_t nObjectPoint = 0; nObjectPoint < numberObjectDescriptors; ++nObjectPoint)
			{
				const TDescriptorDistance distance = tImageObjectDistanceFunction(imagePointDescriptor, objectPointDescriptors[nObjectPoint]);

				if (distance < bestDistance)
				{
					bestDistance = distance;
					bestObjectPointDescriptorIndex = Index32(nObjectPoint);
				}
			}

			if (bestDistance <= maximalDescriptorDistance)
			{
				const Index32 objectPointIndex = objectPointIndices[bestObjectPointDescriptorIndex];

				matchedImagePoints.emplace_back(imagePoints[imagePointIndex]);
				matchedObjectPoints.emplace_back(objectPoints[objectPointIndex]);
			}
		}

		if (matchedImagePoints.size() < size_t(minimalNumberCorrespondences))
		{
			world_T_camera.toNull();
			return false;
		}
	}

	Indices32 internalUsedObjectPointIndices;
	Indices32 internalUsedImagePointIndices;

	const bool useInternalIndices = usedObjectPointIndices != nullptr || usedImagePointIndices != nullptr;

	if (useInternalIndices)
	{
		internalUsedObjectPointIndices.reserve(numberImagePoints);
		internalUsedImagePointIndices.reserve(numberImagePoints);
	}

	const Scalar faultyRate = Scalar(1) - inlierRate;
	ocean_assert(faultyRate >= Scalar(0) && faultyRate < Scalar(1));

	const unsigned int unguidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), faultyRate);

	Indices32 validIndices;
	if (world_T_roughCamera.isValid() || Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, 20u, true, unguidedIterations, maximalSqrProjectionError, &validIndices))
	{
		// now applying guided matching

		if (world_T_roughCamera.isValid())
		{
			world_T_camera = world_T_roughCamera;
		}

		matchedImagePoints.clear();
		matchedObjectPoints.clear();

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		for (size_t imagePointIndex = 0; imagePointIndex < numberImagePoints; ++imagePointIndex)
		{
			const Vector2& imagePoint = imagePoints[imagePointIndex];
			const TImagePointDescriptor& imagePointDescriptor = imagePointDescriptors[imagePointIndex];

			TDescriptorDistance bestDistance = NumericT<TDescriptorDistance>::maxValue();
			Index32 bestObjectPointDescriptorIndex = Index32(-1);

			for (size_t nObjectPoint = 0; nObjectPoint < numberObjectDescriptors; ++nObjectPoint)
			{
				const TDescriptorDistance distance = tImageObjectDistanceFunction(imagePointDescriptor, objectPointDescriptors[nObjectPoint]);

				if (distance < bestDistance)
				{
					const Index32 objectPointIndex = objectPointIndices[nObjectPoint];

					const Vector3& objectPoint = objectPoints[objectPointIndex];

					if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint) && camera.projectToImageIF(flippedCamera_T_world, objectPoint).sqrDistance(imagePoint) <= Scalar(20 * 20))
					{
						bestDistance = distance;
						bestObjectPointDescriptorIndex = Index32(nObjectPoint);
					}
				}
			}

			if (bestDistance <= maximalDescriptorDistance)
			{
				const Index32 objectPointIndex = objectPointIndices[bestObjectPointDescriptorIndex];

				matchedImagePoints.emplace_back(imagePoints[imagePointIndex]);
				matchedObjectPoints.emplace_back(objectPoints[objectPointIndex]);

				if (useInternalIndices)
				{
					internalUsedObjectPointIndices.emplace_back(objectPointIndex);
					internalUsedImagePointIndices.emplace_back(Index32(imagePointIndex));
				}
			}
		}

		if (matchedImagePoints.size() < size_t(minimalNumberCorrespondences))
		{
			world_T_camera.toNull();
			return false;
		}

		constexpr unsigned int guidedIterations = 40u;

		world_T_camera.toNull();
		validIndices.clear();
		if (Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, 20u, true, guidedIterations, maximalSqrProjectionError, &validIndices))
		{
			if (usedObjectPointIndices != nullptr)
			{
				ocean_assert(useInternalIndices);

				usedObjectPointIndices->clear();
				usedObjectPointIndices->reserve(validIndices.size());

				for (const Index32 validIndex : validIndices)
				{
					usedObjectPointIndices->emplace_back(internalUsedObjectPointIndices[validIndex]);
				}
			}

			if (usedImagePointIndices != nullptr)
			{
				ocean_assert(useInternalIndices);

				usedImagePointIndices->clear();
				usedImagePointIndices->reserve(validIndices.size());

				for (const Index32 validIndex : validIndices)
				{
					usedImagePointIndices->emplace_back(internalUsedImagePointIndices[validIndex]);
				}
			}

			return true;
		}
	}

	world_T_camera.toNull();

	return false;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineUnguidedBruteForceMatchings(const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePointDescriptors, const TDescriptorDistance maximalDescriptorDistance, Index32* objectPointDescriptorIndices, Worker* worker, TDescriptorDistance* distances)
{
	ocean_assert(objectPointDescriptors != nullptr);
	ocean_assert(imagePointDescriptors != nullptr);
	ocean_assert(objectPointDescriptorIndices != nullptr);

	if (worker && (numberObjectPointDescriptors >= 1000 || numberImagePointDescriptors >= 1000))
	{
		worker->executeFunction(Worker::Function::createStatic(&determineUnguidedBruteForceMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>, objectPointDescriptors, numberObjectPointDescriptors, imagePointDescriptors, maximalDescriptorDistance, objectPointDescriptorIndices, distances, 0u, 0u), 0u, (unsigned int)(numberImagePointDescriptors));
	}
	else
	{
		determineUnguidedBruteForceMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>(objectPointDescriptors, numberObjectPointDescriptors, imagePointDescriptors, maximalDescriptorDistance, objectPointDescriptorIndices, distances, 0u, (unsigned int)(numberImagePointDescriptors));
	}
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineGuidedBruteForceMatchings(const AnyCamera& camera, const HomogenousMatrix4& world_T_roughCamera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, Index32* objectPointDescriptorIndices, Worker* worker)
{
	ocean_assert(camera.isValid());
	ocean_assert(world_T_roughCamera.isValid());
	ocean_assert(objectPoints != nullptr);
	ocean_assert(objectPointDescriptors != nullptr);
	ocean_assert(imagePoints != nullptr);
	ocean_assert(imagePointDescriptors != nullptr);
	ocean_assert(objectPointDescriptorIndices != nullptr);
	ocean_assert(maximalProjectionError >= 0);

	if (worker && (numberObjectPoints >= 1000 || numberImagePoints >= 1000))
	{
		worker->executeFunction(Worker::Function::createStatic(&determineGuidedBruteForceMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>, &camera, &world_T_roughCamera, objectPoints, objectPointDescriptors, numberObjectPoints, imagePoints, imagePointDescriptors, maximalDescriptorDistance, maximalProjectionError, objectPointDescriptorIndices, 0u, 0u), 0u, (unsigned int)(numberImagePoints));
	}
	else
	{
		determineGuidedBruteForceMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>(&camera, &world_T_roughCamera, objectPoints, objectPointDescriptors, numberObjectPoints, imagePoints, imagePointDescriptors, maximalDescriptorDistance, maximalProjectionError, objectPointDescriptorIndices, 0u, (unsigned int)(numberImagePoints));
	}
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineUnguidedBruteForceMatchingsSubset(const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, const TDescriptorDistance maximalDescriptorDistance, Index32* objectPointDescriptorIndices, TDescriptorDistance* distances, const unsigned int firstImagePointDescriptor, const unsigned int numberImagePointDescriptors)
{
	ocean_assert(objectPointDescriptors != nullptr);
	ocean_assert(imagePointDescriptors != nullptr);

	for (size_t imagePointIndex = size_t(firstImagePointDescriptor); imagePointIndex < size_t(firstImagePointDescriptor + numberImagePointDescriptors); ++imagePointIndex)
	{
		const TImagePointDescriptor& imagePointDescriptor = imagePointDescriptors[imagePointIndex];

		TDescriptorDistance bestDistance = NumericT<TDescriptorDistance>::maxValue();
		Index32 bestObjectPointIndex = Index32(-1);

		for (size_t objectPointIndex = 0; objectPointIndex < numberObjectPointDescriptors; ++objectPointIndex)
		{
			const TDescriptorDistance distance = tImageObjectDistanceFunction(imagePointDescriptor, objectPointDescriptors[objectPointIndex]);

			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestObjectPointIndex = Index32(objectPointIndex);
			}
		}

		if (bestDistance <= maximalDescriptorDistance)
		{
			objectPointDescriptorIndices[imagePointIndex] = bestObjectPointIndex;

			if (distances != nullptr)
			{
				distances[imagePointIndex] = bestDistance;
			}
		}
		else
		{
			objectPointDescriptorIndices[imagePointIndex] = Index32(-1);
		}
	}
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineGuidedBruteForceMatchingsSubset(const AnyCamera* camera, const HomogenousMatrix4* world_T_roughCamera, const Vector3* objectPoints, const TObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, TDescriptorDistance maximalDescriptorDistance, const Scalar maximalProjectionError, Index32* objectPointDescriptorIndices, const unsigned int firstImagePoint, const unsigned int numberImagePoints)
{
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(world_T_roughCamera != nullptr && world_T_roughCamera->isValid());
	ocean_assert(objectPoints != nullptr);
	ocean_assert(objectPointDescriptors != nullptr);
	ocean_assert(imagePoints != nullptr);
	ocean_assert(imagePointDescriptors != nullptr);
	ocean_assert(maximalProjectionError >= 0);

	const Scalar maximalSqrProjectionError = Numeric::sqr(maximalProjectionError);

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(*world_T_roughCamera));

	for (size_t imagePointIndex = size_t(firstImagePoint); imagePointIndex < size_t(firstImagePoint + numberImagePoints); ++imagePointIndex)
	{
		const Vector2& imagePoint = imagePoints[imagePointIndex];
		ocean_assert(camera->isInside(imagePoint));

		const TImagePointDescriptor& imagePointDescriptor = imagePointDescriptors[imagePointIndex];

		TDescriptorDistance bestDistance = NumericT<TDescriptorDistance>::maxValue();
		Index32 bestObjectPointIndex = Index32(-1);

		for (size_t objectPointIndex = 0; objectPointIndex < numberObjectPoints; ++objectPointIndex)
		{
			const TDescriptorDistance distance = tImageObjectDistanceFunction(imagePointDescriptor, objectPointDescriptors[objectPointIndex]);

			if (distance < bestDistance)
			{
				const Vector3& objectPoint = objectPoints[objectPointIndex];

				if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint) && camera->projectToImageIF(flippedCamera_T_world, objectPoint).sqrDistance(imagePoint) <= maximalSqrProjectionError)
				{
					bestDistance = distance;
					bestObjectPointIndex = Index32(objectPointIndex);
				}
			}
		}

		if (bestDistance <= maximalDescriptorDistance)
		{
			objectPointDescriptorIndices[imagePointIndex] = bestObjectPointIndex;
		}
		else
		{
			objectPointDescriptorIndices[imagePointIndex] = Index32(-1);
		}
	}
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineGuidedMatchings(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<TObjectPointDescriptor>& objectPointDescriptorMap, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const TDescriptorDistance maximalDescriptorDistance, Indices32* matchedImagePointIndices, Indices32* matchedObjectPointIds, Worker* worker)
{
	ocean_assert(camera.isValid() && world_T_camera.isValid());
	ocean_assert(imagePoints != nullptr && imagePointDescriptors != nullptr && numberImagePoints >= 1);
	ocean_assert(objectPoints != nullptr && objectPointIds != nullptr);

	ocean_assert(matchedImagePoints.empty());
	ocean_assert(matchedObjectPoints.empty());

	if (matchedObjectPointIds != nullptr)
	{
		matchedObjectPointIds->clear();
	}

	if (matchedImagePointIndices != nullptr)
	{
		matchedImagePointIndices->clear();
	}

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&PoseEstimationT::determineGuidedMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>, &camera, &world_T_camera, imagePoints, imagePointDescriptors, objectPoints, &objectPointOctree, objectPointIds, &objectPointDescriptorMap, &matchedImagePoints, &matchedObjectPoints, matchedImagePointIndices, matchedObjectPointIds, maximalDescriptorDistance, &lock, 0u, 0u), 0u, (unsigned int)(numberImagePoints));
	}
	else
	{
		determineGuidedMatchingsSubset<TImagePointDescriptor, TObjectPointDescriptor, TDescriptorDistance, tImageObjectDistanceFunction>(&camera, &world_T_camera, imagePoints, imagePointDescriptors, objectPoints, &objectPointOctree, objectPointIds, &objectPointDescriptorMap, &matchedImagePoints, &matchedObjectPoints, matchedImagePointIndices, matchedObjectPointIds, maximalDescriptorDistance, nullptr, 0u, (unsigned int)(numberImagePoints));
	}
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
std::vector<TDescriptorDistance> PoseEstimationT::determineDescriptorDistances(const TObjectPointDescriptor* objectPointDescriptors, const TImagePointDescriptor* imagePointDescriptors, const Index32* objectPointDescriptorIndices, const Index32* imagePointDescriptorIndices, const size_t numberCorrespondences)
{
	ocean_assert(objectPointDescriptors != nullptr);
	ocean_assert(imagePointDescriptors != nullptr);
	ocean_assert(objectPointDescriptorIndices != nullptr);
	ocean_assert(imagePointDescriptorIndices != nullptr);
	ocean_assert(numberCorrespondences >= 1);

	std::vector<TDescriptorDistance> result;
	result.reserve(numberCorrespondences);

	for (size_t n = 0; n < numberCorrespondences; ++n)
	{
		const Index32& objectPointDescriptorIndex = objectPointDescriptorIndices[n];
		const Index32& imagePointDescriptorIndex = imagePointDescriptorIndices[n];

		result.emplace_back(tImageObjectDistanceFunction(imagePointDescriptors[imagePointDescriptorIndex], objectPointDescriptors[objectPointDescriptorIndex]));
	}

	return result;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tImageObjectDistanceFunction)(const TImagePointDescriptor&, const TObjectPointDescriptor&)>
void PoseEstimationT::determineGuidedMatchingsSubset(const AnyCamera* camera, const HomogenousMatrix4* world_T_camera, const Vector2* imagePoints, const TImagePointDescriptor* imagePointDescriptors, const Vector3* objectPoints, const Geometry::Octree* objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<TObjectPointDescriptor>* objectPointDescriptorMap, Vectors2* matchedImagePoints, Vectors3* matchedObjectPoints, Indices32* matchedImagePointIndices, Indices32* matchedObjectPointIds, const TDescriptorDistance maximalDescriptorDistance, Lock* lock, const unsigned int firstImagePoint, const unsigned int numberImagePoints)
{
	static_assert(tImageObjectDistanceFunction != nullptr);

	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(world_T_camera && world_T_camera->isValid());

	ocean_assert(imagePoints != nullptr && imagePointDescriptors != nullptr && objectPoints != nullptr);
	ocean_assert(objectPointOctree != nullptr && objectPointIds != nullptr && objectPointDescriptorMap != nullptr);

	ocean_assert(matchedImagePoints != nullptr);
	ocean_assert(matchedObjectPoints != nullptr);

	Vectors2 localMatchedImagePoints;
	Vectors3 localMatchedObjectPoints;
	Indices32 localMatchedImagePointIndices;
	Indices32 localMatchedObjectPointIds;

	localMatchedImagePoints.reserve(numberImagePoints);
	localMatchedObjectPoints.reserve(numberImagePoints);
	localMatchedImagePointIndices.reserve(numberImagePoints);
	localMatchedObjectPointIds.reserve(numberImagePoints);

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(*world_T_camera));

	const Scalar tanHalfAngle = Numeric::tan(Numeric::deg2rad(Scalar(0.2)));

	std::vector<const Indices32*> leafs;
	leafs.reserve(32);

	constexpr Scalar maximalSqrProjectionError = Scalar(20 * 20);

	Geometry::Octree::ReusableData resuableData;

	for (unsigned int nImagePoint = firstImagePoint; nImagePoint < firstImagePoint + numberImagePoints; ++nImagePoint)
	{
		const Vector2& imagePoint = imagePoints[nImagePoint];
		ocean_assert(camera->isInside(imagePoint));

		const TImagePointDescriptor& imagePointDescriptor = imagePointDescriptors[nImagePoint];

		const Line3 ray = camera->ray(imagePoint, *world_T_camera);

		leafs.clear();
		objectPointOctree->intersectingLeafs(ray, tanHalfAngle, leafs, resuableData);

		TDescriptorDistance bestDistance = NumericT<TDescriptorDistance>::maxValue();
		Index32 bestObjectPointIndex = Index32(-1);

		for (const Indices32* leaf : leafs)
		{
			for (const Index32& objectPointIndex : *leaf)
			{
				const Vector3& objectPoint = objectPoints[objectPointIndex];

				if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint) && camera->projectToImageIF(flippedCamera_T_world, objectPoint).sqrDistance(imagePoint) <= maximalSqrProjectionError)
				{
					const Index32 objectPointId = objectPointIds[objectPointIndex];
					const typename UnorderedDescriptorMap<TObjectPointDescriptor>::const_iterator iObjectPointDescriptor = objectPointDescriptorMap->find(objectPointId);
					ocean_assert(iObjectPointDescriptor != objectPointDescriptorMap->cend());

					const TDescriptorDistance distance = tImageObjectDistanceFunction(imagePointDescriptor, iObjectPointDescriptor->second);

					if (distance < bestDistance)
					{
						bestDistance = distance;
						bestObjectPointIndex = objectPointIndex;
					}
				}
			}
		}

		if (bestDistance <= maximalDescriptorDistance)
		{
			ocean_assert(bestObjectPointIndex != Index32(-1));

			localMatchedImagePoints.emplace_back(imagePoint);
			localMatchedObjectPoints.emplace_back(objectPoints[bestObjectPointIndex]);

			localMatchedImagePointIndices.emplace_back(nImagePoint);
			localMatchedObjectPointIds.emplace_back(objectPointIds[bestObjectPointIndex]);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matchedImagePoints->insert(matchedImagePoints->cend(), localMatchedImagePoints.cbegin(), localMatchedImagePoints.cend());
	matchedObjectPoints->insert(matchedObjectPoints->cend(), localMatchedObjectPoints.cbegin(), localMatchedObjectPoints.cend());

	if (matchedImagePointIndices != nullptr)
	{
		matchedImagePointIndices->insert(matchedImagePointIndices->cend(), localMatchedImagePointIndices.cbegin(), localMatchedImagePointIndices.cend());
	}

	if (matchedObjectPointIds != nullptr)
	{
		matchedObjectPointIds->insert(matchedObjectPointIds->cend(), localMatchedObjectPointIds.cbegin(), localMatchedObjectPointIds.cend());
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_T_H
