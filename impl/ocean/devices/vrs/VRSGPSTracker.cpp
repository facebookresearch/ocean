// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSGPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSGPSTracker::VRSGPSTracker(const std::string& name) :
    Device(name, deviceTypeVRSGPSTracker()),
    Measurement(name, deviceTypeVRSGPSTracker()),
    Tracker(name, deviceTypeVRSGPSTracker()),
    GPSTracker(name),
	VRSTracker(name, deviceTypeVRSGPSTracker())
{
	// nothing to do here
}

VRSGPSTracker::~VRSGPSTracker()
{
	// nothing to do here
}

void VRSGPSTracker::forwardSampleEvent(const ObjectIds& internalObjectIds, Locations&& locations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(internalObjectIds.size() == locations.size());

	TemporaryScopedLock scopedLock(deviceLock);

	if (!internalObjectIds.empty())
	{
		ObjectIdSet newFoundExternalObjectIds;

		ObjectIds externalObjectIds;
		externalObjectIds.reserve(internalObjectIds.size());

		for (const ObjectId& internalObjectId : internalObjectIds)
		{
			const ObjectId externalObjectId = objectMapper_.externalObjectIdFromInternalObjectId(internalObjectId);
			externalObjectIds.emplace_back(externalObjectId);

			if (currentlyFoundExternalObjectIds_.emplace(externalObjectId).second)
			{
				newFoundExternalObjectIds.emplace(externalObjectId);
			}
		}

		scopedLock.release();

		// as the VRS recording may have started after an object has been found, we need to create a found event manually for those objects
		postFoundTrackerObjects(newFoundExternalObjectIds, timestamp);

		postNewSample(SampleRef(new GPSTrackerSample(timestamp, referenceSystem, std::move(externalObjectIds), std::move(locations), std::move(metadata))));
	}

}

}

}

}
