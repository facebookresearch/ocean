/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/BoundingSphere.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

bool BoundingSphere::intersections(const Line3& ray, Vector3& position0, Scalar& distance0, Vector3& position1, Scalar& distance1) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	// (d * d) * t^2 + (2 * d * (p - o)) * t + (p - o)*(p - o) - r^2 == 0
	//       a * t^2 +                 b * t +                     c == 0

	const Vector3& d = ray.direction();
	const Vector3& p = ray.point();
	const Vector3& o = center_;

	const Vector3 po(p - o);

	const Scalar a = d * d;
	const Scalar b = (d * po) * 2;
	const Scalar c = po * po - radius_ * radius_;

	ocean_assert(a > Numeric::eps());

	const Scalar factor = b * b - 4 * a * c;
	if (factor < 0)
		return false;

	const Scalar aFactor = 1 / (a * 2);
	const Scalar sqrtFactor = Numeric::sqrt(factor);

	const Scalar t0 = (-b + sqrtFactor) * aFactor;
	const Scalar t1 = (-b - sqrtFactor) * aFactor;

	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t0 * t0 + b * t0 + c));
	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t1 * t1 + b * t1 + c));

	if (t0 < t1)
	{
		distance0 = t0;
		position0 = ray.point() + ray.direction() * t0;

		distance1 = t1;
		position1 = ray.point() + ray.direction() * t1;
	}
	else
	{
		distance0 = t1;
		position0 = ray.point() + ray.direction() * t1;

		distance1 = t0;
		position1 = ray.point() + ray.direction() * t0;
	}

	return true;
}

bool BoundingSphere::positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	// (d * d) * t^2 + (2 * d * (p - o)) * t + (p - o)*(p - o) - r^2 == 0
	//       a * t^2 +                 b * t +                     c == 0

	const Vector3& d = ray.direction();
	const Vector3& p = ray.point();
	const Vector3& o = center_;

	const Vector3 po(p - o);

	const Scalar a = d * d;
	const Scalar b = (d * po) * 2;
	const Scalar c = po * po - radius_ * radius_;

	ocean_assert(a > Numeric::eps());

	const Scalar factor = b * b - 4 * a * c;
	if (factor < 0)
		return false;

	const Scalar aFactor = 1 / (a * 2);
	ocean_assert(aFactor >= 0);

	const Scalar sqrtFactor = Numeric::sqrt(factor);

	const Scalar t0 = (-b + sqrtFactor) * aFactor;
	const Scalar t1 = (-b - sqrtFactor) * aFactor;

	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t0 * t0 + b * t0 + c));
	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t1 * t1 + b * t1 + c));
	ocean_assert_and_suppress_unused(t0 >= t1, t0);

	// check whether the entire sphere is in front of the ray
	if (t1 >= 0)
	{
		ocean_assert(t0 >= 0);
		ocean_assert(t1 <= t0);

		distance = t1;
		position = ray.point() + ray.direction() * t1;

		ocean_assert(ray.direction() * (position - center_) <= 0);
		return true;
	}

	// the ray starts inside the sphere or starts behind the sphere
	return false;
}

bool BoundingSphere::positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	// (d * d) * t^2 + (2 * d * (p - o)) * t + (p - o)*(p - o) - r^2 == 0
	//       a * t^2 +                 b * t +                     c == 0

	const Vector3& d = ray.direction();
	const Vector3& p = ray.point();
	const Vector3& o = center_;

	const Vector3 po(p - o);

	const Scalar a = d * d;
	const Scalar b = (d * po) * 2;
	const Scalar c = po * po - radius_ * radius_;

	ocean_assert(a > Numeric::eps());

	const Scalar factor = b * b - 4 * a * c;
	if (factor < 0)
		return false;

	const Scalar aFactor = 1 / (a * 2);
	ocean_assert(aFactor >= 0);
	const Scalar sqrtFactor = Numeric::sqrt(factor);

	const Scalar t0 = (-b + sqrtFactor) * aFactor;
	const Scalar t1 = (-b - sqrtFactor) * aFactor;

	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t0 * t0 + b * t0 + c));
	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqualEps(a * t1 * t1 + b * t1 + c));
	ocean_assert_and_suppress_unused(t0 >= t1, t1);

	// check whether the entire sphere is in front of the ray, or whether the ray starts inside the sphere
	if (t0 >= 0)
	{
		ocean_assert(t1 <= t0);

		distance = t0;
		position = ray.point() + ray.direction() * t0;

		ocean_assert(ray.direction() * (position - center_) >= 0);
		return true;
	}

	return false;
}

}
