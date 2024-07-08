/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ORIENTATION_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_ORIENTATION_TRACKER_3DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Tracker.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class OrientationTracker3DOF;

/**
 * Definition of a 3DOF orientation tracker reference object.
 * @see OrientationTracker3DOF.
 * @ingroup devices
 */
typedef SmartObjectRef<OrientationTracker3DOF, Device> OrientationTracker3DOFRef;

/**
 * This class is the base class for all 3DOF orientation tracker objects.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT OrientationTracker3DOF : virtual public Tracker
{
	public:

		/**
		 * Definition of a sample holding 3DOF orientation measurements.
		 */
		class OCEAN_DEVICES_EXPORT OrientationTracker3DOFSample : virtual public TrackerSample
		{
			public:

				/**
				 * Definition of a vector holding orientation values.
				 */
				typedef std::vector<Quaternion> Orientations;

			public:

				/**
				 * Creates a new 3DOF orientation sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation measurement
				 * @param orientations Sample orientation measurements
				 * @param metadata Optional metadata of the new sample
				 */
				OrientationTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Metadata& metadata = Metadata());

				/**
				 * Creates a new 3DOF orientation sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation measurement
				 * @param orientations Sample orientation measurements
				 * @param metadata Optional metadata of the new sample
				 */
				OrientationTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Metadata&& metadata = Metadata());

				/**
				 * Returns the 3DOF orientation measurement values.
				 * Each 3DOF measurement corresponds to a different object id.
				 * @return Orientation measurement values as quaternion
				 */
				inline const Orientations& orientations() const;

			protected:

				/// Orientation measurement values stored as unit quaternion.
				Orientations orientations_;
		};

		/**
		 * Definition of a smart object reference for 3DOF orientation tracker samples.
		 */
		typedef SmartObjectRef<OrientationTracker3DOFSample, Sample> OrientationTracker3DOFSampleRef;

	public:

		/**
		 * Return the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeOrientationTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF orientation tracker object.
		 * @param name The name of the orientation tracker, must be valid
		 */
		explicit OrientationTracker3DOF(const std::string& name);

		/**
		 * Destructs a 3DOF orientation tracker.
		 */
		~OrientationTracker3DOF() override;
};

inline OrientationTracker3DOF::DeviceType OrientationTracker3DOF::deviceTypeOrientationTracker3DOF()
{
	return DeviceType(DEVICE_TRACKER, TRACKER_ORIENTATION_3DOF);
}

inline const OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations& OrientationTracker3DOF::OrientationTracker3DOFSample::orientations() const
{
	return orientations_;
}

}

}

#endif // META_OCEAN_DEVICES_ORIENTATION_TRACKER_3DOF_H
