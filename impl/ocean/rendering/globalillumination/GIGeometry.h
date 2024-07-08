/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_GEOMETRY_H
#define META_OCEAN_RENDERING_GI_GEOMETRY_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GINode.h"

#include "ocean/rendering/Geometry.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a geometry node.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIGeometry :
	virtual public GINode,
	virtual public Geometry
{
	friend class GIFactory;

	public:

		/**
		 * Adds a new renderable object to this geometry.
		 * @see Geometry::addRenderable().
		 */
		void addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes) override;

		/**
		 * Removes a renderable object connected to this geometry.
		 * @see Geometry::removeRenderalbe().
		 */
		void removeRenderable(const RenderableRef& renderable) override;

		/**
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation = true) const override;

		/**
		 * Returns the bounding sphere of this node.
		 * @see Node::boundingSphere().
		 */
		BoundingSphere boundingSphere(const bool involveLocalTransformation = true) const override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GINode::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new geometry object.
		 */
		GIGeometry();

		/**
		 * Destructs a geometry object.
		 */
		~GIGeometry() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_GEOMETRY_H
