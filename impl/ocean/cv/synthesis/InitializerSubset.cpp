/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerSubset.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool InitializerSubset::invoke(Worker* worker) const
{
	if (layer_.frame().numberPlanes() != 1u)
	{
		return false;
	}

	const PixelBoundingBox& layerBoundingBox = layer_.boundingBox();
	const unsigned int layerWidth = layer_.width();
	const unsigned int layerHeight = layer_.height();

	const unsigned int firstColumn = layerBoundingBox ? layerBoundingBox.left() : 0u;
	const unsigned int numberColumns = layerBoundingBox ? layerBoundingBox.width() : layerWidth;

	const unsigned int firstRow = layerBoundingBox ? layerBoundingBox.top() : 0u;
	const unsigned int numberRows = layerBoundingBox ? layerBoundingBox.height() : layerHeight;

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &InitializerSubset::initializeSubset, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 2u, 3u);
	}
	else
	{
		initializeSubset(firstColumn, numberColumns, firstRow, numberRows);
	}

	return true;
}

}

}

}
