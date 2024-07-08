/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/PerspectivePose.h"

namespace Ocean
{

namespace Geometry
{

template <typename T>
bool PerspectivePose::calculateCosineSolutions(const T x1, const T x1_2, const T cos12, const T d12_2, T& x2a, T& x2b)
{
	// substitution of x1 into x1^2 + x2^2 - 2 x1 x2 cos12 - d12^2 = 0 results in two solutions:
	// x2a = cos12 * x1 + sqrt(d12^2 + (-1 + cos12^2) * x1^2)
	// x2b = cos12 * x1 - sqrt(d12^2 + (-1 + cos12^2) * x1^2)

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isEqual(x1 * x1, x1_2));

	const T innerSqrt = d12_2 + (-1 + cos12 * cos12) * x1_2;
	T sqrtValue = 0;

	if (innerSqrt < 0)
	{
		if (NumericT<T>::isNotWeakEqualEps(innerSqrt))
		{
			return false;
		}
	}
	else
	{
		sqrtValue = NumericT<T>::sqrt(innerSqrt);
	}

	x2a = cos12 * x1 + sqrtValue;
	x2b = cos12 * x1 - sqrtValue;

	ocean_assert_accuracy(NumericT<T>::isWeakEqual(x1_2 + x2a * x2a - 2 * x1 * x2a * cos12, d12_2));
	ocean_assert_accuracy(NumericT<T>::isWeakEqual(x1_2 + x2b * x2b - 2 * x1 * x2b * cos12, d12_2));

	return true;
}

template bool OCEAN_GEOMETRY_EXPORT PerspectivePose::calculateCosineSolutions<float>(const float x1, const float x1_2, const float cos12, const float d12_2, float& x2a, float& x2b);
template bool OCEAN_GEOMETRY_EXPORT PerspectivePose::calculateCosineSolutions<double>(const double x1, const double x1_2, const double cos12, const double d12_2, double& x2a, double& x2b);

}

}
