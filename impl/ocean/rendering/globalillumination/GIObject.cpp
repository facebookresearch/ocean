/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIObject.h"
#include "ocean/rendering/globalillumination/GIEngine.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIObject::GIObject()
{
	// nothing to do here
}

GIObject::~GIObject()
{
	// nothing to do here
}

const std::string& GIObject::engineName() const
{
	return globalIlluminationEngineName();
}

}

}

}
