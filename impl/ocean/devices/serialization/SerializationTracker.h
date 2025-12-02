/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/devices/Tracker.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationTracker;

/**
 * Definition of a smart object reference for a Serialization tracker.
 * @see SerializationTracker.
 * @ingroup devicesserialization
 */
using SerializationTrackerRef = SmartDeviceRef<SerializationTracker>;

/**
 * This class implements a Serialization tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationTracker : virtual public Tracker
{
	friend class SerializationFactory;

	public:

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

	protected:

		/**
		 * Creates a new tracker object.
		 * @param name The name of the tracker
		 * @param type Major and minor device type of the device
		 */
		SerializationTracker(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new Serialization tracker object.
		 */
		~SerializationTracker() override;

	protected:

		/// True, if the tracker is started.
		bool isStarted_ = false;

		/// The name of the library of this device.
		std::string library_;

		/// The set of all currently found objects, specified by external object ids.
		ObjectIdSet currentlyFoundExternalObjectIds_;
};

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_H
