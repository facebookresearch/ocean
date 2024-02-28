// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSSceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSSceneTracker6DOF::VRSSceneTracker6DOF(const std::string& name, const DeviceType& type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	SceneTracker6DOF(name),
	VRSTracker(name, type),
	VRSTracker6DOF(name, type)
{
	// nothing to do here
}

VRSSceneTracker6DOF::~VRSSceneTracker6DOF()
{
	// nothing to do here
}

void VRSSceneTracker6DOF::forwardSampleEvent(const ObjectIds& internalObjectIds, Quaternions&& orientations, Vectors3&& positions, VRSSceneTracker6DOF::SharedSceneElements&& sceneElements, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(internalObjectIds.size() == orientations.size());
	ocean_assert(internalObjectIds.size() == positions.size());
	ocean_assert(internalObjectIds.size() == sceneElements.size());

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

		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, referenceSystem, std::move(externalObjectIds), std::move(orientations), std::move(positions), std::move(sceneElements), std::move(metadata))));
	}
}

}

}

}
