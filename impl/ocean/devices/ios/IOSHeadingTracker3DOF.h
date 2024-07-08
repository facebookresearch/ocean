/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_HEADING_TRACKER_3_DOF_H
#define META_OCEAN_DEVICES_IOS_IOS_HEADING_TRACKER_3_DOF_H

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSDevice.h"
#include "ocean/devices/ios/MotionManager.h"

#include "ocean/devices/OrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a tracker providing the heading/north direction as 3-DOF orientation.
 * The resulting transformation (3DOF orientation) is defined w.r.t. the device's coordinate system and rotates the negative z-axis so that it points towards true north (the rotated positive z-axis points towards south).
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSHeadingTracker3DOF :
	virtual public IOSDevice,
	virtual public OrientationTracker3DOF
{
	friend class IOSFactory;

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns the name of this sensor.
		 * @return The sensor's name
		 */
		static inline std::string deviceNameIOSHeadingTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF gravity tracker device.
		 */
		IOSHeadingTracker3DOF();

		/**
		 * Destructs a 3DOF gravity sensor device.
		 */
		~IOSHeadingTracker3DOF() override;

	private:

		/**
		 * Callback function for obtaining new samples from the device.
		 * @param deviceMotion iOS DeviceMotion readout
		 */
		void onDeviceMotion(CMDeviceMotion* deviceMotion);

	private:

		// Callback id for this tracker.
		MotionManager::ListenerId deviceMotionListenerId_ = MotionManager::invalidListenerId();

		/// The object id of this tracker.
		ObjectId trackerObjectId_ = invalidObjectId();
};

inline std::string IOSHeadingTracker3DOF::deviceNameIOSHeadingTracker3DOF()
{
	return std::string("IOS 3DOF Heading Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_HEADING_TRACKER_3_DOF_H
