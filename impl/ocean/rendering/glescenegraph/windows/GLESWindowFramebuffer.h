/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_WINDOWS_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_WINDOWS_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"

#include "ocean/rendering/WindowFramebuffer.h"

#ifndef _WINDOWS
	#error This file is available on Windows platforms only!
#endif

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
class OCEAN_RENDERING_GLES_EXPORT GLESWindowFramebuffer :
	virtual public GLESFramebuffer,
	virtual public WindowFramebuffer
{
	friend class GLESEngineWindows;

	public:

		/**
		 * Sets the platform dependent framebuffer base id or handle and initializes the framebuffer.
		 * @see WindowFramebuffer::initializeById().
		 */
		bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Returns whether the framebuffer supports hardware anti-aliasing.
		 * @see Framebuffer::isAntialiasingSupported().
		 */
		bool isAntialiasingSupported(const unsigned int buffers) const override;

		/**
		 * Returns whether the framebuffer uses hardware anti-aliasing.
		 * @see Framebuffer::isAntialiasing().
		 */
		bool isAntialiasing() const override;

		/**
		 * Sets the preferred pixel format of this framebuffer.
		 * @see Framebuffer::setPreferredPixelFormat().
		 */
		void setPreferredPixelFormat(const FrameType::PixelFormat pixelFormat) override;

		/**
		 * Sets whether this framebuffer will support hardware anti-aliasing.
		 * @see setSupportAntialiasing().
		 */
		bool setSupportAntialiasing(const unsigned int buffers) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Makes this framebuffer to the current one.
		 * @see Framebuffer::makeCurrent().
		 */
		void makeCurrent() override;

	private:

		/**
		 * Creates a new GLESceneGraph window framebuffer.
		 */
		GLESWindowFramebuffer();

		/**
		 * Destructs a GLESceneGraph window framebuffer.
		 */
		~GLESWindowFramebuffer() override;

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

	private:

		/// The OpenGL context handle.
		HGLRC contextHandle_ = nullptr;

		/// The window handle.
		HWND windowHandle_ = nullptr;

		/// The device context.
		HDC windowDC_ = nullptr;

		/// The function pointer to wglCreateContextAttribsARB.
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_ = nullptr;

		/// The function pointer to wglChoosePixelFormatARB.
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB_ = nullptr;

		/// True to enable vertical synchronization; False, to disable vertical synchronization
		bool framebufferVerticalSynchronization = true;

		/// Preferred pixel format which should be used during context initialization.
		FrameType::PixelFormat framebufferPreferredPixelFormat = FrameType::FORMAT_RGBA32;

		/// The Number of preferred multi-sampling anti-aliasing buffers.
		unsigned int antialiasingBuffers_ = 4u;
};

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_WINDOWS_GLES_WINDOW_FRAMEBUFFER_H
