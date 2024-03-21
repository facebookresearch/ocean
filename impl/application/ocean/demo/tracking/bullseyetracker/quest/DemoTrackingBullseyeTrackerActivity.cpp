// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/bullseyetracker/quest/BullseyeTrackerApplication.h"


#include <android_native_app_glue.h>

using namespace Ocean;

/**
 * The application's main entry point of native activity of the demo application.
 *
 * The demo application shows how to use the 3D detector for QR codes from the CV::Detector::QRCodes library.
 *
 * @param androidApp The android app structure
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	BullseyeTrackerApplication application(androidApp);
	application.run();
}
