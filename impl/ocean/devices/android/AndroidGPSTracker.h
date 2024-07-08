/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_ANDROID_GPS_TRACKER_H
#define META_OCEAN_DEVICES_ANDROID_ANDROID_GPS_TRACKER_H

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidDevice.h"

#include "ocean/devices/GPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

/**
 * This class implements a GPS tracker.
 * The NDK does not have access to location services, so that the information needs to be provided from the Java side.
 * @ingroup devicesandroid
 */
class OCEAN_DEVICES_EXPORT AndroidGPSTracker :
	virtual public GPSTracker,
	virtual public AndroidDevice
{
	friend class AndroidFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return The sensor's name
		 */
		static inline std::string deviceNameAndroidGPSTracker();

		/**
		 * Returns the device type of this tracker.
		 * @return The sensor's device type
		 */
		static inline DeviceType deviceTypeAndroidGPSTracker();

	protected:

		/**
		 * Creates a new GPS tracker.
		 */
		AndroidGPSTracker();

		/**
		 * Destructs a GPS tracker.
		 */
		~AndroidGPSTracker() override;

		/**
		 * Sets a new GPS location for this tracker.
		 * @param location The GPS location to be set
		 * @param timestamp The timestamp of the GPS location, must be valid
		 **/
		void newGPSLocation(const Location& location, const Timestamp& timestamp);

	protected:

		/// The timestamp of the last sample.
		Timestamp lastTimestamp_;

		/// The unique id for the world object.
		ObjectId gpsObjectId_ = invalidObjectId();
};

inline std::string AndroidGPSTracker::deviceNameAndroidGPSTracker()
{
	return std::string("Android GPS Tracker");
}

inline AndroidGPSTracker::DeviceType AndroidGPSTracker::deviceTypeAndroidGPSTracker()
{
	return deviceTypeGPSTracker();
}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_ANDROID_GPS_TRACKER_H
