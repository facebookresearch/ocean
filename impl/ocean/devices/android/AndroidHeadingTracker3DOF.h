/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_HEADING_TRACKER_3_DOF_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_HEADING_TRACKER_3_DOF_H

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
 * This class implements a tracker providing the heading/north direction as 3-DOF orientation.
 * The resulting transformation (3DOF orientation) is defined w.r.t. the device's coordinate system and rotates the negative z-axis so that it points towards true north (the rotated positive z-axis points towards south).
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_EXPORT AndroidHeadingTracker3DOF :
	virtual public AndroidSensor,
	virtual public OrientationTracker3DOF
{
	friend class AndroidFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The tracker's name
		 */
		static inline std::string deviceNameAndroidHeadingTracker3DOF();

	protected:

		/**
		 * Creates a new 3DOF heading tracker device.
		 * @param sensor The Android sensor providing the sensor measurements, must be valid
		 */
		explicit AndroidHeadingTracker3DOF(const ASensor* sensor);

		/**
		 * Destructs a 3DOF heading tracker device.
		 */
		~AndroidHeadingTracker3DOF() override;

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

inline std::string AndroidHeadingTracker3DOF::deviceNameAndroidHeadingTracker3DOF()
{
	return std::string("Android 3DOF Heading Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_ORIENTATION_TRACKER_3_DOF_H
