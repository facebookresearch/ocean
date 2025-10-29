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

Scalar StereoBullseyeDetector::Parameters::maxDistanceToEpipolarLine() const
{
	return maxDistanceToEpipolarLine_;
}

bool StereoBullseyeDetector::Parameters::setMaxDistanceToEpipolarLine(const Scalar distance)
{
	ocean_assert(distance >= 0);
	if (distance < Scalar(0))
	{
		return false;
	}

	maxDistanceToEpipolarLine_ = distance;
	return true;
}

bool StereoBullseyeDetector::detectBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, const Parameters& parameters, Worker* worker)
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

	// Monocular detection of bullseyes
	// If the cameras have different resolutions, start the monocular detection on the camera with lower resolution.
	BullseyeGroup bullseyeGroup;

	const size_t lowerResolutionCameraIndex = (cameras[0]->width() * cameras[0]->height() <= cameras[1]->width() * cameras[1]->height()) ? 0 : 1;
	for (const size_t cameraIndex : {lowerResolutionCameraIndex, 1 - lowerResolutionCameraIndex})
	{
		if (!MonoBullseyeDetector::detectBullseyes(*cameras[cameraIndex], yFrames[cameraIndex], bullseyeGroup[cameraIndex], parameters, worker))
		{
			return false;
		}

		if (bullseyeGroup[cameraIndex].empty())
		{
			// Nothing found in this camera, so no need to continue.
			return true;
		}
	}

	const HomogenousMatrix4 camera0_T_camera1 = device_T_cameras[0].inverted() * device_T_cameras[1];
	const EpipolarGeometry epipolarGeometry(cameras[0], cameras[1], camera0_T_camera1);

	ocean_assert(epipolarGeometry.isValid());
	if (!epipolarGeometry.isValid())
	{
		return false;
	}

	BullseyePairs candidates;
	if (!matchBullseyes(cameras, yFrames, world_T_device, device_T_cameras, epipolarGeometry, bullseyeGroup, parameters.maxDistanceToEpipolarLine(), candidates))
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

	if (!triangulateBullseyes(cameras, world_T_device, device_T_cameras, candidates, bullseyePairs, bullseyeCenters, reprojectionErrorsA, reprojectionErrorsB))
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

	const Scalar maxSqrDistance = maxDistanceToEpipolarLine * maxDistanceToEpipolarLine;

	// The camera resolutions can be different. To compare similarity
	// of bullseyes using something like their radii, their size has to be
	// normalized to the same scale.
	const Scalar cameraB_s_cameraA = Scalar(cameras[1]->width()) / Scalar(cameras[0]->width());

	// Special case: only one bullseye in each camera. Check if they are close enough to each other.
	if (bullseyeGroup[0].size() == 1 && bullseyeGroup[1].size() == 1)
	{
		bullseyePairs = {{bullseyeGroup[0][0], bullseyeGroup[1][0]}};

		return true;
	}

	Matrix costMatrix;
	if (!computeBullseyeMatchingCostMatrix(bullseyeGroup[0], bullseyeGroup[1], epipolarGeometry, maxSqrDistance, cameraB_s_cameraA, costMatrix))
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

Scalar StereoBullseyeDetector::computeBullseyeMatchingCost(const Bullseye& bullseyeA, const Bullseye& bullseyeB, const EpipolarGeometry& epipolarGeometry, const Scalar maxSqrDistance, const Scalar cameraB_s_cameraA)
{
	ocean_assert(bullseyeA.isValid() && bullseyeB.isValid());
	ocean_assert(epipolarGeometry.isValid());
	ocean_assert(maxSqrDistance >= 0);
	ocean_assert(cameraB_s_cameraA > 0);

	// Distance to epipolar lines
	const Scalar sqrDistanceAtoB = epipolarGeometry.squareDistanceToEpipolarLine(EpipolarGeometry::CI_CAMERA0, bullseyeA.position(), bullseyeB.position());
	const Scalar sqrDistanceBtoA = epipolarGeometry.squareDistanceToEpipolarLine(EpipolarGeometry::CI_CAMERA1, bullseyeB.position(), bullseyeA.position());
	ocean_assert(sqrDistanceAtoB >= 0 && sqrDistanceBtoA >= 0);

	const Scalar epipolarCostLinear = std::max(sqrDistanceAtoB, sqrDistanceBtoA);

	const Scalar epipolarOffset = maxSqrDistance;
	const Scalar epipolarCost = Scalar(1) / (Scalar(1) + Numeric::exp(epipolarOffset - epipolarCostLinear));
	ocean_assert(epipolarCost >= 0 && epipolarCost <= 1);

	// Radius
	const Scalar radiusAScaled = cameraB_s_cameraA * bullseyeA.radius();
	const Scalar radiusB = bullseyeB.radius();

	const Scalar radiusCostLinear = std::abs(radiusAScaled - radiusB);

	const Scalar radiusOffset = Scalar(0.25) * std::min(radiusAScaled, radiusB);
	const Scalar radiusCost = Scalar(1) / (Scalar(1) + Numeric::exp(radiusOffset - radiusCostLinear));
	ocean_assert(radiusCost >= 0 && radiusCost <= 1);

	const Scalar totalCost = Scalar(0.5) * epipolarCost + Scalar(0.5) * radiusCost;
	ocean_assert(totalCost >= 0 && totalCost <= 1);

	return totalCost;
}

bool StereoBullseyeDetector::computeBullseyeMatchingCostMatrix(const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const EpipolarGeometry& epipolarGeometry, const Scalar maxSqrDistance, const Scalar cameraB_s_cameraA, Matrix& costMatrix)
{
	ocean_assert(!bullseyesA.empty() && !bullseyesB.empty());
	ocean_assert(epipolarGeometry.isValid());
	ocean_assert(maxSqrDistance >= 0);

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

			costMatrix(a, b) = computeBullseyeMatchingCost(bullseyeA, bullseyeB, epipolarGeometry, maxSqrDistance, cameraB_s_cameraA);
		}
	}

	return true;
}

bool StereoBullseyeDetector::triangulateBullseye(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Bullseye& bullseyeA, const Bullseye& bullseyeB, Vector3& bullseyeCenter, Scalar& reprojectionErrorA, Scalar& reprojectionErrorB)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraA.isValid());
	ocean_assert(bullseyeA.isValid() && bullseyeB.isValid());

	ocean_assert(cameraA.isInside(bullseyeA.position()));
	ocean_assert(cameraB.isInside(bullseyeB.position()));

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

			bullseyeCenter = objectPoint;
			reprojectionErrorA = projectionErrorA;
			reprojectionErrorB = projectionErrorB;

			return true;
		}
	}

	return false;
}

bool StereoBullseyeDetector::triangulateBullseyes(const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const BullseyePairs& candidates, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, Scalars& reprojectionErrorsA, Scalars& reprojectionErrorsB)
{
	ocean_assert(cameras.size() == 2);
	ocean_assert(cameras[0] && cameras[0]->isValid() && cameras[1] && cameras[1]->isValid());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameras.size() == 2);
	ocean_assert(device_T_cameras[0].isValid() && device_T_cameras[1].isValid());

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

	const AnyCamera& cameraA = *cameras[0];
	const AnyCamera& cameraB = *cameras[1];

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameras[0];
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameras[1];

	for (const BullseyePair& candidate : candidates)
	{
		const Bullseye& bullseyeA = candidate.first;
		const Bullseye& bullseyeB = candidate.second;

		Vector3 bullseyeCenter;
		Scalar reprojectionErrorA;
		Scalar reprojectionErrorB;
		if (triangulateBullseye(cameraA, cameraB, world_T_cameraA, world_T_cameraB, bullseyeA, bullseyeB, bullseyeCenter, reprojectionErrorA, reprojectionErrorB))
		{
			bullseyePairs.emplace_back(bullseyeA, bullseyeB);
			bullseyeCenters.emplace_back(bullseyeCenter);
			reprojectionErrorsA.emplace_back(reprojectionErrorA);
			reprojectionErrorsB.emplace_back(reprojectionErrorB);
		}
	}

	ocean_assert(bullseyePairs.size() == bullseyeCenters.size());
	ocean_assert(bullseyePairs.size() == reprojectionErrorsA.size());
	ocean_assert(bullseyePairs.size() == reprojectionErrorsB.size());

	return true;
#endif
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
