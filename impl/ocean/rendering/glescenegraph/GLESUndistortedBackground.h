/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_UNDISTORTED_BACKGROUND_H
#define META_OCEAN_RENDERING_GLES_UNDISTORTED_BACKGROUND_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESBackground.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Texture2D.h"
#include "ocean/rendering/TriangleStrips.h"
#include "ocean/rendering/UndistortedBackground.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements an undistorted background for the GLESceneGraph render engine.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESUndistortedBackground :
	virtual public GLESBackground,
	virtual public UndistortedBackground
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the timestamp of the most recent camera change.
		 * @return The camera type timestamp
		 */
		const Timestamp& cameraTimestamp() const;

		/**
		 * Returns the normalized camera frustum matrix of the background.
		 * @return The camera frustum matrix
		 */
		const SquareMatrix4& normalizedCameraFrustumMatrix() const;

		/**
		 * Returns the background texture.
		 * @return Background texture
		 */
		Texture2DRef texture() const;

		/**
		 * Returns the offset texture.
		 * @return Offset texture
		 */
		Texture2DRef offsetTexture() const;

		/**
		 * Returns the number of horizontal elements.
		 * The default value is 20.
		 * @return Horizontal elements
		 */
		virtual unsigned int horizontalElements() const;

		/**
		 * Returns the number of vertical elements.
		 * The default value is 20.
		 * @return Vertical elements
		 */
		virtual unsigned int verticalElements() const;

		/**
		 * Sets the number of horizontal elements.
		 * The default value is 20.
		 * @param elements Number of horizontal elements to set
		 */
		virtual void setHorizontalElements(const unsigned int elements);

		/**
		 * Sets the number of vertical elements.
		 * The default value is 20.
		 * @param elements Number of vertical elements to set
		 */
		virtual void setVerticalElements(const unsigned int elements);

		/**
		 * Sets the display type of this background object.
		 * @see UndistortedBackground::setDisplayType().
		 */
		bool setDisplayType(const DisplayType type) override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph undistorted background object.
		 */
		GLESUndistortedBackground();

		/**
		 * Destructs a GLESceneGraph undistorted background object.
		 */
		~GLESUndistortedBackground() override;

		/**
		 * Event function if the camera of the used medium has changed.
		 * @see UndistortedBackground::onMediumCameraChanged().
		 */
		void onMediumCameraChanged(const Timestamp timestamp) override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		virtual void rebuildPrimitive();

	protected:

		/// Triangle strips for the background geometry.
		TriangleStripsRef backgroundTriangleStrips;

		/// Vertexset for the background geometry.
		VertexSetRef backgroundVertexSet;

		/// Textures object holding the 2D texture.
		TexturesRef backgroundTextures;

		/// Attribute set holding rendering attributes of the background object.
		AttributeSetRef backgroundAttributeSet;

		/// Number of horizontal elements.
		unsigned int backgroundHorizontalElements;

		/// Number of vertical elements.
		unsigned int backgroundVerticalElements;

		/// Timestamp of the camera frame type.
		Timestamp backgroundCameraTimestamp;

		SquareMatrix4 backgroundNormalizedCameraFrustumMatrix;
		Texture2DRef backgroundOffsetTexture;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_UNDISTORTED_BACKGROUND_H
