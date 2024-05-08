/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKFactory.h"
#include "ocean/devices/arkit/AKDepthTracker6DOF.h"
#include "ocean/devices/arkit/AKFaceTracker6DOF.h"
#include "ocean/devices/arkit/AKGeoAnchorsTracker6DOF.h"
#include "ocean/devices/arkit/AKPlaneTracker6DOF.h"
#include "ocean/devices/arkit/AKRoomPlanTracker6DOF.h"
#include "ocean/devices/arkit/AKSceneTracker6DOF.h"
#include "ocean/devices/arkit/AKWorldTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/devices/Manager.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKFactory::GeoAnchorAvailabilityChecker::GeoAnchorAvailabilityChecker(AKFactory& owner) :
	owner_(owner)
{
	startThread();
}

AKFactory::GeoAnchorAvailabilityChecker::~GeoAnchorAvailabilityChecker()
{
	stopThreadExplicitly();
}

void AKFactory::GeoAnchorAvailabilityChecker::threadRun()
{
	bool requestSent = false;

	if (@available(iOS 11.0, *))
	{
		while (!shouldThreadStop())
		{
			sleep(10u);

			if (gpsTracker_.isNull())
			{
				gpsTracker_ = Manager::get().device(GPSTracker::deviceTypeGPSTracker());

				if (gpsTracker_.isNull())
				{
					continue;
				}

				gpsTracker_->start();
			}

			ocean_assert(gpsTracker_);
			const GPSTracker::GPSTrackerSampleRef sample = gpsTracker_->sample();
			if (sample.isNull())
			{
				continue;
			}

			ocean_assert(!sample->locations().empty());

			const GPSTracker::Location& location = sample->locations().front();

			if (requestSent)
			{
				const ScopedLock scopedLock(lock_);

				if (availabilityState_ != AS_UNKNOWN)
				{
					owner_.onKnownGeoAnchorAvailability(availabilityState_);
					return;
				}

				continue;
			}

			if (@available(iOS 14.0, *))
			{
				// on iOS 14 we may have access to SLAM tracking + geo anchors

				if (ARGeoTrackingConfiguration.isSupported)
				{
					CLLocationCoordinate2D coordinate;
					coordinate.latitude = location.latitude();
					coordinate.longitude = location.longitude();

					requestSent = true;

					[ARGeoTrackingConfiguration checkAvailabilityAtCoordinate:coordinate completionHandler:^(BOOL isAvailable, NSError* error)
					{
						if (isAvailable == YES)
						{
							Log::info() << "ARKit's Geo Anchors are available at the current location";

							const ScopedLock scopedLock(lock_);
							availabilityState_ = AS_AVAILABLE;
						}
						else
						{
							Log::info() << "ARKit's Geo Anchors are not available at the current location " << location.latitude() << ", " << location.longitude() << ": " << StringApple::toUTF8(error.domain);

							const ScopedLock scopedLock(lock_);
							availabilityState_ = AS_NOT_AVAILABLE;
						}
					}];
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
}

AKFactory::AKFactory() :
	Factory(nameARKitLibrary()),
	geoAnchorAvailabilityChecker_(*this)
{
	registerDevices();
}

bool AKFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new AKFactory()));
}

bool AKFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameARKitLibrary());
}

void AKFactory::registerDevices()
{
	registerDevice(AKWorldTracker6DOF::deviceNameAKWorldTracker6DOF(), AKWorldTracker6DOF::deviceTypeAKWorldTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));
	registerDevice(AKPlaneTracker6DOF::deviceNameAKPlaneTracker6DOF(), AKPlaneTracker6DOF::deviceTypeAKPlaneTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));
	registerDevice(AKSceneTracker6DOF::deviceNameAKSceneTracker6DOF(), AKSceneTracker6DOF::deviceTypeAKSceneTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));
	registerDevice(AKDepthTracker6DOF::deviceNameAKDepthTracker6DOF(), AKDepthTracker6DOF::deviceTypeAKDepthTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));
	registerDevice(AKFaceTracker6DOF::deviceNameAKFaceTracker6DOF(), AKFaceTracker6DOF::deviceTypeAKFaceTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));

	if (AKRoomPlanTracker6DOF::isSupported())
	{
		registerDevice(AKRoomPlanTracker6DOF::deviceNameAKRoomPlanTracker6DOF(), AKRoomPlanTracker6DOF::deviceTypeAKRoomPlanTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKRoomPlanTracker6DOF));
	}
}

void AKFactory::onKnownGeoAnchorAvailability(const GeoAnchorAvailabilityChecker::AvailabilityState availabilityState)
{
	if (availabilityState == GeoAnchorAvailabilityChecker::AS_AVAILABLE)
	{
		registerDevice(AKGeoAnchorsTracker6DOF::deviceNameAKGeoAnchorsTracker6DOF(), AKGeoAnchorsTracker6DOF::deviceTypeAKGeoAnchorsTracker6DOF(), InstanceFunction::create(*this, &AKFactory::createAKTracker6DOF));
	}
}

Device* AKFactory::createAKTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	const GeoAnchorAvailabilityChecker::AvailabilityState availabilityState = geoAnchorAvailabilityChecker_.availabilityState();
	ocean_assert(availabilityState != GeoAnchorAvailabilityChecker::AS_UNKNOWN); // should be know when the first tracker is created

	if (name == AKWorldTracker6DOF::deviceNameAKWorldTracker6DOF())
	{
		ocean_assert(deviceType == AKWorldTracker6DOF::deviceTypeAKWorldTracker6DOF());

		return new AKWorldTracker6DOF();
	}
	else if (name == AKSceneTracker6DOF::deviceNameAKSceneTracker6DOF())
	{
		ocean_assert(deviceType == AKSceneTracker6DOF::deviceTypeAKSceneTracker6DOF());

		return new AKSceneTracker6DOF();
	}
	else if (name == AKPlaneTracker6DOF::deviceNameAKPlaneTracker6DOF())
	{
		ocean_assert(deviceType == AKPlaneTracker6DOF::deviceTypeAKPlaneTracker6DOF());

		return new AKPlaneTracker6DOF();
	}
	else if (name == AKDepthTracker6DOF::deviceNameAKDepthTracker6DOF())
	{
		ocean_assert(deviceType == AKDepthTracker6DOF::deviceTypeAKDepthTracker6DOF());

		return new AKDepthTracker6DOF();
	}
	else if (name == AKFaceTracker6DOF::deviceNameAKFaceTracker6DOF())
	{
		ocean_assert(deviceType == AKFaceTracker6DOF::deviceTypeAKFaceTracker6DOF());

		return new AKFaceTracker6DOF();
	}
	else
	{
		ocean_assert(name == AKGeoAnchorsTracker6DOF::deviceNameAKGeoAnchorsTracker6DOF());
		ocean_assert(deviceType == AKGeoAnchorsTracker6DOF::deviceTypeAKGeoAnchorsTracker6DOF());

		return new AKGeoAnchorsTracker6DOF();
	}
}

Device* AKFactory::createAKRoomPlanTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == AKRoomPlanTracker6DOF::deviceNameAKRoomPlanTracker6DOF());
	ocean_assert(deviceType == AKRoomPlanTracker6DOF::deviceTypeAKRoomPlanTracker6DOF());

	ocean_assert(AKRoomPlanTracker6DOF::isSupported());

	return new AKRoomPlanTracker6DOF();
}

}

}

}
