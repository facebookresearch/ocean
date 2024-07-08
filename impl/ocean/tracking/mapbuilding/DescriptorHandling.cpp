/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/DescriptorHandling.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool DescriptorHandling::computeFreakDescriptor(const CV::FramePyramid& yFramePyramid, const AnyCamera& anyCamera, const Vector2& point, FreakMultiDescriptor256& freakDescriptor)
{
	ocean_assert(yFramePyramid.finestWidth() == anyCamera.width() && yFramePyramid.finestHeight() == anyCamera.height());
	ocean_assert(yFramePyramid.frameType().isPixelFormatCompatible(FrameType::FORMAT_Y8));

	const Vector3 unprojectRayIF = anyCamera.vectorIF(point);

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	anyCamera.pointJacobian2x3IF(unprojectRayIF, jacobianX, jacobianY);

	FreakMultiDescriptor256::CameraDerivativeData data;
	data.unprojectRayIF = Eigen::Vector3f(float(unprojectRayIF.x()), float(unprojectRayIF.y()), float(unprojectRayIF.z()));
	data.pointJacobianMatrixIF << float(jacobianX[0]), float(jacobianX[1]), float(jacobianX[2]), float(jacobianY[0]), float(jacobianY[1]), float(jacobianY[2]);

	return CV::Detector::FREAKDescriptor32::computeDescriptor(yFramePyramid, {point.x(), point.y()}, 0u, freakDescriptor, data.unprojectRayIF, float(anyCamera.inverseFocalLengthX()), data.pointJacobianMatrixIF);
}

bool DescriptorHandling::replaceDescriptorPyramid(const CV::FramePyramid& yFramePyramid, CV::FramePyramid& yFramePyramidForDescriptors, Worker* worker)
{
	ocean_assert(yFramePyramid.isValid());

	if (!yFramePyramid.isValid())
	{
		return false;
	}

	if (yFramePyramidForDescriptors.layers() != yFramePyramid.layers())
	{
		yFramePyramidForDescriptors = CV::FramePyramid(yFramePyramid.finestLayer(), yFramePyramid.layers(), true /*coypFirstLayer*/, worker);
	}

	for (unsigned int n = 0u; n < yFramePyramid.layers(); ++n)
	{
		CV::FrameFilterGaussian::filter(yFramePyramid[n], yFramePyramidForDescriptors[n], 3u, worker);
	}

	return true;
}

}

}

}
