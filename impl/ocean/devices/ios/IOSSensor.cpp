/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSSensor.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSSensor::IOSSensor(const std::string& name, const DeviceType type) :
	Device(name, type),
	IOSDevice(name, type),
	Measurement(name, type),
	Sensor(name, type)
{
	sensorObjectId_ = addUniqueObjectId(name);
}

}

}

}
