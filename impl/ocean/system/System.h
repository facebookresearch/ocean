/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_SYSTEM_H
#define META_OCEAN_SYSTEM_SYSTEM_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace System
{

/**
 * @defgroup system Ocean System Library
 * @{
 * The Ocean System Library provides all system functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::System Namespace of the System library.<p>
 * The Namespace Ocean::System is used in the entire Ocean System Library.
 */

// Defines OCEAN_SYSTEM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SYSTEM_EXPORT
		#define OCEAN_SYSTEM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SYSTEM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SYSTEM_EXPORT
#endif

}

}

#endif // META_OCEAN_SYSTEM_SYSTEM_H
