/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WIN_WINDOW_CONTEXT_H
#define META_OCEAN_PLATFORM_GL_WIN_WINDOW_CONTEXT_H

#include "ocean/platform/gl/win/GLWin.h"

#include "ocean/platform/gl/Context.h"

#include "ocean/platform/win/Window.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

/**
 * This class implements an OpenGL context based on a window.
 * @ingroup platformglwin
 */
class OCEAN_PLATFORM_GL_WIN_EXPORT WindowContext :
	public Platform::Win::Window,
	public Context
{
	public:

		/**
		 * Creates a new main window.
		 * @param applicationInstance Application instance
		 * @param name name of the application window
		 * @param isChild True, if the window is intended to be child window
		 */
		WindowContext(HINSTANCE applicationInstance, const std::wstring& name, const bool isChild);

		/**
		 * Destructs the main window.
		 */
		~WindowContext() override;

		/**
		 * Creates the OpenGL context for this framebuffer.
		 * @see Context::createOpenGLContext().
		 */
		bool createOpenGLContext(const bool initializeOpenGL33, const unsigned int multisamples) override;

		/**
		 * Makes the OpenGL rendering context of this framebuffer the calling thread's current rendering context or makes the calling thread's current rendering context not longer current.
		 * @see Context::makeCurrent().
		 */
		void makeCurrent(const bool state = true) override;

		/**
		 * Swaps the foreground and background buffer if this context has two buffers.
		 * @see Context::swapBuffers().
		 */
		bool swapBuffers() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Allows the modification of the window class before the registration.
		 * @see Window::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASS& windowClass) override;
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WIN_WINDOW_CONTEXT_H
