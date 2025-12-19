/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CALIBRATION_DEBUG_ELEMENTS_H
#define META_OCEAN_CV_CALIBRATION_CALIBRATION_DEBUG_ELEMENTS_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoardObservation.h"
#include "ocean/cv/calibration/Point.h"
#include "ocean/cv/calibration/PointDetector.h"
#include "ocean/cv/calibration/MarkerCandidate.h"
#include "ocean/cv/calibration/MetricCalibrationBoard.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements debug elements for the calibration library.
 * Debug elements allow to visualize results and intermediate steps from calibration components and algorithms.
 * @ingroup cvcalibration
 */
class CalibrationDebugElements final :
	public DebugElements,
	public Singleton<CalibrationDebugElements>
{
	friend class Singleton<CalibrationDebugElements>;

	public:

		/// True, in case debugging is allowed and debugging code will be included into the binary; False, to disable debugging code
		static constexpr bool allowDebugging_ = true;

		/**
		 * Definition of several debug elements.
		 */
		enum ElementId : uint32_t
		{
			/// An invalid element id.
			EI_INVALID = 0u,

			/// PointDetector: Image visualizing the detected point candidates.
			EI_POINT_DETECTOR_POINTS_CANDIDATES,
			/// PointDetector: Image visualizing the detected points without non-maximum suppression.
			EI_POINT_DETECTOR_POINTS_NON_SUPPRESSED,
			/// PointDetector: Image visualizing the detected points after non-maximum suppression.
			EI_POINT_DETECTOR_POINTS_SUPPRESSED,
			/// PointDetector:: Image visualizing the optimization process.
			EI_POINT_DETECTOR_POINTS_OPTIMIZATION,
			/// PointDetector: Image visualizing the point patterns used for optimization.
			EI_POINT_DETECTOR_POINTS_OPTIMIZATION_POINT_PATTERNS,
			/// PointDetector: Image visualizing the detected points after optimization.
			EI_POINT_DETECTOR_POINTS_OPTIMIZED,

			/// CameraCalibrator: Image visualizing the detected points.
			EI_CAMERA_CALIBRATOR_DETECTED_POINTS,
			/// CameraCalibrator: Image visualizing the detected marker candidates with valid board indices and sign.
			EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES,
			/// CameraCalibrator: Image visualizing the detected marker candidates with valid ids.
			EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS,
			/// CameraCalibrator: Image visualizing the detected marker candidates with valid marker coordinates.
			EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES,
			/// CameraCalibrator: Image visualizing the initial camera pose based on valid marker candidates.
			EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES,
			/// CameraCalibrator: Image visualizing the initial camera pose based on valid marker candidates after the camera profile has been optimized.
			EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES_OPTIMIZED_CAMERA,
			/// CameraCalibrator: Image visualizing the optimized camera pose using additional points.
			EI_CAMERA_CALIBRATOR_OPTIMIZED_CAMERA_POSE_WITH_ADDITIONAL_POINTS,
			/// CameraCalibrator: Image visualizing the first iteration while the initial camera fov is optimized.
			EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_0,
			/// CameraCalibrator: Image visualizing the second iteration while the initial camera fov is optimized.
			EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_1,
			/// CameraCalibrator: Image visualizing the third iteration while the initial camera fov is optimized.
			EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_2,
			/// CameraCalibrator: Image visualizing the optimized camera pose with additional correspondences, this is the final step in the per-image optimization.
			EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_0,
			/// CameraCalibrator: Image visualizing the optimized camera pose with additional correspondences after the final refinement in the per-image optimization.
			EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_0_FINAL,
			/// CameraCalibrator: Image visualizing the optimized camera pose with additional correspondences during the second calibration stage.
			EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_1,
			/// CameraCalibrator: Image visualizing the optimized camera pose with additional correspondences after the final refinement during the second calibration stage.
			EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_1_FINAL,
			/// CameraCalibrator: Image visualizing the calibration board with detected and undetected points.
			EI_CAMERA_CALIBRATOR_CALIBRATION_BOARD,

			/// CameraCalibrator: Image visualizing the camera boundary of the final camera profile in the per-image optimization.
			EI_CAMERA_CALIBRATOR_CAMERA_BOUNDARY,

			/// CameraCalibrator: Image visualizing the coverage of the camera area.
			EI_CAMERA_CALIBRATOR_COVERAGE,
			/// CameraCalibrator: Image visualizing the camera distortion with a grid.
			EI_CAMERA_CALIBRATOR_DISTORTION_GRID,
			/// CameraCalibrator: Image visualizing the camera distortion with displacement vectors.
			EI_CAMERA_CALIBRATOR_DISTORTION_VECTORS,
			/// CameraCalibrator: Image showing the final average projection error per bin.
			EI_CAMERA_CALIBRATOR_PROJECTION_ERROR
		};

	public:

		/**
		 * Updates the point element visualizing the detected points without non-maximum suppression.
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param points The points to be visualized before non-maximum suppression
		 */
		inline void updatePointDetectorPointsNonSuppressed(const Frame& yFrame, const Points& points);

		/**
		 * Updates the point element visualizing the detected points after non-maximum suppression.
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param points The points to be visualized after non-maximum suppression
		 */
		inline void updatePointDetectorPointsSuppressed(const Frame& yFrame, const Points& points);

		/**
		 * Updates the point element visualizing the optimization process.
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param pointsMoveFrom The starting point of all points which moved too much during the optimization process
		 * @param pointsMoveTo The end point of all points which moved too much during the optimization process
		 * @param pointsFlip The points with flipped sign during the optimization process
		 * @param pointsFailed The points which failed during the optimization process
		 */
		void updatePointDetectorPointsOptimization(const Frame& yFrame, const Vectors2& pointsMovedFrom, const Vectors2& pointsMovedTo, const Vectors2& pointsFlipped, const Vectors2& pointsFailed);

		/**
		 * Updates the point element visualizing the point patterns used for optimization.
		 * @param pointPatterns The point patterns to be visualized
		 * @param imageSize The size of the resulting image with the point patterns, in pixel
		 */
		void updatePointDetectorPointsOptimizationPointPatterns(const PointDetector::PointPatterns& pointPatterns, const unsigned int imageSize);

		/**
		 * Updates the point element visualizing the detected points after optimization.
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param points The points to be visualized after optimization
		 */
		inline void updatePointDetectorPointsOptimized(const Frame& yFrame, const Points& points);

		/**
		 * Updates a camera calibrator element visualizing the detected points.
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param points The points to be visualized
		 */
		inline void updateCameraCalibratorDetectedPoints(const Frame& yFrame, const Points& points);

		/**
		 * Updates a camera calibrator element visualizing the detected marker candidates.
		 * @param elementId The id of the debug element to update, must be valid
		 * @param yFrame The frame in which the points and marker candidates have been detected, must be valid
		 * @param points The points associated with the marker candidates
		 * @param markerCandidates The marker candidates to be visualized
		 */
		void updateCameraCalibratorMarkerCandidates(const ElementId elementId, const Frame& yFrame, const Points& points, const MarkerCandidates& markerCandidates);

		/**
		 * Updates a camera calibrator element visualizing the initial camera pose based on valid marker candidates.
		 * @param elementId The id of the debug element to update, must be valid
		 * @param yFrame The frame in which the points and marker candidates have been detected, must be valid
		 * @param points The points associated with the marker candidates
		 * @param markerCandidates The marker candidates
		 * @param usedInitialMarkerCandidateIndices The indices of the marker candidates which have been used to determine the initial camera pose, at least one
		 * @param calibrationBoard The calibration board which was detected, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param board_T_camera The camera pose transforming camera to board, must be valid
		 */
		void updateCameraCalibratorInitialCameraPoseWithValidMarkerCandidates(const ElementId elementId, const Frame& yFrame, const Points& points, const MarkerCandidates& markerCandidates, const Indices32& usedInitialMarkerCandidateIndices, const MetricCalibrationBoard& calibrationBoard, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera);

		/**
		 * Updates a camera calibration element visualizing the 2D/3D correspondences which were used to determine the camera pose.
		 * @param elementId The id of the debug element to update, must be valid
		 * @param yFrame The frame in which the calibration board has been detected, must be valid
		 * @param calibrationBoard The calibration board which was detected, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param board_T_camera The camera pose transforming camera to board, must be valid
		 * @param objectPointIds The ids of the object points which have been used to determine the camera pose, at least one
		 * @param objectPoints The 3D object points which have been used to determine the camera pose, one for each object point id
		 * @param imagePoints The 2D image points which have been used to determine the camera pose, one for each object point
		 * @param text Optional text to be painted in the image
		 */
		void updateCameraCalibratorCorrespondences(const ElementId elementId, const Frame& yFrame, const MetricCalibrationBoard& calibrationBoard, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const CalibrationBoard::ObjectPointIds& objectPointIds, const Vectors3& objectPoints, const Vectors2& imagePoints, const std::string& text = std::string());

		/**
		 * Updates a camera calibrator element visualizing the calibration board with detected and undetected points.
		 * Detected points are shown with green lines connecting image points to projected object points.
		 * Undetected points from the calibration board are shown in red.
		 * @param yFrame The frame in which the calibration board has been detected, must be valid
		 * @param calibrationBoard The calibration board which was detected, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param board_T_camera The camera pose transforming camera to board, must be valid
		 * @param objectPointIds The ids of the object points which have been detected, at least one
		 * @param objectPoints The 3D object points which have been detected, one for each object point id
		 * @param imagePoints The 2D image points which have been detected, one for each object point
		 */
		void updateCameraCalibratorCalibrationBoard(const Frame& yFrame, const MetricCalibrationBoard& calibrationBoard, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const CalibrationBoard::ObjectPointIds& objectPointIds, const Vectors3& objectPoints, const Vectors2& imagePoints);

		/**
		 * Updates a camera calibrator element visualizing the camera boundary of a camera profile.
		 * @param cameraClipper The camera clipper to be used, must be valid
		 */
		void updateCameraCalibratorCameraBoundary(const AnyCameraClipper& cameraClipper);

		/**
		 * Updates a coverage element visualizing how many observations have been used per bin.
		 * @param observations The observations for which the coverage will be visualized, must be valid
		 * @param numberObservations The number of observations, with range [1, infinity)
		 * @param expectedCoverage The number of expected observations per bin, a bin with less observations will not be green, with range [1, infinity)
		 * @param showNumbers True, to paint the distortion errors in each bin
		 */
		void updateCameraCalibratorCoverage(const CalibrationBoardObservation* observations, const size_t numberObservations, const unsigned int expectedCoverage, const bool showNumbers = true);

		/**
		 * Updates a projection error element visualizing the average projection error per bin.
		 * @param observations The observations for which the projection error will be visualized, must be valid
		 * @param numberObservations The number of observations, with range [1, infinity)
		 * @param showNumbers True, to paint the distortion errors in each bin
		 */
		void updateCameraCalibratorProjectionError(const CalibrationBoardObservation* observations, const size_t numberObservations, const bool showNumbers = true);

		/**
		 * Updates a distortion element.
		 * @param elementId The id of the debug element to update, must be valid
		 * @param camera The camera profile defining the projection and distortion, must be valid
		 * @param useGrid True, to use a grid visualization; False, to use a vector visualization
		 */
		void updateDistortionElement(const ElementId elementId, const AnyCamera& camera, const bool useGrid);

		/**
		 * Updates a debug elements based on points.
		 * @param elementId The id of the debug element to update, must be valid
		 * @param yFrame The frame in which the points have been detected, must be valid
		 * @param points The points to be visualized
		 */
		void updatePointsElement(const ElementId elementId, const Frame& yFrame, const Points& points);
};

inline void CalibrationDebugElements::updatePointDetectorPointsNonSuppressed(const Frame& yFrame, const Points& points)
{
	updatePointsElement(EI_POINT_DETECTOR_POINTS_NON_SUPPRESSED, yFrame, points);
}

inline void CalibrationDebugElements::updatePointDetectorPointsSuppressed(const Frame& yFrame, const Points& points)
{
	updatePointsElement(EI_POINT_DETECTOR_POINTS_SUPPRESSED, yFrame, points);
}

inline void CalibrationDebugElements::updatePointDetectorPointsOptimized(const Frame& yFrame, const Points& points)
{
	updatePointsElement(EI_POINT_DETECTOR_POINTS_OPTIMIZED, yFrame, points);
}

inline void CalibrationDebugElements::updateCameraCalibratorDetectedPoints(const Frame& yFrame, const Points& points)
{
	updatePointsElement(EI_CAMERA_CALIBRATOR_DETECTED_POINTS, yFrame, points);
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CALIBRATION_DEBUG_ELEMENTS_H
