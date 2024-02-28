// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/geometry/PerspectivePose.h"

namespace Ocean
{

namespace Geometry
{

bool PerspectivePose::calculateCosineSolutions(const Scalar x1, const Scalar x1_2, const Scalar cos12, const Scalar d12_2, Scalar& x2a, Scalar& x2b)
{
	// substitution of x1 into x1^2 + x2^2 - 2 x1 x2 cos12 - d12^2 = 0 results in two solutions:
	// x2a = cos12 * x1 + sqrt(d12^2 + (-1 + cos12^2) * x1^2)
	// x2b = cos12 * x1 - sqrt(d12^2 + (-1 + cos12^2) * x1^2)

	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isEqual(x1 * x1, x1_2));

	const Scalar innerSqrt = d12_2 + (-1 + cos12 * cos12) * x1_2;
	Scalar sqrtValue = 0;

	if (innerSqrt < 0)
	{
		if (Numeric::isNotWeakEqualEps(innerSqrt))
			return false;
	}
	else
		sqrtValue = Numeric::sqrt(innerSqrt);

	x2a = cos12 * x1 + sqrtValue;
	x2b = cos12 * x1 - sqrtValue;

	ocean_assert(Numeric::isWeakEqual(x1_2 + x2a * x2a - 2 * x1 * x2a * cos12, d12_2));
	ocean_assert(Numeric::isWeakEqual(x1_2 + x2b * x2b - 2 * x1 * x2b * cos12, d12_2));

	return true;
}

}

}
