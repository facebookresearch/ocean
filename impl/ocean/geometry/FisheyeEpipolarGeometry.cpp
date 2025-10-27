/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/FisheyeEpipolarGeometry.h"

#include "ocean/geometry/EpipolarGeometry.h"

namespace Ocean
{

namespace Geometry
{

FisheyeEpipolarGeometry::FisheyeEpipolarGeometry(const SharedAnyCamera& camera0, const SharedAnyCamera& camera1, const HomogenousMatrix4& camera0_T_camera1)
{
	ocean_assert(camera0 && camera0->isValid());
	ocean_assert(camera1 && camera1->isValid());
	ocean_assert(camera0_T_camera1.isValid());

	if (!updateCameras(camera0, camera1, camera0_T_camera1))
	{
		ocean_assert(false && "Failed to initialize the epipolar geometry - this should never happen!");
	}
}

bool FisheyeEpipolarGeometry::isValid() const
{
	if (!fisheyeCamera0_ || !fisheyeCamera0_->isValid())
	{
		return false;
	}

	if (!fisheyeCamera1_ || !fisheyeCamera1_->isValid())
	{
		return false;
	}

	if (!pinholeCamera0_ || !pinholeCamera0_->isValid())
	{
		return false;
	}

	if (!pinholeCamera1_ || !pinholeCamera1_->isValid())
	{
		return false;
	}

	if (!camera0_T_camera1_.isValid())
	{
		return false;
	}

	// TODO Check: camera1_E_camera0_ and camera1_F_camera0_

	return true;
}

bool FisheyeEpipolarGeometry::updateCameras(const SharedAnyCamera& camera0, const SharedAnyCamera& camera1, const HomogenousMatrix4& camera0_T_camera1)
{
	if (!camera0 || !camera0->isValid() || !camera1 || !camera1->isValid() || !camera0_T_camera1.isValid())
	{
		ocean_assert(false && "Failed to update the cameras - this should never happen!");
		return false;
	}

	fisheyeCamera0_ = camera0;
	fisheyeCamera1_ = camera1;

	camera0_T_camera1_ = camera0_T_camera1;

	pinholeCamera0_ = std::make_shared<AnyCameraPinhole>(PinholeCamera(fisheyeCamera0_->width() * 75u / 100u, fisheyeCamera0_->height() * 75u / 100u, Numeric::deg2rad(70)));
	pinholeCamera1_ = std::make_shared<AnyCameraPinhole>(PinholeCamera(fisheyeCamera1_->width() * 75u / 100u, fisheyeCamera1_->height() * 75u / 100u, Numeric::deg2rad(70)));

	const SquareMatrix3 intrinsicsPinholeCamera0(Vector3(pinholeCamera0_->focalLengthX(), 0, 0), Vector3(0, pinholeCamera0_->focalLengthY(), 0), Vector3(pinholeCamera0_->principalPointX(), pinholeCamera0_->principalPointY(), 1));
	const SquareMatrix3 intrinsicsPinholeCamera1(Vector3(pinholeCamera1_->focalLengthX(), 0, 0), Vector3(0, pinholeCamera1_->focalLengthY(), 0), Vector3(pinholeCamera1_->principalPointX(), pinholeCamera1_->principalPointY(), 1));

	camera1_E_camera0_ = Geometry::EpipolarGeometry::essentialMatrix(camera0_T_camera1);
	camera1_F_camera0_ = Geometry::EpipolarGeometry::essential2fundamental(camera1_E_camera0_, intrinsicsPinholeCamera0, intrinsicsPinholeCamera1);

	ocean_assert(isValid());

	return true;
}

bool FisheyeEpipolarGeometry::epipolarLine(const Vector2& sourcePointFisheye, Vectors2& fisheyeEpipolarLineSegments, const CameraIdentifier sourceCameraIdentifier, const Scalar lineStep, const size_t maxNumberLineSegments) const
{
	ocean_assert(sourceCameraIdentifier == CI_CAMERA0 || sourceCameraIdentifier == CI_CAMERA1);

	if (!isValid())
	{
		ocean_assert(false && "This function must only be called on epipolar geometry instances that are initialized - this should never happen!");
		return false;
	}

	Line2 targetEpipolarLinePinhole;
	if (!epipolarLine(sourceCameraIdentifier, sourcePointFisheye, targetEpipolarLinePinhole))
	{
		return false;
	}

	ocean_assert(targetEpipolarLinePinhole.isValid());

	const AnyCamera& targetCameraFisheye = sourceCameraIdentifier == CI_CAMERA0 ? *fisheyeCamera1_ : *fisheyeCamera0_;
	const AnyCamera& targetCameraPinhole = sourceCameraIdentifier == CI_CAMERA0 ? *pinholeCamera1_ : *pinholeCamera0_;

	const Scalar maxDistance = Scalar(std::min(targetCameraFisheye.width(), targetCameraFisheye.height()) / 2u);
	const Scalar maxSqrDistance = maxDistance * maxDistance;

	Vector2 nextPinholeLinePoint = targetEpipolarLinePinhole.point();
	unsigned int counter = 0u;

	// Convert the epipolar line from the pinhole camera to the fisheye camera.
	//
	// This is done by traversing the epipolar line with a specific step width along 1) the negative direction
	// of the epipolar line and 2) along its positive direction. Starting point is the line point. Each of the
	// segments is projected back from pinhole camera to the fisheye camera. The traversal is stopped if a
	// certain distance to the image center is exceeded.
	//
	// The epipolar line in the fisheye defined as a sequence of connected points.
	std::deque<Vector2> localFisheyeEpipolarLineSegments;

	// Traverse the epipolar along its negative direction
	while (localFisheyeEpipolarLineSegments.size() < (maxNumberLineSegments / 2u))
	{
		const Vector2 nextFisheyeLinePoint = reprojectPoint(targetCameraPinhole, targetCameraFisheye, nextPinholeLinePoint);
		const Vector2 center_t_nextPoint = nextFisheyeLinePoint - Vector2(targetCameraFisheye.width() / 2u, targetCameraFisheye.height() / 2u);

		if (center_t_nextPoint.sqr() >= maxSqrDistance)
		{
			break;
		}

		localFisheyeEpipolarLineSegments.emplace_front(nextFisheyeLinePoint);

		nextPinholeLinePoint = targetEpipolarLinePinhole.point(Scalar(counter) * -lineStep);
		counter++;
	}

	// Traverse the epipolar along its positive direction
	nextPinholeLinePoint = targetEpipolarLinePinhole.point(lineStep);
	counter = 2u;

	while (localFisheyeEpipolarLineSegments.size() < maxNumberLineSegments)
	{
		const Vector2 nextFisheyeLinePoint = reprojectPoint(targetCameraPinhole, targetCameraFisheye, nextPinholeLinePoint);
		const Vector2 center_t_nextPoint = nextFisheyeLinePoint - Vector2(targetCameraFisheye.width() / 2u, targetCameraFisheye.height() / 2u);

		if (center_t_nextPoint.sqr() >= maxSqrDistance)
		{
			break;
		}

		localFisheyeEpipolarLineSegments.emplace_back(nextFisheyeLinePoint);

		nextPinholeLinePoint = targetEpipolarLinePinhole.point(Scalar(counter) * lineStep);
		counter++;
	}

	fisheyeEpipolarLineSegments = Vectors2(localFisheyeEpipolarLineSegments.begin(), localFisheyeEpipolarLineSegments.end());

	return true;
}

bool FisheyeEpipolarGeometry::isOnEpipolarLine(const CameraIdentifier sourceCameraIdentifier, const Vector2& sourcePointFisheye, const Vector2& targetPointFisheye, const Scalar maxDistance) const
{
	ocean_assert(sourceCameraIdentifier == CI_CAMERA0 || sourceCameraIdentifier == CI_CAMERA1);

	if (!isValid())
	{
		ocean_assert(false && "This function must only be called on epipolar geometry instances that are initialized - this should never happen!");
		return false;
	}

	Line2 targetEpipolarLinePinhole;
	if (!epipolarLine(sourceCameraIdentifier, sourcePointFisheye, targetEpipolarLinePinhole))
	{
		return false;
	}

	ocean_assert(targetEpipolarLinePinhole.isValid());

	const AnyCamera& targetCameraFisheye = sourceCameraIdentifier == CI_CAMERA0 ? *fisheyeCamera1_ : *fisheyeCamera0_;
	const AnyCamera& targetCameraPinhole = sourceCameraIdentifier == CI_CAMERA0 ? *pinholeCamera1_ : *pinholeCamera0_;

	const Vector2 targetPointPinhole = reprojectPoint(targetCameraFisheye, targetCameraPinhole, targetPointFisheye);
	const Scalar sqrDistance = targetEpipolarLinePinhole.sqrDistance(targetPointPinhole);

	return sqrDistance <= maxDistance * maxDistance;
}

Vector2 FisheyeEpipolarGeometry::reprojectPoint(const AnyCamera& sourceCamera, const AnyCamera& targetCamera, const Vector2& sourcePoint) const
{
	ocean_assert(sourceCamera.isValid() && targetCamera.isValid());

	const Vector3 sourceVector = sourceCamera.vector(sourcePoint);
	const Vector2 targetPoint = targetCamera.projectToImage(sourceVector);

	return targetPoint;
}

bool FisheyeEpipolarGeometry::epipolarLine(const CameraIdentifier sourceCameraIdentifier, const Vector2& sourcePointFisheye, Line2& epipolarLinePinhole) const
{
	ocean_assert(sourceCameraIdentifier == CI_CAMERA0 || sourceCameraIdentifier == CI_CAMERA1);

	if (!isValid())
	{
		ocean_assert(false && "This function must only be called on epipolar geometry instances that are initialized - this should never happen!");
		return false;
	}

	const AnyCamera& sourceCameraFisheye = sourceCameraIdentifier == CI_CAMERA0 ? *fisheyeCamera0_ : *fisheyeCamera1_;
	const AnyCamera& sourceCameraPinhole = sourceCameraIdentifier == CI_CAMERA0 ? *pinholeCamera0_ : *pinholeCamera1_;

	const Vector2 sourcePointPinhole = reprojectPoint(sourceCameraFisheye, sourceCameraPinhole, sourcePointFisheye);

	Line2 targetEpipolarLinePinhole;
	if (sourceCameraIdentifier == CI_CAMERA0)
	{
		targetEpipolarLinePinhole = Geometry::EpipolarGeometry::rightEpipolarLine(camera1_F_camera0_, sourcePointPinhole);
	}
	else
	{
		targetEpipolarLinePinhole = Geometry::EpipolarGeometry::leftEpipolarLine(camera1_F_camera0_, sourcePointPinhole);
	}

	if (!targetEpipolarLinePinhole.isValid())
	{
		ocean_assert(false && "The epipolar line is not valid - this should never happen!");
		return false;
	}

	epipolarLinePinhole = targetEpipolarLinePinhole;

	return true;
}

}

}
