// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/oculus/XRPlaygroundApplication.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include <android_native_app_glue.h>

using namespace Ocean;
using namespace Ocean::XRPlayground::Oculus;

/**
 * The application's main entry point of native activity of the XRPlayground application.
 * @param androidApp The android app structure, must be valid
 */
void android_main(struct android_app* androidApp)
{
	ocean_assert(androidApp != nullptr);
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	RandomI::initialize();

	XRPlaygroundApplication xrplaygroundApplication(androidApp);
	xrplaygroundApplication.run();
}
