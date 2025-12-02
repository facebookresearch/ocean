/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ORIENTATION_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ORIENTATION_TRACKER_3DOF_H

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationTracker.h"

#include "ocean/devices/OrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

// Forward declaration.
class SerializationOrientationTracker3DOF;

/**
 * Definition of a smart object reference for a Serialization 3DOF orientation tracker.
 * @see SerializationOrientationTracker3DOF.
 * @ingroup devicesserialization
 */
using SerializationOrientationTracker3DOFRef = SmartDeviceRef<SerializationOrientationTracker3DOF>;

/**
 * This class implements a Serialization 3DOF orientation tracker.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializationOrientationTracker3DOF :
	virtual public OrientationTracker3DOF,
	virtual public SerializationTracker
{
	friend class SerializationFactory;
	friend class SerializerDevicePlayer;

	public:

		/**
		 * Forwards sample events.
		 * @param orientations The orientations to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(OrientationTracker3DOFSample::Orientations&& orientations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSerializationOrientationTracker3DOF();

	protected:

		/**
		 * Creates a new Serialization 3DOF orientation tracker object.
		 * @param name The name of the tracker
		 */
		explicit SerializationOrientationTracker3DOF(const std::string& name);

		/**
		 * Destructs a Serialization 3DOF orientation tracker object.
		 */
		~SerializationOrientationTracker3DOF() override;
};

inline SerializationOrientationTracker3DOF::DeviceType SerializationOrientationTracker3DOF::deviceTypeSerializationOrientationTracker3DOF()
{
	return deviceTypeOrientationTracker3DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZATION_ORIENTATION_TRACKER_3DOF_H
