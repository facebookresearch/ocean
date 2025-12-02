/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_POSITION_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_POSITION_TRACKER_3DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationTracker.h"

#include "ocean/devices/PositionTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationPositionTracker3DOF;

/**
 * Definition of a smart object reference for a Serialization 3DOF position tracker.
 * @see SerializationPositionTracker3DOF.
 * @ingroup devicesserialization
 */
using SerializationPositionTracker3DOFRef = SmartDeviceRef<SerializationPositionTracker3DOF>;

/**
 * This class implements a Serialization 3DOF position tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationPositionTracker3DOF :
	virtual public PositionTracker3DOF,
	virtual public SerializationTracker
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param positions The positions to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(PositionTracker3DOFSample::Positions&& positions, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSerializationPositionTracker3DOF();

	protected:

		/**
		 * Creates a new Serialization 3DOF position tracker object.
		 * @param name The name of the tracker
		 */
		explicit SerializationPositionTracker3DOF(const std::string& name);

		/**
		 * Destructs a Serialization 3DOF position tracker object.
		 */
		~SerializationPositionTracker3DOF() override;
};

inline SerializationPositionTracker3DOF::DeviceType SerializationPositionTracker3DOF::deviceTypeSerializationPositionTracker3DOF()
{
	return deviceTypePositionTracker3DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_POSITION_TRACKER_3DOF_H
