/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SWITCH_H
#define META_OCEAN_RENDERING_GLES_SWITCH_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"

#include "ocean/rendering/Switch.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements an OpenGLESceneGraph switch node.
 * @see Switch
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESSwitch :
	virtual public GLESGroup,
	virtual public Switch
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the index of the active node.
		 * @see Switch::activeNode().
		 */
		Index32 activeNode() const override;

		/**
		 * Sets the index of the active node.
		 * @see Switch::setActiveNode().
		 */
		void setActiveNode(const Index32 index) override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const override;

	private:

		/**
		 * Creates a new OpenGLESceneGraph switch object.
		 */
		GLESSwitch();

		/**
		 * Destructs an OpenGLESceneGraph switch object.
		 */
		~GLESSwitch() override;

	private:

		/// Index of the child to render.
		Index32 activeNodeIndex_ = invalidIndex;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SWITCH_H
