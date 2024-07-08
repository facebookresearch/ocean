/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_MAIN_WINDOW_H

#include "application/ocean/demo/cv/advanced/PoissonBlending/wxw/PoissonBlending.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window.
 * @ingroup applicationdemocvPoissonBlendingwxw
 */
class PoissonBlendingMainWindow : public wxFrame
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
		 * @param position The position of the main window
		 * @param size The size of the main window
		 */
		PoissonBlendingMainWindow(const wxString& title, const wxPoint& position, const wxSize& size);

		/**
		 * Destructs the window.
		 */
		~PoissonBlendingMainWindow() override;

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

	private:

		/**
		 * Loads a new frame.
		 * @param filename The filename of the frame to be loaded
		 * @param frame Receives the loaded frame
		 * @return True if succeeded; otherwise, false is returned.
		 */
		bool loadFrame(const wxString& filename, Frame& frame);

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

	private:

		/// The output window showing the blending result.
		Platform::WxWidgets::BitmapWindow* outputWindow_ = nullptr;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_ADVANCED_POISSONBLENDING_WXWIDGETS_POISSONBLENDING_MAIN_WINDOW_H
