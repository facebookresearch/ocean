/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_DEPTH_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARCORE_AC_DEPTH_TRACKER_6_DOF_H

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
 * This class implements the 6DOF depth tracker.
 * @ingroup devicescore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACDepthTracker6DOF final :
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
		static inline std::string deviceNameACDepthTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeACDepthTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF scene tracker.
		 */
		ACDepthTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~ACDepthTracker6DOF() override;

		/**
		 * Event function for new 6DOF transformations.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param depth The depth frame, must be valid
		 * @param depthCamera The depth camera defining the projection of the depth frame, must be valid
		 * @param device_T_depth The transformation between depth image and device, must be valid
		 * @param timestamp The timestamp of the new transformation
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, Frame&& depth, SharedAnyCamera&& depthCamera, const HomogenousMatrix4& device_T_depth, const Timestamp& timestamp);
};

inline std::string ACDepthTracker6DOF::deviceNameACDepthTracker6DOF()
{
	return std::string("ARCore 6DOF Depth Tracker");
}

inline ACDepthTracker6DOF::DeviceType ACDepthTracker6DOF::deviceTypeACDepthTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_DEPTH_TRACKER_6_DOF_H
