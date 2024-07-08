/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURE_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_TEXTURE_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture.h"

#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/TextureFramebuffer.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	#include <CoreVideo/CoreVideo.h>
#endif

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph texture framebuffer objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTextureFramebuffer :
	virtual public GLESTexture,
	virtual public TextureFramebuffer
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the name of the texture e.g., in a shader.
		 * @see Texture::textureName().
		 */
		std::string textureName() const override;

		/**
		 * Sets the name of the texture e.g., in a shader.
		 * @see Texture::setTextureName().
		 */
		bool setTextureName(const std::string& name) override;

		/**
		 * Resizes the texture framebuffer.
		 * @see TextureFramebuffer::resize().
		 */
		bool resize(const unsigned int width, const unsigned int height) override;

		/**
		 * Returns the texture framebuffer's internal pixel format.
		 * @see TextureFramebuffer::pixelFormat().
		 */
		FrameType::PixelFormat pixelFormat() const override;

		/**
		 * Returns the width of this framebuffer.
		 * @return The framebuffer's width, in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this framebuffer.
		 * @return The framebuffer's height, in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the texture wrap type in s direction.
		 * @see Texture2D::wrapTypeS().
		 */
		WrapType wrapTypeS() const override;

		/**
		 * Returns the texture wrap type in t direction.
		 * @see Texture2D::wrapTypeT().
		 */
		WrapType wrapTypeT() const override;

		/**
		 * Sets the texture framebuffer's internal pixel format.
		 * @see TextureFramebuffer::setPixelFormat().
		 */
		bool setPixelFormat(const FrameType::PixelFormat pixelFormat) override;

		/**
		 * Sets the texture wrap type in s direction.
		 * @see Texture2D::setWrapTypeS().
		 */
		bool setWrapTypeS(const WrapType type) override;

		/**
		 * Sets the texture wrap type in t direction.
		 * @see Texture2D::setWrapTypeT().
		 */
		bool setWrapTypeT(const WrapType type) override;

		/**
		 * Sets the number of multi-samples.
		 * @param multisamples The number of multi-samples the texture will apply, with range [1, infinity)
		 */
		bool setMultisamples(const unsigned int multisamples);

		/**
		 * Binds this texture.
		 * @see GLESTexture::bindTexture().
		 */
		unsigned int bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id) override;

		/**
		 * Binds the framebuffer.
		 * @return True, if succeeded
		 */
		bool bindFramebuffer();

		/**
		 * Unbinds the framebuffer.
		 */
		void unbindFramebuffer();

		/**
		 * Copies the image content of the color texture to a given frame.
		 * @param frame The frame to which the color texture will be copied, the frame will be adjusted if the pixel format does match
		 * @param subRegion Optional sub-region within the framebuffer to copy; an invalid bounding box to copy the entire framebuffer
		 * @return True, if succeeded
		 */
		bool copyColorTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion = CV::PixelBoundingBox()) override;

		/**
		 * Copies the image content of the depth texture to a given frame.
		 * @param frame The frame to which the depth texture will be copied, the frame will be adjusted if the pixel format does match
		 * @param subRegion Optional sub-region within the framebuffer to copy; an invalid bounding box to copy the entire framebuffer
		 * @return True, if succeeded
		 */
		bool copyDepthTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion = CV::PixelBoundingBox()) override;

		/**
		 * Returns the id of the color texture.
		 * @return The OpenGL id of the color texture of this framebuffer.
		 */
		inline GLuint colorTextureId() const;

		/**
		 * Returns the id of the depth texture.
		 * @return The OpenGL id of the depth texture of this framebuffer.
		 */
		inline GLuint depthTextureId() const;

		/**
		 * Returns whether the texture framebuffer is ready for use.
		 * @return True, if so
		 */
		bool isValid() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph texture object.
		 */
		GLESTextureFramebuffer();

		/**
		 * Destructs a GLESceneGraph texture object.
		 */
		~GLESTextureFramebuffer() override;

		/**
		 * Releases this texture framebuffer.
		 */
		void release();

		/**
		 * Updates the mipmap for this texture.
		 * @see GLESTexture::createMipmap().
		 */
		void createMipmap() override;

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Creates a texture for the framebuffer on iOS platforms.
		 * @param width The width of the texture, in pixel
		 * @param height The height of the texture, in pixel
		 * @param textureInternalFormat The internal texture format
		 * @param textureFormat The texture format
		 * @param textureType The texture type
		 * @return True, if succeeded
		 */
		bool createTextureForFramebufferAppleIOS(const unsigned int width, const unsigned int height, const GLint textureInternalFormat, const GLenum textureFormat, const GLenum textureType);

#endif // OCEAN_PLATFORM_BUILD_APPLE

	protected:

		/// The id of the framebuffer object.
		GLuint framebufferObjectId_ = 0u;

		/// The id of the color texture.
		GLuint colorTextureId_ = 0u;

		/// The id of the depth texture.
		GLuint depthTextureId_ = 0u;

		/// The width of the texture framebuffer in pixel, with range [0, infinity).
		unsigned int width_ = 0u;

		/// The height of the texture framebuffer in pixel, with range [0, infinity).
		unsigned int height_ = 0u;

		/// The number of multi samples the framebuffer will apply, with range [1, infinity).
		unsigned int framebufferMultisamples_ = 1u;

		/// The texture's pixel format.
		FrameType::PixelFormat pixelFormat_ = FrameType::FORMAT_RGBA32;

		/// The texture wrap s type.
		WrapType wrapTypeS_ = WRAP_CLAMP;

		/// The texture wrap t type.
		WrapType wrapTypeT_ = WRAP_CLAMP;

		/// The optional texture with one samples, needed when copying the content of the multi sample framebuffer (or depth framebuffer with OpenGLES) to memory.
		TextureFramebufferRef textureFramebufferOneSample_;

		/// The optional shader program object, needed when copying the content of the multi sample framebuffer (or depth framebuffer with OpenGLES) to memory.
		ShaderProgramRef shaderProgramForOneSample_;

		/// The optional vertex set object, needed when copying the content of the multi sample framebuffer (or depth framebuffer with OpenGLES) to memory.
		VertexSetRef vertexSetForOneSample_;

		/// The optional triangles object, needed when copying the content of the multi sample framebuffer (or depth framebuffer with OpenGLES) to memory.
		TrianglesRef trianglesForOneSample_;

		/// The name of the texture in the shader.
		std::string textureName_ = std::string("primaryTexture");

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/// The texture cache on iOS platforms, if supported.
		CVOpenGLESTextureCacheRef iosTextureCache_ = nullptr;

		/// The pixel buffer on iOS platforms, if supported.
		CVPixelBufferRef iosPixelBuffer_ = nullptr;

		/// The texture on iOS platforms, if supported.
		CVOpenGLESTextureRef iosTexture_ = nullptr;

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

		/// The function pointer to glBlitFramebuffer.
		PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer_ = nullptr;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS
};

inline unsigned int GLESTextureFramebuffer::width() const
{
	return width_;
}

inline unsigned int GLESTextureFramebuffer::height() const
{
	return height_;
}

inline GLuint GLESTextureFramebuffer::colorTextureId() const
{
	return colorTextureId_;
}

inline GLuint GLESTextureFramebuffer::depthTextureId() const
{
	return depthTextureId_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_FRAMEBUFFER_H
