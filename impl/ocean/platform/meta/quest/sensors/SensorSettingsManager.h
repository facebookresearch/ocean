// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_SENSOR_HACK_SENSOR_SETTINGS_MANAGER_H
#define META_OCEAN_PLATFORM_META_QUEST_SENSOR_HACK_SENSOR_SETTINGS_MANAGER_H

#include "ocean/platform/meta/quest/sensors/Sensors.h"

#include <vros/sys/sensors/ExposureControlAlgorithm.h>
#include <vros/sys/sensors/FrameType.h>

namespace OSSDK
{

namespace Sensors
{

namespace v4
{

// Forward declaration.
class ISensorSettingsManager;
}

}

}

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Sensors
{

/**
 * This class is a wrapper around SensorSettingsManager and a temporary hack only until T89346811 is fixed.
 * @ingroup platformmetaquestsensors
 */
class OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT SensorSettingsManager
{
	protected:

		/**
		 * Definition of an unordered set holding camera frame types.
		 */
		typedef std::unordered_set<OSSDK::Sensors::v3::FrameType> CameraFrameTypeMap;

	public:

		/**
		 * Default constructor.
		 */
		SensorSettingsManager() = default;

		/**
		 * Releases all resources and resets all custom exposure settings.
		 */
		~SensorSettingsManager();

		/**
		 * Sets custom exposure settings for a specified camera frame type.
		 * @param cameraFrameType The type of the camera frames for which the custom exposure settings will be set
		 * @param exposureSettings The exposure settings to be set, one for each camera
		 * @return True, if succeeded
		 */
		bool setExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, OSSDK::ArrayView<const OSSDK::Sensors::v3::ExposureInfo> exposureSettings);

		/**
		 * Resets the custom exposure settings for a specified camera frame type.
		 * @param cameraFrameType The type of the camera frames for which previously set custom exposure settings will be reset
		 * @return True, if succeeded
		 */
		bool resetExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType);

		/**
		 * Releases all resources and resets all custom exposure settings.
		 */
		void release();

	protected:

		/// The actual manager for exposure settings.
		std::shared_ptr<OSSDK::Sensors::v4::ISensorSettingsManager> ossdkSensorSettingsManager_;

		/// The set holding all types of camera frames which have currently overwritten exposure settings.
		CameraFrameTypeMap activeOverridingCameraFrameTypeMap_;

		/// The lock for this manager.
		Lock lock_;
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_SENSOR_HACK_SENSOR_SETTINGS_MANAGER_H
