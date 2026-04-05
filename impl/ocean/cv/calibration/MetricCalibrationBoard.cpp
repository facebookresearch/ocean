/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/MetricCalibrationBoard.h"
#include "ocean/cv/calibration/PointDetector.h"

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

MetricCalibrationBoard::MetricCalibrationBoard(CalibrationBoard&& calibrationBoard, const MetricSize& measurementMetricIndicationWidth, const MetricSize& measurementMetricIndicationHeight) :
	CalibrationBoard(std::move(calibrationBoard))
{
	ocean_assert(CalibrationBoard::isValid());
	ocean_assert(measurementMetricIndicationWidth.isValid() && measurementMetricIndicationHeight.isValid());

	if (xMarkers_ != 0 && yMarkers_ != 0 && measurementMetricIndicationHeight.isValid())
	{
		measurementMetricIndicationWidth_ = measurementMetricIndicationWidth;
		measurementMetricIndicationHeight_ = measurementMetricIndicationHeight;

		// measurementIndicationWidth == xMetricMarkerSize * xMarkers + xMetricMarkerSize * paddingFactor() * 2

		const double xMeasuredMeter = measurementMetricIndicationWidth.value(MetricSize::UT_MILLIMETER) / 1000.0;
		const double zMeasuredMeter = measurementMetricIndicationHeight.value(MetricSize::UT_MILLIMETER) / 1000.0;

		xMetricMarkerSize_ = Scalar(xMeasuredMeter / (double(xMarkers_) + paddingFactor() * 2.0));
		zMetricMarkerSize_ = Scalar(zMeasuredMeter / (double(yMarkers_) + paddingFactor() * 2.0));

		ocean_assert(isValid());

		Log::debug() << "Measured calibration board marker size: " << String::toAString(xMetricMarkerSize_ * 1000.0, 1u) << "mm x " << String::toAString(zMetricMarkerSize_ * 1000.0, 1u) << "mm";

		const double aspectRatio = xMetricMarkerSize_ / zMetricMarkerSize_;
		ocean_assert(0.99 <= aspectRatio && aspectRatio <= 1.01);

		if (aspectRatio < 0.99 || aspectRatio > 1.01)
		{
			Log::warning() << "The aspect ratio of the calibration board is not 1:1, but " << aspectRatio << ":" << "1";
		}
	}
	else
	{
		ocean_assert(!isValid());
	}
}

Vectors3 MetricCalibrationBoard::objectPoints(ObjectPointIds* objectPointIds) const
{
	Vectors3 result;
	result.reserve(markers() * Marker::numberPoints());

	if (objectPointIds != nullptr)
	{
		ocean_assert(objectPointIds->empty());
		objectPointIds->clear();
	}

	for (size_t yMarker = 0; yMarker < yMarkers_; ++yMarker)
	{
		for (size_t xMarker = 0; xMarker < xMarkers_; ++xMarker)
		{
			const MarkerCoordinate markerCoordinate((unsigned int)(xMarker), (unsigned int)(yMarker));

			for (size_t indexInMarker = 0; indexInMarker < Marker::numberPoints(); ++indexInMarker)
			{
				result.push_back(objectPoint(markerCoordinate, indexInMarker));

				if (objectPointIds != nullptr)
				{
					objectPointIds->emplace_back(markerCoordinate, indexInMarker);
				}
			}
		}
	}

	return result;
}

bool MetricCalibrationBoard::determineCameraPose(const AnyCamera& camera, const ConstIndexedAccessor<MarkerCandidate>& markerCandidates, const Points& points, RandomGenerator& randomGenerator, HomogenousMatrix4& board_T_camera, const Scalar maximalProjectionError, Indices32* usedMarkerCandidates, ObjectPointIds* usedObjectPointIds, Vectors3* usedObjectPoints, Vectors2* usedImagePoints) const
{
	ocean_assert(isValid());
	ocean_assert(camera.isValid());

	ocean_assert(!markerCandidates.isEmpty());
	if (markerCandidates.isEmpty())
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	{
		MarkerCoordinateSet markerCoordinates;

		for (size_t nCandidate = 0; nCandidate < markerCandidates.size(); ++nCandidate)
		{
			const MarkerCandidate& markerCandidate = markerCandidates[nCandidate];

			ocean_assert(markerCandidate.isValid());
			ocean_assert(markerCandidate.hasValidIndices());
			ocean_assert(markerCandidate.hasMarkerId());
			ocean_assert(markerCandidate.hasMarkerCoordinate());

			markerCoordinates.emplace(markerCandidate.markerCoordinate());
		}

		ocean_assert(markerCandidates.size() == markerCoordinates.size()); // TODO activate/CHECK once neighborhood/orientation issue is fixed previously in the pipeline
	}
#endif

	Vectors3 objectPoints;
	Vectors2 imagePoints;

	objectPoints.reserve(markerCandidates.size() * 25);
	imagePoints.reserve(markerCandidates.size() * 25);

	for (size_t n = 0; n < markerCandidates.size(); ++n)
	{
		const MarkerCandidate& markerCandidate = markerCandidates[n];

		ocean_assert(markerCandidate.hasMarkerCoordinate());
		if (!markerCandidate.hasMarkerCoordinate())
		{
			return false;
		}

		const CalibrationBoard::BoardMarker& boardMarker = marker(markerCandidate.markerCoordinate());

		const Vector3 markerPosition = markerCenterPosition(markerCandidate.markerCoordinate());

		for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
		{
			const Index32 pointIndex = markerCandidate.pointIndex(indexInMarker);

			ocean_assert(pointIndex != Index32(-1));
			if (pointIndex == Index32(-1))
			{
				return false;
			}

			ocean_assert(pointIndex < points.size());
			const Point& point = points[pointIndex];

			ocean_assert(point.sign() == boardMarker.pointSign<true>(indexInMarker));

			const Vector3 markerObjectPoint = boardMarker.objectPoint(markerPosition, xMetricMarkerSize_, zMetricMarkerSize_, indexInMarker);

			objectPoints.emplace_back(markerObjectPoint);

			const Vector2& imagePoint = point.observation();
			imagePoints.emplace_back(imagePoint);
		}
	}

	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(objectPoints.size() == markerCandidates.size() * 25);

	Indices32 usedIndices;
	if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, board_T_camera, (unsigned int)(objectPoints.size() / 2), true /*refine*/, 20u, Numeric::sqr(maximalProjectionError), &usedIndices))
	{
		return false;
	}

	if (usedMarkerCandidates != nullptr)
	{
		UnorderedIndexSet32 usedMarkerCandidateSet;
		usedMarkerCandidateSet.reserve(markerCandidates.size());

		for (const Index32 usedIndex : usedIndices)
		{
			const Index32 markerIndex = usedIndex / 25u; // we can determine the index as each of the provided marker candidate had 25 points

			ocean_assert(markerIndex < markerCandidates.size());
			usedMarkerCandidateSet.emplace(markerIndex);
		}

		ocean_assert(usedMarkerCandidates->empty());
		usedMarkerCandidates->assign(usedMarkerCandidateSet.cbegin(), usedMarkerCandidateSet.cend());
	}

	if (usedObjectPointIds != nullptr)
	{
		ocean_assert(usedObjectPointIds->empty());
		usedObjectPointIds->clear();

		usedObjectPointIds->reserve(usedIndices.size());

		for (const Index32 usedIndex : usedIndices)
		{
			const Index32 markerIndex = usedIndex / 25u;

			const MarkerCandidate& markerCandidate = markerCandidates[markerIndex];

			const Index32 indexInMarker = usedIndex % 25u;

			usedObjectPointIds->emplace_back(markerCandidate.markerCoordinate(), indexInMarker);
		}
	}

	if (usedObjectPoints != nullptr)
	{
		ocean_assert(usedObjectPoints->empty());
		usedObjectPoints->clear();

		usedObjectPoints->reserve(usedIndices.size());

		for (const Index32 usedIndex : usedIndices)
		{
			usedObjectPoints->emplace_back(objectPoints[usedIndex]);
		}
	}

	if (usedImagePoints != nullptr)
	{
		ocean_assert(usedImagePoints->empty());
		usedImagePoints->clear();

		usedImagePoints->reserve(usedIndices.size());

		for (const Index32 usedIndex : usedIndices)
		{
			usedImagePoints->emplace_back(imagePoints[usedIndex]);
		}
	}

	return true;
}

bool MetricCalibrationBoard::optimizeCameraPose(const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const ConstIndexedAccessor<MarkerCandidate>& validMarkerCandidates, const CV::PixelPositions& additionalMarkerCoordinates, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, HomogenousMatrix4& board_T_optimizedCamera, const Scalar maximalProjectionError, ObjectPointIds* usedObjectPointIds, Vectors2* usedImagePoints, Vectors3* usedObjectPoints) const
{
	ocean_assert(isValid());
	ocean_assert(camera.isValid());
	ocean_assert(board_T_camera.isValid());

	ocean_assert(!validMarkerCandidates.isEmpty());
	ocean_assert(!additionalMarkerCoordinates.empty());

#ifdef OCEAN_DEBUG
	{
		// let's ensure that both sets of coordinates do not have an intersection

		MarkerCoordinateSet debugMarkerCoordinates;

		for (size_t nCandidate = 0; nCandidate < validMarkerCandidates.size(); ++nCandidate)
		{
			const MarkerCandidate& validMarkerCandidate = validMarkerCandidates[nCandidate];

			ocean_assert(validMarkerCandidate.isValid());
			ocean_assert(validMarkerCandidate.hasValidIndices());
			ocean_assert(validMarkerCandidate.hasMarkerId());
			ocean_assert(validMarkerCandidate.hasMarkerCoordinate());

			debugMarkerCoordinates.emplace(validMarkerCandidate.markerCoordinate());
		}

		debugMarkerCoordinates.insert(additionalMarkerCoordinates.cbegin(), additionalMarkerCoordinates.cend());
		ocean_assert(debugMarkerCoordinates.size() == validMarkerCandidates.size() + additionalMarkerCoordinates.size());
	}
#endif

	const HomogenousMatrix4 flippedCamera_T_board = Camera::standard2InvertedFlipped(board_T_camera);

	ObjectPointIds objectPointIds;
	Vectors3 objectPoints;
	Vectors2 imagePoints;

	objectPointIds.reserve(validMarkerCandidates.size() * 25 * 2);
	objectPoints.reserve(validMarkerCandidates.size() * 25 * 2);
	imagePoints.reserve(validMarkerCandidates.size() * 25 * 2);

	// first, we gather all known 2D/3D correspondences from the valid marker candidates

	for (size_t n = 0; n < validMarkerCandidates.size(); ++n)
	{
		const MarkerCandidate& markerCandidate = validMarkerCandidates[n];

		const CalibrationBoard::BoardMarker& boardMarker = marker(markerCandidate.markerCoordinate());

		const Vector3 markerPosition = markerCenterPosition(markerCandidate.markerCoordinate());

		for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
		{
			const Index32 pointIndex = markerCandidate.pointIndex(indexInMarker);

			if (pointIndex != Index32(-1))
			{
				objectPointIds.emplace_back(markerCandidate.markerCoordinate(), indexInMarker);

				ocean_assert(pointIndex < points.size());
				const Point& point = points[pointIndex];

				ocean_assert(point.sign() == boardMarker.pointSign<true>(indexInMarker));

				const Vector3 markerObjectPoint = boardMarker.objectPoint(markerPosition, xMetricMarkerSize_, zMetricMarkerSize_, indexInMarker);

				objectPoints.emplace_back(markerObjectPoint);

				const Vector2& imagePoint = point.observation();
				imagePoints.emplace_back(imagePoint);
			}
		}
	}

	// now, we try to find additional 2D/3D correspondences from the additional marker coordinates

	const size_t objectPointsFromUsedMarkers = objectPoints.size();

	for (const CV::PixelPosition& additionalMarkerCoordinate : additionalMarkerCoordinates)
	{
		const size_t numberCorrespondencesAtStart = objectPointIds.size();

		const CalibrationBoard::BoardMarker& boardMarker = marker(additionalMarkerCoordinate);

		const Vector3 markerPosition = markerCenterPosition(additionalMarkerCoordinate);

		for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
		{
			const Vector3 markerObjectPoint = boardMarker.objectPoint(markerPosition, xMetricMarkerSize_, zMetricMarkerSize_, indexInMarker);

			const Vector2 predictedImagePoint = camera.projectToImageIF(flippedCamera_T_board, markerObjectPoint);

			if (!camera.isInside(predictedImagePoint, 10))
			{
				continue;
			}

			Index32 closestPointIndex = Index32(-1);
			Index32 secondClosestPointIndex = Index32(-1);
			Scalar closestSqrDistance = Numeric::maxValue();
			Scalar secondClosestSqrDistance = Numeric::maxValue();

			if (!PointDetector::closestPoints(predictedImagePoint, pointsDistributionArray, points, closestPointIndex, secondClosestPointIndex, closestSqrDistance, secondClosestSqrDistance))
			{
				continue;
			}

			if (closestSqrDistance > Numeric::sqr(maximalProjectionError))
			{
				continue;
			}

			if (secondClosestSqrDistance <= closestSqrDistance * Numeric::sqr(Scalar(2))) // we want a unique match
			{
				continue;
			}

			const Point& closestPoint = points[closestPointIndex];

			if (closestPoint.sign() != boardMarker.pointSign<true>(indexInMarker))
			{
				continue;
			}

			const Vector2& imagePoint = closestPoint.observation();

			objectPointIds.emplace_back(additionalMarkerCoordinate, indexInMarker);

			objectPoints.push_back(markerObjectPoint);
			imagePoints.push_back(imagePoint);
		}

		constexpr size_t minimalNewCorrespondences = 21; // 21 out of 25 points to ensure that we don't use wrong points

		const size_t newCorrespondences = objectPointIds.size() - numberCorrespondencesAtStart;

		if (newCorrespondences > 0 && newCorrespondences < minimalNewCorrespondences)
		{
			// let's get rid of all correspondences we have added for this marker

			objectPointIds.resize(numberCorrespondencesAtStart);
			objectPoints.resize(numberCorrespondencesAtStart);
			imagePoints.resize(numberCorrespondencesAtStart);
		}
	}

	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(objectPointIds.size() == objectPoints.size());

	if (objectPoints.size() == objectPointsFromUsedMarkers)
	{
		Log::warning() << "No further improvements possible";

		// TODO handle differently?
		//return false;
	}

	HomogenousMatrix4 optimizedFlippedCamera_T_board;
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(camera, flippedCamera_T_board, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), optimizedFlippedCamera_T_board, 20u, Geometry::Estimator::ET_HUBER))
	{
		return false;
	}

	ocean_assert(optimizedFlippedCamera_T_board.isValid());

	board_T_optimizedCamera = Camera::invertedFlipped2Standard(optimizedFlippedCamera_T_board);

	if (usedObjectPointIds != nullptr || usedImagePoints != nullptr || usedObjectPoints != nullptr)
	{
		if (usedObjectPointIds != nullptr)
		{
			ocean_assert(usedObjectPointIds->empty());
			usedObjectPointIds->clear();
		}

		if (usedImagePoints != nullptr)
		{
			ocean_assert(usedImagePoints->empty());
			usedImagePoints->clear();
		}

		if (usedObjectPoints != nullptr)
		{
			ocean_assert(usedObjectPoints->empty());
			usedObjectPoints->clear();
		}

		for (size_t nCorrespondence = 0; nCorrespondence < objectPoints.size(); ++nCorrespondence)
		{
			const Vector3& objectPoint = objectPoints[nCorrespondence];
			const Vector2& imagePoint = imagePoints[nCorrespondence];

			if (Camera::isObjectPointInFrontIF(optimizedFlippedCamera_T_board, objectPoint))
			{
				const Vector2 projectedObjectPoint = camera.projectToImageIF(optimizedFlippedCamera_T_board, objectPoint);

				const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

				if (sqrDistance < Numeric::sqr(maximalProjectionError))
				{
					if (usedObjectPointIds != nullptr)
					{
						usedObjectPointIds->push_back(objectPointIds[nCorrespondence]);
					}

					if (usedObjectPoints != nullptr)
					{
						usedObjectPoints->push_back(objectPoints[nCorrespondence]);
					}

					if (usedImagePoints != nullptr)
					{
						usedImagePoints->push_back(imagePoints[nCorrespondence]);
					}
				}
			}
		}
	}

	return true;
}

bool MetricCalibrationBoard::createMetricCalibrationBoard(const unsigned int id, const size_t xMarkers, const size_t yMarkers, const MetricSize& measurementMetricIndicationWidth, const MetricSize& measurementMetricIndicationHeight, MetricCalibrationBoard& metricCalibrationBoard)
{
	ocean_assert(xMarkers >= 1 && yMarkers >= 1);
	ocean_assert(measurementMetricIndicationWidth.isValid() && measurementMetricIndicationHeight.isValid());

	if (xMarkers == 0 || yMarkers == 0 || !measurementMetricIndicationWidth.isValid() || !measurementMetricIndicationHeight.isValid())
	{
		return false;
	}

	CalibrationBoard calibrationBoard;
	if (!createCalibrationBoard(id, xMarkers, yMarkers, calibrationBoard))
	{
		return false;
	}

	metricCalibrationBoard = MetricCalibrationBoard(std::move(calibrationBoard), measurementMetricIndicationWidth, measurementMetricIndicationHeight);

	return true;
}

bool MetricCalibrationBoard::determineOptimalMarkerGrid(const MetricSize& paperWidth, const MetricSize& paperHeight, size_t& xMarkers, size_t& yMarkers, const MetricSize& minMarkerSize, const MetricSize& margin, const double paddingFactor)
{
	ocean_assert(paperWidth.isValid() && paperHeight.isValid());
	ocean_assert(minMarkerSize.isValid() && margin.isValid());
	ocean_assert(paddingFactor > 0.0);

	ocean_assert(minMarkerSize > MetricSize(0.0, MetricSize::UT_MILLIMETER));
	if (minMarkerSize <= MetricSize(0.0, MetricSize::UT_MILLIMETER))
	{
		return false;
	}

	const MetricSize contentWidth = paperWidth - margin * 2.0;
	const MetricSize contentHeight = paperHeight - margin * 2.0;

	const MetricSize minContentSize = minMarkerSize * (1.0 + paddingFactor * 2.0);

	if (contentWidth < minContentSize || contentHeight < minContentSize)
	{
		ocean_assert(false && "The provided paper size is too small!");
		return false;
	}

	const MetricSize maxMarkersWidth = contentWidth - minMarkerSize * 2.0 * paddingFactor;
	const MetricSize maxMarkersHeight = contentHeight - minMarkerSize * 2.0 * paddingFactor;

	const double xMarkersD = maxMarkersWidth / minMarkerSize;
	const double yMarkersD = maxMarkersHeight / minMarkerSize;

	ocean_assert(xMarkersD >= 1.0 && yMarkersD >= 1.0);
	if (xMarkersD < 1.0 || yMarkersD < 1.0)
	{
		return false;
	}

	xMarkers = size_t(xMarkersD);
	yMarkers = size_t(yMarkersD);

	return true;
}

}

}

}
