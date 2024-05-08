/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_DEVICES_H
#define META_OCEAN_MEDIA_AVF_DEVICES_H

#include "ocean/media/avfoundation/AVFoundation.h"

#include <vector>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements a simple enumerator for devices available via the AVFoundation library.
 * @ingroup mediaavf
 */
class AVFDevices
{
	public:

		/**
		 * Definition of a simple class combining device name and unique device id.
		 */
		class Device
		{
			friend class AVFDevices;

			public:

				/**
				 * Returns the user-friendly name of this device.
				 * @return The device's readable name
				 */
				inline const std::wstring& friendlyName() const;

				/**
				 * Returns the unique id of this device.
				 * @return The device's unique id
				 */
				inline const std::wstring& uniqueId() const;

			protected:

				/**
				 * Creates a new device object.
				 * @param friendlyName The user-friendly name of the device
				 * @param uniqueId The unique id of the device
				 */
				inline Device(const std::wstring& friendlyName, const std::wstring& uniqueId);

			protected:

				/// The user-friendly name of this device.
				std::wstring deviceFriendlyName;

				/// The unique id of this device.
				std::wstring deviceUniqueId;
		};

		/**
		 * Definition of a vector holding devices.
		 */
		typedef std::vector<Device> Devices;

	public:

		/**
		 * Returns the list of currently available video devices.
		 * @return The currently available video devices
		 */
		static Devices videoDevices();
};

inline AVFDevices::Device::Device(const std::wstring& friendlyName, const std::wstring& uniqueId) :
	deviceFriendlyName(friendlyName),
	deviceUniqueId(uniqueId)
{
	// nothing to do here
}

inline const std::wstring& AVFDevices::Device::friendlyName() const
{
	return deviceFriendlyName;
}

inline const std::wstring& AVFDevices::Device::uniqueId() const
{
	return deviceUniqueId;
}

}

}

}

#endif // META_OCEAN_MEDIA_AVF_DEVICES_H
