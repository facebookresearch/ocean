/*
 * Portions Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

FREAKDescriptor::PinholeCameraDerivativeFunctor::PinholeCameraDerivativeFunctor(const PinholeCamera& pinholeCamera, const unsigned int pyramidLevels)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(pyramidLevels != 0u);

	cameras_.reserve(pyramidLevels);
	cameras_.emplace_back(pinholeCamera);

	unsigned int width = pinholeCamera.width();
	unsigned int height = pinholeCamera.height();

	for (unsigned int level = 1u; level < pyramidLevels; ++level)
	{
		width /= 2u;
		height /= 2u;

		if (width == 0u || height == 0u)
		{
			break;
		}

		cameras_.emplace_back(width, height, pinholeCamera);
	}

	cameras_.shrink_to_fit();
}

FREAKDescriptor::CameraDerivativeData FREAKDescriptor::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, float& inverseFocalLength) const
{
	ocean_assert(pointPyramidLevel < cameras_.size());

	inverseFocalLength = float(cameras_[pointPyramidLevel].inverseFocalLengthX() + cameras_[pointPyramidLevel].inverseFocalLengthY()) * 0.5f;

	return computeCameraDerivativeData(cameras_[pointPyramidLevel], point);
}

unsigned int FREAKDescriptor::PinholeCameraDerivativeFunctor::supportedPyramidLevels() const
{
	return (unsigned int)(cameras_.size());
}

FREAKDescriptor::CameraDerivativeData FREAKDescriptor::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(const PinholeCamera& pinholeCamera, const Eigen::Vector2f& point)
{
	const Vector3 unprojectRayIF = pinholeCamera.vectorIF(Vector2(point.x(), point.y()));
	ocean_assert(Numeric::isEqualEps((Vector3((Scalar(point.x()) - pinholeCamera.principalPointX()) * pinholeCamera.inverseFocalLengthX(), (Scalar(point.y()) - pinholeCamera.principalPointY()) * pinholeCamera.inverseFocalLengthY(), 1.0f).normalized() - unprojectRayIF).length()));

	// TODOX Revisit this when enabling camera distortions
	ocean_assert(pinholeCamera.hasDistortionParameters() == false);

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	Geometry::Jacobian::calculatePointJacobian2x3(jacobianX, jacobianY, pinholeCamera, HomogenousMatrix4(true), unprojectRayIF, /* distort */ false);

	CameraDerivativeData data;

	data.unprojectRayIF = Eigen::Vector3f(float(unprojectRayIF.x()), float(unprojectRayIF.y()), float(unprojectRayIF.z()));

	// Note: the assignment below is row-major order but Eigen memory will be column-major. I know ...
	data.pointJacobianMatrixIF << float(jacobianX[0]), float(jacobianX[1]), float(jacobianX[2]), float(jacobianY[0]), float(jacobianY[1]), float(jacobianY[2]);
	ocean_assert(data.pointJacobianMatrixIF.IsRowMajor == false);

	return data;
}

FREAKDescriptor::AnyCameraDerivativeFunctor::AnyCameraDerivativeFunctor(const SharedAnyCamera& camera, const unsigned int pyramidLevels)
{
	ocean_assert(camera && camera->isValid());
	ocean_assert(pyramidLevels != 0u);

	cameras_.reserve(pyramidLevels);
	cameras_.emplace_back(camera);

	unsigned int width = camera->width();
	unsigned int height = camera->height();

	for (unsigned int level = 1u; level < pyramidLevels; ++level)
	{
		width /= 2u;
		height /= 2u;

		if (width == 0u || height == 0u)
		{
			break;
		}

		cameras_.emplace_back(cameras_.back()->clone(width, height));
	}

	inverseFocalLengths_.reserve(pyramidLevels);
	for (const SharedAnyCamera& levelCamera : cameras_)
	{
		ocean_assert(levelCamera && levelCamera->isValid());

		const float inverseFocalLength = float(levelCamera->inverseFocalLengthX() + levelCamera->inverseFocalLengthY()) * 0.5f;

		inverseFocalLengths_.emplace_back(inverseFocalLength);
	}
}

FREAKDescriptor::CameraDerivativeData FREAKDescriptor::AnyCameraDerivativeFunctor::computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, float& inverseFocalLength) const
{
	ocean_assert(pointPyramidLevel < cameras_.size());
	ocean_assert(cameras_.size() == inverseFocalLengths_.size());

	inverseFocalLength = inverseFocalLengths_[pointPyramidLevel];

	return computeCameraDerivativeData(*cameras_[pointPyramidLevel], point);
}

unsigned int FREAKDescriptor::AnyCameraDerivativeFunctor::supportedPyramidLevels() const
{
	return (unsigned int)(cameras_.size());
}

FREAKDescriptor::CameraDerivativeData FREAKDescriptor::AnyCameraDerivativeFunctor::computeCameraDerivativeData(const AnyCamera& camera, const Eigen::Vector2f& point)
{
	const Vector3 unprojectRayIF = camera.vectorIF(Vector2(point.x(), point.y()));

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	camera.pointJacobian2x3IF(unprojectRayIF, jacobianX, jacobianY);

	CameraDerivativeData data;

	data.unprojectRayIF = Eigen::Vector3f(float(unprojectRayIF.x()), float(unprojectRayIF.y()), float(unprojectRayIF.z()));

	// Note: the assignment below is row-major order but Eigen memory will be column-major. I know ...
	data.pointJacobianMatrixIF << float(jacobianX[0]), float(jacobianX[1]), float(jacobianX[2]), float(jacobianY[0]), float(jacobianY[1]), float(jacobianY[2]);
	ocean_assert(data.pointJacobianMatrixIF.IsRowMajor == false);

	return data;
}

}

}

}
