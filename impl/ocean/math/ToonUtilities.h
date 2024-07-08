/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_TOON_UTILITIES_H
#define META_OCEAN_MATH_TOON_UTILITIES_H

#include "ocean/math/HomogenousMatrix4.h"

#include <TooN/TooN.h>
#include <TooN/se3.h>

namespace Ocean
{

/**
 * This class implements utilitiy functions for TooN.
 * @ingroup math
 */
class ToonUtilities
{
	public:

		/**
		 * Converts a TooN::SE3 object to an Ocean HomogenousMatrix4 object.
		 * @param toonSE The TooN object to be converted
		 * @return The resulting Ocean object
		 * @tparam TSource The data type of the source matrix elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline HomogenousMatrixT4<TTarget> toHomogenousMatrix4(const TooN::SE3<TSource>& toonSE);

		/**
		 * Converts an Ocean HomogenousMatrix4 object to a TooN::SE3 object.
		 * @param matrix The Ocean object to convert
		 * @return The resulting TooN object
		 * @tparam TSource The data type of the source matrix elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline TooN::SE3<TTarget> toSE3(const HomogenousMatrixT4<TSource>& matrix);
};

template <typename TSource, typename TTarget>
inline HomogenousMatrixT4<TTarget> ToonUtilities::toHomogenousMatrix4(const TooN::SE3<TSource>& toonSE)
{
	const auto& translation = toonSE.get_translation();
	const auto& rotationMatrix = toonSE.get_rotation().get_matrix();

	HomogenousMatrixT4<TTarget> result(true);

	for (unsigned int r = 0u; r < 3u; ++r)
	{
		for (unsigned int c = 0u; c < 3u; ++c)
		{
			result(r, c) = TTarget(rotationMatrix(r, c));
		}
	}

	result(0, 3) = TTarget(translation[0]);
	result(1, 3) = TTarget(translation[1]);
	result(2, 3) = TTarget(translation[2]);

	return result;
}

template <typename TSource, typename TTarget>
inline TooN::SE3<TTarget> ToonUtilities::toSE3(const HomogenousMatrixT4<TSource>& matrix)
{
	TooN::Matrix<3, 3, TTarget> toonRotationMatrix;

	for (unsigned int r = 0u; r < 3u; ++r)
	{
		for (unsigned int c = 0u; c < 3u; ++c)
		{
			toonRotationMatrix(r, c) = TTarget(matrix(r, c));
		}
	}

	const TooN::Vector<3, TTarget> toonTranslationVector = TooN::makeVector<TTarget>(TTarget(matrix(0, 3)), TTarget(matrix(1, 3)), TTarget(matrix(2, 3)));

	return TooN::SE3<TTarget>(TooN::SO3<TTarget>(toonRotationMatrix), toonTranslationVector);
}

}

#endif // META_OCEAN_MATH_TOON_UTILITIES_H
