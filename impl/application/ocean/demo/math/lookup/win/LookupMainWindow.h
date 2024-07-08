/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_WIN_LOOKUP_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_WIN_LOOKUP_MAIN_WINDOW_H

#include "ocean/base/Frame.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/Vector3.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the demo application.
 * @ingroup applicationdemomathlookupwin
 */
class LookupMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		LookupMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~LookupMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Toggles the view.
		 */
		void toggleView();

	protected:

		/// Center bin position lookup object.
		LookupCenter2<Vector3> windowLookupCenter_;

		/// Corner bin position lookup object.
		LookupCorner2<Vector3> windowLookupCorner_;

		/// The id of the view.
		unsigned int windowViewId_ = 0u;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_WIN_LOOKUP_MAIN_WINDOW_H
