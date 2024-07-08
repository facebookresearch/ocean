/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_MAINWINDOW_H

#include "application/ocean/demo/rendering/renderer/wxw/RendererWxWidgets.h"

#include "ocean/rendering/Engine.h"

/**
 * This class implements the main window.
 * @ingroup applicationdemorenderingrendererwxw
 */
class RendererWxWidgetsMainWindow : public wxFrame
{
	friend class RendererWxWidgetsApplication;

	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Quit event id.
			ID_Quit = 1
		};

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the main window to be created
		 * @param pos Position of the main window
		 * @param size The size of the main window
		 */
		RendererWxWidgetsMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

	protected:

		/**
		 * Initializes this window.
		 */
		void initalize();

		/**
		 * Renders the scene.
		 */
		void render();

		/**
		 * The resize event function.
		 * @param event The event object
		 */
		void onResize(wxSizeEvent& event);

		/**
		 * The point event function.
		 * @param event The event object
		 */
		void onPaint(wxPaintEvent& event);

		/**
		 * The event function for mouse button down events.
		 * @param event The event object
		 */
		void onMouseDown(wxMouseEvent& event);

		/**
		 * The event function for mouse move events.
		 * @param event The event object
		 */
		void onMouseMove(wxMouseEvent& event);

		/**
		 * The event function for mouse button up events.
		 * @param event The event object
		 */
		void onMouseUp(wxMouseEvent& event);

		/**
		 * The event function for mouse wheel events.
		 * @param event The event object
		 */
		void onMouseWheel(wxMouseEvent& event);

		/**
		 * Quit event function.
		 * @param event The event object
		 */
		void onQuit(wxCommandEvent& event);

	protected:

		/// Rendering engine.
		Rendering::EngineRef engine_;

		/// Rendering framebuffer.
		Rendering::WindowFramebufferRef framebuffer_;

		/// Old horizontal mouse cursor position.
		int oldCursorX_ = NumericT<int>::minValue();

		/// Old vertical mouse cursor position.
		int oldCursorY_ = NumericT<int>::minValue();

		/// The window's lock.
		Lock lock_;

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_MAINWINDOW_H
