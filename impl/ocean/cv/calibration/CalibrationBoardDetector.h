/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_DETECTOR_H
#define META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_DETECTOR_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoardObservation.h"
#include "ocean/cv/calibration/MetricCalibrationBoard.h"
#include "ocean/cv/calibration/MarkerCandidate.h"
#include "ocean/cv/calibration/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/SpatialDistribution.h"

#include <deque>

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements the detector for the calibration board.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT CalibrationBoardDetector
{
	protected:

		/**
		 * Definition of a double-ended queue holding indices of continuous lines of marker points.
		 */
		using ContinuousLineIndices = std::deque<Index32>;

		/**
		 * Definition of a vector holding a binary mask indicating whether a marker point has been used already.
		 */
		using UsedPoints = std::vector<uint8_t>;

	public:

		/**
		 * Determines the marker candidates based on marker points detected in an image.
		 * @param points The marker points detected in the image, must be valid
		 * @param pointsDistributionArray The distribution array associated with the detected marker points, must be valid
		 * @param maximalDistance The maximal distance between two neighboring marker points, in pixels, with range (0, infinity)
		 * @param maxDistancePercentage The percentage defining the maximal distance between the predicted point and the actual point based on the distance between previous point and predicted point, with range [0, infinity)
		 * @param markerCandidates The resulting marker candidates with valid boarder indices
		 * @return True, if succeeded
		 */
		static bool determineMarkerCandidates(const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maximalDistance, const Scalar maxDistancePercentage, MarkerCandidates& markerCandidates);

		/**
		 * Determines the initial camera field-of-view based on several marker candidates with valid border indices.
		 * Each marker candidate and it's potential 4-neighborhood is checked against several field-of-views.<br>
		 * The field-of-view resulting in the lowest projection error is used.
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param points The marker points detected in the image, must be valid
		 * @param markerCandidates The marker candidates from which the initial field-of-view will be determined, must have valid border indices, at least one
		 * @param anyCameraType The type of the camera profile to be used, must be valid
		 * @param fovX The resulting initial horizontal field-of-view, in radian, with range [minFovX, maxFovX]
		 * @param minFovX The minimal horizontal field-of-view to be used, in radian, with range (0, maxFovX)
		 * @param maxFovX The maximal horizontal field-of-view to be used, in radian, with range (minFovX, PI)
		 * @param steps The number of steps to be used for the field-of-view search, with range [2, infinity)
		 * @return True, if succeeded
		 */
		static bool determineInitialCameraFieldOfView(const unsigned int width, const unsigned height, const Points& points, const MarkerCandidates& markerCandidates, const AnyCameraType anyCameraType, Scalar& fovX, const Scalar minFovX = Numeric::deg2rad(20), const Scalar maxFovX = Numeric::deg2rad(175), const size_t steps = 20);

		/**
		 * Determines the camera pose for a marker candidate.
		 * This function is intended for marker candidates with only valid border indices.<br>
		 * The resulting camera pose is the transformation between camera and the marker candidate.<br>
		 * The marker is defined in the xz-plane, the origin is in the center of the marker, and marker points have a distance of 1 to each other.
		 * @param camera The camera profile to be used, must be valid
		 * @param markerCandidate The marker candidate for which the camera pose will be determined, must have valid border indices
		 * @param points The marker points detected in the image
		 * @param markerCandidate_T_camera The resulting camera pose in relation to the marker candidate
		 * @param maximalProjectionError The maximal projection error between the 3D object points and the 2D image points, in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool determineCameraPoseForMarker(const AnyCamera& camera, const MarkerCandidate& markerCandidate, const Points& points, HomogenousMatrix4& markerCandidate_T_camera, const Scalar maximalProjectionError);

		/**
		 * Determines the remaining unknown point indices of a marker candidate.
		 * This function is intended for marker candidates with only valid border indices.
		 * @param camera The camera profile to be used, must be valid
		 * @param markerCandidate_T_camera The known transformation between camera and the marker candidate, must be valid
		 * @param markerCandidate The marker candidate to update
		 * @param points The marker points detected in the image
		 * @param pointsDistributionArray The distribution array associated with the detected marker points, must be valid
		 * @param maximalProjectionError The maximal projection error between the 3D object points and the 2D image points, in pixel, with range [0, infinity)
		 * @return True, if all remaining (9) point indices could be determined
		 */
		static bool determineRemainingMarkerPointIndices(const AnyCamera& camera, const HomogenousMatrix4& markerCandidate_T_camera, MarkerCandidate& markerCandidate, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maximalProjectionError);

		/**
		 * Determines the marker candidates most suitable to start the detection of the calibration board.
		 * Marker candidates with most neighbors located in the center of the image are preferred due to fisheye cameras.
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param markerCandidates The marker candidates from which the best candidates will be determined, at least one
		 * @param points The marker points detected in the image, must be valid
		 * @param bestMarkerCandidateIndices The resulting indices of the best marker candidates, with range [0, markerCandidates.size() - 1]
		 * @return True, if succeeded
		 */
		static bool determineStartMarkerCandidates(const unsigned int width, const unsigned int height, const MarkerCandidates& markerCandidates, const Points& points, Indices32& bestMarkerCandidateIndices);

		/**
		 * Detects a calibration board in an image.
		 * @param camera The camera profile to be used, must be valid
		 * @param yFrame The image in which the calibration board will be detected, must be valid
		 * @param calibrationBoard The calibration board to be detected, must be valid
		 * @param observation The resulting observation of the calibration board
		 * @param maximalProjectionError The maximal projection error between the 3D object points and the 2D image points, in pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectCalibrationBoard(const AnyCamera& camera, const Frame& yFrame, const MetricCalibrationBoard& calibrationBoard, CalibrationBoardObservation& observation, const Scalar maximalProjectionError, Worker* worker = nullptr);

	protected:

		/**
		 * Determines a continuous line of marker points with same sign.
		 * @param pointIndexA The index of the first marker point, located on the line, with range [0, points.size() - 1]
		 * @param pointIndexB The index of the second neighbor marker point, located on the line, with range [0, points.size() - 1]
		 * @param points The marker points detected in the image
		 * @param usedPoints Binary mask indicating whether a marker point has been used already, one for each point, only points not yet used will be used for the determination of the continuous line
		 * @param pointsDistributionArray The distribution array associated with the detected marker points, must be valid
		 * @param lineIndices The resulting indices of the marker points located on the continuous line
		 * @param maxDistancePercentage The percentage defining the maximal distance between the predicted point and the actual point based on the distance between previous point and predicted point, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool determineContinuousLine(const size_t pointIndexA, const size_t pointIndexB, const Points& points, const UsedPoints& usedPoints, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, ContinuousLineIndices& lineIndices, const Scalar maxDistancePercentage);

		/**
		 * Determines a closed rectangle of marker points (the 16 marker points of the border of a marker).
		 * @param lineIndices The indices of the marker points located on a known continuous line with 5 points, will be extended by the remaining 11 points of the rectangle
		 * @param points The marker points detected in the image
		 * @param usedPoints Binary mask indicating whether a marker point has been used already, one for each point, only points not yet used will be used for the determination of the rectangle
		 * @param pointsDistributionArray The distribution array associated with the detected marker points, must be valid
		 * @param maxDistancePercentage The percentage defining the maximal distance between the predicted point and the actual point based on the distance between previous point and predicted point, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool determineClosedRectangle(ContinuousLineIndices& lineIndices, const Points& points, const UsedPoints& usedPoints, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maxDistancePercentage);

		/**
		 * Determines the neighborhood relationship in marker candidates.
		 * @param markerCandidates The marker candidates for which the neighborhood relationship will be determined
		 * @param points The marker points detected in the image
		 * @param maxDistancePercentage The percentage defining the maximal distance between the predicted point and the actual point based on the distance between previous point and predicted point, with range [0, infinity)
		 */
		static void determineMarkerCandidateNeighborhood(MarkerCandidates& markerCandidates, const Points& points, const Scalar maxDistancePercentage);
};

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CALIBRATION_BOARD_DETECTOR_H
