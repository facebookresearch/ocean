/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Delaunay.h"

#include "ocean/math/Box2.h"
#include "ocean/math/FiniteLine2.h"

namespace Ocean
{

namespace Geometry
{

Delaunay::IndexTriangles Delaunay::triangulation(const Vectors2& points)
{
	ocean_assert(points.size() >= 3);

	// first we create a vector of point indices sorted according to the x coordinate of the corresponding points

	Indices32 sortedIndices;
	sortedIndices.reserve(points.size());

	while (sortedIndices.size() < points.size())
	{
		sortedIndices.push_back((unsigned int)(sortedIndices.size()));
	}

	std::sort(sortedIndices.begin(), sortedIndices.end(), ComparePointsX(points.data()));


	// now we determine a super triangle which contains all points (equilateral triangle with horizontal base)

	const Box2 pointsBoundingBox(points);

	// safety margin just to avoid any errors due to rounding
	const Scalar safetyMarginX = pointsBoundingBox.width() * Scalar(0.1);
	const Scalar safetyMarginY = pointsBoundingBox.height() * Scalar(0.1);

	const Vector2 pointsSuperTriangle[3] =
	{
		Vector2(pointsBoundingBox.left() - safetyMarginX - pointsBoundingBox.height() * Scalar(0.57735026918962576451), pointsBoundingBox.bottom() + safetyMarginY), // 0.5773 = sqrt(3) / 3
		Vector2(pointsBoundingBox.right() + safetyMarginX + pointsBoundingBox.height() * Scalar(0.57735026918962576451), pointsBoundingBox.bottom() + safetyMarginY),
		Vector2(pointsBoundingBox.center().x(), pointsBoundingBox.top() - safetyMarginY - pointsBoundingBox.width() * Scalar(0.8660254037844386467637)) // 0.8660 = sqrt(3) / 2
	};


	// now we can start the actual Bowyer-Watson algorithm

	// the working set of triangles, with super triangle as first triangle
	std::list<CircumCricleIndexTriangle> workingTriangles(1, CircumCricleIndexTriangle((unsigned int)(points.size()), (unsigned int)(points.size()) + 1u, (unsigned int)(points.size()) + 2u, points.data(), points.size(), pointsSuperTriangle));

	/// the resulting triangles
	IndexTriangles result;
	result.reserve(points.size() * 2);

	// we iterate through all points and add one at a time to the triangulation (as we use the sorted index list we handle points in horizontally ascending order)
	for (const Index32 sortedIndex : sortedIndices)
	{
		ocean_assert(sortedIndex < points.size());
		const Vector2& point = points[sortedIndex];

		// we determine all triangles from the set of working triangles which are ready for set of resulting triangles
		// a triangle is completed/finished if all remaining points are completely right of the triangle's circumcircle (on a horizontal line)

		for (CircumCricleIndexTriangleList::iterator itTriangles = workingTriangles.begin(); itTriangles != workingTriangles.end(); /*noop*/)
		{
			if (itTriangles->isRightOfCircumCircle(point))
			{
				// since the (remaining) points are ordered ascending on a horizontal line, this means that once a point is reached which is right of the circumcircle,
				// there will be no more points which could potentially fall inside the circumcircle, so that we have a completed/finished triangle

				// store to result list only if it does not belong to the super triangle
				if (itTriangles->index0() < points.size() && itTriangles->index1() < points.size() && itTriangles->index2() < points.size())
				{
					result.emplace_back(*itTriangles);
				}

				itTriangles = workingTriangles.erase(itTriangles);
			}
			else
			{
				++itTriangles;
			}
		}

		// now we remove 'bad' triangles but keep the edges which form the boundary of the polygonal hole

		EdgeMap edgeMap;

		for (CircumCricleIndexTriangleList::iterator itTriangles = workingTriangles.begin(); itTriangles != workingTriangles.end(); /*noop*/)
		{
			if (itTriangles->isInsideCircumCircle(point))
			{
				// the point lies within the circumcircle of the triangle; therefore, remove it but keep track of the edges
				// create edge objects and add it to the edge-set (while the smaller index is always the first element of the pair)

				edgeMap[IndexEdge(itTriangles->index0(), itTriangles->index1())]++;
				edgeMap[IndexEdge(itTriangles->index0(), itTriangles->index2())]++;
				edgeMap[IndexEdge(itTriangles->index1(), itTriangles->index2())]++;

				itTriangles = workingTriangles.erase(itTriangles);
			}
			else
			{
				++itTriangles;
			}
		}

		// now we fill the hole by new triangles, one triangle for each boundary edge/segment

		for (EdgeMap::const_iterator iEdge = edgeMap.cbegin(); iEdge != edgeMap.cend(); ++iEdge)
		{
			ocean_assert(iEdge->second >= 1u && iEdge->second <= 2u);

			if (iEdge->second == 1u)
			{
				workingTriangles.push_back(CircumCricleIndexTriangle(sortedIndex, iEdge->first.firstIndex(), iEdge->first.secondIndex(), points.data(), points.size(), pointsSuperTriangle));
			}
		}
	}

	// we finally accept all remaining triangles from the working set as long as the points do not belong to the super triangle

	for (const CircumCricleIndexTriangle& workingTriangle : workingTriangles)
	{
		if (workingTriangle.index0() < (unsigned int)(points.size()) && workingTriangle.index1() < (unsigned int)(points.size()) && workingTriangle.index2() < (unsigned int)(points.size()))
		{
			result.emplace_back(workingTriangle);
		}
	}

	if constexpr (std::is_same<double, Scalar>::value)
	{
		// make sure we got a valid triangulation, we skip the test for low precision floating points
		ocean_assert(checkTriangulation(result, points, Numeric::weakEps() * Scalar(100)));
	}

	return result;
}

bool Delaunay::checkTriangulation(const IndexTriangles& triangles, const Vectors2& points, const Scalar epsilon)
{
	CircumCricleIndexTriangles circumCircleTriangles;
	circumCircleTriangles.reserve(triangles.size());

	for (const IndexTriangle& triangle : triangles)
	{
		ocean_assert(triangle.index0() < points.size() && triangle.index1() < points.size() && triangle.index2() < points.size());

		circumCircleTriangles.emplace_back(triangle.index0(), triangle.index1(), triangle.index2(), points.data());
	}

	return checkTriangulation(circumCircleTriangles, points, epsilon);
}

bool Delaunay::checkTriangulation(const CircumCricleIndexTriangles& triangles, const Vectors2& points, const Scalar epsilon)
{
	for (const CircumCricleIndexTriangle& triangle : triangles)
	{
		// there should be no other point inside the circumcircle

		for (unsigned int n = 0u; n < (unsigned int)(points.size()); ++n)
		{
			// we do not test the corners of the triangle
			if (n != triangle.index0() && n != triangle.index1() && n != triangle.index2() && !triangle.isOutsideCircumCircle(points[n], epsilon))
			{
				return false;
			}
		}
	}

	return true;
}

}

}
