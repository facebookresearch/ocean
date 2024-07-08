/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/DiscreteCosineTransform.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

void DiscreteCosineTransform::transform2(const Scalar* data, const unsigned int xElements, const unsigned int yElements, const unsigned int stride, Scalar* coefficients)
{
	ocean_assert(data && coefficients);
	ocean_assert(xElements != 0u && yElements != 0u);

	const Scalar pi_xElements = Numeric::pi() / Scalar(xElements);
	const Scalar pi_yElements = Numeric::pi() / Scalar(yElements);

	const Scalar xNormalization0 = Numeric::sqrt(Scalar(1) / Scalar(xElements));
	const Scalar yNormalization0 = Numeric::sqrt(Scalar(1) / Scalar(yElements));

	const Scalar xNormalizationK = Numeric::sqrt(Scalar(2) / Scalar(xElements));
	const Scalar yNormalizationK = Numeric::sqrt(Scalar(2) / Scalar(yElements));

	for (unsigned int ky = 0u; ky < yElements; ++ky)
		for (unsigned int kx = 0u; kx < xElements; ++kx)
		{
			Scalar value = Scalar(0);

			for (unsigned int x = 0u; x < xElements; ++x)
			{
				const Scalar cosValueX = Numeric::cos(pi_xElements * (Scalar(x) + Scalar(0.5)) * Scalar(kx)) * (kx == 0u ? xNormalization0 : xNormalizationK);

				for (unsigned int y = 0u; y < yElements; ++y)
				{
					const Scalar cosValueY = Numeric::cos(pi_yElements * (Scalar(y) + Scalar(0.5)) * Scalar(ky)) * (ky == 0u ? yNormalization0 : yNormalizationK);

					value += data[y * stride + x] * cosValueX * cosValueY;
				}
			}

			*coefficients++ = value;
		}
}

void DiscreteCosineTransform::iTransform2(const Scalar* coefficients, const unsigned int xElements, const unsigned int yElements, const unsigned int stride, Scalar* data)
{
	ocean_assert(coefficients && data);
	ocean_assert(xElements != 0u && yElements != 0u);

	const Scalar pi_xElements = Numeric::pi() / Scalar(xElements);
	const Scalar pi_yElements = Numeric::pi() / Scalar(yElements);

	const Scalar xNormalization0 = Numeric::sqrt(Scalar(1) / Scalar(xElements));
	const Scalar yNormalization0 = Numeric::sqrt(Scalar(1) / Scalar(yElements));

	const Scalar xNormalizationK = Numeric::sqrt(Scalar(2) / Scalar(xElements));
	const Scalar yNormalizationK = Numeric::sqrt(Scalar(2) / Scalar(yElements));

	for (unsigned int y = 0u; y < yElements; ++y)
		for (unsigned int x = 0u; x < xElements; ++x)
		{
			Scalar value = Scalar(0);

			for (unsigned int ky = 0u; ky < yElements; ++ky)
			{
				const Scalar cosValueY = Numeric::cos(pi_yElements * (Scalar(y) + Scalar(0.5)) * Scalar(ky)) * (ky == 0u ? yNormalization0 : yNormalizationK);

				for (unsigned int kx = 0u; kx < xElements; ++kx)
				{
					const Scalar cosValueX = Numeric::cos(pi_xElements * (Scalar(x) + Scalar(0.5)) * Scalar(kx)) * (kx == 0u ? xNormalization0 : xNormalizationK);

					value += coefficients[ky * xElements + kx] * cosValueX * cosValueY;
				}
			}

			data[y * stride + x] = value;
		}
}

}
