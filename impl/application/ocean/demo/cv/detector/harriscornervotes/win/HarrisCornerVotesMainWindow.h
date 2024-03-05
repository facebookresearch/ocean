// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_VOTES_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_VOTES_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorharriscornervoteswin
 */
class HarrisCornerVotesMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 */
		HarrisCornerVotesMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~HarrisCornerVotesMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDonw().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

	protected:

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// The index of the color map to be used.
		unsigned int colorMapIndex_ = 0u;

		/// True, to skip negative responses; False, to show negative responses as well.
		bool skipNegativeResponses_ = false;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_HARRIS_CORNER_WIN_HARRIS_CORNER_VOTES_MAINWINDOW_H
