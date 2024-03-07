// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_CONVERTER_YUV_WIN_FRAME_CONVERTER_YUV_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_CONVERTER_YUV_WIN_FRAME_CONVERTER_YUV_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvframeconverteryuvwin
 */
class FrameConverterYUVMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 * @param file Optional media file
		 */
		FrameConverterYUVMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~FrameConverterYUVMainWindow() override;

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

		/// The window showing the Y component.
		Platform::Win::BitmapWindow windowY_;

		/// The window showing the U component.
		Platform::Win::BitmapWindow windowU_;

		/// The window showing the V component.
		Platform::Win::BitmapWindow windowV_;

		/// The frame medium providing the input data.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp recentFrameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// True, to ignore the frame timestamp.
		bool ignoreTimestamp_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_FRAME_CONVERTER_YUV_WIN_FRAME_CONVERTER_YUV_MAINWINDOW_H
