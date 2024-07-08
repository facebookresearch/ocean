/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_APPLICATION_WINDOW_H
#define META_OCEAN_PLATFORM_WIN_APPLICATION_WINDOW_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/Window.h"

#include <string>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements an application window.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT ApplicationWindow : virtual public Window
{
	public:

		/**
		 * Creates a new application window.
		 * @param instance Application instance
		 * @param name The name of the application window
		 */
		ApplicationWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the application window.
		 */
		~ApplicationWindow() override;

		/**
		 * Starts the application.
		 */
		int start();

		/**
		 * Closes the application explicitly.
		 */
		void close();

	protected:

		/**
		 * Function for a destroy event.
		 */
		void onDestroy() override;

	private:

		/// State determining whether the application is about to be closed.
		bool applicationAboutToClosed_ = false;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_APPLICATION_WINDOW_H
