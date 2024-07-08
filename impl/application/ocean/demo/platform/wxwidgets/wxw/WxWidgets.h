/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WX_WIDGETS_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WX_WIDGETS_H

#include "application/ocean/demo/platform/ApplicationDemoPlatform.h"

#ifdef _WINDOWS
	#include <winsock2.h>
#endif

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcomma"
#endif
    #include <wx/wx.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

/**
 * @ingroup applicationdemoplatform
 * @defgroup applicationdemoplatformwxwidgetswxw WxWidgets (WxWidgets)
 * @{
 * The demo application demonstrates the implementation of a simple WxWidgets application.<br>
 * This application is platform independent (as long as WxWidgets supports the platform).<br>
 * @}
 */

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_WX_WIDGETS_WXW_WX_WIDGETS_H
