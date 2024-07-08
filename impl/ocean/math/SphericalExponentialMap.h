/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SPHERICAL_EXPONENTIAL_MAP_H
#define META_OCEAN_MATH_SPHERICAL_EXPONENTIAL_MAP_H

#include "ocean/math/Math.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

/**
 * This class implements an exponential map representing a 2-DOF rotation.
 * Thus, this class allows to define a rotation between two 3D coordinates located on a sphere (with 2 degrees of freedom).<br>
 * The rotation is defined by two scalar parameters defining a 3D vector located in the xz-plane (having zero as value for the y-axis).<br>
 * The 3D vector defines the rotation axis and the length of the vector defined the rotation angle.<br>
 * This class can be used to store the 3D position of a point lying on a sphere by two scalar parameters.<br>
 * Further, this class can be used to rotate a 3D point lying on a sphere to any other position on that sphere.<br>
 * Compared to an Euler rotation (3-DOF), the SphericalExponentialMap (2-DOF) holds/provides/stores a yaw-angle and a pitch-angle, but not roll-angle.<br>
 * The SphericalExponentialMap class can be seen as a specialization of the ExponentialMap.<br>
 * @see ExponentialMap, Rotation, Quaternion, Euler.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT SphericalExponentialMap
{
	public:

		/**
		 * Creates a new exponential map object with default (no) rotation.
		 */
		inline SphericalExponentialMap();

		/**
		 * Creates an exponential map object by a given 3D rotation axis (lying in the xz-plane) with axis length defining the rotation angle in radian.
		 * @param wx X component of the rotation axis
		 * @param wz Z component of the rotation axis
		 */
		inline SphericalExponentialMap(const Scalar wx, const Scalar wz);

		/**
		 * Creates a new exponential map object that rotates a given offset vector to a given reference vector both lying on the same unit sphere in 3D space.
		 * The following equation holds: offset = SphericalExponentialMap(reference, offset).rotation() * reference.<br>
		 * @param reference The reference vector, with unit length
		 * @param offset The offset vector, with unit length
		 */
		SphericalExponentialMap(const Vector3& reference, const Vector3& offset);

		/**
		 * Returns the (non-normalized) rotation axis of this object.
		 * Beware: This axis has a length equal to the rotation angle.
		 * @return Non normalized rotation axis (in 3D space lying in the xz-plane)
		 */
		inline const Vector3 axis() const;

		/**
		 * Returns the angle of this rotation object.
		 * @return Rotation angle, in radian
		 */
		inline Scalar angle() const;

		/**
		 * Returns this rotation representation as quaternion object.
		 * @return Equivalent quaternion rotation
		 */
		inline Quaternion quaternion() const;

		/**
		 * Returns this rotation representation as angle-axis object.
		 * @return Equivalent rotation
		 */
		inline Rotation rotation() const;

		/**
		 * Returns a pointer to the two rotation values of this object (for the x-axis and z-axis).
		 * @return Pointer to the rotation values, with order [wx, wz]
		 */
		inline const Scalar* data() const;

		/**
		 * Returns a pointer to the two rotation values of this object (for the x-axis and z-axis).
		 * @return Pointer to the rotation values, with order [wx, wz]
		 */
		inline Scalar* data();

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 1]
		 * @return The requested element
		 */
		inline const Scalar& operator[](const unsigned int index) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 1]
		 * @return The requested element
		 */
		inline Scalar& operator[](const unsigned int index);

	protected:

		/// The 2-DOF orientation values with order [wx, wz]
		Vector2 mapRotationAxis;
};

inline SphericalExponentialMap::SphericalExponentialMap() :
	mapRotationAxis(0, 0)
{
	// nothing to do here
}

inline SphericalExponentialMap::SphericalExponentialMap(const Scalar wx, const Scalar wz) :
	mapRotationAxis(wx, wz)
{
	// nothing to do here
}

inline const Vector3 SphericalExponentialMap::axis() const
{
	return Vector3(mapRotationAxis[0], 0, mapRotationAxis[1]);
}

inline Scalar SphericalExponentialMap::angle() const
{
	return mapRotationAxis.length();
}

inline Quaternion SphericalExponentialMap::quaternion() const
{
	return Quaternion(rotation());
}

inline Rotation SphericalExponentialMap::rotation() const
{
	const Scalar length = mapRotationAxis.length();

	if (Numeric::isEqualEps(length))
	{
		return Rotation();
	}

	return Rotation(Vector3(mapRotationAxis[0], 0, mapRotationAxis[1]) / length, length);
}

inline const Scalar* SphericalExponentialMap::data() const
{
	return mapRotationAxis.data();
}

inline Scalar* SphericalExponentialMap::data()
{
	return mapRotationAxis.data();
}

inline const Scalar& SphericalExponentialMap::operator[](const unsigned int index) const
{
	ocean_assert(index <= 1u);

	return mapRotationAxis[index];
}

inline Scalar& SphericalExponentialMap::operator[](const unsigned int index)
{
	ocean_assert(index <= 1u);

	return mapRotationAxis[index];
}

}

#endif // META_OCEAN_MATH_SPHERICAL_EXPONENTIAL_MAP_H
