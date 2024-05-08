/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_FRAMEBUFFER_OBJECT_H
#define META_OCEAN_PLATFORM_GL_FRAMEBUFFER_OBJECT_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/ContextAssociated.h"
#include "ocean/platform/gl/Texture.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a frame buffer objects that may be used for rendering to texture.
 * The current implementation covers 2D textures as render targets only and does not provide a depth buffer.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT FramebufferObject : public ContextAssociated
{
	friend class ScopedRenderToTexture;

	public:

		/**
		 * Scoped object simplifying the rendering to a texture.
		 */
		class OCEAN_PLATFORM_GL_EXPORT ScopedRenderToTexture
		{
			public:

				/**
				 * Creates a new scoped render to texture object.
				 * @param framebufferObject Frame buffer object instance
				 * @param texture The texture into which is rendered
				 */
				inline ScopedRenderToTexture(FramebufferObject& framebufferObject, Texture& texture);

				/**
				 * Destroys the scoped render to texture object.
				 */
				inline ~ScopedRenderToTexture();

				/**
				 * Explicitly releases the scoped object and unbinds the associated render target (already before the scope ends).
				 */
				inline void release();

			protected:

				/**
				 * Disabled copy constructor.
				 * @param object The object that would be copied
				 */
				ScopedRenderToTexture(const ScopedRenderToTexture& object) = delete;

				/**
				 * The disabled assign operator.
				 * @param object The object that would be assigned
				 */
				ScopedRenderToTexture& operator=(const ScopedRenderToTexture& object) = delete;

			protected:

				/// Framebuffer object for rendering to texture
				FramebufferObject* scopedFramebufferObject;
		};

	public:

		/**
		 * Creates a new frame buffer object.
		 */
		FramebufferObject();

		/**
		 * Destroys the frame buffer object
		 */
		virtual ~FramebufferObject();

		/**
		 * Releases the frame buffer object.
		 */
		void release();

	protected:

		/**
		 * Binds the specified texture as render target.
		 * @param texture The texture which will be used as render target
		 */
		void bindRenderTarget(Texture& texture);

		/**
		 * Unbinds the render target from a frame buffer.
		 */
		void unbindRenderTarget();

	protected:

		/// The id of the framebuffer object.
		GLuint framebufferObjectId;

		/// The viewport coordinates set before changing the render target.
		GLint framebufferOldViewportCoordinates[4];

		/// Indicates if the frame buffer object is currently used for rendering into a texture.
		bool framebufferIsBound;
};

inline FramebufferObject::ScopedRenderToTexture::ScopedRenderToTexture(FramebufferObject& framebufferObject, Texture& texture) :
	scopedFramebufferObject(&framebufferObject)
{
	scopedFramebufferObject->bindRenderTarget(texture);
}

inline FramebufferObject::ScopedRenderToTexture::~ScopedRenderToTexture()
{
	release();
}

inline void FramebufferObject::ScopedRenderToTexture::release()
{
	if (scopedFramebufferObject)
	{
		scopedFramebufferObject->unbindRenderTarget();
		scopedFramebufferObject = nullptr;
	}
}

}

}

}

#endif // FACEBOOK_PLATOFRM_GL_FRAMEBUFFER_OBJECT_H
