/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ORCA_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ORCA_H

#include <afxcontrolbars.h>

#include "resource.h"

#include <windows.h>
#include <winsock2.h>

// remove the Windows definition of min
#ifdef min
	#undef min
#endif

// remove the Windows definition of max
#ifdef max
	#undef max
#endif

#include "application/ocean/orca/Orca.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * @ingroup orca
 * @defgroup orcawin Ocean Orca VR/AR viewer for Windows desktop platforms.
 * @{
 * The VR/AR viewer of the Ocean framework for desktop platforms running Windows.<br>
 * The application is platform dependent and to date available on Windows desktop platforms only.
 * @}
 */

/**
 * @namespace Ocean::Orca::Win Namespace of the Ocean Orca VR/AR viewer application for Windows desktop platforms.<p>
 */

#ifndef _WINDOWS
	#error This application is available on windows platforms only!
#endif // _WINDOWS

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_ORCA_H
