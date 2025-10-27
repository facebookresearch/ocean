/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/StereoBullseyeDetector.h"

#include "ocean/cv/detector/bullseyes/AssignmentSolver.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

StereoBullseyeDetector::Parameters StereoBullseyeDetector::Parameters::defaultParameters()
{
	return Parameters();
}

bool StereoBullseyeDetector::detectBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, const MonoBullseyeDetector::Parameters& parameters, Worker* worker)
{
	ocean_assert(cameras.size() == 2 && yFrames.size() == 2);
	if (cameras.size() != 2 || yFrames.size() != 2)
	{
		return false;
	}

	ocean_assert(cameras[0] != nullptr && cameras[0]->isValid() && cameras[1] != nullptr && cameras[1]->isValid());
	if (cameras[0] == nullptr || !cameras[0]->isValid() || cameras[1] == nullptr || !cameras[1]->isValid())
	{
		return false;
	}

	ocean_assert(yFrames[0].isValid() && yFrames[1].isValid());
	if (!yFrames[0].isValid() || !yFrames[1].isValid())
	{
		return false;
	}

	ocean_assert(cameras[0]->width() == yFrames[0].width() && cameras[0]->height() == yFrames[0].height());
	ocean_assert(cameras[1]->width() == yFrames[1].width() && cameras[1]->height() == yFrames[1].height());
	if (cameras[0]->width() != yFrames[0].width() || cameras[0]->height() != yFrames[0].height() || cameras[1]->width() != yFrames[1].width() || cameras[1]->height() != yFrames[1].height())
	{
		return false;
	}

	ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[0].pixelFormat(), FrameType::FORMAT_Y8));
	ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[1].pixelFormat(), FrameType::FORMAT_Y8));
	if (!FrameType::arePixelFormatsCompatible(yFrames[0].pixelFormat(), FrameType::FORMAT_Y8) || !FrameType::arePixelFormatsCompatible(yFrames[1].pixelFormat(), FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameras.size() == 2 && device_T_cameras[0].isValid() && device_T_cameras[1].isValid());
	if (!world_T_device.isValid() || device_T_cameras.size() != 2 || !device_T_cameras[0].isValid() || !device_T_cameras[1].isValid())
	{
		return false;
	}

	// Clear the output parameters. Will be filled later.
	bullseyePairs.clear();
	bullseyeCenters.clear();

	// If the cameras have different resolutions, start the monocular detection on the camera with lower resolution.
	const size_t cameraIndex0 = (cameras[0]->width() * cameras[0]->height() <= cameras[1]->width() * cameras[1]->height()) ? 0 : 1;
	const size_t cameraIndex1 = 1 - cameraIndex0;

	BullseyeGroup bullseyeGroup;

	if (!MonoBullseyeDetector::detectBullseyes(*cameras[cameraIndex0], yFrames[cameraIndex0], bullseyeGroup[cameraIndex0], parameters, worker))
	{
		return false;
	}

	if (bullseyeGroup[cameraIndex0].empty())
	{
		// Nothing found in the first camera, so no need to even try the second camera.
		return true;
	}

#if 0
	// TODO Based on the results from the first camera above, try to narrow the search space in the second camera.
#else
	if (!MonoBullseyeDetector::detectBullseyes(*cameras[cameraIndex1], yFrames[cameraIndex1], bullseyeGroup[cameraIndex1], parameters, worker))
	{
		return false;
	}
#endif

	constexpr Scalar maxDistanceToEpipolarLine = Scalar(3.0);

	const HomogenousMatrix4 camera0_T_camera1 = device_T_cameras[0].inverted() * device_T_cameras[1];
	const EpipolarGeometry epipolarGeometry(cameras[0], cameras[1], camera0_T_camera1);

	ocean_assert(epipolarGeometry.isValid());
	if (!epipolarGeometry.isValid())
	{
		return false;
	}

	BullseyePairs candidates;
	if (!matchBullseyes(cameras, yFrames, world_T_device, device_T_cameras, epipolarGeometry, bullseyeGroup, maxDistanceToEpipolarLine, candidates))
	{
		return false;
	}

	if (candidates.empty())
	{
		// No matches found, so nothing to triangulate.
		return true;
	}

	Scalars reprojectionErrorsA;
	Scalars reprojectionErrorsB;

	if (!triangulateBullseyes(cameras, world_T_device, device_T_cameras, epipolarGeometry, candidates, bullseyePairs, bullseyeCenters, reprojectionErrorsA, reprojectionErrorsB))
	{
		return false;
	}

	ocean_assert(bullseyePairs.size() == bullseyeCenters.size());

	return true;
}

bool StereoBullseyeDetector::matchBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const EpipolarGeometry& epipolarGeometry, const BullseyeGroup& bullseyeGroup, const Scalar maxDistanceToEpipolarLine, BullseyePairs& bullseyePairs)
{
	ocean_assert(cameras.size() == 2);
	ocean_assert(cameras[0] && cameras[0]->isValid() && cameras[1] && cameras[1]->isValid());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameras.size() == 2);
	ocean_assert(device_T_cameras[0].isValid() && device_T_cameras[1].isValid());
	ocean_assert(epipolarGeometry.isValid());
	ocean_assert(maxDistanceToEpipolarLine >= 0);

	bullseyePairs.clear();

	if (bullseyeGroup[0].empty() || bullseyeGroup[1].empty())
	{
		// No matches possible.
		return true;
	}

	// Handle different image scales - for future implementation if cameras have different resolutions
	// const Scalar scaleFactorA = Scalar(1.0);
	// const Scalar scaleFactorB = Scalar(1.0);
	// Note: Scale factors would be computed based on different camera resolutions if needed
	// For now, assuming cameras have compatible resolutions

	const Scalar maxSqrDistanceToEpipolarLine = maxDistanceToEpipolarLine * maxDistanceToEpipolarLine;

	// Special case: only one bullseye in each camera. Check if they are close enough to each other.
	if (bullseyeGroup[0].size() == 1 && bullseyeGroup[1].size() == 1)
	{
		if (computeBullseyeMatchingCost(bullseyeGroup[0][0], bullseyeGroup[1][0], epipolarGeometry, maxSqrDistanceToEpipolarLine) != invalidMatchingCost())
		{
			bullseyePairs = {{bullseyeGroup[0][0], bullseyeGroup[1][0]}};
		}

		return true;
	}

	Matrix costMatrix;
	if (!computeBullseyeMatchingCostMatrix(bullseyeGroup[0], bullseyeGroup[1], epipolarGeometry, maxSqrDistanceToEpipolarLine, costMatrix))
	{
		return false;
	}

	AssignmentSolver::Assignments assignments;
	if (!AssignmentSolver::solve(std::move(costMatrix), assignments))
	{
		return false;
	}

	// Convert assignments to bullseye pairs
	const Bullseyes& bullseyesA = bullseyeGroup[0];
	const Bullseyes& bullseyesB = bullseyeGroup[1];

	bullseyePairs.reserve(assignments.size());
	for (const AssignmentSolver::Assignment& assignment : assignments)
	{
		const Index32 indexA = assignment.first;
		const Index32 indexB = assignment.second;

		ocean_assert(indexA < bullseyesA.size());
		ocean_assert(indexB < bullseyesB.size());
		if (indexA >= bullseyesA.size() || indexB >= bullseyesB.size())
		{
			return false;
		}

		bullseyePairs.emplace_back(bullseyesA[indexA], bullseyesB[indexB]);
	}

	return true;
}

Scalar StereoBullseyeDetector::computeBullseyeMatchingCost(const Bullseye& bullseyeA, const Bullseye& bullseyeB, const EpipolarGeometry& epipolarGeometry, Scalar maxSqrDistanceToEpipolarLine)
{
	ocean_assert(bullseyeA.isValid() && bullseyeB.isValid());
	ocean_assert(epipolarGeometry.isValid());
	ocean_assert(maxSqrDistanceToEpipolarLine >= 0);

	const Scalar sqrDistanceAtoB = epipolarGeometry.squareDistanceToEpipolarLine(EpipolarGeometry::CI_CAMERA0, bullseyeA.position(), bullseyeB.position());
	const Scalar sqrDistanceBtoA = epipolarGeometry.squareDistanceToEpipolarLine(EpipolarGeometry::CI_CAMERA1, bullseyeB.position(), bullseyeA.position());
	ocean_assert(sqrDistanceAtoB >= 0 && sqrDistanceBtoA >= 0);

	const Scalar maxSqrEpipolarDistance = std::max(sqrDistanceAtoB, sqrDistanceBtoA);

	if (maxSqrEpipolarDistance > maxSqrDistanceToEpipolarLine)
	{
		return invalidMatchingCost();
	}

	const Scalar epipolarCost = maxSqrEpipolarDistance;

	const Scalar totalCost = epipolarCost;

	return totalCost;
}

bool StereoBullseyeDetector::computeBullseyeMatchingCostMatrix(const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const EpipolarGeometry& epipolarGeometry, Scalar maxSqrDistanceToEpipolarLine, Matrix& costMatrix)
{
	ocean_assert(!bullseyesA.empty() && !bullseyesB.empty());
	ocean_assert(epipolarGeometry.isValid());
	ocean_assert(maxSqrDistanceToEpipolarLine >= 0);

	const size_t numBullseyesA = bullseyesA.size();
	const size_t numBullseyesB = bullseyesB.size();

	costMatrix = Matrix(numBullseyesA, numBullseyesB, invalidMatchingCost());

	for (size_t a = 0; a < numBullseyesA; ++a)
	{
		const Bullseye& bullseyeA = bullseyesA[a];
		ocean_assert(bullseyesA[a].isValid());

		for (size_t b = 0; b < numBullseyesB; ++b)
		{
			const Bullseye& bullseyeB = bullseyesB[b];
			ocean_assert(bullseyesB[b].isValid());

			costMatrix(a, b) = computeBullseyeMatchingCost(bullseyeA, bullseyeB, epipolarGeometry, maxSqrDistanceToEpipolarLine);
		}
	}

	return true;
}

bool StereoBullseyeDetector::triangulateBullseyes(const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const EpipolarGeometry& epipolarGeometry, const BullseyePairs& candidates, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, Scalars& reprojectionErrorsA, Scalars& reprojectionErrorsB)
{
	ocean_assert(cameras.size() == 2);
	ocean_assert(cameras[0] && cameras[0]->isValid() && cameras[1] && cameras[1]->isValid());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameras.size() == 2);
	ocean_assert(device_T_cameras[0].isValid() && device_T_cameras[1].isValid());
	ocean_assert(epipolarGeometry.isValid());

	ocean_assert(!candidates.empty());
	if (candidates.empty())
	{
		return false;
	}

	bullseyePairs.clear();
	bullseyeCenters.clear();
	reprojectionErrorsA.clear();
	reprojectionErrorsB.clear();

#if 0
	// TODO Implement this function properly.
	return false;
#else
	// Naive implementation for now.
	if (candidates.size() != 1)
	{
		return false;
	}

	const AnyCamera& cameraA = *cameras[0];
	const AnyCamera& cameraB = *cameras[1];

	const HomogenousMatrix4& device_T_cameraA = device_T_cameras[0];
	const HomogenousMatrix4& device_T_cameraB = device_T_cameras[1];

	const Bullseye& bullseyeA = candidates[0].first;
	const Bullseye& bullseyeB = candidates[0].second;

	ocean_assert(cameraA.isInside(bullseyeA.position()));
	ocean_assert(cameraB.isInside(bullseyeB.position()));

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

	const Line3 rayA = cameraA.ray(bullseyeA.position(), world_T_cameraA);
	const Line3 rayB = cameraB.ray(bullseyeB.position(), world_T_cameraB);

	Vector3 objectPoint;
	if (rayA.nearestPoint(rayB, objectPoint))
	{
		if (AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), objectPoint)
			&& AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), objectPoint))
		{
			const Vector2 projectedObjectPointA = cameraA.projectToImage(world_T_cameraA, objectPoint);
			const Scalar projectionErrorA = projectedObjectPointA.distance(bullseyeA.position());

			const Vector2 projectedObjectPointB = cameraB.projectToImage(world_T_cameraB, objectPoint);
			const Scalar projectionErrorB = projectedObjectPointB.distance(bullseyeB.position());

			bullseyePairs = {{bullseyeA, bullseyeB}};
			bullseyeCenters = {objectPoint};
			reprojectionErrorsA = {projectionErrorA};
			reprojectionErrorsB = {projectionErrorB};
		}
	}

	return true;
#endif
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
