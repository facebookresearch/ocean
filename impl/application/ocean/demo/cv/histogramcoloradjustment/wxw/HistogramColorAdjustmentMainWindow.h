/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_COLOR_ADJUSTMENT_WXWIDGETS_HISTOGRAM_COLOR_ADJUSTMENT_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_COLOR_ADJUSTMENT_WXWIDGETS_HISTOGRAM_COLOR_ADJUSTMENT_MAIN_WINDOW_H

#include "application/ocean/demo/cv/histogramcoloradjustment/wxw/HistogramColorAdjustment.h"

#include "ocean/cv/Histogram.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window.
 * @ingroup applicationdemocvhistogramcoloradjustmentwxw
 */
class HistogramColorAdjustmentMainWindow : public wxFrame
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
		 * @param size The size of the main window
		 */
		HistogramColorAdjustmentMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

		/**
		 * Destructs the window.
		 */
		virtual ~HistogramColorAdjustmentMainWindow();

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
		 * Loads a new reference frame.
		 * @param filename The filename of the reference frame to be loaded
		 */
		bool loadReference(const std::string& filename);

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
		 * Draws a histogram into a bitmap.
		 * @param histogram The histogram that will be drawn
		 * @param width The width of the resulting histogram bitmap in pixel
		 * @param height The height of the resulting histogram bitmap in pixel
		 * @param bitmap Resulting bitmap
		 * @return True, if succeeded
		 */
		static bool drawHistogram3Channels(const CV::Histogram::HistogramBase8BitPerChannel<3u>& histogram, const unsigned int width, const unsigned int height, wxBitmap& bitmap);

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
		bool onFileDragAndDropFrame(const std::vector<std::string> &files);

		/**
		 * File drag and drop event function.
		 * @param files Names of the files
		 * @return True, if the files are accepted
		 */
		bool onFileDragAndDropReference(const std::vector<std::string> &files);

	private:

		/// Bitmap preview window.
		Platform::WxWidgets::BitmapWindow* bitmapWindow_ = nullptr;

		/// Reference bitmap window.
		Platform::WxWidgets::BitmapWindow* referenceWindow_ = nullptr;

		/// Result bitmap window.
		Platform::WxWidgets::BitmapWindow* resultWindow_ = nullptr;

		/// Source frame to be adjusted.
		Frame sourceFrame_;

		/// Reference frame providing the color reference.
		Frame referenceFrame_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_HISTOGRAM_COLOR_ADJUSTMENT_WXWIDGETS_HISTOGRAM_COLOR_ADJUSTMENT_MAIN_WINDOW_H
