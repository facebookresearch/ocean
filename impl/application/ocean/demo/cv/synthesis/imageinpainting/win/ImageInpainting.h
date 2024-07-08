/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_H

#include "application/ocean/demo/cv/synthesis/ApplicationDemoCVSynthesis.h"

#include "ocean/platform/wxwidgets/WxWidgets.h"

/**
 * @ingroup applicationdemocvsynthesis
 * @defgroup applicationdemocvsynthesisimageinpaintingwxw ImageInpainting (WxWidgets)
 * @{
 * The demo application demonstrates the implementation of an image inpainting application.<br>
 * This application is platform independent (as long as WxWidgets supports the platform).<br>
 * @}
 */

/**
 * Definition of interaction modes.
 * @ingroup applicationdemocvsynthesisimageinpaintingwin
 */
enum InteractionMode
{
	/// No interaction.
	IM_NONE,
	/// Mark interaction.
	IM_MARK,
	/// Line interaction.
	IM_LINE
};

using namespace Ocean;

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_H
