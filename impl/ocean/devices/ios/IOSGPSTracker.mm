/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOSGPSTracker.h"

#include "ocean/base/StringApple.h"

using namespace Ocean;

@implementation IOSGPSTrackerDelegate
{
	/// The Core Location Manager
	CLLocationManager* clLocationManager_;

	/// The owner of this delegate object.
	@public Ocean::Devices::IOS::IOSGPSTracker* owner_;
}

-(void)initializeAndStart
{
	if (clLocationManager_ == nullptr)
	{
		clLocationManager_ = [[CLLocationManager alloc] init];
		clLocationManager_.delegate = self;
	}

	clLocationManager_.desiredAccuracy = kCLLocationAccuracyBest;
	clLocationManager_.distanceFilter = kCLDistanceFilterNone;

	[clLocationManager_ requestWhenInUseAuthorization];
	[clLocationManager_ requestAlwaysAuthorization];

	[clLocationManager_ startUpdatingLocation];
	[clLocationManager_ startUpdatingHeading];
}

-(BOOL)start
{
	if ([NSThread isMainThread])
	{
		[self initializeAndStart];
	}
	else
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			[self initializeAndStart];
		});
	}

	return true;
}

-(BOOL)stop
{
	if (clLocationManager_ != nullptr)
	{
		[clLocationManager_ stopUpdatingHeading];
		[clLocationManager_ stopUpdatingLocation];
	}

	return true;
}

-(void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations
{
	ocean_assert(locations.count >= 1);

	if (owner_)
	{
		CLLocation* currentLocation = [locations lastObject];
		ocean_assert(currentLocation != nullptr);

		const double latitude = currentLocation.coordinate.latitude;
		const double longitude = currentLocation.coordinate.longitude;

		const float altitude = float(currentLocation.altitude);
		const float direction = float(currentLocation.course);
		const float speed = float(currentLocation.speed);

		const float accuracy = float(currentLocation.horizontalAccuracy);
		const float altitudeAccuracy = float(currentLocation.verticalAccuracy);

		float directionAccuracy = -1.0f;
		if (@available(iOS 13.4, *))
		{
			directionAccuracy = float(currentLocation.courseAccuracy);
		}

		const float speedAccuracy = float(currentLocation.speedAccuracy);

		const Timestamp timestamp = Timestamp(currentLocation.timestamp.timeIntervalSince1970);

		owner_->newGPSLocation(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy, timestamp);
	}
}

-(void)locationManager:(CLLocationManager*)manager didFailWithError:(NSError*)error
{
	Log::error() << "IOSGPSTracker: error: '" << StringApple::toUTF8([error localizedDescription]) << "'";
}

@end

namespace Ocean
{

namespace Devices
{

namespace IOS
{

IOSGPSTracker::IOSGPSTracker() :
	Device(deviceNameIOSGPSTracker(), deviceTypeIOSGPSTracker()),
	IOSDevice(deviceNameIOSGPSTracker(), deviceTypeIOSGPSTracker()),
	Measurement(deviceNameIOSGPSTracker(), deviceTypeIOSGPSTracker()),
	Tracker(deviceNameIOSGPSTracker(), deviceTypeIOSGPSTracker()),
	GPSTracker(deviceNameIOSGPSTracker())
{
	gpsObjectId_ = addUniqueObjectId(deviceNameIOSGPSTracker());
}

IOSGPSTracker::~IOSGPSTracker()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool IOSGPSTracker::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (sensorIsStarted == true)
	{
		return true;
	}

	if (iosGPSTrackerDelegate_ == nullptr)
	{
		iosGPSTrackerDelegate_ = [IOSGPSTrackerDelegate new];
		iosGPSTrackerDelegate_->owner_ = this;
	}

	if ([iosGPSTrackerDelegate_ start] != TRUE)
	{
		return false;
	}

	sensorIsStarted = true;
	return true;
}

bool IOSGPSTracker::pause()
{
	return stop();
}

bool IOSGPSTracker::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!sensorIsStarted)
	{
		return true;
	}

	if (iosGPSTrackerDelegate_)
	{
		if ([iosGPSTrackerDelegate_ stop] != TRUE)
		{
			return false;
		}
	}

	sensorIsStarted = false;
	return true;
}

void IOSGPSTracker::newGPSLocation(const double latitude, const double longitude, const float altitude, const float direction, const float speed, const float accuracy, const float altitudeAccuracy, const float directionAccuracy, const float speedAccuracy, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (lastTimestamp_ == timestamp)
	{
		return;
	}

	if (lastTimestamp_.isInvalid())
	{
		postFoundTrackerObjects({gpsObjectId_}, timestamp);
	}

	Locations locations(1, Location(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy));

	ObjectIds objectIds(1, gpsObjectId_);

	postNewSample(SampleRef(new GPSTrackerSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(objectIds), std::move(locations))));

	lastTimestamp_ = timestamp;
}

}

}

}
