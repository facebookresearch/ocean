/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/Triangulation.h"

#include "ocean/base/Utilities.h"

#include "ocean/cv/advanced/PixelLine.h"

#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

template <typename T>
Triangulation::IndexTriangles Triangulation::triangulate(const PixelContourT<T>& pixelContour, const bool forceTriangulation, bool* triangulationForced)
{
	static_assert(SignedTyper<T>::hasSigned, "Invalid data type!");

	typedef typename SignedTyper<T>::Type TSigned;

	if (triangulationForced)
	{
		*triangulationForced = false;
	}

	if (pixelContour.pixels().empty())
	{
		return IndexTriangles();
	}

	const std::vector<PixelPositionT<T>>& coordinates = pixelContour.pixels();

	PixelPositionPairs corners;
	corners.reserve(coordinates.size());

	for (unsigned int n = 0u; n < coordinates.size(); ++n)
	{
		if (coordinates[modulo(int(n) - 1, int(coordinates.size()))] != coordinates[n])
		{
			corners.push_back(PixelPositionPair(n, false));
		}
	}

	if (corners.empty())
	{
		return IndexTriangles();
	}

	if (corners.size() == 1)
	{
		return IndexTriangles(1, IndexTriangle(corners[0u].first, corners[0u].first, corners[0u].first));
	}

	if (corners.size() == 2)
	{
		return IndexTriangles(1, IndexTriangle(corners[0u].first, corners[1u].first, corners[0u].first));
	}

	if (corners.size() == 3 || pixelContour.boundingBox().width() == 1u || pixelContour.boundingBox().height() == 1u)
	{
		return IndexTriangles(1, IndexTriangle(corners[0u].first, corners[1u].first, corners[2u].first));
	}

	const bool counterClockwise = pixelContour.isCounterClockwise();

	// sign of the a convex corner:
	// negative if contourClockwise, positive otherwise

	for (unsigned int n = 0u; n < corners.size(); ++n)
	{
		if (coordinates[corners[modulo(int(n) - 1, int(corners.size()))].first] != coordinates[corners[n].first])
		{
			const Advanced::PixelLineT<T> lMinus(coordinates[corners[modulo(int(n) - 1, int(corners.size()))].first], coordinates[corners[n].first]);
			const Advanced::PixelLineT<T> lPlus(coordinates[corners[modulo(int(n) + 1, int(corners.size()))].first], coordinates[corners[n].first]);

			const TSigned signCorner = TSigned(lPlus * lMinus);

			const bool cornerIsConvex = (counterClockwise && signCorner <= TSigned(0)) || (!counterClockwise && signCorner > TSigned(0));
			corners[n].second = cornerIsConvex;
		}
	}

	IndexTriangles result;
	result.reserve(corners.size());

	unsigned int index = 0u;
	for (PixelPositionPairs::const_iterator i = corners.begin(); i != corners.end(); ++i)
	{
		if (i->second)
		{
			break;
		}

		++index;
	}

	// the start corner must be convex
	ocean_assert(index < corners.size());

	unsigned int invalidIterations = 0;
	bool mayForce = false;

	while (corners.size() >= 3)
	{
		if (index >= corners.size())
		{
			index = 0u;
		}

		// we take convex corners only
		if (corners[index].second || mayForce)
		{
			const unsigned int indexMinus = modulo(int(index) - 1, int(corners.size()));
			const unsigned int indexPlus = modulo(int(index) + 1, int(corners.size()));

			const Triangle2 triangle(Vector2(Scalar(coordinates[corners[indexMinus].first].x()), Scalar(coordinates[corners[indexMinus].first].y())),
										Vector2(Scalar(coordinates[corners[index].first].x()), Scalar(coordinates[corners[index].first].y())),
										Vector2(Scalar(coordinates[corners[indexPlus].first].x()), Scalar(coordinates[corners[indexPlus].first].y())));

			bool intersection = false;

			if (!mayForce)
			{
				// check whether any concave corner lies inside the triangle to be cut off
				for (unsigned int n = 0; n < corners.size(); ++n)
				{
					if (n != indexMinus && n != index && n != indexPlus
						&& !corners[n].second
						&& triangle.isValid()
						&& triangle.isInside(Vector2(Scalar(coordinates[corners[n].first].x()), Scalar(coordinates[corners[n].first].y()))))
					{
						intersection = true;
						break;
					}
				}
			}

			if (!intersection)
			{
				result.push_back(IndexTriangle(corners[indexMinus].first, corners[index].first, corners[indexPlus].first));
				corners.erase(corners.begin() + index);
				ocean_assert(corners.size() >= 2);

				// the convex or concave states of both neighbor corners must be recalculated

				// previous minus corner
				const unsigned int iMinus0 = modulo(int(indexMinus - 1), int(corners.size()));
				const unsigned int i0 = modulo(int(indexMinus), int(corners.size()));
				const unsigned int iPlus0 = modulo(int(indexMinus + 1), int(corners.size()));

				const Advanced::PixelLineT<T> lMinus0(coordinates[corners[iMinus0].first], coordinates[corners[i0].first]);
				const Advanced::PixelLineT<T> lPlus0(coordinates[corners[iPlus0].first], coordinates[corners[i0].first]);

				const TSigned signCorner0 = TSigned(lPlus0 * lMinus0);

				corners[i0].second = (counterClockwise && signCorner0 <= TSigned(0)) || (!counterClockwise && signCorner0 > TSigned(0));

				// previous plus corner
				const unsigned int iMinus1 = modulo(int(index - 1), int(corners.size()));
				const unsigned int i1 = modulo(int(index), int(corners.size()));
				const unsigned int iPlus1 = modulo(int(index + 1), int(corners.size()));

				const Advanced::PixelLineT<T> lMinus1(coordinates[corners[iMinus1].first], coordinates[corners[i1].first]);
				const Advanced::PixelLineT<T> lPlus1(coordinates[corners[iPlus1].first], coordinates[corners[i1].first]);

				const TSigned signCorner1 = TSigned(lPlus1 * lMinus1);

				corners[i1].second = (counterClockwise && signCorner1 <= TSigned(0)) || (!counterClockwise && signCorner1 > TSigned(0));

				invalidIterations = 0;
				mayForce = false;

				continue;
			}
		}

		++index;
		++invalidIterations;

		if (invalidIterations > corners.size())
		{
			if (forceTriangulation)
			{
				mayForce = true;

				if (triangulationForced)
				{
					*triangulationForced = true;
				}
			}
			else
			{
				return IndexTriangles();
			}
		}
	}

	return result;
}

template Triangulation::IndexTriangles OCEAN_CV_SEGMENTATION_EXPORT Triangulation::triangulate<int32_t>(const PixelContourT<int32_t>& pixelContour, const bool forceTriangulation, bool* triangulationForced);
template Triangulation::IndexTriangles OCEAN_CV_SEGMENTATION_EXPORT Triangulation::triangulate<int64_t>(const PixelContourT<int64_t>& pixelContour, const bool forceTriangulation, bool* triangulationForced);
template Triangulation::IndexTriangles OCEAN_CV_SEGMENTATION_EXPORT Triangulation::triangulate<uint32_t>(const PixelContourT<uint32_t>& pixelContour, const bool forceTriangulation, bool* triangulationForced);
template Triangulation::IndexTriangles OCEAN_CV_SEGMENTATION_EXPORT Triangulation::triangulate<uint64_t>(const PixelContourT<uint64_t>& pixelContour, const bool forceTriangulation, bool* triangulationForced);

}

}

}
