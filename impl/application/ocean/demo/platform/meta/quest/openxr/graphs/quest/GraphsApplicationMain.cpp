/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplication.h"

using namespace Ocean;

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows how to create a simple app allowing to visualize 3D graphs.
 * Further, the application is based on Platform::Meta::Quest::OpenXR::VRApplication wrapping most parts parts of the OVR/Android code.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	GraphsApplication graphsApplication(androidApp);
	graphsApplication.run();
}
