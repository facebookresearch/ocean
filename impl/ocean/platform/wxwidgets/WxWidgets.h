/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_H

#include "ocean/platform/Platform.h"

#ifdef _WINDOWS
	#include <winsock2.h>
#endif

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcomma"
#endif

#include <wx/wx.h>

#ifndef OCEAN_WXWIDGETS_HANDLE
	#if defined(OCEAN_PLATFORM_BUILD_LINUX)
		#define OCEAN_WXWIDGETS_HANDLE WXWidget
	#else
		#define OCEAN_WXWIDGETS_HANDLE WXHWND
	#endif
#endif

#ifdef __clang__
    #pragma clang diagnostic pop
#endif

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * @ingroup platform
 * @defgroup platformwxw Ocean Platform WxWidgets Library
 * @{
 * The Ocean WxWidgets Library provides specific functionalities for platforms supported by WxWidgets.
 * The library is available on several platforms.
 * @}
 */

/**
 * @namespace Ocean::Platform::WxWidgets Namespace of the Platform WxWidgets library.<p>
 * The Namespace Ocean::Platform::WxWidgets is used in the entire Ocean Platform WxWidgets Library.
 */

// Defines OCEAN_PLATFORM_WXWIDGETS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_WXWIDGETS_EXPORT
		#define OCEAN_PLATFORM_WXWIDGETS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_WXWIDGETS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_WXWIDGETS_EXPORT
#endif

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_WIDGETS_H
