/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/Frame2FrameTracker.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

bool Frame2FrameTracker::trackPlanarObject(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& previousPose, const Plane3& previousPlane, const Vector2* previousImagePoints, const Vector2* nextImagePoints, const size_t correspondences, HomogenousMatrix4& nextPose)
{
	ocean_assert(pinholeCamera && previousImagePoints && nextImagePoints);
	ocean_assert(previousPose.isValid() && previousPlane.isValid());

	const Vectors3 previousObjectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, previousPose, previousPlane, previousImagePoints, correspondences, pinholeCamera.hasDistortionParameters()));

	if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, previousPose, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(nextImagePoints, correspondences), pinholeCamera.hasDistortionParameters(), nextPose, 20u, Geometry::Estimator::ET_HUBER))
	{
		return false;
	}

	const HomogenousMatrix4 intermediatePose(nextPose);
	return Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, intermediatePose, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(nextImagePoints, correspondences), pinholeCamera.hasDistortionParameters(), nextPose, 5u, Geometry::Estimator::ET_TUKEY);
}

}

}

}
