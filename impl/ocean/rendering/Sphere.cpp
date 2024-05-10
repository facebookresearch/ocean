/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Sphere.h"

namespace Ocean
{

namespace Rendering
{

Sphere::Sphere() :
	Shape()
{
	// nothing to do here
}

Sphere::~Sphere()
{
	// nothing to do here
}

Scalar Sphere::radius() const
{
	throw NotSupportedException("Sphere::radius() is not supported.");
}

bool Sphere::setRadius(const Scalar /*radius*/)
{
	throw NotSupportedException("Sphere::setRadius() is not supported.");
}

Vertex Sphere::vertex(const Scalar latitude, const Scalar longitude)
{
	const Scalar d = Numeric::cos(latitude);
	const Vector3 v(d * Numeric::sin(longitude), Numeric::sin(latitude), d * Numeric::cos(longitude));

	return v.normalized();
}

Sphere::ObjectType Sphere::type() const
{
	return TYPE_SPHERE;
}

}

}
