// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_GPS_TRACKER_H
#define META_OCEAN_DEVICES_VRS_VRS_GPS_TRACKER_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSTracker.h"

#include "ocean/devices/GPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSGPSTracker;

/**
 * Definition of a smart object reference for a VRS GPS tracker.
 * @see VRSGPSTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSGPSTracker> VRSGPSTrackerRef;

/**
 * This class implements a VRS GPS tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSGPSTracker :
	virtual public GPSTracker,
	virtual public VRSTracker
{
	friend class VRSFactory;

	public:

		/**
		 * Forwards sample events.
		 * @param internalObjectIds The ids of all (internal) objects for which a sample is known
		 * @param locations The GPS locations to forward, one for each object id
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(const ObjectIds& internalObjectIds, Locations&& locations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSGPSTracker();

	protected:

		/**
		 * Creates a new VRS GPS tracker object.
		 * @param name The name of the tracker
		 */
		explicit VRSGPSTracker(const std::string& name);

		/**
		 * Destructs a new VRS GPS tracker object.
		 */
		~VRSGPSTracker() override;
};

inline VRSGPSTracker::DeviceType VRSGPSTracker::deviceTypeVRSGPSTracker()
{
	return DeviceType(GPSTracker::deviceTypeGPSTracker());
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_GPS_TRACKER_H
