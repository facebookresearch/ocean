/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/ColorChannelCurve.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

ColorChannelCurve::ColorChannelCurve(const TransformationType type)
{
	ocean_assert(type == TT_LINEAR_SRGB || type == TT_IDENTITY);

	switch (type)
	{
		case TT_LINEAR_SRGB:
			setTransformation(TransformationFunction::createStatic(&ColorChannelCurve::linearizeSRGB));
			break;

		default:
			setTransformation(TransformationFunction::createStatic(&ColorChannelCurve::identity));
	}
}

void ColorChannelCurve::setTransformation(const TransformationFunction& transformFunction)
{
	ocean_assert(transformFunction);

	for (unsigned int i = 0u; i <= 255u; i++)
	{
		transformData[i] = transformFunction((unsigned char)(i));
	}
}

}

}

}
