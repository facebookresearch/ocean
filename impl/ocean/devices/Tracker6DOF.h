/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_TRACKER_6DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/PositionTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class Tracker6DOF;

/**
 * Definition of a smart object reference for a 6DOF tracker.
 * @see Tracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<Tracker6DOF> Tracker6DOFRef;

/**
 * This class implements the base for all 6DOF trackers.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Tracker6DOF :
	virtual public OrientationTracker3DOF,
	virtual public PositionTracker3DOF
{
	public:

		/**
		 * Definition of a sample holding one single 6DOF tracker measurement.
		 */
		class OCEAN_DEVICES_EXPORT Tracker6DOFSample :
			virtual public OrientationTracker3DOF::OrientationTracker3DOFSample,
			virtual public PositionTracker3DOF::PositionTracker3DOFSample
		{
			public:

				/**
				 * Creates a new 6DOF tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation and position measurement
				 * @param orientations Sample orientation measurements
				 * @param positions Sample position measurements in meter
				 * @param metadata Optional metadata of the new sample
				 */
				Tracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Positions& positions, const Metadata& metadata = Metadata());

				/**
				 * Creates a new 6DOF tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation and position measurement
				 * @param orientations Sample orientation measurements
				 * @param positions Sample position measurements in meter
				 * @param metadata Optional metadata of the new sample
				 */
				Tracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Positions&& positions, Metadata&& metadata = Metadata());
		};

		/**
		 * Definition of a smart object reference for 6DOF tracker samples.
		 */
		typedef SmartObjectRef<Tracker6DOFSample, Sample> Tracker6DOFSampleRef;

	public:

		/**
		 * Definition of this device type.
		 */
		static inline DeviceType deviceTypeTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF tracker object.
		 * @param name The name of the 6DOF tracker, must be valid
		 */
		explicit Tracker6DOF(const std::string& name);

		/**
		 * Destructs a 6DOF tracker object.
		 */
		~Tracker6DOF() override;
};

inline Tracker6DOF::DeviceType Tracker6DOF::deviceTypeTracker6DOF()
{
	return Device::DeviceType(DEVICE_TRACKER, TRACKER_6DOF);
}

}

}

#endif // META_OCEAN_DEVICES_TRACKER_6DOF_H
