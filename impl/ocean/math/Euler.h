/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_EULER_H
#define META_OCEAN_MATH_EULER_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

// Forward declaration
template <typename T> class HomogenousMatrixT4;
// Forward declaration
template <typename T> class RotationT;
// Forward declaration
template <typename T> class QuaternionT;
// Forward declaration
template <typename T> class SquareMatrixT3;

// Forward declaration.
template <typename T> class EulerT;

/**
 * Definition of the Euler object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see EulerT
 * @ingroup math
 */
typedef EulerT<Scalar> Euler;

/**
 * Instantiation of the EulerT template class using a double precision float data type.
 * @see EulerT
 * @ingroup math
 */
typedef EulerT<double> EulerD;

/**
 * Instantiation of the EulerT template class using a single precision float data type.
 * @see EulerT
 * @ingroup math
 */
typedef EulerT<float> EulerF;

/**
 * Definition of a typename alias for vectors with EulerT objects.
 * @see EulerT
 * @ingroup math
 */
template <typename T>
using EulersT = std::vector<EulerT<T>>;

/**
 * Definition of a vector holding euler objects.
 * @see Euler
 * @ingroup math
 */
typedef std::vector<Euler> Eulers;

/**
 * This class implements an euler rotation with angles: yaw, pitch and roll.
 * The yaw angle is defined about the positive y-axis with range [-Pi, Pi].<br>
 * The pitch angle is defined about the positive x-axis with range [-Pi/2, Pi/2].<br>
 * The roll angle is defined about the positive z-axis with range [-Pi, Pi].<br>
 * The entire rotation can be written as matrix product: Ry(yaw) * Rx(pitch) * Rz(roll).<br>
 * All angles are defined in radian.
 * @tparam T Data type of angle values_
 * @see Euler, EulerF, EulerD, Rotation, Quaternion, SquareMatrix3, ExponentialMap.
 * @ingroup math
 */
template <typename T>
class EulerT
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new Euler rotation with all angles zero.
		 */
		EulerT();

		/**
		 * Creates a new euler rotation by given angles.
		 * @param yaw The yaw angle with range [-Pi, Pi]
		 * @param pitch The pitch angle with range [-Pi/2, Pi/2]
		 * @param roll The roll angle with range [-Pi, Pi]
		 */
		EulerT(const T yaw, const T pitch, const T roll);

		/**
		 * Creates a new euler rotation by an array of angles.
		 * @param arrayValue Array with three angles, with order yaw, pitch, roll, must be valid
		 */
		explicit EulerT(const T* arrayValue);

		/**
		 * Creates a new euler rotation by a given homogeneous transformation.
		 * @param transformation The homogeneous transformation to create a euler rotation from
		 */
		explicit EulerT(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a new euler rotation by a given angle-axis rotation.
		 * @param rotation Angle-axis rotation to create a euler rotation from
		 */
		explicit EulerT(const RotationT<T>& rotation);

		/**
		 * Creates a new euler rotation by a given quaternion rotation.
		 * @param quaternion The quaternion rotation to create a euler rotation from
		 */
		explicit EulerT(const QuaternionT<T>& quaternion);

		/**
		 * Creates a new euler rotation by a given 3x3 matrix rotation.
		 * @param matrix The rotation matrix to create a euler rotation from
		 */
		explicit EulerT(const SquareMatrixT3<T>& matrix);

		/**
		 * Returns the yaw angle.
		 * @return Yaw angle in radian
		 */
		inline const T& yaw() const;

		/**
		 * Returns the yaw angle.
		 * @return Yaw angle in radian
		 */
		inline T& yaw();

		/**
		 * Returns the pitch angle.
		 * @return Pitch angle in radian
		 */
		inline const T& pitch() const;

		/**
		 * Returns the pitch angle.
		 * @return Pitch angle in radian
		 */
		inline T& pitch();

		/**
		 * Returns the roll angle.
		 * @return Roll angle in radian
		 */
		inline const T& roll() const;

		/**
		 * Returns the roll angle.
		 * @return Roll angle in radian
		 */
		inline T& roll();

		/**
		 * Returns whether the euler rotation holds valid parameters.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether two euler rotations are identical up to a small epsilon.
		 * @param euler Right euler rotation
		 * @return True, if so
		 */
		bool operator==(const EulerT<T>& euler) const;

		/**
		 * Returns whether two euler rotations are not identical up to a small epsilon.
		 * @param euler Right euler rotation
		 * @return True, if so
		 */
		inline bool operator!=(const EulerT<T>& euler) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the euler rotation
		 */
		inline const T& operator[](const unsigned int index) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the euler rotation
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the euler rotation
		 */
		inline const T& operator()(const unsigned int index) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the euler rotation
		 */
		inline T& operator()(const unsigned int index);

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline const T* operator()() const;

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline T* operator()();

		/**
		 * Decomposes a 3x3 rotation matrix to the corresponding yaw, pitch and roll angles as defined by the euler class.
		 * The provided rotation matrix can be recreated by the matrix product: Ry(yAngle) * Rx(xAngle) * Rz(zAngle).
		 * @param matrix The 3x3 square matrix to decompose, must be an orthonormal rotation matrix
		 * @param yAngle The resulting (yaw) angle (rotation around the y-axis), with range [-PI, PI]
		 * @param xAngle The resulting (pitch) angle (rotation around the x-axis), with range [-PI/2, PI/2]
		 * @param zAngle The resulting (roll) angle (rotation around the z-axis), with range [-PI, PI]
		 */
		static void decomposeRotationMatrixToYXZ(const SquareMatrixT3<T>& matrix, T& yAngle, T& xAngle, T& zAngle);

		/**
		 * Decomposes a 3x3 rotation matrix to the corresponding x, y and z angles.
		 * The provided rotation matrix can be recreated by the matrix product: Rx(xAngle) * Ry(yAngle) * Rz(zAngle).
		 * @param matrix The 3x3 square matrix to decompose, must be an orthonormal rotation matrix
		 * @param xAngle The resulting x angle (rotation around the x-axis), with range [-PI, PI]
		 * @param yAngle The resulting y angle (rotation around the y-axis), with range [-PI/2, PI/2]
		 * @param zAngle The resulting z angle (rotation around the z-axis), with range [-PI, PI]
		 */
		static void decomposeRotationMatrixToXYZ(const SquareMatrixT3<T>& matrix, T& xAngle, T& yAngle, T& zAngle);

		/**
		 * Adjusts euler angles with arbitrary value (e.g., outside the valid value range) to euler angles within the value range of a valid Euler object.
		 * @param yaw The yaw angle to be adjusted, in radian, afterwards will be in the value range of [-PI, PI], with range (-infinity, infinity)
		 * @param pitch The pitch angle to be adjusted, in radian, afterwards will be in the value range of [-PI/2, PI/2], with range (-infinity, infinity)
		 * @param roll The roll angle to be adjusted, in radian, afterwards will be in the value range of [-PI, PI], with range (-infinity, infinity)
		 */
		static void adjustAngles(T& yaw, T& pitch, T& roll);

	protected:

		/// The three angles of the euler rotation.
		T values_[3];
};

template <typename T>
EulerT<T>::EulerT()
{
	values_[0] = T(0);
	values_[1] = T(0);
	values_[2] = T(0);
}

template <typename T>
EulerT<T>::EulerT(const T yaw, const T pitch, const T roll)
{
	values_[0] = yaw;
	values_[1] = pitch;
	values_[2] = roll;
}

template <typename T>
EulerT<T>::EulerT(const T* arrayValue)
{
	ocean_assert(arrayValue);
	memcpy(values_, arrayValue, sizeof(T) * 3);
}

template <typename T>
EulerT<T>::EulerT(const HomogenousMatrixT4<T>& transformation)
{
	decomposeRotationMatrixToYXZ(transformation.orthonormalRotationMatrix(), values_[0], values_[1], values_[2]);
	ocean_assert(isValid());
}

template <typename T>
EulerT<T>::EulerT(const RotationT<T>& rotation)
{
	decomposeRotationMatrixToYXZ(SquareMatrixT3<T>(rotation), values_[0], values_[1], values_[2]);
	ocean_assert(isValid());
}

template <typename T>
EulerT<T>::EulerT(const QuaternionT<T>& quaternion)
{
	decomposeRotationMatrixToYXZ(SquareMatrixT3<T>(quaternion), values_[0], values_[1], values_[2]);
	ocean_assert(isValid());
}

template <typename T>
EulerT<T>::EulerT(const SquareMatrixT3<T>& matrix)
{
	decomposeRotationMatrixToYXZ(matrix, values_[0], values_[1], values_[2]);
	ocean_assert(isValid());
}

template <typename T>
inline const T& EulerT<T>::yaw() const
{
	return values_[0];
}

template <typename T>
inline T& EulerT<T>::yaw()
{
	return values_[0];
}

template <typename T>
inline const T& EulerT<T>::pitch() const
{
	return values_[1];
}

template <typename T>
inline T& EulerT<T>::pitch()
{
	return values_[1];
}

template <typename T>
inline const T& EulerT<T>::roll() const
{
	return values_[2];
}

template <typename T>
inline T& EulerT<T>::roll()
{
	return values_[2];
}

template <typename T>
bool EulerT<T>::isValid() const
{
	return NumericT<T>::isInsideRange(-NumericT<T>::pi(), values_[0], NumericT<T>::pi())
			&& NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), values_[1], NumericT<T>::pi_2())
			&& NumericT<T>::isInsideRange(-NumericT<T>::pi(), values_[2], NumericT<T>::pi());
}

template <typename T>
bool EulerT<T>::operator==(const EulerT<T>& euler) const
{
	return NumericT<T>::isEqual(values_[0], euler.values_[0]) && NumericT<T>::isEqual(values_[1], euler.values_[1]) && NumericT<T>::isEqual(values_[2], euler.values_[2]);
}

template <typename T>
inline bool EulerT<T>::operator!=(const EulerT<T>& euler) const
{
	return !(*this == euler);
}

template <typename T>
inline const T& EulerT<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline T& EulerT<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline const T& EulerT<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline T& EulerT<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline const T* EulerT<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* EulerT<T>::operator()()
{
	return values_;
}

template <typename T>
void EulerT<T>::decomposeRotationMatrixToYXZ(const SquareMatrixT3<T>& matrix, T& yAngle, T& xAngle, T& zAngle)
{
	ocean_assert(matrix.isOrthonormal());

	/**
	 * Combined rotation matrix for R(y)R(x)R(z)
	 * [  cy cz + sx sy sz     cz sx sy - cy sz      cx sy  ]
	 * [       cx sz                 cx cz            -sx   ]
	 * [  -cz sy + cy sx sz    cy cz sx + sy sz      cx cy  ]
	 */

	if (matrix(1, 2) > T(-1) + NumericT<T>::eps())
	{
		if (matrix(1, 2) < T(1) - NumericT<T>::eps())
		{
			// we have the normal case without any extreme angles

			xAngle = NumericT<T>::asin(-matrix(1, 2));

			ocean_assert(NumericT<T>::isNotEqualEps(matrix(0, 2)) || NumericT<T>::isNotEqualEps(matrix(2, 2)));
			yAngle = NumericT<T>::atan2(matrix(0, 2), matrix(2, 2));

			ocean_assert(NumericT<T>::isNotEqualEps(matrix(1, 0)) || NumericT<T>::isNotEqualEps(matrix(1, 1)));
			zAngle = NumericT<T>::atan2(matrix(1, 0), matrix(1, 1));
		}
		else
		{
			// we have a special case where sx == -1
			ocean_assert((std::is_same<float, T>::value) ? NumericT<T>::isWeakEqual(matrix(1, 2), 1) : NumericT<T>::isEqual(matrix(1, 2), 1));

			/**
			 * Combined rotation matrix for R(y)R(x)R(z), with sx == -1 and cx == 0
			 * [  cy cz - sy sz         - cz sy - cy sz        0    ]     [ cos(y + z)      -sin(y + z)       0 ]
			 * [         0                     0              -1    ]  =  [      0               0           -1 ]
			 * [  -cz sy - cy sz        - cy cz + sy sz        0    ]     [ -sin(y + z)     -cos(y + z)       0 ]
			 */

			// tan(y + z) = sin(y + z) / cos(y + z), z == 0

			yAngle = NumericT<T>::atan2(-matrix(0, 1), -matrix(2, 1));
			xAngle = -NumericT<T>::pi_2();
			zAngle = T(0.0);
		}
	}
	else
	{
		// we have a special case where sx == 1
		ocean_assert((std::is_same<float, T>::value) ? NumericT<T>::isWeakEqual(matrix(1, 2), -1) : NumericT<T>::isEqual(matrix(1, 2), -1));

		/**
		 * Combined rotation matrix for R(y)R(x)R(z), with sx == 1 and cx == 0
		 * [  cy cz + sy sz           cz sy - cy sz        0    ]     [ cos(y - z)       sin(y - z)       0 ]
		 * [         0                     0              -1    ]  =  [      0               0           -1 ]
		 * [  -cz sy + cy sz          cy cz + sy sz        0    ]     [ sin(z - y)       cos(y - z)       0 ]
		 */

		// tan(y - z) = sin(y - z) / cos(y - z), z == 0

		yAngle = NumericT<T>::atan2(matrix(0, 1), matrix(2, 1));
		xAngle = NumericT<T>::pi_2();
		zAngle = T(0.0);
	}

	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), yAngle, NumericT<T>::pi()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), xAngle, NumericT<T>::pi_2()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), zAngle, NumericT<T>::pi()));
}

template <typename T>
void EulerT<T>::decomposeRotationMatrixToXYZ(const SquareMatrixT3<T>& matrix, T& xAngle, T& yAngle, T& zAngle)
{
	ocean_assert(matrix.isOrthonormal());

	/**
	 * Combined rotation matrix for R(x)R(y)R(z)
	 * [       cy cz               -cy sz            sy   ]
	 * [  cx sz + sx sy cz    cx cz - sx sy sz    -sx cy  ]
	 * [  sx sz - cx sy cz    sx cz + cx sy sz     cx cy  ]
	 */

	if (matrix(0, 2) < T(1) - NumericT<T>::eps())
	{
		if (matrix(0, 2) > T(-1) + NumericT<T>::eps())
		{
			// we have the normal case without any extreme angles

			ocean_assert(NumericT<T>::isNotEqualEps(matrix(1, 2)) || NumericT<T>::isNotEqualEps(matrix(2, 2)));
			xAngle = NumericT<T>::atan2(-matrix(1, 2), matrix(2, 2));

			yAngle = NumericT<T>::asin(matrix(0, 2));

			ocean_assert(NumericT<T>::isNotEqualEps(matrix(0, 1)) || NumericT<T>::isNotEqualEps(matrix(0, 0)));
			zAngle = NumericT<T>::atan2(-matrix(0, 1), matrix(0, 0));
		}
		else
		{
			// we have a special case where sx == -1
			ocean_assert((std::is_same<float, T>::value) ? NumericT<T>::isWeakEqual(matrix(0, 2), -1) : NumericT<T>::isEqual(matrix(0, 2), -1));

			/**
			 * Combined rotation matrix for R(x)R(y)R(z), with sy == -1 and cy == 0
			 * [        0                0          1  ]     [        0             0        1  ]
			 * [  cx sz - sx cz    cx cz + sx sz    0  ]  =  [  -sin(x - z)    cos(x - z)    0  ]
			 * [  sx sz + cx cz    sx cz - cx sz    0  ]     [   cos(x - z)    sin(x - z)    0  ]
			 */

			// tan(x - z) = sin(x - z) / cos(x - z), z == 0

			xAngle = NumericT<T>::atan2(matrix(2, 1), matrix(1, 1));
			yAngle = -NumericT<T>::pi_2();
			zAngle = T(0.0);
		}
	}
	else
	{
		// we have a special case where sy == 1
		ocean_assert((std::is_same<float, T>::value) ? NumericT<T>::isWeakEqual(matrix(0, 2), 1) : NumericT<T>::isEqual(matrix(0, 2), 1));

		/**
		 * Combined rotation matrix for R(x)R(y)R(z), with sy == 1 and cy == 0
		 * [        0                0          1  ]     [        0             0        1  ]
		 * [  cx sz + sx cz    cx cz - sx sz    0  ]  =  [   sin(x + z)    cos(x + z)    0  ]
		 * [  sx sz - cx cz    sx cz + cx sz    0  ]     [  -cos(x + z)    sin(x + z)    0  ]
		 */

		// tan(x + z) = sin(x + z) / cos(x + z), z == 0

		xAngle = NumericT<T>::atan2(matrix(1, 0), matrix(1, 1));
		yAngle = NumericT<T>::pi_2();
		zAngle = T(0.0);
	}

	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), xAngle, NumericT<T>::pi()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), yAngle, NumericT<T>::pi_2()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), zAngle, NumericT<T>::pi()));
}

template <typename T>
void EulerT<T>::adjustAngles(T& yaw, T& pitch, T& roll)
{
	pitch = NumericT<T>::angleAdjustNull(pitch);

	if (NumericT<T>::abs(pitch) > NumericT<T>::pi_2())
	{
		pitch = NumericT<T>::copySign(NumericT<T>::pi(), pitch) - pitch;
		yaw += NumericT<T>::pi();
		roll += NumericT<T>::pi();
	}

	yaw = NumericT<T>::angleAdjustNull(yaw);
	roll = NumericT<T>::angleAdjustNull(roll);

	ocean_assert(Euler(yaw, pitch, roll).isValid());
}

}

#endif // META_OCEAN_MATH_EULER_H
