/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_POSITION_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_POSITION_TRACKER_3DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Tracker.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class PositionTracker3DOF;

/**
 * Definition of a smart object reference for a 3DOF position tracker.
 * @see PositionTracker3DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<PositionTracker3DOF> PositionTracker3DOFRef;

/**
 * This class is the base class for all 3DOF position trackers.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT PositionTracker3DOF : virtual public Tracker
{
	public:

		/**
		 * Definition of a sample holding 3DOF position measurements.
		 */
		class OCEAN_DEVICES_EXPORT PositionTracker3DOFSample : virtual public Tracker::TrackerSample
		{
			public:

				/**
				 * Definition of a vector holding position values.
				 */
				typedef std::vector<Vector3> Positions;

			public:

				/**
				 * Creates a new 3DOF position sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different position measurement
				 * @param positions Sample position measurements in meter
				 * @param metadata Optional metadata of the new sample
				 */
				PositionTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Positions& positions, const Metadata& metadata = Metadata());

				/**
				 * Creates a new 3DOF position sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different position measurement
				 * @param positions Sample position measurements in meter
				 * @param metadata Optional metadata of the new sample
				 */
				PositionTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Positions&& positions, Metadata&& metadata = Metadata());

				/**
				 * Returns all 3DOF position measurement values.
				 * Each 3DOF measurement corresponds to a different object id.
				 * @return Position measurement value in meter
				 */
				inline const Positions& positions() const;

			protected:

				/// position measurement values stored in meter.
				Positions positions_;
		};

		/**
		 * Definition of a smart object reference for 3DOF position tracker samples.
		 */
		typedef SmartObjectRef<PositionTracker3DOFSample, Sample> PositionTracker3DOFSampleRef;

	public:

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypePositionTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF position tracker object.
		 * @param name The name of the position tracker, must be valid
		 */
		explicit PositionTracker3DOF(const std::string& name);

		/**
		 * Destructs a 3DOF position tracker object.
		 */
		~PositionTracker3DOF() override;
};

inline PositionTracker3DOF::DeviceType PositionTracker3DOF::deviceTypePositionTracker3DOF()
{
	return DeviceType(DEVICE_TRACKER, TRACKER_POSITION_3DOF);
}

inline const PositionTracker3DOF::PositionTracker3DOFSample::Positions& PositionTracker3DOF::PositionTracker3DOFSample::positions() const
{
	return positions_;
}

}

}

#endif // META_OCEAN_DEVICES_POSITION_TRACKER_3DOF_H
