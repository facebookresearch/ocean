// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H

#include "application/ocean/demo/cv/detector/linedetector/win/LineDetector.h"

#include "application/ocean/demo/cv/detector/linedetector/LineDetectorWrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorlinedetectorwin
 */
class LineDetectorMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 * @param commandArguments The command arguments used to configure the tracker
		 */
		LineDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		virtual ~LineDetectorMainWindow();

	protected:

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

	protected:

		/// The platform independent wrapper for the line detector.
		LineDetectorWrapper lineDetectorWrapper_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H
