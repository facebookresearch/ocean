// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
