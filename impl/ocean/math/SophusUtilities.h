/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SOPHUS_UTILITIES_H
#define META_OCEAN_MATH_SOPHUS_UTILITIES_H

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"

#include <sophus/se3.hpp>
#include <sophus/sim3.hpp>

namespace Ocean
{

/**
 * This class implements utilitiy functions for Sophus.
 * @ingroup math
 */
class SophusUtilities
{
	public:

		/**
		 * Converts a Sophus::SE3 object to an Ocean HomogenousMatrix4 object.
		 * @param sophusSE The Sophus object to be converted
		 * @return The resulting Ocean object
		 * @tparam TSource The data type of the source SE elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline HomogenousMatrixT4<TTarget> toHomogenousMatrix4(const Sophus::SE3<TSource>& sophusSE);

		/**
		 * Converts a Sophus::Sim3 object to an Ocean HomogenousMatrix4 object.
		 * @param sophusSim The Sophus object to be converted
		 * @return The resulting Ocean object
		 * @tparam TSource The data type of the source SE elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline HomogenousMatrixT4<TTarget> toHomogenousMatrix4(const Sophus::Sim3<TSource>& sophusSim);

		/**
		 * Converts an Ocean HomogenousMatrix4 object to a Sophus::SE3 object.
		 * @param homogenousMatrix The Ocean homgenous matrix object to convert, must be valid
		 * @return The resulting Sophus::SE3 object
		 * @tparam TSource The data type of the source matrix elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target SE elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline Sophus::SE3<TTarget> toSE3(const HomogenousMatrixT4<TSource>& homogenousMatrix);
};

template <typename TSource, typename TTarget>
inline HomogenousMatrixT4<TTarget> SophusUtilities::toHomogenousMatrix4(const Sophus::SE3<TSource>& sophusSE)
{
	const Sophus::Matrix4<TSource> sophusMatrix = sophusSE.matrix();

	ocean_assert(NumericT<TSource>::isEqualEps(sophusMatrix(3, 0)));
	ocean_assert(NumericT<TSource>::isEqualEps(sophusMatrix(3, 1)));
	ocean_assert(NumericT<TSource>::isEqualEps(sophusMatrix(3, 2)));
	ocean_assert(NumericT<TSource>::isEqual(sophusMatrix(3, 3), TSource(1)));

	const VectorT3<TTarget> xAxis(TTarget(sophusMatrix(0, 0)), TTarget(sophusMatrix(1, 0)), TTarget(sophusMatrix(2, 0)));
	const VectorT3<TTarget> yAxis(TTarget(sophusMatrix(0, 1)), TTarget(sophusMatrix(1, 1)), TTarget(sophusMatrix(2, 1)));
	const VectorT3<TTarget> zAxis(TTarget(sophusMatrix(0, 2)), TTarget(sophusMatrix(1, 2)), TTarget(sophusMatrix(2, 2)));
	const VectorT3<TTarget> translation(TTarget(sophusMatrix(0, 3)), TTarget(sophusMatrix(1, 3)), TTarget(sophusMatrix(2, 3)));

	return HomogenousMatrixT4<TTarget>(xAxis, yAxis, zAxis, translation);
}

template <typename TSource, typename TTarget>
inline HomogenousMatrixT4<TTarget> SophusUtilities::toHomogenousMatrix4(const Sophus::Sim3<TSource>& sophusSim)
{
	const Sophus::Matrix<TSource, 4, 4> matrix(sophusSim.matrix());

	return HomogenousMatrixT4<TTarget>(matrix.data(), matrix.IsRowMajor);
}

template <typename TSource, typename TTarget>
inline Sophus::SE3<TTarget> SophusUtilities::toSE3(const HomogenousMatrixT4<TSource>& homogenousMatrix)
{
	const QuaternionT<TSource> quaternion = homogenousMatrix.rotation();
	const VectorT3<TSource> translation = homogenousMatrix.translation();

	const Eigen::Quaternion<TTarget> eigenQuaternion(TTarget(quaternion.w()), TTarget(quaternion.x()), TTarget(quaternion.y()), TTarget(quaternion.z()));
	const Eigen::Matrix<TTarget, 3, 1, 0> eigenTranslation(TTarget(translation.x()), TTarget(translation.y()), TTarget(translation.z()));

	return Sophus::SE3<TTarget>(eigenQuaternion, eigenTranslation);
}

}

#endif // META_OCEAN_MATH_SOPHUS_UTILITIES_H
