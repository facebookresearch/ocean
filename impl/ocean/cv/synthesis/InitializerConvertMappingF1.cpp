/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerConvertMappingF1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

void InitializerConvertMappingF1::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(layerF_.width() == sourceMapping_.width());
	ocean_assert(layerF_.height() == sourceMapping_.height());

	ocean_assert(firstColumn + numberColumns <= layer_.width());
	ocean_assert(firstRow + numberRows <= layer_.height());

	Vector2* const targetMapping = layerF_.mapping()();
	const PixelPosition* const sourceMapping = sourceMapping_();

	const unsigned int layerWidth = layerF_.height();

	for (unsigned int y = firstRow; y <= firstRow + numberRows; ++y)
	{
		Vector2* target = targetMapping + y * layerWidth;
		const PixelPosition* source = sourceMapping + y * layerWidth;

		for (unsigned int x = firstColumn; x <= firstColumn + numberColumns; ++x)
		{
			target[x].x() = Scalar(source[x].x());
			target[x].y() = Scalar(source[x].y());
		}
	}
}

}

}

}
