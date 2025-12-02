/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GRAVITY_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GRAVITY_TRACKER_3DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationTracker.h"

#include "ocean/devices/GravityTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationGravityTracker3DOF;

/**
 * Definition of a smart object reference for a Serialization 3DOF gravity tracker.
 * @see SerializationGravityTracker3DOF.
 * @ingroup devicesserialization
 */
using SerializationGravityTracker3DOFRef = SmartDeviceRef<SerializationGravityTracker3DOF>;

/**
 * This class implements a Serialization 3DOF gravity tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationGravityTracker3DOF :
	virtual public GravityTracker3DOF,
	virtual public SerializationTracker
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param gravities The gravity measurements to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(GravityTracker3DOFSample::Gravities&& gravities, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSerializationGravityTracker3DOF();

	protected:

		/**
		 * Creates a new Serialization 3DOF gravity tracker object.
		 * @param name The name of the tracker
		 */
		explicit SerializationGravityTracker3DOF(const std::string& name);

		/**
		 * Destructs a Serialization 3DOF gravity tracker object.
		 */
		~SerializationGravityTracker3DOF() override;
};

inline SerializationGravityTracker3DOF::DeviceType SerializationGravityTracker3DOF::deviceTypeSerializationGravityTracker3DOF()
{
	return deviceTypeGravityTracker3DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_GRAVITY_TRACKER_3DOF_H
