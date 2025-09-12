/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/nativeapplication/quest/OpenXRNativeApplication.h"

OpenXRNativeApplication::OpenXRNativeApplication(struct android_app* androidApp) :
	NativeApplication(androidApp)
{
	// nothing to do here
}

bool OpenXRNativeApplication::createOpenXRSession(const XrViewConfigurationViews& /*xrViewConfigurationViews*/)
{
	Log::info() << "Now, the OpenXR session needs to be created";

	return true;
}

void OpenXRNativeApplication::releaseOpenXRSession()
{
	Log::info() << "Now, the OpenXR session needs to be released";
}

void OpenXRNativeApplication::onIdle()
{
	Log::info() << "The scene needs to be rendered";
}
