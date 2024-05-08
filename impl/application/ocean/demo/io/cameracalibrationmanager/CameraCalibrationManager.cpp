/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/cameracalibrationmanager/CameraCalibrationManager.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/Numeric.h"

#include "ocean/platform/win/System.h"

using namespace Ocean;
using namespace Ocean::IO;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__)
	// main function on OSX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	CameraCalibrationManager::get().registerCalibrationFile(frameworkPath + std::string("/data/cameracalibration/cameracalibration.occ"));

	const std::string cameraNames[] =
	{
		"Logitech QuickCam for Notebooks Pro",
		"Logitech QuickCam for Notebooks Pro",
		"Logitech HD Pro Webcam C920",
		"Logitech HD Pro Webcam C920",
		"Unknown Camera"
	};

	const Indices32 cameraWidths = {320u, 640u, 800u, 1920u};
	const Indices32 cameraHeights = {240u, 480u, 600u, 1080u};

	ocean_assert(cameraWidths.size() == cameraHeights.size());

	for (size_t n = 0; n < cameraWidths.size(); ++n)
	{
		std::cout << "Calibration \"" << cameraNames[n] << "\" with resolution " << cameraWidths[n] << "x" << cameraHeights[n] << ":" << std::endl;

		CameraCalibrationManager::Quality calibrationQuality = CameraCalibrationManager::QUALITY_DEFAULT;
		const PinholeCamera pinholeCamera = CameraCalibrationManager::get().camera(cameraNames[n], cameraWidths[n], cameraHeights[n], &calibrationQuality);

		std::cout << "Horizontal field of view: " << String::toAString(Numeric::rad2deg(pinholeCamera.fovX()), 1u) << "deg, vertical field of view " << String::toAString(Numeric::rad2deg(pinholeCamera.fovY()), 1u) << "deg." << std::endl;

		if (calibrationQuality == CameraCalibrationManager::QUALITY_EXACT)
		{
			std::cout << "Quality: Exact." << std::endl << std::endl;
		}

		if (calibrationQuality == CameraCalibrationManager::QUALITY_INTERPOLATED)
		{
			std::cout << "Quality: Interpolated." << std::endl << std::endl;
		}

		if (calibrationQuality == CameraCalibrationManager::QUALITY_DEFAULT)
		{
			std::cout << "Quality: Default." << std::endl << std::endl;
		}
	}

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	return 0;
}
