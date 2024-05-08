/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_DEVICE_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_DEVICE_H

#include "ocean/devices/android/Android.h"

#include "ocean/devices/Device.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements a device for the Android library.
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_ANDROID_EXPORT AndroidDevice : virtual public Device
{
	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

	protected:

		/**
		 * Creates a new device by is name.
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		AndroidDevice(const std::string& name, const DeviceType type);
};

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_DEVICE_H
