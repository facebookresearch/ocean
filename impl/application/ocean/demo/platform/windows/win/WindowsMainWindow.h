/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WINDOWS_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WINDOWS_WIN_MAINWINDOW_H

#include "application/ocean/demo/platform/windows/win/Windows.h"

#include "ocean/platform/win/ApplicationWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemoplatformwindowswin
 */
class WindowsMainWindow : public Ocean::Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 */
		WindowsMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		virtual ~WindowsMainWindow();

		/**
		 * Repaints the window.
		 * @see ApplicationWindow::onPaint().
		 */
		virtual void onPaint();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WINDOWS_WIN_MAINWINDOW_H
