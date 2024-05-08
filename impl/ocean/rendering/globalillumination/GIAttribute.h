/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GI_ATTRIBUTE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"

#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class GIFramebuffer;

/**
 * This class wraps a Global Illumination attribute object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIAttribute :
	virtual public GIObject,
	virtual public Attribute
{
	protected:

		/**
		 * Creates a new Global Illumination attribute object.
		 */
		GIAttribute();

		/**
		 * Destructs a Global Illumination attribute object.
		 */
		~GIAttribute() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_ATTRIBUTE_H
