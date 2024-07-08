/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_PERSPECTIVE_POSE_H
#define META_OCEAN_GEOMETRY_PERSPECTIVE_POSE_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class is the base class for all perspective pose classes.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT PerspectivePose
{
	protected:

		/**
		 * Calculates the two possible solutions for the law of cosines.
		 * @param x1 First leg distance
		 * @param x1_2 Square of the first leg distance
		 * @param cos12 Cosine value of the angle between the two legs
		 * @param d12_2 Square of the far distance
		 * @param x2a First solution of the second leg distance
		 * @param x2b Second solution of the second leg distance
		 * @return True, if succeeded
		 * @tparam T The data type of the scalar
		 */
		template <typename T>
		static bool calculateCosineSolutions(const T x1, const T x1_2, const T cos12, const T d12_2, T& x2a, T& x2b);

		/**
		 * Returns the result of the law of cosines.
		 * For valid parameters this result should be zero.
		 * The law is: 0 = x1^2 + x2^2 - 2 * x1 * x2 * cos12 - d12^2
		 * @param x1 First leg distance
		 * @param x1_2 Square of the first leg distance
		 * @param x2 Second leg distance
		 * @param x2_2 Square of the second leg distance
		 * @param cos12 Cosine value of the angle between the two legs
		 * @param d12_2 Square of the far distance
		 * @return Result of the law of cosines
		 * @tparam T The data type of the scalar
		 */
		template <typename T>
		static inline T calculateCosineResults(const T x1, const T x1_2, const T x2, const T x2_2, const T cos12, const T d12_2);
};

template <typename T>
inline T PerspectivePose::calculateCosineResults(const T x1, const T x1_2, const T x2, const T x2_2, const T cos12, const T d12_2)
{
	return x1_2 + x2_2 - 2 * x1 * x2 * cos12 - d12_2;
}

}

}

#endif // META_OCEAN_GEOMETRY_PERSPECTIVE_POSE_H
