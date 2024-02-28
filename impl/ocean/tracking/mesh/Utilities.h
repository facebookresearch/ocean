// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MESH_UTILITIES_H
#define META_OCEAN_TRACKING_MESH_UTILITIES_H

#include "ocean/tracking/mesh/Mesh.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Triangle3.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

class OCEAN_TRACKING_MESH_EXPORT Utilities
{
	 public:

		/**
		 * Converts a known object pose for one camera profile into a new pose for a different camera profile such that the new pose minimizes reprojection error.
		 * Pose matrices for this function are assumed to be world-to-camera, with the camera looking down the +Z axis with +X to the right and +Y down.
		 * Beware: The conversion is an approximation only and does not reflect a mathematically perfect solution.
		 * @param referenceCamera The camera profile for which the known (reference) pose has been determined, must be valid
		 * @param newCamera The new camera profile for which the new pose will be calculated, must be valid
		 * @param referencePose_cameraFlipped_T_world The known (reference) pose which will be converted based on the new camera profile, must be valid
		 * @param newPose_cameraFlipped_T_world The resulting new pose matching with the new camera profile
		 * @return True, if succeeded
		 */
		static bool convertPoseForCameraIF(const PinholeCamera& referenceCamera, const PinholeCamera& newCamera, const HomogenousMatrix4& referencePose_cameraFlipped_T_world, HomogenousMatrix4& newPose_cameraFlipped_T_world);

		/**
		 * Checks whether a triangle is front-facing and visible in an image, i.e., if its normal points towards the camera and it overlaps with the image.
		 * @param triangle3 3D triangle with vertices expressed in world coordinates
		 * @param pinholeCamera The pinhole camera into which we'll project the triangle
		 * @param pose_cameraFlipped_T_world Pose matrix converting world coordinates into camera coordinates
		 * @param imageTriangle2 Output 2D projection of the triangle into the camera
		 * @return True, if the triangle is visible in the image, otherwise false
		 */
		static bool triangleIsVisible(const Triangle3& triangle3, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose_cameraFlipped_T_world, Triangle2& imageTriangle2);
};

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_UTILITIES_H
