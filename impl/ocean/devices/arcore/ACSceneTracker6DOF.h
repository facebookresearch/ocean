/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_SCENE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARCORE_AC_SCENE_TRACKER_6_DOF_H

#include "ocean/devices/arcore/ARCore.h"
#include "ocean/devices/arcore/ACDevice.h"

#include "ocean/devices/SceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/*
 * This class implements the 6DOF scene tracker.
 * @ingroup devicescore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACSceneTracker6DOF final :
	virtual public ACDevice,
	virtual public SceneTracker6DOF
{
	friend class ARSessionManager;
	friend class ACFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameACSceneTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeACSceneTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF scene tracker.
		 */
		ACSceneTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~ACSceneTracker6DOF() override;

		/**
		 * Event function for new 6DOF transformations.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param objectPoints The 3D object points the tracker has used to determine the current pose, in world, may be empty
		 * @param objectPointIds The ids of the object points, one for each object point
		 * @param timestamp The timestamp of the new transformation
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, Vectors3&& objectPoints, Indices64&& objectPointIds, const Timestamp& timestamp);
};

inline std::string ACSceneTracker6DOF::deviceNameACSceneTracker6DOF()
{
	return std::string("ARCore 6DOF Scene Tracker");
}

inline ACSceneTracker6DOF::DeviceType ACSceneTracker6DOF::deviceTypeACSceneTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_SCENE_TRACKER_6_DOF_H
