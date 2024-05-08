/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_PLANE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARCORE_AC_PLANE_TRACKER_6_DOF_H

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
 * This class implements the 6DOF plane tracker.
 * @ingroup devicescore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACPlaneTracker6DOF final :
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
		static inline std::string deviceNameACPlaneTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeACPlaneTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF scene tracker.
		 */
		ACPlaneTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~ACPlaneTracker6DOF() override;

		/**
		 * Event function for new 6DOF transformations.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param planes The detected planes
		 * @param timestamp The timestamp of the new transformation
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, SceneTracker6DOF::SceneElementPlanes::Planes&& planes, const Timestamp& timestamp);
};

inline std::string ACPlaneTracker6DOF::deviceNameACPlaneTracker6DOF()
{
	return std::string("ARCore 6DOF Plane Tracker");
}

inline ACPlaneTracker6DOF::DeviceType ACPlaneTracker6DOF::deviceTypeACPlaneTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_PLANE_TRACKER_6_DOF_H
