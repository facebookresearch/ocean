/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSGravityTracker3DOF.h"

#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSGravityTracker3DOF::IOSGravityTracker3DOF() :
	Device(deviceNameIOSGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	IOSDevice(deviceNameIOSGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	Measurement(deviceNameIOSGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	Tracker(deviceNameIOSGravityTracker3DOF(), deviceTypeGravityTracker3DOF()),
	GravityTracker3DOF(deviceNameIOSGravityTracker3DOF())
{
	ocean_assert(deviceType.minorType() == Tracker::TRACKER_GRAVITY_3DOF);

	trackerObjectId_ = addUniqueObjectId(deviceNameIOSGravityTracker3DOF());
}

IOSGravityTracker3DOF::~IOSGravityTracker3DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSGravityTracker3DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_GRAVITY_3DOF);

	deviceMotionListenerId_ = MotionManager::get().addListener(MotionManager::DeviceMotionCallback(*this, &IOSGravityTracker3DOF::onDeviceMotion));

	isStarted_ = true;

	return true;
}

bool IOSGravityTracker3DOF::pause()
{
	return stop();
}

bool IOSGravityTracker3DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	ocean_assert(deviceType.minorType() == Tracker::TRACKER_GRAVITY_3DOF);

	MotionManager::get().removeListener(deviceMotionListenerId_);

	isStarted_ = false;

	return true;
}

void IOSGravityTracker3DOF::onDeviceMotion(CMDeviceMotion* deviceMotion)
{
	ocean_assert(deviceMotion != nullptr);

	Timestamp relativeTimestamp;
	const Timestamp unixTimestamp = convertTimestamp(deviceMotion.timestamp, relativeTimestamp);

	ObjectIds objectIds(1, trackerObjectId_);

	VectorD3 gravity(deviceMotion.gravity.x, deviceMotion.gravity.y, deviceMotion.gravity.z);

	if (gravity.normalize())
	{
		if (waitingForFirstSample_)
		{
			postFoundTrackerObjects({trackerObjectId_}, unixTimestamp);
			waitingForFirstSample_ = false;
		}

		GravityTracker3DOFSample::Gravities gravities(1, Vector3(gravity));

		const SampleRef sample(new GravityTracker3DOFSample(unixTimestamp, RS_OBJECT_IN_DEVICE, std::move(objectIds), std::move(gravities)));
		sample->setRelativeTimestamp(relativeTimestamp);

		postNewSample(sample);
	}
}

}

}

}
