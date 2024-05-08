/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_MAIN_WINDOW_H

#include "application/ocean/demo/media/videopreview/wxw/VideoPreview.h"

#include "ocean/platform/wxwidgets/FrameMediumWindow.h"

/**
 * This class implements the main window.
 * @ingroup applicationdemomediavideopreviewwxwidgets
 */
class MainWindow : public wxFrame
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
		 * @param size The size of the main window
		 */
		MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

	private:

		/**
		 * Quit event function.
		 * @param event The event command
		 */
		void onQuit(wxCommandEvent& event);

		/**
		 * About event function.
		 * @param event The event command
		 */
		void onAbout(wxCommandEvent& event);

		/**
		 * On drag and drop file event function.
		 * @param files Event files
		 */
		bool onFileDragAndDrop(const std::vector<std::string> &files);

	private:

		/// Frame window displaying the preview.
		Platform::WxWidgets::FrameMediumWindow* frameWindow_ = nullptr;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEO_PREVIEW_WXW_VIDEO_PREVIEW_MAIN_WINDOW_H
