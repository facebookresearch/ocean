/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static SquareMatrixT3<T> normalizedTransformation(const SquareMatrixT3<T>& transformation);

		/**
		 * Normalizes a given 3x3 transformation matrix which is defined up to a scale factor forcing a 1 in the lower right matrix corner after normalization.
		 * @param transformation The transformation to normalize, with lower right corner not zero
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static void normalizeTransformation(SquareMatrixT3<T>& transformation);
};

template <typename T>
SquareMatrixT3<T> Normalization::normalizedTransformation(const SquareMatrixT3<T>& transformation)
{
	ocean_assert(NumericT<T>::isNotEqualEps(transformation[8]));

	if (NumericT<T>::isNotEqualEps(transformation[8]))
	{
		const T factor = T(1) / transformation[8];

		return SquareMatrixT3<T>(transformation[0] * factor, transformation[1] * factor, transformation[2] * factor,
								transformation[3] * factor, transformation[4] * factor, transformation[5] * factor,
								transformation[6] * factor, transformation[7] * factor, 1);
	}

	return transformation;
}

template <typename T>
void Normalization::normalizeTransformation(SquareMatrixT3<T>& transformation)
{
	ocean_assert(NumericT<T>::isNotEqualEps(transformation[8]));

	if (NumericT<T>::isNotEqualEps(transformation[8]))
	{
		transformation *= T(1) / transformation[8];
	}
}

}

}

#endif // META_OCEAN_GEOMETRY_NORMALIZATION_H
