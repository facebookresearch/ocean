/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SLAM_SLAM_H
#define META_OCEAN_DEVICES_SLAM_SLAM_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

/**
 * @ingroup devices
 * @defgroup devicesslam Ocean Devices SLAM Library
 * @{
 * The Ocean Devices SLAM Library provides a SLAM tracker based on feature points.
 * This device plugin provides a 6DOF feature tracker with unique name <strong>"SLAM Feature Based 6DOF Tracker"</strong>.<br>
 * Do not create a tracker object directory, but use the Devices::Manager object instead.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::SLAM Namespace of the Devices SLAM library.<p>
 * The Namespace Ocean::Devices::SLAM is used in the entire Ocean Devices SLAM Library.
 */

// Defines OCEAN_DEVICES_SLAM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_SLAM_EXPORT
		#define OCEAN_DEVICES_SLAM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_SLAM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_SLAM_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesslam
 */
OCEAN_DEVICES_SLAM_EXPORT std::string nameSLAMLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this SLAM device library at the global device manager.
 * This function calls SLAMFactory::registerFactory() only.
 * @ingroup devicesslam
 */
void registerSLAMLibrary();

/**
 * Unregisters this SLAM device library at the global device manager.
 * This function calls SLAMFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesslam
 */
bool unregisterSLAMLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_SLAM_SLAM_H
