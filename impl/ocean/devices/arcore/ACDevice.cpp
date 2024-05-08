/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACDevice.h"
#include "ocean/devices/arcore/ARSessionManager.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/media/android/Android.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACDevice::ACDevice(const TrackerCapabilities trackerCapabilities, const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type),
	VisualTracker(name, type),
	trackerCapabilities_(trackerCapabilities)
{
	// nothing to do here
}

ACDevice::~ACDevice()
{
	if (hasBeenRegistered_)
	{
		const bool result = ARSessionManager::get().unregisterTracker(this);
		ocean_assert_and_suppress_unused(result, result);
	}
}

const std::string& ACDevice::library() const
{
	return nameARCoreLibrary();
}

bool ACDevice::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (!hasBeenRegistered_)
	{
		if (frameMediums_.size() == 1 && frameMediums_.front())
		{
			const bool result = ARSessionManager::get().registerTracker(this, frameMediums_.front());
			ocean_assert_and_suppress_unused(result, result);

			hasBeenRegistered_ = true;
		}
		else
		{
			Log::error() << "ARCore: Invalid number of input mediums, expected 1, got " << frameMediums_.size();
			return false;
		}
	}

	return ARSessionManager::get().start(this);
}

bool ACDevice::pause()
{
	const ScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	if (worldIsTracked_)
	{
		postLostTrackerObjects({worldObjectId_}, Timestamp(true));
	}

	worldIsTracked_ = false;

	return true;
}

bool ACDevice::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().stop(this))
	{
		return false;
	}

	if (worldIsTracked_)
	{
		postLostTrackerObjects({worldObjectId_}, Timestamp(true));
	}

	worldIsTracked_ = false;

	return true;
}

bool ACDevice::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return worldIsTracked_;
}

void ACDevice::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() == 1 && frameMediums.front())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();

		if (frameMedium->library() == Media::Android::nameAndroidLibrary() && (frameMedium->type() & Media::Medium::LIVE_VIDEO) == Media::Medium::LIVE_VIDEO)
		{
			if (frameMedium->url() == "LiveVideoId:0" || frameMedium->url() == "LiveVideoId:1")
			{
				VisualTracker::setInput(std::move(frameMediums));
				return;
			}
		}
	}

	ocean_assert(false && "Invalid frameMedium!");
}

}

}

}
