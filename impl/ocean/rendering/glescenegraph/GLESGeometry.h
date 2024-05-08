/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_GEOMETRY_H
#define META_OCEAN_RENDERING_GLES_GEOMETRY_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESNode.h"

#include "ocean/rendering/Geometry.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph geometry node.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESGeometry :
	virtual public GLESNode,
	virtual public Geometry
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation) const override;

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
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph geometry object.
		 */
		GLESGeometry();

		/**
		 * Destructs a GLESceneGraph geometry object.
		 */
		~GLESGeometry() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_GEOMETRY_H
