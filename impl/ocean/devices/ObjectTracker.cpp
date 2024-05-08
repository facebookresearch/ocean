/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ObjectTracker.h"

namespace Ocean
{

namespace Devices
{

ObjectTracker::ObjectTracker(const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type)
{
	// nothing to do here
}

ObjectTracker::~ObjectTracker()
{
	// nothing to do here
}

ObjectTracker::ObjectId ObjectTracker::registerObject(const std::string& /*description*/, const Vector3& /*dimension*/)
{
	return invalidObjectId();
}

bool ObjectTracker::unregisterObject(const ObjectId /*objectId*/)
{
	return false;
}

}

}
