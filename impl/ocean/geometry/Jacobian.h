/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_JACOBIAN_H
#define META_OCEAN_GEOMETRY_JACOBIAN_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/ExponentialMap.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Pose.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/StaticMatrix.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements function to calculate the jacobian matrices for geometry functions.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Jacobian
{
	public:

		/**
		 * This function determines the 3x3 Jacobian of a rotation function rotating a 3D object point by application of an exponential map.
		 * The given exponential map stores the rotation as the rotation axis with a vector length equal to the rotation angle.<br>
		 * The Jacobian is determined by application of the Rodrigues formula for the specified exponential map.<br>
		 * The resulting 3x3 Jacobian depends on the three rotation parameters and on the 3D object point.<br>
		 * However, we can separate the dependency allowing to calculate the major parts of the Jacobian for the rotation parameters first.<br>
		 * Therefore, we calculate three 3x3 matrices depending only on the rotation parameters.<br>
		 * Each of the matrix can be used to calculate one column of the final 3x3 Jacobian by multiplication with the 3D object point.<br>
		 * Thus, we can reuse the three 3x3 matrices if we have several 3D object points which can improve the performance significantly.<br>
		 * The final 3x3 Jacobian for the provided exponential map and an object point O is defined by the following three vectors:<br>
		 * [dwx * O | dwy * O | dwz * O]
		 * @param rotation The rotation for which the three derivative rotations will be determined
		 * @param dwx The resulting rotation matrix derived to wx
		 * @param dwy The resulting rotation matrix derived to wy
		 * @param dwz The resulting rotation matrix derived to wz
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static void calculateRotationRodriguesDerivative(const ExponentialMapT<T>& rotation, SquareMatrixT3<T>& dwx, SquareMatrixT3<T>& dwy, SquareMatrixT3<T>& dwz);

		/**
		 * Calculates the three jacobian rows for a given exponential rotation map representing the location of a 3D object point.
		 * The rotation map defines the rotation of the vector [0, 0, -objectPointDistance].<br>
		 * The corresponding function f, which transforms the 3D object point defined in coordinates into a 3D object point defined in the Cartesian coordinate system, is given as follows:<br>
		 * f(w, r) = f(wx, wy, wz, r) = R(wx, wy, wz) * [0, 0, -r] = [ox, oy, oz]<br>
		 * The resulting 3x3 jacobian has the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * | dfz / dwx, dfz / dwy, dfz / dwz |
		 * </pre>
		 * @param jx First row of the resulting jacobian, with 3 column entries
		 * @param jy Second row of the resulting jacobian, with 3 column entries
		 * @param jz Third row of the resulting jacobian, with 3 column entries
		 * @param sphericalObjectPoint The rotation defining the 3D object point for which the derivatives will be determined
		 * @param objectPointDistance The distance of the 3D object point, which is the distance to the origin, with range (0, infinity)
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static void calculateSphericalObjectPointJacobian3x3(T* jx, T* jy, T* jz, const ExponentialMapT<T>& sphericalObjectPoint, const T objectPointDistance);

		/**
		 * Calculates the two jacobian rows for a given exponential rotation map representing the location of a 3D object point projecting into the camera frame with orientational camera pose.
		 * The rotation map defines the rotation of the vector [0, 0, -objectPointDistance].<br>
		 * The corresponding function f is given as follows:<br>
		 * f(w, r) = fproj(fori(fobj(wx, wy, wz, r))) = [x, y]<br>
		 * Where fobj translates the location of the 3D object point defined in spherical coordinates into a 3D object point defined in the Cartesian coordinate system.
		 * fproj defines the function which projects (and optional distorts) a 3D object point into the camera,<br>
		 * and fori defines the function which applies the orientation (rotational pose) of the camera.<br>
		 * The resulting 2x3 jacobian has the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * </pre>
		 * @param jx First row of the resulting jacobian, with 3 column entries
		 * @param jy Second row of the resulting jacobian, with 3 column entries
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_R_world The inverted and flipped orientation of the camera, transforming the world to the flipped camera, with default flipped camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param sphericalObjectPoint The rotation defining the 3D object point for which the derivatives will be determined
		 * @param objectPointDistance The distance of the 3D object point, which is the distance to the origin, with range (0, infinity)
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static void calculateSphericalObjectPointOrientationJacobian2x3IF(T* jx, T* jy, const AnyCameraT<T>& camera, const SquareMatrixT3<T>& flippedCamera_R_world, const ExponentialMapT<T>& sphericalObjectPoint, const T objectPointDistance);

		/**
		 * Calculates the two Jacobian rows for the 3-DOF rotational part of a 6-DOF camera pose and a given 3D object point.
		 * The 6-DOF camera pose is separated into a (fixed) translational part and a (flexible) rotational part.<br>
		 * This function uses the pre-calculated 3x3 Jacobian matrix of the camera's orientation provided by three separated 3x3 matrices.<br>
		 * The 3 derivatives are calculated for the 3-DOF orientation.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * </pre>
		 * With transformation function f = (fx, fy) and exponential map w = (wx, wy, wz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.
		 * In the following, how to separate a common (inverted flipped) 6-DOF camera pose into a translational and rotational part.
		 * <pre>
		 * world_T_camera:     translational part   rotational part
		 * | R | t |           | I | t |            | R | 0 |
		 * | 0 | 1 |         = | 0 | 1 |      *     | 0 | 1 |
		 *
		 * flippedCamera_T_world:   rotational part   translational part
		 * | R | t |                | R | 0 |         | I | R^-1 t |
		 * | 0 | 1 |              = | 0 | 1 |    *    | 0 |    1   |
		 * </pre>
		 * @param anyCamera The camera profile to determine the jacobian values for, must be valid
		 * @param flippedCamera_R_translation The rotation between the translational part of the camera and the flipped camera pose, with default flipped camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param translation_T_world The translation between the world and the translational part of the camera pose
		 * @param worldObjectPoint The 3D object point to determine the jacobian for, defined in world
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @param jx The resulting first row of the jacobian, with 3 column entries, must be valid
		 * @param jy The resulting second row of the jacobian, with 3 column entries, must be valid
		 * @see calculateRotationRodriguesDerivative().
		 * @tparam T The scalar data type, either 'float' or 'double'
		 * @tparam TRotation The data type of the provided rotation, either 'QuaternionT<T>' or 'SquareMatrixT3<T>'
		 */
		template <typename T, typename TRotation>
		static OCEAN_FORCE_INLINE void calculateOrientationalJacobianRodrigues2x3IF(const AnyCameraT<T>& anyCamera, const TRotation& flippedCamera_R_translation, const VectorT3<T>& translation_T_world, const VectorT3<T>& worldObjectPoint, const SquareMatrixT3<T>& dwx, const SquareMatrixT3<T>& dwy, const SquareMatrixT3<T>& dwz, T* jx, T* jy);

		/**
		 * Calculates all 3-DOF orientational jacobian rows for a given (flexible) camera pose and a set of static 3D object points.
		 * The 3 derivatives are calculated for the orientation part of the 6-DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * objectPoints.size() rows and 6 columns
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_R_world Inverted and flipped camera pose to determine the jacobian for
		 * @param objectPoints The accessor providing the 3D object points to determine the jacobian for
		 * @tparam T The scalar data type, either 'float' or 'double'
		 * @see calculateOrientationJacobianRodrigues2x3().
		 */
		template <typename T>
		static void calculateOrientationJacobianRodrigues2nx3IF(T* jacobian, const AnyCameraT<T>& camera, const ExponentialMapT<T>& flippedCamera_R_world, const ConstIndexedAccessor<VectorT3<T>>& objectPoints);

		/**
		 * Deprecated.
		 *
		 * Calculates all 3-DOF orientational jacobian rows for a given (flexible) pose and a set of static 3D object points.
		 * The 3 derivatives are calculated for the orientation part of the 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * objectPoints.size() rows and 6 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for, while the rotational part is use only
		 * @param objectPoints The accessor providing the 3D object points to determine the jacobian for
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @see calculateOrientationJacobianRodrigues2x3().
		 */
		static void calculateOrientationJacobianRodrigues2nx3(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints, const bool distortImagePoints);

		/**
		 * Calculates the two jacobian rows for a given (flexible) pose and static camera and one static 3D object point.
		 * The 6 derivatives are calculated for the 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @see calculatePoseJacobianRodrigues2nx6().
		 */
		static inline void calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint);

		/**
		 * Deprecated.
		 *
		 * Calculates the two jacobian rows for a given (flexible) pose and one static 3D object point.
		 * This function uses the pre-calculated 3x3 Jacobian matrix of the camera's orientation provided by three separated 3x3 matrices.<br>
		 * The 6 derivatives are calculated for the 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @see calculateRotationRodriguesDerivative().
		 */
		static void calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz);

		/**
		 * Deprecated.
		 *
		 * Calculates the two jacobian rows for a given (flexible) pose and one static 3D object point.
		 * This function uses the pre-calculated 3x3 Jacobian matrix of the camera's orientation provided by three separated 3x3 matrices.<br>
		 * The 6 derivatives are calculated for the 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param fisheyeCamera Fisheye camera to determine the jacobian values for, must be valid
		 * @param flippedCamera_T_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param worldObjectPoint 3D object point to determine the jacobian for, in world coordinate system
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @see calculateRotationRodriguesDerivative().
		 */
		static void calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldObjectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz);

		/**
		 * Calculates the two jacobian rows for a given (flexible) 6-DOF camera pose and one static 3D object point.
		 * This function uses the pre-calculated 3x3 Jacobian matrix of the camera's orientation provided by three separated 3x3 matrices.<br>
		 * The 6 derivatives are calculated for the 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param anyCamera The camera profile to determine the jacobian values for, must be valid
		 * @param flippedCamera_T_world The inverted and flipped camera pose to determine the jacobian for, with default flipped camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param worldObjectPoint 3D object point to determine the jacobian for, in world coordinate system
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @param jx The resulting first row of the jacobian, with 6 column entries, must be valid
		 * @param jy The resulting second row of the jacobian, with 6 column entries, must be valid
		 * @see calculateRotationRodriguesDerivative().
		 * @tparam T The scalar data type
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE void calculatePoseJacobianRodrigues2x6IF(const AnyCameraT<T>& anyCamera, const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, const SquareMatrixT3<T>& dwx, const SquareMatrixT3<T>& dwy, const SquareMatrixT3<T>& dwz, T* jx, T* jy);

		/**
		 * Calculates all jacobian rows for a given (flexible) 6-DOF camera pose with a static camera profile and several static 3D object points.
		 * The 6 derivatives are calculated for the entire 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the (row major aligned) jacobian matrix, with 2 * numberObjectPoints rows and 6 columns
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_P_world The inverted and flipped pose to determine the jacobian for, with default flipped camera pointing towards the positive z-space with y-axis downwards, must be valid
		 * @param objectPoints The 3D object points to determine the jacobian for, must be valid
		 * @param numberObjectPoints The number of given object points, with range [1, infinity)
		 * @see calculatePoseJacobianRodrigues2x6IF().
		 */
		template <typename T>
		static void calculatePoseJacobianRodrigues2nx6IF(T* jacobian, const AnyCameraT<T>& camera, const PoseT<T>& flippedCamera_P_world, const VectorT3<T>* objectPoints, const size_t numberObjectPoints);

		/**
		 * Deprecated.
		 *
		 * Calculates all pose jacobian rows for a given (flexible) pose with a static camera profile supporting distortion and a set of static 3D object points.
		 * The 6 derivatives are calculated for the entire 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 6 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoints 3D object points to determine the jacobian for
		 * @param numberObjectPoints Number of given object points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @see calculatePoseJacobianRodrigues2x6().
		 */
		static void calculatePoseJacobianRodrigues2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints);

		/**
		 * Calculates all pose jacobian rows for a given (flexible) pose with a static camera profile supporting distortion and a set of static 3D object points.
		 * The distortion is damped for 3D object points not projecting into the camera frame but outside the camera frame.<br>
		 * The 6 derivatives are calculated for the entire 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 6 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param objectPoints 3D object points to determine the jacobian for
		 * @param numberObjectPoints Number of given object points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 * @see calculatePoseJacobianRodrigues2x6().
		 */
		static void calculatePoseJacobianRodriguesDampedDistortion2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar dampingFactor, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints);

		/**
		 * Calculates the two jacobian rows for a given (flexible) pose with (flexible) zoom factor and one static 3D object point.
		 * The 7 derivatives are calculated for the entire 6DOF pose including the zoom factor.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz, dfx / dts |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz, dfy / dts |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz), translation t = (tx, ty, tz) and zoom factor s.<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 7 column entries
		 * @param jy Second row of the jacobian, with 7 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param zoom The zoom factor of the pose, with range (0, infinity)
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 */
		static void calculatePoseZoomJacobianRodrigues2x7(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar zoom, const Vector3& objectPoint, const bool distortImagePoints);

		/**
		 * Calculates all pose jacobian rows for a given (flexible) pose with (flexible) zoom factor and a set of static 3D object points.
		 * The 7 derivatives are calculated for the entire 6DOF pose including the zoom factor.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz, dfx / dts |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz, dfy / dts |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz), translation t = (tx, ty, tz) and zoom factor s.<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 7 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param zoom The zoom factor of the pose, with range (0, infinity)
		 * @param objectPoints 3D object points to determine the jacobian for
		 * @param numberObjectPoints Number of given object points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 */
		static void calculatePoseZoomJacobianRodrigues2nx7(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar zoom, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints);

		/**
		 * Calculates the two jacobian rows for a given (flexible) 6-DOF object transformation, and a static 6-DOF camera pose, and a static camera, and one static 3D object point.
		 * The 6 derivatives are calculated for the 6-DOF object transformation.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param extrinsicIF Inverted and flipped camera pose, must be valid
		 * @param objectPose The 6-DOF object point transformation (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @see calculateObjectTransformation2nx6().
		 */
		static inline void calculateObjectTransformation2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3& objectPoint);

		/**
		 * Calculates the two jacobian rows for a given (flexible) 6-DOF object transformation, and a static 6-DOF camera pose, and a static camera, and one static 3D object point.
		 * This function uses the pre-calculated 3x3 Jacobian matrix of the object transformation's orientation provided by three separated 3x3 matrices.<br>
		 * The 6 derivatives are calculated for the 6-DOF object transformation.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param extrinsicIF Inverted and flipped camera pose, must be valid
		 * @param objectPose The 6-DOF object point transformation (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @see calculateObjectTransformation2nx6().
		 */
		static void calculateObjectTransformation2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3& objectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz);

		/**
		 * Calculates all pose jacobian rows for a given (flexible) 6-DOF object transformation, and a static 6-DOF camera pose, and a static camera, and several static 3D object points.
		 * The 6 derivatives are calculated for the entire 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * |                               ...                                |
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * |                               ...                                |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with `2 * numberObjectPoints` rows and 6 columns, must be valid
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param extrinsicIF Inverted and flipped camera pose, must be valid
		 * @param objectPose The 6-DOF object point transformation (rotation and translation) to determine the jacobian for
		 * @param objectPoints 3D object points to determine the jacobian for, must be valid
		 * @param numberObjectPoints Number of given object points, with range [1, infinity)
		 * @see calculateObjectTransformation2x6().
		 */
		static void calculateObjectTransformation2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3* objectPoints, const size_t numberObjectPoints);

		/**
		 * Calculates all pose jacobian rows for a given (flexible) 6-DOF object transformation, and a static 6-DOF camera pose, and a static fisheye camera, and several static 3D object points.
		 * The 6 derivatives are calculated for the entire 6DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * |                               ...                                |
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * |                               ...                                |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tx, ty, tz).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with `2 * numberObjectPoints` rows and 6 columns, must be valid
		 * @param fisheyeCamera The fisheye camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world Inverted and flipped camera pose, must be valid
		 * @param world_T_object The 6-DOF object point transformation (rotation and translation) to determine the jacobian for
		 * @param objectPoints 3D object points to determine the jacobian for, must be valid
		 * @param numberObjectPoints Number of given object points, with range [1, infinity)
		 * @see calculateObjectTransformation2x6().
		 */
		static void calculateObjectTransformation2nx6(Scalar* jacobian, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& world_T_object, const Vector3* objectPoints, const size_t numberObjectPoints);

		/**
		 * Calculates the two jacobian rows for a given pose with translation scale ambiguities and static object point.
		 * The 5 derivatives are calculated for the 5DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtu, dfx / dtv |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtu, dfy / dtv |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tu, tv, sqrt(1 - tu*tu - tv*tv)).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 5 column entries
		 * @param jy Second row of the jacobian, with 5 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param rotation Exponential map defining the rotation to determine the jacobian for
		 * @param translation Translation with scale ambiguities to determine the jacobian for, with range tx*tx + ty*ty < 1
		 * @param objectPoint 3D object point to determine the jacobian for
		 */
		static void calculatePoseJacobianRodrigues2x5(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const ExponentialMap& rotation, const Vector2& translation, const Vector3& objectPoint);

		/**
		 * Calculates all jacobian rows for a given pose with translation scale ambiguities and a set of static object points.
		 * The 5 derivatives are calculated for the 5DOF pose.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtu, dfx / dtv |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtu, dfy / dtv |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and translation t = (tu, tv, sqrt(1 - tu*tu - tv*tv)).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 5 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param rotation Exponential map defining the rotation to determine the jacobian for
		 * @param translation Translation with scale ambiguities to determine the jacobian for, with range tx*tx + ty*ty < 1
		 * @param objectPoints 3D object points to determine the jacobian for
		 * @param numberObjectPoints Number of given object points
		 */
		static void calculatePoseJacobianRodrigues2nx5(Scalar* jacobian, const PinholeCamera& pinholeCamera, const ExponentialMap& rotation, const Vector2& translation, const Vector3* objectPoints, const size_t numberObjectPoints);

		/**
		 * Calculates the two jacobian rows for a given pose and dynamic object point.
		 * The derivatives are calculated for the 3D object point only.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dX, dfx / dY, dfx / dZ |
		 * | dfy / dX, dfy / dY, dfy / dZ |
		 * </pre>
		 * @param jx First row of the jacobian, with 3 column entries receiving the point derivatives
		 * @param jy Second row of the jacobian, with 3 column entries receiving the point derivatives
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Pose to determine the jacobian for (inverted and flipped)
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 */
		static void calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint);

		/**
		 * Calculates the two jacobian rows for a given pose and dynamic object point.
		 * The derivatives are calculated for the 3D object point only.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dX, dfx / dY, dfx / dZ |
		 * | dfy / dX, dfy / dY, dfy / dZ |
		 * </pre>
		 * @param jx First row of the jacobian, with 3 column entries receiving the point derivatives
		 * @param jy Second row of the jacobian, with 3 column entries receiving the point derivatives
		 * @param fisheyeCamera Fisheye camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world Flipped and inverted pose of the camera, must be valid
		 * @param worldObjectPoint The 3D object point, defined in world
		 */
		static void calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldObjectPoint);

		/**
		 * Calculates the two jacobian rows for a given pose and dynamic object point.
		 * The derivatives are calculated for the 3D object point only.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dX, dfx / dY, dfx / dZ |
		 * | dfy / dX, dfy / dY, dfy / dZ |
		 * </pre>
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world Flipped and inverted pose of the camera, must be valid
		 * @param worldObjectPoint The 3D object point, defined in world
		 * @param jx The resulting first row of the jacobian, with 3 column entries receiving the point derivatives, must be valid
		 * @param jy The resulting second row of the jacobian, with 3 column entries receiving the point derivatives, must be valid
		 * @tparam T The scalar data type
		 */
		template <typename T>
		static OCEAN_FORCE_INLINE void calculatePointJacobian2x3IF(const AnyCameraT<T>& anyCamera, const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, T* jx, T* jy);

		/**
		 * Calculates the two jacobian rows for a given pose and several dynamic object points.
		 * The derivatives are calculated for the 3D object point only.<br>
		 * The resulting jacobian rows have the following form:
		 * <pre>
		 * | dfx / dX0, dfx / dY0, dfx / dZ0 |
		 * | dfy / dX0, dfy / dY0, dfy / dZ0 |
		 * | dfx / dX1, dfx / dY1, dfx / dZ1 |
		 * | dfx / dX1, dfx / dY1, dfx / dZ1 |
		 * | ............................... |
		 * | ............................... |
		 * </pre>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 3 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Pose to determine the jacobian for (inverted and flipped)
		 * @param objectPoints 3D objects point to determine the jacobian for
		 * @param numberObjectPoints Number of 3D object points
		 * @param distortImagePoints True, to force the distortion of the image points using the distortion parameters of this camera object
		 */
		static void calculatePointJacobian2nx3(Scalar* jacobian, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints);

		/**
		 * Calculates the two jacobian rows for a given camera and image point.
		 * The jacobian is determined for the radial and tangential distortion parameters.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2 |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2 |
		 * </pre>
		 * @param jx First row of the jacobian, with 4 column entries
		 * @param jy Second row of the jacobian, with 4 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param normalizedImagePoint Normalized 2D image point to determine the jacobian for
		 */
		static void calculateCameraDistortionJacobian2x4(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint);

		/**
		 * Calculates the two jacobian rows for a given camera and image point.
		 * The jacobian is determined for the focal length, the principal point and the radial distortion parameters.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 * </pre>
		 * @param jx First row of the jacobian, with 6 column entries
		 * @param jy Second row of the jacobian, with 6 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param normalizedImagePoint Normalized 2D image point to determine the jacobian for
		 */
		static void calculateCameraJacobian2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint);

		/**
		 * Calculates the two jacobian rows for a given camera and image point.
		 * The jacobian is determined for the focal length (same for horizontal and vertical axis), the principal point and the radial and tangential distortion parameters.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dF, dfx / dmx, dfx / dmy |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dF, dfy / dmx, dfy / dmy |
		 * </pre>
		 * @param jx First row of the jacobian, with 7 column entries
		 * @param jy Second row of the jacobian, with 7 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param normalizedImagePoint Normalized 2D image point to determine the jacobian for
		 */
		static void calculateCameraJacobian2x7(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint);

		/**
		 * Calculates the two jacobian rows for a given camera and image point.
		 * The jacobian is determined for the focal length, the principal point and the radial and tangential distortion parameters.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 * </pre>
		 * @param jx First row of the jacobian, with 8 column entries
		 * @param jy Second row of the jacobian, with 8 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param normalizedImagePoint Normalized 2D image point to determine the jacobian for
		 */
		static void calculateCameraJacobian2x8(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint);

		/**
		 * Calculates the two jacobian rows for a given (orientational pose) and a camera and a static object point.
		 * The jacobian is determined for the three rotational angle of the pose, the radial and tangential distortion of the camera and the intrinsic parameters of the camera.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and radial distortion k1, k2, tangential distortion p1, p2, focal parameters Fx, Fy and principal point (mx, my).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jx First row of the jacobian, with 11 column entries
		 * @param jy Second row of the jacobian, with 11 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 */
		static void calculateOrientationCameraJacobianRodrigues2x11(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3& objectPoint);

		/**
		 * Calculates the two jacobian rows for a given (orientational pose) and a camera and a set of static object points.
		 * The jacobian is determined for the three rotational angle of the pose, the radial and tangential distortion of the camera and the intrinsic parameters of the camera.<br>
		 * The resulting jacobian has the following form:<br>
		 * <pre>
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 * </pre>
		 * With transformation function f = (fx, fy), exponential map w = (wx, wy, wz) and radial distortion k1, k2, tangential distortion p1, p2, focal parameters Fx, Fy and principal point (mx, my).<br>
		 * The jacobian calculation uses the Rodrigues rotation formula to determine the rotation derivatives.<br>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 11 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoints The accessor providing the 3D object points to determine the jacobian for
		 */
		static void calculateOrientationCameraJacobianRodrigues2nx11(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints);

		/**
		 * Calculates the entire jacobian matrix for an object point to image point transformation covering a flexible 6-DOF camera pose, the four intrinsic camera parameters and two parameters for radial distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * @param jx First row of the jacobian, with 12 column entries
		 * @param jy Second row of the jacobian, with 12 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_T_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 */
		static void calculateJacobianCameraPoseRodrigues2x12(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint);

		/**
		 * Calculates the entire jacobian matrix for an object point to image point transformation covering a flexible 6-DOF camera pose, the four intrinsic camera parameters and two parameters for radial distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * @param jx First row of the jacobian, with 12 column entries
		 * @param jy Second row of the jacobian, with 12 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_T_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param flippedCamera_P_world Inverted and flipped pose identical to 'flippedCamera_T_world'
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @param objectPoint 3D object point to determine the jacobian for
		 */
		static void calculateJacobianCameraPoseRodrigues2x12(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz);

		/**
		 * Calculates the entire jacobian matrix for an object point to image point transformation covering a flexible 6-DOF camera pose, the four intrinsic camera parameters and four parameters for radial and tangential distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * @param jx First row of the jacobian, with 14 column entries
		 * @param jy Second row of the jacobian, with 14 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_T_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoint 3D object point to determine the jacobian for
		 */
		static void calculateJacobianCameraPoseRodrigues2x14(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint);

		/**
		 * Calculates the entire jacobian matrix for an object point to image point transformation covering a flexible 6-DOF camera pose, the four intrinsic camera parameters and four parameters for radial and tangential distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * @param jx First row of the jacobian, with 14 column entries
		 * @param jy Second row of the jacobian, with 14 column entries
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_T_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param flippedCamera_P_world Inverted and flipped pose identical to 'flippedCamera_T_world'
		 * @param objectPoint 3D object point to determine the jacobian for
		 * @param dwx Rotation matrix derived to wx, as determined by calculateRotationRodriguesDerivative()
		 * @param dwy Rotation matrix derived to wy, as determined by calculateRotationRodriguesDerivative()
		 * @param dwz Rotation matrix derived to wz, as determined by calculateRotationRodriguesDerivative()
		 * @see calculateRotationRodriguesDerivative().
		 */
		static void calculateJacobianCameraPoseRodrigues2x14(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz);

		/**
		 * Calculates the two jacobian rows for a given (6-DOF pose) and a camera and a set of static object points.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 * </pre>
		 * @param jacobian First element in the first row of the entire row aligned jacobian matrix, with 2 * numberObjectPoints rows and 14 columns
		 * @param pinholeCamera The pinhole camera to determine the jacobian values for
		 * @param flippedCamera_P_world Inverted and flipped pose (rotation and translation) to determine the jacobian for
		 * @param objectPoints The accessor providing the 3D object points to determine the jacobian for
		 */
		static void calculateJacobianCameraPoseRodrigues2nx14(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints);

		/**
		 * Determines the 2x8 Jacobian of a homography function that transforms a 2D coordinate (interpreted as a 3D vector with homogeneous extension) to a 2D coordinate (the de-homogenization is included).
		 * This Jacobian can be used e.g., for additive image alignment.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dh0, dfx / dh1, dfx / dh2, dfx / dh3, dfx / dh4, dfx / dh5, dfx / dh6, dfx / dh7 |
		 * | dfy / dh0, dfy / dh1, dfy / dh2, dfy / dh3, dfy / dh4, dfy / dh5, dfy / dh6, dfy / dh7 |
		 * </pre>
		 * @param jx First row of the jacobian, with 8 column entries
		 * @param jy Second row of the jacobian, with 8 column entries
		 * @param x The horizontal coordinate to be used
		 * @param y The vertical coordinate to be used
		 * @param homography The homography for which the Jacobian will be determined, must have 1 in the lower right corner
		 * @see calculateHomographyJacobian2x9(), calculateIdentityHomographyJacobian2x8().
		 */
		static void calculateHomographyJacobian2x8(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& homography);

		/**
		 * Determines the 2x9 Jacobian of a homography function that transforms a 2D coordinate (interpreted as a 3D vector with homogeneous extension) to a 2D coordinate (the de-homogenization is included).
		 * This Jacobian can be used e.g., for additive image alignment.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dh0, dfx / dh1, dfx / dh2, dfx / dh3, dfx / dh4, dfx / dh5, dfx / dh6, dfx / dh7, dfx / dh8 |
		 * | dfy / dh0, dfy / dh1, dfy / dh2, dfy / dh3, dfy / dh4, dfy / dh5, dfy / dh6, dfy / dh7, dfx / dh8 |
		 * </pre>
		 * @param jx First row of the jacobian, with 9 column entries
		 * @param jy Second row of the jacobian, with 9 column entries
		 * @param x The horizontal coordinate to be used
		 * @param y The vertical coordinate to be used
		 * @param homography The homography for which the Jacobian will be determined, must have 1 in the lower right corner
		 * @see calculateHomographyJacobian2x8(), calculateIdentityHomographyJacobian2x9().
		 */
		static void calculateHomographyJacobian2x9(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& homography);

		/**
		 * Determines the 2x8 Jacobian of the identity homography function that transforms a 2D coordinate (interpreted as a 3D vector with homogeneous extension) to a 2D coordinate (the de-homogenization is included).
		 * This Jacobian can be used e.g., for inverse compositional image alignment..<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dh0, dfx / dh1, dfx / dh2, dfx / dh3, dfx / dh4, dfx / dh5, dfx / dh6, dfx / dh7 |
		 * | dfy / dh0, dfy / dh1, dfy / dh2, dfy / dh3, dfy / dh4, dfy / dh5, dfy / dh6, dfy / dh7 |
		 * </pre>
		 * @param jx First row of the jacobian, with 8 column entries
		 * @param jy Second row of the jacobian, with 8 column entries
		 * @param x The horizontal coordinate to be used
		 * @param y The vertical coordinate to be used
		 * @see calculateIdentityHomographyJacobian2x9(), calculateHomographyJacobian2x8().
		 */
		static void calculateIdentityHomographyJacobian2x8(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y);

		/**
		 * Determines the 2x9 Jacobian of the identity homography function that transforms a 2D coordinate (interpreted as a 3D vector with homogeneous extension) to a 2D coordinate (the de-homogenization is included).
		 * This Jacobian can be used e.g., for inverse compositional image alignment.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dh0, dfx / dh1, dfx / dh2, dfx / dh3, dfx / dh4, dfx / dh5, dfx / dh6, dfx / dh7, dfx / dh8 |
		 * | dfy / dh0, dfy / dh1, dfy / dh2, dfy / dh3, dfy / dh4, dfy / dh5, dfy / dh6, dfy / dh7, dfx / dh8 |
		 * </pre>
		 * @param jx First row of the jacobian, with 9 column entries
		 * @param jy Second row of the jacobian, with 9 column entries
		 * @param x The horizontal coordinate to be used
		 * @param y The vertical coordinate to be used
		 * @see calculateIdentityHomographyJacobian2x8(), calculateHomographyJacobian2x9().
		 */
		static void calculateIdentityHomographyJacobian2x9(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y);

		/**
		 * Determines the 2x4 Jacobian of a similarity transformation that transforms a 2D coordinate (interpreted as a 3D vector with homogeneous extension) to a 2D coordinate (the de-homogenization is included).
		 * This Jacobian can be used e.g., for additive image alignment.<br>
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / ds0, dfx / ds1, dfx / ds2, dfx / ds3 |
		 * | dfy / ds0, dfy / ds1, dfy / ds2, dfy / ds3 |
		 * </pre>
		 * @param jx First row of the jacobian, with 4 column entries
		 * @param jy Second row of the jacobian, with 4 column entries
		 * @param x The horizontal coordinate to be used
		 * @param y The vertical coordinate to be used
		 * @param similarity The similarity for which the Jacobian will be determined, must have 1 in the lower right corner
		 */
		static inline void calculateSimilarityJacobian2x4(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& similarity);

		/**
		 * Determines the 2x2 Jacobian of distorting a normalized image point in a fisheye camera with radial and tangential distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dx, dfx / dy |
		 * | dfy / dx, dfy / dy |
		 * </pre>
		 * @param jx First row of the jacobian, with 2 column entries, must be valid
		 * @param jy Second row of the jacobian, with 2 column entries, must be valid
		 * @param x The horizontal coordinate of the normalized image point to be distorted
		 * @param y The vertical coordinate of the normalized image point to be distorted
		 * @param radialDistortion The six radial distortion parameters, must be valid
		 * @param tangentialDistortion The two radial distortion parameters, must be valid
		 * @tparam T The data type of a scalar, 'float' or 'double'
		 */
		template <typename T>
		static void calculateFisheyeDistortNormalized2x2(T* jx, T* jy, const T x, const T y, const T* radialDistortion, const T* tangentialDistortion);
};

inline void Jacobian::calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint)
{
	SquareMatrix3 dwx, dwy, dwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), dwx, dwy, dwz);

	calculatePoseJacobianRodrigues2x6(jx, jy, pinholeCamera, flippedCamera_P_world.transformation(), objectPoint, distortImagePoint, dwx, dwy, dwz);
}

inline void Jacobian::calculateObjectTransformation2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3& objectPoint)
{
	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(objectPose.rx(), objectPose.ry(), objectPose.rz())), Rwx, Rwy, Rwz);

	calculateObjectTransformation2x6(jx, jy, pinholeCamera, extrinsicIF, objectPose, objectPoint, Rwx, Rwy, Rwz);
}

inline void Geometry::Jacobian::calculateSimilarityJacobian2x4(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& similarity)
{
	ocean_assert(jx != nullptr && jy != nullptr);

	ocean_assert_and_suppress_unused(Numeric::isEqual(similarity(2, 0), 0), similarity);
	ocean_assert(Numeric::isEqual(similarity(2, 1), 0));
	ocean_assert(Numeric::isEqual(similarity(2, 2), 1));

	// Similarity:
	// |   s0   -s1    s2   |
	// |   s1    s0    s3   |
	// |    0     0     1   |

	// sx(x, y) = s0*x - s1*y + s2
	// sy(x, y) = s1*x + s0*y + s3

	// Jacobian:  x  -y  1  0
	//            y   x  0  1

	jx[0] = x;
	jx[1] = -y;
	jx[2] = 1;
	jx[3] = 0;

	jy[0] = y;
	jy[1] = x;
	jy[2] = 0;
	jy[3] = 1;
}

template <typename T, typename TRotation>
OCEAN_FORCE_INLINE void Jacobian::calculateOrientationalJacobianRodrigues2x3IF(const AnyCameraT<T>& anyCamera, const TRotation& flippedCamera_R_translation, const VectorT3<T>& translation_T_world, const VectorT3<T>& worldObjectPoint, const SquareMatrixT3<T>& Rwx, const SquareMatrixT3<T>& Rwy, const SquareMatrixT3<T>& Rwz, T* jx, T* jy)
{
	static_assert(std::is_same<TRotation, QuaternionT<T>>::value || std::is_same<TRotation, SquareMatrixT3<T>>::value, "Invalid rotation type!");

	ocean_assert(anyCamera.isValid());
	ocean_assert(jx != nullptr && jy != nullptr);

#ifdef OCEAN_DEBUG
	if constexpr (std::is_same<TRotation, QuaternionT<T>>::value)
	{
		ocean_assert(flippedCamera_R_translation.isValid());
	}
	else
	{
		ocean_assert(flippedCamera_R_translation.isOrthonormal());
	}
#endif

	/**
	 * f = fC(fR(fT(X))
	 *
	 * with fC camera function, fR object point rotation function, fT object point translation function
	 *
	 * fR(fT(X)) = R(X + t)
	 */

	const VectorT3<T> translatedWorldObjectPoint = translation_T_world + worldObjectPoint;
	const VectorT3<T> flippedCameraObjectPoint = flippedCamera_R_translation * translatedWorldObjectPoint;

	T jxPoint[3];
	T jyPoint[3];

	// let's determine the left 2x3 sub-matrix first
	anyCamera.pointJacobian2x3IF(flippedCameraObjectPoint, jxPoint, jyPoint);

	const VectorT3<T> dwx(Rwx * translatedWorldObjectPoint);
	const VectorT3<T> dwy(Rwy * translatedWorldObjectPoint);
	const VectorT3<T> dwz(Rwz * translatedWorldObjectPoint);

	// now, we apply the chain rule to determine the 2x3 Jacobian
	jx[0] = jxPoint[0] * dwx[0] + jxPoint[1] * dwx[1] + jxPoint[2] * dwx[2];
	jx[1] = jxPoint[0] * dwy[0] + jxPoint[1] * dwy[1] + jxPoint[2] * dwy[2];
	jx[2] = jxPoint[0] * dwz[0] + jxPoint[1] * dwz[1] + jxPoint[2] * dwz[2];

	jy[0] = jyPoint[0] * dwx[0] + jyPoint[1] * dwx[1] + jyPoint[2] * dwx[2];
	jy[1] = jyPoint[0] * dwy[0] + jyPoint[1] * dwy[1] + jyPoint[2] * dwy[2];
	jy[2] = jyPoint[0] * dwz[0] + jyPoint[1] * dwz[1] + jyPoint[2] * dwz[2];
}

template <typename T>
OCEAN_FORCE_INLINE void Jacobian::calculatePoseJacobianRodrigues2x6IF(const AnyCameraT<T>& anyCamera, const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, const SquareMatrixT3<T>& Rwx, const SquareMatrixT3<T>& Rwy, const SquareMatrixT3<T>& Rwz, T* jx, T* jy)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(jx != nullptr && jy != nullptr);

	anyCamera.pointJacobian2x3IF(flippedCamera_T_world * worldObjectPoint, jx + 3, jy + 3);

	const VectorT3<T> dwx(Rwx * worldObjectPoint);
	const VectorT3<T> dwy(Rwy * worldObjectPoint);
	const VectorT3<T> dwz(Rwz * worldObjectPoint);

	// now, we apply the chain rule to determine the left 2x3 sub-matrix
	jx[0] = jx[3] * dwx[0] + jx[4] * dwx[1] + jx[5] * dwx[2];
	jx[1] = jx[3] * dwy[0] + jx[4] * dwy[1] + jx[5] * dwy[2];
	jx[2] = jx[3] * dwz[0] + jx[4] * dwz[1] + jx[5] * dwz[2];

	jy[0] = jy[3] * dwx[0] + jy[4] * dwx[1] + jy[5] * dwx[2];
	jy[1] = jy[3] * dwy[0] + jy[4] * dwy[1] + jy[5] * dwy[2];
	jy[2] = jy[3] * dwz[0] + jy[4] * dwz[1] + jy[5] * dwz[2];
}

template <typename T>
OCEAN_FORCE_INLINE void Jacobian::calculatePointJacobian2x3IF(const AnyCameraT<T>& anyCamera, const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, T* jx, T* jy)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(jx != nullptr && jy != nullptr);

	// | Fx   0  |   | df_distx_u   df_distx_v |   | 1/W    0   -U/W^2 |
	// | 0    Fy | * | df_disty_u   df_disty_v | * |  0    1/W  -V/W^2 | * R

	T pointJacobian2x3[6];
	anyCamera.pointJacobian2x3IF(flippedCamera_T_world * worldObjectPoint, pointJacobian2x3 + 0, pointJacobian2x3 + 3);

	jx[0] = pointJacobian2x3[0] * flippedCamera_T_world[0] + pointJacobian2x3[1] * flippedCamera_T_world[1] + pointJacobian2x3[2] * flippedCamera_T_world[2];
	jx[1] = pointJacobian2x3[0] * flippedCamera_T_world[4] + pointJacobian2x3[1] * flippedCamera_T_world[5] + pointJacobian2x3[2] * flippedCamera_T_world[6];
	jx[2] = pointJacobian2x3[0] * flippedCamera_T_world[8] + pointJacobian2x3[1] * flippedCamera_T_world[9] + pointJacobian2x3[2] * flippedCamera_T_world[10];

	jy[0] = pointJacobian2x3[3] * flippedCamera_T_world[0] + pointJacobian2x3[4] * flippedCamera_T_world[1] + pointJacobian2x3[5] * flippedCamera_T_world[2];
	jy[1] = pointJacobian2x3[3] * flippedCamera_T_world[4] + pointJacobian2x3[4] * flippedCamera_T_world[5] + pointJacobian2x3[5] * flippedCamera_T_world[6];
	jy[2] = pointJacobian2x3[3] * flippedCamera_T_world[8] + pointJacobian2x3[4] * flippedCamera_T_world[9] + pointJacobian2x3[5] * flippedCamera_T_world[10];
}

template <typename T>
void Jacobian::calculateFisheyeDistortNormalized2x2(T* jx, T* jy, const T x, const T y, const T* radialDistortion, const T* tangentialDistortion)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(radialDistortion != nullptr && tangentialDistortion != nullptr);

	const T& k3 = radialDistortion[0];
	const T& k5 = radialDistortion[1];
	const T& k7 = radialDistortion[2];
	const T& k9 = radialDistortion[3];
	const T& k11 = radialDistortion[4];
	const T& k13 = radialDistortion[5];

	const T& p1 = tangentialDistortion[0];
	const T& p2 = tangentialDistortion[1];

	const T x2 = x * x;
	const T y2 = y * y;

	const T xy2 = x2 + y2;

	const T r = NumericT<T>::sqrt(xy2);
	const T r3 = r * r * r;

	const T t = NumericT<T>::atan(r);
	const T t2 = t * t;
	const T t3 = t2 * t;
	const T t4 = t3 * t;
	const T t5 = t4 * t;
	const T t6 = t5 * t;
	const T t7 = t6 * t;
	const T t8 = t7 * t;
	const T t9 = t8 * t;
	const T t10 = t9 * t;
	const T t11 = t10 * t;
	const T t12 = t11 * t;
	const T t13 = t12 * t;

	const T term0 = k13 * t13 + k11 * t11 + k9 * t9 + k7 * t7 + k5 * t5 + k3 * t3 + t;
	const T term1 = 13 * k13 * t12 + 11 * k11 * t10 + 9 * k9 * t8 + 7 * k7 * t6 + 5 * k5 * t4 + 3 * k3 * t2 + 1;

	const T term2 = (xy2 + 1) * term0;
	const T term3 = r3 * (xy2 + 1);
	const T invTerm3 = T(1) / term3;

	const T xDistortion_dx = (xy2 * term2 - x2 * term2 + x2 * r * term1) * invTerm3;
	const T xDistortion_dy = (x * term1 * y) / (xy2 * (xy2 + 1)) - (x * y * term0) / r3;

	//const T yDistortion_dx = (y * term1 * x) / (xy2 * (xy2 + 1)) - (y * x * term0) / r3; == xDistortion_dy
	const T& yDistortion_dx = xDistortion_dy;
	const T yDistortion_dy = (xy2 * term2 - y2 * term2 + y2 * r * term1) * invTerm3;

	const T radialDistortionFactor = term0 / r;

	const T rx = x * radialDistortionFactor;
	const T ry = y * radialDistortionFactor;

	const T xTangential_dx = 6 * p1 * rx + 2 * p2 * ry + 1;
	const T xTangential_dy = 2 * p1 * ry + 2 * p2 * rx;

	// const T yTangential_dx = 2 * p2 * rx + 2 * p1 * ry; // == yTangential_dx
	const T& yTangential_dx = xTangential_dy;
	const T yTangential_dy = 6 * p2 * ry + 2 * p1 * rx + 1;

	// chain rule
	// | xTangential_dx  xTangential_dy |   | xDistortion_dx  xDistortion_dy |
	// | yTangential_dx  yTangential_dy | * | yDistortion_dx  yDistortion_dy |

	jx[0] = xTangential_dx * xDistortion_dx + xTangential_dy * yDistortion_dx;
	jx[1] = xTangential_dx * xDistortion_dy + xTangential_dy * yDistortion_dy;

	jy[0] = yTangential_dx * xDistortion_dx + yTangential_dy * yDistortion_dx;
	jy[1] = yTangential_dx * xDistortion_dy + yTangential_dy * yDistortion_dy;
}

}

}

#endif // META_OCEAN_GEOMETRY_JACOBIAN_H
