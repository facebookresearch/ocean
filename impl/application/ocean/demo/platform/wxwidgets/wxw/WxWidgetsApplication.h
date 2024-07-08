/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_APPLICATION_H

#include "application/ocean/demo/platform/wxwidgets/wxw/WxWidgets.h"

/**
 * This class implements the application.
 * @ingroup applicationdemoplatformwxwidgetswxw
 */
class WxWidgetsApplication : public wxApp
{
	private:

		/**
		 * Initialization event function.
		 * @return True, if succeeded
		 */
		virtual bool OnInit();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WXWIDGETS_APPLICATION_H
