/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidGravityTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidGravityTracker3DOF::AndroidGravityTracker3DOF(const ASensor* sensor) :
	Device(deviceNameAndroidGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	AndroidDevice(deviceNameAndroidGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	Measurement(deviceNameAndroidGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	AndroidEventDevice(deviceNameAndroidGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	Tracker(deviceNameAndroidGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	GravityTracker3DOF(deviceNameAndroidGravityTracker3DOF())
{
	ocean_assert(sensor != nullptr);

	ocean_assert(sensor_ == nullptr);
	sensor_ = sensor;

	ocean_assert(sensorManager_ != nullptr);

	if (!registerForEventFunction(sensorManager_))
	{
		deviceIsValid = false;
		return;
	}
}

AndroidGravityTracker3DOF::~AndroidGravityTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();

	ocean_assert(sensorManager_ && eventQueue_);
	ASensorManager_destroyEventQueue(sensorManager_, eventQueue_);
}

int AndroidGravityTracker3DOF::onEventFunction()
{
	TemporaryScopedLock scopedLock(deviceLock);

	/**
	 * Android Device Coordinate System:
	 *
	 *                    y (up)
	 *                    |
	 *                    |
	 *       +------------+------------+
	 *       |            |            |
	 *       |            |            |
	 *       |            |            |
	 *       |            |            |
	 *       |         Screen          |
	 *       |       (Portrait)        |
	 *       |            |            |
	 *       |            +------------------ x (right)
	 *       |           /             |
	 *       |          /              |
	 *       |         z (out)         |
	 *       |                         |
	 *       |                         |
	 *       +-------------------------+
	 *
	 * X-axis: horizontal, points to the right
	 * Y-axis: vertical, points up
	 * Z-axis: points towards the outside of the front face of the screen
	 *
	 * Android's gravity vector is pointing away from the Earths center, thus we need to negate the vector.
	 */

	ASensorEvent sensorEvent;

	while (ASensorEventQueue_getEvents(eventQueue_, &sensorEvent, 1) > 0)
	{
		scopedLock.release();

		ocean_assert(sensorEvent.type == AST_GRAVITY);

		Timestamp relativeTimestamp;
		const Timestamp unixTimestamp = convertTimestamp(sensorEvent, relativeTimestamp);

		// We need to negate the gravity direction as Android provides gravity vector ~(0, 0, +9.8) if device is placed on a table with screen upwards.
		Vector3 gravity(-sensorEvent.vector.x, -sensorEvent.vector.y, -sensorEvent.vector.z);

		if (gravity.normalize())
		{
			if (waitingForFirstSample_)
			{
				postFoundTrackerObjects({sensorObjectId_}, unixTimestamp);
				waitingForFirstSample_ = false;
			}

			ObjectIds objectIds(1, sensorObjectId_);
			GravityTracker3DOFSample::Gravities gravities(1, gravity);

			SampleRef sample(new GravityTracker3DOFSample(unixTimestamp, RS_OBJECT_IN_DEVICE, std::move(objectIds), std::move(gravities)));
			sample->setRelativeTimestamp(relativeTimestamp);

			postNewSample(sample);
		}

		scopedLock.relock(deviceLock);
	}

	return 1;
}

}

}

}
