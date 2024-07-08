/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_DEVICES_H
#define META_OCEAN_DEVICES_DEVICES_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Devices
{

/**
 * @defgroup devices Ocean Devices Abstraction Library
 * @{
 * The Ocean Devices Library is the base library and organizer of all libraries implementing access to devices.
 * This library mainly implements the interfaces for all devices like sensors or trackers and provides the plugin mechanism so that platform-specific devices can be accessed though the same functions and interfaces.
 *
 * The Devices::Manager allows to access the individual devices.
 * Here is the code which allows to access a GPS tracker on any platform (as long as a GPS tracker is available).
 * @code
 * ...
 *
 * // we request a GPS tracker
 * Devices::GPSTrackerRef gpsTracker = Manager::get().device(GPSTracker::deviceTypeGPSTracker());
 *
 * if (gpsTracker)
 * {
 *     // the resulting object is valid so that we can start the GPS tracker
 *     gpsTracker->start();
 *
 *     ...
 *
 *     // access the latest GPS sample
 *     GPSTracker::GPSTrackerSampleRef gpsSample = gpsTracker->sample();
 *
 *     if (gpsSample)
 *     {
 *         size_t numberLocations = gpsSample->locations().size();
 *         ...
 *     }
 * }
 * @endcode
 *
 * Currently the following devices are available:
 * <table>
 * <tr><th>Name <th>Category <th>Platforms <th>Implementation <th> Description
 * <tr><td>"Android 3DOF Acceleration Sensor" <td> Sensor <td> Android <td> Devices::Android::AndroidAccelerationSensor3DOF <td> Sensor providing access to the acceleration force that is applied to a device on all three physical axes, including the force of gravity, with Android's sensor enum TYPE_ACCELEROMETER
 * <tr><td>"Android 3DOF Linear Acceleration Sensor" <td> Sensor <td> Android <td> Devices::Android::AndroidAccelerationSensor3DOF <td> Sensor providing access to the acceleration force that is applied to a device on all three physical axes, excluding the force of gravity, with Android's sensor enum TYPE_LINEAR_ACCELERATION
 * <tr><td>"Android GPS Tracker" <td> Tracker <td> Android <td> Devices::Android::AndroidGPSTracker <td> Tracker providing GPS locations
 * <tr><td>"Android 3DOF Gravity Tracker" <td> Tracker <td> Android <td> Devices::Android::AndroidGravityTracker3DOF <td> 3-DOF orientation tracker providing gravity (direction to ground), with Android's sensor enum TYPE_GRAVITY
 * <tr><td>"Android 3DOF Gyro Raw Sensor" <td> Sensor <td> Android <td> Devices::Android::AndroidGyroSensor3DOF <td>
 * <tr><td>"Android 3DOF Gyro Unbiased Sensor" <td> Sensor <td> Android <td> Devices::Android::AndroidGyroSensor3DOF <td> Sensor providing access to a device's rate of rotation around each of the three physical axes, with Android's enum TYPE_GYROSCOPE
 * <tr><td>"Android 3DOF Heading Tracker" <td> Tracker <td> Android <td> Devices::Android::AndroidHeadingTracker3DOF <td> 3-DOF orientation tracker providing heading (direction to north), with Android's sensor enum TYPE_ROTATION_VECTOR
 * <tr><td>"Android 3DOF Orientation Tracker" <td> Tracker <td> Android <td> Devices::Android::AndroidOrientationTracker3DOF <td> 3-DOF orientation tracker providing the device's attitude, with Android's sensor enum TYPE_GAME_ROTATION_VECTOR
 * <tr><td>
 * <tr><td>"ARCore 6DOF World Tracker" <td> Tracker <td> Android <td> Devices::ARCore::ACWorldTracker6DOF <td> 6-DOF SLAM tracker provided via ARCore
 * <tr><td>
 * <tr><td>"ARKit 6DOF Geo Anchors Tracker" <td> Tracker <td> iOS <td> Devices::ARKit::AKGeoAnchorsTracker6DOF <td> 6-DOF SLAM tracker anchored at GPS locations
 * <tr><td>"ARKit 6DOF World Tracker" <td> Tracker <td> iOS <td> Devices::ARKit::AKWorldTracker6DOF <td> 6-DOF SLAM tracker provided via ARKit
 * <tr><td>
 * <tr><td>"IOS 3DOF Acceleration Sensor" <td> Sensor <td> iOS <td> Devices::IOS::IOSAccelerationSensor3DOF <td> Sensor providing access to the acceleration force that is applied to a device on all three physical axes, including the force of gravity
 * <tr><td>"IOS 3DOF Linear Acceleration Sensor" <td> Sensor <td> iOS <td> Devices::IOS::IOSAccelerationSensor3DOF <td> Sensor providing access to the acceleration force that is applied to a device on all three physical axes, excluding the force of gravity
 * <tr><td>"IOS GPS Tracker" <td> Tracker <td> iOS <td> Devices::IOS::IOSGPSTracker <td> Tracker providing GPS locations
 * <tr><td>"IOS 3DOF Gravity Tracker" <td> Tracker <td> iOS <td> Devices::IOS::IOSGravityTracker3DOF <td> 3-DOF orientation tracker providing gravity (direction to ground)
 * <tr><td>"IOS 3DOF Gyro Raw Sensor" <td> Sensor <td> iOS <td> Devices::IOS::IOSGyroSensor3DOF <td>
 * <tr><td>"IOS 3DOF Gyro Unbiased Sensor" <td> Sensor <td> iOS <td> Devices::IOS::IOSGyroSensor3DOF <td> Sensor providing access to a device's rate of rotation around each of the three physical axes
 * <tr><td>"IOS 3DOF Heading Tracker" <td> Tracker <td> iOS <td> Devices::IOS::IOSHeadingTracker3DOF <td> 3-DOF orientation tracker providing heading (direction to north)
 * <tr><td>"IOS 3DOF Orientation Tracker" <td> Tracker <td> iOS <td> Devices::IOS::IOSOrientationTracker3DOF <td> 3-DOF orientation tracker providing the device's attitude
 * <tr><td>
 * <tr><td>"Headset 6DOF Tracker" <td> Tracker <td> Quest <td> Devices::Quest::VrApi::HeadsetTracker6DOF <td> 6-DOF tracker for the headset
 * <tr><td>"Quest Controller 6DOF Tracker" <td> Tracker <td> Quest <td> Devices::Quest::VrApi::ControllerTracker6DOF <td> 6-DOF tracker for controllers
 * <tr><td>"Floor 6DOF Tracker" <td> Tracker <td> Quest <td> Devices::Quest::VrApi::FloorTracker6DOF <td> 6-DOF tracker for the floor
 * <tr><td>
 * <tr><td>"Pattern 6DOF Tracker" <td> Tracker <td> any <td> Devices::Pattern::PatternTracker6DOF <td> 6-DOF tracker for dynamic/moving images
 * <tr><td>"Static Pattern 6DOF Tracker" <td> Tracker <td> any <td> Devices::Pattern::StaticPatternTracker6DOF <td> 6-DOF tracker for mainly static images also using SLAM
 * </table>
 * @see Manager, Device.
 * @}
 */

/**
 * @namespace Ocean::Devices Namespace of the Devices library.<p>
 * The Namespace Ocean::Devices is used in the entire Ocean Devices Library.
 */

// Defines OCEAN_DEVICES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_EXPORT
		#define OCEAN_DEVICES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_EXPORT
#endif

}

}

#endif // META_OCEAN_DEVICES_DEVICES_H
