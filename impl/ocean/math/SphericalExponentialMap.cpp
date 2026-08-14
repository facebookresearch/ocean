/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/SphericalExponentialMap.h"

namespace Ocean
{

#ifdef OCEAN_DEBUG

namespace
{

bool hasAccurateRotation(const Rotation& rotation, const Vector3& reference, const Vector3& offset)
{
	const VectorD3 rotatedReference(rotation * reference);

	return NumericD::rad2deg(rotatedReference.angle(VectorD3(offset))) <= 0.001;
}

}

#endif // OCEAN_DEBUG

SphericalExponentialMap::SphericalExponentialMap(const Vector3& reference, const Vector3& offset)
{
	ocean_assert(Numeric::isEqual(reference.length(), 1));
	ocean_assert(Numeric::isEqual(offset.length(), 1));

	/**
	 * the resulting rotation axis must lie in the X-Z plane, and the angle between the rotation axis and the reference and offset vector must be identical
	 *
	 * we first determine the rotation axis ra:
	 * ra lies in the X-Z plane xzp
	 * ra lies in the plane rop between reference and offset where each point on the plane has the same distance to reference and offset
	 * the normal of reference and offset lies also in rop
	 *
	 * afterwards we have to determine the angle for the determined rotation axis ra:
	 * we need to find a transformed reference vector r' and a transformed offset vector o' which are each perpendicular to the determined rotation axis
	 * further r' must lie in the axis-reference plane and o' must lie in the axis-offset plane
	 * the angle between r' and o' is the rotation angle
	 */

	const Vector3 normal(reference.cross(offset));

	// check whether we have a valid cross product
	if (normal.isNull())
	{
		// reference and offset are parallel
		ocean_assert(reference == offset || reference == -offset);

		if (reference == offset)
		{
			// we have no rotation between both vectors
			mapRotationAxis = Vector2(0, 0);

			ocean_assert(hasAccurateRotation(rotation(), reference, offset));
		}
		else
		{
			// we have a 180 deg rotation between both vectors, so we need to determine a vector perpendicular to both vectors lying in the X-Z plane

			Vector3 axis(Vector3(0, 1, 0).cross(reference));

			if (!axis.normalize())
			{
				// reference is parallel to the y-axis, so that every axis lying in the X-Z plane is perpendicular to reference
				axis = Vector3(1, 0, 0);
			}

			ocean_assert(Numeric::isEqualEps(axis[1]) && Numeric::isEqual(axis.length(), 1));

			mapRotationAxis = Vector2(axis[0], axis[2]) * Numeric::pi();

			ocean_assert(hasAccurateRotation(rotation(), reference, offset));
		}
	}
	else
	{
		// For unit vectors, the equal-distance plane has normal reference - offset; computing it directly avoids cancellation near anti-parallel inputs.
		const Vector3 planeNormal(reference - offset);
		ocean_assert(!planeNormal.isNull());

		/// the rotation axis lying in rop and lying in xzp
		Vector3 axis(planeNormal.cross(Vector3(0, 1, 0)));

		if (!axis.normalize())
		{
			// reference and offset differ only along the y-axis, so that their shared projection into the X-Z plane is the rotation axis
			axis = Vector3(reference[0] + offset[0], 0, reference[2] + offset[2]);
			axis.normalize();
			ocean_assert(axis.isUnit());
		}

		ocean_assert(Numeric::isEqual(Numeric::rad2deg(reference.angle(axis)), Numeric::rad2deg(offset.angle(axis)), Scalar(0.001)));
		ocean_assert(Numeric::isEqualEps(axis[1]));

		const Vector3 pReference(axis.cross(reference).cross(axis));
		const Vector3 pOffset(axis.cross(offset.cross(axis)));
		const Vector3 pCross(pReference.cross(pOffset));

		const Scalar angle = Numeric::atan2(pCross.length(), pReference * pOffset);
		ocean_assert(Numeric::isNotEqualEps(angle));

		if (pCross * axis < 0)
		{
			axis = -axis;
		}

#ifdef OCEAN_DEBUG
		const Rotation debugRotation(axis, angle);
		ocean_assert(hasAccurateRotation(debugRotation, reference, offset));
#endif

		mapRotationAxis = Vector2(axis[0], axis[2]) * angle;

		ocean_assert(hasAccurateRotation(rotation(), reference, offset));
	}

	ocean_assert(hasAccurateRotation(rotation(), reference, offset));
}

}
