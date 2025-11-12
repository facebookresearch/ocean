/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_GRAVITY_TRACKER_3DOF_H
#define META_OCEAN_DEVICES_GRAVITY_TRACKER_3DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Tracker.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class GravityTracker3DOF;

/**
 * Definition of a smart object reference for a 3DOF gravity tracker.
 * @see GravityTracker3DOF.
 * @ingroup devices
 */
using GravityTracker3DOFRef = SmartDeviceRef<GravityTracker3DOF>;

/**
 * This class is the base class for all 3DOF gravity trackers.
 * A gravity tracker provides the gravity direction (normalized gravity vector) in the device's coordinate system.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT GravityTracker3DOF : virtual public Tracker
{
	public:

		/**
		 * Definition of a sample holding 3DOF gravity measurements.
		 */
		class OCEAN_DEVICES_EXPORT GravityTracker3DOFSample : virtual public Tracker::TrackerSample
		{
			public:

				/**
				 * Definition of a vector holding gravity values.
				 */
				using Gravities = std::vector<Vector3>;

			public:

				/**
				 * Creates a new 3DOF gravity sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different gravity measurement
				 * @param gravities Sample gravity measurements (normalized gravity vectors)
				 * @param metadata Optional metadata of the new sample
				 */
				GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Gravities& gravities, const Metadata& metadata = Metadata());

				/**
				 * Creates a new 3DOF gravity sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different gravity measurement
				 * @param gravities Sample gravity measurements (normalized gravity vectors)
				 * @param metadata Optional metadata of the new sample
				 */
				GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Gravities&& gravities, Metadata&& metadata = Metadata());

				/**
				 * Returns all 3DOF gravity measurement values.
				 * Each 3DOF measurement corresponds to a different object id.
				 * @return Gravity measurement values (normalized gravity vectors)
				 */
				inline const Gravities& gravities() const;

			protected:

				/// Gravity measurement values stored as normalized vectors.
				Gravities gravities_;
		};

		/**
		 * Definition of a smart object reference for 3DOF gravity tracker samples.
		 */
		using GravityTracker3DOFSampleRef = SmartObjectRef<GravityTracker3DOFSample, Sample>;

	public:

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeGravityTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF gravity tracker object.
		 * @param name The name of the gravity tracker, must be valid
		 */
		explicit GravityTracker3DOF(const std::string& name);

		/**
		 * Destructs a 3DOF gravity tracker object.
		 */
		~GravityTracker3DOF() override;

		/**
		 * Interpolates between two 3DOF gravity tracker samples.
		 * @see Measurement::interpolateSamples().
		 */
		SampleRef interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double interpolationFactor, const Timestamp& interpolatedTimestamp) const override;
};

inline GravityTracker3DOF::DeviceType GravityTracker3DOF::deviceTypeGravityTracker3DOF()
{
	return DeviceType(DEVICE_TRACKER, TRACKER_GRAVITY_3DOF);
}

inline const GravityTracker3DOF::GravityTracker3DOFSample::Gravities& GravityTracker3DOF::GravityTracker3DOFSample::gravities() const
{
	return gravities_;
}

}

}

#endif // META_OCEAN_DEVICES_GRAVITY_TRACKER_3DOF_H
