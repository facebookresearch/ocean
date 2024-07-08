/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_MAIN_WINDOW_H

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreview.h"
#include "application/ocean/demo/media/moviepreview/wxw/MoviePreviewBitmapWindow.h"

#include "ocean/media/MovieFrameProvider.h"

#include "ocean/platform/wxwidgets/FramePreviewWindow.h"

using namespace Ocean;

/**
 * This class implements the main window.
 * @ingroup applicationdemomediamoviepreviewwxw
 */
class MoviePreviewMainWindow : public wxFrame
{
	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Quit event id.
			ID_Quit = 1,
			/// Open media even id.
			ID_Open_Media,
			/// Save image event id.
			ID_Save_Image,
			/// Save images event id.
			ID_Save_Images,
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
		MoviePreviewMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

		/**
		 * Destructs the window.
		 */
		~MoviePreviewMainWindow() override;

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

	private:

		/**
		 * Open media event function.
		 * @param event The event command
		 */
		void onOpenMedia(wxCommandEvent& event);

		/**
		 * Save image event function.
		 * @param event The event command
		 */
		void onSaveImage(wxCommandEvent& event);

		/**
		 * Save images event function.
		 * @param event The event command
		 */
		void onSaveImages(wxCommandEvent& event);

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
		 * Idle event function.
		 * @param event The event command
		 */
		void onIdle(wxIdleEvent& event);

		/**
		 * Frame selection event.
		 * @param index Selection index
		 */
		void onSelection(const unsigned int index);

		/**
		 * File drag and drop event function.
		 * @param files Names of the files
		 * @return True, if the files are accepted
		 */
		bool onFileDragAndDrop(const std::vector<std::string> &files);

	private:

		/// Bitmap window.
		MoviePreviewBitmapWindow* bitmapWindow_ = nullptr;

		/// Frame preview window.
		Platform::WxWidgets::FramePreviewWindow* previewWindow_ = nullptr;

		/// Frame provider object.
		Media::MovieFrameProviderRef frameProvider_;

		/// Recent progress value, with range [0, 100]
		unsigned int recentProgress_ = 0u;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_MAIN_WINDOW_H
