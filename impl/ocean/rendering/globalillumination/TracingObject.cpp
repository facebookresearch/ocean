/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/TracingObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

TracingObject::~TracingObject()
{
	// nothing to do here
}

void TracingObject::setLightSources(const LightSources& lightSources)
{
	ocean_assert(lightSources_.empty());
	lightSources_ = lightSources;
}

void TracingObject::setAttributes(const AttributeSetRef& attributes)
{
	ocean_assert(attributes_.isNull());
	attributes_ = attributes;

	material_ = nullptr;
	textures_ = nullptr;

	if (attributes)
	{
		const MaterialRef materialRef(attributes->attribute(Object::TYPE_MATERIAL));

		if (materialRef)
		{
			material_ = dynamic_cast<GIMaterial*>(&*materialRef);
		}

		const SmartObjectRef<GITextures> texturesRef(attributes->attribute(Object::TYPE_TEXTURES));

		if (texturesRef)
		{
			textures_ = &*texturesRef;
		}
	}
}

}

}

}
