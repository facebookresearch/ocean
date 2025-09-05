/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_H
#define META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoardObservation.h"
#include "ocean/cv/calibration/MetricCalibrationBoard.h"
#include "ocean/cv/calibration/PointDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements a camera calibrator using a couple of images of a calibration board.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT CameraCalibrator
{
	public:

		/**
		 * This class holds the properties of the initial camera used during the calibration.
		 */
		class InitialCameraProperties
		{
			public:

				/**
				 * Creates initial camera properties with default parameters.
				 */
				inline InitialCameraProperties();

				/**
				 * Creates initial camera properties.
				 * @param anyCameraType The type of the initial camera to be used, AnyCameraType::INVALID to determine the camera type automatically
				 */
				explicit inline InitialCameraProperties(const AnyCameraType anyCameraType);

				/**
				 * Creates initial camera properties.
				 * @param anyCameraType The type of the initial camera to be used, AnyCameraType::INVALID to determine the camera type automatically
				 * @param minFovX The minimal horizontal field of view of the initial camera, in radian, with range (0, maxFovX)
				 * @param maxFovX The maximal horizontal field of view of the initial camera, in radian, with range (minFovX, PI)
				 */
				inline InitialCameraProperties(const AnyCameraType anyCameraType, const Scalar minFovX, const Scalar maxFovX);

				/**
				 * Returns the type of the initial camera to be used.
				 * @return The camera type, AnyCameraType::INVALID to determine the camera type automatically
				 */
				inline AnyCameraType anyCameraType() const;

				/**
				 * Returns the minimal horizontal field of view of the initial camera.
				 * @return Minimal horizontal field of view, in radian
				 */
				inline Scalar minFovX() const;

				/**
				 * Returns the maximal horizontal field of view of the initial camera.
				 * @return Maximal horizontal field of view, in radian
				 */
				inline Scalar maxFovX() const;

			protected:

				/// The type of the initial camera to be used, AnyCameraType::INVALID to determine the camera type automatically.
				AnyCameraType anyCameraType_ = AnyCameraType::INVALID;

				/// The minimal horizontal field of view of the initial camera, in radian, with range (0, maxFovX_).
				Scalar minFovX_ = Numeric::deg2rad(20);

				/// The maximal horizontal field of view of the initial camera, in radian, with range (minFovX_, PI)
				Scalar maxFovX_ = Numeric::deg2rad(175);
		};

		/**
		 * Definition of individual calibration stages, **TODO** not yet used.
		 */
		enum CalibrationStage : uint32_t
		{
			/// The calibration stage is unknown.
			CS_UNKNOWN = 0u,
			/// The calibration stage is determining the initial camera field of view.
			CS_DETERMINE_INITIAL_CAMERA_FOV,
			/// The calibration stage is calibrating the camera.
			CS_CALIBRATE_CAMERA
		};

		/**
		 * Definition of individual image results.
		 */
		enum ImageResult : uint32_t
		{
			/// An error occurred during the handling of the image.
			IR_ERROR = 0u,
			/// The image was handled but the calibration board was not detected.
			IR_BOARD_WAS_NOT_DETECTED,
			/// The calibration board was detected successfully.
			IR_BOARD_WAS_DETECTED
		};

		/**
		 * Alias for the camera's optimization strategy.
		 */
		using OptimizationStrategy = Geometry::NonLinearOptimizationCamera::OptimizationStrategy;

	public:

		/**
		 * Default constructor creating an invalid camera calibrator.
		 */
		CameraCalibrator() = default;

		/**
		 * Creates a new camera calibrator.
		 * @param metricCalibrationBoard The metric calibration board which will be visible in the camera images, must be valid
		 * @param initialCameraProperties The initial camera properties which can be used to jump start the calibration, must be valid
		 */
		explicit CameraCalibrator(const MetricCalibrationBoard& metricCalibrationBoard, const InitialCameraProperties& initialCameraProperties = InitialCameraProperties());

		/**
		 * Handles a new image.
		 * @param imageId The unique id of the image, must be valid
		 * @param frame The frame to handle, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return The result of the image handling
		 */
		ImageResult handleImage(const size_t imageId, const Frame& frame, Worker* worker = nullptr);

		/**
		 * Finalizes the calibration and determines the precise camera profile.
		 * This function should be call after all images have been handled.
		 * @return True, if succeeded
		 */
		bool finalize();

		/**
		 * Returns the current calibration stage.
		 * @return The calibrator's calibration stage
		 */
		inline CalibrationStage calibrationStage() const;

		/**
		 * Returns the metric calibration board of this calibrator.
		 * @return The calibrator's calibration board
		 */
		inline const MetricCalibrationBoard& metricCalibrationBoard() const;

		/**
		 * Returns the calibrator's current camera profile.
		 * @param projectionError Optional resulting projection error, with range [0, infinity)
		 * @return The calibrator's current camera profile, nullptr if the camera profile has not been determined yet
		 */
		inline SharedAnyCamera camera(Scalar* projectionError = nullptr) const;

		/**
		 * Returns whether this calibrator holds at least one valid observation.
		 * @return True, if so
		 */
		inline bool hasObservations() const;

		/**
		 * Returns all observations which have been made so far.
		 * @return The calibration board observations
		 */
		inline const CalibrationBoardObservations& observations() const;

		/**
		 * Returns the latest observation which has been made.
		 * @return The latest calibration board observation
		 */
		inline const CalibrationBoardObservation& latestObservation() const;

		/**
		 * Returns the overall number of correspondences which have been gathered so far.
		 * @return The number of correspondences across all observations
		 */
		size_t numberCorrespondences() const;

	protected:

		/**
		 * Determines the initial camera pose based on marker candidates with known marker coordinate.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param points The points which have been detected in the image, must be valid
		 * @param board_T_camera The resulting initial camera pose, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param usedMarkerCandidateIndices The resulting indices of the marker candidates which have been used to determine the initial pose
		 * @return True, if succeeded
		 */
		bool determineInitialPoseWithValidMarkerCandidates(const AnyCamera& camera, const Points& points, HomogenousMatrix4& board_T_camera, Indices32& usedMarkerCandidateIndices) const;

		/**
		 * Optimizes the camera pose using known valid marker candidates with known marker coordinates and iteratively adds addtional correspondences from neighboring marker candidates.
		 * The area around the valid marker candidates is iteratively growing until no additonal marker candidates can be added.<br>
		 * The growing strategy helps with handling fisheye cameras.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param board_T_camera The initial camera pose to improve, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param points The points which have been detected in the image
		 * @param pointsDistributionArray The distribution array of the points, must be valid
		 * @param validMarkerCandidateIndices The indices of the marker candidates which have been used to determine the initial pose, must be valid
		 * @param maximalProjectionError The maximal projection error between image point and reprojected object point, in pixel, with range [0, infinity)
		 * @param board_T_optimizedCamera The resulting optimized camera pose, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param objectPointIds The resulting ids of the object points which have been used to determine the optimized camera pose
		 * @param objectPoints The resulting object points which have been used to determine the optimized camera pose, one for each object point id
		 * @param imagePoints The resulting image points which have been used to determine the optimized camera pose, one for each object point id
		 */
		bool optimizeCameraPoseWithAdditionalPointsFromMarkerCandidates(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Indices32& validMarkerCandidateIndices, const Scalar maximalProjectionError, HomogenousMatrix4& board_T_optimizedCamera, CalibrationBoard::ObjectPointIds& objectPointIds, Vectors3& objectPoints, Vectors2& imagePoints) const;

		/**
		 * Determines additional 2D/3D correspondences for a valid observation of a calibration board which has not yet used all possible marker points of the calibration board.
		 * @param calibrationBoard The calibration board which has been observed, must be valid
		 * @param observation The observation of the calibration board, must be valid
		 * @param points The points which have been detected in the image
		 * @param pointsDistributionArray The distribution array of the points, must be valid
		 * @param additionalObjectPointIds The resulting ids of additional object points which are not part of the provided observation but which are part of the calibration board
		 * @param additionalObjectPoints The resulting additional object points, one for each object point id
		 * @param additionalImagePoints The resulting additional image points, one for each object point id
		 * @param maximalProjectionError The maximal projection error between image point and reprojected object point, in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool determineAdditionalCorrespondences(const MetricCalibrationBoard& calibrationBoard, const CalibrationBoardObservation& observation, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, CalibrationBoard::ObjectPointIds& additionalObjectPointIds, Vectors3& additionalObjectPoints, Vectors2& additionalImagePoints, const Scalar maximalProjectionError);

		/**
		 * Optimizes the camera profile based on all marker points from known valid marker candidates with known marker coordinates.
		 * @param camera The camera profile to optimize, must be valid
		 * @param board_T_camera The initial camera pose to improve, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param points The points which have been detected in the image
		 * @param validMarkerCandidateIndices The indices of the marker candidates with known marker coordinates, at least one
		 * @param optimizationStrategy The optimization strategy to be used
		 * @param board_T_optimizedCamera Optional resulting optimized camera pose, nullptr if not of interest
		 * @param estimatorType The estimator type to be used
		 * @param initialError Optional resulting initial projection error, with range [0, infinity)
		 * @param finalError Optional resulting final projection error, with range [0, infinity)
		 * @return The resulting optimized camera profile, nullptr if the camera profile could not be determined
		 */
		SharedAnyCamera optimizeCamera(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const Points& points, const Indices32& validMarkerCandidateIndices, const OptimizationStrategy optimizationStrategy, HomogenousMatrix4* board_T_optimizedCamera = nullptr, const Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_SQUARE, Scalar* initialError = nullptr, Scalar* finalError = nullptr) const;

		/**
		 * Determines the initial field of view of the camera for a single image of a calibration board.
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param points The points which have been detected in the image
		 * @param markerCandidates The marker candidates which have been detected in the image with valid border indices (and signs), at least one
		 * @param initialCameraProperties The initial camera properties which can be used to jump start the calibration
		 * @return The resulting initial camera profile, nullptr if the initial camera profile could not be determined
		 */
		static SharedAnyCamera determineInitialCameraFieldOfView(const unsigned int width, const unsigned int height, const Points& points, const MarkerCandidates& markerCandidates, const InitialCameraProperties& initialCameraProperties);

		/**
		 * Determines the precise camera profile based on several observations of a calibration board.
		 * @param observations The observations of the calibration board, must be valid
		 * @param numberObservations The number of observations, with range [1, infinity)
		 * @param optimizationStrategy The optimization strategy to be used
		 * @param board_T_optimizedCameras Optional resulting camera poses, one for each observation, with default camera pointing towards the negative z-space with y-axis upwards; nullptr if not of interest
		 * @param estimatorType The estimator type to be used
		 * @param initialError Optional resulting initial projection error, with range [0, infinity)
		 * @param finalError Optional resulting final projection error, with range [0, infinity)
		 * @return The resulting precise camera profile, nullptr if the camera profile could not be determined
		 */
		static SharedAnyCamera determinePreciseCamera(const CalibrationBoardObservation* observations, const size_t numberObservations, const OptimizationStrategy optimizationStrategy, HomogenousMatrices4* board_T_optimizedCameras = nullptr, const Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_SQUARE, Scalar* initialError = nullptr, Scalar* finalError = nullptr);

	protected:

		/// The current calibration stage.
		CalibrationStage calibrationStage_ = CS_UNKNOWN;

		/// The metric calibration board which is used for calibration.
		MetricCalibrationBoard metricCalibrationBoard_;

		/// The initial camera properties which can be used to jump start the calibration.
		InitialCameraProperties initialCameraProperties_;

		/// The current frame with pixel format FORMAT_Y8.
		Frame yFrame_;

		/// The point detector which is used to detect the marker points of the calibration board.
		PointDetector pointDetector_;

		/// The marker candidates which have been detected in the current image.
		MarkerCandidates markerCandidates_;

		/// The random generator to be used.
		mutable RandomGenerator randomGenerator_;

		/// Reusable frame object to avoid memory re-allocations.
		mutable Frame reusableMarkerCoordinateUsageFrame_;

		/// The observations which have been made so far.
		CalibrationBoardObservations observations_;

		/// The final camera profile.
		SharedAnyCamera camera_;

		/// The resulting projection error of the final camera profile, with range [0, infinity)
		Scalar cameraProjectionError_ = Numeric::maxValue();
};

inline CameraCalibrator::InitialCameraProperties::InitialCameraProperties()
{
	// nothing to do here
}

inline CameraCalibrator::InitialCameraProperties::InitialCameraProperties(const AnyCameraType anyCameraType) :
	anyCameraType_(anyCameraType)
{
	// nothing to do here
}

inline CameraCalibrator::InitialCameraProperties::InitialCameraProperties(const AnyCameraType anyCameraType, const Scalar minFovX, const Scalar maxFovX) :
	anyCameraType_(anyCameraType),
	minFovX_(minFovX),
	maxFovX_(maxFovX)
{
	// nothing to do here
}

inline AnyCameraType CameraCalibrator::InitialCameraProperties::anyCameraType() const
{
	return anyCameraType_;
}

inline Scalar CameraCalibrator::InitialCameraProperties::minFovX() const
{
	ocean_assert(minFovX_ <= maxFovX_);
	return minFovX_;
}

inline Scalar CameraCalibrator::InitialCameraProperties::maxFovX() const
{
	ocean_assert(minFovX_ <= maxFovX_);
	return maxFovX_;
}

inline CameraCalibrator::CalibrationStage CameraCalibrator::calibrationStage() const
{
	return calibrationStage_;
}

inline const MetricCalibrationBoard& CameraCalibrator::metricCalibrationBoard() const
{
	return metricCalibrationBoard_;
}

inline SharedAnyCamera CameraCalibrator::camera(Scalar* projectionError) const
{
	if (projectionError != nullptr)
	{
		*projectionError = cameraProjectionError_;
	}

	return camera_;
}

inline bool CameraCalibrator::hasObservations() const
{
	return !observations_.empty();
}

inline const CalibrationBoardObservations& CameraCalibrator::observations() const
{
	return observations_;
}

inline const CalibrationBoardObservation& CameraCalibrator::latestObservation() const
{
	return observations_.back();
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_H
