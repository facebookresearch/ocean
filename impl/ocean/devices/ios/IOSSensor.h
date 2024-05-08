/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_SENSOR_H
#define META_OCEAN_DEVICES_IOS_IOS_SENSOR_H

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSDevice.h"

#include "ocean/devices/Sensor.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a sensor for the IOS library.
 * The coordinate system of each IOS sensor is defined so that the origin of the coordinate system is located in the center of the device.<br>
 * The x-axis is horizontal and pointing to the right of the device (if the device is held in default orientation).<br>
 * The y-axis is vertical and pointing to the top of the device.<br>
 * The z-axis is perpendicular to the screen plane and pointing towards the user (a right handed coordinate system).
 * @ingroup deviceios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSSensor :
	virtual public IOSDevice,
	virtual public Sensor
{
	protected:

		/**
		 * Creates a new sensor by its name and type.
		 * @param name The name of the sensor
		 * @param type Major and minor device type of the sensor
		 */
		IOSSensor(const std::string& name, const DeviceType type);

	protected:

		/// The object id of this sensor.
		ObjectId sensorObjectId_ = invalidObjectId();
};

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_SENSOR_H
