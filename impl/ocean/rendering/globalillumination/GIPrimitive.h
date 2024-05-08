/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_PRIMITIVE_H
#define META_OCEAN_RENDERING_GI_PRIMITIVE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIRenderable.h"

#include "ocean/rendering/Primitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements the base class for all primitive objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIPrimitive :
	virtual public GIRenderable,
	virtual public Primitive
{
	protected:

		/**
		 * Creates a new primitive object.
		 */
		GIPrimitive();

		/**
		 * Destructs a primitive object.
		 */
		~GIPrimitive() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_PRIMITIVE_H
