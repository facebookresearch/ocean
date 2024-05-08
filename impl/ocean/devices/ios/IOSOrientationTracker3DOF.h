/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_ORIENTATION_TRACKER_3_DOF_H
#define META_OCEAN_DEVICES_IOS_IOS_ORIENTATION_TRACKER_3_DOF_H

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
 * This class implements all 3DOF orientation tracker purely based on the IMU information of the device.
 * The resulting transformation (3DOF orientation) is defined w.r.t. the world coordinate system.<br>
 * That means the transformation will transform points defined in the coordinate system of the device into points defined in the world coordinate system (pointWorld = wTd * pointDevice).<br>
 * When holding the device (in portrait mode), the x-axis points towards the right of the device, the y-axis points upwards, and the z-axis points towards the user:<br>
 * <pre>
 *          device top
 * ...........................
 * .                         .
 * .         ^               .
 * .       Y |               .
 * .         |               .
 * .         |               .
 * .         O --------->    .
 * .        /        X       .
 * .       / Z               .
 * .      v                  .
 * .                         .
 * ...........................
 * .      home button        .
 * ...........................
 *       device bottom
 * </pre>
 * The x-axis and y-axis of the world coordinate system will be arbitrary.<br>
 * The negative z-axis of the world coordinate system will always point towards the ground (the negative z-axis is identical with the gravity vector).
 *
 * Beware: The coordinate system of the camera may be different from the coordinate system of the device.<br>
 * E.g., the image of an iPhone camera is commonly located in the upper right corner of the device,<br>
 * with horizontal image axis pointing to the bottom of the device (along the negative y-axis of the device),
 * and with vertical image axis pointing to the left of the device (along the negative x-axis of the device).
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSOrientationTracker3DOF :
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
		static inline std::string deviceNameIOSOrientationTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF orientation tracker device.
		 */
		IOSOrientationTracker3DOF();

		/**
		 * Destructs a 3DOF orientation sensor device.
		 */
		~IOSOrientationTracker3DOF() override;

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

inline std::string IOSOrientationTracker3DOF::deviceNameIOSOrientationTracker3DOF()
{
	return std::string("IOS 3DOF Orientation Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_ORIENTATION_TRACKER_3_DOF_H
