/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WXWIDGETS_GL_WXWIDGETS_H
#define META_OCEAN_PLATFORM_GL_WXWIDGETS_GL_WXWIDGETS_H

#include "ocean/platform/gl/GL.h"

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

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

/**
 * @ingroup platformgl
 * @defgroup platformglwxwidgets Ocean Platform GL WxWidgets Library
 * @{
 * The Ocean GL Library provides specific functionalities for OpenGL using WxWidgets.
 * The library is available for all platforms WxWidgets supports.
 * @}
 */

/**
 * @namespace Ocean::Platform::GL::WxWidgets Namespace of the Platform GL WxWidgets library.<p>
 * The Namespace Ocean::Platform::GL::WxWidgets is used in the entire Ocean Platform GL WxWidgets Library.
 */

// Defines OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT
		#define OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_GL_WXWIDGETS_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WXWIDGETS_GL_WXWIDGETS_H
