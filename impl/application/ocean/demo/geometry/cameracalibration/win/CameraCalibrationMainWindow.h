/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_CAMERA_CALIBRATION_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_CAMERA_CALIBRATION_WIN_MAINWINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/CalibrationPatternDetector.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemogeometrycameracalibrationwin
 */
class CameraCalibrationMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	protected:

		/**
		 * Definition of a vector holding 2D vectors.
		 */
		typedef std::vector<Vectors2> Vectors2Groups;

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 */
		CameraCalibrationMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~CameraCalibrationMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Executes the next calibration step.
		 * @return True, if succeeded
		 */
		bool calibration();

	protected:

		/// Undistorted frame window.
		Platform::Win::BitmapWindow undistortWindow_;

		/// Undistorted frame window.
		Platform::Win::BitmapWindow undistortGridWindow_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// Calibration pattern detector.
		CV::Detector::CalibrationPatternDetector calibrationPatternDetector_;

		/// The horizontal (width) size of the calibration pattern, in meter.
		Scalar calibrationPatternHorizontalSize_ = Scalar(0.1791);

		/// The vertical (height) size of the calibration pattern, in meter.
		Scalar calibrationPatternVerticalSize_ = Scalar(0.2620);

		/// Set of detected patterns.
		Geometry::CameraCalibration::Patterns calibrationPatterns_;

		/// The perfect/default camera profile.
		PinholeCamera perfectCamera_;

		/// The most recent camera profile.
		PinholeCamera calibrationCamera_;

		/// Number of poses to be used for one calibration execution.
		unsigned int numberPosesToUse_ = 10u;

		/// The set of object points representing the corners of the calibration pattern, horizontal row aligned.
		Vectors3 calibrationPatternObjectPoints_;

		/// The group of 2D image points corresponding to the object points.
		Vectors2Groups calibrationPatternImagePointsGroups_;

		/// The camera poses corresponding with the image groups of image points.
		HomogenousMatrices4 calibrationPatternPoses_;

		/// The random generator object.
		RandomGenerator randomGenerator_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_GEOMETRY_CAMERA_CALIBRATION_WIN_MAINWINDOW_H
