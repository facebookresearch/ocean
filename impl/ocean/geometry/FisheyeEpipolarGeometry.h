/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_FISHEYEEPIPOLARGEOMETRY_H
#define META_OCEAN_GEOMETRY_FISHEYEEPIPOLARGEOMETRY_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line2.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements fisheye epipolar geometry functionality.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT FisheyeEpipolarGeometry
{
	public:

		/**
		 * Definition of camera identifiers.
		 */
		enum CameraIdentifier : unsigned int
		{
			/// Camera 0
			CI_CAMERA0 = 0u,
			/// Camera 1
			CI_CAMERA1
		};

		/**
		 * Definition of an epipolar line as a vector of 2D points.
		 */
		using EpipolarLine = std::vector<Vector2>;

		/**
		 * Definition of a vector holding epipolar lines.
		 */
		using EpipolarLines = std::vector<EpipolarLine>;

	public:

		/**
		 * Default constructor creating an invalid epipolar geometry object.
		 */
		FisheyeEpipolarGeometry() = default;

		/**
		 * Creates a new fisheye epipolar geometry object.
		 * @param camera0 The first camera, must be valid
		 * @param camera1 The second camera, must be valid
		 * @param camera0_T_camera1 The transformation from camera 1 to camera 0, must be valid
		 */
		FisheyeEpipolarGeometry(const SharedAnyCamera& camera0, const SharedAnyCamera& camera1, const HomogenousMatrix4& camera0_T_camera1);

		/**
		 * Returns whether this epipolar geometry object is valid.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Updates the cameras and transformation of this epipolar geometry object.
		 * @param camera0 The first camera, must be valid
		 * @param camera1 The second camera, must be valid
		 * @param camera0_T_camera1 The transformation from camera 1 to camera 0, must be valid
		 * @return True, if succeeded
		 */
		bool updateCameras(const SharedAnyCamera& camera0, const SharedAnyCamera& camera1, const HomogenousMatrix4& camera0_T_camera1);

		/**
		 * Computes the epipolar line in the target camera corresponding to a point in the source camera.
		 * @param sourcePointFisheye The point in the source camera, with range [0, sourceCamera.width())x[0, sourceCamera.height())
		 * @param fisheyeEpipolarLineSegments The resulting epipolar line in the target camera as a sequence of connected points
		 * @param sourceCameraIdentifier The identifier of the source camera, either CI_CAMERA0 or CI_CAMERA1
		 * @param lineStep The step size along the epipolar line, in pixels, with range (0, infinity)
		 * @param maxNumberLineSegments The maximum number of line segments to compute, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool epipolarLine(const Vector2& sourcePointFisheye, Vectors2& fisheyeEpipolarLineSegments, const CameraIdentifier sourceCameraIdentifier = CI_CAMERA0, const Scalar lineStep = Scalar(70), const size_t maxNumberLineSegments = 100u) const;

		/**
		 * Determines whether a target point lies on the epipolar line corresponding to a source point.
		 * @param sourceCameraIdentifier The identifier of the source camera, either CI_CAMERA0 or CI_CAMERA1
		 * @param sourcePointFisheye The point in the source camera, with range [0, sourceCamera.width())x[0, sourceCamera.height())
		 * @param targetPointFisheye The point in the target camera to check, with range [0, targetCamera.width())x[0, targetCamera.height())
		 * @param maxDistance The maximum allowed distance from the epipolar line, in pixels, with range [0, infinity)
		 * @return True, if the target point lies on the epipolar line
		 */
		bool isOnEpipolarLine(const CameraIdentifier sourceCameraIdentifier, const Vector2& sourcePointFisheye, const Vector2& targetPointFisheye, const Scalar maxDistance = Scalar(2)) const;

	protected:

		/**
		 * Reprojects a point from one camera to another.
		 * @param sourceCamera The source camera, must be valid
		 * @param targetCamera The target camera, must be valid
		 * @param sourcePoint The point in the source camera
		 * @return The reprojected point in the target camera
		 */
		Vector2 reprojectPoint(const AnyCamera& sourceCamera, const AnyCamera& targetCamera, const Vector2& sourcePoint) const;

		/**
		 * Computes the epipolar line in the target camera corresponding to a point in the source camera (in pinhole space).
		 * @param sourceCameraIdentifier The identifier of the source camera, either CI_CAMERA0 or CI_CAMERA1
		 * @param sourcePointFisheye The point in the source camera (fisheye space), with range [0, sourceCamera.width())x[0, sourceCamera.height())
		 * @param epipolarLinePinhole The resulting epipolar line in the target camera (pinhole space)
		 * @return True, if succeeded
		 */
		bool epipolarLine(const CameraIdentifier sourceCameraIdentifier, const Vector2& sourcePointFisheye, Line2& epipolarLinePinhole) const;

	protected:

		/// The first fisheye camera
		SharedAnyCamera fisheyeCamera0_;

		/// The second fisheye camera
		SharedAnyCamera fisheyeCamera1_;

		/// The first pinhole camera
		SharedAnyCamera pinholeCamera0_;

		/// The second pinhole camera
		SharedAnyCamera pinholeCamera1_;

		/// The transformation from camera 1 to camera 0
		HomogenousMatrix4 camera0_T_camera1_;

		/// The essential matrix from camera 0 to camera 1
		SquareMatrix3 camera1_E_camera0_;

		/// The fundamental matrix from camera 0 to camera 1
		SquareMatrix3 camera1_F_camera0_;
};

} // namespace Geometry

} // namespace Ocean

#endif // META_OCEAN_GEOMETRY_FISHEYEEPIPOLARGEOMETRY_H
