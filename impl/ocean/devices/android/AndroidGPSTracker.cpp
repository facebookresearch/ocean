/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidGPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidGPSTracker::AndroidGPSTracker() :
	Device(deviceNameAndroidGPSTracker(), deviceTypeAndroidGPSTracker()),
	Measurement(deviceNameAndroidGPSTracker(), deviceTypeAndroidGPSTracker()),
	Tracker(deviceNameAndroidGPSTracker(), deviceTypeAndroidGPSTracker()),
	GPSTracker(deviceNameAndroidGPSTracker()),
	AndroidDevice(deviceNameAndroidGPSTracker(), deviceTypeAndroidGPSTracker())
{
	gpsObjectId_ = addUniqueObjectId(deviceNameAndroidGPSTracker());
}

AndroidGPSTracker::~AndroidGPSTracker()
{
	if (lastTimestamp_.isValid())
	{
		postLostTrackerObjects({gpsObjectId_}, Timestamp(true));
	}
}

void AndroidGPSTracker::newGPSLocation(const Location& location, const Timestamp& timestamp)
{
	if (lastTimestamp_ == timestamp)
	{
		return;
	}

	if (lastTimestamp_.isInvalid())
	{
		postFoundTrackerObjects({gpsObjectId_}, timestamp);
	}

	Locations locations(1, location);

	ObjectIds objectIds(1, gpsObjectId_);

	postNewSample(SampleRef(new GPSTrackerSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(locations))));

	lastTimestamp_ = timestamp;
}

}

}

}
