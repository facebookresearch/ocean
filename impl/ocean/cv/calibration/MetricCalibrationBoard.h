/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_METRIC_CALIBRATION_BOARD_H
#define META_OCEAN_CV_CALIBRATION_METRIC_CALIBRATION_BOARD_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoard.h"
#include "ocean/cv/calibration/MarkerCandidate.h"
#include "ocean/cv/calibration/MetricSize.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class extends the calibration board with metric information.
 * The metric information comes from the measured width and height of the measurement indication of the calibration board in reality.<br>
 * The calibration board is defined in the xz-plane with y-axis upwards, the origin of the coordinate system is defined in the center of the board.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT MetricCalibrationBoard : public CalibrationBoard
{
	protected:

		/**
		 * Definition of an unordered set holding marker coordinates.
		 */
		using MarkerCoordinateSet = std::unordered_set<MarkerCoordinate, MarkerCoordinate>;

	public:

		/**
		 * Creates an invalid calibration board.
		 */
		MetricCalibrationBoard() = default;

		/**
		 * Copies a metric calibration board.
		 * @param metricCalibrationBoard The calibration board to copy
		 */
		MetricCalibrationBoard(const MetricCalibrationBoard& metricCalibrationBoard) = default;

		/**
		 * Creates a new metric calibration board based on a calibration board and measured width and height of the measurement indication of the calibration board in reality.
		 * @param calibrationBoard The calibration board for which the metric calibration board will be created, must be valid
		 * @param measurementMetricIndicationWidth The measured metric distance between the left and right measurement indication on the real calibration board, with range (0, infinity)
		 * @param measurementMetricIndicationHeight The measured metric distance between the top and bottom measurement indication on the real calibration board, with range (0, infinity)
		 */
		MetricCalibrationBoard(CalibrationBoard&& calibrationBoard, const MetricSize& measurementMetricIndicationWidth, const MetricSize& measurementMetricIndicationHeight);

		/**
		 * Returns the metric width of the measurement indication of the calibration board in the real world.
		 * @return The board's measured width, with range (0, infinity)
		 */
		inline const MetricSize& measurementMetricIndicationWidth() const;

		/**
		 * Returns the metric height of the measurement indication of the calibration board in the real world.
		 * @return The board's measured height, with range (0, infinity)
		 */
		inline const MetricSize& measurementMetricIndicationHeight() const;

		/**
		 * Returns the metric horizontal edge length of a marker in the real calibration board.
		 * @return The board's horizontal marker edge length, with range (0, infinity)
		 */
		inline Scalar xMetricMarkerSize() const;

		/**
		 * Returns the metric vertical edge length of a marker in the real calibration board.
		 * @return The board's vertical marker edge length, with range (0, infinity)
		 */
		inline Scalar zMetricMarkerSize() const;

		/**
		 * Returns the 3D center location of a marker within this calibration board.
		 * @param markerCoordinate The coordinate of the marker for which the center point will be returned, must be valid
		 * @return The marker's center point in the coordinate system of the calibration board, defined in the board's xz-plane
		 */
		inline Vector3 markerCenterPosition(const MarkerCoordinate& markerCoordinate) const;

		/**
		 * Returns the 3D location of a point of a marker within this calibration board.
		 * @param markerCoordinate The coordinate of the marker for which the point will be returned, must be valid
		 * @param indexInMarker The index of the point within the marker, with range [0, 24]
		 * @return The marker's point in the coordinate system of the calibration board, defined in the board's xz-plane
		 */
		inline Vector3 objectPoint(const MarkerCoordinate& markerCoordinate, const size_t indexInMarker) const;

		/**
		 * Returns all object points of this calibration board.
		 * @param objectPointIds Optional resulting ids of the object points, nullptr if not of interest
		 * @return The object points of this calibration board, will be markers() * Marker::numberPoints()
		 */
		Vectors3 objectPoints(ObjectPointIds* objectPointIds = nullptr) const;

		/**
		 * Determines the camera pose using only 2D/3D correspondences from given marker candidates.
		 * @param camera The camera profile to be used, must be valid
		 * @param markerCandidates The marker candidates with valid marker coordinate, at least one
		 * @param points The detected points associated with the marker candidates
		 * @param randomGenerator The random generator to be used
		 * @param board_T_camera The resulting camera pose transforming camera to board, with default camera looking towards the positive z-space with y-axis upwards
		 * @param maximalProjectionError The maximal projection error between projected 3D object points and their corresponding 2D observations, with range [0, infinity)
		 * @param useMarkerCandidateIndices Optional resulting indices of the marker candidates which were used to determine the camera pose, nullptr if not of interest
		 * @param usedObjectPointIds Optional resulting ids of the object points which were used to determine the camera pose, nullptr if not of interest
		 * @param usedObjectPoints Optional resulting 3D object points which were used to determine the camera pose, nullptr if not
		 * @param usedImagePoints Optional resulting 2D image points which were used to determine the camera pose, nullptr if not
		 * @return True, if succeeded
		 */
		bool determineCameraPose(const AnyCamera& camera, const ConstIndexedAccessor<MarkerCandidate>& markerCandidates, const Points& points, RandomGenerator& randomGenerator, HomogenousMatrix4& board_T_camera, const Scalar maximalProjectionError, Indices32* usedMarkerCandidates = nullptr, ObjectPointIds* usedObjectPointIds = nullptr, Vectors3* usedObjectPoints = nullptr, Vectors2* usedImagePoints = nullptr) const;

		/**
		 * Optimizes the camera pose using 2D/3D correspondences from known valid marker candidates and from known additional marker coordinates.
		 * @param camera The camera profile to be used, must be valid
		 * @param board_T_camera The initial camera pose to be optimized, with default camera looking towards the positive z-space with y-axis upwards, must be valid
		 * @param validMarkerCandidates The known valid marker candidates, at least one
		 * @param additionalMarkerCoordinates The known additional marker coordinates, at least one
		 * @param points The detected points associated with the image
		 * @param pointsDistributionArray The distribution array of the detected points
		 * @param board_T_optimizedCamera The resulting optimized camera pose, with default camera looking towards the positive z-space with y-axis upwards
		 * @param maximalProjectionError The maximal projection error between projected 3D object points and their corresponding 2D observations, with range [0, infinity)
		 * @param usedObjectPointIds Optional resulting ids of the object points which were used to determine the camera pose, nullptr if not of interest
		 * @param usedImagePoints Optional resulting 2D image points which were used to determine the camera pose, nullptr if not of interest
		 * @param usedObjectPoints Optional resulting 3D object points which were used to determine the camera pose, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool optimizeCameraPose(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const ConstIndexedAccessor<MarkerCandidate>& validMarkerCandidates, const CV::PixelPositions& additionalMarkerCoordinates, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, HomogenousMatrix4& board_T_optimizedCamera, const Scalar maximalProjectionError, ObjectPointIds* usedObjectPointIds, Vectors2* usedImagePoints = nullptr, Vectors3* usedObjectPoints = nullptr) const;

		/**
		 * Returns whether this calibration board holds valid data and valid measured indication distances.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Default copy assignment operator.
		 * @param calibrationBoard The calibration board to assign
		 * @return Reference to this object
		 */
		MetricCalibrationBoard& operator=(const MetricCalibrationBoard& calibrationBoard) = default;

		/**
		 * Creates a unique metric calibration board based on a unique board id (the seed) and the number of markers the board has.
		 * @param id The unique board id defining the seed for the random number generator, with range [0, infinity)
		 * @param xMarkers The number of horizontal markers the calibration board will have, with range [1, infinity)
		 * @param yMarkers The number of vertical markers the calibration board will have, with range [1, infinity)
		 * @param measurementMetricIndicationWidth The measured metric distance between the left and right measurement indication on the real calibration board, with range (0, infinity)
		 * @param measurementMetricIndicationHeight The measured metric distance between the top and bottom measurement indication on the real calibration board, with range (0, infinity)
		 * @param calibrationBoard The resulting calibration board
		 * @return True, if succeeded
		 */
		static bool createMetricCalibrationBoard(const unsigned int id, const size_t xMarkers, const size_t yMarkers, const MetricSize& measurementMetricIndicationWidth, const MetricSize& measurementMetricIndicationHeight, MetricCalibrationBoard& metricCalibrationBoard);

		/**
		 * Returns the padding factor of this metric calibration board.
		 * The padding factor specifies the size of the padding around the actual board markers and the measurement indications in relation to the size of the individual board markers.
		 * @return The padding factor, with range (0, 1)
		 */
		static constexpr double paddingFactor();

		/**
		 * Determines the optimal marker grid for a calibration board with specific paper width and height and minimal marker size.
		 * @param paperWidth The width of the paper, with range (0, infinity)
		 * @param paperHeight The height of the paper, with range (0, infinity)
		 * @param xMarkers The resulting number of horizontal markers, with range [1, infinity)
		 * @param yMarkers The resulting number of vertical markers, with range [1, infinity)
		 * @param minMarkerSize The minimal size of a marker, with range (0, infinity)
		 * @param margin The margin between the paper border and the measurement indications (the margin due to physical limitations of the printer), with range [0, infinity)
		 * @param paddingFactor The padding factor, with range (0, 1)
		 */
		static bool determineOptimalMarkerGrid(const MetricSize& paperWidth, const MetricSize& paperHeight, size_t& xMarkers, size_t& yMarkers, const MetricSize& minMarkerSize = MetricSize(30.0, MetricSize::UT_MILLIMETER), const MetricSize& margin = MetricSize(6.0, MetricSize::UT_MILLIMETER), const double paddingFactor = MetricCalibrationBoard::paddingFactor());

	protected:

		/// The measured metric distance between the left and right measurement indication on the real calibration board, with range (0, infinity).
		MetricSize measurementMetricIndicationWidth_;

		/// The measured metric distance between the top and bottom measurement indication on the real calibration board, with range (0, infinity).
		MetricSize measurementMetricIndicationHeight_;

		/// The board's horizontal marker edge length, with range (0, infinity).
		Scalar xMetricMarkerSize_ = 0;

		/// The board's vertical marker edge length, with range (0, infinity).
		Scalar zMetricMarkerSize_ = 0;
};

inline const MetricSize& MetricCalibrationBoard::measurementMetricIndicationWidth() const
{
	return measurementMetricIndicationWidth_;
}

inline const MetricSize& MetricCalibrationBoard::measurementMetricIndicationHeight() const
{
	return measurementMetricIndicationHeight_;
}

inline Scalar MetricCalibrationBoard::xMetricMarkerSize() const
{
	return xMetricMarkerSize_;
}

inline Scalar MetricCalibrationBoard::zMetricMarkerSize() const
{
	return zMetricMarkerSize_;
}

Vector3 MetricCalibrationBoard::markerCenterPosition(const MarkerCoordinate& markerCoordinate) const
{
	ocean_assert(markerCoordinate.x() < xMarkers_ && markerCoordinate.y() < yMarkers_);

	const Scalar markersSizeX = Scalar(xMarkers_) * xMetricMarkerSize_;
	const Scalar markersSizeZ = Scalar(yMarkers_) * zMetricMarkerSize_;

	const Scalar metricMarkerCenterX = (Scalar(markerCoordinate.x()) + Scalar(0.5)) * xMetricMarkerSize_ - markersSizeX / Scalar(2);
	const Scalar metricMarkerCenterZ = (Scalar(markerCoordinate.y()) + Scalar(0.5)) * zMetricMarkerSize_ - markersSizeZ / Scalar(2);

	return Vector3(metricMarkerCenterX, 0, metricMarkerCenterZ);
}

inline Vector3 MetricCalibrationBoard::objectPoint(const MarkerCoordinate& markerCoordinate, const size_t indexInMarker) const
{
	ocean_assert(markerCoordinate.x() < xMarkers_ && markerCoordinate.y() < yMarkers_);

	const Vector3 markerCenter = markerCenterPosition(markerCoordinate);

	const BoardMarker& boardMarker = marker(markerCoordinate);

	return boardMarker.objectPoint(markerCenter, xMetricMarkerSize_, zMetricMarkerSize_, indexInMarker);
}

inline bool MetricCalibrationBoard::isValid() const
{
	ocean_assert(xMetricMarkerSize_ == 0 || zMetricMarkerSize_ > Numeric::eps());
	return CalibrationBoard::isValid() && xMetricMarkerSize_ > Numeric::eps();
}

constexpr double MetricCalibrationBoard::paddingFactor()
{
	return 0.1; // 10% of the marker size
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_METRIC_CALIBRATION_BOARD_H
