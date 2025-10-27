/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/StereoBullseyeDetector.h"

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

#if 0
	// TODO Implement this function properly.
	return false;
#else
	// Naive implementation for now.
	const Bullseyes& bullseyesA = bullseyeGroup[0];
	const Bullseyes& bullseyesB = bullseyeGroup[1];

	if (bullseyesA.size() != 1 || bullseyesB.size() != 1)
	{
		return false;
	}

	bullseyePairs = {{bullseyesA[0], bullseyesB[0]}};

	return true;
#endif
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
