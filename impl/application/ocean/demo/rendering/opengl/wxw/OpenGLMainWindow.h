/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_MAIN_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_MAIN_APPLICATION_H

#include "application/ocean/demo/rendering/opengl/wxw/OpenGL.h"

#include "ocean/base/Thread.h"

#include "ocean/platform/gl/MediumTexture.h"
#include "ocean/platform/gl/WindowFramebuffer.h"

#include "ocean/platform/gl/wxwidgets/WindowContext.h"

/**
 * This class implements the main window.
 * @ingroup applicationdemorenderingopenglwxw
 */
class OpenGLMainWindow :
#ifdef __APPLE__
	protected Ocean::Thread,
#endif
	public wxFrame
{
	friend class OpenGLApplication;

	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Quit event id.
			ID_Quit = 1,
			/// About event id.
			ID_About,
		};

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the main window to be created
		 * @param pos Position of the main window
		 * @param size The size of the main window
		 */
		OpenGLMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

	private:

		/**
		 * Initializes this window.
		 */
		void initalize();

		/**
		 * Renders the frame using OpenGL.
		 */
		void render();

#ifdef __APPLE__

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		virtual void threadRun();

#endif

		/**
		 * Quit event function.
		 * @param event The event object
		 */
		void onQuit(wxCommandEvent& event);

		/**
		 * The window resize event function.
		 * @param event The event object
		 */
		void onResize(wxSizeEvent& event);

		/**
		 * The point event function.
		 * @param event The event object
		 */
		void onPaint(wxPaintEvent& event);

		/**
		 * The erase background event function.
		 * @param event The event object
		 */
		void onEraseBackground(wxEraseEvent& event);

		/**
		 * The idle event function.
		 * @param event The event object
		 */
		void onIdle(wxIdleEvent& event);

		/**
		 * About event function.
		 * @param event The event object
		 */
		void onAbout(wxCommandEvent& event);

	private:

		/// The OpenGL child window.
		Platform::GL::WxWidgets::WindowContext* windowContext_ = nullptr;

		/// The OpenGL framebuffer.
		Platform::GL::WindowFramebuffer windowFramebuffer_;

		/// The OpenGL texture object.
		Platform::GL::MediumTexture windowTexture_;

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_MAIN_APPLICATION_H
