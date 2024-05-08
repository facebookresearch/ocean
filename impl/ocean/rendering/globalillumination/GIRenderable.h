/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_RENDERABLE_H
#define META_OCEAN_RENDERING_GI_RENDERABLE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"
#include "ocean/rendering/globalillumination/RayIntersection.h"
#include "ocean/rendering/globalillumination/TracingGroup.h"

#include "ocean/math/Line3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Node.h"
#include "ocean/rendering/Renderable.h"
#include "ocean/rendering/Scene.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class GIFramebuffer;

/**
 * This class is the base class for all Global Illumination renderable objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIRenderable :
	virtual public GIObject,
	virtual public Renderable
{
	public:

		/**
		 * Returns the bounding box of this renderable object.
		 * @return Bounding box
		 */
		virtual BoundingBox boundingBox() const = 0;

		/**
		 * Returns the bounding sphere of this renderable object.
		 * @return Bounding sphere
		 */
		virtual BoundingSphere boundingSphere() const = 0;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @param group The existing group of tracing objects to which the new tracing object will be added
		 * @param modelTransform The model transformation matrix for this node
		 * @param attributes The attributes of this renderable object defining the appearance of the object
		 * @param lightSources The light sources which will be used to light this node
		 */
		virtual void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const = 0;

	protected:

		/**
		 * Creates a new Global Illumination renderable object.
		 */
		GIRenderable();

		/**
		 * Destructs a Global Illumination renderable object.
		 */
		~GIRenderable() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_RENDERABLE_H
