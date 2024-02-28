// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/globalillumination/GIAttributeSet.h"
#include "ocean/rendering/globalillumination/GIAttribute.h"
#include "ocean/rendering/globalillumination/GIFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIAttributeSet::GIAttributeSet() :
	GIObject(),
	AttributeSet()
{
	// nothing to do here
}

GIAttributeSet::~GIAttributeSet()
{
	// nothing to do here
}

void GIAttributeSet::addAttribute(const AttributeRef& attribute)
{
	AttributeSet::addAttribute(attribute);
}

void GIAttributeSet::removeAttribute(const AttributeRef& attribute)
{
	AttributeSet::removeAttribute(attribute);
}

}

}

}
