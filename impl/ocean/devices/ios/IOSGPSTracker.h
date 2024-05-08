/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_IOS_GPS_TRACKER_H
#define META_OCEAN_DEVICES_IOS_IOS_GPS_TRACKER_H

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSDevice.h"

#include "ocean/devices/GPSTracker.h"

#include <CoreLocation/CoreLocation.h>

@interface IOSGPSTrackerDelegate : NSObject<CLLocationManagerDelegate>
@end

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a GPS tracker for iOS platforms.
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT IOSGPSTracker :
	virtual public IOSDevice,
	virtual public GPSTracker
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
		 * Sets a new GPS location for this tracker.
		 * @param latitude The position's latitude, in degree, always valid, with range [-90, 90]
		 * @param longitude The position's longitude, in degree, must be valid, with range [-180, 180]
		 * @param altitude The position's altitude, in meter, NumericF::minValue() if unknown
		 * @param direction The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown
		 * @param speed The device's speed, in meter per second, with range [0, infinity), -1 if unknown
		 * @param accuracy The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown
		 * @param altitudeAccuracy The vertical accuracy in meter, with range [0, infinity), -1 if unknown
		 * @param directionAccuracy The direction accuracy, in degree, with range [0, 180], -1 if unknown
		 * @param speedAccuracy The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown
		 * @param timestamp The timestamp of the GPS location, must be valid
		 */
		void newGPSLocation(const double latitude, const double longitude, const float altitude, const float direction, const float speed, const float accuracy, const float altitudeAccuracy, const float directionAccuracy, const float speedAccuracy, const Timestamp& timestamp);

		/**
		 * Returns the name of this tracker.
		 * @return The tracker's name
		 */
		static inline std::string deviceNameIOSGPSTracker();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeIOSGPSTracker();

	protected:

		/**
		 * Creates a new GPS tracker device.
		 */
		IOSGPSTracker();

		/**
		 * Destructs a GPS tracker device.
		 */
		~IOSGPSTracker() override;

	protected:

		/// The delegate of the IOS GPS Tracker.
		IOSGPSTrackerDelegate* iosGPSTrackerDelegate_ = nullptr;

		/// The timestamp of the last GPS signal;
		Timestamp lastTimestamp_;

		/// The unique id for the world object.
		ObjectId gpsObjectId_ = invalidObjectId();
};

inline std::string IOSGPSTracker::deviceNameIOSGPSTracker()
{
	return std::string("IOS GPS Tracker");
}

inline IOSGPSTracker::DeviceType IOSGPSTracker::deviceTypeIOSGPSTracker()
{
	return deviceTypeGPSTracker();
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_IOS_GPS_TRACKER_H
