/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_GRAVITY_TRACKER_3_DOF_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_GRAVITY_TRACKER_3_DOF_H

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
 * This class implements a tracker providing the gravity direction as 3-DOF orientation.
 * The resulting transformation (3DOF orientation) is defined w.r.t. the device's coordinate system and rotates the negative y-axis so that it points towards gravity (the rotated positive y-axis points towards sky).<br>
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_EXPORT AndroidGravityTracker3DOF :
	virtual public AndroidSensor,
	virtual public OrientationTracker3DOF
{
	friend class AndroidFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The tracker's name
		 */
		static inline std::string deviceNameAndroidGravityTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF gravity tracker device.
		 * @param sensor The Android sensor providing the sensor measurements, must be valid
		 */
		explicit AndroidGravityTracker3DOF(const ASensor* sensor);

		/**
		 * Destructs a 3DOF gravity tracker device.
		 */
		~AndroidGravityTracker3DOF() override;

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

inline std::string AndroidGravityTracker3DOF::deviceNameAndroidGravityTracker3DOF()
{
	return std::string("Android 3DOF Gravity Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_GRAVITY_TRACKER_3_DOF_H
