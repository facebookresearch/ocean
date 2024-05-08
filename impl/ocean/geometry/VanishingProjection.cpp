/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/VanishingProjection.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

bool VanishingProjection::planeNormal(const PinholeCamera& pinholeCamera, const Vector2 parallelogramPoints[4], const bool undistortImagePoints, Vector3& normal)
{
	ocean_assert(pinholeCamera.isValid());

	Vector2 normalizedImagePoints[4];

	for (unsigned int n = 0u; n < 4u; ++n)
		normalizedImagePoints[n] = pinholeCamera.imagePoint2normalizedImagePointDamped(parallelogramPoints[n], undistortImagePoints);

	Line2 vLine;
	if (vanishingLine(normalizedImagePoints, vLine))
	{
		const Vector3 lineParameters(vLine.decomposeNormalDistance());

		// the plane normal is equivalent with the line

		normal = lineParameters;
		if (!normal.normalize())
			return false;

		normal = pinholeCamera.flipMatrix3() * normal;
		ocean_assert(Numeric::isEqual(normal.length(), 1));
	}
	else
	{
		// we have no valid vanishing point and thus both line pairs are parallel (we have a rectangle parallel with the viewing plane)
		normal = Vector3(0, 0, 1);
	}

	if (normal * Vector3(0, 0, 1) < 0)
		normal = -normal;

	return true;
}

bool VanishingProjection::vanishingLine(const Vector2 parallelogramPoints[4], Line2& line)
{
	// determine the two vanishing points

	Vector2 direction03(parallelogramPoints[3] - parallelogramPoints[0]);
	Vector2 direction12(parallelogramPoints[2] - parallelogramPoints[1]);

	Vector2 direction01(parallelogramPoints[1] - parallelogramPoints[0]);
	Vector2 direction32(parallelogramPoints[2] - parallelogramPoints[3]);

	// check whether two of the given points are identical
	if (!direction03.normalize() || !direction12.normalize() || !direction01.normalize() || !direction32.normalize())
		return false;

	const Line2 line03(parallelogramPoints[0], direction03);
	const Line2 line12(parallelogramPoints[1], direction12);

	const Line2 line01(parallelogramPoints[0], direction01);
	const Line2 line32(parallelogramPoints[3], direction32);

	Vector2 vanishingPoint0, vanishingPoint1;

	const bool validVanishingPoint0 = line03.intersection(line12, vanishingPoint0);
	const bool validVanishingPoint1 = line01.intersection(line32, vanishingPoint1);

	// check whether both vanishing points are valid (both line pairs are not parallel)
	if (validVanishingPoint0 && validVanishingPoint1)
	{
		Vector2 vanishingDirection01(vanishingPoint1 - vanishingPoint0);
		if (!vanishingDirection01.normalize())
			return false;

		line = Line2(vanishingPoint0, vanishingDirection01);
		return true;
	}
	else if (validVanishingPoint0)
	{
		// we have only one valid vanishing point, as one line pair is parallel
		ocean_assert(!validVanishingPoint1);
		ocean_assert(line01.isParallel(line32));

		line = Line2(vanishingPoint0, line01.direction());
		return true;
	}
	else if (validVanishingPoint1)
	{
		// we have only one valid vanishing point, as one line pair is parallel
		ocean_assert(!validVanishingPoint0);
		ocean_assert(line12.isParallel(line03));

		line = Line2(vanishingPoint1, line12.direction());
		return true;
	}

	return false;
}

Indices32 VanishingProjection::perspectiveParallelLines(const ConstIndexedAccessor<Line2>& lines, const Scalar projectiveParallelAngle, const Scalar orientationError, const Scalar maximalAngle)
{
	ocean_assert(projectiveParallelAngle >= 0 && projectiveParallelAngle < Numeric::pi_2());
	ocean_assert(orientationError >= 0 && orientationError < Numeric::pi_2());

	ocean_assert(maximalAngle >= projectiveParallelAngle);
	ocean_assert(maximalAngle > orientationError && maximalAngle < Numeric::pi_2());

	const SquareMatrix3 rotationPlus3(Quaternion(Vector3(0, 0, 1), orientationError * Scalar(0.5)));
	const SquareMatrix3 rotationMinus3(Quaternion(Vector3(0, 0, 1), -orientationError * Scalar(0.5)));

	const SquareMatrix2 rotationPlus(rotationPlus3(0, 0), rotationPlus3(1, 0), rotationPlus3(0, 1), rotationPlus3(1, 1));
	const SquareMatrix2 rotationMinus(rotationMinus3(0, 0), rotationMinus3(1, 0), rotationMinus3(0, 1), rotationMinus3(1, 1));

	const Scalar projectiveParallelAngleCos = Numeric::cos(projectiveParallelAngle);
	const Scalar maximalAngleCos = Numeric::cos(maximalAngle);

	const ScopedConstMemoryAccessor<Line2> scopedMemory(lines);
	const Line2* lineData = scopedMemory.data();

	Indices32 bestIndices;
	bestIndices.reserve(scopedMemory.size());

	Indices32 candidateIndices;
	candidateIndices.reserve(scopedMemory.size());

	for (size_t a = 0; a < scopedMemory.size(); ++a)
	{
		const Line2& referenceLine = lineData[a];

		for (size_t b = a + 1; b < scopedMemory.size(); ++b)
		{
			const Line2& candidateLine = lineData[b];

			candidateIndices.clear();

			candidateIndices.push_back(Index32(a));
			candidateIndices.push_back(Index32(b));

			const Scalar absCosValue = Numeric::abs(referenceLine.direction() * candidateLine.direction());

			if (absCosValue > projectiveParallelAngleCos)
			{
				ocean_assert(referenceLine.direction().angle(candidateLine.direction()) <= projectiveParallelAngle || referenceLine.direction().angle(-candidateLine.direction()) <= projectiveParallelAngle);

				// we have two parallel lines, so we seek for parallel lines only

				for (size_t n = 0; n < scopedMemory.size(); ++n)
					if (n != a && n != b && Numeric::abs(referenceLine.direction() * lineData[n].direction()) > projectiveParallelAngleCos)
						candidateIndices.push_back(Index32(n));
			}
			else if (absCosValue > maximalAngleCos)
			{
				ocean_assert(referenceLine.direction().angle(candidateLine.direction()) >= orientationError * Scalar(2) && referenceLine.direction().angle(-candidateLine.direction()) >= orientationError * Scalar(2));
				ocean_assert(referenceLine.direction().angle(candidateLine.direction()) <= maximalAngle || referenceLine.direction().angle(-candidateLine.direction()) <= maximalAngle);

				// we have two non-parallel lines so that we now seek for lines all intersecting in 'almost' the same point
				// we determine the radius in which the intersection with the remaining lines must be located by slightly changing the pair's orientations and by measuring the resulting error

				Vector2 intersectionPoint;
				if (referenceLine.intersection(candidateLine, intersectionPoint))
				{
					Vector2 badIntersectionPoint0, badIntersectionPoint1;
					if (Line2(referenceLine.point(), rotationPlus * referenceLine.direction()).intersection(Line2(candidateLine.point(), rotationMinus * candidateLine.direction()), badIntersectionPoint0)
								&& Line2(referenceLine.point(), rotationMinus * referenceLine.direction()).intersection(Line2(candidateLine.point(), rotationPlus * candidateLine.direction()), badIntersectionPoint1))
					{
						const Scalar maxSqrDistance = max(intersectionPoint.sqrDistance(badIntersectionPoint0), intersectionPoint.sqrDistance(badIntersectionPoint1));

						Vector2 newIntersectionPoint;
						for (size_t n = 0; n < scopedMemory.size(); ++n)
							if (n != a && n != b && Numeric::abs(referenceLine.direction() * lineData[n].direction()) > maximalAngleCos && referenceLine.intersection(lineData[n], newIntersectionPoint) && intersectionPoint.sqrDistance(newIntersectionPoint) <= maxSqrDistance)
								candidateIndices.push_back(Index32(n));
					}
				}
			}

			if (candidateIndices.size() > bestIndices.size())
				std::swap(bestIndices, candidateIndices);
		}
	}

	return bestIndices;
}

}

}
