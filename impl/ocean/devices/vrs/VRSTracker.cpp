// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSTracker::VRSTracker(const std::string& name, const DeviceType& type) :
    Device(name, type),
    Measurement(name, type),
    Tracker(name, type),
    library_(nameVRSLibrary()),
    objectMapper_(*this)
{
	// nothing to do here
}

VRSTracker::~VRSTracker()
{
	// nothing to do here
}

bool VRSTracker::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isStarted_;
}

bool VRSTracker::start()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = true;
	return true;
}

bool VRSTracker::stop()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = false;
	return true;
}

const std::string& VRSTracker::library() const
{
	return library_;
}

bool VRSTracker::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return currentlyFoundExternalObjectIds_.find(objectId) != currentlyFoundExternalObjectIds_.cend();
}

void VRSTracker::updateVRSObjects(const VRSObjectDescriptionMap& vrsObjectDescriptionMap)
{
	const ScopedLock scopedLock(deviceLock);

	for (VRSObjectDescriptionMap::const_iterator i = vrsObjectDescriptionMap.cbegin(); i != vrsObjectDescriptionMap.cend(); ++i)
	{
		const std::string& description = i->first;
		const unsigned int internalObjectId = i->second;

		if (!objectMapper_.hasInternalObject(internalObjectId))
		{
			objectMapper_.newInternalObjectId(internalObjectId, description);
		}
	}
}

void VRSTracker::forwardObjectEvent(const ObjectIdSet& internalFoundObjects, const ObjectIdSet& internalLostObjects, const Timestamp& timestamp)
{
	TemporaryScopedLock scopedLock(deviceLock);

	ObjectIdSet foundExternalObjectIds;
	ObjectIdSet lostExternalObjectIds;

	if (!internalFoundObjects.empty())
	{
		foundExternalObjectIds.reserve(internalFoundObjects.size());

		for (const ObjectId& internalObjectId : internalFoundObjects)
		{
			const ObjectId externalObjectId = objectMapper_.externalObjectIdFromInternalObjectId(internalObjectId);
			foundExternalObjectIds.emplace(externalObjectId);
		}

		currentlyFoundExternalObjectIds_.insert(foundExternalObjectIds.cbegin(), foundExternalObjectIds.cend());
	}

	if (!internalLostObjects.empty())
	{
		lostExternalObjectIds.reserve(internalLostObjects.size());

		for (const ObjectId& internalObjectId : internalLostObjects)
		{
			const ObjectId externalObjectId = objectMapper_.externalObjectIdFromInternalObjectId(internalObjectId);

			currentlyFoundExternalObjectIds_.erase(externalObjectId);

			lostExternalObjectIds.emplace(externalObjectId);
		}
	}

	scopedLock.release();

	postFoundTrackerObjects(foundExternalObjectIds, timestamp);
	postLostTrackerObjects(lostExternalObjectIds, timestamp);
}

}

}

}
