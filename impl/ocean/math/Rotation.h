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

#include "ocean/base/Exception.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class SquareMatrixT3;

// Forward declaration.
template <typename T> class HomogenousMatrixT4;

// Forward declaration.
template <typename T> class RotationT;

/**
 * Definition of the Rotation object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION flag either with single or double precision float data type.
 * @see RotationT
 * @ingroup math
 */
typedef RotationT<Scalar> Rotation;

/**
 * Instantiation of the RotationT template class using a double precision float data type.
 * @see RotationT
 * @ingroup math
 */
typedef RotationT<double> RotationD;

/**
 * Instantiation of the RotationT template class using a single precision float data type.
 * @see RotationT
 * @ingroup math
 */
typedef RotationT<float> RotationF;

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
typedef std::vector<Rotation> Rotations;

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
		typedef T Type;

	public:

		/**
		 * Creates a rotation object with default values so that the rotation represents the identity rotation.
		 * The axis will be set to (0, 1, 0) and the angle to 0.
		 */
		RotationT() = default;

		/**
		 * Creates a rotation object by four given values.
		 * The axis must have length 1.
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
		RotationT inverted() const;

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

	protected:

		/// The four values of the angle-axis rotation.
		T values[4] = {T(0), T(1), T(0), T(0)};
};

template <typename T>
RotationT<T>::RotationT(const T x, const T y, const T z, const T angle)
{
	values[0] = x;
	values[1] = y;
	values[2] = z;

	if (angle > NumericT<T>::pi2())
	{
		values[3] = NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else if (angle < T(0.0))
	{
		ocean_assert(NumericT<T>::fmod(angle, NumericT<T>::pi2()) < T(0.0));
		values[3] = NumericT<T>::pi2() + NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else
	{
		values[3] = angle;
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const VectorT3<T>& axis, const T angle)
{
	values[0] = axis[0];
	values[1] = axis[1];
	values[2] = axis[2];

	if (angle > NumericT<T>::pi2())
	{
		values[3] = NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else if (angle < T(0.0))
	{
		ocean_assert(NumericT<T>::fmod(angle, NumericT<T>::pi2()) < T(0.0));
		values[3] = NumericT<T>::pi2() + NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else
	{
		values[3] = angle;
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const VectorT3<T>& reference, const VectorT3<T>& offset)
{
	ocean_assert(NumericT<T>::isWeakEqual(reference.length(), 1));
	ocean_assert(NumericT<T>::isWeakEqual(offset.length(), 1));

	if (reference == offset)
	{
		values[0] = 0;
		values[1] = 1;
		values[2] = 0;
		values[3] = 0;
	}
	else if (reference == -offset)
	{
		const VectorT3<T> perpendicular(reference.perpendicular().normalized());

		values[0] = perpendicular[0];
		values[1] = perpendicular[1];
		values[2] = perpendicular[2];
		values[3] = NumericT<T>::pi();
	}
	else
	{
		const VectorT3<T> axis(reference.cross(offset).normalized());

		values[0] = axis.x();
		values[1] = axis.y();
		values[2] = axis.z();
		values[3] = reference.angle(offset);
	}

	ocean_assert(isValid());
	ocean_assert(offset.isEqual(*this * reference, NumericT<T>::weakEps()));
}

template <typename T>
RotationT<T>::RotationT(const QuaternionT<T>& quaternion)
{
	ocean_assert(quaternion.isValid());

	if (NumericT<T>::isEqual(NumericT<T>::abs(quaternion.w()), T(1.0)))
	{
		values[0] = T(0.0);
		values[1] = T(1.0);
		values[2] = T(0.0);
		values[3] = T(0.0);
	}
	else
	{
		ocean_assert(quaternion.w() < T(1.0));

		values[3] = T(2.0) * NumericT<T>::acos(quaternion.w());

		const T factor = T(1.0) / NumericT<T>::sqrt(T(1.0) - quaternion.w() * quaternion.w());

		VectorT3<T> axis(quaternion.x() * factor, quaternion.y() * factor, quaternion.z() * factor);
		axis.normalize();

		values[0] = axis[0];
		values[1] = axis[1];
		values[2] = axis[2];
	}

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
		values[0] = T(0.0);
		values[1] = T(1.0);
		values[2] = T(0.0);
		values[3] = T(0.0);
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
			}
			if (maximum < matrix(2, 2))
			{
				select = 2;
			}

			switch (select)
			{
				case 0:
				{
					axis(0) = T(0.5) * sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + T(1.0));
					T factor = T(0.5) / axis(0);

					axis(1) = matrix(0, 1) * factor;
					axis(2) = matrix(0, 2) * factor;
					break;
				}

				case 1:
				{
					axis(1) = T(0.5) * sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + T(1.0));
					T factor = T(0.5) / axis(1);

					axis(0) = matrix(0, 1) * factor;
					axis(2) = matrix(1, 2) * factor;
					break;
				}

				case 2:
				{
					axis(2) = T(0.5) * sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + T(1.0));
					T factor = T(0.5) / axis(2);

					axis(0) = matrix(0, 2) * factor;
					axis(1) = matrix(1, 2) * factor;
					break;
				}
			}

			values[3] = NumericT<T>::pi();
		}
		else
		{
			axis = VectorT3<T>(matrix(2, 1) - matrix(1, 2), matrix(0, 2) - matrix(2, 0), matrix(1, 0) - matrix(0, 1));
			values[3] = acos(cosValue);
		}

		axis.normalize();

		values[0] = axis(0);
		values[1] = axis(1);
		values[2] = axis(2);
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const HomogenousMatrixT4<T>& transformation)
{
	ocean_assert(transformation.isValid());

	const SquareMatrixT3<T> matrix(transformation.orthonormalRotationMatrix());
	ocean_assert(NumericT<T>::isEqual(matrix.determinant(), T(1.0)));

	const T cosValue = (matrix.trace() - T(1.0)) * T(0.5);
	ocean_assert(NumericT<T>::isInsideRange(T(-1.0), cosValue, T(1.0)));

	if (NumericT<T>::isEqual(cosValue, T(1.0)))
	{
		values[0] = T(0.0);
		values[1] = T(1.0);
		values[2] = T(0.0);
		values[3] = T(0.0);
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
			}

			values[3] = NumericT<T>::pi();
		}
		else
		{
			axis =VectorT3<T>(matrix(2, 1) - matrix(1, 2), matrix(0, 2) - matrix(2, 0), matrix(1, 0) - matrix(0, 1));
			values[3] = NumericT<T>::acos(cosValue);
		}

		axis.normalize();

		values[0] = axis(0);
		values[1] = axis(1);
		values[2] = axis(2);
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T>::RotationT(const T* valueArray)
{
	memcpy(values, valueArray, sizeof(T) * 4);

	ocean_assert(isValid());
}

template <typename T>
void RotationT<T>::setAxis(const VectorT3<T>& axis)
{
	ocean_assert(NumericT<T>::isEqual(axis.length(), T(1.0)));

	memcpy(values, axis(), sizeof(T) * 3);

	ocean_assert(isValid());
}

template <typename T>
void RotationT<T>::setAngle(const T angle)
{
	if (angle > NumericT<T>::pi2())
	{
		values[3] = NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else if (angle < T(0.0))
	{
		ocean_assert(NumericT<T>::fmod(angle, NumericT<T>::pi2()) < T(0.0));
		values[3] = NumericT<T>::pi2() + NumericT<T>::fmod(angle, NumericT<T>::pi2());
	}
	else
	{
		values[3] = angle;
	}

	ocean_assert(isValid());
}

template <typename T>
RotationT<T> RotationT<T>::inverted() const
{
	ocean_assert(isValid());
	return RotationT<T>(-values[0], -values[1], -values[2], values[3]);
}

template <typename T>
void RotationT<T>::invert()
{
	ocean_assert(isValid());
	values[0] = -values[0];
	values[1] = -values[1];
	values[2] = -values[2];
}

template <typename T>
bool RotationT<T>::isValid() const
{
	return NumericT<T>::isEqual(axis().length(), T(1.0)) && NumericT<T>::isInsideRange(T(0.0), angle(), NumericT<T>::pi2());
}

template <typename T>
bool RotationT<T>::operator==(const RotationT<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return (NumericT<T>::isEqual(values[0], right.values[0]) && NumericT<T>::isEqual(values[1], right.values[1])
		&& NumericT<T>::isEqual(values[2], right.values[2]) && NumericT<T>::isEqual(values[3], right.values[3]))
		|| (NumericT<T>::isEqual(values[0], -right.values[0]) && NumericT<T>::isEqual(values[1], -right.values[1])
		&& NumericT<T>::isEqual(values[2], -right.values[2]) && NumericT<T>::isEqual(values[3] + right.values[3], NumericT<T>::pi2()));
}

template <typename T>
RotationT<T> RotationT<T>::operator-() const
{
	ocean_assert(isValid());
	return RotationT<T>(-values[0], -values[1], -values[2], values[3]);
}

template <typename T>
RotationT<T> RotationT<T>::operator*(const QuaternionT<T>& quaternion) const
{
	ocean_assert(isValid() && quaternion.isValid());
	return RotationT<T>(QuaternionT<T>(*this) * quaternion);
}

template <typename T>
RotationT<T>& RotationT<T>::operator*=(const QuaternionT<T>& quaternion)
{
	ocean_assert(isValid() && quaternion.isValid());
	*this = RotationT<T>(QuaternionT<T>(*this) * quaternion);

	return *this;
}

template <typename T>
RotationT<T> RotationT<T>::operator*(const RotationT<T>& right) const
{
	ocean_assert(isValid() && right.isValid());
	return RotationT<T>(QuaternionT<T>(*this) * QuaternionT<T>(right));
}

template <typename T>
RotationT<T>& RotationT<T>::operator*=(const RotationT<T>& right)
{
	ocean_assert(isValid() && right.isValid());
	*this = RotationT<T>(QuaternionT<T>(*this) * QuaternionT<T>(right));

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
	return values;
}

template <typename T>
inline T* RotationT<T>::data()
{
	return values;
}

template <typename T>
inline VectorT3<T> RotationT<T>::axis() const
{
	return VectorT3<T>(values);
}

template <typename T>
inline T RotationT<T>::angle() const
{
	return values[3];
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
	return values[index];
}

template <typename T>
inline T& RotationT<T>::operator()(unsigned int index)
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline T RotationT<T>::operator[](unsigned int index) const
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline T& RotationT<T>::operator[](unsigned int index)
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline const T* RotationT<T>::operator()() const
{
	return values;
}

template <typename T>
inline T* RotationT<T>::operator()()
{
	return values;
}

}

#endif // META_OCEAN_MATH_ROTATION_H
