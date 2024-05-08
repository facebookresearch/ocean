/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_REF_H
#define META_OCEAN_DEVICES_REF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Device.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Devices
{

/**
 * This class implements a device reference with an internal reference counter.
 * @see Device
 * @ingroup devices
 */
typedef ObjectRef<Device> DeviceRef;

/**
 * This class implements a smart device reference.
 * @tparam T Type of the derived object that has to be encapsulated by the smart reference object
 * @ingroup devices
 */
template <typename T>
class SmartDeviceRef : public SmartObjectRef<T, Device>
{
	private:

		/**
		 * Redefinition of the release callback function defined in ObjectRef.
		 */
		typedef typename SmartObjectRef<T, Device>::ReleaseCallback ReleaseCallback;

	public:

		/**
		 * Creates an empty smart device reference.
		 */
		SmartDeviceRef();

		/**
		 * Creates a new smart device reference by a given device reference.
		 * @param deviceRef Device reference to copy
		 */
		SmartDeviceRef(const DeviceRef& deviceRef);

		/**
		 * Creates a new SmartDeviceRef by a given object.
		 * This given object will be released by the smart device reference itself.
		 * @param object Internal object
		 */
		explicit SmartDeviceRef(Device* object);

		/**
		 * Copies a smart device reference.
		 * @param reference Reference to copy
		 */
		template <typename T2> SmartDeviceRef(const SmartDeviceRef<T2>& reference);

		/**
		 * Assigns a smart device reference.
		 * @param deviceRef reference Reference to assign
		 * @return Reference to this object
		 */
		SmartDeviceRef& operator=(const DeviceRef& deviceRef);
};

/**
 * This class implements a manager for device references.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT DeviceRefManager : public Singleton<DeviceRefManager>
{
	friend class Device;
	friend class Singleton<DeviceRefManager>;
	friend class ObjectRef<Device>;
	friend class Manager;

	public:

		/**
		 * Definition of a callback function for devices.
		 */
		typedef Callback<void, Device*, bool> DeviceCallback;

	protected:

		/**
		 * Definition of a pair combining a device reference with a state specifying whether the medium is used exclusively.
		 */
		typedef std::pair<DeviceRef, bool> DevicePair;

		/**
		 * Map mapping urls to device references.
		 */
		typedef std::multimap<std::string, DevicePair> DeviceMap;

		/**
		 * Definition of device callback functions.
		 */
		typedef Callbacks<DeviceCallback> DeviceCallbacks;

	public:

		/**
		 * Registers a new device.
		 * @param device Device object to manage
		 * @param exclusive State specifying whether the device will be used exclusively
		 * @return Device reference
		 */
		DeviceRef registerDevice(Device* device, const bool exclusive);

		/**
		 * Returns a device by a given device name.
		 * If the device does not exist an empty reference is returned.
		 * @param name The name of the new device
		 * @return Device reference of the requested device
		 */
		DeviceRef device(const std::string& name) const;

		/**
		 * Returns a specified device by it's device type.
		 * @param type Device type
		 * @return Requested device
		 */
		DeviceRef device(const Device::DeviceType type) const;

		/**
		 * Returns whether no device is registered currently.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether no device is registered created by a specific library.
		 * @param library The name of the library to check, must be valid
		 * @return True, if so
		 */
		bool isEmpty(const std::string& library) const;

		/**
		 * Returns the name of all existing devices which belong to a specific library.
		 * @param library The name of the library to which the devices belong, must be valid
		 * @return The names of all currently existing devices
		 */
		std::vector<std::string> devicesFromLibrary(const std::string& library) const;

		/**
		 * Adds a callback function which is called whenever a new device is created or deleted.
		 * The callback function will be called immediately for all already existing devices.
		 * @param callback The callback function, must be valid
		 */
		void addDeviceCallbackFunction(const DeviceCallback& callback);

		/**
		 * Removes a previously added callback function for device events.
		 * @param callback The callback function, must be valid
		 */
		inline void removeDeviceCallbackFunction(const DeviceCallback& callback);

	protected:

		/**
		 * Destructs the manager.
		 */
		virtual ~DeviceRefManager();

		/**
		 * Returns whether a specified device is registered as exclusive.
		 * @param device Device to check
		 * @return True, if so
		 */
		bool isExclusive(const Device* device);

		/**
		 * Unregisters a device.
		 */
		void unregisterDevice(const Device* device);

	protected:

		/// Map holding all device references.
		DeviceMap deviceMap_;

		/// The callback functions for device events.
		DeviceCallbacks deviceCallbacks_;

		/// Lock for the device map
		mutable Lock lock_;
};

template <typename T>
SmartDeviceRef<T>::SmartDeviceRef() :
	SmartObjectRef<T, Device>()
{
	// nothing to do here
}

template <typename T>
SmartDeviceRef<T>::SmartDeviceRef(const DeviceRef& deviceRef) :
	SmartObjectRef<T, Device>(deviceRef)
{
	// nothing to do here
}

template <typename T>
SmartDeviceRef<T>::SmartDeviceRef(Device* object) :
	SmartObjectRef<T, Device>(object, ReleaseCallback(DeviceRefManager::get(), &DeviceRefManager::unregisterDevice))
{
	// nothing to do here
}

template <typename T>
template <typename T2>
SmartDeviceRef<T>::SmartDeviceRef(const SmartDeviceRef<T2>& reference) :
	SmartObjectRef<T, Device>(reference)
{
	// nothing to do here
}

template <typename T>
SmartDeviceRef<T>& SmartDeviceRef<T>::operator=(const DeviceRef& deviceRef)
{
	SmartObjectRef<T, Device>::operator=(deviceRef);
	return *this;
}

inline bool DeviceRefManager::isEmpty() const
{
	const ScopedLock scopedLock(lock_);

	return deviceMap_.empty();
}

inline void DeviceRefManager::removeDeviceCallbackFunction(const DeviceCallback& callback)
{
	ocean_assert(callback);
	deviceCallbacks_.removeCallback(callback);
}

}

}

#endif // META_OCEAN_DEVICES_DEF_H
