/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MULTI_VIEW_PLANE_FINDER_H
#define META_OCEAN_TRACKING_MULTI_VIEW_PLANE_FINDER_H

#include "ocean/tracking/Tracking.h"
#include "ocean/tracking/PlaneFinder.h"

#include "ocean/base/Accessor.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a 3D plane finder that determines a plane within several frames taken from different camera positions.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT MultiViewPlaneFinder : public PlaneFinder
{
	public:

		/**
		 * Determines the initial 3D plane and the corresponding 6DOF poses of the camera.
		 * The plane and camera positions can be determined only up to an unknown scale factor.<br>
		 * This function needs at least two corresponding sets of image points.<br>
		 * @param pinholeCamera The pinhole camera object that is applied for the projection
		 * @param plane Resulting 3D plane
		 * @param poses Resulting poses that correspond to the given image points
		 * @param initialPose Pose of the first camera position
		 * @param initialPlane Initial plane that will be determined more accurate (the plane can be very rough while it should be in front of the initial pose)
		 * @return True, if succeeded
		 */
		bool determinePlane(const PinholeCamera& pinholeCamera, Plane3& plane, HomogenousMatrices4& poses, const HomogenousMatrix4& initialPose = HomogenousMatrix4(Vector3(0, 0, 1)), const Plane3& initialPlane = Plane3(Vector3(0, 0, 1), 0)) const;

		/**
		 * Returns whether this plane finder object holds at least two sets of corresponding image points.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Determines the initial plane from two individual views.
		 * @param pinholeCamera The pinhole camera object that is applied for the projection
		 * @param poseFirst First pose that is associated with the image points from the first view
		 * @param roughPlane Rough plane that should be a very rough guess of the resulting initial plane (at least the plane should lie in front of the camera)
		 * @param imagePointsFirst Image points in the first view associated with the first pose
		 * @param imagePointsSecond Image points in the second view associated with the resulting second pose, each point corresponds to a point in the first view (by index)
		 * @param poseSecond Resulting second pose
		 * @param plane Resulting initial plane best matching to the given image point correspondences
		 * @return True, if succeeded
		 */
		static bool determinePlaneFromTwoViews(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseFirst, const Plane3& roughPlane, const ConstIndexedAccessor<Vector2>& imagePointsFirst, const ConstIndexedAccessor<Vector2>& imagePointsSecond, HomogenousMatrix4& poseSecond, Plane3& plane);

		/**
		 * Determines the poses that correspond to the given sets of image point correspondences and the given rough plane.
		 * @param pinholeCamera The pinhole camera object that is applied for the projection
		 * @param poseFirst First pose that is associated with the image points from the first view
		 * @param plane 3D plane on that all 3D object points visible in the first view are located
		 * @param imagePointsFirst Image points in the first view associated with the first pose
		 * @param imagePointsSuccessive The sets of image points visible in the successive views
		 * @param posesSuccessive Resulting poses for the successive views
		 * @return True, if succeeded
		 */
		static bool determineInitialPoses(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseFirst, const Plane3& plane, const Vectors2& imagePointsFirst, const std::vector<Vectors2>& imagePointsSuccessive, HomogenousMatrices4& posesSuccessive);
};

inline MultiViewPlaneFinder::operator bool() const
{
	return imagePointCorrespondences.size() >= 2;
}

}

}

#endif // META_OCEAN_TRACKING_MULTI_VIEW_PLANE_FINDER_H
