// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNOTATOR_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNOTATOR_H

#include "application/ocean/demo/misc/ApplicationDemoMisc.h"

#include "ocean/platform/wxwidgets/WxWidgets.h"

/**
 * @ingroup applicationdemomisc
 * @defgroup applicationdemomiscimageannoatoarwxw ImageAnnotator (WxWidgets)
 * @{
 * The demo application demonstrates the implementation of an image annotator application.<br>
 * This application is platform independent (as long as WxWidgets supports the platform).<br>
 * @}
 */

using namespace Ocean;

/**
 * Definition of interaction modes.
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
enum InteractionMode
{
	/// No interaction.
	IM_NONE,
	/// The user is creating lines or is adjusting lines.
	IM_LINE,
	/// The user is selecting lines.
	IM_SELECT
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNOTATOR_H
