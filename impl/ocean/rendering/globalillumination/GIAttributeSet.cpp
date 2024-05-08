/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
