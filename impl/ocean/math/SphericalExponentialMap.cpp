/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/SphericalExponentialMap.h"

namespace Ocean
{

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

			ocean_assert(Numeric::rad2deg((rotation() * reference).angle(offset)) <= 0.001);
		}
		else
		{
			// we have a 180 deg rotation between both vectors, so we need to determine a vector perpendicular to both vectors lying in the X-Z plane

			const Vector3 axis(Vector3(0, 1, 0).cross(reference));
			ocean_assert(Numeric::isEqualEps(axis[1]) && Numeric::isEqual(axis.length(), 1));

			mapRotationAxis = Vector2(axis[0], axis[2]) * Numeric::pi();

			ocean_assert(Numeric::rad2deg((rotation() * reference).angle(offset)) <= 0.001);
		}
	}
	else
	{
		const Vector3 bisect(reference + offset);
		ocean_assert(!bisect.isNull() && Numeric::isEqualEps(bisect * normal));

		/// the normal of the plane rop
		const Vector3 planeNormal(bisect.cross(normal));
		ocean_assert(!planeNormal.isNull());

		/// the rotation axis lying in rop and lying in xzp
		Vector3 axis(planeNormal.cross(Vector3(0, 1, 0)));
		ocean_assert(!axis.isNull());
		axis.normalize();

		ocean_assert(Numeric::isEqual(Numeric::rad2deg(reference.angle(axis)), Numeric::rad2deg(offset.angle(axis)), Scalar(0.001)));
		ocean_assert(Numeric::isEqualEps(axis[1]));

		const Vector3 pReference(axis.cross(reference).cross(axis));
		const Vector3 pOffset(axis.cross(offset.cross(axis)));

		const Scalar angle = pReference.angle(pOffset);
		ocean_assert(Numeric::isNotEqualEps(angle));

		if (pReference.cross(pOffset) * axis < 0)
		{
			axis = -axis;
		}

#ifdef OCEAN_DEBUG
		Rotation debugRotation(axis, angle);
		const Vector3 rOffset = debugRotation * reference;
		const Scalar debugAngle = Numeric::rad2deg(rOffset.angle(offset));
		ocean_assert(debugAngle <= 0.001);
#endif

		mapRotationAxis = Vector2(axis[0], axis[2]) * angle;

		ocean_assert(Numeric::rad2deg((rotation() * reference).angle(offset)) <= 0.001);
	}

	ocean_assert(Numeric::rad2deg((rotation() * reference).angle(offset)) <= 0.001);
}

}
