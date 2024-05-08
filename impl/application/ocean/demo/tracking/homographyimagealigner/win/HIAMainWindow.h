/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_WIN_HIA_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_WIN_HIA_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/homographyimagealigner/win/HIAMain.h"

#include "application/ocean/demo/tracking/homographyimagealigner/HomographyImageAligner.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackinghomographyimagealignerwin
 */
class HIAMainWindow :
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
		HIAMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		virtual ~HIAMainWindow();

	protected:

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		virtual void onIdle();

	protected:

		/// The platform independent implementation of the homography image aligner.
		HomographyImageAligner applicationHomographyImageAligner;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_WIN_HIA_MAIN_WINDOW_H
