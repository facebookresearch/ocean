/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/depth/Rectifier.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

namespace Ocean
{

namespace CV
{

namespace Depth
{

bool Rectifier::rectify(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Frame& frameA, const Frame& frameB, const PinholeCamera& pinholeCamera, Frame& rectifiedFrameA, Frame& rectifiedFrameB, HomogenousMatrix4& world_T_rectifiedA, HomogenousMatrix4& world_T_rectifiedB, Worker* worker)
{
	ocean_assert(cameraA.isValid());
	ocean_assert(cameraB.isValid());

	ocean_assert(world_T_cameraA.isValid());
	ocean_assert(world_T_cameraB.isValid());

	ocean_assert(frameA.isValid());
	ocean_assert(frameB.isValid());

	ocean_assert(pinholeCamera.isValid());

	Quaternion world_R_rectified;
	if (!detemineRectificationRotation(world_T_cameraA, world_T_cameraB, world_R_rectified))
	{
		return false;
	}

	const Quaternion cameraA_R_world = world_T_cameraA.rotation().inverted();
	const Quaternion cameraB_R_world = world_T_cameraB.rotation().inverted();

	const SquareMatrix3 cameraA_R_rectified(cameraA_R_world * world_R_rectified);
	const SquareMatrix3 cameraB_R_rectified(cameraB_R_world * world_R_rectified);

#ifdef OCEAN_DEBUG
		const HomogenousMatrix4 rectifiedA_T_rectifiedB = HomogenousMatrix4(cameraA_R_rectified.inverted()) * world_T_cameraA.inverted() * world_T_cameraB * HomogenousMatrix4(cameraB_R_rectified);
		constexpr Scalar debugEps = Scalar(0.0001);
		ocean_assert(Numeric::isEqual(rectifiedA_T_rectifiedB.rotation().angle(), 0, debugEps));
		ocean_assert(Numeric::isEqual(rectifiedA_T_rectifiedB.translation().length(), world_T_cameraB.translation().distance(world_T_cameraA.translation()), debugEps));
		ocean_assert(Numeric::isEqualEps(rectifiedA_T_rectifiedB.translation().y()) && Numeric::isEqualEps(rectifiedA_T_rectifiedB.translation().z()));
#endif

	constexpr unsigned int binSizeInPixel = 4u;

	if (!Ocean::CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(frameA, cameraA, cameraA_R_rectified, AnyCameraPinhole(pinholeCamera), rectifiedFrameA, nullptr, worker, binSizeInPixel))
	{
		ocean_assert(false && "This should never happen!");

		Log::error() << "Failed to rectify first image";
		return false;
	}

	if (!Ocean::CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(frameB, cameraB, cameraB_R_rectified, AnyCameraPinhole(pinholeCamera), rectifiedFrameB, nullptr, worker, binSizeInPixel))
	{
		ocean_assert(false && "This should never happen!");

		Log::error() << "Failed to rectify second image";
		return false;
	}

	world_T_rectifiedA = world_T_cameraA * HomogenousMatrix4(cameraA_R_rectified);
	world_T_rectifiedB = world_T_cameraB * HomogenousMatrix4(cameraB_R_rectified);

	return true;
}

bool Rectifier::detemineRectificationRotation(const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, Quaternion& world_R_rectified)
{
	ocean_assert(world_T_cameraA.isValid());
	ocean_assert(world_T_cameraB.isValid());

	// the new x-axis will be identical to the baseline between both cameras (in world)

	Vector3 xAxis = world_T_cameraB.translation() - world_T_cameraA.translation();

	if (!xAxis.normalize())
	{
		Log::error() << "Both camera centers are identical";
		return false;
	}

	if (world_T_cameraA.xAxis() * xAxis > 0)
	{
		Log::debug() << "First camera is left of second camera";
	}
	else
	{
		Log::debug() << "Second camera is left of first camera, images will be upside down";
	}

	// now, we define the new y-axis, perpendicular to the new x-axis and the old z-axis

	Vector3 yAxis = world_T_cameraA.rotationMatrix().zAxis().cross(xAxis);

	// now, we update the z-axis

	Vector3 zAxis = xAxis.cross(yAxis);

	yAxis = zAxis.cross(xAxis).normalized();
	zAxis = xAxis.cross(yAxis).normalized();

	ocean_assert(SquareMatrix3(xAxis, yAxis, zAxis).isOrthonormal());

	// we have the new rotation for both cameras defined in world

	world_R_rectified = Quaternion(SquareMatrix3(xAxis, yAxis, zAxis));

	return true;
}

}

}

}
