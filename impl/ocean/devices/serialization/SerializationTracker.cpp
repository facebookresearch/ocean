/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

std::string nameSerializationLibrary();

SerializationTracker::SerializationTracker(const std::string& name, const DeviceType& type) :
    Device(name, type),
    Measurement(name, type),
    Tracker(name, type),
    library_(nameSerializationLibrary())
{
	// nothing to do here
}

SerializationTracker::~SerializationTracker()
{
	// nothing to do here
}

bool SerializationTracker::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isStarted_;
}

bool SerializationTracker::start()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = true;
	return true;
}

bool SerializationTracker::stop()
{
	const ScopedLock scopedLock(deviceLock);

	isStarted_ = false;
	return true;
}

const std::string& SerializationTracker::library() const
{
	return library_;
}

bool SerializationTracker::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return currentlyFoundExternalObjectIds_.find(objectId) != currentlyFoundExternalObjectIds_.cend();
}

}

}

}
