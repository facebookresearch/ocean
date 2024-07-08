/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_LINES_H
#define META_OCEAN_RENDERING_GLES_LINES_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESIndependentPrimitive.h"

#include "ocean/rendering/Lines.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph lines object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESLines :
	virtual public GLESIndependentPrimitive,
	virtual public Lines
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the indices of the used vertex lines.
		 * @see Lines::indices().
		 */
		VertexIndices indices() const override;

		/**
		 * Returns the number of line indices defined in this primitive.
		 * @see Lines::numberIndices().
		 */
		unsigned int numberIndices() const override;

		/**
		 * Sets the indices of the used vertex lines.
		 * @see Lines::setIndices().
		 */
		void setIndices(const VertexIndices& indices) override;

		/**
		 * Sets the indices of the used vertex lines.
		 * @see Lines::setIndices().
		 */
		void setIndices(const unsigned int numberImplicitLines) override;

		/**
		 * Renders the lines defined by the associated vertex set and the defined indices.
		 * @see GLESRenderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) override;

		/**
		 * Draws all lines with the currently bound shader program.
		 */
		void drawLines();

	protected:

		/**
		 * Creates a new GLESceneGraph lines object.
		 */
		GLESLines();

		/**
		 * Destructs a GLESceneGraph lines object.
		 */
		~GLESLines() override;

		/**
		 * Releases the internal vertex buffer object containing the line indices.
		 */
		void release();

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	protected:

		/// Vertex buffer object for line indices.
		GLuint vboIndices_ = 0u;

		/// The line indices, in case lines are defined via explicit line indices.
		VertexIndices explicitLineIndices_;

		/// Number of lines, in case lines are defined implicitly based on lines with consecutive indices.
		unsigned int numberImplicitLines_ = 0u;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_LINES_H
