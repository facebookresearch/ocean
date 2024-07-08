/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_PATTERN_PATTERN_H
#define META_OCEAN_DEVICES_PATTERN_PATTERN_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

/**
 * @ingroup devices
 * @defgroup devicespattern Ocean Devices Pattern Library
 * @{
 * The Ocean Devices Pattern Library provides a Pattern feature based tracking library.
 * This device plugin provides a 6DOF feature tracker with unique name <strong>"Pattern 6DOF Tracker"</strong>.<br>
 * Do not create a tracker object directly, but use the Devices::Manager object instead.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::Pattern Namespace of the Devices Pattern library.<p>
 * The Namespace Ocean::Devices::Pattern is used in the entire Ocean Devices Pattern Library.
 */

// Defines OCEAN_DEVICES_PATTERN_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_PATTERN_EXPORT
		#define OCEAN_DEVICES_PATTERN_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_PATTERN_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_PATTERN_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicespattern
 */
OCEAN_DEVICES_PATTERN_EXPORT std::string namePatternLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this Pattern device library at the global device manager.
 * This function calls PatternFactory::registerFactory() only.
 * @ingroup devicespattern
 */
void registerPatternLibrary();

/**
 * Unregisters this Pattern device library at the global device manager.
 * This function calls PatternFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicespattern
 */
bool unregisterPatternLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_PATTERN_PATTERN_H
