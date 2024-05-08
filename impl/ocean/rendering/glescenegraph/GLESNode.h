/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_NODE_H
#define META_OCEAN_RENDERING_GLES_NODE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/rendering/Node.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

// Forward declaration.
class GLESFramebuffer;

/**
 * This class implements a GLESceneGraph node object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESNode :
	virtual public GLESObject,
	virtual public Node
{
	public:

		/**
		 * Adds this node and all child nodes to a traverser.
		 * @param framebuffer The framebuffer which the traverser will use when rendering the node
		 * @param projectionMatrix The projection matrix to be applied, must be valid
		 * @param camera_T_object The transformation between object and camera, often denoted as model view matrix, must be valid
		 * @param lights The lights used the render the node and all child nodes, can be empty
		 * @param traverser The traverser to which the node will be added
		 */
		virtual void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const = 0;

		/**
		 * Returns whether the node is visible.
		 * @see Node::visible().
		 */
		bool visible() const override;

		/**
		 * Sets whether the node is visible.
		 * @see Node::setVisible().
		 */
		void setVisible(const bool visible) override;

	protected:

		/**
		 * Creates a new GLESceneGraph node object.
		 */
		GLESNode();

		/**
		 * Destructs a GLESceneGraph node object.
		 */
		~GLESNode() override;

	protected:

		/// True, if the node and all child nodes are visible.
		bool visible_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_NODE_H
