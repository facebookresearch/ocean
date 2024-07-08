/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SCENE_H
#define META_OCEAN_RENDERING_GLES_SCENE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTransform.h"

#include "ocean/rendering/Scene.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph scene object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESScene :
	virtual public GLESTransform,
	virtual public Scene
{
	friend class GLESFactory;

	public:

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph scene object.
		 */
		GLESScene();

		/**
		 * Destructs a GLESceneGraph scene object.
		 */
		~GLESScene() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SCENE_H
