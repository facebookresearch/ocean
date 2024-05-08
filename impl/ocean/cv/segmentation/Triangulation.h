/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_TRIANGULATION_H
#define META_OCEAN_CV_SEGMENTATION_TRIANGULATION_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/advanced/PixelTriangle.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions allowing for triangulation.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT Triangulation
{
	public:

		/**
		 * This class holds coordinate index of a triangle.
		 */
		class OCEAN_CV_SEGMENTATION_EXPORT IndexTriangle
		{
			public:

				/**
				 * Creates a new index triangle object with invalid indices.
				 */
				inline IndexTriangle();

				/**
				 * Creates a new index triangle object with three valid indices.
				 * @param first First coordinate index, with range [0, infinity)
				 * @param second Second coordinate index, with range [0, infinity)
				 * @param third Third coordinate index, with range [0, infinity)
				 */
				inline IndexTriangle(const unsigned int first, const unsigned int second, const unsigned int third);

				/**
				 * Returns whether this object is valid, which means that all three coordinate indices are different.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns the coordinate indices for the triangle.
				 * @param index The corner index for which the coordinate index will be returned, with range [0, 2]
				 * @return The triangle's coordinate indices
				 */
				inline unsigned int operator[](const unsigned int index) const;

			protected:

				/// The three coordinate indices of the triangle.
				unsigned int indices_[3];
		};

		/**
		 * Definition of a vector holding index triangle objects.
		 */
		typedef std::vector<IndexTriangle> IndexTriangles;

	private:

		/**
		 * Definition of a pair combining a pixel position index and a boolean state.
		 */
		typedef std::pair<unsigned int, bool> PixelPositionPair;

		/**
		 * Definition of a vector holding pixel position pairs.
		 */
		typedef std::vector<PixelPositionPair> PixelPositionPairs;

	public:

		/**
		 * Triangulates a given (sparse or dense) contour.
		 * Due to performance reasons and due to irregular coordinates a sparse contours is recommended.
		 * @param pixelContour The contour to be triangulated
		 * @param forceTriangulation True, to return a triangulation even if some of the coordinates does not allow a valid triangulation
		 * @param triangulationForced Optional resulting state whether the triangulation needed to be forced
		 * @return Resulting triangulation
		 * @tparam T The data type of the pixel contour, can be 'int' or 'unsigned int'
		 */
		template <typename T>
		static IndexTriangles triangulate(const PixelContourT<T>& pixelContour, const bool forceTriangulation = false, bool* triangulationForced = nullptr);

		/**
		 * Converts a given set of pixel coordinates and a corresponding triangulation into triangle objects.
		 * @param coordinates Set of pixel coordinates
		 * @param triangulation Corresponding triangulation
		 * @return Resulting triangles
		 * @tparam T The data type of the pixel coordinates, can be "int" or "unsigned int"
		 */
		template <typename T>
		static inline std::vector<Advanced::PixelTriangleT<T>> triangulation2triangles(const std::vector<PixelPositionT<T>>& coordinates, const IndexTriangles& triangulation);
};

inline Triangulation::IndexTriangle::IndexTriangle()
{
	indices_[0] = (unsigned int)(-1);
	indices_[1] = (unsigned int)(-1);
	indices_[2] = (unsigned int)(-1);
}

inline Triangulation::IndexTriangle::IndexTriangle(const unsigned int first, const unsigned int second, const unsigned int third)
{
	indices_[0] = first;
	indices_[1] = second;
	indices_[2] = third;
}

inline bool Triangulation::IndexTriangle::isValid() const
{
	return indices_[0] != indices_[1] && indices_[0] != indices_[2] && indices_[1] != indices_[2];
}

inline unsigned int Triangulation::IndexTriangle::operator[](const unsigned int index) const
{
	ocean_assert(index < 3u);
	return indices_[index];
}

template <typename T>
inline std::vector<Advanced::PixelTriangleT<T>> Triangulation::triangulation2triangles(const std::vector<PixelPositionT<T>>& coordinates, const IndexTriangles& triangulation)
{
	Advanced::PixelTriangles result;
	result.reserve(triangulation.size());

	for (IndexTriangles::const_iterator i = triangulation.begin(); i != triangulation.end(); ++i)
	{
		ocean_assert((*i)[0] < coordinates.size());
		ocean_assert((*i)[1] < coordinates.size());
		ocean_assert((*i)[2] < coordinates.size());

		result.push_back(Advanced::PixelTriangleT<T>(coordinates[(*i)[0]], coordinates[(*i)[1]], coordinates[(*i)[2]]));
	}

	return result;
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_TRIANGULATION_H
