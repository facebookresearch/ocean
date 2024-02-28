// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/QuestGPSTracker.h"

#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

QuestGPSTracker::QuestGPSTracker() :
	Device(deviceNameQuestGPSTracker(), deviceTypeQuestGPSTracker()),
	Measurement(deviceNameQuestGPSTracker(), deviceTypeQuestGPSTracker()),
	Tracker(deviceNameQuestGPSTracker(), deviceTypeQuestGPSTracker()),
	GPSTracker(deviceNameQuestGPSTracker())
{
	gpsObjectId_ = addUniqueObjectId(deviceNameQuestGPSTracker());
}

QuestGPSTracker::~QuestGPSTracker()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool QuestGPSTracker::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (!locationManager_)
	{
		locationManager_ = OSSDK::Location::v2::createLocationManager();
		ocean_assert(locationManager_);

		std::string packageName;
		if (!Platform::Android::Utilities::determinePackageName(packageName) || !locationManager_->getLocation(packageName, *this))
		{
			Log::error() << "Failed to initialize location callbacks";

			locationManager_ = nullptr;
			return false;
		}
	}

	return true;
}

bool QuestGPSTracker::pause()
{
	return stop();
}

bool QuestGPSTracker::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (locationManager_)
	{
		locationManager_ = nullptr;
	}

	return true;
}

const std::string& QuestGPSTracker::library() const
{
	static const std::string staticLibraryName(nameQuestLibrary());
	return staticLibraryName;
}

void QuestGPSTracker::onLocation(OSSDK::Location::v2::Location& location)
{
	const Timestamp timestamp(true);

	if (lastTimestamp_ == timestamp)
	{
		return;
	}

	if (lastTimestamp_.isInvalid())
	{
		postFoundTrackerObjects({gpsObjectId_}, timestamp);
	}

	constexpr float direction = -1.0f;
	constexpr float speed = -1.0f;

	Locations locations(1, Location(location.latitude, location.longitude, float(location.altitudeMeters), direction, speed, location.horizontalAccuracyMeters, location.verticalAccuracyMeters));

	ObjectIds objectIds(1, gpsObjectId_);

	postNewSample(SampleRef(new GPSTrackerSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(locations))));

	lastTimestamp_ = timestamp;
}

}

}

}
