// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_RENDERING_GLES_TRIANGLE_FANS_H
#define META_OCEAN_RENDERING_GLES_TRIANGLE_FANS_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESStripPrimitive.h"

#include "ocean/rendering/TriangleFans.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph triangle fans object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTriangleFans :
	virtual public GLESStripPrimitive,
	virtual public TriangleFans
{
	friend class GLESFactory;

	protected:

		/**
		 * Definition of a pair holding a vertex buffer object and the number of indices.
		 */
		typedef std::pair<GLuint, unsigned int> VertexBufferPair;

		/**
		 * Definition of a vector holding vertex buffer objects.
		 */
		typedef std::vector<VertexBufferPair> VertexBufferPairs;

	public:

		/**
		 * Returns the strips of this primitive object.
		 * @see StripPrimitive::strips().
		 */
		VertexIndexGroups strips() const override;

		/**
		 * Returns the number of triangle strips defined in this primitive.
		 * @see StripPrimitive::numberStrips().
		 */
		unsigned int numberStrips() const override;

		/**
		 * Sets the strips of this primitive object.
		 * @see StripPtrimitive::setStrips().
		 */
		void setStrips(const VertexIndexGroups& strips) override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights) override;

		/**
		 * Renders the triangles defined by the associated vertex set and the defined triangle faces.
		 * @see Renderable::render().
		 */
		void render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram) override;

	protected:

		/**
		 * Creates a new GLESceneGraph triangle fans object.
		 */
		GLESTriangleFans();

		/**
		 * Destructs a GLESceneGraph triangle fans object.
		 */
		~GLESTriangleFans() override;

		/**
		 * Releases the internal vertex buffer object containing the triangle strips indices.
		 */
		void release();

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	protected:

		/// Vertex buffer pairs holding the indices of the fans.
		VertexBufferPairs vertexBufferPairs_;

		/// The strips of the fans.
		VertexIndexGroups strips_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TRIANGLE_FANS_H
