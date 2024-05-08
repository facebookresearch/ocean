/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_APPLICATION_H

#include "application/ocean/demo/rendering/renderer/wxw/RendererWxWidgets.h"

/**
 * This class implements the application.
 * @ingroup applicationdemorenderingrendererwxw
 */
class RendererWxWidgetsApplication : public wxApp
{
	private:

		/**
		 * Initialization event function.
		 * @return True, if succeeded
		 */
		bool OnInit() override;

		/**
		 * Exit event function.
		 * @return The return value of the base class implementation.
		 */
		int OnExit() override;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WXW_RENDERER_WXW_APPLICATION_H
