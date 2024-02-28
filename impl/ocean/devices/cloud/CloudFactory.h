// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_FACTORY_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_FACTORY_H

#include "ocean/devices/cloud/Cloud.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a device factory for the Cloud tracking library.
 * @ingroup devicescloud
 */
class OCEAN_DEVICES_CLOUD_EXPORT CloudFactory : public Factory
{
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
		CloudFactory();

		/**
		 * Registers all devices.
		 */
		void registerDevices();

		/**
		 * Creates a new 6DOF tracker that creates camera poses based on a per-frame localization.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createCloudPerFrameRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 6DOF tracker that creates camera poses based on a cloud relocalizer and fused poses from SLAM world tracking.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createCloudRelocalizerTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 6DOF tracker that creates camera poses based on a cloud relocalizer for placements and fused poses from SLAM world tracking.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createCloudPlacementsTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 6DOF tracker that creates camera poses based on a cloud relocalizer for ECEF-based coordinate systems and fused poses from SLAM world tracking.
		 * The caller is responsible to release the sensor.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createCloudRelocalizerECEFTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Creates a new 6DOF tracker that creates camera poses based on a cloud relocalizer for ECEF-based coordinate systems,
		 * fused poses from SLAM world tracking, and fused poses from GeoAnchor tracking.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new sensor, nullptr if the sensor could not be created
		 */
		static Device* createCloudRelocalizerFusedGeoTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_FACTORY_H
