/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_OFFLINE_CAMERA_CALIBRATOR_OFFLINE_MAIN_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_OFFLINE_CAMERA_CALIBRATOR_OFFLINE_MAIN_H

#include "application/ocean/demo/cv/calibration/ApplicationDemoCVCalibration.h"

#include "ocean/base/CommandArguments.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/cv/calibration/MetricCalibrationBoard.h"

/**
 * @ingroup applicationdemocvcalibration
 * @defgroup applicationdemocvcalibrationoffline Offline camera calibrator
 * @{
 * This console application allows to calibrate a camera based on several images of a calibration board.
 * The application is based on the Ocean Calibration Library.<br>
 * The application is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * Parses the command inputs and returns the calibration board to be used.
 * @param commandArguments The command arguments to be parsed
 * @param calibrationBoard The resulting calibration board
 * @ingroup applicationdemocvcalibrationoffline
 */
CV::Calibration::MetricCalibrationBoard determineCalibrationBoard(const CommandArguments& commandArguments);

/**
 * Writes the debug output information for the latest frame to the output directory.
 * @param inputFile The input file which was used
 * @param outputDirectory The output directory to which the debug information will be written
 * @ingroup applicationdemocvcalibrationoffline
 */
void writeDebugOutput(const IO::File& inputFile, const IO::Directory& outputDirectory);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_OFFLINE_CAMERA_CALIBRATOR_OFFLINE_MAIN_H
