/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_INDEPENDENT_PRIMITIVE_H
#define META_OCEAN_RENDERING_GI_INDEPENDENT_PRIMITIVE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIPrimitive.h"

#include "ocean/rendering/IndependentPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements the base class for all independent primitive.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIIndependentPrimitive :
	virtual public GIPrimitive,
	virtual public IndependentPrimitive
{
	protected:

		/**
		 * Creates a new independent primitive object.
		 */
		GIIndependentPrimitive();

		/**
		 * Destructs a independent primitive object.
		 */
		~GIIndependentPrimitive() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_INDEPENDENT_PRIMITIVE_H
