// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/demo/cv/detector/barcodes/detector2d/win/Application.h"

#include "application/ocean/demo/cv/detector/barcodes/detector2d/Wrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorbarcodesbarcodedetector2dwin
 */
class MainWindow :
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
		MainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		~MainWindow() override;

	protected:

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

	protected:

		/// The platform independent wrapper for the barcode detector.
		Wrapper wrapper_;
};
