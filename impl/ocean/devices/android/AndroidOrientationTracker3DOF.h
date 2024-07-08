/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_ORIENTATION_TRACKER_3_DOF_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_ORIENTATION_TRACKER_3_DOF_H

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidSensor.h"

#include "ocean/devices/OrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements a 3DOF orientation tracker.
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
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_EXPORT AndroidOrientationTracker3DOF :
	virtual public AndroidSensor,
	virtual public OrientationTracker3DOF
{
	friend class AndroidFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The tracker's name
		 */
		static inline std::string deviceNameAndroidOrientationTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF orientation tracker device.
		 * @param sensor The Android sensor providing the sensor measurements, must be valid
		 */
		explicit AndroidOrientationTracker3DOF(const ASensor* sensor);

		/**
		 * Destructs a 3DOF orientation tracker device.
		 */
		~AndroidOrientationTracker3DOF() override;

		/**
		 * The actual event function of this device.
		 * @see AndroidDevice::onEventFunction().
		 */
		int onEventFunction() override;

	protected:

		/// The Android event timestamp of the first sensor event.
		int64_t firstAndroidEventTimestamp_ = 0ll;

		/// The Unix event timestamp of the first sensor event.
		Timestamp firstUnixEventTimestamp_ = Timestamp(false);
};

inline std::string AndroidOrientationTracker3DOF::deviceNameAndroidOrientationTracker3DOF()
{
	return std::string("Android 3DOF Orientation Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_ORIENTATION_TRACKER_3_DOF_H
