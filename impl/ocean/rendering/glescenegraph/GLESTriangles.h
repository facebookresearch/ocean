/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TRIANGLES_H
#define META_OCEAN_RENDERING_GLES_TRIANGLES_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESIndependentPrimitive.h"

#include "ocean/rendering/Triangles.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph triangles object.
 * This class holds independent and separated triangles and is able to render them.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTriangles :
	virtual public GLESIndependentPrimitive,
	virtual public Triangles
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the faces of this triangles object.
		 * @see Triangles::faces().
		 */
		TriangleFaces faces() const override;

		/**
		 * Returns the number of triangle faces defined in this primitive.
		 * @see Triangles::numberFaces().
		 */
		unsigned int numberFaces() const override;

		/**
		 * Sets the faces of this triangles object.
		 * @see Triangles::setFaces().
		 */
		void setFaces(const TriangleFaces& faces) override;

		/**
		 * Sets the faces of this triangles object.
		 * @see Triangles::setFaces().
		 */
		void setFaces(const unsigned int numberImplicitTriangleFaces) override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Renders the renderable node.
		 * @see Renderable::render().
		 */
		void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) override;

		/**
		 * Draws all triangles with the currently bound shader program.
		 */
		void drawTriangles();

		/**
		 * Draws a subset of all triangles with the currently bound shader program.
		 * @param firstTriangle The index of the first triangle to draw, with range [0, numberFaces() - 1]
		 * @param numberTriangles The number of triangles to draw, with range [0, numberFaces() - firstTriangle]
		 */
		void drawTriangles(const unsigned int firstTriangle, const unsigned int numberTriangles);

	protected:

		/**
		 * Creates a new GLESceneGraph triangles object.
		 */
		GLESTriangles();

		/**
		 * Destructs a GLESceneGraph triangles object.
		 */
		~GLESTriangles() override;

		/**
		 * Releases the internal vertex buffer object containing the triangle indices.
		 */
		void release();

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	protected:

		/// Vertex buffer object for triangle indices.
		GLuint vboIndices_ = 0u;

		/// The vertex indices, in case triangles are defined via vertex indices, three for each triangle.
		TriangleFaces explicitTriangleFaces_;

		/// Number of triangle faces, in case triangles are defined implicitly based on vertices with consecutive indices.
		unsigned int numberImplicitTriangleFaces_ = 0u;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TRIANGLES_H
