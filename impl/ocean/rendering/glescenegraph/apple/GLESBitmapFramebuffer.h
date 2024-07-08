/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_APPLE_GLES_BITMAP_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_APPLE_GLES_BITMAP_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/TextureFramebuffer.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

/**
 * This class is the base class for all GLESceneGraph bitmap framebuffers for Apple platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESBitmapFramebuffer :
	virtual public GLESFramebuffer,
	virtual public BitmapFramebuffer
{
	friend class GLESEngineApple;

	public:

		/**
		 * Sets the viewport of this framebuffer.
		 * @see Framebuffer::setViewport().
		 */
		void setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Renders the next frame and copies the image content into to a given frame.
		 * @see Framebuffer::render().
		 */
		bool render(Frame& frame, Frame* depthFrame = nullptr) override;

		/**
		 * Makes this framebuffer to the current one.
		 * @see Framebuffer::makeCurrent().
		 */
		void makeCurrent() override;

		/**
		 * Makes this framebuffer to non current.
		 * @see Framebuffer::makeNoncurrent().
		 */
		void makeNoncurrent() override;

	private:

		/**
		 * Creates a new window framebuffer.
		 */
		GLESBitmapFramebuffer();

		/**
		 * Destructs a window framebuffer.
		 */
		~GLESBitmapFramebuffer() override;

		/**
		 * Initializes the framebuffer.
		 * @see Framebuffer::initialize().
		 */
		bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Initialize the OpenGL ES dispaly and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

		/**
		 * Releases the framebuffer.
		 * @see Framebuffer::release().
		 */
		void release() override;

	protected:

		/// The CGL context object.
		CGLContextObj contextObject_ = nullptr;

		/// The texture framebuffer in which this framebuffer will be rendered.
		TextureFramebufferRef textureFrambuffer_;
};

}

}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS

#endif // META_OCEAN_RENDERING_GLES_APPLE_GLES_BITMAP_FRAMEBUFFER_H
