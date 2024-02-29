// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/meta/quest/openxr/nativeapplication/OpenXRNativeApplication.h"

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows how to create a native application using Ocean's OpenXR code base.
 * Further, the application is based on Platform::Meta::Quest::OpenXR::NativeApplication.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	OpenXRNativeApplication nativeApplication(androidApp);
	nativeApplication.run();
}
