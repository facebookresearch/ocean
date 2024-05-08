/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_WORLD_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARCORE_AC_WORLD_TRACKER_6_DOF_H

#include "ocean/devices/arcore/ARCore.h"
#include "ocean/devices/arcore/ACDevice.h"

#include "ocean/devices/Tracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/*
 * This class implements the 6DOF world tracker.
 * @ingroup devicescore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACWorldTracker6DOF final :
	virtual public ACDevice,
	virtual public Tracker6DOF
{
	friend class ARSessionManager;
	friend class ACFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameACWorldTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeACWorldTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF world tracker.
		 */
		ACWorldTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~ACWorldTracker6DOF() override;

		/**
		 * Event function for new 6DOF transformations.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param timestamp The timestamp of the new transformation
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp);
};

inline std::string ACWorldTracker6DOF::deviceNameACWorldTracker6DOF()
{
	return std::string("ARCore 6DOF World Tracker");
}

inline ACWorldTracker6DOF::DeviceType ACWorldTracker6DOF::deviceTypeACWorldTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_WORLD_TRACKER_6_DOF_H
