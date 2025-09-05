/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_BOARD_OBSERVATION_H
#define META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_BOARD_OBSERVATION_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoard.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/// Forward declaration.
class CalibrationBoardObservation;

/**
 * Definition of a vector holding CalibrationBoardObservation objects.
 * @ingroup cvcalibration
 */
using CalibrationBoardObservations = std::vector<CalibrationBoardObservation>;

/**
 * This class implements an observation of a calibration board.
 * The observation is mainly defined by 2D/3D correspondences between image points and object points, a camera pose and a camera profile.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT CalibrationBoardObservation
{
	protected:

		/**
		 * Definition of an invalid image id.
		 */
		static constexpr size_t invalidImageId_ = size_t(-1);

	public:

		/**
		 * Default creator for an invalid calibration board observation.
		 */
		CalibrationBoardObservation() = default;

		/**
		 * Creates a new calibration board observation.
		 * @param imageId The id of the image in which the calibration board has been observed, must be valid
		 * @param camera The camera profile which were used to observe the calibration board, must be valid
		 * @param board_T_camera The camera pose under which the calibration board has been observed, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param objectPointIds The ids of all object points which were used to observe the calibration board, at least 4
		 * @param objectPoints The 3D object points which were used to observe the calibration board, one for each object point id
		 * @param imagePoints The 2D image points which were used to observe the calibration board, one for each object point
		 */
		CalibrationBoardObservation(const size_t imageId, SharedAnyCamera camera, const HomogenousMatrix4& board_T_camera, CalibrationBoard::ObjectPointIds&& objectPointIds, Vectors3&& objectPoints, Vectors2&& imagePoints);

		/**
		 * Creates a new calibration board observation based on a given observation with improved camera profile and camera pose.
		 * @param camera The camera profile which were used to observe the calibration board, must be valid
		 * @param board_T_camera The camera pose under which the calibration board has been observed, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param observation The observation providing image id and correspondences
		 */
		inline CalibrationBoardObservation(SharedAnyCamera camera, const HomogenousMatrix4& board_T_camera, CalibrationBoardObservation&& observation);

		/**
		 * Returns the id of the image in which the calibration board has been observed.
		 * @return The id of the image, with range [0, infinity), an invalid id if the observation is invalid
		 */
		inline size_t imageId() const;

		/**
		 * Returns the camera profile which were used to observe the calibration board.
		 * @return The camera profile, nullptr if the observation is invalid
		 */
		inline const SharedAnyCamera& camera() const;

		/**
		 * Returns the camera pose under which the calibration board has been observed.
		 * @return The transformation between camera and calibration board, with default camera pointing towards the negative z-space with y-axis upwards, invalid if the observation is invalid
		 */
		inline const HomogenousMatrix4& board_T_camera() const;

		/**
		 * Returns the ids of all object points which were used to observe the calibration board.
		 * @return The object point ids
		 */
		inline const CalibrationBoard::ObjectPointIds& objectPointIds() const;

		/**
		 * Returns the 3D object points which were used to observe the calibration board.
		 * @return The object points, one for each object point id
		 * @see objectPointIds().
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the 2D image points which were used to observe the calibration board.
		 * @return The image points, one for each object point
		 */
		inline const Vectors2& imagePoints() const;

		/**
		 * Returns the image coverage of this observation.
		 * To calculate the coverage, the image is divided into bins and the number of bins which are covered by the observation is determined.
		 * @return The image coverage, with range [0, 1], -1 if the coverage could not be
		 */
		float coverage() const;

		/**
		 * Returns the occupancy array of this observation.
		 * @return The occupancy array, invalid if the coverage could not be determined
		 */
		inline const Geometry::SpatialDistribution::OccupancyArray& occupancyArray() const;

		/**
		 * Adds additional 2D/3D correspondences to this observation.
		 * @param objectPointIds The ids of all object points which were used to observe the calibration board, at least one
		 * @param objectPoints The 3D object points which were used to observe the calibration board, one for each object point id
		 * @param imagePoints The 2D image points which were used to observe the calibration board, one for each object point
		 */
		void addCorrespondences(const CalibrationBoard::ObjectPointIds& objectPointIds, const Vectors3& objectPoints, const Vectors2& imagePoints);

		/**
		 * Updates the camera profile and the camera pose of this observation.
		 * @param camera The new camera profile of this observation, must be valid
		 * @param board_T_camera The new camera pose of this observation, transforming camera to calibration board, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 */
		inline void updateCamera(const SharedAnyCamera& camera, const HomogenousMatrix4& board_T_camera);

		/**
		 * Returns whether this observation is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns an invalid image id.
		 * @return The invalid image id
		 */
		static constexpr size_t invalidImageId();

		/**
		 * Compares the coverage of two observations.
		 * @param observationA The first observation to compare
		 * @param observationB The second observation to compare
		 * @return True, if the first observation has a lower coverage than the second observation
		 */
		static inline bool compareObservations(const CalibrationBoardObservation& observationA, const CalibrationBoardObservation& observationB);

	protected:

		/**
		 * Determines the image coverage of this observation.
		 * To calculate the coverage, the image is divided into bins and the number of bins which are covered by the observation is determined.
		 * @param pixelsPerBin The number of pixels per bin, with range [1, infinity)
		 * @return The image coverage, with range [0, 1]
		 */
		float determineCoverage(const unsigned int pixelsPerBin) const;

	protected:

		/// The id of the image in which the calibration board has been observed.
		size_t imageId_ = invalidImageId_;

		/// The camera profile which were used to observe the calibration board.
		SharedAnyCamera camera_;

		/// The camera pose under which the calibration board has been observed, with default camera pointing towards the negative z-space with y-axis upwards.
		HomogenousMatrix4 board_T_camera_;

		/// The ids of all object points which were used to observe the calibration board.
		CalibrationBoard::ObjectPointIds objectPointIds_;

		/// The 3D object points which were used to observe the calibration board.
		Vectors3 objectPoints_;

		/// The 2D image points which were used to observe the calibration board.
		Vectors2 imagePoints_;

		/// The image coverage of this observation, with range [0, 1], -1 if the coverage could not be determined.
		mutable float coverage_ = -1.0f;

		/// The occupancy array of this observation which is used to determine the image coverage.
		mutable Geometry::SpatialDistribution::OccupancyArray occupancyArray_;
};

inline CalibrationBoardObservation::CalibrationBoardObservation(SharedAnyCamera camera, const HomogenousMatrix4& board_T_camera, CalibrationBoardObservation&& observation) :
	CalibrationBoardObservation(observation.imageId(), std::move(camera), board_T_camera, std::move(observation.objectPointIds_), std::move(observation.objectPoints_), std::move(observation.imagePoints_))
{
	// nothing to do here
}

inline size_t CalibrationBoardObservation::imageId() const
{
	return imageId_;
}

inline const SharedAnyCamera& CalibrationBoardObservation::camera() const
{
	return camera_;
}

inline const HomogenousMatrix4& CalibrationBoardObservation::board_T_camera() const
{
	return board_T_camera_;
}

inline const CalibrationBoard::ObjectPointIds& CalibrationBoardObservation::objectPointIds() const
{
	return objectPointIds_;
}

inline const Vectors3& CalibrationBoardObservation::objectPoints() const
{
	return objectPoints_;
}

inline const Vectors2& CalibrationBoardObservation::imagePoints() const
{
	return imagePoints_;
}

inline const Geometry::SpatialDistribution::OccupancyArray& CalibrationBoardObservation::occupancyArray() const
{
	if (coverage() < 0.0f)
	{
		ocean_assert(false && "This should never happen!");
	}

	return occupancyArray_;
}

inline void CalibrationBoardObservation::updateCamera(const SharedAnyCamera& camera, const HomogenousMatrix4& board_T_camera)
{
	ocean_assert(camera && camera->isValid());
	ocean_assert(board_T_camera.isValid());

	camera_ = camera;
	board_T_camera_ = board_T_camera;
}

inline bool CalibrationBoardObservation::isValid() const
{
	return imageId_ != invalidImageId_ && camera_ && camera_->isValid() && board_T_camera_.isValid() && !objectPointIds_.empty() && objectPointIds_.size() == objectPoints_.size() && objectPoints_.size() == imagePoints_.size();
}

constexpr size_t CalibrationBoardObservation::invalidImageId()
{
	return invalidImageId_;
}

inline bool CalibrationBoardObservation::compareObservations(const CalibrationBoardObservation& observationA, const CalibrationBoardObservation& observationB)
{
	ocean_assert(observationA.isValid() && observationB.isValid());

	return observationA.coverage() < observationB.coverage();
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CAMERA_CALIBRATOR_BOARD_OBSERVATION_H
