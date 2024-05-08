/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_FACTORY_H
#define META_OCEAN_DEVICES_ARKIT_AK_FACTORY_H

#include "ocean/devices/arkit/ARKit.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/Factory.h"
#include "ocean/devices/GPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements a device factory for the ARKit tracking library.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKFactory : public Factory
{
	friend class GeoAnchorAvailabilityChecker;
	
	protected:

		/**
		 * This class implements a checker for the availability of GeoAnchors at the current location.
		 */
		class OCEAN_DEVICES_ARKIT_EXPORT GeoAnchorAvailabilityChecker : protected Thread
		{
			public:

				/**
				 * Definition of individual availability states.
				 */
				enum AvailabilityState : uint32_t
				{
					/// The availability is still not known.
					AS_UNKNOWN = 0u,
					/// Geo Anchors are know to be not available at the current location.
					AS_NOT_AVAILABLE,
					/// Geo Anchors are know to be available at the current location.
					AS_AVAILABLE
				};

			public:

				/**
				 * Creates a new object allowing to determine whether geo anchors are available.
				 * @param owner The owner of this object
				 */
				explicit GeoAnchorAvailabilityChecker(AKFactory& owner);

				/**
				 * Destructs this object.
				 */
				~GeoAnchorAvailabilityChecker() override;

				/**
				 * Returns the availability state.
				 * @return The current availability state
				 */
				inline AvailabilityState availabilityState() const;

			protected:

				/**
				 * The thread run function in which the availability state will be determined.
				 */
				void threadRun() override;

			protected:

				/// The owner of this object.
				AKFactory& owner_;

				/// The checker's availability state.
				AvailabilityState availabilityState_ = AS_UNKNOWN;

				/// The GPS tracker providing the current location.
				GPSTrackerRef gpsTracker_;

				/// The checker's lock.
				mutable Lock lock_;
		};

	public:

		/**
		 * Registers this factory at the manager.
		 * Beware: Unregister this factory if not needed anymore.
		 * @return True, if this factory hasn't been registered before
		 */
		static bool registerFactory();

		/**
		 * Unregisters this factory at the manger.
		 * This unregistration should be done after all created devices have been released.
		 * @return True, if this factory chould be unregistered
		 */
		static bool unregisterFactory();

	private:

		/**
		 * Creates a new factory.
		 */
		AKFactory();

		/**
		 * Registers all devices.
		 */
		void registerDevices();

		/**
		 * Event function which is called once the availability of geo anchors is known.
		 * @param availabilityState The known availability state, either AS_NOT_AVAILABLE or AS_AVAILABLE
		 */
		void onKnownGeoAnchorAvailability(const GeoAnchorAvailabilityChecker::AvailabilityState availabilityState);

		/**
		 * Creates a new 6DOF tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		Device* createAKTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 6DOF room plan tracker.
		 * The caller is responsible to release the tracker.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		Device* createAKRoomPlanTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

	private:

		/// The checker for the availability of ARKit's Geo Anchors.
		GeoAnchorAvailabilityChecker geoAnchorAvailabilityChecker_;
};

inline AKFactory::GeoAnchorAvailabilityChecker::AvailabilityState AKFactory::GeoAnchorAvailabilityChecker::availabilityState() const
{
	const ScopedLock scopedLock(lock_);
	return availabilityState_;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_FACTORY_H
