/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_H

#include "application/ocean/demo/rendering/ApplicationDemoRendering.h"

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
 * @ingroup applicationdemorendering
 * @defgroup applicationdemorenderingopenglwxw Rendering OpenGL (WxWidgets)
 * @{
 * The demo application demonstrates the implementation of a simple OpenGL application.<br>
 * This application is platform independent (as long as WxWidgets supports the platform).<br>
 * @}
 */

using namespace Ocean;

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_H
