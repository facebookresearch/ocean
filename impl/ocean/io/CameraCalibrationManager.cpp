/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/FisheyeCamera.h"

namespace Ocean
{

namespace IO
{

CameraCalibrationManager::CameraCalibrationManager()
{
	registerFactoryFunction(AnyCameraPinhole::WrappedCamera::name(), createOceanPinhole);
	registerFactoryFunction(AnyCameraFisheye::WrappedCamera::name(), createOceanFisheye);
}

bool CameraCalibrationManager::CalibrationGroup::addCamera(SharedAnyCamera&& camera)
{
	ocean_assert(camera != nullptr && camera->isValid());

	if (camera == nullptr || !camera->isValid())
	{
		return false;
	}

	const unsigned int width = camera->width();
	const unsigned int height = camera->height();

	for (const SharedAnyCamera& existingCamera : cameras_)
	{
		if (existingCamera->width() == width && existingCamera->height() == height)
		{
			// we already have a camera model with identical resolution

			return false;
		}
	}

	cameras_.emplace_back(std::move(camera));
	return true;
}

SharedAnyCamera CameraCalibrationManager::CalibrationGroup::camera(const unsigned int width, const unsigned int height, CalibrationQuality& calibrationQuality) const
{
	ocean_assert(width != 0u && height != 0u);

	if (cameras_.empty())
	{
		return nullptr;
	}

	for (const SharedAnyCamera& camera : cameras_)
	{
		if (camera->width() == width && camera->height() == height)
		{
			calibrationQuality = CQ_EXACT;

			return camera;
		}
	}

	// we could not find a perfect match, let's see whether we can interpolate a camera model

	for (const SharedAnyCamera& camera : cameras_)
	{
		ocean_assert(camera && camera->isValid());

		const unsigned int interpolatedWidth = (camera->width() * height) / camera->height();

		if (interpolatedWidth == width)
		{
			SharedAnyCamera interpolatedCamera = camera->clone(width, height);

			if (interpolatedCamera)
			{
				calibrationQuality = CQ_INTERPOLATED;

				return interpolatedCamera;
			}
		}
	}

	return nullptr;
}

SharedAnyCamera CameraCalibrationManager::camera(const std::string& cameraName, unsigned int width, unsigned int height, CalibrationQuality* calibrationQuality) const
{
	ocean_assert(!cameraName.empty());
	ocean_assert(width != 0u && height != 0u);

	if (cameraName.empty() || width == 0u || height == 0u)
	{
		return nullptr;
	}

	const ScopedLock scopedLock(lock_);

	CameraMap::const_iterator iCamera = cameraMap_.find(cameraName);

	if (iCamera == cameraMap_.end())
	{
		const AliasMap::const_iterator iAlias = aliasMap_.find(cameraName);

		if (iAlias != aliasMap_.end())
		{
			iCamera = cameraMap_.find(iAlias->second);
		}
	}

	if (iCamera == cameraMap_.end())
	{
		return nullptr;
	}

	const CalibrationGroups& calibrationGroups = iCamera->second;

	// let's find the camera model with highest priority and calibration quality, quality goes over priority

	SharedAnyCamera bestCamera;
	CalibrationQuality bestCalibrationQuality = CQ_UNKNOWN;
	int32_t bestPriority = std::numeric_limits<int32_t>::min();

	for (const CalibrationGroup& calibrationGroup : calibrationGroups)
	{
		CalibrationQuality candidateCalibrationQuality = CQ_UNKNOWN;
		SharedAnyCamera candidateCamera = calibrationGroup.camera(width, height, candidateCalibrationQuality);

		if (!candidateCamera)
		{
			continue;
		}

		if (candidateCalibrationQuality > bestCalibrationQuality || (candidateCalibrationQuality == bestCalibrationQuality && calibrationGroup.priority_ > bestPriority))
		{
			bestCamera = std::move(candidateCamera);
			bestCalibrationQuality = candidateCalibrationQuality;
			bestPriority = calibrationGroup.priority_;
		}
	}

	if (!bestCamera)
	{
		return nullptr;
	}

	if (calibrationQuality != nullptr)
	{
		*calibrationQuality = bestCalibrationQuality;
	}

	return bestCamera;
}

bool CameraCalibrationManager::registerCalibrations(const std::string& url)
{
	if (url.empty())
	{
		return false;
	}

	std::string errorMessage;
	const JSONParser::JSONValue jsonValue = JSONParser::parse(url, std::string(), false, &errorMessage);

	if (!jsonValue.isValid())
	{
		Log::error() << "Failed to parse camera calibration file '" << url << "': " << errorMessage;
		return false;
	}

	return registerCalibrations(jsonValue);
}

bool CameraCalibrationManager::registerCalibrations(const void* buffer, const size_t size)
{
	if (buffer == nullptr || size == 0)
	{
		return false;
	}

	const std::string bufferString((const char*)(buffer), size);

	std::string errorMessage;
	const JSONParser::JSONValue jsonValue = JSONParser::parse(std::string(), bufferString, false, &errorMessage);

	if (!jsonValue.isValid())
	{
		Log::error() << "Failed to parse camera calibration buffer: " << errorMessage;
		return false;
	}

	return registerCalibrations(jsonValue);
}

bool CameraCalibrationManager::registerCalibrations(const JSONParser::JSONValue& jsonValue)
{
	if (!jsonValue.isObject())
	{
		Log::error() << "Invalid camera calibration format: root must be an object";
		return false;
	}

	const JSONParser::JSONValue::Array* camerasArray = jsonValue.arrayFromObject("cameras");
	if (camerasArray == nullptr)
	{
		Log::error() << "Invalid camera calibration format: 'cameras' array not found";
		return false;
	}

	const ScopedLock scopedLock(lock_);

	for (const JSONParser::JSONValue& cameraValue : *camerasArray)
	{
		if (!cameraValue.isObject())
		{
			Log::warning() << "Skipping invalid camera entry: not an object";
			continue;
		}

		const std::string* cameraName = cameraValue.stringFromObject("name");
		if (cameraName == nullptr)
		{
			Log::warning() << "Skipping camera entry: 'name' not found or invalid";
			continue;
		}

		const JSONParser::JSONValue::Array* calibrationsArray = cameraValue.arrayFromObject("calibrations");
		if (calibrationsArray == nullptr)
		{
			Log::warning() << "Skipping camera '" << *cameraName << "': 'calibrations' array not found";
			continue;
		}

		int32_t priority = 0;
		const double* priorityValue = cameraValue.numberFromObject("priority");
		if (priorityValue != nullptr)
		{
			priority = NumericD::round32(*priorityValue);
		}

		CalibrationGroup calibrationGroup(priority);

		for (const JSONParser::JSONValue& calibrationObject : *calibrationsArray)
		{
			if (!calibrationObject.isObject())
			{
				Log::warning() << "Skipping invalid calibration entry for camera '" << *cameraName << "': not an object";
				continue;
			}

			const std::string* model = calibrationObject.stringFromObject("model");
			if (model == nullptr)
			{
				Log::warning() << "Skipping calibration for camera '" << *cameraName << "': 'model' not found";
				continue;
			}

			const FactoryFunctionMap::const_iterator iFactory = factoryFunctionMap_.find(*model);
			if (iFactory == factoryFunctionMap_.end())
			{
				Log::error() << "No factory registered for camera model '" << *model << "'";
				continue;
			}

			SharedAnyCamera camera = iFactory->second(calibrationObject);

			if (camera == nullptr || !camera->isValid())
			{
				Log::warning() << "Failed to create camera model '" << *model << "' for '" << *cameraName << "'";
				continue;
			}

			calibrationGroup.addCamera(std::move(camera));
		}

		if (calibrationGroup.size() >= 1)
		{
			cameraMap_[*cameraName].emplace_back(std::move(calibrationGroup));

			const JSONParser::JSONValue::Array* aliasesArray = cameraValue.arrayFromObject("aliases");
			if (aliasesArray != nullptr)
			{
				for (const JSONParser::JSONValue& aliasValue : *aliasesArray)
				{
					if (aliasValue.isString())
					{
						const std::string& alias = aliasValue.string();
						aliasMap_[alias] = *cameraName;
					}
				}
			}
		}
	}

	return true;
}

bool CameraCalibrationManager::registerCamera(const std::string& cameraName, SharedAnyCamera&& camera, const int32_t priority)
{
	ocean_assert(!cameraName.empty());
	ocean_assert(camera != nullptr && camera->isValid());

	if (cameraName.empty() || camera == nullptr || !camera->isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	CalibrationGroup calibrationGroup(priority);
	calibrationGroup.addCamera(std::move(camera));

	cameraMap_[cameraName].emplace_back(std::move(calibrationGroup));

	return true;
}

bool CameraCalibrationManager::registerFactoryFunction(const std::string& modelName, FactoryFunction&& factoryFunction)
{
	ocean_assert(!modelName.empty());
	if (modelName.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (factoryFunction == nullptr)
	{
		ocean_assert(factoryFunctionMap_.contains(modelName));

		factoryFunctionMap_.erase(modelName);
	}

	return factoryFunctionMap_.emplace(modelName, std::move(factoryFunction)).second;
}

SharedAnyCamera CameraCalibrationManager::parseCamera(const std::string& jsonCameraCalibrationFile, std::string&& jsonCameraCalibratio) const
{
	ocean_assert(!jsonCameraCalibrationFile.empty() || !jsonCameraCalibratio.empty());
	ocean_assert(jsonCameraCalibrationFile.empty() || jsonCameraCalibratio.empty());

	if (jsonCameraCalibrationFile.empty() && jsonCameraCalibratio.empty())
	{
		return nullptr;
	}

	std::string errorMessage;
	const JSONParser::JSONValue jsonValue = JSONParser::parse(jsonCameraCalibrationFile, jsonCameraCalibratio, false, &errorMessage);

	if (!jsonValue.isValid())
	{
		Log::error() << "Failed to parse camera calibration: " << errorMessage;
		return nullptr;
	}

	const std::string* modelString = jsonValue.stringFromObject("model");

	if (modelString == nullptr)
	{
		return nullptr;
	}

	const ScopedLock scopedLock(lock_);

	const FactoryFunctionMap::const_iterator iFactory = factoryFunctionMap_.find(*modelString);
	if (iFactory == factoryFunctionMap_.end())
	{
		Log::error() << "No factory registered for camera model '" << *modelString << "'";
		return nullptr;
	}

	return iFactory->second(jsonValue);
}

void CameraCalibrationManager::clear()
{
	const ScopedLock scopedLock(lock_);

	cameraMap_.clear();
	aliasMap_.clear();
}

bool CameraCalibrationManager::parseResolution(const JSONParser::JSONValue& calibrationObject, unsigned int& width, unsigned int& height)
{
	ocean_assert(calibrationObject.isObject());

	const JSONParser::JSONValue* resolutionValue = calibrationObject.valueFromObject("resolution");
	if (resolutionValue == nullptr || !resolutionValue->isObject())
	{
		return false;
	}

	const double* widthValue = resolutionValue->numberFromObject("width");
	const double* heightValue = resolutionValue->numberFromObject("height");

	if (widthValue == nullptr || heightValue == nullptr)
	{
		return false;
	}

	const int32_t signedWidth = NumericD::round32(*widthValue);
	const int32_t signedHeight = NumericD::round32(*heightValue);

	if (signedWidth <= 0 || signedHeight <= 0)
	{
		return false;
	}

	width = (unsigned int)(signedWidth);
	height = (unsigned int)(signedHeight);

	return true;
}

SharedAnyCamera CameraCalibrationManager::createOceanPinhole(const JSONParser::JSONValue& modelObject)
{
	ocean_assert(modelObject.isObject());
	if (!modelObject.isObject())
	{
		return nullptr;
	}

	unsigned int width = 0u;
	unsigned int height = 0u;
	if (!parseResolution(modelObject, width, height))
	{
		return nullptr;
	}

	const std::string* modelName = modelObject.stringFromObject("model");
	if (modelName == nullptr || *modelName != AnyCameraPinhole::WrappedCamera::name())
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	// let's see whether the camera model is defined via a configuration/parameter pair

	const std::string* configuration = modelObject.stringFromObject("configuration");
	const JSONParser::JSONValue::Array* parametersArray = modelObject.arrayFromObject("parameters");

	if (configuration != nullptr && parametersArray != nullptr)
	{
		PinholeCamera::ParameterConfiguration parameterConfiguration = PinholeCamera::PC_UNKNOWN;
		size_t expectedParameterCount = 0;

		if (*configuration == "3_PARAMETERS_ONE_FOCAL_LENGTH")
		{
			// F, mx, my

			parameterConfiguration = PinholeCamera::PC_3_PARAMETERS_ONE_FOCAL_LENGTH;
			expectedParameterCount = 3;
		}
		else if (*configuration == "4_PARAMETERS")
		{
			// Fx, Fy, mx, my

			parameterConfiguration = PinholeCamera::PC_4_PARAMETERS;
			expectedParameterCount = 4;
		}
		else if (*configuration == "7_PARAMETERS_ONE_FOCAL_LENGTH")
		{
			// F, mx, my, k1, k2, p1, p2

			parameterConfiguration = PinholeCamera::PC_7_PARAMETERS_ONE_FOCAL_LENGTH;
			expectedParameterCount = 7;
		}
		else if (*configuration == "8_PARAMETERS")
		{
			// Fx, Fy, mx, my, k1, k2, p1, p2

			parameterConfiguration = PinholeCamera::PC_8_PARAMETERS;
			expectedParameterCount = 8;
		}
		else
		{
			Log::warning() << "CameraCalibrationManager::createOceanPinhole(): Unknown parameter configuration: " << *configuration;
			return nullptr;
		}

		ocean_assert(parameterConfiguration != PinholeCamera::PC_UNKNOWN);
		ocean_assert(expectedParameterCount != 0);

		if (parametersArray->size() != expectedParameterCount)
		{
			Log::warning() << "CameraCalibrationManager::createOceanPinhole(): Invalid number of parameters: " << parametersArray->size() << ", expected: " << expectedParameterCount;
			return nullptr;
		}

		Scalars parameters;

		for (const JSONParser::JSONValue& parameterValue : *parametersArray)
		{
			if (!parameterValue.isNumber())
			{
				Log::warning() << "CameraCalibrationManager::createOceanPinhole(): Invalid parameter value in Ocean Pinhole camera model";
				return nullptr;
			}

			parameters.push_back(parameterValue.number());
		}

		ocean_assert(parameters.size() == expectedParameterCount);

		return std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, parameterConfiguration, parameters.data()));
	}

	// let's see whether the camera model is defined via a simple horizontal field of view

	const double* numberFovX = modelObject.numberFromObject("fovx");

	if (numberFovX != nullptr)
	{
		const double fovX = *numberFovX;

		if (fovX <= NumericD::deg2rad(1) || fovX > NumericD::deg2rad(175))
		{
			Log::warning() << "CameraCalibrationManager::createOceanPinhole(): Invalid horizontal field of view: " << fovX << " rad (" << NumericD::rad2deg(fovX) << " deg)";
			return nullptr;
		}

		return std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, Scalar(fovX)));
	}

	// let's see whether the camera model is defined via a simple vertical field of view

	const double* numberFovY = modelObject.numberFromObject("fovy");

	if (numberFovY != nullptr)
	{
		const double fovY = *numberFovY;

		if (fovY <= NumericD::deg2rad(1) || fovY > NumericD::deg2rad(175))
		{
			Log::warning() << "CameraCalibrationManager::createOceanPinhole(): Invalid vertical field of view: " << fovY << " rad (" << NumericD::rad2deg(fovY) << " deg)";
			return nullptr;
		}

		ocean_assert(height != 0u);
		const Scalar fovX = Camera::fovY2X(Scalar(fovY), Scalar(width) / Scalar(height));

		return std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, fovX));
	}

	ocean_assert(false && "Invalid Ocean Pinhole camera model");
	return nullptr;
}

SharedAnyCamera CameraCalibrationManager::createOceanFisheye(const JSONParser::JSONValue& modelObject)
{
	ocean_assert(modelObject.isObject());
	if (!modelObject.isObject())
	{
		return nullptr;
	}

	unsigned int width = 0u;
	unsigned int height = 0u;
	if (!parseResolution(modelObject, width, height))
	{
		return nullptr;
	}

	const std::string* modelName = modelObject.stringFromObject("model");
	if (modelName == nullptr || *modelName != AnyCameraFisheye::WrappedCamera::name())
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	// let's see whether the camera model is defined via a configuration/parameter pair

	const std::string* configuration = modelObject.stringFromObject("configuration");
	const JSONParser::JSONValue::Array* parametersArray = modelObject.arrayFromObject("parameters");

	if (configuration != nullptr && parametersArray != nullptr)
	{
		FisheyeCamera::ParameterConfiguration parameterConfiguration = FisheyeCamera::PC_UNKNOWN;
		size_t expectedParameterCount = 0;

		if (*configuration == "3_PARAMETERS_ONE_FOCAL_LENGTH")
		{
			// F, mx, my

			parameterConfiguration = FisheyeCamera::PC_3_PARAMETERS_ONE_FOCAL_LENGTH;
			expectedParameterCount = 3;
		}
		else if (*configuration == "4_PARAMETERS")
		{
			// Fx, Fy, mx, my

			parameterConfiguration = FisheyeCamera::PC_4_PARAMETERS;
			expectedParameterCount = 4;
		}
		else if (*configuration == "11_PARAMETERS_ONE_FOCAL_LENGTH")
		{
			// F, mx, my, k3, k5, k7, k9, p1, p3, p5, p7

			parameterConfiguration = FisheyeCamera::PC_11_PARAMETERS_ONE_FOCAL_LENGTH;
			expectedParameterCount = 11;
		}
		else if (*configuration == "12_PARAMETERS")
		{
			// Fx, Fy, mx, my, k3, k5, k7, k9, p1, p3, p5, p7

			parameterConfiguration = FisheyeCamera::PC_12_PARAMETERS;
			expectedParameterCount = 12;
		}
		else
		{
			Log::warning() << "CameraCalibrationManager::createOceanFisheye(): Unknown parameter configuration: " << *configuration;
			return nullptr;
		}

		ocean_assert(parameterConfiguration != FisheyeCamera::PC_UNKNOWN);
		ocean_assert(expectedParameterCount != 0);

		if (parametersArray->size() != expectedParameterCount)
		{
			Log::warning() << "CameraCalibrationManager::createOceanFisheye(): Invalid number of parameters: " << parametersArray->size() << ", expected: " << expectedParameterCount;
			return nullptr;
		}

		Scalars parameters;

		for (const JSONParser::JSONValue& parameterValue : *parametersArray)
		{
			if (!parameterValue.isNumber())
			{
				Log::warning() << "CameraCalibrationManager::createOceanFisheye(): Invalid parameter value in Ocean Fisheye camera model";
				return nullptr;
			}

			parameters.push_back(parameterValue.number());
		}

		ocean_assert(parameters.size() == expectedParameterCount);

		return std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, parameterConfiguration, parameters.data()));
	}

	// let's see whether the camera model is defined via a simple horizontal field of view

	const double* numberFovX = modelObject.numberFromObject("fovx");

	if (numberFovX != nullptr)
	{
		const double fovX = *numberFovX;

		if (fovX <= NumericD::deg2rad(1))
		{
			Log::warning() << "CameraCalibrationManager::createOceanFisheye(): Invalid horizontal field of view: " << fovX << " rad (" << NumericD::rad2deg(fovX) << " deg)";
			return nullptr;
		}

		return std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, Scalar(fovX)));
	}

	// let's see whether the camera model is defined via a simple vertical field of view

	const double* numberFovY = modelObject.numberFromObject("fovy");

	if (numberFovY != nullptr)
	{
		const double fovY = *numberFovY;

		if (fovY <= NumericD::deg2rad(1))
		{
			Log::warning() << "CameraCalibrationManager::createOceanFisheye(): Invalid vertical field of view: " << fovY << " rad (" << NumericD::rad2deg(fovY) << " deg)";
			return nullptr;
		}

		const Scalar fovX = Camera::fovY2X(Scalar(fovY), Scalar(width) / Scalar(height));

		return std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, fovX));
	}

	ocean_assert(false && "Invalid Ocean Fisheye camera model");
	return nullptr;
}

}

}
