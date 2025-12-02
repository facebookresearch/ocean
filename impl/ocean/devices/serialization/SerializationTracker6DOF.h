/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_6DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationTracker.h"

#include "ocean/devices/Tracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationTracker6DOF;

/**
 * Definition of a smart object reference for a Serialization 6DOF tracker.
 * @see SerializationTracker6DOF.
 * @ingroup devicesserialization
 */
using SerializationTracker6DOFRef = SmartDeviceRef<SerializationTracker6DOF>;

/**
 * This class implements a Serialization 6DOF tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationTracker6DOF :
	virtual public Tracker6DOF,
	virtual public SerializationTracker
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param orientations The orientations to forward
		 * @param positions The positions to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(Tracker6DOFSample::Orientations&& orientations, Tracker6DOFSample::Positions&& positions, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSerializationTracker6DOF();

	protected:

		/**
		 * Creates a new Serialization 6DOF tracker object.
		 * @param name The name of the tracker
		 */
		explicit SerializationTracker6DOF(const std::string& name);

		/**
		 * Destructs a Serialization 6DOF tracker object.
		 */
		~SerializationTracker6DOF() override;
};

inline SerializationTracker6DOF::DeviceType SerializationTracker6DOF::deviceTypeSerializationTracker6DOF()
{
	return deviceTypeTracker6DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_TRACKER_6DOF_H
