/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_QUATERNION_H
#define META_OCEAN_MATH_QUATERNION_H

#include "ocean/math/Math.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class RotationT;

// Forward declaration.
template <typename T> class EulerT;

// Forward declaration.
template <typename T> class SquareMatrixT3;

// Forward declaration.
template <typename T> class HomogenousMatrixT4;

// Forward declaration.
template <typename T> class QuaternionT;

/**
 * Definition of the Quaternion object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see QuaternionT
 * @ingroup math
 */
typedef QuaternionT<Scalar> Quaternion;

/**
 * Instantiation of the QuaternionT template class using a double precision float data type.
 * @see QuaternionT
 * @ingroup math
 */
typedef QuaternionT<double> QuaternionD;

/**
 * Instantiation of the QuaternionT template class using a single precision float data type.
 * @see QuaternionT
 * @ingroup math
 */
typedef QuaternionT<float> QuaternionF;

/**
 * Definition of a typename alias for vectors with QuaternionT objects.
 * @see QuaternionT
 * @ingroup math
 */
template <typename T>
using QuaternionsT = std::vector<QuaternionT<T>>;

/**
 * Definition of a vector holding quaternion objects.
 * @see Quaternion
 * @ingroup math
 */
typedef std::vector<Quaternion> Quaternions;

/**
 * Definition of a vector holding quaternion objects with single precision float data type.
 * @see Quaternion
 * @ingroup math
 */
typedef std::vector<QuaternionF> QuaternionsF;

/**
 * Definition of a vector holding quaternion objects with double precision float data type.
 * @see Quaternion
 * @ingroup math
 */
typedef std::vector<QuaternionD> QuaternionsD;

/**
 * This class implements a unit quaternion rotation.
 * A quaternion is defined by four values:
 * <pre>
 * w + xi + yj + zk
 * with
 * i * i = -1, j * j = -1, k * k = -1
 * </pre>
 * The elements are stored in the following order: w, x, y, z
 * @tparam T Data type used to represent the quaternion values
 * @see Quaternion, QuaternionF, QuaternionD, Rotation, Euler, SquareMatrix3, ExponentialMap.
 * @ingroup math
 */
template <typename T>
class QuaternionT
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new quaternion with default values, representation an identity rotation.
		 */
		QuaternionT() = default;

		/**
		 * Creates a new quaternion with either default values (representation an identity rotation) or an invalid rotation.
		 * @param toIdentity True, to create an identity rotation; False, to created an invalid rotation
		 */
		explicit QuaternionT(const bool toIdentity);

		/**
		 * Copies a quaternion with different element data type than T.
		 * @param quaternion The quaternion object to be copied
		 * @tparam U The element data type of the given quaternion
		 */
		template <typename U>
		explicit inline QuaternionT(const QuaternionT<U>& quaternion);

		/**
		 * Creates a new quaternion by four given values.
		 * @param w W value of the quaternion, with range (-infinity, infinity)
		 * @param x X value of the quaternion, with range (-infinity, infinity)
		 * @param y Y value of the quaternion, with range (-infinity, infinity)
		 * @param z Z value of the quaternion, with range (-infinity, infinity)
		 */
		QuaternionT(const T w, const T x, const T y, const T z);

		/**
		 * Creates a new quaternion by a given axis and rotation angle.
		 * @param axis The axis of the angle-axis rotation with unit length
		 * @param angle The angle of the angle-axis rotation in radian, with range (-infinity, infinity), however will be converted to the range [0.0, 2 * PI)
		 */
		QuaternionT(const VectorT3<T>& axis, const T angle);

		/**
		 * Creates a quaternion object based on two given unit vectors.
		 * The resulting rotation defines a transformation that rotates that reference vector into the offset vector: Quaternion(reference, offset) = offset_Q_reference.<br>
		 * The following equation holds:
		 * <pre>
		 * offset = Quaternion(reference, offset) * reference.
		 * </pre>
		 * @param reference The reference vector, with length 1
		 * @param offset The offset vector, with length 1
		 */
		QuaternionT(const VectorT3<T>& reference, const VectorT3<T>& offset);

		/**
		 * Creates a new quaternion by a given angle-axis rotation.
		 * @param rotation The rotation to create the quaternion from, must be valid
		 */
		explicit QuaternionT(const RotationT<T>& rotation);

		/**
		 * Creates a new quaternion by a given Euler rotation.
		 * @param euler Rotation to create the quaternion from, must be valid
		 */
		explicit QuaternionT(const EulerT<T>& euler);

		/**
		 * Creates a new quaternion by a given 3x3 rotation matrix.
		 * @param matrix Rotation matrix to create the quaternion from, with determinant 1
		 */
		explicit QuaternionT(const SquareMatrixT3<T>& matrix);

		/**
		 * Creates a new quaternion by a given 4x4 homogeneous transformation matrix.
		 * @param transformation The transformation matrix to create the quaternion from
		 */
		explicit QuaternionT(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a new quaternion by a 4D vector.
		 * @param vector The vector holding the four quaternion parameter
		 */
		explicit QuaternionT(const VectorT4<T>& vector);

		/**
		 * Creates a new quaternion by an array with at least four elements.
		 * @param arrayValue Array with elements, must be valid
		 */
		explicit QuaternionT(const T* arrayValue);

		/**
		 * Returns the normalized quaternion.
		 * Beware: This function does not throw an exception if the quaternion is singular and cannot be normalized.<br>
		 * Thus ensure that the quaternion is valid before calling this function.<br>
		 * In case, this quaternion is not valid, a default quaternion will be returned.
		 * @return Normalized quaternion
		 * @see isValid(), normalize().
		 */
		QuaternionT<T> normalized() const;

		/**
		 * Normalizes the quaternion in place.
		 * @return True, if the quaternion is not singular and could be normalized.
		 * @see normalized().
		 */
		bool normalize();

		/**
		 * Normalizes the quaternion and returns the result as parameter.
		 * @param normalizedQuaternion The resulting normalized quaternion
		 * @return True, if the quaternion is not singular and could be normalized.
		 * @see normalized().
		 */
		bool normalize(QuaternionT<T>& normalizedQuaternion) const;

		/**
		 * Returns the inverted quaternion.
		 * Beware: This function does not throw an exception if the quaternion is singular and cannot be inverted.<br>
		 * Thus ensure that the quaternion is valid before calling this function.<br>
		 * In case, this quaternion is not valid, a default quaternion will be returned.
		 * @return Inverted quaternion
		 * @see isValid(), invert().
		 */
		QuaternionT<T> inverted() const;

		/**
		 * Inverts this quaternion in place.
		 * @return True, if the quaternion is not singular and could be inverted.
		 * @see inverted().
		 */
		bool invert();

		/**
		 * Inverts this quaternion and returns the result as parameter.
		 * @param invertedQuaternion The resulting inverted quaternion
		 * @return True, if the quaternion is not singular and could be inverted.
		 * @see inverted().
		 */
		bool invert(QuaternionT<T>& invertedQuaternion) const;

		/**
		 * Returns the conjugate of this quaternion.
		 * The conjugated quaternion is defined as: (w - xi - yj - zk)
		 * @return Conjugate quaternion
		 */
		QuaternionT<T> conjugate() const;

		/**
		 * Returns the norm of this quaternion.
		 * @return Quaternion norm
		 */
		T norm() const;

		/**
		 * Returns the square of the quaternion norm.
		 * @return Square of quaternion norm
		 */
		T sqr() const;

		/**
		 * Returns the dot product between this quaternion and a second quaternion.
		 * The function actually returns: w0 * w1 + x0 * x1 + y0 * y1 + z0 * z1, with quaternions (w0, x0, y0, z0) and (w1, x1, y1, z1).
		 * @param quaternion The second quaternion for dot product calculation
		 * @return The dot product between both quaternions
		 */
		T dot(const QuaternionT<T>& quaternion) const;

		/**
		 * Returns the rotation angle defined by the quaternion.
		 * This angle is calculated based on the real part of the quaternion.<br>
		 * Beware: Ensure that this quaternion is valid.
		 * @return Rotation angle in radian, with range [0, 2PI), 0 in case of an invalid quaternion
		 */
		T angle() const;

		/**
		 * Returns the angle between two quaternion rotations.
		 * This function may return angles larger than PI.
		 * Beware: Ensure that this quaternion is valid.
		 * @param quaternion Second quaternion for angle determination, must be valid
		 * @return Rotation angle in radian, with range [0, 2PI), 0 in case of an invalid quaternion
		 * @see smallestAngle().
		 */
		T angle(const QuaternionT<T>& quaternion) const;

		/**
		 * Returns the smallest angle between two quaternion rotations.
		 * In contrast to angle(), this function returns the smallest (absolute) angle with range [0, PI).
		 * Beware: Ensure that this quaternion is valid.
		 * @param quaternion Second quaternion for angle determination, must be valid
		 * @return Rotation angle in radian, with range [0, PI), 0 in case of an invalid quaternion
		 * @see angle().
		 */
		T smallestAngle(const QuaternionT<T>& quaternion) const;

		/**
		 * Returns the cosine value of the half angle between two quaternion rotations.
		 * To determine whether two quaternion rotations have a lesser angle offset than 30 degrees<br>
		 * this cos2 value must be larger than the cosine of 15 degrees.<br>
		 * Beware: Ensure that this quaternion is valid.
		 * @param quaternion Second quaternion for value determination, must be valid
		 * @return Cosine value of the half angle
		 */
		T cos2(const QuaternionT<T>& quaternion) const;

		/**
		 * Spherical linear interpolation between two quaternions.
		 * This quaternion will be the resulting interpolation if 'factor == 0'.
		 * @param quaternion The right quaternion which will be the resulting interpolation if 'factor == 1'
		 * @param factor Interpolation factor has to be between [0, 1]
		 * @return The interpolated quaternion
		 */
		QuaternionT<T> slerp(const QuaternionT<T>& quaternion, T factor) const;

		/**
		 * Returns whether this quaternion is a valid unit quaternion.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns the w value of the quaternion.
		 * @return w value
		 */
		inline const T& w() const;

		/**
		 * Returns the w value of the quaternion.
		 * @return w value
		 */
		inline T& w();

		/**
		 * Returns the x value of the quaternion.
		 * @return x value
		 */
		inline const T& x() const;

		/**
		 * Returns the x value of the quaternion.
		 * @return x value
		 */
		inline T& x();

		/**
		 * Returns the y value of the quaternion.
		 * @return x value
		 */
		inline const T& y() const;

		/**
		 * Returns the y value of the quaternion.
		 * @return x value
		 */
		inline T& y();

		/**
		 * Returns the z value of the quaternion.
		 * @return z value
		 */
		inline const T& z() const;

		/**
		 * Returns the z value of the quaternion.
		 * @return z value
		 */
		inline T& z();

		/**
		 * Returns whether two quaternions are equal up to a specified epsilon.
		 * @param quaternion Second quaternion to compare
		 * @param eps The epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const QuaternionT<T>& quaternion, const T eps) const;

		/**
		 * Returns whether two quaternions are identical up to a small epsilon.
		 * @param right The right quaternion
		 * @return True, if so
		 */
		bool operator==(const QuaternionT<T>& right) const;

		/**
		 * Returns whether two quaternions are not identical up to a small epsilon.
		 * @param right The right quaternion
		 * @return True, if so
		 */
		inline bool operator!=(const QuaternionT<T>& right) const;

		/**
		 * Combines two quaternion to a new combined rotation.
		 * @param right The right quaternion
		 * @return Combined quaternion
		 */
		QuaternionT<T> operator*(const QuaternionT<T>& right) const;

		/**
		 * Combines a angle-axis rotation with this quaternion.
		 * @param right The right angle-axis rotation
		 * @return Combined quaternion rotation
		 */
		inline QuaternionT<T> operator*(const RotationT<T>& right) const;

		/**
		 * Rotates a 3D vector by this quaternion.
		 * @param vector 3D vector to rotate
		 * @return Rotated 3D vector
		 */
		VectorT3<T> operator*(const VectorT3<T>& vector) const;

		/**
		 * Combines and assigns two quaternions.
		 * @param right The right quaternion
		 * @return Reference to this quaternion
		 */
		inline QuaternionT<T>& operator*=(const QuaternionT<T>& right);

		/**
		 * Combines and assigns a quaternion with a rotation.
		 * @param right The right quaternion
		 * @return Reference to this quaternion
		 */
		inline QuaternionT<T>& operator*=(const RotationT<T>& right);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T operator[](const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int index);

		/**
		 * Access operator.
		 * @return Pointer to the internal values
		 */
		inline const T* operator()() const;

		/**
		 * Access operator.
		 * @return Pointer to the internal values
		 */
		inline T* operator()();

	protected:

		/// The four values of the quaternion.
		T values_[4] = {T(1), T(0), T(0), T(0)};
};

template <typename T>
QuaternionT<T>::QuaternionT(const bool toIdentity)
{
	if (toIdentity)
	{
		values_[0] = T(1);
		values_[1] = T(0);
		values_[2] = T(0);
		values_[3] = T(0);

		ocean_assert(isValid() == true);
	}
	else
	{
		values_[0] = T(0);
		values_[1] = T(0);
		values_[2] = T(0);
		values_[3] = T(0);

		ocean_assert(isValid() == false);
	}
}

template <typename T>
template <typename U>
inline QuaternionT<T>::QuaternionT(const QuaternionT<U>& quaternion)
{
	values_[0] = T(quaternion[0]);
	values_[1] = T(quaternion[1]);
	values_[2] = T(quaternion[2]);
	values_[3] = T(quaternion[3]);
}

template <typename T>
QuaternionT<T>::QuaternionT(const T w, const T x, const T y, const T z)
{
	values_[0] = w;
	values_[1] = x;
	values_[2] = y;
	values_[3] = z;
}

template <typename T>
QuaternionT<T>::QuaternionT(const VectorT3<T>& axis, const T angle)
{
	ocean_assert_accuracy(axis.isUnit(NumericT<T>::weakEps()));

	const T angleValue = angle * T(0.5);
	const T sinValue = NumericT<T>::sin(angleValue);

	values_[0] = NumericT<T>::cos(angleValue);
	values_[1] = sinValue * axis[0];
	values_[2] = sinValue * axis[1];
	values_[3] = sinValue * axis[2];

	ocean_assert_accuracy(isValid());
}

template <typename T>
QuaternionT<T>::QuaternionT(const VectorT3<T>& reference, const VectorT3<T>& offset)
{
	ocean_assert_accuracy(reference.isUnit(NumericT<T>::weakEps()));
	ocean_assert_accuracy(offset.isUnit(NumericT<T>::weakEps()));

	if (reference == offset)
	{
		values_[0] = T(1);
		values_[1] = T(0);
		values_[2] = T(0);
		values_[3] = T(0);
	}
	else if (reference == -offset)
	{
		const VectorT3<T> perpendicular(reference.perpendicular().normalized());

		values_[0] = T(0);
		values_[1] = perpendicular[0];
		values_[2] = perpendicular[1];
		values_[3] = perpendicular[2];
	}
	else
	{
		const VectorT3<T> axis(reference.cross(offset));

		values_[0] = T(1) + reference * offset;
		values_[1] = axis.x();
		values_[2] = axis.y();
		values_[3] = axis.z();

		normalize();
	}

	ocean_assert(isValid());
	ocean_assert_accuracy(offset.isEqual(*this * reference, NumericT<T>::weakEps()));
}

template <typename T>
QuaternionT<T>::QuaternionT(const RotationT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	const T angle = rotation.angle() * T(0.5);
	const T sinValue = NumericT<T>::sin(angle);

	values_[0] = NumericT<T>::cos(angle);
	values_[1] = sinValue * rotation[0];
	values_[2] = sinValue * rotation[1];
	values_[3] = sinValue * rotation[2];

	ocean_assert(isValid());
}

template <typename T>
QuaternionT<T>::QuaternionT(const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	const T roll = euler.roll() * T(0.5);
	const T pitch = euler.pitch() * T(0.5);
	const T yaw = euler.yaw() * T(0.5);

	const T cosRoll = NumericT<T>::cos(roll);
	const T sinRoll = NumericT<T>::sin(roll);

	const T cosPitch = NumericT<T>::cos(pitch);
	const T sinPitch = NumericT<T>::sin(pitch);

	const T cosYaw = NumericT<T>::cos(yaw);
	const T sinYaw = NumericT<T>::sin(yaw);

	const T cc = cosRoll * cosYaw;
	const T cs = cosRoll * sinYaw;

	const T sc = sinRoll * cosYaw;
	const T ss = sinRoll * sinYaw;

	values_[0] = cosPitch * cc + sinPitch * ss;
	values_[1] = cosPitch * ss + sinPitch * cc;
	values_[2] = cosPitch * cs - sinPitch * sc;
	values_[3] = cosPitch * sc - sinPitch * cs;

	normalize();
	ocean_assert(isValid());
}

template <typename T>
QuaternionT<T>::QuaternionT(const SquareMatrixT3<T>& matrix)
{
	ocean_assert_accuracy(NumericT<T>::isWeakEqual(matrix.determinant(), T(1.0)));
	const T trace = matrix.trace() + T(1.0);

	if (trace > T(2.0) * NumericT<T>::eps() * NumericT<T>::eps())
	{
		values_[0] = T(0.5) * NumericT<T>::sqrt(trace);
		ocean_assert_accuracy(NumericT<T>::isNotEqualEps(values_[0]));

		const T factor = T(0.25) / values_[0];

		values_[1] = (matrix(2, 1) - matrix(1, 2)) * factor;
		values_[2] = (matrix(0, 2) - matrix(2, 0)) * factor;
		values_[3] = (matrix(1, 0) - matrix(0, 1)) * factor;
	}
	else
	{
		if (matrix(0, 0) > matrix(1, 1) && matrix(0, 0) > matrix(2, 2))
		{
			values_[1] = T(0.5) * NumericT<T>::sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + T(1.0));
			ocean_assert_accuracy(NumericT<T>::isNotEqualEps(values_[1]));
			const T factor = T(0.25) / values_[1];

			values_[0] = (matrix(2, 1) - matrix(1, 2)) * factor;
			values_[2] = (matrix(0, 1) + matrix(1, 0)) * factor;
			values_[3] = (matrix(0, 2) + matrix(2, 0)) * factor;
		}
		else if (matrix(1, 1) > matrix(2, 2))
		{
			values_[2] = T(0.5) * NumericT<T>::sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + T(1.0));
			ocean_assert_accuracy(NumericT<T>::isNotEqualEps(values_[2]));
			const T factor = T(0.25) / values_[2];

			values_[0] = (matrix(0, 2) - matrix(2, 0)) * factor;
			values_[1] = (matrix(0, 1) + matrix(1, 0)) * factor;
			values_[3] = (matrix(1, 2) + matrix(2, 1)) * factor;
		}
		else
		{
			values_[3] = T(0.5) * NumericT<T>::sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + T(1.0));
			ocean_assert_accuracy(NumericT<T>::isNotEqualEps(values_[3]));
			const T factor = T(0.25) / values_[3];

			values_[0] = (matrix(1, 0) - matrix(0, 1)) * factor;
			values_[1] = (matrix(0, 2) + matrix(2, 0)) * factor;
			values_[2] = (matrix(1, 2) + matrix(2, 1)) * factor;
		}
	}

	normalize();
	ocean_assert(isValid());
}

template <typename T>
QuaternionT<T>::QuaternionT(const HomogenousMatrixT4<T>& transformation)
{
	*this = QuaternionT<T>(transformation.orthonormalRotationMatrix());
	ocean_assert(isValid());
}

template <typename T>
QuaternionT<T>::QuaternionT(const VectorT4<T>& vector)
{
	memcpy(values_, vector(), sizeof(T) * 4);
}

template <typename T>
QuaternionT<T>::QuaternionT(const T* arrayValue)
{
	ocean_assert(arrayValue);
	memcpy(values_, arrayValue, sizeof(T) * 4);
}

template <typename T>
QuaternionT<T> QuaternionT<T>::normalized() const
{
	const T normValue = norm();

	if (NumericT<T>::isEqualEps(normValue))
	{
		return QuaternionT<T>(true);
	}

	const T factor = T(1.0) / normValue;

	return QuaternionT<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
}

template <typename T>
bool QuaternionT<T>::normalize()
{
	const T normValue = norm();

	if (NumericT<T>::isEqualEps(normValue))
	{
		return false;
	}

	const T factor = T(1.0) / normValue;

	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;
	values_[3] *= factor;

	return true;
}

template <typename T>
bool QuaternionT<T>::normalize(QuaternionT<T>& normalizedQuaternion) const
{
	const T normValue = norm();

	if (NumericT<T>::isEqualEps(normValue))
	{
		return false;
	}

	const T factor = T(1.0) / normValue;

	normalizedQuaternion.values_[0] = values_[0] * factor;
	normalizedQuaternion.values_[1] = values_[1] * factor;
	normalizedQuaternion.values_[2] = values_[2] * factor;
	normalizedQuaternion.values_[3] = values_[3] * factor;

	return true;
}

template <typename T>
QuaternionT<T> QuaternionT<T>::inverted() const
{
	const T square = sqr();

	if (NumericT<T>::isEqualEps(square))
	{
		return QuaternionT<T>(true);
	}

	const T factor = T(1) / square;

	return QuaternionT<T>(values_[0] * factor, -values_[1] * factor, -values_[2] * factor, -values_[3] * factor);
}

template <typename T>
bool QuaternionT<T>::invert()
{
	const T square = sqr();

	if (NumericT<T>::isEqualEps(square))
	{
		return false;
	}

	const T factor = T(1.0) / square;

	values_[0] *= factor;
	values_[1] *= -factor;
	values_[2] *= -factor;
	values_[3] *= -factor;

	return true;
}

template <typename T>
bool QuaternionT<T>::invert(QuaternionT<T>& invertedQuaternion) const
{
	const T square = sqr();

	if (NumericT<T>::isEqualEps(square))
	{
		return false;
	}

	const T factor = T(1.0) / square;

	invertedQuaternion.values_[0] = values_[0] * factor;
	invertedQuaternion.values_[1] = values_[1] * -factor;
	invertedQuaternion.values_[2] = values_[2] * -factor;
	invertedQuaternion.values_[3] = values_[3] * -factor;

	return true;
}

template <typename T>
QuaternionT<T> QuaternionT<T>::conjugate() const
{
	return QuaternionT<T>(values_[0], -values_[1], -values_[2], -values_[3]);
}

template <typename T>
T QuaternionT<T>::norm() const
{
	return NumericT<T>::sqrt(values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2] + values_[3] * values_[3]);
}

template <typename T>
T QuaternionT<T>::sqr() const
{
	return values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2] + values_[3] * values_[3];
}

template <typename T>
T QuaternionT<T>::dot(const QuaternionT<T>& quaternion) const
{
	return values_[0] * quaternion.values_[0] + values_[1] * quaternion.values_[1] + values_[2] * quaternion.values_[2] + values_[3] * quaternion.values_[3];
}

template <typename T>
T QuaternionT<T>::angle() const
{
	ocean_assert(isValid());

	return NumericT<T>::angleAdjustPositive(T(2.0) * NumericT<T>::acos(w()));
}

template <typename T>
T QuaternionT<T>::angle(const QuaternionT<T>& quaternion) const
{
	ocean_assert(isValid() && quaternion.isValid());

	return (inverted() * quaternion).angle();
}

template <typename T>
T QuaternionT<T>::smallestAngle(const QuaternionT<T>& quaternion) const
{
	ocean_assert(isValid() && quaternion.isValid());

	return NumericT<T>::abs(NumericT<T>::angleAdjustNull(angle(quaternion)));
}

template <typename T>
T QuaternionT<T>::cos2(const QuaternionT<T>& quaternion) const
{
	ocean_assert(isValid() && quaternion.isValid());

	return (inverted() * quaternion).w();
}

template <typename T>
QuaternionT<T> QuaternionT<T>::slerp(const QuaternionT<T>& quaternion, T factor) const
{
	ocean_assert(factor >= 0 && factor <= T(1.0));
	ocean_assert(isValid() && quaternion.isValid());

	T sigma = minmax(T(-1), w() * quaternion.w() + x() * quaternion.x() + y() * quaternion.y() + z() * quaternion.z(), T(1));

	QuaternionT<T> adjustedQuaternion(quaternion);

	if (sigma < 0)
	{
		sigma = -sigma;
		adjustedQuaternion = QuaternionT<T>(-quaternion.w(), -quaternion.x(), -quaternion.y(), -quaternion.z());
	}

	const T angle = NumericT<T>::acos(sigma);

	T factorA = T(1.0) - factor;
	T factorB = factor;

	if (NumericT<T>::abs(angle) > T(0.05))
	{
		ocean_assert_accuracy(NumericT<T>::isNotEqualEps(NumericT<T>::sin(angle)));

		const T sinAngleValue = T(1.0) / NumericT<T>::sin(angle);

		factorA = NumericT<T>::sin((T(1.0) - factor) * angle) * sinAngleValue;
		factorB = NumericT<T>::sin(factor * angle) * sinAngleValue;
	}

	return QuaternionT<T>(factorA * w() + factorB * adjustedQuaternion.w(), factorA * x() + factorB * adjustedQuaternion.x(), factorA * y() + factorB * adjustedQuaternion.y(), factorA * z() + factorB * adjustedQuaternion.z()).normalized();
}

template <typename T>
bool QuaternionT<T>::isValid() const
{
	return NumericT<T>::isWeakEqual(norm(), T(1.0));
}

template <typename T>
inline const T& QuaternionT<T>::w() const
{
	return values_[0];
}

template <typename T>
inline T& QuaternionT<T>::w()
{
	return values_[0];
}

template <typename T>
inline const T& QuaternionT<T>::x() const
{
	return values_[1];
}

template <typename T>
inline T& QuaternionT<T>::x()
{
	return values_[1];
}

template <typename T>
inline const T& QuaternionT<T>::y() const
{
	return values_[2];
}

template <typename T>
inline T& QuaternionT<T>::y()
{
	return values_[2];
}

template <typename T>
inline const T& QuaternionT<T>::z() const
{
	return values_[3];
}

template <typename T>
inline T& QuaternionT<T>::z()
{
	return values_[3];
}

template <typename T>
inline bool QuaternionT<T>::isEqual(const QuaternionT<T>& quaternion, const T eps) const
{
	return (NumericT<T>::isEqual(values_[0], quaternion.values_[0], eps) && NumericT<T>::isEqual(values_[1], quaternion.values_[1], eps) && NumericT<T>::isEqual(values_[2], quaternion.values_[2], eps) && NumericT<T>::isEqual(values_[3], quaternion.values_[3], eps))
				|| (NumericT<T>::isEqual(values_[0], -quaternion.values_[0], eps) && NumericT<T>::isEqual(values_[1], -quaternion.values_[1], eps) && NumericT<T>::isEqual(values_[2], -quaternion.values_[2], eps) && NumericT<T>::isEqual(values_[3], -quaternion.values_[3], eps));
}

template <typename T>
bool QuaternionT<T>::operator==(const QuaternionT<T>& right) const
{
	return isEqual(right, NumericT<T>::eps());
}

template <typename T>
QuaternionT<T> QuaternionT<T>::operator*(const QuaternionT<T>& right) const
{
	return QuaternionT(values_[0] * right.values_[0] - values_[1] * right.values_[1] - values_[2] * right.values_[2] - values_[3] * right.values_[3],
						values_[0] * right.values_[1] + values_[1] * right.values_[0] + values_[2] * right.values_[3] - values_[3] * right.values_[2],
						values_[0] * right.values_[2] - values_[1] * right.values_[3] + values_[2] * right.values_[0] + values_[3] * right.values_[1],
						values_[0] * right.values_[3] + values_[1] * right.values_[2] -  values_[2] * right.values_[1] + values_[3] * right.values_[0]);
}

template <typename T>
VectorT3<T> QuaternionT<T>::operator*(const VectorT3<T>& vector) const
{
	ocean_assert(isValid());

	const QuaternionT<T> quaternion(0, vector[0], vector[1], vector[2]);
	const QuaternionT<T> result(*this * quaternion * inverted());

	return VectorT3<T>(result.values_[1], result.values_[2], result.values_[3]);
}

template <typename T>
inline bool QuaternionT<T>::operator!=(const QuaternionT<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline QuaternionT<T> QuaternionT<T>::operator*(const RotationT<T>& right) const
{
	return *this * QuaternionT(right);
}

template <typename T>
inline QuaternionT<T>& QuaternionT<T>::operator*=(const QuaternionT<T>& right)
{
	*this = *this * right;
	return *this;
}

template <typename T>
inline QuaternionT<T>& QuaternionT<T>::operator*=(const RotationT<T>& right)
{
	*this = *this * QuaternionT(right);
	return *this;
}

template <typename T>
inline T QuaternionT<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline T& QuaternionT<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline T QuaternionT<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline T& QuaternionT<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline const T* QuaternionT<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* QuaternionT<T>::operator()()
{
	return values_;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const QuaternionT<T>& quaternion)
{
	stream << "[" << quaternion.w() << ", " << quaternion.x() << ", " << quaternion.y() << ", " << quaternion.z() << "]";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const QuaternionT<T>& quaternion)
{
	return messageObject << "[" << quaternion.w() << ", " << quaternion.x() << ", " << quaternion.y() << ", " << quaternion.z() << "]";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const QuaternionT<T>& quaternion)
{
	return messageObject << "[" << quaternion.w() << ", " << quaternion.x() << ", " << quaternion.y() << ", " << quaternion.z() << "]";
}

}

#endif // META_OCEAN_MATH_QUATERNION_H
