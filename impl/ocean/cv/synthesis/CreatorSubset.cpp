/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/CreatorSubset.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool CreatorSubset::invoke(Worker* worker) const
{
	const PixelBoundingBox& layerBoundingBox = layer_.boundingBox();

	const unsigned int firstColumn = layerBoundingBox ? layerBoundingBox.left() : 0u;
	const unsigned int numberColumns = layerBoundingBox ? layerBoundingBox.width() : layer_.width();

	const unsigned int firstRow = layerBoundingBox ? layerBoundingBox.top() : 0u;
	const unsigned int numberRows = layerBoundingBox ? layerBoundingBox.height() : layer_.height();

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &CreatorSubset::createSubset, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 2u, 3u, 10u);
	}
	else
	{
		createSubset(firstColumn, numberColumns, firstRow, numberRows);
	}

	return true;
}

}

}

}
