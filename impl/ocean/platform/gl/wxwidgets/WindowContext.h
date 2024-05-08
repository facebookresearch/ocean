/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WXWIDGETS_WINDOW_CONTEXT_H
#define META_OCEAN_PLATFORM_GL_WXWIDGETS_WINDOW_CONTEXT_H

#include "ocean/platform/gl/wxwidgets/GLWxWidgets.h"

#include "ocean/platform/gl/Context.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace WxWidgets
{

/**
 * This class implements an OpenGL context based on a window.
 * @ingroup platformglwxwidgets
 */
class OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT WindowContext :
	public wxWindow,
	public Context
{
	public:

		/**
		 * Creates a new object without creating the window.
		 */
		WindowContext();

		/**
		 * Creates a new object and directly creates the window.
		 * @param parent The parent window, must be valid
		 * @param id The id of the new window
		 * @param pos The position of the window
		 * @param size The size of the window
		 * @param style The style of the window
		 * @param name The name of the window
		 */
		WindowContext(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxString());

		/**
		 * Destructs the window.
		 */
		virtual ~WindowContext();

		/**
		 * Creates the OpenGL context for this framebuffer.
		 * @see Context::createOpenGLContext().
		 */
		virtual bool createOpenGLContext(const bool initializeOpenGL33, const unsigned int multisamples);

		/**
		 * Makes the OpenGL rendering context of this framebuffer the calling thread's current rendering context or makes the calling thread's current rendering context not longer current.
		 * @see Context::makeCurrent().
		 */
		virtual void makeCurrent(const bool state = true);

		/**
		 * Swaps the foreground and background buffer if this context has two buffers.
		 * @see Context::swapBuffers().
		 */
		virtual bool swapBuffers();

	protected:

		/**
		 * Releases the OpenGL context of this framebuffer.
		 * @return True, if succeeded
		 */
		virtual bool releaseOpenGLContext();

		/**
		 * The window resize event function.
		 * @param event The event object
		 */
		void onResize(wxSizeEvent& event);

	protected:

#ifdef __APPLE__
		NSOpenGLContext* nsContext_ = nullptr;
#endif

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WXWIDGETS_WINDOW_CONTEXT_H
