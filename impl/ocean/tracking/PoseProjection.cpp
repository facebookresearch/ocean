/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/PoseProjection.h"

#include "ocean/geometry/Error.h"

#include <algorithm>

namespace Ocean
{

namespace Tracking
{

PoseProjection::PoseProjection() :
	poseDistortionState(CV::Detector::PointFeature::DS_INVALID)
{
	// nothing to do here
}

PoseProjection::PoseProjection(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const size_t number, const bool distortImagePoints) :
	poseTransformation(pose),
	poseImagePoints(number)
{
	ocean_assert(objectPoints);

	if (number > 0)
		pinholeCamera.projectToImageIF<true>(PinholeCamera::standard2InvertedFlipped(pose), objectPoints, number, distortImagePoints, poseImagePoints.data());

	if (distortImagePoints)
		poseDistortionState = CV::Detector::PointFeature::DS_DISTORTED;
	else
		poseDistortionState = CV::Detector::PointFeature::DS_UNDISTORTED;
}

PoseProjectionSet::PoseProjectionSet() :
	projectionSetCameraWidth(0),
	projectionSetCameraHeight(0)
{
	// nothing to do here
}

PoseProjectionSet::~PoseProjectionSet()
{
	// nothing to do here
}

void PoseProjectionSet::clear()
{
	projectionSetPoseProjections.clear();
}

}

}
