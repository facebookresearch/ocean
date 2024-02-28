// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSVisualObjectSceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSVisualObjectSceneTracker6DOF::VRSVisualObjectSceneTracker6DOF(const std::string& name) :
	Device(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	Measurement(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	Tracker(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	SceneTracker6DOF(name),
	VRSTracker(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	VRSTracker6DOF(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	VRSSceneTracker6DOF(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	VisualTracker(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	VRSVisualSceneTracker6DOF(name, deviceTypeVRSVisualObjectSceneTracker6DOF()),
	ObjectTracker(name, deviceTypeVRSVisualObjectSceneTracker6DOF())
{
	// nothing to do here
}

VRSVisualObjectSceneTracker6DOF::~VRSVisualObjectSceneTracker6DOF()
{
	// nothing to do here
}

VRSVisualObjectSceneTracker6DOF::ObjectId VRSVisualObjectSceneTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	const Strings vrsObjectDescriptions = objectDescriptions();

	// first we check whether we have a perfect hit

	for (std::string vrsObjectDescription : vrsObjectDescriptions)
	{
		if (vrsObjectDescription == description)
		{
			const ObjectId externalObjectId = objectId(vrsObjectDescription);
			ocean_assert(externalObjectId != invalidObjectId());

			return externalObjectId;
		}
	}

	// now we check whether we have a file (matching the end of the path only)

	for (std::string vrsObjectDescription : vrsObjectDescriptions)
	{
		if (description.size() < vrsObjectDescription.size() && vrsObjectDescription.find(description) == vrsObjectDescription.size() - description.size())
		{
			const ObjectId externalObjectId = objectId(vrsObjectDescription);
			ocean_assert(externalObjectId != invalidObjectId());

			return externalObjectId;
		}
	}

	Log::warning() << "The VRS tracker does not have an object " << description;

	return invalidObjectId();
}

bool VRSVisualObjectSceneTracker6DOF::unregisterObject(const ObjectId objectId)
{
	return false;
}

}

}

}
