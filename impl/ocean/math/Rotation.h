/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_ROTATION_H
#define META_OCEAN_MATH_ROTATION_H

#include "ocean/math/Math.h"
#include "ocean/math/Euler.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class HomogenousMatrixT4;
// Forward declaration.
template <typename T> class RotationT;
// Forward declaration.
template <typename T> class SquareMatrixT3;

/**
 * Definition of the Rotation object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION flag either with single or double precision float data type.
 * @see RotationT
 * @ingroup math
 */
using Rotation = RotationT<Scalar>;

/**
 * Instantiation of the RotationT template class using a double precision float data type.
 * @see RotationT
 * @ingroup math
 */
using RotationD = RotationT<double>;

/**
 * Instantiation of the RotationT template class using a single precision float data type.
 * @see RotationT
 * @ingroup math
 */
using RotationF = RotationT<float>;

/**
 * Definition of a typename alias for vectors with RotationT objects.
 * @see RotationT
 * @ingroup math
 */
template <typename T>
using RotationsT = std::vector<RotationT<T>>;

/**
 * Definition of a vector holding rotation objects.
 * @see Rotation
 * @ingroup math
 */
using Rotations = std::vector<Rotation>;

/**
 * This class implements a axis-angle rotation using floating point values.
 * The angle is defined in radian [0, 2*PI).<br>
 * The four elements are stored with order: (x, y, z, angle).
 * @tparam T Data type used to represent axis and angle
 * @see Rotation, RotationF, RotationD, Quaternion, Euler, SquareMatrix3, ExponentialMap.
 * @ingroup math
 */
template <typename T>
class RotationT
{
	public:

		/**
		 * Definition of the used data type.
		 */
		using Type = T;

	public:

		/**
		 * Creates a rotation object with default values so that the rotation represents the identity rotation.
		 * The axis will be set to (0, 1, 0) and the angle to 0.
		 */
		RotationT() = default;

		/**
		 * Creates a rotation object by four given values.
		 * The axis must be a unit vector with length 1.
		 * @param x X value of the rotation axis
		 * @param y Y value of the rotation axis
		 * @param z Z value of the rotation axis
		 * @param angle The angle of the rotation in radian, with range (-infinity, infinity), however will be converted to the range [0.0, 2 * PI)
		 */
		RotationT(const T x, const T y, const T z, const T angle);

		/**
		 * Creates a rotation object by an axis and the angle.
		 * @param axis The axis of the rotation with length 1
		 * @param angle The angle of the rotation in radian, with range (-infinity, infinity), however will be converted to the range [0.0, 2 * PI)
		 */
		RotationT(const VectorT3<T>& axis, const T angle);

		/**
		 * Creates a rotation object based on two given unit vectors.
		 * The resulting rotation defines a transformation that rotates that reference vector into the offset vector: Rotation(reference, offset) = offset_R_reference.<br>
		 * The following equation holds:
		 * <pre>
		 * offset = Rotation(reference, offset) * reference.
		 * </pre>
		 * @param reference The reference vector, with length 1
		 * @param offset The offset vector, with length 1
		 */
		RotationT(const VectorT3<T>& reference, const VectorT3<T>& offset);

		/**
		 * Creates a rotation object by a given quaternion rotation.
		 * @param quaternion the quaternion rotation to create an axis-angle rotation from, must be valid
		 */
		explicit RotationT(const QuaternionT<T>& quaternion);

		/**
		 * Creates an angle-axis rotation by a given euler rotation.
		 * @param euler Euler rotation to create a angle-axis rotation from, must be valid
		 */
		explicit RotationT(const EulerT<T>& euler);

		/**
		 * Creates a rotation object by a given 3x3 rotation matrix.
		 * Beware: Ensure that the provided matrix does not contain any scale.
		 * @param matrix Rotation matrix to create a axis-angle rotation from, with determinant 1
		 */
		explicit RotationT(const SquareMatrixT3<T>& matrix);

		/**
		 * Creates a rotation object by a given 4x4 transformation matrix.
		 * @param transformation The transformation matrix to create a axis-angle rotation from, must be valid
		 */
		explicit RotationT(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a rotation object by an array with four elements.
		 * The element order in the array has to be: (x, y, z, angle).<br>
		 * The axis must have length 1, the angle is provided as radian with range (-infinity, infinity), however will be converted to the range [0.0, 2 * PI)
		 * @param valueArray Array with four elements
		 */
		explicit RotationT(const T* valueArray);

		/**
		 * Sets the axis of the rotation.
		 * @param axis The axis with length 1
		 */
		void setAxis(const VectorT3<T>& axis);

		/**
		 * Sets the angle of the rotation.
		 * @param angle The angle of rotation in radian, with range (-infinity, infinity), however will be converted to the range [0.0, 2 * PI)
		 */
		void setAngle(const T angle);

		/**
		 * Returns a pointer to the internal values.
		 * @return Pointer to the internal values
		 */
		inline const T* data() const;

		/**
		 * Returns a pointer to the internal values.
		 * @return Pointer to the internal values
		 */
		inline T* data();

		/**
		 * Returns the axis of the rotation.
		 * @return Rotation axis with unit length if the rotation is valid
		 */
		inline VectorT3<T> axis() const;

		/**
		 * Returns the angle of the rotation.
		 * @return Rotation angle in radian, with range [0.0, 2 * PI)
		 */
		inline T angle() const;

		/**
		 * Returns the inverted rotation, this rotation must be valid.
		 * @return Inverted rotation
		 * @see isValid(), invert().
		 */
		[[nodiscard]] RotationT inverted() const;

		/**
		 * Inverts this rotation, this rotation must be valid.
		 * @see isValid(), inverted().
		 */
		void invert();

		/**
		 * Returns whether this rotation has valid parameters.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether two rotations are equal up to a specified epsilon.
		 * @param rotation The second rotation to compare
		 * @param eps Epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const RotationT<T>& rotation, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether two rotations are identical up to a small epsilon.
		 * @param right The right operand
		 * @return True, if so
		 */
		bool operator==(const RotationT<T>& right) const;

		/**
		 * Returns whether two rotations are not identical up to a small epsilon.
		 * @param right The right operand
		 * @return True, if so
		 */
		inline bool operator!=(const RotationT<T>& right) const;

		/**
		 * Returns the inverse rotation, this rotation must be valid.
		 * @return Inverse rotation
		 */
		RotationT operator-() const;

		/**
		 * Multiplies two rotations, this rotation must be valid.
		 * @param quaternion The right rotation as unit quaternion, must be valid
		 * @return Resulting rotation
		 */
		RotationT operator*(const QuaternionT<T>& quaternion) const;

		/**
		 * Multiplies and assign two rotations, this rotation must be valid.
		 * @param quaternion The right rotation as unit quaternion, must be valid
		 * @return Reference to this rotation
		 */
		RotationT& operator*=(const QuaternionT<T>& quaternion);

		/**
		 * Multiplies two rotations, this rotation must be valid.
		 * @param right The right rotation to multiply, must be valid
		 * @return Resulting rotation
		 */
		RotationT operator*(const RotationT<T>& right) const;

		/**
		 * Multiplies and assign two rotations, this rotation must be valid.
		 * @param right The right rotation to multiply, must be valid
		 * @return Reference to this rotation
		 */
		RotationT& operator*=(const RotationT<T>& right);

		/**
		 * Rotates a 3D vector with this rotation, this rotation must be valid.
		 * @param vector The vector to rotate
		 * @return Rotated vector
		 */
		VectorT3<T> operator*(const VectorT3<T>& vector) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to return, with range [0, 3]
		 * @return Internal element
		 */
		inline T operator()(unsigned int index) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to return, with range [0, 3]
		 * @return Internal element
		 */
		inline T& operator()(unsigned int index);

		/**
		 * Element access operator.
		 * @param index The index of the element to return, with range [0, 3]
		 * @return Internal element
		 */
		inline T operator[](unsigned int index) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to return, with range [0, 3]
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
		 * Returns a rotation object based on two given unit vectors.
		 * The resulting rotation rotates the right vector to the left vector:
		 * <pre>
		 * left = Rotation::left_R_right(left, right) * right;
		 * </pre>
		 * @param left The left unit vector
		 * @param right The right unit vector
		 * @return The resulting rotation rotating the right vector to the left vector
		 */
		static RotationT<T> left_R_right(const VectorT3<T>& left, const VectorT3<T>& right);

	protected:

		/// The four values of the angle-axis rotation.
		T values_[4] = {T(0), T(1), T(0), T(0)};
};

template <typename T>
RotationT<T>::RotationT(const T x, const T y, const T z, const T angle)
{
	values_[0] = x;
	values_[1] = y;
	values_[2] = z;

	values_[3] = NumericT<T>::angleAdjustPositive(angle);

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const VectorT3<T>& axis, const T angle) :
	RotationT<T>(axis.x(), axis.y(), axis.z(), angle)
{
	// nothing to do here
}

template <typename T>
RotationT<T>::RotationT(const VectorT3<T>& reference, const VectorT3<T>& offset) :
	RotationT<T>(left_R_right(offset, reference))
{
	// nothing to do here
}

template <typename T>
RotationT<T>::RotationT(const QuaternionT<T>& quaternion)
{
	ocean_assert(quaternion.isValid());

	const T invFactor = NumericT<T>::sqrt(T(1.0) - quaternion.w() * quaternion.w());

	if (NumericT<T>::isEqualEps(invFactor))
	{
		values_[0] = T(0.0);
		values_[1] = T(1.0);
		values_[2] = T(0.0);
		values_[3] = T(0.0);

		return;
	}

	const T factor = T(1.0) / invFactor;

	VectorT3<T> axis(quaternion.x() * factor, quaternion.y() * factor, quaternion.z() * factor);
	axis.normalize();

	values_[0] = axis[0];
	values_[1] = axis[1];
	values_[2] = axis[2];

	values_[3] = T(2.0) * NumericT<T>::acos(quaternion.w());

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	*this = RotationT(QuaternionT<T>(euler));
	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const SquareMatrixT3<T>& matrix)
{
	const T cosValue = (matrix.trace() - T(1.0)) * T(0.5);

	if (NumericT<T>::isInsideRange(T(-1.0), cosValue, T(1.0)) == false)
	{
		ocean_assert(false && "Invalid rotation matrix, containing scale.");
	}

	if (NumericT<T>::isEqual(cosValue, T(1.0)))
	{
		values_[0] = T(0.0);
		values_[1] = T(1.0);
		values_[2] = T(0.0);
		values_[3] = T(0.0);
	}
	else
	{
		VectorT3<T> axis;

		if (NumericT<T>::isEqual(cosValue, T(-1.0)))
		{
			unsigned int select = 0;
			T maximum = matrix(0, 0);

			if (maximum < matrix(1, 1))
			{
				select = 1;
				maximum = matrix(1, 1);
			}
			if (maximum < matrix(2, 2))
			{
				select = 2;
			}

			switch (select)
			{
				case 0:
				{
					axis(0) = T(0.5) * NumericT<T>::sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + T(1.0));
					T factor = T(0.5) / axis(0);

					axis(1) = matrix(0, 1) * factor;
					axis(2) = matrix(0, 2) * factor;
					break;
				}

				case 1:
				{
					axis(1) = T(0.5) * NumericT<T>::sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + T(1.0));
					T factor = T(0.5) / axis(1);

					axis(0) = matrix(0, 1) * factor;
					axis(2) = matrix(1, 2) * factor;
					break;
				}

				case 2:
				{
					axis(2) = T(0.5) * NumericT<T>::sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + T(1.0));
					T factor = T(0.5) / axis(2);

					axis(0) = matrix(0, 2) * factor;
					axis(1) = matrix(1, 2) * factor;
					break;
				}

				default:
					ocean_assert(false);
					break;
			}

			values_[3] = NumericT<T>::pi();
		}
		else
		{
			axis = VectorT3<T>(matrix(2, 1) - matrix(1, 2), matrix(0, 2) - matrix(2, 0), matrix(1, 0) - matrix(0, 1));
			values_[3] = NumericT<T>::acos(cosValue);
		}

		axis.normalize();

		values_[0] = axis(0);
		values_[1] = axis(1);
		values_[2] = axis(2);
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const HomogenousMatrixT4<T>& transformation)
{
	ocean_assert(transformation.isValid());

	const SquareMatrixT3<T> matrix(transformation.orthonormalRotationMatrix());
	ocean_assert(NumericT<T>::isEqual(matrix.determinant(), T(1.0)));

	*this = RotationT<T>(matrix);
}

template <typename T>
RotationT<T>::RotationT(const T* valueArray)
{
	ocean_assert(valueArray != nullptr);

	values_[0] = valueArray[0];
	values_[1] = valueArray[1];
	values_[2] = valueArray[2];

	values_[3] = NumericT<T>::angleAdjustPositive(valueArray[3]);

	ocean_assert(isValid());
}

template <typename T>
void RotationT<T>::setAxis(const VectorT3<T>& axis)
{
	ocean_assert(axis.isUnit());

	values_[0] = axis[0];
	values_[1] = axis[1];
	values_[2] = axis[2];

	ocean_assert(isValid());
}

template <typename T>
void RotationT<T>::setAngle(const T angle)
{
	values_[3] = NumericT<T>::angleAdjustPositive(angle);

	ocean_assert(isValid());
}

template <typename T>
RotationT<T> RotationT<T>::inverted() const
{
	ocean_assert(isValid());
	return RotationT<T>(-values_[0], -values_[1], -values_[2], values_[3]);
}

template <typename T>
void RotationT<T>::invert()
{
	ocean_assert(isValid());

	values_[0] = -values_[0];
	values_[1] = -values_[1];
	values_[2] = -values_[2];
}

template <typename T>
bool RotationT<T>::isValid() const
{
	return axis().isUnit() && NumericT<T>::isInsideRange(T(0.0), angle(), NumericT<T>::pi2());
}

template <typename T>
inline bool RotationT<T>::isEqual(const RotationT<T>& rotation, const T eps) const
{
	ocean_assert(isValid() && rotation.isValid());
	ocean_assert(eps >= T(0.0));

	return (NumericT<T>::isEqual(values_[0], rotation.values_[0], eps) && NumericT<T>::isEqual(values_[1], rotation.values_[1], eps) && NumericT<T>::isEqual(values_[2], rotation.values_[2], eps) && NumericT<T>::isEqual(values_[3], rotation.values_[3], eps))
		|| (NumericT<T>::isEqual(values_[0], -rotation.values_[0], eps) && NumericT<T>::isEqual(values_[1], -rotation.values_[1], eps) && NumericT<T>::isEqual(values_[2], -rotation.values_[2], eps) && NumericT<T>::angleIsEqual(values_[3] + rotation.values_[3], NumericT<T>::pi2(), eps));
}

template <typename T>
bool RotationT<T>::operator==(const RotationT<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return isEqual(right);
}

template <typename T>
RotationT<T> RotationT<T>::operator-() const
{
	ocean_assert(isValid());

	return RotationT<T>(-values_[0], -values_[1], -values_[2], values_[3]);
}

template <typename T>
RotationT<T> RotationT<T>::operator*(const QuaternionT<T>& quaternion) const
{
	ocean_assert(isValid() && quaternion.isValid());

	const QuaternionT<T> result(QuaternionT<T>(*this) * quaternion);

	return RotationT<T>(result.normalized());
}

template <typename T>
RotationT<T>& RotationT<T>::operator*=(const QuaternionT<T>& quaternion)
{
	ocean_assert(isValid() && quaternion.isValid());

	const QuaternionT<T> result(QuaternionT<T>(*this) * quaternion);

	*this = RotationT<T>(result.normalized());

	return *this;
}

template <typename T>
RotationT<T> RotationT<T>::operator*(const RotationT<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	const QuaternionT<T> result(QuaternionT<T>(*this) * QuaternionT<T>(right));

	return RotationT<T>(result.normalized());
}

template <typename T>
RotationT<T>& RotationT<T>::operator*=(const RotationT<T>& right)
{
	ocean_assert(isValid() && right.isValid());

	const QuaternionT<T> result(QuaternionT<T>(*this) * QuaternionT<T>(right));

	*this = RotationT<T>(result.normalized());

	return *this;
}

template <typename T>
VectorT3<T> RotationT<T>::operator*(const VectorT3<T>& vector) const
{
	ocean_assert(isValid());

	return QuaternionT<T>(*this) * vector;
}

template <typename T>
inline const T* RotationT<T>::data() const
{
	return values_;
}

template <typename T>
inline T* RotationT<T>::data()
{
	return values_;
}

template <typename T>
inline VectorT3<T> RotationT<T>::axis() const
{
	return VectorT3<T>(values_);
}

template <typename T>
inline T RotationT<T>::angle() const
{
	return values_[3];
}

template <typename T>
inline bool RotationT<T>::operator!=(const RotationT& right) const
{
	return !(*this == right);
}

template <typename T>
inline T RotationT<T>::operator()(unsigned int index) const
{
	ocean_assert(index < 4u);

	return values_[index];
}

template <typename T>
inline T& RotationT<T>::operator()(unsigned int index)
{
	ocean_assert(index < 4u);

	return values_[index];
}

template <typename T>
inline T RotationT<T>::operator[](unsigned int index) const
{
	ocean_assert(index < 4u);

	return values_[index];
}

template <typename T>
inline T& RotationT<T>::operator[](unsigned int index)
{
	ocean_assert(index < 4u);

	return values_[index];
}

template <typename T>
inline const T* RotationT<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* RotationT<T>::operator()()
{
	return values_;
}

template <typename T>
RotationT<T> RotationT<T>::left_R_right(const VectorT3<T>& left, const VectorT3<T>& right)
{
	ocean_assert(left.isUnit(NumericT<T>::weakEps()));
	ocean_assert(right.isUnit(NumericT<T>::weakEps()));

	if (left == right)
	{
		// identity

		return RotationT<T>(VectorT3<T>(0, 1, 0), T(0));
	}
	else if (left == -right)
	{
		const VectorT3<T> perpendicular(right.perpendicular().normalized());

		return RotationT<T>(perpendicular, NumericT<T>::pi());
	}

	const VectorT3<T> axis(right.cross(left).normalized());
	const T angle = right.angle(left);

	const RotationT<T> result(axis, angle);
	ocean_assert(result.isValid());

	return result;
}

}

#endif // META_OCEAN_MATH_ROTATION_H
