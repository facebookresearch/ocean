/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GPS_TRACKER_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GPS_TRACKER_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationTracker.h"

#include "ocean/devices/GPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationGPSTracker;

/**
 * Definition of a smart object reference for a Serialization GPS tracker.
 * @see SerializationGPSTracker.
 * @ingroup devicesserialization
 */
using SerializationGPSTrackerRef = SmartDeviceRef<SerializationGPSTracker>;

/**
 * This class implements a Serialization GPS tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationGPSTracker :
	virtual public GPSTracker,
	virtual public SerializationTracker
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param locations The GPS locations to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(GPSTracker::Locations&& locations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSerializationGPSTracker();

	protected:

		/**
		 * Creates a new Serialization GPS tracker object.
		 * @param name The name of the tracker
		 */
		explicit SerializationGPSTracker(const std::string& name);

		/**
		 * Destructs a Serialization GPS tracker object.
		 */
		~SerializationGPSTracker() override;
};

inline SerializationGPSTracker::DeviceType SerializationGPSTracker::deviceTypeSerializationGPSTracker()
{
	return deviceTypeGPSTracker();
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GPS_TRACKER_H
