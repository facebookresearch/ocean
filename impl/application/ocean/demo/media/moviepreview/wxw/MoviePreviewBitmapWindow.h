/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_BITMAP_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_BITMAP_WINDOW_H

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreview.h"

#include "ocean/base/Scheduler.h"

#include "ocean/media/MovieFrameProvider.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the extended bitmap window.
 * @ingroup applicationdemomediamoviepreviewwxw
 */
class MoviePreviewBitmapWindow : public Platform::WxWidgets::BitmapWindow
{
	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the main window to be created
		 * @param parent Parent window
		 * @param pos Position of the main window
		 * @param size Size of the main window
		 */
		MoviePreviewBitmapWindow(const wxString& title, wxWindow* parent = nullptr, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(-1, -1));

		/**
		 * Destructs the window.
		 */
		~MoviePreviewBitmapWindow() override;

		/**
		 * Sets or changes the frame provider.
		 * @param frameProvider Frame provider to be set
		 */
		void setMovieFrameProvider(const Media::MovieFrameProviderRef& frameProvider);

		/**
		 * Sets the index of the next frame.
		 * @param index Frame of the index to be displayed
		 */
		void setSelectionIndex(const unsigned int index);

		/**
		 * Returns the current frame.
		 * @return Current frame
		 */
		inline const Frame& frame() const;

	private:

		/**
		 * Frame event function.
		 * @param index Frame index
		 * @param isRequested True, if the frame has been requested
		 */
		virtual void onFrame(const unsigned int index, const bool isRequested);

		/**
		 * Scheduler event function.
		 */
		void onScheduler();

	private:

		/// Frame provider object.
		Media::MovieFrameProviderRef frameProvider_;

		/// The current frame.
		Frame frame_;

		/// Index of the frame to be displayed.
		unsigned int selectionIndex_ = (unsigned int)(-1);

		/// Index of the next frame.
		unsigned int newFrameIndex_ = (unsigned int)(-1);

		/// Index of the current frame.
		unsigned int frameIndex_ = (unsigned int)(-1);
};

inline const Frame& MoviePreviewBitmapWindow::frame() const
{
	return frame_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_BITMAP_WINDOW_H
