/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_LINE_STRIPS_H
#define META_OCEAN_RENDERING_GLES_LINE_STRIPS_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESStripPrimitive.h"

#include "ocean/rendering/LineStrips.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph line strips object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESLineStrips :
	virtual public GLESStripPrimitive,
	virtual public LineStrips
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the strips of this primitive object.
		 * @see StripPrimitive::strips().
		 */
		VertexIndexGroups strips() const override;

		/**
		 * Returns the number of line strips defined in this primitive.
		 * @see StripPrimitive::numberStrips().
		 */
		unsigned int numberStrips() const override;

		/**
		 * Sets the strips of this primitive object.
		 * @see StripPtrimitive::setStrips().
		 */
		void setStrips(const VertexIndexGroups& strips) override;

		/**
		 * Renders the line strips defined by the associated vertex set and the defined line faces.
		 * @see Renderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Renders the line strips defined by the associated vertex set and the defined line faces.
		 * @see Renderable::render().
		 */
		void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) override;

	protected:

		/**
		 * Creates a new GLESceneGraph line strips object.
		 */
		GLESLineStrips();

		/**
		 * Destructs a GLESceneGraph line strips object.
		 */
		~GLESLineStrips() override;

		/**
		 * Releases the internal vertex buffer object containing the line strips indices.
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

		/// The line strips.
		VertexIndexGroups strips_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_LINE_STRIPS_H
