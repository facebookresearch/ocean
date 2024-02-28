// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_SCENE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_SCENE_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSVisualSceneTracker6DOF.h"

#include "ocean/devices/ObjectTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSVisualObjectSceneTracker6DOF;

/**
 * Definition of a smart object reference for a VRS 6DOF visual object scene tracker.
 * @see VRSVisualObjectSceneTracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSVisualObjectSceneTracker6DOF> VRSVisualObjectSceneTracker6DOFRef;

/**
 * This class implements a VRS 6-DOF visual object scene tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSVisualObjectSceneTracker6DOF :
	virtual public VRSVisualSceneTracker6DOF,
	virtual public ObjectTracker
{
	friend class VRSFactory;

	public:

		/**
		 * Registers (adds) a new object to be tracked.
		 * @see ObjectTracker::regiserObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Unregisters (removes) a previously registered object.
		 * @see ObjectTracker::unregisterObject().
		 */
		bool unregisterObject(const ObjectId objectId) override;

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSVisualObjectSceneTracker6DOF();

	protected:

		/**
		 * Creates a new VRS 6-DOF visual object scene tracker object.
         * @param name The name of the tracker
		 */
		explicit VRSVisualObjectSceneTracker6DOF(const std::string& name);

		/**
		 * Destructs a new VRS 6-DOF visual object scene tracker object.
		 */
		~VRSVisualObjectSceneTracker6DOF() override;
};

inline VRSVisualObjectSceneTracker6DOF::DeviceType VRSVisualObjectSceneTracker6DOF::deviceTypeVRSVisualObjectSceneTracker6DOF()
{
	return DeviceType(deviceTypeVRSVisualSceneTracker6DOF(), TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_SCENE_TRACKER_6_DOF_H
