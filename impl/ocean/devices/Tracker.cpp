/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Tracker.h"

namespace Ocean
{

namespace Devices
{

Tracker::TrackerSample::TrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	referenceSystem_(referenceSystem)
{
	// nothing to do here
}

Tracker::TrackerSample::TrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	referenceSystem_(referenceSystem)
{
	// nothing to do here
}

Tracker::TrackerObjectEventSubscription::TrackerObjectEventSubscription(const Tracker& tracker, const SubscriptionId subscriptionId) :
	subscriptionId_(subscriptionId)
{
	tracker_ = DeviceRefManager::get().device(tracker.name());
	ocean_assert(tracker_);

	weakTracker_ = &*tracker_;
	ocean_assert(weakTracker_ == &tracker);
}

void Tracker::TrackerObjectEventSubscription::release()
{
	if (weakTracker_)
	{
		weakTracker_->unsubscribeTrackerObjectEvent(subscriptionId_);
		subscriptionId_ = invalidSubscriptionId();

		weakTracker_ = nullptr;
	}

	tracker_.release();
}

Tracker::TrackerObjectEventSubscription& Tracker::TrackerObjectEventSubscription::operator=(TrackerObjectEventSubscription&& trackerObjectEventSubscription)
{
	if (this != &trackerObjectEventSubscription)
	{
		release();

		tracker_ = std::move(trackerObjectEventSubscription.tracker_);
		weakTracker_ = trackerObjectEventSubscription.weakTracker_;
		trackerObjectEventSubscription.weakTracker_ = nullptr;

		subscriptionId_ = trackerObjectEventSubscription.subscriptionId_;
		trackerObjectEventSubscription.subscriptionId_ = invalidSubscriptionId();
	}

	return *this;
}

Tracker::Tracker(const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type)
{
	// nothing to do here
}

Tracker::~Tracker()
{
	ocean_assert(trackerObjectSubscriptionMap_.empty());
}

Tracker::Frequency Tracker::frequency() const
{
	return unknownFrequency();
}

bool Tracker::isObjectTracked(const ObjectId& /*objectId*/) const
{
	return false;
}

Tracker::TrackerObjectEventSubscription Tracker::subscribeTrackerObjectEvent(TrackerObjectCallback&& callback)
{
	if (callback.isNull())
	{
		return TrackerObjectEventSubscription();
	}

	const ScopedLock scopedLock(subscriptionLock_);

	const SubscriptionId subscriptionId = nextTrackerObjectSubscriptionId_++;

	ocean_assert(subscriptionId != invalidSubscriptionId());
	ocean_assert(trackerObjectSubscriptionMap_.find(subscriptionId) == trackerObjectSubscriptionMap_.end());
	trackerObjectSubscriptionMap_[subscriptionId] = std::move(callback);

	return TrackerObjectEventSubscription(*this, subscriptionId);
}

HomogenousMatrixD4 Tracker::reference() const
{
	return HomogenousMatrixD4(false);
}

std::string Tracker::translateTrackerType(const TrackerType trackerType)
{
	std::string result;

	if (trackerType & SCENE_TRACKER_6DOF)
	{
		result += "SCENE_TRACKER_6DOF ";
	}
	else if (trackerType & TRACKER_6DOF)
	{
		result += "TRACKER_6DOF ";
	}
	else if (trackerType & TRACKER_ORIENTATION_3DOF)
	{
		result += "TRACKER_ORIENTATION_3DOF ";
	}
	else if (trackerType & TRACKER_POSITION_3DOF)
	{
		result += "TRACKER_POSITION_3DOF ";
	}

	if (trackerType & TRACKER_GPS)
	{
		result += "TRACKER_GPS ";
	}

	if (trackerType & TRACKER_MAGNETIC)
	{
		result += "TRACKER_MAGNETIC ";
	}

	if (trackerType & TRACKER_VISUAL)
	{
		result += "TRACKER_VISUAL ";
	}

	if (trackerType & TRACKER_OBJECT)
	{
		result += "TRACKER_OBJECT ";
	}

	if (result.empty())
	{
		return "TRACKER_INVALID";
	}

	// popping the last empty space

	ocean_assert(result.back() == ' ');
	result.pop_back();

	return result;
}

Tracker::TrackerType Tracker::translateTrackerType(const std::string& trackerType)
{
	TrackerType result = TRACKER_INVALID;
	ocean_assert(result == 0u);

	if (trackerType.find("SCENE_TRACKER_6DOF") != std::string::npos)
	{
		result = TrackerType(result | SCENE_TRACKER_6DOF);
	}
	else if (trackerType.find("TRACKER_6DOF") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_6DOF);
	}
	else if (trackerType.find("TRACKER_ORIENTATION_3DOF") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_ORIENTATION_3DOF);
	}
	else if (trackerType.find("TRACKER_POSITION_3DOF") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_POSITION_3DOF);
	}

	if (trackerType.find("TRACKER_GPS") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_GPS);
	}

	if (trackerType.find("TRACKER_MAGNETIC") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_MAGNETIC);
	}

	if (trackerType.find("TRACKER_VISUAL") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_VISUAL);
	}

	if (trackerType.find("TRACKER_OBJECT") != std::string::npos)
	{
		result = TrackerType(result | TRACKER_OBJECT);
	}

	return result;
}

void Tracker::postFoundTrackerObjects(const ObjectIdSet& objectIds, const Timestamp& timestamp)
{
	if (objectIds.empty())
	{
		return;
	}

	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(subscriptionLock_);

	for (TrackerObjectSubscriptionMap::const_iterator i = trackerObjectSubscriptionMap_.cbegin(); i != trackerObjectSubscriptionMap_.cend(); ++i)
	{
		i->second(this, true, objectIds, timestamp);
	}
}

void Tracker::postLostTrackerObjects(const ObjectIdSet& objectIds, const Timestamp& timestamp)
{
	if (objectIds.empty())
	{
		return;
	}

	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(subscriptionLock_);

	for (TrackerObjectSubscriptionMap::const_iterator i = trackerObjectSubscriptionMap_.cbegin(); i != trackerObjectSubscriptionMap_.cend(); ++i)
	{
		i->second(this, false, objectIds, timestamp);
	}
}

void Tracker::unsubscribeTrackerObjectEvent(const SubscriptionId subscriptionId)
{
	if (subscriptionId != invalidSubscriptionId())
	{
		const ScopedLock scopedLock(subscriptionLock_);

		ocean_assert(trackerObjectSubscriptionMap_.find(subscriptionId) != trackerObjectSubscriptionMap_.end());
		trackerObjectSubscriptionMap_.erase(subscriptionId);
	}
}

Tracker::ObjectIdSet Tracker::determineFoundObjects(const ObjectIdSet& previousObjects, const ObjectIdSet& currentObjects)
{
	ObjectIdSet foundObjects;

	for (const ObjectId& currentObject : currentObjects)
	{
		if (previousObjects.find(currentObject) == previousObjects.cend())
		{
			foundObjects.emplace(currentObject);
		}
	}

	return foundObjects;
}

Tracker::ObjectIdSet Tracker::determineLostObjects(const ObjectIdSet& previousObjects, const ObjectIdSet& currentObjects)
{
	ObjectIdSet lostObjects;

	for (const ObjectId& previousObject : previousObjects)
	{
		if (currentObjects.find(previousObject) == currentObjects.cend())
		{
			lostObjects.emplace(previousObject);
		}
	}

	return lostObjects;
}

}

}
