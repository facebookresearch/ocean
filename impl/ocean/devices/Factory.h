/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_FACTORY_H
#define META_OCEAN_DEVICES_FACTORY_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Device.h"
#include "ocean/devices/DeviceRef.h"

#include <vector>

namespace Ocean
{

namespace Devices
{

/**
 * This class implements a factory able to create instances of devices.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Factory
{
	friend class Manager;
	friend struct std::default_delete<Factory>;

	protected:

		/**
		 * Definition of a callback function creating a specific device.
		 */
		typedef Callback<Device*, const std::string&, const Device::DeviceType&> InstanceFunction;

		/**
		 * This class stores information to describe and to creator a device.
		 */
		class DeviceDescriptor
		{
			public:

				/**
				 * Creates an empty device triple object.
				 */
				inline DeviceDescriptor();

				/**
				 * Creates a new device descritpr object.
				 * @param name The name of the device
				 * @param type The type of the device
				 * @param instanceFunction The callback function to create the new device, must be valid
				 */
				inline DeviceDescriptor(const std::string& name, const Device::DeviceType type, const InstanceFunction& instanceFunction);

			public:

				/// Device name.
				std::string name_;

				/// Device type.
				Device::DeviceType type_;

				/// Device instance function.
				InstanceFunction instanceFunction_;
		};

		/**
		 * Definition of a vector holding device descriptor objects.
		 */
		typedef std::vector<DeviceDescriptor> DeviceDescriptors;

	public:

		/**
		 * Returns the name of this factory.
		 */
		inline const std::string& name() const;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param factory Object which would be copied
		 */
		Factory(const Factory& factory) = delete;

		/**
		 * Creates a new factory.
		 * @param name The name of the factory to create
		 */
		explicit Factory(const std::string& name);

		/**
		 * Destructs this factory.
		 */
		virtual ~Factory();

		/**
		 * Returns a list of available devices.
		 * @return Device list
		 */
		virtual Strings devices() const;

		/**
		 * Returns a list of available devices matching a specified major device type.
		 * @param type Type to return device names for
		 * @return Device list
		 */
		virtual Strings devices(const Device::DeviceType type) const;

		/**
		 * Returns the type of a specified device.
		 * @param device Name of the device to return the type for
		 * @return Device type holding major and minor type
		 */
		virtual Device::DeviceType deviceType(const std::string& device) const;

		/**
		 * Creates a new device by its name or returns an existing one if an exclusive use is not necessary.
		 * @param name The name of the device to create
		 * @param useExclusive Determines whether the device will be used exclusively
		 * @return Requested device
		 */
		virtual DeviceRef device(const std::string& name, const bool useExclusive = false) const;

		/**
		 * Creates a new device defined by a major and a minor device type or returns an existing one if an exclusive use is not necessary.
		 * @param type Type of the device to return
		 * @param useExclusive Determines whether the device will be used exclusively
		 * @return Requested device
		 */
		virtual DeviceRef device(const Device::DeviceType type, const bool useExclusive = false);

		/**
		 * Returns a new adapter measurement device, which is exclusive always.
		 * @param type Type of the device to return
		 * @param name Unique name of the resulting adapter device, this name will be assigned to the new adapter device
		 */
		DeviceRef adapterDevice(const Device::DeviceType type, const std::string& name);

		/**
		 * Disabled copy operator.
		 * @param factory Object which would be copied
		 * @return Reference to this object
		 */
		Factory& operator=(const Factory& factory) = delete;

		/**
		 * Registers a factory at the manager.
		 * Each factory should be registered at most once.
		 * @param factory The factory to register
		 * @return True, if the factory hasn't been registered before
		 */
		static bool registerFactory(std::unique_ptr<Factory>&& factory);

		/**
		 * Unregisters a factory at the manager.
		 * Beware: All devices created by the factory must be released before,<br>
		 * otherwise the factory cannot be unregistered.
		 * @param factory Name of the factory to unregister
		 * @return True, if the factory could be unregistered
		 */
		static bool unregisterFactory(const std::string& factory);

		/**
		 * Registes a device at this factory.
		 * @param deviceName Unique name of the device to register
		 * @param deviceType Type of the device
		 * @param deviceInstanceFunction Function creating an instance of the device
		 * @see unregisterDevice().
		 */
		bool registerDevice(const std::string& deviceName, const Device::DeviceType deviceType, const InstanceFunction& deviceInstanceFunction);

		/**
		 * Unregisters a previously registered device from this factory.
		 * Beware: Ensure that the device is not used anymore before unregistering it.
		 * @param deviceName The name of the device to unregister, must be valid
		 * @return True, if succeeded
		 * @see registerDevice().
		 */
		bool unregisterDevice(const std::string& deviceName);

		/**
		 * Creates a new device by a given device triple.
		 * @param deviceDescriptor The descriptor of the device to be created
		 * @param useExclusive Determines whether the device will be used exclusively
		 * @return New device if succeeded
		 */
		static DeviceRef createDevice(const DeviceDescriptor& deviceDescriptor, bool useExclusive);

	private:

		/// The factory's name.
		std::string name_;

		/// Vector holding all registered devices with name and type.
		DeviceDescriptors deviceDescriptors_;

		/// Factory lock.
		mutable Lock lock_;
};

inline Factory::DeviceDescriptor::DeviceDescriptor() :
	type_(Device::DEVICE_INVALID)
{
	// nothing to do here
}

inline Factory::DeviceDescriptor::DeviceDescriptor(const std::string& name, const Device::DeviceType type, const InstanceFunction& instanceFunction) :
	name_(name),
	type_(type),
	instanceFunction_(instanceFunction)
{
	// nothing to do here
}

inline const std::string& Factory::name() const
{
	return name_;
}

}

}

#endif // META_OCEAN_DEVICES_FACTORY_H
