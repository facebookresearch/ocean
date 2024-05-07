// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

// clang-format off

#include "application/ocean/demo/tracking/pointtracker/meta/quest/openxr/PointTrackerApplication.h"

#include <android_native_app_glue.h>

using namespace Ocean;

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows how track points in the Quest's camera.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	PointTrackerApplication pointTrackerApplication(androidApp);
	pointTrackerApplication.run();
}
