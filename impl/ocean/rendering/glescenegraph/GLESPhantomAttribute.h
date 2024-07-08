/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PHANTOM_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_PHANTOM_ATTRIBUTE_H

#include "ocean/rendering/DynamicObject.h"
#include "ocean/rendering/PhantomAttribute.h"

#include "ocean/rendering/glescenegraph/GLESAttribute.h"
#include "ocean/rendering/glescenegraph/GLESTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

class GLESPhantomAttribute :
	virtual public GLESAttribute,
	virtual public DynamicObject,
	virtual public PhantomAttribute
{
	friend class GLESFactory;

	public:

		/**
		 * Binds this attribute.
		 * @param framebuffer Framebuffer initiated the rendering process
		 * @param shaderProgram Shader program to be applied
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Unbindes this attribute.
		 */
		void unbindAttribute() override;

		/**
		 * Update function called by the framebuffer.
		 * @param view Associated view
		 * @param timestamp Update timestamp
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Returns the shader type necessary to render an object with this attribute.
		 * @return Shader program type
		 */
		GLESAttribute::ProgramType necessaryShader() const override;

	protected:

		/**
		 * Constructor
		 */
		GLESPhantomAttribute();

		/**
		 * Destructor
		 */
		~GLESPhantomAttribute() override;

		/**
		 * Update matrices and texture related to the background image.
		 * @param view Associated view
		 * @param timestamp Update timestamp
		 * @see onDynamicUpdate()
		 */
		void updateBackgroundInformation(const ViewRef& view, const Timestamp timestamp);

	protected:

		/// Timestamp of the last video camera type change.
		Timestamp phantomVideoTimestamp;

		/// Timestamp of the last color change.
		Timestamp phantomColorTimestamp;

		/// Phantom mode.
		unsigned int phantomMode;

		/// Currently set background color; used for PM_COLOR
		RGBAColor phantomBackgroundColor;

		/// Texture transformation matrix (required for moveable object with fixed texture on them)
		SquareMatrix4 phantomTextureCoordinateTransform;

		/// Normalized camera frustum matrix (required to convert vertex positions into texture coordinates)
		SquareMatrix4 phantomNormalizedCameraFrustumMatrix;

		/// Reference to the (undistorted) background texture.
		Texture2DRef phantomUndistortedBackgroundTexture;

		/// Reference to the (undistorted) background offset texture.
		Texture2DRef phantomUndistortedBackgroundOffsetTexture;
};

} // namespace GLESceneGraph

} // namespace Rendering

} // namespace Ocean

#endif // META_OCEAN_RENDERING_GLES_PHANTOM_ATTRIBUTE_H
