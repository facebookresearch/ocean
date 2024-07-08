/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerMoveMappingI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool InitializerMoveMappingI1::invoke(Worker* /*worker*/) const
{
	if (mapping_.width() == layerI_.width() && mapping_.height() == layerI_.height())
	{
		ocean_assert(dynamic_cast<MappingI1*>(&layerI_.mapping()));
		(MappingI1&)(layerI_.mapping()) = std::move(mapping_);

		return true;
	}

	ocean_assert(false && "Invalid mapping dimension!");
	return false;
}

}

}

}
