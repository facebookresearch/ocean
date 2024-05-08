/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/CreatorInpaintingContentI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

void CreatorInpaintingContentI1::createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(target_.numberPlanes() == 1u && target_.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (target_.channels())
	{
		case 1u:
			createSubsetChannels<1u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 2u:
			createSubsetChannels<2u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 3u:
			createSubsetChannels<3u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 4u:
			createSubsetChannels<4u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		default:
			ocean_assert(false && "Invalid frame type.");
	}
}

}

}

}
