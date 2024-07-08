/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H

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
		 * @param name The name of the main window
		 * @param commandArguments The command arguments used to configure the tracker
		 */
		LineDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		~LineDetectorMainWindow() override;

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

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_MAINWINDOW_H
