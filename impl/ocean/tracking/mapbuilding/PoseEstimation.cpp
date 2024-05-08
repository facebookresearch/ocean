/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/PoseEstimation.h"

#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool PoseEstimation::determinePose(const AnyCamera& anyCamera, const UnifiedUnguidedMatching& unifiedUnguidedMatching, const UnifiedGuidedMatching& unifiedGuidedMatching, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIds, Indices32* usedImagePointIndices, const HomogenousMatrix4& world_T_roughCamera, Worker* worker)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(maximalDescriptorDistance.isValid());

	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	if (unifiedUnguidedMatching.numberImagePoints() < size_t(minimalNumberCorrespondences) || unifiedUnguidedMatching.numberImagePoints() != unifiedGuidedMatching.numberImagePoints())
	{
		return false;
	}

	Vectors2 matchedImagePoints;
	Vectors3 matchedObjectPoints;

	matchedImagePoints.reserve(unifiedUnguidedMatching.numberImagePoints());
	matchedObjectPoints.reserve(unifiedUnguidedMatching.numberImagePoints());

	if (!world_T_roughCamera.isValid())
	{
		if (!unifiedUnguidedMatching.determineUnguidedMatchings(minimalNumberCorrespondences, maximalDescriptorDistance, matchedImagePoints, matchedObjectPoints, worker))
		{
			world_T_camera.toNull();
			return false;
		}
	}

	const Scalar maximalSqrProjectionError = maximalProjectionError * maximalProjectionError;

	const Scalar faultyRate = Scalar(1) - inlierRate;
	ocean_assert(faultyRate >= Scalar(0) && faultyRate < Scalar(1));

	const unsigned int unguidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), faultyRate);

	if (world_T_roughCamera.isValid() || Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, 20u, true, unguidedIterations, maximalSqrProjectionError))
	{
		if (world_T_roughCamera.isValid())
		{
			world_T_camera = world_T_roughCamera;
		}

		matchedImagePoints.clear();
		matchedObjectPoints.clear();

		Indices32 localMatchedImagePointIndices;
		Indices32 localMatchedObjectPointIds;
		unifiedGuidedMatching.determineGuidedMatchings(anyCamera, world_T_camera, matchedImagePoints, matchedObjectPoints, maximalDescriptorDistance, &localMatchedImagePointIndices, &localMatchedObjectPointIds, worker);

		ocean_assert(matchedImagePoints.size() == matchedObjectPoints.size());

		world_T_camera.toNull();

		if (matchedImagePoints.size() < size_t(minimalNumberCorrespondences))
		{
			return false;
		}

		const unsigned int guidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), Scalar(0.75)); // 25% inlier due to guided matching

		Indices32 validIndices;
		if (Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, minimalNumberCorrespondences, true, guidedIterations, maximalSqrProjectionError, &validIndices))
		{
			if (usedObjectPointIds != nullptr)
			{
				usedObjectPointIds->clear();
				usedObjectPointIds->reserve(validIndices.size());

				for (const Index32& validIndex : validIndices)
				{
					ocean_assert(validIndex < localMatchedObjectPointIds.size());

					usedObjectPointIds->emplace_back(localMatchedObjectPointIds[validIndex]);
				}
			}

			if (usedImagePointIndices != nullptr)
			{
				usedImagePointIndices->clear();
				usedImagePointIndices->reserve(validIndices.size());

				for (const Index32& validIndex : validIndices)
				{
					ocean_assert(validIndex < localMatchedImagePointIndices.size());

					usedImagePointIndices->emplace_back(localMatchedImagePointIndices[validIndex]);
				}
			}

			return true;
		}
	}

	return false;
}

bool PoseEstimation::determinePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const UnifiedUnguidedMatching& unifiedUnguidedMatchingA, const UnifiedUnguidedMatching& unifiedUnguidedMatchingB, const UnifiedGuidedMatching& unifiedGuidedMatchingA, const UnifiedGuidedMatching& unifiedGuidedMatchingB, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const UnifiedMatching::DistanceValue& maximalDescriptorDistance, const Scalar maximalProjectionError, const Scalar inlierRate, Indices32* usedObjectPointIdsA, Indices32* usedObjectPointIdsB, Indices32* usedImagePointIndicesA, Indices32* usedImagePointIndicesB, const HomogenousMatrix4& world_T_roughDevice, Worker* worker)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(device_T_cameraA.isValid() && device_T_cameraB.isValid());
	ocean_assert(maximalDescriptorDistance.isValid());

	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	Vectors2 matchedImagePointsA;
	Vectors2 matchedImagePointsB;

	Vectors3 matchedObjectPointsA;
	Vectors3 matchedObjectPointsB;

	matchedImagePointsA.reserve(unifiedUnguidedMatchingA.numberImagePoints());
	matchedImagePointsB.reserve(unifiedUnguidedMatchingB.numberImagePoints());

	matchedObjectPointsA.reserve(unifiedUnguidedMatchingA.numberImagePoints());
	matchedObjectPointsB.reserve(unifiedUnguidedMatchingB.numberImagePoints());

	if (!world_T_roughDevice.isValid())
	{
		if (!unifiedUnguidedMatchingA.determineUnguidedMatchings(minimalNumberCorrespondences, maximalDescriptorDistance, matchedImagePointsA, matchedObjectPointsA, worker))
		{
			world_T_device.toNull();
			return false;
		}

		if (!unifiedUnguidedMatchingB.determineUnguidedMatchings(minimalNumberCorrespondences, maximalDescriptorDistance, matchedImagePointsB, matchedObjectPointsB, worker))
		{
			world_T_device.toNull();
			return false;
		}
	}

	const Scalar maximalSqrProjectionError = maximalProjectionError * maximalProjectionError;

	const Scalar faultyRate = Scalar(1) - inlierRate;
	ocean_assert(faultyRate >= Scalar(0) && faultyRate < Scalar(1));

	const unsigned int unguidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), faultyRate);

	HomogenousMatrix4 device_T_world(false);
	if (world_T_roughDevice.isValid() || Geometry::RANSAC::objectTransformationStereo(anyCameraA, anyCameraB, device_T_cameraA, device_T_cameraB, ConstArrayAccessor<Vector3>(matchedObjectPointsA), ConstArrayAccessor<Vector3>(matchedObjectPointsB), ConstArrayAccessor<Vector2>(matchedImagePointsA), ConstArrayAccessor<Vector2>(matchedImagePointsB), randomGenerator, device_T_world, 20u, true, unguidedIterations, maximalSqrProjectionError))
	{
		if (world_T_roughDevice.isValid())
		{
			device_T_world = world_T_roughDevice.inverted();
		}

		world_T_device = device_T_world.inverted();

		matchedImagePointsA.clear();
		matchedImagePointsB.clear();

		matchedObjectPointsA.clear();
		matchedObjectPointsB.clear();

		Indices32 localMatchedImagePointIndicesA;
		Indices32 localMatchedObjectPointIdsA;
		unifiedGuidedMatchingA.determineGuidedMatchings(anyCameraA, world_T_device * device_T_cameraA, matchedImagePointsA, matchedObjectPointsA, maximalDescriptorDistance, &localMatchedImagePointIndicesA, &localMatchedObjectPointIdsA, worker);

		Indices32 localMatchedImagePointIndicesB;
		Indices32 localMatchedObjectPointIdsB;
		unifiedGuidedMatchingB.determineGuidedMatchings(anyCameraB, world_T_device * device_T_cameraB, matchedImagePointsB, matchedObjectPointsB, maximalDescriptorDistance, &localMatchedImagePointIndicesB, &localMatchedObjectPointIdsB, worker);

		ocean_assert(matchedImagePointsA.size() == matchedObjectPointsA.size());
		ocean_assert(matchedImagePointsB.size() == matchedObjectPointsB.size());

		world_T_device.toNull();

		if (matchedImagePointsA.size() + matchedImagePointsB.size() < size_t(minimalNumberCorrespondences))
		{
			return false;
		}

		device_T_world.toNull();

		const unsigned int guidedIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), Scalar(0.75)); // 25% inlier due to guided matching

		Indices32 validIndicesA;
		Indices32 validIndicesB;
		if (Geometry::RANSAC::objectTransformationStereo(anyCameraA, anyCameraB, device_T_cameraA, device_T_cameraB, ConstArrayAccessor<Vector3>(matchedObjectPointsA), ConstArrayAccessor<Vector3>(matchedObjectPointsB), ConstArrayAccessor<Vector2>(matchedImagePointsA), ConstArrayAccessor<Vector2>(matchedImagePointsB), randomGenerator, device_T_world, minimalNumberCorrespondences, true, guidedIterations, maximalSqrProjectionError, &validIndicesA, &validIndicesB))
		{
			world_T_device = device_T_world.inverted();

			if (usedObjectPointIdsA != nullptr)
			{
				usedObjectPointIdsA->clear();
				usedObjectPointIdsA->reserve(validIndicesA.size());

				for (const Index32& validIndexA : validIndicesA)
				{
					ocean_assert(validIndexA < localMatchedObjectPointIdsA.size());

					usedObjectPointIdsA->emplace_back(localMatchedObjectPointIdsA[validIndexA]);
				}
			}

			if (usedObjectPointIdsB != nullptr)
			{
				usedObjectPointIdsB->clear();
				usedObjectPointIdsB->reserve(validIndicesB.size());

				for (const Index32& validIndexB : validIndicesB)
				{
					ocean_assert(validIndexB < localMatchedObjectPointIdsB.size());

					usedObjectPointIdsB->emplace_back(localMatchedObjectPointIdsB[validIndexB]);
				}
			}

			if (usedImagePointIndicesA != nullptr)
			{
				usedImagePointIndicesA->clear();
				usedImagePointIndicesA->reserve(validIndicesA.size());

				for (const Index32& validIndexA : validIndicesA)
				{
					ocean_assert(validIndexA < localMatchedImagePointIndicesA.size());

					usedImagePointIndicesA->emplace_back(localMatchedImagePointIndicesA[validIndexA]);
				}
			}

			if (usedImagePointIndicesB != nullptr)
			{
				usedImagePointIndicesB->clear();
				usedImagePointIndicesB->reserve(validIndicesB.size());

				for (const Index32& validIndexB : validIndicesB)
				{
					ocean_assert(validIndexB < localMatchedImagePointIndicesB.size());

					usedImagePointIndicesB->emplace_back(localMatchedImagePointIndicesB[validIndexB]);
				}
			}

			return true;
		}
	}

	return false;
}

}

}

}
