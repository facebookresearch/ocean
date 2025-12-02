/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

/**
 * @ingroup devices
 * @defgroup devicesserialization Ocean Devices Serialization Library
 * @{
 * The Ocean Devices Serialization Library provides serialization and deserialization functionality for device samples.
 * This library enables device measurements to be stored to and loaded from binary streams using the DataSerializer framework.
 *
 * The library supports serialization of various device types including:
 * - AccelerationSensor3DOF: 3-DOF acceleration sensor measurements
 * - GyroSensor3DOF: 3-DOF gyro sensor measurements
 * - GravityTracker3DOF: 3-DOF gravity tracker measurements
 * - OrientationTracker3DOF: 3-DOF orientation tracker measurements
 * - PositionTracker3DOF: 3-DOF position tracker measurements
 * - Tracker6DOF: 6-DOF tracker measurements (orientation + position)
 * - GPSTracker: GPS location measurements
 * @}
 */

/**
 * @namespace Ocean::Devices::Serialization Namespace of the Devices Serialization library.<p>
 * The Namespace Ocean::Devices::Serialization is used in the entire Ocean Devices Serialization Library.
 */

// Defines OCEAN_DEVICES_SERIALIZATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_SERIALIZATION_EXPORT
		#define OCEAN_DEVICES_SERIALIZATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_SERIALIZATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_SERIALIZATION_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesserialization
 */
OCEAN_DEVICES_SERIALIZATION_EXPORT std::string nameSerializationLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers this Serialization device library at the global device manager.
 * This function calls SerializationFactory::registerFactory() only.
 * @ingroup devicesserialization
 */
void registerSerializationLibrary();

/**
 * Unregisters this Serialization device library at the global device manager.
 * This function calls SerializationFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesserialization
 */
bool unregisterSerializationLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_H
