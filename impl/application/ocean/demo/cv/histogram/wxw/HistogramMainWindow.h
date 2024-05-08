/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_MAIN_WINDOW_H

#include "application/ocean/demo/cv/histogram/wxw/Histogram.h"

#include "ocean/cv/Histogram.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window.
 * @ingroup applicationdemocvhistogramwxw
 */
class HistogramMainWindow : public wxFrame
{
	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Quit event id.
			ID_Quit = 1,
			/// Open even id.
			ID_Open,
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
		HistogramMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

		/**
		 * Destructs the window.
		 */
		virtual ~HistogramMainWindow();

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

	private:

		/**
		 * Loads a new frame.
		 * @param filename The filename of the frame to be loaded
		 */
		bool loadFrame(const std::string& filename);

		/**
		 * Visualizes the histogram for a frame with three channels.
		 * @param frame The frame for that a histogram has to be visualized
		 * @param width The width of the resulting histogram bitmap in pixel
		 * @param height The height of the resulting histogram bitmap in pixel
		 * @param bitmapHistogram Resulting histogram bitmap
		 * @param bitmapIntegral Resulting integral histogram bitmap
		 */
		bool visualizeHistogram3Channels(const Frame& frame, const unsigned int width, const unsigned int height, wxBitmap& bitmapHistogram, wxBitmap& bitmapIntegral);

		/**
		 * Open event function.
		 * @param event The event command
		 */
		void onOpen(wxCommandEvent& event);

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
		 * File drag and drop event function.
		 * @param files Names of the files
		 * @return True, if the files are accepted
		 */
		bool onFileDragAndDrop(const std::vector<std::string> &files);

		/**
		 * Draws a histogram into a bitmap.
		 * @param histogram The histogram that will be drawn
		 * @param width The width of the resulting histogram bitmap in pixel
		 * @param height The height of the resulting histogram bitmap in pixel
		 * @param bitmap Resulting bitmap
		 * @return True, if succeeded
		 */
		static bool drawHistogram3Channels(const CV::Histogram::HistogramBase8BitPerChannel<3u>& histogram, const unsigned int width, const unsigned int height, wxBitmap& bitmap);

	private:

		/// Bitmap preview window.
		Platform::WxWidgets::BitmapWindow* bitmapWindow_ = nullptr;

		/// Histogram bitmap window.
		Platform::WxWidgets::BitmapWindow* histogramWindow_ = nullptr;

		/// Integral bitmap window.
		Platform::WxWidgets::BitmapWindow* integralWindow_ = nullptr;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_WXWIDGETS_HISTOGRAM_MAIN_WINDOW_H
