/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/MultiViewPlaneFinder.h"

#include "ocean/geometry/NonLinearOptimizationPlane.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace Tracking
{

bool MultiViewPlaneFinder::determinePlane(const PinholeCamera& pinholeCamera, Plane3& plane, HomogenousMatrices4& poses, const HomogenousMatrix4& initialPose, const Plane3& initialPlane) const
{
	ocean_assert(imagePointCorrespondences.size() >= 2);

	const std::vector<Vectors2>& correspondences = imagePointCorrespondences.correspondences();
	if (correspondences.size() < 2)
		return false;

	const Vectors2& imagePointsFirst = correspondences.front();

	std::vector<Vectors2> successiveImagePoints;
	successiveImagePoints.reserve(correspondences.size() - 1);
	for (size_t n = 1; n < correspondences.size(); ++n)
	{
		ocean_assert(imagePointsFirst.size() == correspondences[n].size());
		successiveImagePoints.push_back(correspondences[n]);
	}

	HomogenousMatrix4 secondPose;
	if (!determinePlaneFromTwoViews(pinholeCamera, initialPose, initialPlane, ConstArrayAccessor<Vector2>(imagePointsFirst), ConstArrayAccessor<Vector2>(successiveImagePoints.front()), secondPose, plane))
		return false;

	HomogenousMatrices4 successivePoses;
	if (!determineInitialPoses(pinholeCamera, initialPose, plane, imagePointsFirst, successiveImagePoints, successivePoses))
		return false;

	if (!Geometry::NonLinearOptimizationPlane::optimizePosesPlane(pinholeCamera, initialPose, imagePointsFirst, successivePoses, initialPlane, successiveImagePoints, pinholeCamera.hasDistortionParameters(), poses, plane, 30u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true))
		return false;

	ocean_assert(plane.isValid());

	poses.insert(poses.begin(), initialPose);
	ocean_assert(poses.size() == correspondences.size());

	return true;
}

bool MultiViewPlaneFinder::determinePlaneFromTwoViews(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseFirst, const Plane3& roughPlane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, HomogenousMatrix4& poseSecond, Plane3& plane)
{
	ocean_assert(pinholeCamera.isValid() && poseFirst.isValid() && roughPlane.isValid());
	ocean_assert(imagePointsFirst.size() && imagePointsSecond.size());

	const HomogenousMatrix4 poseFirstOffset(poseFirst * HomogenousMatrix4(Vector3(Scalar(0.01), 0, 0), Euler(Numeric::deg2rad(Scalar(1.5)), 0, 0)));

	return Geometry::NonLinearOptimizationPlane::optimizeOnePoseOnePlane(pinholeCamera, poseFirst, poseFirstOffset, roughPlane, imagePointsFirst, imagePointsSecond, pinholeCamera.hasDistortionParameters(), poseSecond, plane, 30u, Geometry::Estimator::ET_SQUARE);
}

bool MultiViewPlaneFinder::determineInitialPoses(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseFirst, const Plane3& plane, const Vectors2& imagePointsFirst, const std::vector<Vectors2>& imagePointsSuccessive, HomogenousMatrices4& posesSuccessive)
{
	ocean_assert(pinholeCamera.isValid() && poseFirst.isValid() && plane.isValid());
	ocean_assert(!imagePointsSuccessive.empty());
	ocean_assert(imagePointsFirst.size() == imagePointsSuccessive.front().size());

	const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, poseFirst, plane, imagePointsFirst.data(), imagePointsFirst.size(), pinholeCamera.hasDistortionParameters()));
	posesSuccessive.resize(imagePointsSuccessive.size());

	for (size_t n = 0; n < imagePointsSuccessive.size(); ++n)
	{
		const Vectors2& imagePoints = imagePointsSuccessive[n];
		ocean_assert(imagePoints.size() == objectPoints.size());

		if (imagePoints.size() != objectPoints.size())
			return false;

		if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, poseFirst, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), posesSuccessive[n]))
			return false;
	}

	return true;
}

}

}
