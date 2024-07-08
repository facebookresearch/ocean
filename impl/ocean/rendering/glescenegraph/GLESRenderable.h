/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_RENDERABLE_H
#define META_OCEAN_RENDERING_GLES_RENDERABLE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/Renderable.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
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
 * This class is the base class for all GLESceneGraph renderable objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESRenderable :
	virtual public GLESObject,
	virtual public Renderable
{
	public:

		/**
		 * Returns the bounding box of this renderable.
		 * @return The renderable's bounding box, invalid if the renderable does not contain any geometry
		 */
		inline const BoundingBox& boundingBox() const;

		/**
		 * Renders the renderable node, the shader program is determined automatically.
		 * @param framebuffer The framebuffer in which the renderable will be rendered
		 * @param projectionMatrix The projection matrix to be applied, must be valid
		 * @param camera_T_object The transformation between object and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera, must be valid
		 * @param normalMatrix The normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 * @param attributeSet The attributes defining the appearance of this
		 * @param lights The lights used the render this renderable, can be empty
		 */
		virtual void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) = 0;

		/**
		 * Renders the renderable node with a given shader program.
		 * @param projectionMatrix The projection matrix to be applied, must be valid
		 * @param camera_T_object The transformation between object and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera, must be valid
		 * @param normalMatrix The normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 * @param shaderProgram The shader program to be used, must be compiled
		 */
		virtual void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) = 0;

	protected:

		/**
		 * Creates a new GLESceneGraph renderable object.
		 */
		GLESRenderable();

		/**
		 * Destructs a GLESceneGraph renderable object.
		 */
		~GLESRenderable() override;

	protected:

		/// The renderable's bounding box.
		BoundingBox boundingBox_;
};

inline const BoundingBox& GLESRenderable::boundingBox() const
{
	return boundingBox_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_RENDERABLE_H
