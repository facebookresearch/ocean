/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_PLATFORM_H
#define META_OCEAN_PLATFORM_PLATFORM_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Platform
{

/**
 * @defgroup platform Ocean Platform Library
 * @{
 * The Ocean Platform Library provides specific functionalities for available on specific platforms only.
 * The library is platform dependent.
 * @}
 */

/**
 * @namespace Ocean::Platform Namespace of the Platform library.<p>
 * The Namespace Ocean::Platform is used in the entire Ocean Platform Library.
 */

// Defines OCEAN_PLATFORM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_EXPORT
		#define OCEAN_PLATFORM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_EXPORT
#endif

}

}

#endif // META_OCEAN_PLATFORM_PLATFORM_H
