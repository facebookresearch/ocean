/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_UTILITIES_H
#define META_OCEAN_CV_CALIBRATION_UTILITIES_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoardObservation.h"
#include "ocean/cv/calibration/MetricCalibrationBoard.h"
#include "ocean/cv/calibration/MetricSize.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements utility functions for calibration.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT Utilities
{
	public:

		/**
		 * Paints the outline of a calibration board.
		 * @param frame The frame to which the outline will be painted, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param board_T_camera The camera pose transforming camera to board, with default camera looking towards the negative z-space with y-axis upwards, must be valid
		 * @param calibrationBoard The calibration board for which the outline will be painted, must be valid
		 * @param color The color of the outline, must be valid
		 * @param thickness The thickness of the outline, possible values are {1, 3, 5, 7}
		 * @return True, if succeeded
		 */
		static bool paintCalibrationBoardOutline(Frame& frame, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const MetricCalibrationBoard& calibrationBoard, const uint8_t* color, const unsigned int thickness = 1u);

		/**
		 * Paints the observation information of a calibration board.
		 * @param frame The frame to which the observation information will be painted, must be valid
		 * @param calibrationBoard The calibration board for which the observation information will be painted, must be valid
		 * @param calibrationBoardObservation The observation information of the calibration board, must be valid
		 * @param drawCorrespondences True to draw the 2D/3D correspondences of the observation; False, to draw the projected 3D object points of all visible marker points
		 * @return True, if succeeded
		 */
		static bool paintCalibrationBoardObservation(Frame& frame, const MetricCalibrationBoard& calibrationBoard, const CalibrationBoardObservation& calibrationBoardObservation, const bool drawCorrespondences);

		/**
		 * Visualizes the distortion of a camera with a grid.
		 * @param camera The camera profile for which the distortion/undistortion is visualized, must be valid
		 * @param horizontalBins Number of horizontal bins, with range [1, camera.width() / 2u)
		 * @param verticalBins Number of vertical bins, with range [1, camera.height() / 2u)
		 * @param undistort True, to visualize the undistorted frame, otherwise the distorted frame is visualized
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting frame with visualized distortion/undistortion
		 */
		static Frame visualizeDistortionGrid(const AnyCamera& camera, const unsigned int horizontalBins, const unsigned int verticalBins, const bool undistort, Worker* worker = nullptr);

		/**
		 * Visualizes the distortion of a camera with displacement vectors.
		 * @param camera The camera profile for which the distortion is visualized, must be valid
		 * @param horizontalBins Number of horizontal bins, with range [1, camera.width() / 2]
		 * @param verticalBins Number of vertical bins, with range [1, camera.height() / 2]
		 * @return Resulting distortion frame with visualized distortion
		 */
		static Frame visualizeDistortionVectors(const AnyCamera& camera, const unsigned int horizontalBins, const unsigned int verticalBins);

		/**
		 * Parses the type of a calibration board from a string.
		 * The string must have the format 'calibrationBoard_<Id>_<HorizontalMarkers>x<VerticalMarkers> - e.g., 'calibrationBoard_0_8x13'
		 * @param calibrationBoardType The string with the calibration board type to parse
		 * @param calibrationBoard The resulting calibration board
		 * @return True, if succeeded
		 */
		static bool parseCalibrationBoard(const std::string& calibrationBoardType, CalibrationBoard& calibrationBoard);

		/**
		 * Parses the marker dimension from a string.
		 * The string must have the format '<HorizontalMarkers>x<VerticalMarkers>' - e.g., '8x13'
		 * @param markerDimension The string with the marker dimension to parse
		 * @param horizontalMarkers The resulting number of horizontal markers, with range [1, infinity)
		 * @param verticalMarkers The resulting number of vertical markers, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool parseMarkerDimension(const std::string& markerDimension, int32_t& horizontalMarkers, int32_t& verticalMarkers);

		/**
		 * Returns an ideal calibration board for a specific paper type.
		 * @param paperType The type of the paper to be used, must be valid
		 * @param id The id of the calibration board, with range [0, infinity)
		 * @param paperWidth Optional resulting width of the paper, nullptr if not of interest
		 * @param paperHeight Optional resulting height of the paper, nullptr if not of interest
		 * @return The calibration board optimized for the specified paper type, invalid if the calibration board could not be created
		 */
		static CalibrationBoard createBoardForPaper(const MetricSize::PaperType paperType, const unsigned int id, MetricSize* paperWidth = nullptr, MetricSize* paperHeight = nullptr);
};

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_MARKER_H
