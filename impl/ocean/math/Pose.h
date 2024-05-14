/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_POSE_H
#define META_OCEAN_MATH_POSE_H

#include "ocean/math/Math.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class PoseT;

/**
 * Definition of the Pose object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION flag either with single or double precision float data type.
 * @see PoseT
 * @ingroup math
 */
using Pose = PoseT<Scalar>;

/**
 * Instantiation of the ExponentialMapT template class using a double precision float data type.
 * @see ExponentialMapT
 * @ingroup math
 */
using PoseD = PoseT<double>;

/**
 * Instantiation of the ExponentialMapT template class using a single precision float data type.
 * @see ExponentialMapT
 * @ingroup math
 */
using PoseF = PoseT<float>;

/**
 * Definition of a typename alias for vectors with PoseT objects.
 * @see PoseT
 * @ingroup math
 */
template <typename T>
using PosesT = std::vector<PoseT<T>>;

/**
 * Definition of a vector holding ExponentialMap objects.
 * @see ExponentialMap
 * @ingroup math
 */
using Poses = std::vector<Pose>;

/**
 * This class implements a camera pose with six degrees of freedom (6-DOF).
 * Three degrees for the translation or position and three for the orientation or rotation.<br>
 * This object stores six elements. The first three elements define the translation.<br>
 * The last three elements define the orientation as exponential map (rotation axis and angle as axis length).<br>
 * The element order is defined as: (Tx, Ty, Tz, Rx, Ry, Rz).
 * @tparam T The scalar floating point data type to be used, either 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class PoseT
{
	public:

		/**
		 * Creates a new pose object with default values (no translation and no rotation).
		 */
		PoseT();

		/**
		 * Copies a pose with different element data type than T.
		 * @param pose The pose object to be copied
		 * @tparam U The element data type of the given pose
		 */
		template <typename U>
		explicit inline PoseT(const PoseT<U>& pose);

		/**
		 * Creates a new pose object with a translation only.
		 * @param translation 3D vector defining the translation of the pose object
		 */
		explicit PoseT(const VectorT3<T>& translation);

		/**
		 * Creates a new pose object with a rotation component on.
		 * @param euler The Euler rotation defining the rotation of the pose object
		 */
		explicit PoseT(const EulerT<T>& euler);

		/**
		 * Creates a new pose object with a rotation component on.
		 * @param quaternion Unit quaternion rotation defining the rotation of the pose object
		 */
		explicit PoseT(const QuaternionT<T>& quaternion);

		/**
		 * Creates a new pose object with a rotation component on.
		 * @param rotation Angle-axis rotation defining the rotation of the pose object
		 */
		explicit PoseT(const RotationT<T>& rotation);

		/**
		 * Creates a new pose object by a specified 4x4 homogeneous transformation matrix.
		 * @param transformation Matrix defining the position and rotation of the new pose
		 */
		explicit PoseT(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a new pose by at least six pose values.
		 * The specified pose must have the following order: (Tx, Ty, Tz, Rx, Ry, Rz).<br>
		 * With (Tx, Ty, Tz) as translation vector, (Rx, Ry, Rz) as rotation axis and sqrt(Rx * Rx + Ry * Ry + Rz * Rz) as rotation angle.
		 * @param values The six pose values specifying the new pose, must be valid
		 */
		explicit PoseT(const T* values);

		/**
		 * Creates a new pose by six pose parameters.
		 * @param tx Translation value for the x-axis
		 * @param ty Translation value for the y-axis
		 * @param tz Translation value for the z-axis
		 * @param rx X parameter of the rotation
		 * @param ry Y parameter of the rotation
		 * @param rz Z parameter of the rotation
		 */
		PoseT(const T tx, const T ty, const T tz, const T rx, const T ry, const T rz);

		/**
		 * Creates a new pose object.
		 * @param translation 3D vector defining the translation component of the pose object
		 * @param euler The Euler rotation defining the rotation component of the pose object
		 */
		PoseT(const VectorT3<T>& translation, const EulerT<T>& euler);

		/**
		 * Creates a new pose object.
		 * @param translation 3D vector defining the translation component of the pose object
		 * @param quaternion Unit quaternion rotation defining the rotation component of the pose object
		 */
		PoseT(const VectorT3<T>& translation, const QuaternionT<T>& quaternion);

		/**
		 * Creates a new pose object.
		 * @param translation 3D vector defining the translation component of the pose object
		 * @param rotation Angle-axis rotation defining the rotation component of the pose object
		 */
		PoseT(const VectorT3<T>& translation, const RotationT<T>& rotation);

		/**
		 * Returns the translation of this pose.
		 * @return 3D translation vector
		 */
		inline VectorT3<T> translation() const;

		/**
		 * Returns the orientation of this pose.
		 * @return Unit quaternion defining the pose orientation
		 */
		QuaternionT<T> orientation() const;

		/**
		 * Returns the 4x4 homogeneous transformation matrix of this pose.
		 * @return Transformation matrix
		 */
		HomogenousMatrixT4<T> transformation() const;

		/**
		 * Returns the translation value for the x-axis.
		 * @return Translation value for x-axis
		 */
		inline T x() const;

		/**
		 * Returns the translation value for the x-axis.
		 * @return Translation value for x-axis
		 */
		inline T& x();

		/**
		 * Returns the translation value for the y-axis.
		 * @return Translation value for y-axis
		 */
		inline T y() const;

		/**
		 * Returns the translation value for the y-axis.
		 * @return Translation value for y-axis
		 */
		inline T& y();

		/**
		 * Returns the translation value for the z-axis.
		 * @return Translation value for z-axis
		 */
		inline T z() const;

		/**
		 * Returns the translation value for the z-axis.
		 * @return Translation value for z-axis
		 */
		inline T& z();

		/**
		 * Returns the x parameter of the rotation.
		 * @return Rotation x parameter
		 */
		inline T rx() const;

		/**
		 * Returns the x parameter of the rotation.
		 * @return Rotation x parameter
		 */
		inline T& rx();

		/**
		 * Returns the y parameter of the rotation.
		 * @return Rotation y parameter
		 */
		inline T ry() const;

		/**
		 * Returns the y parameter of the rotation.
		 * @return Rotation y parameter
		 */
		inline T& ry();

		/**
		 * Returns the z parameter of the rotation.
		 * @return Rotation z parameter
		 */
		inline T rz() const;

		/**
		 * Returns the z parameter of the rotation.
		 * @return Rotation z parameter
		 */
		inline T& rz();

		/**
		 * Returns the angle of the pose rotation.
		 * @return Rotation angle in radian
		 */
		T angle() const;

		/**
		 * Returns whether this pose holds no translation and no rotation.
		 * @return True, if so
		 */
		bool isNull() const;

		/**
		 * Data access operator.
		 * @return Pointer to the internal elements.
		 */
		inline const T* data() const;

		/**
		 * Data access operator.
		 * @return Pointer to the internal elements.
		 */
		inline T* data();

		/**
		 * Returns whether two poses are identical up to a small epsilon.
		 * @param pose Right pose to compare
		 * @return True, if so
		 */
		inline bool operator==(const PoseT<T>& pose) const;

		/**
		 * Returns whether two poses are not identical up to a small epsilon.
		 * @param pose Right pose to compare
		 * @return True, if so
		 */
		inline bool operator!=(const PoseT<T>& pose) const;

		/**
		 * Returns a new pose created by the element wise sum of two poses.
		 * @param pose Second pose to add
		 * @return New pose
		 */
		PoseT<T> operator+(const PoseT<T>& pose) const;

		/**
		 * Adds element wise the six values of a second pose to this pose.
		 * @param pose Second pose to add the six values
		 * @return Reference to this object
		 */
		PoseT<T>& operator+=(const PoseT<T>& pose);

		/**
		 * Returns a new pose created by the element wise subtraction of two poses.
		 * @param pose Second pose to subtract
		 * @return New pose
		 */
		PoseT<T> operator-(const PoseT<T>& pose) const;

		/**
		 * Subtracts element wise the six values of a second pose from this pose.
		 * @param pose Second pose to subtract the six values
		 * @return Reference to this object
		 */
		PoseT<T>& operator-=(const PoseT<T>& pose);

		/**
		 * Multiplies this pose by a scalar value and returns the new result.
		 * The multiplication is done element wise.
		 * @param value Scalar value for multiplication
		 * @return New resulting pose
		 */
		PoseT<T> operator*(const T value) const;

		/**
		 * Multiplies this pose by a scalar.
		 * The multiplication is done element wise.
		 * @param value Scalar value for multiplication
		 * @return Reference to this object
		 */
		PoseT<T>& operator*=(const T value);

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return, with range [0, 5]
		 * @return Internal element
		 */
		inline T operator()(unsigned int index) const;

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return, with range [0, 5]
		 * @return Internal element
		 */
		inline T& operator()(unsigned int index);

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return, with range [0, 5]
		 * @return Internal element
		 */
		inline T operator[](unsigned int index) const;

		/**
		 * Element access operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return, with range [0, 5]
		 * @return Internal element
		 */
		inline T& operator[](unsigned int index);

		/**
		 * Access operator.
		 * @return Pointer to the internal elements.
		 */
		inline const T* operator()() const;

		/**
		 * Access operator.
		 * @return Pointer to the internal elements.
		 */
		inline T* operator()();

		/**
		 * Determines a subset of a set of given poses best representing the entire set of poses.
		 * @param poses The entire set of given poses from which a subset is extracted, must be valid
		 * @param subsetSize The number of poses in the resulting subset, with range [1, poses.size()]
		 * @param explicitIndexOffset The explicit offset which is added to each index of the resulting (pose) subset allowing to add an explicit index shift
		 * @return The indices of the poses defining the subset
		 */
		static Indices32 representativePoses(const PosesT<T>& poses, const size_t subsetSize, const size_t explicitIndexOffset = 0);

		/**
		 * Determines a subset of a set of given poses (defined as matrices) best representing the entire set of poses.
		 * @param poseMatrices The entire set of given pose matrices from which a subset is extracted, must be valid
		 * @param size The number of provided set of pose matrices, with range [1, infinity)
		 * @param subsetSize The number of poses in the resulting subset, with range [1, size]
		 * @param explicitIndexOffset The explicit offset which is added to each index of the resulting (pose) subset allowing to add an explicit index shift
		 * @return The indices of the poses defining the subset
		 */
		static Indices32 representativePoses(const HomogenousMatrixT4<T>* poseMatrices, const size_t size, const size_t subsetSize, const size_t explicitIndexOffset = 0);

		/**
		 * Interpolates two camera poses by a linear interpolation.
		 * @param pose0 The first camera pose
		 * @param pose1 The second camera pose
		 * @param factor The interpolation factor which is applied as follows: pose0 * (1 - factor) + pose1 * factor, with range [0, 1]
		 * @return The resulting interpolated camera pose
		 */
		static HomogenousMatrixT4<T> linearPoseInterpolation(const HomogenousMatrixT4<T>& pose0, const HomogenousMatrixT4<T>& pose1, const T& factor);

	protected:

		/// The six values of the pose, with element order: (Tx, Ty, Tz, Rx, Ry, Rz).
		T values_[6];
};

template <typename T>
template <typename U>
inline PoseT<T>::PoseT(const PoseT<U>& pose)
{
	values_[0] = T(pose[0]);
	values_[1] = T(pose[1]);
	values_[2] = T(pose[2]);
	values_[3] = T(pose[3]);
	values_[4] = T(pose[4]);
	values_[5] = T(pose[5]);
}

template <typename T>
inline VectorT3<T> PoseT<T>::translation() const
{
	return VectorT3<T>(values_);
}

template <typename T>
inline T PoseT<T>::x() const
{
	return values_[0];
}

template <typename T>
inline T& PoseT<T>::x()
{
	return values_[0];
}

template <typename T>
inline T PoseT<T>::y() const
{
	return values_[1];
}

template <typename T>
inline T& PoseT<T>::y()
{
	return values_[1];
}

template <typename T>
inline T PoseT<T>::z() const
{
	return values_[2];
}

template <typename T>
inline T& PoseT<T>::z()
{
	return values_[2];
}

template <typename T>
inline T PoseT<T>::rx() const
{
	return values_[3];
}

template <typename T>
inline T& PoseT<T>::rx()
{
	return values_[3];
}

template <typename T>
inline T PoseT<T>::ry() const
{
	return values_[4];
}

template <typename T>
inline T& PoseT<T>::ry()
{
	return values_[4];
}

template <typename T>
inline T PoseT<T>::rz() const
{
	return values_[5];
}

template <typename T>
inline T& PoseT<T>::rz()
{
	return values_[5];
}

template <typename T>
inline bool PoseT<T>::operator==(const PoseT<T>& pose) const
{
	return NumericT<T>::isEqual(values_[0], pose.values_[0]) && NumericT<T>::isEqual(values_[1], pose.values_[1]) && NumericT<T>::isEqual(values_[2], pose.values_[2]) && orientation() == pose.orientation();
}

template <typename T>
inline bool PoseT<T>::operator!=(const PoseT<T>& pose) const
{
	return !(*this == pose);
}

template <typename T>
inline T PoseT<T>::operator()(unsigned int index) const
{
	ocean_assert(index < 6);
	return values_[index];
}

template <typename T>
inline T& PoseT<T>::operator()(unsigned int index)
{
	ocean_assert(index < 6);
	return values_[index];
}

template <typename T>
inline T PoseT<T>::operator[](unsigned int index) const
{
	ocean_assert(index < 6);
	return values_[index];
}

template <typename T>
inline T& PoseT<T>::operator[](unsigned int index)
{
	ocean_assert(index < 6);
	return values_[index];
}

template <typename T>
inline const T* PoseT<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* PoseT<T>::operator()()
{
	return values_;
}

template <typename T>
inline const T* PoseT<T>::data() const
{
	return values_;
}

template <typename T>
inline T* PoseT<T>::data()
{
	return values_;
}

}

#endif // META_OCEAN_MATH_POSE_H
