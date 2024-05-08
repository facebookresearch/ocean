/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Quadric.h"

#include "ocean/math/Vector4.h"

namespace Ocean
{

namespace Geometry
{

Quadric::Quadric()
{
	// nothing to do here
}

bool Quadric::isInside(const Vector3 &point) const
{
	Vector4 vector(point);

	return vector * (combinedQuadric * vector) <= 0;
}

bool Quadric::isOnSurface(const Vector3& point) const
{
	Vector4 vector(point);

	return Numeric::isEqualEps(vector * (combinedQuadric * vector));
}

Scalar Quadric::value(const Vector3& point) const
{
	Vector4 vector(point);
	return vector * (combinedQuadric * vector);
}

void Quadric::createCombinedQuadric(const SquareMatrix4& quadric, const HomogenousMatrix4 &transformation)
{
	HomogenousMatrix4 iViewingMatrix(transformation.inverted());
	SquareMatrix4 tivMatrix(iViewingMatrix);
	tivMatrix.transpose();

	combinedQuadric = tivMatrix * quadric * iViewingMatrix;
}

Cone::Cone() :
	Quadric()
{
	// nothing to do here
}

Cone::Cone(const Scalar angle, const HomogenousMatrix4 &transformation)
{
	ocean_assert(angle > 0.0 || angle < Numeric::pi());

	Scalar t = Numeric::tan(angle);
	t *= t;
	const Scalar quadricValues[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -t, 0,
		0, 0, 0, 0
	};

	createCombinedQuadric(SquareMatrix4(quadricValues), transformation);
}

Sphere::Sphere() :
	Quadric()
{
	// nothing to do here
}

Sphere::Sphere(const Scalar radius, const HomogenousMatrix4 &transformation)
{
	ocean_assert(radius > 0.0);

	const Scalar quadricValues[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, radius * radius
	};

	createCombinedQuadric(SquareMatrix4(quadricValues), transformation);
}


}

}
