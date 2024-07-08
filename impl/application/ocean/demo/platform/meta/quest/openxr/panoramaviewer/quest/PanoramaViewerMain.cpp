/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest/PanoramaViewer.h"

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows a simple viewer for panorama images.
 * Further, the application is based on Platform::Meta::Quest::OpenXR::PanoramaViewer.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	PanoramaViewer nativeApplication(androidApp);
	nativeApplication.run();
}
