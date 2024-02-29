// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/meta/quest/openxr/nativeapplication/OpenXRNativeApplication.h"

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
