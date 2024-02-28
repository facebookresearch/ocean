// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		 */
		static bool calculateCosineSolutions(const Scalar x1, const Scalar x1_2, const Scalar cos12, const Scalar d12_2, Scalar& x2a, Scalar& x2b);

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
		 */
		inline static Scalar calculateCosineResults(const Scalar x1, const Scalar x1_2, const Scalar x2, const Scalar x2_2, const Scalar cos12, const Scalar d12_2);
};

inline Scalar PerspectivePose::calculateCosineResults(const Scalar x1, const Scalar x1_2, const Scalar x2, const Scalar x2_2, const Scalar cos12, const Scalar d12_2)
{
	return x1_2 + x2_2 - 2 * x1 * x2 * cos12 - d12_2;
}

}

}

#endif // META_OCEAN_GEOMETRY_PERSPECTIVE_POSE_H
