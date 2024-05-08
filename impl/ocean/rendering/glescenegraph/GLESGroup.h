/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_GROUP_H
#define META_OCEAN_RENDERING_GLES_GROUP_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESNode.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph group object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESGroup :
	virtual public GLESNode,
	virtual public Group
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation) const override;

		/**
		 * Adds a new child node to this group.
		 * @see Group::addChild().
		 */
		void addChild(const NodeRef& node) override;

		/**
		 * Registers a light lighting all children of this group node.
		 * @see Group::registerLight().
		 */
		void registerLight(const LightSourceRef& light) override;

		/**
		 * Removes a child node connected to this group.
		 * @see Group::removeChild().
		 */
		void removeChild(const NodeRef& node) override;

		/**
		 * Unregisters a light.
		 * @see Group::unregisterLight().
		 */
		void unregisterLight(const LightSourceRef& light) override;

		/**
		 * Removes all child nodes connected to this group.
		 * @see Group::clear().
		 */
		void clear() override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph group object.
		 */
		GLESGroup();

		/**
		 * Destructs a GLESceneGraph group object.
		 */
		~GLESGroup() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_GROUP_H
