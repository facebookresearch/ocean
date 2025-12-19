/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/CameraCalibrator.h"
#include "ocean/cv/calibration/CalibrationBoardDetector.h"
#include "ocean/cv/calibration/CalibrationDebugElements.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

CameraCalibrator::CameraCalibrator(const MetricCalibrationBoard& metricCalibrationBoard, const InitialCameraProperties& initialCameraProperties)
{
	ocean_assert(metricCalibrationBoard.isValid());

	if (metricCalibrationBoard.isValid())
	{
		metricCalibrationBoard_ = metricCalibrationBoard;
		initialCameraProperties_ = initialCameraProperties;

		calibrationStage_ = CS_DETERMINE_INITIAL_CAMERA_FOV;
	}
}

CameraCalibrator::ImageResult CameraCalibrator::handleImage(const size_t imageId, const Frame& frame, Worker* worker)
{
	ocean_assert(imageId != CalibrationBoardObservation::invalidImageId());

	ocean_assert(calibrationStage_ != CS_UNKNOWN);
	if (calibrationStage_ == CS_UNKNOWN)
	{
		return IR_ERROR;
	}

	ocean_assert(frame.isValid());
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		ocean_assert(false && "This should never happen!");
		return IR_ERROR;
	}

	if (!pointDetector_.detectPoints(yFrame_, worker))
	{
		return IR_ERROR;
	}

	const Points& points = pointDetector_.points();
	const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray = pointDetector_.pointsDistributionArray();

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorDetectedPoints(yFrame_, points);
	}

	constexpr Scalar maximalDistance = Scalar(50);
	constexpr Scalar maxDistancePercentage = Scalar(0.25);

	markerCandidates_.clear();
	if (!CalibrationBoardDetector::determineMarkerCandidates(points, pointsDistributionArray, maximalDistance, maxDistancePercentage, markerCandidates_))
	{
		return IR_ERROR;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorMarkerCandidates(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES, yFrame_, points, markerCandidates_);
	}

	if (markerCandidates_.empty())
	{
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	SharedAnyCamera initialCamera;

	if (calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV)
	{
		initialCamera = determineInitialCameraFieldOfView(yFrame_.width(), yFrame_.height(), points, markerCandidates_, initialCameraProperties_);

		if (!initialCamera)
		{
			return IR_BOARD_WAS_NOT_DETECTED;
		}
	}
	else
	{
		ocean_assert(camera_);
		initialCamera = camera_;
	}

	ocean_assert(initialCamera);

	for (size_t nMarkerCandidate = 0; nMarkerCandidate < markerCandidates_.size(); /*noop*/)
	{
		MarkerCandidate& markerCandidate = markerCandidates_[nMarkerCandidate];

		constexpr Scalar maximalProjectionError = Scalar(10);

		HomogenousMatrix4 markerCandidate_T_camera(false);
		if (CalibrationBoardDetector::determineCameraPoseForMarker(*initialCamera, markerCandidate, points, markerCandidate_T_camera, Scalar(maximalProjectionError)))
		{
			if (CalibrationBoardDetector::determineRemainingMarkerPointIndices(*initialCamera, markerCandidate_T_camera, markerCandidate, points, pointsDistributionArray, maximalProjectionError))
			{
				if (markerCandidate.determineMarkerId(points))
				{
					++nMarkerCandidate;
					continue;
				}
			}
		}

		MarkerCandidate::removeMarkerCandidate(markerCandidates_, nMarkerCandidate);
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorMarkerCandidates(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS, yFrame_, points, markerCandidates_);
	}

	if (markerCandidates_.empty())
	{
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	// now let's determine the correctness of the neighborhood of marker candidates

	for (size_t markerCandidateIndex = 0; markerCandidateIndex < markerCandidates_.size(); ++markerCandidateIndex)
	{
		CV::Calibration::MarkerCandidate& markerCandidate = markerCandidates_[markerCandidateIndex];

		if (markerCandidate.hasMarkerId())
		{
			ocean_assert(markerCandidate.neighbors().size() <= 4);

			if (markerCandidate.hasNeighborWithMarkerId(markerCandidates_))
			{
				CalibrationBoard::MarkerCoordinate markerCoordinate;
				CalibrationBoard::NeighborMarkerCoordinateMap neighborMarkerCoordinateMap;

				if (metricCalibrationBoard_.containsMarkerCandidateWithNeighborhood(markerCandidates_, markerCandidateIndex, &markerCoordinate, &neighborMarkerCoordinateMap))
				{
					if (neighborMarkerCoordinateMap.size() >= 1) // **TODO** expect more neighbors?
					{
						markerCandidate.setMarkerCoordinate(markerCoordinate);
					}
				}
			}
		}
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorMarkerCandidates(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES, yFrame_, points, markerCandidates_);
	}

	HomogenousMatrix4 board_T_initialCamera(false);
	Indices32 usedInitialMarkerCandidateIndices;

	if (!determineInitialPoseWithValidMarkerCandidates(*initialCamera, points, board_T_initialCamera, usedInitialMarkerCandidateIndices))
	{
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	ocean_assert(!usedInitialMarkerCandidateIndices.empty());

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorInitialCameraPoseWithValidMarkerCandidates(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES, yFrame_, points, markerCandidates_, usedInitialMarkerCandidateIndices, metricCalibrationBoard_, *initialCamera, board_T_initialCamera);
	}

	HomogenousMatrix4 board_T_camera = board_T_initialCamera;
	SharedAnyCamera camera = initialCamera;

	if (calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV)
	{
		constexpr OptimizationStrategy optimizationStrategy = OptimizationStrategy::OS_UP_TO_MAJOR_DISTORTION_AFTER_ANOTHER;

		camera = optimizeCamera(*initialCamera, board_T_initialCamera, points, usedInitialMarkerCandidateIndices, optimizationStrategy, &board_T_camera);
	}

	if (!camera)
	{
		ocean_assert(false && "This should never happen!");
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorInitialCameraPoseWithValidMarkerCandidates(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES_OPTIMIZED_CAMERA, yFrame_, points, markerCandidates_, usedInitialMarkerCandidateIndices, metricCalibrationBoard_, *camera, board_T_camera);
	}

	CalibrationBoard::ObjectPointIds objectPointIds;
	Vectors3 objectPoints;
	Vectors2 imagePoints;

	constexpr Scalar maximalProjectionError = Scalar(10); // **TODO** change depending on the stage

	board_T_initialCamera = board_T_camera;
	if (!optimizeCameraPoseWithAdditionalPointsFromMarkerCandidates(*camera, board_T_initialCamera, points, pointsDistributionArray, usedInitialMarkerCandidateIndices, maximalProjectionError, board_T_camera, objectPointIds, objectPoints, imagePoints))
	{
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorCorrespondences(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_CAMERA_POSE_WITH_ADDITIONAL_POINTS, yFrame_, metricCalibrationBoard_, *camera, board_T_camera, objectPointIds, objectPoints, imagePoints);
	}

	ocean_assert(board_T_camera.isValid());

	if (!board_T_camera.isValid() || objectPoints.size() < 25 * 4) // **TODO** find better threshold
	{
		return IR_BOARD_WAS_NOT_DETECTED;
	}

	CalibrationBoardObservation observation(imageId, camera, board_T_camera, std::move(objectPointIds), std::move(objectPoints), std::move(imagePoints));

	if (calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV)
	{
		constexpr OptimizationStrategy optimizationStrategy = OptimizationStrategy::OS_UP_TO_MAJOR_DISTORTION_AFTER_ANOTHER;

		HomogenousMatrices4 board_T_optimizedCameras;
		Scalar initialError;
		Scalar finalError;

		constexpr bool startWithFocalLength = true;
		constexpr Scalar distortionConstrainmentFactor = Scalar(2);

		SharedAnyCamera optimizedCamera = determinePreciseCamera(&observation, 1, optimizationStrategy, &board_T_optimizedCameras, Geometry::Estimator::ET_SQUARE, startWithFocalLength, distortionConstrainmentFactor, &initialError, &finalError);

		if (!optimizedCamera)
		{
			ocean_assert(false && "This should never happen!");
			return IR_BOARD_WAS_NOT_DETECTED;
		}

		HomogenousMatrix4 board_T_optimizedCamera = board_T_optimizedCameras.front();

		if constexpr (CalibrationDebugElements::allowDebugging_)
		{
			CalibrationDebugElements::get().updateCameraCalibratorCorrespondences(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_0, yFrame_, metricCalibrationBoard_, *optimizedCamera, board_T_optimizedCamera, observation.objectPointIds(), observation.objectPoints(), observation.imagePoints());
		}

		objectPointIds = CalibrationBoard::ObjectPointIds();
		objectPoints = Vectors3();
		imagePoints = Vectors2();

		objectPointIds.reserve(observation.objectPoints().size());
		objectPoints.reserve(observation.objectPoints().size());
		imagePoints.reserve(observation.objectPoints().size());

		constexpr Scalar tighterMaximalProjectionError = Scalar(5);

		if (!optimizeCameraPoseWithAdditionalPointsFromMarkerCandidates(*optimizedCamera, board_T_optimizedCamera, points, pointsDistributionArray, usedInitialMarkerCandidateIndices, tighterMaximalProjectionError, board_T_camera, objectPointIds, objectPoints, imagePoints))
		{
			return IR_BOARD_WAS_NOT_DETECTED;
		}

		if constexpr (CalibrationDebugElements::allowDebugging_)
		{
			CalibrationDebugElements::get().updateCameraCalibratorCorrespondences(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_1, yFrame_, metricCalibrationBoard_, *optimizedCamera, board_T_camera, objectPointIds, objectPoints, imagePoints);
		}

		observation = CalibrationBoardObservation(imageId, optimizedCamera, board_T_camera, std::move(objectPointIds), std::move(objectPoints), std::move(imagePoints));

		board_T_optimizedCameras.clear();
		optimizedCamera = determinePreciseCamera(&observation, 1, optimizationStrategy, &board_T_optimizedCameras, Geometry::Estimator::ET_SQUARE, startWithFocalLength, distortionConstrainmentFactor, &initialError, &finalError);

		if (!optimizedCamera)
		{
			ocean_assert(false && "This should never happen!");
			return IR_BOARD_WAS_NOT_DETECTED;
		}

		board_T_optimizedCamera = board_T_optimizedCameras.front();

		observation = CalibrationBoardObservation(optimizedCamera, board_T_optimizedCamera, std::move(observation));

		if constexpr (CalibrationDebugElements::allowDebugging_)
		{
			CalibrationDebugElements::get().updateCameraCalibratorCorrespondences(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_2, yFrame_, metricCalibrationBoard_, *observation.camera(), observation.board_T_camera(), observation.objectPointIds(), observation.objectPoints(), observation.imagePoints());
		}
	}

	constexpr Scalar tightMaximalProjectionError = 2;

	OptimizationStrategy optimizationStrategy = OptimizationStrategy::OS_INVALID;

	if (calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV)
	{
		optimizationStrategy = OptimizationStrategy::OS_UP_TO_MAJOR_DISTORTION_AFTER_ANOTHER;
	}
	else
	{
		optimizationStrategy = OptimizationStrategy::OS_ALL_PARAMETERS_AFTER_ANOTHER;
	}

	CalibrationBoard::ObjectPointIds additionalObjectPointIds;
	Vectors3 additionalObjectPoints;
	Vectors2 additionalImagePoints;

	size_t iterationIndex = 0;

	while (true)
	{
		additionalObjectPointIds.clear();
		additionalObjectPoints.clear();
		additionalImagePoints.clear();

		if (determineAdditionalCorrespondences(metricCalibrationBoard_, observation, points, pointsDistributionArray, additionalObjectPointIds, additionalObjectPoints, additionalImagePoints, tightMaximalProjectionError))
		{
			if (!additionalObjectPointIds.empty())
			{
				observation.addCorrespondences(additionalObjectPointIds, additionalObjectPoints, additionalImagePoints);

				if constexpr (CalibrationDebugElements::allowDebugging_)
				{
					CalibrationDebugElements::ElementId elementId = CalibrationDebugElements::EI_INVALID;

					if (optimizationStrategy == OptimizationStrategy::OS_ALL_PARAMETERS_AFTER_ANOTHER)
					{
						elementId = iterationIndex == 0 ? CalibrationDebugElements::EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_1 : CalibrationDebugElements::EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_1_FINAL;
					}
					else
					{
						elementId = iterationIndex == 0 ? CalibrationDebugElements::EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_0 : CalibrationDebugElements::EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES_0_FINAL;
					}

					CalibrationDebugElements::get().updateCameraCalibratorCorrespondences(elementId, yFrame_, metricCalibrationBoard_, *observation.camera(), observation.board_T_camera(), observation.objectPointIds(), observation.objectPoints(), observation.imagePoints(), "Additional points: " + String::toAString(additionalObjectPointIds.size()));
				}
			}
		}

		++iterationIndex;

		if (additionalObjectPointIds.empty())
		{
			if (optimizationStrategy == OptimizationStrategy::OS_ALL_PARAMETERS_AFTER_ANOTHER)
			{
				break;
			}

			optimizationStrategy = OptimizationStrategy::OS_ALL_PARAMETERS_AFTER_ANOTHER;
			iterationIndex = 0;
		}

		HomogenousMatrices4 board_T_optimizedCameras;
		Scalar initialError;
		Scalar finalError;

		constexpr bool startWithFocalLength = true;
		constexpr Scalar distortionConstrainmentFactor = Scalar(2);

		SharedAnyCamera optimizedCamera = determinePreciseCamera(&observation, 1, optimizationStrategy, &board_T_optimizedCameras, Geometry::Estimator::ET_SQUARE, startWithFocalLength, distortionConstrainmentFactor, &initialError, &finalError);

		if (!optimizedCamera)
		{
			ocean_assert(false && "This should never happen!");
			return IR_BOARD_WAS_NOT_DETECTED;
		}

		const HomogenousMatrix4& board_T_optimizedCamera = board_T_optimizedCameras.front();

		observation = CalibrationBoardObservation(optimizedCamera, board_T_optimizedCamera, std::move(observation));
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorCalibrationBoard(yFrame_, metricCalibrationBoard_, *observation.camera(), observation.board_T_camera(), observation.objectPointIds(), observation.objectPoints(), observation.imagePoints());
	}

	for (const CalibrationBoardObservation& existingObservation : observations_)
	{
		if (existingObservation.imageId() == observation.imageId())
		{
			ocean_assert(false && "This should never happen!");
			return IR_ERROR;
		}
	}

	observations_.emplace_back(std::move(observation));

	return IR_BOARD_WAS_DETECTED;
}

bool CameraCalibrator::finalize(bool& needAdditionalIteration)
{
	needAdditionalIteration = false;

	if (observations_.empty())
	{
		return false;
	}

	for (size_t iteration = 0; iteration < 2; ++iteration)
	{
		HomogenousMatrices4 board_T_optimizedCameras;

		bool startWithFocalLength = true;
		Scalar distortionConstrainmentFactor = Scalar(2);
		const CameraCalibrator::OptimizationStrategy optimizationStrategy = CameraCalibrator::OptimizationStrategy::OS_ALL_PARAMETERS_AFTER_ANOTHER;

		if (iteration == 1)
		{
			startWithFocalLength = false;
			distortionConstrainmentFactor = Scalar(2.5);
		}

		Scalar initialError = Numeric::maxValue();
		Scalar finalError = Numeric::maxValue();

		camera_ = CameraCalibrator::determinePreciseCamera(observations_.data(), observations_.size(), optimizationStrategy, &board_T_optimizedCameras, Geometry::Estimator::ET_SQUARE, startWithFocalLength, distortionConstrainmentFactor, &initialError, &finalError);

		if (!camera_)
		{
			cameraProjectionError_ = Numeric::maxValue();

			return false;
		}

		ocean_assert(board_T_optimizedCameras.size() == observations_.size());

		for (size_t nObservation = 0; nObservation < observations_.size(); ++nObservation)
		{
			CalibrationBoardObservation& observation = observations_[nObservation];

			observation.updateCamera(camera_, board_T_optimizedCameras[nObservation]);
		}

		cameraProjectionError_ = finalError;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		constexpr unsigned int expectedCoverage = 10u;

		CalibrationDebugElements::get().updateCameraCalibratorCoverage(observations_.data(), observations_.size(), expectedCoverage);
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorProjectionError(observations_.data(), observations_.size());
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateDistortionElement(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_DISTORTION_GRID, *camera_, true);
		CalibrationDebugElements::get().updateDistortionElement(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_DISTORTION_VECTORS, *camera_, false);
	}

	ocean_assert(calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV || calibrationStage_ == CS_CALIBRATE_CAMERA);

	if (calibrationStage_ == CS_DETERMINE_INITIAL_CAMERA_FOV)
	{
		observations_.clear();

		needAdditionalIteration = true;
	}

	calibrationStage_ = CS_CALIBRATE_CAMERA;

	return true;
}

size_t CameraCalibrator::numberCorrespondences() const
{
	size_t result = 0;

	for (const CalibrationBoardObservation& observation : observations_)
	{
		result += observation.objectPoints().size();
	}

	return result;
}

bool CameraCalibrator::determineInitialPoseWithValidMarkerCandidates(const AnyCamera& camera, const Points& points, HomogenousMatrix4& board_T_camera, Indices32& usedMarkerCandidateIndices) const
{
	ocean_assert(camera.isValid());

	// let's find the marker candidates with known marker coordinate, with as many valid neighbors as possible, and as central in the image as possible

	Indices32 markerCandidateIndices;
	if (!CalibrationBoardDetector::determineStartMarkerCandidates(yFrame_.width(), yFrame_.height(), markerCandidates_, points, markerCandidateIndices))
	{
		return false;
	}

	ocean_assert(!markerCandidateIndices.empty());

	// we start with the initial marker candidates as decided above, afterwards we add more candidates from the direct surrounding (with a growing region) to improve the accuracy of the camera pose
	// the region is growing from the center to address fisheye camera lenses in which the center of the image is less distorted than the outer regions

	reusableMarkerCoordinateUsageFrame_.set(FrameType((unsigned int)(metricCalibrationBoard_.xMarkers()), (unsigned int)(metricCalibrationBoard_.yMarkers()), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true, true);
	reusableMarkerCoordinateUsageFrame_.setValue(0xFFu);

	for (const Index32 markerCandidateIndex : markerCandidateIndices)
	{
		ocean_assert(markerCandidateIndex < markerCandidates_.size());
		const MarkerCandidate& markerCandidate = markerCandidates_[markerCandidateIndex];

		ocean_assert(markerCandidate.hasMarkerCoordinate());
		const CV::PixelPosition& markerCoordinate = markerCandidate.markerCoordinate();

		reusableMarkerCoordinateUsageFrame_.pixel<uint8_t>(markerCoordinate.x(), markerCoordinate.y())[0] = 0x00u;
	}

	Indices32 bestUsedMarkerCandidatesAccessorIndices;
	bestUsedMarkerCandidatesAccessorIndices.reserve(32);

	Indices32 iterationUsedMarkerCandidatesAccessorIndices;
	iterationUsedMarkerCandidatesAccessorIndices.reserve(32);

	CV::PixelPositions iterationNewMarkerCandidateCoordinates;
	iterationNewMarkerCandidateCoordinates.reserve(32);

	board_T_camera.toNull();

	size_t noImprovementIteration = 0;

	while (true)
	{
		iterationUsedMarkerCandidatesAccessorIndices.clear();

		const ConstArraySubsetAccessor<MarkerCandidate, Index32> markerCandidateAccessor(markerCandidates_, markerCandidateIndices);

		if (!metricCalibrationBoard_.determineCameraPose(camera, markerCandidateAccessor, points, randomGenerator_, board_T_camera, Scalar(10), &iterationUsedMarkerCandidatesAccessorIndices))
		{
			break;
		}

		ocean_assert(iterationUsedMarkerCandidatesAccessorIndices.size() >= 1);

		// let's see whether we can add a new marker candidate (which is a direct neighbor of the already used marker candidates)

		iterationNewMarkerCandidateCoordinates.clear();

		for (unsigned int y = 0u; y < reusableMarkerCoordinateUsageFrame_.height(); ++y)
		{
			for (unsigned int x = 0u; x < reusableMarkerCoordinateUsageFrame_.width(); ++x)
			{
				if (reusableMarkerCoordinateUsageFrame_.constpixel<uint8_t>(x, y)[0] == 0xFF)
				{
					if (CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4<true, uint8_t>(reusableMarkerCoordinateUsageFrame_.constdata<uint8_t>(), reusableMarkerCoordinateUsageFrame_.width(), reusableMarkerCoordinateUsageFrame_.height(), reusableMarkerCoordinateUsageFrame_.paddingElements(), CV::PixelPosition(x, y), 0x00u))
					{
						iterationNewMarkerCandidateCoordinates.emplace_back(x, y);
					}
				}
			}
		}

		for (const CV::PixelPosition& iterationNewMarkerCandidateCoordinate : iterationNewMarkerCandidateCoordinates)
		{
			reusableMarkerCoordinateUsageFrame_.pixel<uint8_t>(iterationNewMarkerCandidateCoordinate.x(), iterationNewMarkerCandidateCoordinate.y())[0] = 0x00u;
		}

		markerCandidateIndices.clear();

		for (size_t markerCandidateIndex = 0; markerCandidateIndex < markerCandidates_.size(); ++markerCandidateIndex) // TODO perhaps only use neighbor candidates of used marker candidates (currently we simply grow the region)
		{
			const CV::Calibration::MarkerCandidate& markerCandidate = markerCandidates_[markerCandidateIndex];

			if (markerCandidate.hasMarkerCoordinate())
			{
				const CV::PixelPosition& markerCoordinate = markerCandidate.markerCoordinate();

				if (reusableMarkerCoordinateUsageFrame_.constpixel<uint8_t>(markerCoordinate.x(), markerCoordinate.y())[0] == 0x00)
				{
					markerCandidateIndices.push_back(Index32(markerCandidateIndex));
				}
			}
		}

		if (bestUsedMarkerCandidatesAccessorIndices.size() < iterationUsedMarkerCandidatesAccessorIndices.size())
		{
			std::swap(bestUsedMarkerCandidatesAccessorIndices, iterationUsedMarkerCandidatesAccessorIndices);

			noImprovementIteration = 0;
		}
		else
		{
			++noImprovementIteration;
		}

		if (noImprovementIteration == 2)
		{
			// we could not improve the marker candidate selection within the last two iterations, so we use the best selection

			ocean_assert(usedMarkerCandidateIndices.empty());
			usedMarkerCandidateIndices.clear();

			for (const Index32& usedMarkerCandidateAccessorIndex : bestUsedMarkerCandidatesAccessorIndices)
			{
				ocean_assert(usedMarkerCandidateAccessorIndex < markerCandidateIndices.size());
				usedMarkerCandidateIndices.push_back(markerCandidateIndices[usedMarkerCandidateAccessorIndex]);
			}

			ocean_assert(board_T_camera.isValid());

			return true;
		}
	}

	return false;
}

bool CameraCalibrator::optimizeCameraPoseWithAdditionalPointsFromMarkerCandidates(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Indices32& validMarkerCandidateIndices, const Scalar maximalProjectionError, HomogenousMatrix4& board_T_optimizedCamera, CalibrationBoard::ObjectPointIds& objectPointIds, Vectors3& objectPoints, Vectors2& imagePoints) const
{
	ocean_assert(camera.isValid());
	ocean_assert(board_T_camera.isValid());
	ocean_assert(maximalProjectionError >= 0);
	ocean_assert(!validMarkerCandidateIndices.empty());

	if (!board_T_camera.isValid())
	{
		return false;
	}

	ocean_assert(validMarkerCandidateIndices.size() != metricCalibrationBoard_.markers()); // **TODO** handle this case, could be extremely rare

	// let's try to find individual points which are not part of the used markers

	reusableMarkerCoordinateUsageFrame_.set(FrameType((unsigned int)(metricCalibrationBoard_.xMarkers()), (unsigned int)(metricCalibrationBoard_.yMarkers()), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true, true);
	reusableMarkerCoordinateUsageFrame_.setValue(0xFFu);

	for (const Index32& usedInitialMarkerCandidateIndex : validMarkerCandidateIndices)
	{
		ocean_assert(usedInitialMarkerCandidateIndex < markerCandidates_.size());
		const CV::Calibration::MarkerCandidate& usedMarkerCandidate = markerCandidates_[usedInitialMarkerCandidateIndex];

		ocean_assert(usedMarkerCandidate.hasMarkerCoordinate());
		const CV::PixelPosition& usedMarkerCoordinate = usedMarkerCandidate.markerCoordinate();

		reusableMarkerCoordinateUsageFrame_.pixel<uint8_t>(usedMarkerCoordinate.x(), usedMarkerCoordinate.y())[0] = 0x00u;
	}

	const ConstArraySubsetAccessor<MarkerCandidate, Index32> useMarkerCandidates(markerCandidates_, validMarkerCandidateIndices);

	CV::PixelPositions additionalMarkerCoordinates;

	HomogenousMatrix4 board_T_internalCamera(board_T_camera);

	while (true)
	{
		bool foundNewCoordinate = false;

		for (unsigned int y = 0u; y < reusableMarkerCoordinateUsageFrame_.height(); ++y)
		{
			for (unsigned int x = 0u; x < reusableMarkerCoordinateUsageFrame_.width(); ++x)
			{
				if (reusableMarkerCoordinateUsageFrame_.constpixel<uint8_t>(x, y)[0] == 0xFF)
				{
					if (CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4<true, uint8_t>(reusableMarkerCoordinateUsageFrame_.constdata<uint8_t>(), reusableMarkerCoordinateUsageFrame_.width(), reusableMarkerCoordinateUsageFrame_.height(), reusableMarkerCoordinateUsageFrame_.paddingElements(), CV::PixelPosition(x, y), 0x00u))
					{
						additionalMarkerCoordinates.emplace_back(x, y);

						foundNewCoordinate = true;
					}
				}
			}
		}

		if (!foundNewCoordinate)
		{
			break;
		}

		for (const CV::PixelPosition& additionalMarkerCoordiate : additionalMarkerCoordinates)
		{
			reusableMarkerCoordinateUsageFrame_.pixel<uint8_t>(additionalMarkerCoordiate.x(), additionalMarkerCoordiate.y())[0] = 0x00u;
		}

		HomogenousMatrix4 board_T_internalOptimizedCamera(false);

		objectPointIds.clear();
		objectPoints.clear();
		imagePoints.clear();
		if (metricCalibrationBoard_.optimizeCameraPose(camera, board_T_internalCamera, useMarkerCandidates, additionalMarkerCoordinates, points, pointsDistributionArray, board_T_internalOptimizedCamera, maximalProjectionError, &objectPointIds, &imagePoints, &objectPoints))
		{
			board_T_internalCamera = board_T_internalOptimizedCamera;
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	ocean_assert(board_T_internalCamera.isValid());
	ocean_assert(objectPoints.size() >= validMarkerCandidateIndices.size() * Marker::numberPoints());
	ocean_assert(objectPoints.size() == imagePoints.size());

	board_T_optimizedCamera = board_T_internalCamera;

	return true;
}

bool CameraCalibrator::determineAdditionalCorrespondences(const MetricCalibrationBoard& calibrationBoard, const CalibrationBoardObservation& observation, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, CalibrationBoard::ObjectPointIds& additionalObjectPointIds, Vectors3& additionalObjectPoints, Vectors2& additionalImagePoints, const Scalar maximalProjectionError)
{
	ocean_assert(observation.isValid());
	ocean_assert(maximalProjectionError >= 0);

	const AnyCameraClipper cameraClipper(observation.camera());

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updateCameraCalibratorCameraBoundary(cameraClipper);
	}

	Indices32 usedMarkerPoints(calibrationBoard.markers(), 0u);

	constexpr Index32 bitsAllIndicesUsed = (1u << Marker::numberPoints()) - 1u; // all 25 bits set

	for (const CalibrationBoard::ObjectPointId& objectPointId : observation.objectPointIds())
	{
		const CalibrationBoard::MarkerCoordinate& markerCoordinate = objectPointId.markerCoordinate();

		const size_t& indexInMarker = objectPointId.indexInMarker();
		ocean_assert(indexInMarker < 25);

		const size_t markerIndex = markerCoordinate.y() * calibrationBoard.xMarkers() + markerCoordinate.x();
		ocean_assert(markerIndex < usedMarkerPoints.size());

		uint32_t& usedIndicesInMarker = usedMarkerPoints[markerIndex];

		const uint32_t indexInMarkerBit = uint32_t(1) << indexInMarker;
		ocean_assert((usedIndicesInMarker & indexInMarkerBit) == 0u);

		usedIndicesInMarker |= indexInMarkerBit;

		ocean_assert(usedIndicesInMarker >= 1u && usedIndicesInMarker <= bitsAllIndicesUsed);
	}

	const HomogenousMatrix4 flippedCamera_T_board(Camera::standard2InvertedFlipped(observation.board_T_camera()));

	ocean_assert(additionalObjectPointIds.empty());
	ocean_assert(additionalObjectPoints.empty());
	ocean_assert(additionalImagePoints.empty());

	additionalObjectPointIds.clear();
	additionalObjectPoints.clear();
	additionalImagePoints.clear();

	size_t markerIndex = 0;

	for (unsigned int yMarker = 0u; yMarker < calibrationBoard.yMarkers(); ++yMarker)
	{
		for (unsigned int xMarker = 0u; xMarker < calibrationBoard.xMarkers(); ++xMarker)
		{
			const Index32& usedIndicesInMarker = usedMarkerPoints[markerIndex++];

			if (usedIndicesInMarker == bitsAllIndicesUsed)
			{
				// the marker is already entirely used, so we can skip it
				continue;
			}

			const CalibrationBoard::MarkerCoordinate markerCoordinate(xMarker,  yMarker);

			const CalibrationBoard::BoardMarker& boardMarker = calibrationBoard.marker(markerCoordinate);

			for (Index32 indexInMarker = 0; indexInMarker < Marker::numberPoints(); ++indexInMarker)
			{
				const Index32 indexInMarkerBit = uint32_t(1) << indexInMarker;

				if ((usedIndicesInMarker & indexInMarkerBit) != 0u)
				{
					continue;
				}

				const Vector3 objectPoint = calibrationBoard.objectPoint(markerCoordinate, indexInMarker); // **TODO** not most efficient way to determine 3D object point

				Vector2 projectedObjectPoint;

				if (!cameraClipper.projectToImageIF(flippedCamera_T_board, objectPoint, &projectedObjectPoint))
				{
					// the 3D object point projects outside of the camera image
					continue;
				}

				Index32 closestPointIndex = Index32(-1);
				Index32 secondClosestPointIndex = Index32(-1);

				Scalar closestSqrDistance = Numeric::maxValue();
				Scalar secondClosestSqrDistance = Numeric::maxValue();

				if (!PointDetector::closestPoints(projectedObjectPoint, pointsDistributionArray, points, closestPointIndex, secondClosestPointIndex, closestSqrDistance, secondClosestSqrDistance))
				{
					continue;
				}

				ocean_assert(closestPointIndex < points.size());

				if (closestSqrDistance > Numeric::sqr(maximalProjectionError))
				{
					continue;
				}

				if (secondClosestSqrDistance <= closestSqrDistance * Numeric::sqr(Scalar(2))) // we want a unique match
				{
					continue;
				}

				// let's verify the sign of the point

				const Point& closestPoint = points[closestPointIndex];

				if (closestPoint.sign() != boardMarker.pointSign<true>(indexInMarker))
				{
					continue;
				}

				additionalObjectPointIds.emplace_back(markerCoordinate, indexInMarker);
				additionalObjectPoints.emplace_back(objectPoint);
				additionalImagePoints.emplace_back(closestPoint.observation());
			}
		}
	}

	ocean_assert(additionalObjectPointIds.size() == additionalObjectPoints.size());
	ocean_assert(additionalObjectPointIds.size() == additionalImagePoints.size());

#ifdef OCEAN_DEBUG
	{
		// let's ensure that we do not have any additional object point which is already part of the observation

		CalibrationBoard::ObjectPointIdSet objectPointIdSet(observation.objectPointIds().cbegin(), observation.objectPointIds().cend());
		ocean_assert(objectPointIdSet.size() == observation.objectPointIds().size());

		for (const CalibrationBoard::ObjectPointId& additionalObjectPointId : additionalObjectPointIds)
		{
			objectPointIdSet.insert(additionalObjectPointId);
		}

		ocean_assert(objectPointIdSet.size() == observation.objectPointIds().size() + additionalObjectPointIds.size());
	}

#endif

	return true;
}

SharedAnyCamera CameraCalibrator::optimizeCamera(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const Points& points, const Indices32& validMarkerCandidateIndices, const OptimizationStrategy optimizationStrategy, HomogenousMatrix4* board_T_optimizedCamera, const Geometry::Estimator::EstimatorType estimatorType, Scalar* initialError, Scalar* finalError) const
{
	ocean_assert(camera.isValid());
	ocean_assert(board_T_camera.isValid());
	ocean_assert(optimizationStrategy != OptimizationStrategy::OS_INVALID);

	Vectors3 objectPoints;
	Vectors2 imagePoints;

	objectPoints.reserve(validMarkerCandidateIndices.size() * Marker::numberPoints());
	imagePoints.reserve(validMarkerCandidateIndices.size() * Marker::numberPoints());

	const Scalar xMetricMarkerSize = metricCalibrationBoard_.xMetricMarkerSize();
	const Scalar zMetricMarkerSize = metricCalibrationBoard_.zMetricMarkerSize();

	for (const Index32& validMarkerCandidateIndex : validMarkerCandidateIndices)
	{
		ocean_assert(validMarkerCandidateIndex < markerCandidates_.size());
		const MarkerCandidate& markerCandidate = markerCandidates_[validMarkerCandidateIndex];

		ocean_assert(markerCandidate.hasMarkerCoordinate());

		const CalibrationBoard::BoardMarker& boardMarker = metricCalibrationBoard_.marker(markerCandidate.markerCoordinate());

		const Vector3 markerPosition = metricCalibrationBoard_.markerCenterPosition(markerCandidate.markerCoordinate());

		for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
		{
			const Index32 pointIndex = markerCandidate.pointIndex(indexInMarker);

			ocean_assert(pointIndex != Index32(-1));
			if (pointIndex != Index32(-1))
			{
				ocean_assert(pointIndex < points.size());
				const Point& point = points[pointIndex];

				ocean_assert(point.sign() == boardMarker.pointSign<true>(indexInMarker));

				const Vector3 markerObjectPoint = boardMarker.objectPoint(markerPosition, xMetricMarkerSize, zMetricMarkerSize, indexInMarker);

				objectPoints.emplace_back(markerObjectPoint);

				const Vector2& imagePoint = point.observation();
				imagePoints.emplace_back(imagePoint);
			}
		}
	}

	SharedAnyCamera optimizedCamera;

	HomogenousMatrices4 world_T_optimizedCameras(1);
	NonconstArrayAccessor optimizedPoses(world_T_optimizedCameras);

	constexpr unsigned int iterations = 20u;

	constexpr Scalar distortionConstrainmentFactor = Scalar(2);

	Scalars debugIntermediateErrors;
	if (!Geometry::NonLinearOptimizationCamera::optimizeCameraPoses(camera, ConstElementAccessor<HomogenousMatrix4>(1, board_T_camera), ConstElementAccessor<Vectors3>(1, objectPoints), ConstElementAccessor<Vectors2>(1, imagePoints), optimizedCamera, &optimizedPoses, iterations, optimizationStrategy, estimatorType, Scalar(0.001), Scalar(5), true, distortionConstrainmentFactor, initialError, finalError, &debugIntermediateErrors))
	{
		return nullptr;
	}

	ocean_assert(!debugIntermediateErrors.empty());
	Log::debug() << "Optimized projection error from " << debugIntermediateErrors.front() << " -> " << debugIntermediateErrors.back() << " in " << debugIntermediateErrors.size() << " iterations";

	if (board_T_optimizedCamera != nullptr)
	{
		ocean_assert(world_T_optimizedCameras.size() == 1);
		*board_T_optimizedCamera = world_T_optimizedCameras.front();
	}

	return optimizedCamera;
}

SharedAnyCamera CameraCalibrator::determineInitialCameraFieldOfView(const unsigned int width, const unsigned int height, const Points& points, const MarkerCandidates& markerCandidates, const InitialCameraProperties& initialCameraProperties)
{
	if (initialCameraProperties.anyCameraType() == AnyCameraType::PINHOLE || initialCameraProperties.anyCameraType() == AnyCameraType::INVALID)
	{
		Scalar fovX = 0;

		if (CalibrationBoardDetector::determineInitialCameraFieldOfView(width, height, points, markerCandidates, AnyCameraType::PINHOLE, fovX, initialCameraProperties.minFovX(), initialCameraProperties.maxFovX()))
		{
			if (initialCameraProperties.anyCameraType() == AnyCameraType::PINHOLE || fovX <= Numeric::deg2rad(100))
			{
				Log::debug() << "Initial camera: PINHOLE, fovX: " << Numeric::rad2deg(fovX);

				return std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, fovX));
			}
		}
	}

	if (initialCameraProperties.anyCameraType() == AnyCameraType::FISHEYE || initialCameraProperties.anyCameraType() == AnyCameraType::INVALID)
	{
		Scalar fovX = 0;

		if (CalibrationBoardDetector::determineInitialCameraFieldOfView(width, height, points, markerCandidates, AnyCameraType::FISHEYE, fovX))
		{
			Log::debug() << "Initial camera: FISHEYE, fovX: " << Numeric::rad2deg(fovX);

			return std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, fovX));
		}
	}

	return nullptr;
}

SharedAnyCamera CameraCalibrator::determinePreciseCamera(const CalibrationBoardObservation* observations, const size_t numberObservations, const Geometry::NonLinearOptimizationCamera::OptimizationStrategy optimizationStrategy, HomogenousMatrices4* board_T_optimizedCameras, const Geometry::Estimator::EstimatorType estimatorType, const bool startWithFocalLength, const Scalar distortionConstrainmentFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(observations != nullptr && numberObservations >= 1);
	ocean_assert(optimizationStrategy != Geometry::NonLinearOptimizationCamera::OS_INVALID);

	RandomGenerator randomGenerator;

	SharedAnyCamera camera;
	HomogenousMatrices4 world_T_cameras;

	std::vector<Vectors3> objectPointGroups;
	std::vector<Vectors2> imagePointGroups;

	objectPointGroups.reserve(numberObservations);
	imagePointGroups.reserve(numberObservations);

	for (size_t nObservation = 0; nObservation < numberObservations; ++nObservation)
	{
		const CalibrationBoardObservation& observation = observations[nObservation];

		if (!camera)
		{
			if (startWithFocalLength)
			{
				if (observation.camera()->name() == AnyCameraFisheye::WrappedCamera::name())
				{
					const AnyCameraFisheye& anyCameraFisheye = (const AnyCameraFisheye&)(*observation.camera());
					const FisheyeCamera& fisheyeCamera = anyCameraFisheye.actualCamera();

					camera = std::make_shared<AnyCameraFisheye>(FisheyeCamera(fisheyeCamera.width(), fisheyeCamera.height(), fisheyeCamera.fovX()));
				}
				else
				{
					ocean_assert(observation.camera()->name() == AnyCameraPinhole::WrappedCamera::name());

					const AnyCameraPinhole& anyCameraPinhole = (const AnyCameraPinhole&)(*observation.camera());
					const PinholeCamera& pinholeCamera = anyCameraPinhole.actualCamera();

					camera = std::make_shared<AnyCameraPinhole>(PinholeCamera(pinholeCamera.width(), pinholeCamera.height(), pinholeCamera.fovX()));
				}
			}
			else
			{
				camera = observation.camera();
			}
		}

		HomogenousMatrix4 world_T_camera;
		if (Geometry::RANSAC::p3p(*camera, ConstArrayAccessor<Vector3>(observation.objectPoints()), ConstArrayAccessor<Vector2>(observation.imagePoints()), randomGenerator, world_T_camera))
		{
			world_T_cameras.push_back(world_T_camera);
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			world_T_cameras.push_back(observation.board_T_camera());
		}

		objectPointGroups.push_back(observation.objectPoints());
		imagePointGroups.push_back(observation.imagePoints());
	}

	SharedAnyCamera optimizedCamera;

	HomogenousMatrices4 world_T_optimizedCameras(world_T_cameras.size());
	NonconstArrayAccessor optimizedPoses(world_T_optimizedCameras);

	constexpr unsigned int iterations = 100u;

	Scalars debugIntermediateErrors;
	if (!Geometry::NonLinearOptimizationCamera::optimizeCameraPoses(*camera, ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), ConstArrayAccessor<Vectors3>(objectPointGroups), ConstArrayAccessor<Vectors2>(imagePointGroups), optimizedCamera, &optimizedPoses, iterations, optimizationStrategy, estimatorType, Scalar(0.001), Scalar(5), true, distortionConstrainmentFactor, initialError, finalError, &debugIntermediateErrors))
	{
		return nullptr;
	}

	ocean_assert(!debugIntermediateErrors.empty());
	Log::info() << "Optimized projection error from " << debugIntermediateErrors.front() << " -> " << debugIntermediateErrors.back() << " in " << debugIntermediateErrors.size() << " iterations";

	if (board_T_optimizedCameras != nullptr)
	{
		*board_T_optimizedCameras = std::move(world_T_optimizedCameras);
	}

	return optimizedCamera;
}

}

}

}
