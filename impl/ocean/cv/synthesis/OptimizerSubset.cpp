/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/OptimizerSubset.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool OptimizerSubset::invoke(const unsigned int radii, const unsigned int iterations, const unsigned int maxSpatialCost, Worker* worker, const bool applyInitialMapping) const
{
	if (layer_.frame().numberPlanes() != 1u)
	{
		return false;
	}

	const unsigned int firstColumn = layer_.boundingBox() ? layer_.boundingBox().left() : 0u;
	const unsigned int numberColumns = layer_.boundingBox() ? layer_.boundingBox().width() : layer_.width();

	const unsigned int firstRow = layer_.boundingBox() ? layer_.boundingBox().top() : 0u;
	const unsigned int numberRows = layer_.boundingBox() ? layer_.boundingBox().height() : layer_.height();

	ocean_assert(firstColumn + numberColumns <= layer_.width());
	ocean_assert(firstRow + numberRows <= layer_.height());

	if (applyInitialMapping)
	{
		layer_.mapping().applyMapping(layer_.frame(), layer_.mask(), firstColumn, numberColumns, firstRow, numberRows, worker);
	}

	bool downIsMain = true;

	for (unsigned int n = 0; n < 2u * iterations; ++n)
	{
		const unsigned int rowOffset = RandomI::random(randomGenerator_, numberRows - 1u);

		if (worker)
		{
			worker->executeFunction(Worker::Function::create(*this, &OptimizerSubset::optimizeSubset, radii, maxSpatialCost, firstRow, numberRows, downIsMain, firstColumn, numberColumns, rowOffset, 0u, 0u, 0u), firstRow, numberRows, 8u, 9u, 10u, 10u);
		}
		else
		{
			optimizeSubset(radii, maxSpatialCost, firstRow, numberRows, downIsMain, firstColumn, numberColumns, rowOffset, firstRow, numberRows, 0u);
		}

		downIsMain = !downIsMain;
	}

	return true;
}

}

}

}
