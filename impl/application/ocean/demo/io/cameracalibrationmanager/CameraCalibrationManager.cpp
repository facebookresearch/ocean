/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/cameracalibrationmanager/CameraCalibrationManager.h"

#include "ocean/base/CommandArguments.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/math/Numeric.h"

#include "ocean/platform/Utilities.h"

#ifdef _WINDOWS
	#include "ocean/platform/win/System.h"
#endif

using namespace Ocean;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_STANDARD | Messenger::OUTPUT_DEBUG_WINDOW));

	CommandArguments commandArguments("Demo application for camera calibration manager");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as calibration file");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("calibration", "c", "The filename of the camera calibration file (*.occ or *.json)");
	commandArguments.registerParameter("serial", "s", "Optional: the device serial number for device-specific calibrations");
	commandArguments.registerParameter("version", "v", "Optional: the device hardware version for device-specific calibrations");
	commandArguments.registerParameter("product", "p", "Optional: the device product name for device-specific calibrations");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

#ifdef _WINDOWS
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
#endif

	IO::File cameraCalibrationFile;

	Value calibrationValue;
	if (commandArguments.hasValue("calibration", &calibrationValue, false, 0u) && calibrationValue.isString())
	{
		const IO::File file(calibrationValue.stringValue());

		if (file.exists())
		{
			cameraCalibrationFile = file;
		}
		else
		{
			Log::warning() << "Warning: Specified calibration file '" << calibrationValue.stringValue() << "' does not exist.";
		}
	}

	if (cameraCalibrationFile.isNull())
	{
		const IO::File relativeFile("res/ocean/cv/calibration/camera_calibration.json");

		cameraCalibrationFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (cameraCalibrationFile.exists())
	{
		Log::info() << "Using calibration file: " << cameraCalibrationFile();
		IO::CameraCalibrationManager::get().registerCalibrations(cameraCalibrationFile());
	}
	else
	{
		Log::error() << "Error: No valid calibration file found.";
		return 1;
	}

	std::string serialValue;
	if (commandArguments.hasValue("serial", serialValue))
	{
		if (IO::CameraCalibrationManager::get().setDeviceSerial(serialValue))
		{
			Log::info() << "Device context set to serial: " << serialValue;
		}
		else
		{
			Log::warning() << "Failed to set device serial: " << serialValue;
		}
	}
	else
	{
		std::string versionValue;
		if (commandArguments.hasValue("version", versionValue))
		{
			if (IO::CameraCalibrationManager::get().setDeviceVersion(versionValue))
			{
				Log::info() << "Device context set to version: " << versionValue;
			}
			else
			{
				Log::warning() << "Failed to set device version: " << versionValue;
			}
		}
		else
		{
			std::string productValue;
			if (commandArguments.hasValue("product", productValue))
			{
				if (IO::CameraCalibrationManager::get().setDeviceProduct(productValue))
				{
					Log::info() << "Device context set to product: " << productValue;
				}
				else
				{
					Log::warning() << "Failed to set device product: " << productValue;
				}
			}
		}
	}

	const IndexPairs32 cameraResolutions =
	{
		{320u, 240u},
		{640u, 480u},
		{800u, 600u},
		{1280u, 720u},
		{1920u, 1080u}
	};

	const Strings cameraNames = IO::CameraCalibrationManager::get().cameraNames();

	Log::info() << "Available cameras: " << cameraNames.size();

	for (const std::string& cameraName : cameraNames)
	{
		if (cameraName != cameraNames.front())
		{
			Log::info() << " ";
		}

		Log::info() << cameraName << ":";

		for (const IndexPair32& cameraResolution : cameraResolutions)
		{
			Log::info() << "  " << cameraResolution.first << "x" << cameraResolution.second;

			IO::CameraCalibrationManager::CalibrationQuality calibrationQuality = IO::CameraCalibrationManager::CQ_UNKNOWN;
			SharedAnyCamera camera = IO::CameraCalibrationManager::get().camera(cameraName, cameraResolution.first, cameraResolution.second, &calibrationQuality);

			if (camera)
			{
				Log::info() << "Camera name: " << camera->name();

				std::string qualityString;

				switch (calibrationQuality)
				{
					case IO::CameraCalibrationManager::CQ_UNKNOWN:
						qualityString = "unknown";
						break;

					case IO::CameraCalibrationManager::CQ_INTERPOLATED:
						qualityString = "interpolated";
						break;

					case IO::CameraCalibrationManager::CQ_EXACT:
						qualityString = "exact";
						break;
				}

				ocean_assert(!qualityString.empty());

				Log::info() << "Calibration quality: " << qualityString;
			}
			else
			{
				Log::info() << "No calibration available";
			}
		}
	}

	return 0;
}
