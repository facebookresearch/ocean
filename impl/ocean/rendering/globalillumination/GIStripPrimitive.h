/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_STRIP_PRIMITIVE_H
#define META_OCEAN_RENDERING_GI_STRIP_PRIMITIVE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIPrimitive.h"

#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all stripped primitives.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIStripPrimitive :
	virtual public GIPrimitive,
	virtual public StripPrimitive
{
	public:

		/**
		 * Returns the strips of this primitive object.
		 * @see StripPrimitive::strips().
		 */
		VertexIndexGroups strips() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph stripped primitive object.
		 */
		GIStripPrimitive();

		/**
		 * Destructs a GLESceneGraph stripped ptrimitive object.
		 */
		~GIStripPrimitive() override;

	protected:

		/// Primitive strips.
		VertexIndexGroups primitiveStrips;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_STRIP_PRIMITIVE_H
