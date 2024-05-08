/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_H

#include "application/ocean/demo/media/ApplicationDemoMedia.h"

#ifdef _WINDOWS
	#include <WinSock2.h>
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
 * @ingroup applicationdemomedia
 * @defgroup applicationdemomediamoviepreviewwxw Moview Preview (WxWidgets)
 * @{
 * The demo application demonstrates the implementation of a simple movie preview application.<br>
 * This application is platform independent.<br>
 * @}
 */

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_MOVIE_PREVIEW_WXWIDGETS_MOVIE_PREVIEW_H
