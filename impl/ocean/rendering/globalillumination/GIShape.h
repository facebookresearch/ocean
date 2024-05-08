/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_SHAPE_H
#define META_OCEAN_RENDERING_GI_SHAPE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIRenderable.h"

#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all Global Illumination shape objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIShape :
	virtual public GIRenderable,
	virtual public Shape
{
	protected:

		/**
		 * Creates a new Global Illumination shape object.
		 */
		GIShape();

		/**
		 * Destructs a Global Illumination shape object.
		 */
		~GIShape() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_SHAPE_H
