/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_GRAVITY_CONSTRAINTS_H
#define META_OCEAN_GEOMETRY_GRAVITY_CONSTRAINTS_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/StackHeapVector.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

// Forward declaration.
template <typename T>
class GravityConstraintsT;

/**
 * Definition of the GravityConstraints object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see GravityConstraintsT
 * @ingroup math
 */
using GravityConstraints = GravityConstraintsT<Scalar>;

/**
 * Definition of the GravityConstraints object, using double floating point precision.
 * @see GravityConstraintsT
 * @ingroup math
 */
using GravityConstraintsD = GravityConstraintsT<double>;

/**
 * Definition of the GravityConstraints object, using single floating point precision.
 * @see GravityConstraintsT
 * @ingroup math
 */
using GravityConstraintsF = GravityConstraintsT<float>;

/**
 * This class implements a container allowing to define gravity constraints during e.g., camera poses estimation or optimization.
 * The constraint is defined by a gravity vector defined in the world coordinate system (e.g., VectorT3<T>(0, -1, 0) for a gravity vector pointing towards the earth, in case the y-axis is pointing towards the sky).<br>
 * Further, the constraints define a gravity vector for each camera, defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.<br>
 * The camera gravity vector(s) may be provided by a IMU sensor.<br>
 * All gravity vectors must be normalized (must be unit vectors).
 * @tparam T The data type of a scalar, either 'float' or 'double'
 * @ingroup geometry
 */
template <typename T>
class GravityConstraintsT
{
	protected:

		/**
		 * Definition of a vector holding gravity vectors.
		 */
		using Gravities = StackHeapVector<VectorT3<T>, 2>;

	public:

		/**
		 * Creates a default constraint object with invalid parameters.
		 */
		GravityConstraintsT() = default;

		/**
		 * Creates a new gravity constraints object for one camera for which the gravity vector is known (in the camera coordinate system).
		 * @param gravityInCamera The camera gravity vector (which is known for a camera pose), defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.
		 * @param worldGravityInWorld The world gravity vector in the world coordinate system, mainly defining how the world coordinate system is oriented/aligned wrt gravity
		 * @param weightFactor The weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a normal/default gravity correction, larger values to apply a stronger gravity correction, with range [0, infinity)
		 * @param maximalAngle The maximal angle between world and camera gravity vector (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm, in radian, with range [0, PI/2).
		 */
		explicit inline GravityConstraintsT(const VectorT3<T>& cameraGravityInCamera, const VectorT3<T>& worldGravityInWorld = VectorT3<T>(0, -1, 0), const T weightFactor = T(1), const T maximalAngle = NumericT<T>::deg2rad(5));

		/**
		 * Creates a new gravity constraints object for several cameras for which the gravity vectors are known (in the camera coordinate system).
		 * @param gravityInCameras The camera gravity vectors (which are known for several camera poses), defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.
		 * @param worldGravityInWorld The world gravity vector in the world coordinate system, mainly defining how the world coordinate system is oriented/aligned wrt gravity
		 * @param weightFactor The weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a normal/default gravity correction, larger values to apply a stronger gravity correction, with range [0, infinity)
		 * @param maximalAngle The maximal angle between world and camera gravity vectors (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm, in radian, with range [0, PI/2).
		 */
		explicit inline GravityConstraintsT(VectorsT3<T>&& cameraGravityInCameras, const VectorT3<T>& worldGravityInWorld = VectorT3<T>(0, -1, 0), const T weightFactor = T(1), const T maximalAngle = NumericT<T>::deg2rad(5));

		/**
		 * Creates a new gravity constraints object for one camera for which the precise camera pose is known.
		 * The camera gravity vector is determined from the precise camera pose.
		 * @param world_T_camera The transformation transforming camera to world, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param worldGravityInWorld The world gravity vector in the world coordinate system, mainly defining how the world coordinate system is oriented/aligned wrt gravity
		 * @param weightFactor The weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a normal/default gravity correction, larger values to apply a stronger gravity correction, with range [0, infinity)
		 * @param maximalAngle The maximal angle between world and camera gravity vector (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm, in radian, with range [0, PI/2).
		 */
		explicit inline GravityConstraintsT(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldGravityInWorld = VectorT3<T>(0, -1, 0), const T weightFactor = T(1), const T maximalAngle = NumericT<T>::deg2rad(5));

		/**
		 * Creates a new gravity constraints object for several cameras for which the precise camera poses are known.
		 * The camera gravity vectors are determined from the precise camera poses.
		 * @param world_T_cameras The transformations transforming camera to world, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param worldGravityInWorld The world gravity vector in the world coordinate system, mainly defining how the world coordinate system is oriented/aligned wrt gravity
		 * @param weightFactor The weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a normal/default gravity correction, larger values to apply a stronger gravity correction, with range [0, infinity)
		 * @param maximalAngle The maximal angle between world and camera gravity vectors (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm, in radian, with range [0, PI/2).
		 */
		explicit inline GravityConstraintsT(const HomogenousMatricesT4<T>& world_T_cameras, const VectorT3<T>& worldGravityInWorld = VectorT3<T>(0, -1, 0), const T weightFactor = T(1), const T maximalAngle = NumericT<T>::deg2rad(5));

		/**
		 * Creates a new gravity constraints object for a single camera from a given constraints object with several cameras.
		 * @param constraints The constraints object with several cameras, must be valid
		 * @param cameraIndex The index of the camera for which the gravity constraint will be extracted, with range [0, constraints.numberCameras() - 1]
		 */
		inline GravityConstraintsT(const GravityConstraintsT<T>& constraints, const size_t cameraIndex);

		/**
		 * Creates a new gravity constraints object for a single camera from a given constraints object with several cameras.
		 * The constructor will create an invalid object if the given constraints object is nullptr.
		 * @param constraints The constraints object with several cameras, nullptr to create an invalid constraint object, must be valid if not nullptr
		 * @param cameraIndex The index of the camera for which the gravity constraint will be extracted, with range [0, constraints.numberCameras() - 1]
		 */
		inline GravityConstraintsT(const GravityConstraintsT<T>* constraints, const size_t cameraIndex);

		/**
		 * Returns the camera gravity vector (which is known for a camera pose), defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.
		 * @param cameraIndex The index of the camera for which the gravity vector is requested, with range [0, numberCameras() - 1]
		 * @return The known gravity vector for a specified camera, defined in the camera coordinate system
		 */
		inline const VectorT3<T>& cameraGravityInCamera(const size_t cameraIndex = 0) const;

		/**
		 * Returns the camera gravity vector (which is known for a camera pose), defined in the flipped camera coordinate system, with default flipped camera pointing towards the positive z-axis and y-axis pointing downwards.
		 * @param cameraIndex The index of the camera for which the gravity vector is requested, with range [0, numberCameras() - 1]
		 * @return The known gravity vector for a specified camera, defined in the flipped camera coordinate system
		 */
		inline VectorT3<T> cameraGravityInFlippedCamera(const size_t cameraIndex = 0) const;

		/**
		 * Returns the camera gravity vector (which is known for a camera pose), defined in the world coordinate system.
		 * @param world_Q_camera The rotation rotating camera to world, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the gravity vector is requested, with range [0, numberCameras() - 1]
		 * @return The known gravity vector for a specified camera, defined in the world coordinate system
		 */
		inline VectorT3<T> cameraGravityInWorld(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the camera gravity vector (which is known for a camera pose), defined in the world coordinate system.
		 * @param world_T_camera The transformation transforming camera to world, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the gravity vector is requested, with range [0, numberCameras() - 1]
		 * @return The known gravity vector for a specified camera, defined in the world coordinate system
		 */
		inline VectorT3<T> cameraGravityInWorld(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the camera gravity vectors (which are known for one or several camera poses) defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.
		 * @return The known gravity vectors for one or several cameras
		 */
		inline const Gravities& cameraGravitiesInCamera() const;

		/**
		 * Returns the world gravity vector defined in the world coordinate system.
		 * @return The world's gravity vector in world
		 */
		inline const VectorT3<T>& worldGravityInWorld() const;

		/**
		 * Returns the world gravity vector defined in the flipped camera coordinate system, with default flipped camera pointing towards the positive z-axis and y-axis pointing downwards.
		 * @param flippedCamera_Q_world The rotation rotating world to flipped camera, must be valid
		 * @return The world's gravity vector in flipped camera
		 */
		inline VectorT3<T> worldGravityInFlippedCameraIF(const QuaternionT<T>& flippedCamera_Q_world) const;

		/**
		 * Returns the world gravity vector defined in the flipped camera coordinate system, with default flipped camera pointing towards the positive z-axis and y-axis pointing downwards.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, must be valid
		 * @return The world's gravity vector in flipped camera
		 */
		inline VectorT3<T> worldGravityInFlippedCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world) const;

		/**
		 * Returns one possible rotation between camera and world.
		 * The resulting rotation contains a remaining degree of freedom as the rotation is only based on the corresponding gravity vectors.<br>
		 * Any rotation around the gravity vector would be a valid rotation as well.
		 * @param cameraIndex The index of the camera for which the rotation will be returned, with range [0, numberCameras() - 1]
		 * @return The requested rotation, with default camera pointing towards the negative z-space and y-axis pointing upwards
		 */
		inline QuaternionT<T> world_R_camera(const size_t cameraIndex) const;

		/**
		 * Returns one possible rotation between world and camera.
		 * The resulting rotation contains a remaining degree of freedom as the rotation is only based on the corresponding gravity vectors.<br>
		 * Any rotation around the gravity vector would be a valid rotation as well.
		 * @param cameraIndex The index of the camera for which the rotation will be returned, with range [0, numberCameras() - 1]
		 * @return The requested rotation, with default camera pointing towards the negative z-space and y-axis pointing upwards
		 */
		inline QuaternionT<T> camera_R_world(const size_t cameraIndex) const;

		/**
		 * Returns one possible rotation between world and the flipped camera.
		 * The resulting rotation contains a remaining degree of freedom as the rotation is only based on the corresponding gravity vectors.<br>
		 * Any rotation around the gravity vector would be a valid rotation as well.
		 * @param cameraIndex The index of the camera for which the rotation will be returned, with range [0, numberCameras() - 1]
		 * @return The requested rotation, with default flipped camera pointing towards the positive z-axis and y-axis pointing downwards
		 */
		inline QuaternionT<T> flippedCamera_R_world(const size_t cameraIndex) const;

		/**
		 * Returns the weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a normal/default gravity correction, larger values to apply a stronger gravity correction.
		 * @return The object's weight factor, with range [0, infinity)
		 */
		inline T weightFactor() const;

		/**
		 * Returns the maximal angle between world and camera gravity vectors (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm.
		 * @return The object's maximal angle, in radian, with range [0, PI/2)
		 */
		inline T maximalAngle() const;

		/**
		 * Returns the cosine value of maximalAngle().
		 * @return The object's minimal angle cosine, with range [0, 1]
		 */
		inline T minimalAngleCos() const;

		/**
		 * Returns the number of cameras for which gravity constraints are defined.
		 * @return The number of cameras, with range [1, infinity)
		 */
		inline size_t numberCameras() const;

		/**
		 * Returns the cosine between the world gravity vector and the camera gravity vector (after converting into the same coordinate system).
		 * @param world_Q_camera The rotation rotating camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the cosine will be returned, with range [0, numberCameras() - 1]
		 * @return The cosine of the angle between the world gravity vector and the camera gravity vector, with range [-1, 1]
		 */
		inline T alignmentCosine(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the cosine between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param world_T_camera The transformation transforming camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the cosine will be returned, with range [0, numberCameras() - 1]
		 * @return The cosine of the angle between the world gravity vector and the camera gravity vector, with range [-1, 1]
		 */
		inline T alignmentCosine(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the cosine between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param flippedCamera_Q_world The rotation rotating world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the cosine will be returned, with range [0, numberCameras() - 1]
		 * @return The cosine of the angle between the world gravity vector and the camera gravity vector, with range [-1, 1]
		 */
		inline T alignmentCosineIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns the cosine between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the cosine will be returned, with range [0, numberCameras() - 1]
		 * @return The cosine of the angle between the world gravity vector and the camera gravity vector, with range [-1, 1]
		 */
		inline T alignmentCosineIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns the angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param world_Q_camera The rotation rotating camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the angle will be returned, with range [0, numberCameras() - 1]
		 * @return The angle between the world gravity vector and the camera gravity vector, in radian, with range [0, PI]
		 */
		inline T alignmentAngle(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param world_T_camera The transformation transforming camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the angle will be returned, with range [0, numberCameras() - 1]
		 * @return The angle between the world gravity vector and the camera gravity vector, in radian, with range [0, PI]
		 */
		inline T alignmentAngle(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns the angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param flippedCamera_Q_world The rotation rotating world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the angle will be returned, with range [0, numberCameras() - 1]
		 * @return The angle between the world gravity vector and the camera gravity vector, in radian, with range [0, PI]
		 */
		inline T alignmentAngleIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns the angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the angle will be returned, with range [0, numberCameras() - 1]
		 * @return The angle between the world gravity vector and the camera gravity vector, in radian, with range [0, PI]
		 */
		inline T alignmentAngleIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns whether a camera pose is aligned with the gravity constraints using the specified angle threshold.
		 * @param world_Q_camera The rotation rotating camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @return True, if so
		 */
		bool isCameraAlignedWithGravity(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns whether a camera pose is aligned with the gravity constraints using the specified angle threshold.
		 * @param world_T_camera The transformation transforming camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @return True, if so
		 */
		inline bool isCameraAlignedWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex = 0) const;

		/**
		 * Returns whether a provided flipped and inverted camera pose is aligned with the gravity constraints using the specified angle threshold.
		 * @param flippedCamera_Q_world The rotation rotating world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		bool isCameraAlignedWithGravityIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns whether a provided flipped and inverted camera pose is aligned with the gravity constraints using the specified angle threshold.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		inline bool isCameraAlignedWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns whether a camera pose is aligned with the gravity constraints using a custom angle threshold.
		 * @param world_Q_camera The rotation rotating camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		bool isCameraAlignedWithGravity(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex, const T maxAngle) const;

		/**
		 * Returns whether a camera pose is aligned with the gravity constraints using a custom angle threshold.
		 * @param world_T_camera The transformation transforming camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		inline bool isCameraAlignedWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex, const T maxAngle) const;

		/**
		 * Returns whether a provided flipped and inverted camera pose is aligned with the gravity constraints using a custom angle threshold.
		 * @param flippedCamera_Q_world The rotation rotating world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		bool isCameraAlignedWithGravityIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex, const T maxAngle) const;

		/**
		 * Returns whether a provided flipped and inverted camera pose is aligned with the gravity constraints using a custom angle threshold.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment check will be performed, with range [0, numberCameras() - 1]
		 * @param maxAngle The maximal angle between the world gravity vector and the camera gravity vector (after converting into the same coordinate system), in radian, with range [0, PI/2)
		 * @return True, if so
		 */
		inline bool isCameraAlignedWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex, const T maxAngle) const;

		/**
		 * Rotates a camera pose so that the camera is aligned with the gravity constraints.
		 * @param world_T_camera The transformation transforming camera to world, with a default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment will be performed, with range [0, numberCameras() - 1]
		 * @return The camera pose aligned with the gravity constraints (world_T_alignedCamera)
		 */
		HomogenousMatrixT4<T> alignCameraWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex = 0) const;

		/**
		 * Rotates a inverted and flipped camera pose so that the flipped camera is aligned with the gravity constraints.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with a default flipped camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param cameraIndex The index of the camera for which the alignment will be performed, with range [0, numberCameras() - 1]
		 * @return The flipped inverted and flipped camera pose aligned with the gravity constraints (alignedFlippedCamera_T_world)
		 */
		HomogenousMatrixT4<T> alignCameraWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex = 0) const;

		/**
		 * Returns the pointer to this gravity constraints object if it is valid.
		 * @param pointerIfValid True, to return a pointer to this object if this object is valid; False, to always return a null pointer
		 * @return The pointer to this object if the object holds valid constraints and if 'pointerIfValid == true', nullptr if this object does not hold valid constraints or if 'pointerIfValid == false'
		 */
		inline const GravityConstraintsT<T>* conditionalPointer(const bool pointerIfValid = true) const;

		/**
		 * Returns whether this constraint object holds valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Helper function returning the camera gravity vector in the camera coordinate system based on a given camera pose and the world gravity vector.
		 * The resulting camera gravity vector is obtained by converting the world gravity vector into the camera coordinate system.
		 * @param camera_Q_world The rotation rotating world to camera, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param worldGravityInWorld The world gravity vector defined in the world coordinate system, must be a unit vector
		 * @return The camera gravity vector in the camera coordinate system
		 */
		static VectorT3<T> cameraGravityInCamera(const QuaternionT<T>& camera_Q_world, const VectorT3<T>& worldGravityInWorld);

		/**
		 * Helper function returning the camera gravity vector in the camera coordinate system based on a given camera pose and the world gravity vector.
		 * The resulting camera gravity vector is obtained by converting the world gravity vector into the camera coordinate system.
		 * @param camera_T_world The transformation transforming world to camera, with default camera pointing towards the negative z-space and y-axis pointing upwards, must be valid
		 * @param worldGravityInWorld The world gravity vector defined in the world coordinate system, must be a unit vector
		 * @return The camera gravity vector in the camera coordinate system
		 */
		static VectorT3<T> cameraGravityInCamera(const HomogenousMatrixT4<T>& camera_T_world, const VectorT3<T>& worldGravityInWorld);

		/**
		 * Helper function returning the camera gravity vector in the camera coordinate system based on a given camera pose and the world gravity vector.
		 * The resulting camera gravity vector is obtained by converting the world gravity vector into the camera coordinate system.,br>
		 * This function return the same gravity vector as cameraGravityInCamera(), but uses the flipped camera pose as input parameter.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with default camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param worldGravityInWorld The world gravity vector defined in the world coordinate system, must be a unit vector
		 * @return The camera gravity vector in the standard camera coordinate system (not in the flipped camera coordinate system)
		 * @see cameraGravityInCamera(), cameraGravityInFlippedCameraIF().
		 */
		static VectorT3<T> cameraGravityInCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldGravityInWorld);

		/**
		 * Helper function returning the camera gravity vector in the flipped camera coordinate system based on a given camera pose and the world gravity vector.
		 * The resulting camera gravity vector is obtained by converting the world gravity vector into the flipped camera coordinate system.
		 * @param flippedCamera_T_world The transformation transforming world to flipped camera, with default camera pointing towards the positive z-space and y-axis pointing downwards, must be valid
		 * @param worldGravityInWorld The world gravity vector defined in the world coordinate system, must be a unit vector
		 * @return The camera gravity vector in the flipped camera coordinate system
		 */
		static VectorT3<T> cameraGravityInFlippedCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldGravityInWorld);

	protected:

		/// The gravity vectors which are known for one or more camera poses, defined in the camera coordinate system, with default camera pointing towards the negative z-space and y-axis pointing upwards.
		Gravities cameraGravityInCameras_;

		/// The world gravity vector defined in the world coordinate system, mainly defining how the world coordinate system is oriented/aligned wrt gravity.
		VectorT3<T> worldGravityInWorld_ = VectorT3<T>(0, -1, 0);

		/// The weight factor to be used during a non-linear optimization of a camera pose; 0 to skip any gravity correction, 1 to apply a full gravity correction; with range [0, 1).
		T weightFactor_ = T(-1);

		/// The maximal angle between world and camera gravity vectors (when converted into the same coordinate system), can be used e.g., when the camera pose is determined e.g., with RANSAC or a PnP algorithm, in radian, with range [0, PI/2).
		T maximalAngle_ = T(-1);

		/// The cosine value of maximalAngle(), with range [0, 1]
		T minimalAngleCos_ = T(-1);
};

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(const VectorT3<T>& cameraGravityInCamera, const VectorT3<T>& worldGravityInWorld, const T weightFactor, const T maximalAngle) :
	cameraGravityInCameras_(1, cameraGravityInCamera),
	worldGravityInWorld_(worldGravityInWorld),
	weightFactor_(weightFactor),
	maximalAngle_(maximalAngle)
{
	minimalAngleCos_ = NumericT<T>::cos(maximalAngle_);
	ocean_assert(minimalAngleCos_ >= T(0) && minimalAngleCos_ <= T(1));

	ocean_assert(isValid());
}

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(VectorsT3<T>&& cameraGravityInCameras, const VectorT3<T>& worldGravityInWorld, const T weightFactor, const T maximalAngle) :
	cameraGravityInCameras_(std::move(cameraGravityInCameras)),
	worldGravityInWorld_(worldGravityInWorld),
	weightFactor_(weightFactor),
	maximalAngle_(maximalAngle)
{
	minimalAngleCos_ = NumericT<T>::cos(maximalAngle_);
	ocean_assert(minimalAngleCos_ >= T(0) && minimalAngleCos_ <= T(1));

	ocean_assert(isValid());
}

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldGravityInWorld, const T weightFactor, const T maximalAngle) :
	GravityConstraintsT<T>(cameraGravityInCamera(world_T_camera.rotation().inverted(), worldGravityInWorld), worldGravityInWorld, weightFactor, maximalAngle)
{
	ocean_assert(isValid());

	ocean_assert(isCameraAlignedWithGravity(world_T_camera, 0, NumericT<T>::deg2rad(1)));
}

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(const HomogenousMatricesT4<T>& world_T_cameras, const VectorT3<T>& worldGravityInWorld, const T weightFactor, const T maximalAngle)
{
	cameraGravityInCameras_.reserve(world_T_cameras.size());

	for (const HomogenousMatrixT4<T>& world_T_camera : world_T_cameras)
	{
		cameraGravityInCameras_.emplaceBack(cameraGravityInCamera(world_T_camera.rotation().inverted(), worldGravityInWorld));
	}

	worldGravityInWorld_ = worldGravityInWorld;
	weightFactor_ = weightFactor;
	maximalAngle_ = maximalAngle;

	minimalAngleCos_ = NumericT<T>::cos(maximalAngle_);
	ocean_assert(minimalAngleCos_ >= T(0) && minimalAngleCos_ <= T(1));

#ifdef OCEAN_DEBUG
	for (size_t nCamera = 0; nCamera < world_T_cameras.size(); ++nCamera)
	{
		const HomogenousMatrixT4<T>& world_T_camera = world_T_cameras[nCamera];

		ocean_assert(isCameraAlignedWithGravity(world_T_camera, nCamera, NumericT<T>::deg2rad(1)));
	}
#endif

	ocean_assert(isValid());
}

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(const GravityConstraintsT<T>& constraints, const size_t cameraIndex) :
	GravityConstraintsT<T>(constraints.cameraGravityInCamera(cameraIndex), constraints.worldGravityInWorld_, constraints.weightFactor_, constraints.maximalAngle_)
{
	ocean_assert(isValid());
	ocean_assert(numberCameras() == 1);
}

template <typename T>
inline GravityConstraintsT<T>::GravityConstraintsT(const GravityConstraintsT<T>* constraints, const size_t cameraIndex)
{
	if (constraints != nullptr)
	{
		*this = GravityConstraintsT<T>(*constraints, cameraIndex);

		ocean_assert(isValid());
		ocean_assert(numberCameras() == 1);
	}
	else
	{
		ocean_assert(!isValid());
	}
}

template <typename T>
inline const VectorT3<T>& GravityConstraintsT<T>::cameraGravityInCamera(const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(cameraIndex < cameraGravityInCameras_.size());

	return cameraGravityInCameras_[cameraIndex];
}

template <typename T>
inline VectorT3<T> GravityConstraintsT<T>::cameraGravityInFlippedCamera(const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(cameraIndex < cameraGravityInCameras_.size());

	const VectorT3<T>& cameraGravityInCamera = cameraGravityInCameras_[cameraIndex];

	// the flipped camera is pointing towards the positive z-space and y-axis pointing downwards

	return VectorT3<T>(cameraGravityInCamera.x(), -cameraGravityInCamera.y(), -cameraGravityInCamera.z()); // Camera::standard2InvertedFlipped(QuaternionT<T>(true)) * targetGravityInCamera;
}

template <typename T>
inline VectorT3<T> GravityConstraintsT<T>::cameraGravityInWorld(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(world_Q_camera.isValid());

	return world_Q_camera * cameraGravityInCamera(cameraIndex);
}

template <typename T>
inline VectorT3<T> GravityConstraintsT<T>::cameraGravityInWorld(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(world_T_camera.isValid());

	return cameraGravityInWorld(world_T_camera.rotation(), cameraIndex);
}

template <typename T>
inline const typename GravityConstraintsT<T>::Gravities& GravityConstraintsT<T>::cameraGravitiesInCamera() const
{
	ocean_assert(isValid());

	return cameraGravityInCameras_;
}

template <typename T>
inline const VectorT3<T>& GravityConstraintsT<T>::worldGravityInWorld() const
{
	ocean_assert(isValid());

	return worldGravityInWorld_;
}

template <typename T>
inline VectorT3<T> GravityConstraintsT<T>::worldGravityInFlippedCameraIF(const QuaternionT<T>& flippedCamera_Q_world) const
{
	ocean_assert(isValid());
	ocean_assert(flippedCamera_Q_world.isValid());

	return flippedCamera_Q_world * worldGravityInWorld_;
}

template <typename T>
inline VectorT3<T> GravityConstraintsT<T>::worldGravityInFlippedCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world) const
{
	ocean_assert(isValid());
	ocean_assert(flippedCamera_T_world.isValid());

	return worldGravityInFlippedCameraIF(flippedCamera_T_world.rotation());
}

template <typename T>
inline QuaternionT<T> GravityConstraintsT<T>::world_R_camera(const size_t cameraIndex) const
{
	ocean_assert(isValid());

	return QuaternionT<T>(cameraGravityInCamera(cameraIndex), worldGravityInWorld_);
}

template <typename T>
inline QuaternionT<T> GravityConstraintsT<T>::camera_R_world(const size_t cameraIndex) const
{
	ocean_assert(isValid());

	return QuaternionT<T>(worldGravityInWorld_, cameraGravityInCamera(cameraIndex));
}

template <typename T>
inline QuaternionT<T> GravityConstraintsT<T>::flippedCamera_R_world(const size_t cameraIndex) const
{
	ocean_assert(isValid());

	return QuaternionT<T>(worldGravityInWorld_, cameraGravityInFlippedCamera(cameraIndex));
}

template <typename T>
inline T GravityConstraintsT<T>::weightFactor() const
{
	ocean_assert(isValid());

	return weightFactor_;
}

template <typename T>
inline T GravityConstraintsT<T>::maximalAngle() const
{
	ocean_assert(isValid());

	return maximalAngle_;
}

template <typename T>
inline T GravityConstraintsT<T>::minimalAngleCos() const
{
	ocean_assert(isValid());

	return minimalAngleCos_;
}

template <typename T>
inline size_t GravityConstraintsT<T>::numberCameras() const
{
	ocean_assert(isValid());

	return cameraGravityInCameras_.size();
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentCosine(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(world_Q_camera.isValid());

	const VectorT3<T> knownCameraGravityInWorld = cameraGravityInWorld(world_Q_camera, cameraIndex);

	ocean_assert(worldGravityInWorld_.isUnit());
	ocean_assert(knownCameraGravityInWorld.isUnit());

	const T cosine = worldGravityInWorld_ * knownCameraGravityInWorld;
	ocean_assert(NumericT<T>::isInsideRange(-1, cosine, 1));

	return cosine;
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentCosine(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex) const
{
	ocean_assert(world_T_camera.isValid());

	return alignmentCosine(world_T_camera.rotation(), cameraIndex);
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentCosineIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(flippedCamera_Q_world.isValid());

	const VectorT3<T> worldGravityInFlippedCamera = worldGravityInFlippedCameraIF(flippedCamera_Q_world);
	const VectorT3<T> knownCameraGravityInFlippedCamera = cameraGravityInFlippedCamera(cameraIndex);

	ocean_assert(worldGravityInFlippedCamera.isUnit());
	ocean_assert(knownCameraGravityInFlippedCamera.isUnit());

	const T cosine = worldGravityInFlippedCamera * knownCameraGravityInFlippedCamera;
	ocean_assert(NumericT<T>::isInsideRange(-1, cosine, 1));

	return cosine;
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentCosineIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex) const
{
	ocean_assert(flippedCamera_T_world.isValid());

	return alignmentCosineIF(flippedCamera_T_world.rotation(), cameraIndex);
}


template <typename T>
inline T GravityConstraintsT<T>::alignmentAngle(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex) const
{
	return NumericT<T>::acos(alignmentCosine(world_Q_camera, cameraIndex));
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentAngle(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex) const
{
	ocean_assert(world_T_camera.isValid());

	return alignmentAngle(world_T_camera.rotation(), cameraIndex);
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentAngleIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex) const
{
	return NumericT<T>::acos(alignmentCosineIF(flippedCamera_Q_world, cameraIndex));
}

template <typename T>
inline T GravityConstraintsT<T>::alignmentAngleIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex) const
{
	ocean_assert(flippedCamera_T_world.isValid());

	return alignmentAngleIF(flippedCamera_T_world.rotation(), cameraIndex);
}

template <typename T>
bool GravityConstraintsT<T>::isCameraAlignedWithGravity(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex) const
{
	ocean_assert(isValid());

	return alignmentCosine(world_Q_camera, cameraIndex) >= minimalAngleCos_;
}

template <typename T>
inline bool GravityConstraintsT<T>::isCameraAlignedWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex) const
{
	ocean_assert(world_T_camera.isValid());

	return isCameraAlignedWithGravity(world_T_camera.rotation(), cameraIndex);
}

template <typename T>
bool GravityConstraintsT<T>::isCameraAlignedWithGravityIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex) const
{
	ocean_assert(isValid());

	return isCameraAlignedWithGravityIF(flippedCamera_Q_world, cameraIndex) >= minimalAngleCos_;
}

template <typename T>
inline bool GravityConstraintsT<T>::isCameraAlignedWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex) const
{
	ocean_assert(flippedCamera_T_world.isValid());
	return isCameraAlignedWithGravityIF(flippedCamera_T_world.rotation(), cameraIndex);
}

template <typename T>
bool GravityConstraintsT<T>::isCameraAlignedWithGravity(const QuaternionT<T>& world_Q_camera, const size_t cameraIndex, const T maxAngle) const
{
	ocean_assert(maxAngle >= 0 && maxAngle < NumericT<T>::deg2rad(90));

	const T angle = alignmentAngle(world_Q_camera, cameraIndex);

	return angle <= maxAngle;
}

template <typename T>
inline bool GravityConstraintsT<T>::isCameraAlignedWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex, const T maxAngle) const
{
	ocean_assert(world_T_camera.isValid());

	return isCameraAlignedWithGravity(world_T_camera.rotation(), cameraIndex, maxAngle);
}

template <typename T>
bool GravityConstraintsT<T>::isCameraAlignedWithGravityIF(const QuaternionT<T>& flippedCamera_Q_world, const size_t cameraIndex, const T maxAngle) const
{
	ocean_assert(maxAngle >= 0 && maxAngle < NumericT<T>::deg2rad(90));

	const T angle = alignmentAngleIF(flippedCamera_Q_world, cameraIndex);

	return angle <= maxAngle;
}

template <typename T>
inline bool GravityConstraintsT<T>::isCameraAlignedWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex, const T maxAngle) const
{
	ocean_assert(flippedCamera_T_world.isValid());

	return isCameraAlignedWithGravityIF(flippedCamera_T_world.rotation(), cameraIndex, maxAngle);
}

template <typename T>
HomogenousMatrixT4<T> GravityConstraintsT<T>::alignCameraWithGravity(const HomogenousMatrixT4<T>& world_T_camera, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(world_T_camera.isValid());

	const VectorT3<T> knownCameraGravityInCamera = cameraGravityInCamera(cameraIndex);

	const QuaternionT<T> camera_Q_world = world_T_camera.rotation().inverted();

	const VectorT3<T> worldGravityInCamera = camera_Q_world * worldGravityInWorld_;

	const QuaternionT<T> camera_Q_aligned(knownCameraGravityInCamera, worldGravityInCamera);

	const HomogenousMatrixT4<T> world_T_alignedCamera = world_T_camera * HomogenousMatrixT4<T>(camera_Q_aligned);

	ocean_assert(isCameraAlignedWithGravity(world_T_alignedCamera, cameraIndex, NumericT<T>::deg2rad(1)));

	return world_T_alignedCamera;
}

template <typename T>
HomogenousMatrixT4<T> GravityConstraintsT<T>::alignCameraWithGravityIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const size_t cameraIndex) const
{
	ocean_assert(isValid());
	ocean_assert(flippedCamera_T_world.isValid());

	const VectorT3<T> knownCameraGravityInFlippedCamera = cameraGravityInFlippedCamera(cameraIndex);
	const VectorT3<T> worldGravityInFlippedCamera = worldGravityInFlippedCameraIF(flippedCamera_T_world);

	const QuaternionT<T> aligned_Q_flippedCamera(worldGravityInFlippedCamera, knownCameraGravityInFlippedCamera);

	const HomogenousMatrixT4<T> alignedFlippedCamera_T_world = HomogenousMatrixT4<T>(aligned_Q_flippedCamera) * flippedCamera_T_world;

	ocean_assert(isCameraAlignedWithGravityIF(alignedFlippedCamera_T_world, cameraIndex, NumericT<T>::deg2rad(1)));

	return alignedFlippedCamera_T_world;
}

template <typename T>
inline const GravityConstraintsT<T>* GravityConstraintsT<T>::conditionalPointer(const bool pointerIfValid) const
{
	if (pointerIfValid && isValid())
	{
		return this;
	}

	return nullptr;
}

template <typename T>
inline bool GravityConstraintsT<T>::isValid() const
{
	const bool valid = weightFactor_ >= T(0)
		&& maximalAngle_ >= NumericT<T>::rad2deg(0) && maximalAngle_ <= NumericT<T>::rad2deg(90)
		&& !cameraGravityInCameras_.isEmpty() && !worldGravityInWorld_.isNull();

#ifdef OCEAN_DEBUG
	if (valid)
	{
		for (const VectorT3<T>& cameraGravityInCamera : cameraGravityInCameras_)
		{
			ocean_assert(cameraGravityInCamera.isUnit());
		}

		ocean_assert(worldGravityInWorld_.isUnit());
	}
#endif

	return valid;
}

template <typename T>
VectorT3<T> GravityConstraintsT<T>::cameraGravityInCamera(const QuaternionT<T>& camera_Q_world, const VectorT3<T>& worldGravityInWorld)
{
	ocean_assert(camera_Q_world.isValid());
	ocean_assert(worldGravityInWorld.isUnit());

	return camera_Q_world * worldGravityInWorld;
}

template <typename T>
VectorT3<T> GravityConstraintsT<T>::cameraGravityInCamera(const HomogenousMatrixT4<T>& camera_T_world, const VectorT3<T>& worldGravityInWorld)
{
	ocean_assert(camera_T_world.isValid());
	ocean_assert(worldGravityInWorld.isUnit());

	const SquareMatrixT3<T> camera_R_world(camera_T_world.rotationMatrix());
	ocean_assert(camera_R_world.isOrthonormal(NumericT<T>::weakEps()));

	return camera_R_world * worldGravityInWorld;
}

template <typename T>
VectorT3<T> GravityConstraintsT<T>::cameraGravityInCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldGravityInWorld)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(worldGravityInWorld.isUnit());

	const VectorT3<T> cameraGravityInFlippedCamera = cameraGravityInFlippedCameraIF(flippedCamera_T_world, worldGravityInWorld);

	return VectorT3<T>(cameraGravityInFlippedCamera.x(), -cameraGravityInFlippedCamera.y(), -cameraGravityInFlippedCamera.z());
}

template <typename T>
VectorT3<T> GravityConstraintsT<T>::cameraGravityInFlippedCameraIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldGravityInWorld)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(worldGravityInWorld.isUnit());

	const SquareMatrixT3<T> flippedCamera_R_world(flippedCamera_T_world.rotationMatrix());
	ocean_assert(flippedCamera_R_world.isOrthonormal(NumericT<T>::weakEps()));

	return flippedCamera_R_world * worldGravityInWorld;
}

}

}

#endif // META_OCEAN_GEOMETRY_GRAVITY_CONSTRAINTS_H
