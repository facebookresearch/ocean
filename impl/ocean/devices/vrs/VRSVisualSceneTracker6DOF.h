// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_VISUAL_SCENE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_VISUAL_SCENE_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSSceneTracker6DOF.h"

#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSVisualSceneTracker6DOF;

/**
 * Definition of a smart reference for a VRS 6DOF visual scene tracker.
 * @see VRSVisualSceneTracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSVisualSceneTracker6DOF> VRSVisualSceneTracker6DOFRef;

/**
 * This class implements a VRS 6-DOF visual scene tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSVisualSceneTracker6DOF :
	virtual public VRSSceneTracker6DOF,
	virtual public VisualTracker
{
	friend class VRSFactory;

	public:

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSVisualSceneTracker6DOF();

	protected:

		/**
		 * Creates a new VRS 6-DOF visual scene tracker object.
         * @param name The name of the tracker
		 * @param type The type of the new tracker
		 */
		explicit VRSVisualSceneTracker6DOF(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new VRS 6-DOF visual scene tracker object.
		 */
		~VRSVisualSceneTracker6DOF() override;
};

inline VRSVisualSceneTracker6DOF::DeviceType VRSVisualSceneTracker6DOF::deviceTypeVRSVisualSceneTracker6DOF()
{
	return DeviceType(deviceTypeVRSSceneTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_VISUAL_SCENE_TRACKER_6_DOF_H
