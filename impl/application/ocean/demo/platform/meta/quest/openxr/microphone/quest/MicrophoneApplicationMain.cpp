/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.h"

using namespace Ocean;

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows how to create a native activity using Ocean's rendering engine to render virtual 3D content (a coordinate system in front of the user).
 * Further, the application is based on Platform::Meta::Quest::OpenXR::VRApplication wrapping most parts parts of the OVR/Android code.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	MicrophoneApplication microphoneApplication(androidApp);
	microphoneApplication.run();
}
