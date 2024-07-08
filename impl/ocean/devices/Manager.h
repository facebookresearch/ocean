/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MANAGER_H
#define META_OCEAN_DEVICES_MANAGER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Device.h"
#include "ocean/devices/Factory.h"
#include "ocean/devices/Measurement.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <vector>

namespace Ocean
{

namespace Devices
{

/**
 * This class implements as singleton-based manager which allows to access all available devices.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;
	friend class Factory;
	friend class Measurement;

	public:

		/**
		 * Definition of a callback function creating an ad-hoc device.
		 */
		using AdhocInstanceFunction = Factory::InstanceFunction;

	protected:

		/**
		 * This class implements an ad-hoc factory for devices which are implemented outside of an own library e.g., during rapid prototyping.
		 * In general, devices should be implemented in an own dedicated plugin-based library.
		 * However, for prototyping the ad-hoc factory offers a simple possibility to create devices without the overhead of writing the plugin related code.
		 */
		class AdhocFactory : public Factory
		{
			friend class Manager;

			protected:

				/**
				 * Creates a new factory object.
				 */
				AdhocFactory();
		};

		/**
		 * Definition of a vector holding unique pointers of factories.
		 */
		typedef std::vector<std::unique_ptr<Factory>> FactoryUniquePointers;

		/**
		 * Definition of an unsorted multimap mapping object ids to descriptions.
		 */
		typedef std::unordered_multimap<Measurement::ObjectId, std::string> ObjectIdMultimap;

	public:

		/**
		 * Returns a specific device.
		 * @param name The name of the device to return, must be valid
		 * @param useExclusive True, if the caller would like to use this device exclusively; False, if the device can be shared
		 * @return The requested device, if available
		 */
		DeviceRef device(const std::string& name, const bool useExclusive = false);

		/**
		 * Returns a specified device.
		 * @param type The major type of the device to return
		 * @param useExclusive True, if the caller would like to use this device exclusively; False, if the device can be shared
		 * @return The requested device, if available
		 */
		DeviceRef device(const Device::DeviceType type, const bool useExclusive = false);

		/**
		 * Returns a list with the names of all available devices.
		 * @return Device names
		 */
		Strings devices() const;

		/**
		 * Returns a list with the names of all available devices matching a major and minor device type.
		 * @param type Type of the device to return
		 * @return Device names
		 */
		Strings devices(const Device::DeviceType type) const;

		/**
		 * Returns whether a specified library is registered at this manager.
		 * @param library Name of the library to check
		 * @return True, if so
		 */
		bool isRegistered(const std::string& library);

		/**
		 * Returns the names of all registered libraries.
		 * @return Registered library names
		 */
		Strings libraries() const;

		/**
		 * Returns the description of an object id.
		 * @param objectId The object id for which the description will be returned
		 * @return The description of the object, empty if the object id is unknown
		 */
		std::string objectDescription(const Measurement::ObjectId objectId) const;

		/**
		 * Destructs all registered libraries and their corresponding factories.
		 */
		void release();

		/**
		 * Registes an ad-hoc device at this manager.
		 * In general, devices should be implemented in an own decicated plugin-based library.
		 * However, for prototyping an ad-hoc device cames with the same features as a device implemented in a dedicatd library without the overhead of writing the code for the plugin mechanisms etc.
		 * @param deviceName Unique name of the device to register
		 * @param deviceType Type of the device
		 * @param deviceInstanceFunction Function creating an instance of the device
		 * @see unregisterAdhocDevice().
		 */
		bool registerAdhocDevice(const std::string& deviceName, const Device::DeviceType& deviceType, const AdhocInstanceFunction& deviceInstanceFunction);

		/**
		 * Unregisters a previously registered ad-hoc device from this manager.
		 * Beware: Ensure that the device is not used anymore before unregistering it.
		 * @param deviceName The name of the device to unregister, must be valid
		 * @return True, if succeeded
		 * @see registerAdhocDevice().
		 */
		bool unregisterAdhocDevice(const std::string& deviceName);

		/**
		 * Returns the name of the ad-hoc factory.
		 * The ad-hoc factory is for devices which are implemented outside of an own library e.g., during rapid prototyping.
		 * @return The name of the ad-hoc factor
		 */
		static std::string nameAdhocFactory();

	private:

		/**
		 * Creates a new manager object.
		 */
		Manager();

		/**
		 * Destructs a manager object.
		 */
		virtual ~Manager();

		/**
		 * Creates a unique object id for a new object (e.g., a tracking object like an image, a marker, or a location).
		 * @param description The description of the new object, must be valid
		 * @return The unique id which is unique across all devices
		 */
		Measurement::ObjectId createUniqueObjectId(const std::string& description);

		/**
		 * Registers a new factory.
		 * @param factory The factory to register
		 * @return True, if the factory hasn't been registered before
		 */
		bool registerFactory(std::unique_ptr<Factory>&& factory);

		/**
		 * Unregisters a factory.
		 * @param factory Name of the factory to unregister
		 * @return True, if succeeded
		 */
		bool unregisterFactory(const std::string& factory);

	private:

		/// The map mapping object ids to descriptions.
		ObjectIdMultimap objectIdMultimap_;

		/// The counter for unique object ids.
		Measurement::ObjectId uniqueObjectIdCounter_ = Measurement::ObjectId(0);

		/// All registered device factories.
		FactoryUniquePointers factories_;

		/// The Manager's lock.
		mutable Lock lock_;
};

}

}

#endif // META_OCEAN_DEVICES_MANAGER_H
