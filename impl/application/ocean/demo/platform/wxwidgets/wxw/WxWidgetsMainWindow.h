/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_MAINWINDOW_H

#include "application/ocean/demo/platform/wxwidgets/wxw/WxWidgets.h"

/**
 * This class implements the main window.
 * @ingroup applicationdemoplatformwxwidgetswxw
 */
class WxWidgetsMainWindow : public wxFrame
{
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
		 * @param size Size of the main window
		 */
		WxWidgetsMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

	private:

		/**
		 * Quit event function.
		 * @param event Event command
		 */
		void onQuit(wxCommandEvent& event);

		/**
		 * About event function.
		 * @param event Event command
		 */
		void onAbout(wxCommandEvent& event);

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_MAINWINDOW_H
