/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/FileConfig.h"

namespace Ocean
{

namespace IO
{

PinholeCamera CameraCalibrationManager::Device::camera(const unsigned int width, const unsigned int height, Quality* quality, const Scalar defaultFovX) const
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(defaultFovX > Numeric::eps() && defaultFovX < Numeric::pi());

	if (width != 0u && height != 0u)
	{
		const ResolutionMap::const_iterator iExact = resolutionMap_.find(Resolution(width, height));
		if (iExact != resolutionMap_.end())
		{
			if (quality != nullptr)
			{
				*quality = QUALITY_EXACT;
			}

			return iExact->second.first;
		}

		const Scalar targetAspect = Scalar(width) / Scalar(height);
		for (ResolutionMap::const_iterator i = resolutionMap_.begin(); i != resolutionMap_.end(); ++i)
		{
			const Scalar aspect = Scalar(i->first.first) / Scalar(i->first.second);

			if (Numeric::isEqual(aspect, targetAspect))
			{
				if (quality != nullptr)
				{
					*quality = QUALITY_INTERPOLATED;
				}

				const Scalar fovX = (--resolutionMap_.end())->second.first.fovX();

				const Scalar principalX = i->second.first.principalPointX() * Scalar(width) / Scalar(i->second.first.width());
				const Scalar principalY = i->second.first.principalPointY() * Scalar(height) / Scalar(i->second.first.height());

				PinholeCamera result(width, height, fovX, principalX, principalY);
				result.setRadialDistortion(i->second.first.radialDistortion());
				result.setTangentialDistortion(i->second.first.tangentialDistortion());

				return result;
			}
		}
	}

	if (quality != nullptr)
	{
		*quality = QUALITY_DEFAULT;
	}

	return PinholeCamera(width, height, defaultFovX);
}

bool CameraCalibrationManager::Device::addResolution(const PinholeCamera& pinholeCamera, const Priority priority)
{
	ResolutionMap::iterator i = resolutionMap_.find(Resolution(pinholeCamera.width(), pinholeCamera.height()));
	if (i == resolutionMap_.end())
	{
		resolutionMap_[Resolution(pinholeCamera.width(), pinholeCamera.height())] = CameraPair(pinholeCamera, priority);
		return true;
	}

	if (i->second.second <= priority)
	{
		i->second.first = pinholeCamera;
		i->second.second = priority;

		return true;
	}

	return false;
}

CameraCalibrationManager::CameraCalibrationManager()
{
	// nothing to do here
}

CameraCalibrationManager::~CameraCalibrationManager()
{
	// nothing to do here
}

bool CameraCalibrationManager::registerCalibration(const std::string& camera, const unsigned int width, const unsigned int height, const Scalar fovX)
{
	ocean_assert(!camera.empty());
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(fovX > Numeric::eps() && fovX < Numeric::pi());

	if (camera.empty() || width == 0u || height == 0u || fovX <= Numeric::eps() || fovX >= Numeric::pi())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	DeviceMap::iterator i = deviceMap_.find(camera);
	if (i == deviceMap_.end())
	{
		i = deviceMap_.insert(std::make_pair(camera, Device())).first;
	}

	ocean_assert(i != deviceMap_.end());
	return i->second.addResolution(PinholeCamera(width, height,fovX), PRIORITY_EXPLICIT);
}

bool CameraCalibrationManager::registerCalibrationFile(const std::string& url)
{
	if (url.empty())
	{
		return false;
	}

	FileConfig config(url);
	if (config.isEmpty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	for (unsigned int nCamera = 0u; nCamera < config.values("Camera"); ++nCamera)
	{
		Config::Value& camera = config.value("Camera", nCamera);

		if (camera.type() == Config::TYPE_GROUP)
		{
			const std::string name = camera["Name"]("");
			if (name.empty())
			{
				continue;
			}

			Priority priority = PRIORITY_LOW;
			std::string prorityString = camera["Priority"]("");
			if (prorityString == "MEDIUM")
			{
				priority = PRIORITY_MEDIUM;
			}
			else if (prorityString == "HIGH")
			{
				priority = PRIORITY_HIGH;
			}

			DeviceMap::iterator i = deviceMap_.find(name);
			if (i == deviceMap_.end())
			{
				i = deviceMap_.insert(std::make_pair(name, Device())).first;
			}

			for (unsigned int nAlias = 0; nAlias < camera.values("Alias"); ++nAlias)
			{
				Config::Value& alias = camera.value("Alias", nAlias);

				const std::string aliasName(alias(""));

				if (aliasName.empty() == false)
				{
					registerAlias(name, aliasName);
				}
			}

			for (unsigned int nCalibration = 0; nCalibration < camera.values("Calibration"); ++nCalibration)
			{
				Config::Value& calibration = camera.value("Calibration", nCalibration);

				const double resolutionX = calibration["ResolutionX"](0.0);
				const double resolutionY = calibration["ResolutionY"](0.0);

				if (resolutionX <= 0 || resolutionY <= 0)
				{
					Log::warning() << "Camera calibration file parsing: Invalid camera resolution of camera \"" << name << "\". The resolution must be defined as positive pixel values.";
					continue;
				}

				if (calibration.exist("FovX"))
				{
					const double fovX = calibration["FovX"](0.0);
					if (fovX <= 0 || fovX >= Numeric::pi_2())
					{
						Log::warning() << "Camera calibration file parsing: Invalid horizontal field of view of camera \"" << name << "\". The fov must be inside the range (0, PI / 2)";
						continue;
					}

					i->second.addResolution(PinholeCamera((unsigned int)(resolutionX), (unsigned int)(resolutionY), Scalar(fovX)), priority);
					continue;
				}

				if (calibration.exist("FovY"))
				{
					const double fovY = calibration["FovY"](0.0);
					if (fovY <= 0 || fovY >= Numeric::pi_2())
					{
						Log::warning() << "Camera calibration file parsing: Invalid vertical field of view of camera \"" << name << "\". The fov must be inside the range (0, PI / 2)";
						continue;
					}

					Scalar fovX = PinholeCamera::fovY2X(Scalar(fovY), Scalar(resolutionX) / Scalar(resolutionY));

					PinholeCamera newCamera((unsigned int)(resolutionX), (unsigned int)(resolutionY), fovX);
					i->second.addResolution(newCamera, priority);
					continue;
				}

				const double focalLengthX = calibration["FocalLengthX"](0.0);
				if (focalLengthX <= 0)
				{
					Log::warning() << "Camera calibration file parsing: Invalid horizontal focal length value camera \"" << name << "\". The value must be positive!";
					continue;
				}

				const double focalLengthY = calibration["FocalLengthY"](0.0);
				if (focalLengthY <= 0)
				{
					Log::warning() << "Camera calibration file parsing: Invalid vertical focal length value camera \"" << name << "\". The value must be positive!";
					continue;
				}

				const double principalPointX = calibration["PrincipalPointX"](resolutionX * 0.5);
				const double principalPointY = calibration["PrincipalPointY"](resolutionY * 0.5);

				PinholeCamera::DistortionPair radialDistortion;
				radialDistortion.first = Scalar(calibration["RadialDistortionR2"](0.0));
				radialDistortion.second = Scalar(calibration["RadialDistortionR4"](0.0));

				PinholeCamera::DistortionPair tangentialDistortion;
				tangentialDistortion.first = Scalar(calibration["TangentialDistortion0"](0.0));
				tangentialDistortion.second = Scalar(calibration["TangentialDistortion1"](0.0));

				PinholeCamera newCamera((unsigned int)(resolutionX), (unsigned int)(resolutionY),
									Scalar(focalLengthX), Scalar(focalLengthY), Scalar(principalPointX), Scalar(principalPointY),
									radialDistortion, tangentialDistortion);

				i->second.addResolution(newCamera, priority);
			}
		}
	}

	return true;
}

bool CameraCalibrationManager::registerCalibration(const std::string& camera, const PinholeCamera& calibration)
{
	const ScopedLock scopedLock(lock_);

	DeviceMap::iterator i = deviceMap_.find(camera);
	if (i == deviceMap_.end())
	{
		i = deviceMap_.insert(std::make_pair(camera, Device())).first;
	}

	i->second.addResolution(calibration, PRIORITY_EXPLICIT);

	return true;
}

bool CameraCalibrationManager::registerAlias(const std::string& camera, const std::string& alias)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(aliasMap_.find(alias) == aliasMap_.end());
	aliasMap_[alias] = camera;

	return true;
}

PinholeCamera CameraCalibrationManager::camera(const std::string& device, const unsigned int width, const unsigned int height, Quality* quality, const Scalar defaultFovX) const
{
	ocean_assert(!device.empty());
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(defaultFovX > Numeric::eps() && defaultFovX < Numeric::pi());

	const ScopedLock scopedLock(lock_);

	DeviceMap::const_iterator i = deviceMap_.find(device);
	if (i == deviceMap_.end())
	{
		const AliasMap::const_iterator iA = aliasMap_.find(device);

		if (iA != aliasMap_.end())
		{
			i = deviceMap_.find(iA->second);
		}

		if (i == deviceMap_.end())
		{
			if (quality != nullptr)
			{
				*quality = QUALITY_DEFAULT;
			}

			return PinholeCamera(width, height, defaultFovX);
		}
	}

	return i->second.camera(width, height, quality, defaultFovX);
}

bool CameraCalibrationManager::hasCalibration(const std::string& device) const
{
	const ScopedLock scopedLock(lock_);

	const DeviceMap::const_iterator i = deviceMap_.find(device);

	if (i != deviceMap_.end())
	{
		return true;
	}

	const AliasMap::const_iterator iA = aliasMap_.find(device);
	if (iA == aliasMap_.end())
	{
		return false;
	}

	return deviceMap_.find(iA->second) != deviceMap_.end();
}

}

}
