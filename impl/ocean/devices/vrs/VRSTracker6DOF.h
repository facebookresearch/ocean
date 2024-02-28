// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSTracker.h"

#include "ocean/devices/Tracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSTracker6DOF;

/**
 * Definition of a smart object reference for a VRS 6DOF tracker.
 * @see VRSTracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSTracker6DOF> VRSTracker6DOFRef;

/**
 * This class implements a VRS 6-DOF tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSTracker6DOF :
	virtual public Tracker6DOF,
	virtual public VRSTracker
{
	friend class VRSFactory;

	public:

		/**
		 * Forwards sample events.
		 * @param internalObjectIds The ids of all (internal) objects for which a sample is known
		 * @param orientations The orientations of the sample, one for each object id
		 * @param positions The positions of the sample, one for each object id
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(const ObjectIds& internalObjectIds, Quaternions&& orientations, Vectors3&& positions, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSTracker6DOF();

	protected:

		/**
		 * Creates a new VRS 6-DOF tracker object.
         * @param name The name of the tracker
		 * @param type The type of the new tracker
		 */
		explicit VRSTracker6DOF(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new VRS 6-DOF tracker object.
		 */
		~VRSTracker6DOF() override;
};

inline VRSTracker6DOF::DeviceType VRSTracker6DOF::deviceTypeVRSTracker6DOF()
{
	return deviceTypeTracker6DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_TRACKER_6_DOF_H
