// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
