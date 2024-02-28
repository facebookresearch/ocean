// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/sensors/SensorSettingsManager.h"

#include <vros/sys/sensors/SensorSettingsManager.h>

using namespace OSSDK::Sensors;

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

SensorSettingsManager::~SensorSettingsManager()
{
	release();
}

bool SensorSettingsManager::setExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, OSSDK::ArrayView<const OSSDK::Sensors::v3::ExposureInfo> exposureSettings)
{
	const ScopedLock scopedLock(lock_);

	if (!ossdkSensorSettingsManager_)
	{
		ossdkSensorSettingsManager_ = v4::createSensorSettingsManager();

		ocean_assert(ossdkSensorSettingsManager_);
		if (!ossdkSensorSettingsManager_)
		{
			return false;
		}
	}

	if (activeOverridingCameraFrameTypeMap_.find(cameraFrameType) == activeOverridingCameraFrameTypeMap_.cend())
	{
		if (ossdkSensorSettingsManager_->startOverridingExposureSettings(cameraFrameType) != 0)
		{
			ocean_assert(false && "Valid to start exposure overriding!");
			return false;
		}

		activeOverridingCameraFrameTypeMap_.insert(cameraFrameType);
	}

	return ossdkSensorSettingsManager_->setExposureSettings(cameraFrameType, exposureSettings) == 0;
}

bool SensorSettingsManager::resetExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType)
{
	const ScopedLock scopedLock(lock_);

	if (!ossdkSensorSettingsManager_)
	{
		// we actually have nothing to reset
		return true;
	}

	const CameraFrameTypeMap::iterator iActive = activeOverridingCameraFrameTypeMap_.find(cameraFrameType);

	if (iActive == activeOverridingCameraFrameTypeMap_.cend())
	{
		// we actually have nothing to reset
		return true;
	}

	if (ossdkSensorSettingsManager_->stopOverridingExposureSettings(cameraFrameType) != 0)
	{
		ocean_assert(false && "Failed to reset exposure settings");
		return false;
	}

	activeOverridingCameraFrameTypeMap_.erase(iActive);

	return true;
}

void SensorSettingsManager::release()
{
	const ScopedLock scopedLock(lock_);

	if (!ossdkSensorSettingsManager_)
	{
		return;
	}

	while (!activeOverridingCameraFrameTypeMap_.empty())
	{
		if (!resetExposureSettings(*activeOverridingCameraFrameTypeMap_.begin()))
		{
			Log::error() << "Failed to reset exposure settings!";
			break;
		}
	}

	ossdkSensorSettingsManager_ = nullptr;
}

}

}

}

}

}
