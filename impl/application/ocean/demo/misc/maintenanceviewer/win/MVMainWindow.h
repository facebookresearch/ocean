/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAIN_WINDOW_H

#include "application/ocean/demo/misc/maintenanceviewer/win/MVApplication.h"
#include "application/ocean/demo/misc/maintenanceviewer/win/MVRenderWindow.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"
#include "ocean/platform/win/TextWindow.h"

/**
 * This class implements the main window of the application.
 * @ingroup applicationdemomiscmaintenanceviewerwin
 */
class MVMainWindow : virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Definition of a map mapping application ids to readable ids.
		 */
		typedef std::map<unsigned long long, unsigned int> ApplicationIdMap;

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 */
		MVMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		virtual ~MVMainWindow();

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		virtual void onInitialized();

		/**
		 * Function for window resize event.
		 * @see Window::onResize().
		 */
		virtual void onResize(const unsigned int clientWidth, const unsigned int clientHeight);

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		virtual void onIdle();

	protected:

		/// The render window.
		MVRenderWindow renderWindow_;

		/// The text window.
		Platform::Win::TextWindow textWindow_;

		/// The frame window.
		Platform::Win::BitmapWindow bitmapWindow_;

		/// Map mapping application ids to readable ids.
		ApplicationIdMap applicationIdMap_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAIN_WINDOW_H
