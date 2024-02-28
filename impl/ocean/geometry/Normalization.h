// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_GEOMETRY_NORMALIZATION_H
#define META_OCEAN_GEOMETRY_NORMALIZATION_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements functions to normalize geometric data.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Normalization
{
	public:

		/**
		 * Normalizes the given 2D image points so that the root of the mean square distance of the normalized 2D points to the origin is equal to sqrt(2).
		 * This function will return the identity transformation if no normalization can be determined.
		 * @param points The 2D points to normalize, can be invalid if 'number == 0'
		 * @param number The number of points to be normalized, with range [0, infinity)
		 * @param points_T_normalizedPoints Optional resulting transformation allowing to transform the normalized image points back to not-normalized image points
		 * @return The transformation matrix which was used to normalize the image points, will be normalizedPoints_T_points
		 */
		static SquareMatrix3 calculateNormalizedPoints(Vector2* points, const size_t number, SquareMatrix3* points_T_normalizedPoints = nullptr);

		/**
		 * Normalizes a given 3x3 transformation matrix which is defined up to a scale factor forcing a 1 in the lower right matrix corner after normalization.
		 * @param transformation The transformation to normalize, with lower right corner not zero
		 * @return The resulting normalized transformation
		 */
		static SquareMatrix3 normalizedTransformation(const SquareMatrix3& transformation);

		/**
		 * Normalizes a given 3x3 transformation matrix which is defined up to a scale factor forcing a 1 in the lower right matrix corner after normalization.
		 * @param transformation The transformation to normalize, with lower right corner not zero
		 */
		static void normalizeTransformation(SquareMatrix3& transformation);
};

}

}

#endif // META_OCEAN_GEOMETRY_NORMALIZATION_H
