/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_WINDOWS_GLES_BITMAP_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_WINDOWS_GLES_BITMAP_FRAMEBUFFER_H

#ifndef _WINDOWS
	#error This file is available on Windows platforms only!
#endif

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/TextureFramebuffer.h"

#include <gl/wglext.h>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

/**
 * This class is the base class for all GLESceneGraph window framebuffers for Windows platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESBitmapFramebuffer :
	virtual public GLESFramebuffer,
	virtual public BitmapFramebuffer
{
	friend class GLESEngineWindows;

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

		/**
		 * Returns the texture framebuffer object of this framebuffer.
		 * @return The framebuffer's texture framebuffer object
		 */
		inline TextureFramebufferRef textureFramebuffer() const;

	private:

		/**
		 * Creates a new GLESceneGraph bitmap framebuffer.
		 */
		GLESBitmapFramebuffer();

		/**
		 * Destructs a GLESceneGraph bitmap framebuffer.
		 */
		~GLESBitmapFramebuffer() override;

		/**
		 * Initializes the framebuffer.
		 * @see Framebuffer::initialize().
		 */
		bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Initialize the OpenGL ES display and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

		/**
		 * Releases the framebuffer.
		 * @see Framebuffer::release().
		 */
		void release() override;

		/**
		 * Acquires all OpenGL function pointers.
		 * @return True, if succeeded
		 */
		bool acquireGLFunctions();

	protected:

		/// The OpenGL context handle.
		HGLRC contextHandle_ = nullptr;

		/// The function pointer to wglCreateContextAttribsARB.
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_ = nullptr;

		/// The function pointer to wglChoosePixelFormatARB.
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB_ = nullptr;

		/// The bitmap of the framebuffer for which the OpenGL context will be created.
		Platform::Win::BitmapWindow contextBitmapWindow_;

		/// The texture framebuffer in which this framebuffer will be rendered.
		TextureFramebufferRef textureFrambuffer_;
};

inline TextureFramebufferRef GLESBitmapFramebuffer::textureFramebuffer() const
{
	return textureFrambuffer_;
}

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_WINDOWS_GLES_BITMAP_FRAMEBUFFER_H
