/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_POINTS_H
#define META_OCEAN_RENDERING_GLES_POINTS_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESIndependentPrimitive.h"

#include "ocean/rendering/Points.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph points object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESPoints :
	virtual public GLESIndependentPrimitive,
	virtual public Points
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the indices of the used vertex points.
		 * @see Points::indices().
		 */
		VertexIndices indices() const override;

		/**
		 * Returns the number of point indices defined in this primitive.
		 * @see Points::numberIndices().
		 */
		unsigned int numberIndices() const override;

		/**
		 * Returns the size in pixels at which all points will be rendered.
		 * @see Points::pointSize().
		 */
		Scalar pointSize() const override;

		/**
		 * Sets the indices of the used vertex points.
		 * @see Points::setIndices().
		 */
		void setIndices(const VertexIndices& indices) override;

		/**
		 * Sets the indices of the used vertex points.
		 * @see Points::setIndices().
		 */
		void setIndices(const unsigned int numberImplicitPoints) override;

		/**
		 * Sets the size in pixels at which all points will be rendered.
		 * @see Points::setPointSize().
		 */
		void setPointSize(const Scalar size) override;

		/**
		 * Renders the points defined by the associated vertex set and the defined indices.
		 * @see GLESRenderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) override;

		/**
		 * Draws all points with the currently bound shader program.
		 */
		void drawPoints();

	protected:

		/**
		 * Creates a new GLESceneGraph points object.
		 */
		GLESPoints();

		/**
		 * Destructs a GLESceneGraph points object.
		 */
		~GLESPoints() override;

		/**
		 * Releases the internal vertex buffer object containing the point indices.
		 */
		void release();

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	protected:

		/// Vertex buffer object for point indices.
		GLuint vboIndices_ = 0u;

		/// The point indices, in case points are defined via explicit point indices.
		VertexIndices explicitPointIndices_;

		/// Number of points, in case points are defined implicitly based on points with consecutive indices.
		unsigned int numberImplicitPoints_ = 0u;

		/// The size of all points (the diameter), in pixels, with range (0, infinity).
		Scalar pointSize_ = Scalar(1);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_POINTS_H
