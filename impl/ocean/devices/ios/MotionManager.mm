/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

 
#include "ocean/devices/ios/MotionManager.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{
	
CMMotionManager* MotionManager::object()
{
	const ScopedLock scopedLock(lock_);
	
	if (motionManager_ != nullptr)
	{
		return motionManager_;
	}
	
	motionManager_ = [[CMMotionManager alloc] init];
	
	ocean_assert(motionManager_);
	return motionManager_;
}
	
bool MotionManager::setObject(CMMotionManager* object)
{
	if (object == nullptr)
	{
		return false;
	}
	
	const ScopedLock scopedLock(lock_);

	if (motionManager_ != nullptr)
	{
		return false;
	}
	
	motionManager_ = object;
	return true;
}

MotionManager::ListenerId MotionManager::addListener(const DeviceMotionCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	++nextListenerId_;

	deviceMotionListenerMap_[nextListenerId_] = callback;

	// If we previously had no listeners, then we'll (re)start the DeviceMotion interface.
	if (deviceMotionListenerMap_.size() == 1u)
	{
		start();
	}

	return nextListenerId_;
}

void MotionManager::removeListener(const ListenerId listenerId)
{
	ocean_assert(listenerId != invalidListenerId());

	const ScopedLock scopedLock(lock_);

	const DeviceMotionListenerMap::const_iterator iterator = deviceMotionListenerMap_.find(listenerId);
	ocean_assert(iterator != deviceMotionListenerMap_.cend());
	if (iterator != deviceMotionListenerMap_.end())
	{
		deviceMotionListenerMap_.erase(iterator);
	}

	// Stop if no listeners remain.
	if (deviceMotionListenerMap_.empty())
	{
		CMMotionManager* motionManager = MotionManager::get().object();
		ocean_assert(motionManager);

		[motionManager stopDeviceMotionUpdates];
	}
}

void MotionManager::start()
{
	CMMotionManager* motionManager = MotionManager::get().object();

	ocean_assert(motionManager != nullptr);
	ocean_assert(motionManager.deviceMotionAvailable);

	CMAttitudeReferenceFrame cvAttitudeReferenceFrame = CMAttitudeReferenceFrameXArbitraryZVertical;

	if ([CMMotionManager availableAttitudeReferenceFrames] & CMAttitudeReferenceFrameXTrueNorthZVertical)
	{
		cvAttitudeReferenceFrame = CMAttitudeReferenceFrameXTrueNorthZVertical;
	}
	else if ([CMMotionManager availableAttitudeReferenceFrames] & CMAttitudeReferenceFrameXMagneticNorthZVertical)
	{
		cvAttitudeReferenceFrame = CMAttitudeReferenceFrameXMagneticNorthZVertical;
	}

	[motionManager startDeviceMotionUpdatesUsingReferenceFrame:cvAttitudeReferenceFrame toQueue:[[NSOperationQueue alloc] init] withHandler:^(CMDeviceMotion* deviceMotion, NSError* nsError)
		{
			if (deviceMotion != nullptr)
			{
				const ScopedLock scopedLock(lock_);

				for (DeviceMotionListenerMap::const_iterator i = deviceMotionListenerMap_.cbegin(); i != deviceMotionListenerMap_.cend(); ++i)
				{
					i->second(deviceMotion);
				}
			}
		}];
}
	
}

}

}
