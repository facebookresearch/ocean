/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR_WIN_DETECTOR_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR_WIN_DETECTOR_H

#include "application/ocean/demo/cv/ApplicationDemoCV.h"

#include "ocean/base/CommandArguments.h"

#include "ocean/cv/calibration/MetricCalibrationBoard.h"

/**
 * @ingroup applicationdemocvcalibration
 * @defgroup applicationdemocvcalibrationdetectorwin Calibration Board Detector (Windows)
 * @{
 * The demo application demonstrates the detection of a calibration board.<br>
 * This application is platform dependent and is implemented for windows platforms.<br>
 * @}
 */

using namespace Ocean;

/**
 * Parses the command inputs and returns the calibration board to be used.
 * @param commandArguments The command arguments to be parsed
 * @return The resulting calibration board
 * @ingroup applicationdemocvcalibrationdetectorwin
 */
CV::Calibration::MetricCalibrationBoard determineCalibrationBoard(const CommandArguments& commandArguments);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBBRATION_DETECTOR_WIN_DETECTOR_H
