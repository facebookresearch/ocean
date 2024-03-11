// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/mesh/Utilities.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

bool Utilities::convertPoseForCameraIF(const PinholeCamera& referenceCamera, const PinholeCamera& newCamera, const HomogenousMatrix4& referencePose_cameraFlipped_T_world, HomogenousMatrix4& newPose_cameraFlipped_T_world)
{
	ocean_assert(newCamera.isValid());
	ocean_assert(referenceCamera.isValid());
	ocean_assert(referencePose_cameraFlipped_T_world.isValid());
	ocean_assert(referencePose_cameraFlipped_T_world.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	if (newCamera == referenceCamera)
	{
		// Both camera profiles are actually identical, so the pose does not change.
		newPose_cameraFlipped_T_world = referencePose_cameraFlipped_T_world;
		return true;
	}

	constexpr size_t kNumberPoints = 5;

	const Scalar cameraWidth = Scalar(newCamera.width());
	const Scalar cameraHeight = Scalar(newCamera.height());

	const Vector2 distortedImagePoints[kNumberPoints] =
	{
		Vector2(cameraWidth * Scalar(0.1), cameraHeight * Scalar(0.1)), // top left
		Vector2(cameraWidth * Scalar(0.1), cameraHeight * Scalar(0.9)), // bottom left
		Vector2(cameraWidth * Scalar(0.9), cameraHeight * Scalar(0.9)), // bottom right
		Vector2(cameraWidth * Scalar(0.9), cameraHeight * Scalar(0.1)), // top right
		Vector2(cameraWidth * Scalar(0.5), cameraHeight * Scalar(0.5)) // center
	};

	Vector3 objectPoints[kNumberPoints];

	const HomogenousMatrix4 referencePose_world_T_camera = PinholeCamera::invertedFlipped2Standard(referencePose_cameraFlipped_T_world);

	// Project to a plane through the origin that is orthogonal to the viewing direction.
	const Plane3 plane(Vector3(0, 0, 0), referencePose_world_T_camera.zAxis().normalizedOrZero());

	for (size_t n = 0; n < kNumberPoints; ++n)
	{
		const Vector2& distortedImagePoint = distortedImagePoints[n];
		const Vector2 undistortedImagePoint = referenceCamera.undistortDamped(distortedImagePoint);

		const Line3 ray = referenceCamera.ray(undistortedImagePoint, referencePose_world_T_camera);

		if (!plane.intersection(ray, objectPoints[n]) || !referenceCamera.isObjectPointInFrontIF(referencePose_cameraFlipped_T_world, objectPoints[n]))
		{
			newPose_cameraFlipped_T_world = referencePose_cameraFlipped_T_world;
			return false;
		}
	}

	// Now, use the object points to determine the camera pose for the new camera profile.
	return Geometry::NonLinearOptimizationPose::optimizePoseIF(newCamera, referencePose_cameraFlipped_T_world, ConstArrayAccessor<Vector3>(objectPoints, kNumberPoints), ConstArrayAccessor<Vector2>(distortedImagePoints, kNumberPoints), true, newPose_cameraFlipped_T_world);
}

bool Utilities::triangleIsVisible(const Triangle3& triangle3, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose_cameraFlipped_T_world, Triangle2& imageTriangle2)
{
	imageTriangle2 = pinholeCamera.projectToImageIF<true>(pose_cameraFlipped_T_world, triangle3, pinholeCamera.hasDistortionParameters());

	// Check that the triangle is front-facing: Assuming the triangle's normal faces outward from the
	// surface of the object, a visible triangle will have a normal that points towards the camera.
	// After projection, this still holds -- if we take the cross product n of the two triangle legs
	// in the z=0 plane, a visible triangle will have n.z < 0 in the "inverted, flipped" camera
	// coordinate convention. This is equivalent to checking that the three projected points have a
	// counter-clockwise ordering in the image (note that the image coordinate frame also has the
	// y-axis pointing down).
	const Vector2 segment10 = imageTriangle2.point0() - imageTriangle2.point1();
	const Vector2 segment12 = imageTriangle2.point2() - imageTriangle2.point1();
	if (Numeric::isAbove(segment12.cross(segment10), Scalar(0.0)))
	{
		return false;
	}

	// Split the image into two triangles and see if the current triangle overlaps either.
	const Triangle2 upperTriangle(Vector2(Scalar(0.0), Scalar(0.0)), Vector2(Scalar(0.0), pinholeCamera.height()), Vector2(Scalar(pinholeCamera.width()), Scalar(0.0)));
	const Triangle2 lowerTriangle(Vector2(Scalar(0.0), pinholeCamera.height()), Vector2(Scalar(pinholeCamera.width()), Scalar(0.0)), Vector2(Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height())));

	return imageTriangle2.intersects(upperTriangle) || imageTriangle2.intersects(lowerTriangle);
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
