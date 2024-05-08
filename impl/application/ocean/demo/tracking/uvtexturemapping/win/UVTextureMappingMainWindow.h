// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/uvtexturemapping/win/UVTextureMappingMain.h"

#include "application/ocean/demo/tracking/uvtexturemapping/UVTextureMappingWrapper.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackinguvtexturemappingwin
 */
class UVTextureMappingMainWindow :
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
		UVTextureMappingMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs the main window.
		 */
		~UVTextureMappingMainWindow() override;

	protected:

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

	protected:

		// The platform independent wrapper for the UV texture mapping.
		UVTextureMappingWrapper uvTextureMappingWrapper_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_WINDOW_H
