// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MATH_EXPONENTIAL_MAP_H
#define META_OCEAN_MATH_EXPONENTIAL_MAP_H

#include "ocean/math/Math.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class ExponentialMapT;

/**
 * Definition of the ExponentialMap object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION flag either with single or double precision float data type.
 * @see ExponentialMapT
 * @ingroup math
 */
typedef ExponentialMapT<Scalar> ExponentialMap;

/**
 * Instantiation of the ExponentialMapT template class using a double precision float data type.
 * @see ExponentialMapT
 * @ingroup math
 */
typedef ExponentialMapT<double> ExponentialMapD;

/**
 * Instantiation of the ExponentialMapT template class using a single precision float data type.
 * @see ExponentialMapT
 * @ingroup math
 */
typedef ExponentialMapT<float> ExponentialMapF;

/**
 * Definition of a typename alias for vectors with ExponentialMapT objects.
 * @see ExponentialMapT
 * @ingroup math
 */
template <typename T>
using ExponentialMapsT = std::vector<ExponentialMapT<T>>;

/**
 * Definition of a vector holding ExponentialMap objects.
 * @see ExponentialMap
 * @ingroup math
 */
typedef std::vector<ExponentialMap> ExponentialMaps;

/**
 * This class implements an exponential map defining a rotation by three parameters.<br>
 * The three parameter define the rotation axis, while the length of the axis vector defines the rotation angle in radian.<br>
 * @tparam T Floating point data type to be used
 * @see SphericalExponentialMap, Rotation, Quaternion, Euler.
 * @ingroup math
 */
template <typename T>
class ExponentialMapT
{
	public:

		/**
		 * Creates a default rotation.
		 */
		inline ExponentialMapT();

		/**
		 * Creates an exponential map rotation by a given 3D axis with axis length defining the rotation angle in radian.
		 * @param rotation 3D vector defining the rotation
		 */
		explicit inline ExponentialMapT(const VectorT3<T>& rotation);

		/**
		 * Creates an exponential map roation by a given 3D axis with axis length defining the rotation angle in radian.
		 * @param wx X component of the rotation axis
		 * @param wy Y component of the rotation axis
		 * @param wz Z component of the rotation axis
		 */
		inline ExponentialMapT(const T wx, const T wy, const T wz);

		/**
		 * Creates an exponential map rotation by a given noramlized 3D axis with length 1 and a given rotation angle.
		 * @param axis Normalized rotation axis
		 * @param angle Angle of the rotation, in radian
		 */
		inline ExponentialMapT(const VectorT3<T>& axis, const T angle);

		/**
		 * Creates an exponential map rotation by a given angle axis rotation object.
		 * @param rotation Rotation to be used
		 */
		explicit inline ExponentialMapT(const RotationT<T>& rotation);

		/**
		 * Creates an exponential map rotation by a given quaternion rotation object.
		 * @param rotation Rotation to be used
		 */
		explicit ExponentialMapT(const QuaternionT<T>& rotation);

		/**
		 * Creates an exponential map rotation by a given 3x3 rotation matrix.
		 * @param rotation Rotation matrix to be used
		 */
		explicit ExponentialMapT(const SquareMatrixT3<T>& rotation);

		/**
		 * Returns the (non-normalized) axis of this rotation object.
		 * Beware: This axis has a length equal to the rotation angle in radian.
		 * @return Non normalized rotation axis
		 */
		inline const VectorT3<T>& axis() const;

		/**
		 * Returns the angle of this rotation object.
		 * @return Rotation angle in radian
		 */
		inline T angle() const;

		/**
		 * Returns this rotation object as quaternion object.
		 * @return Equivalent quaternion rotation
		 */
		inline QuaternionT<T> quaternion() const;

		/**
		 * Returns this rotation object as angle-axis object.
		 * @return Equivalent rotation
		 */
		inline RotationT<T> rotation() const;

		/**
		 * Returns a pointer to the three rotation values of this object.
		 * @return The three rotation values
		 */
		inline const T* data() const;

		/**
		 * Returns the individual rotation values of this map.
		 * @param index The index of the value to return, with range [0, 2]
		 * @return The requested index
		 */
		inline const T& operator[](const unsigned int index) const;

		/**
		 * Returns the individual rotation values of this map.
		 * @param index The index of the value to return, with range [0, 2]
		 * @return The requested index
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Adds two exponential map objects.
		 * @param exponentialMap Exponential map to be (component wise) added
		 * @return New exponential map object
		 */
		inline ExponentialMapT<T> operator+(const ExponentialMapT<T>& exponentialMap) const;

		/**
		 * Adds two exponential map objects.
		 * @param exponentialMap Exponential map to be (component wise) added
		 * @return Reference to this object
		 */
		inline ExponentialMapT<T>& operator+=(const ExponentialMapT<T>& exponentialMap);

		/**
		 * Subtracts two exponential map objects.
		 * @param exponentialMap Exponential map to be (component wise) subtracted
		 * @return New exponential map object
		 */
		inline ExponentialMapT<T> operator-(const ExponentialMapT<T>& exponentialMap) const;

		/**
		 * Subtracts two exponential map objects.
		 * @param exponentialMap Exponential map to be (component wise) subtracted
		 * @return Reference to this object
		 */
		inline ExponentialMapT<T>& operator-=(const ExponentialMapT<T>& exponentialMap);

	private:

		/// Axis defining the rotation normal while the length defines the rotation angle.
		VectorT3<T> rotationValues_;
};

template <typename T>
inline ExponentialMapT<T>::ExponentialMapT() :
	rotationValues_(0, 0, 0)
{
	// nothing to do here
}

template <typename T>
inline ExponentialMapT<T>::ExponentialMapT(const VectorT3<T>& rotation) :
	rotationValues_(rotation)
{
	// nothing to do here
}

template <typename T>
inline ExponentialMapT<T>::ExponentialMapT(const T wx, const T wy, const T wz) :
	rotationValues_(wx, wy, wz)
{
	// nothing to do here
}

template <typename T>
inline ExponentialMapT<T>::ExponentialMapT(const VectorT3<T>& axis, const T angle) :
	rotationValues_(axis * angle)
{
	ocean_assert(axis.isUnit());
}

template <typename T>
inline ExponentialMapT<T>::ExponentialMapT(const RotationT<T>& rotation) :
	rotationValues_(rotation.axis() * rotation.angle())
{
	ocean_assert(rotation.isValid());
}

template <typename T>
ExponentialMapT<T>::ExponentialMapT(const QuaternionT<T>& rotation) :
	rotationValues_(0, 0, 0)
{
	const RotationT<T> angleAxis(rotation);
	ocean_assert(angleAxis.isValid());

	const VectorT3<T> axis(angleAxis.axis());
	ocean_assert(axis.isUnit());

	rotationValues_ = axis * angleAxis.angle();
}

template <typename T>
ExponentialMapT<T>::ExponentialMapT(const SquareMatrixT3<T>& rotation) :
	rotationValues_(0, 0, 0)
{
	const RotationT<T> angleAxis(rotation);
	ocean_assert(angleAxis.isValid());

	const VectorT3<T> axis(angleAxis.axis());
	ocean_assert(axis.isUnit());

	rotationValues_ = axis * angleAxis.angle();
}

template <typename T>
inline const VectorT3<T>& ExponentialMapT<T>::axis() const
{
	return rotationValues_;
}

template <typename T>
inline T ExponentialMapT<T>::angle() const
{
	return rotationValues_.length();
}

template <typename T>
inline QuaternionT<T> ExponentialMapT<T>::quaternion() const
{
	const T angle = rotationValues_.length();

	if (NumericT<T>::isEqualEps(angle))
	{
		return QuaternionT<T>();
	}

	return QuaternionT<T>(rotationValues_ / angle, angle);
}

template <typename T>
inline RotationT<T> ExponentialMapT<T>::rotation() const
{
	const T angle = rotationValues_.length();

	if (NumericT<T>::isEqualEps(angle))
	{
		return RotationT<T>();
	}

	return RotationT<T>(rotationValues_ / angle, angle);
}

template <typename T>
inline const T* ExponentialMapT<T>::data() const
{
	return rotationValues_.data();
}

template <typename T>
inline const T& ExponentialMapT<T>::operator[](const unsigned int index) const
{
	ocean_assert(index <= 2u);
	return rotationValues_[index];
}

template <typename T>
inline T& ExponentialMapT<T>::operator[](const unsigned int index)
{
	ocean_assert(index <= 2u);
	return rotationValues_[index];
}

template <typename T>
inline ExponentialMapT<T> ExponentialMapT<T>::operator+(const ExponentialMapT<T>& exponentialMap) const
{
	return ExponentialMapT<T>(rotationValues_ + exponentialMap.rotationValues_);
}

template <typename T>
inline ExponentialMapT<T>& ExponentialMapT<T>::operator+=(const ExponentialMapT<T>& exponentialMap)
{
	rotationValues_ += exponentialMap.rotationValues_;
	return *this;
}

template <typename T>
inline ExponentialMapT<T> ExponentialMapT<T>::operator-(const ExponentialMapT<T>& exponentialMap) const
{
	return ExponentialMapT<T>(rotationValues_ - exponentialMap.rotationValues_);
}

template <typename T>
inline ExponentialMapT<T>& ExponentialMapT<T>::operator-=(const ExponentialMapT<T>& exponentialMap)
{
	rotationValues_ -= exponentialMap.rotationValues_;
	return *this;
}

}

#endif // META_OCEAN_MATH_EXPONENTIAL_MAP_H
