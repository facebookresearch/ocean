/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/CalibrationBoardDetector.h"
#include "ocean/cv/calibration/PointDetector.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

bool CalibrationBoardDetector::determineMarkerCandidates(const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maximalDistance, const Scalar maxDistancePercentage, MarkerCandidates& markerCandidates)
{
	ocean_assert(pointsDistributionArray.isValid());

	UsedPoints usedPoints(points.size(), 0u);

	ocean_assert(markerCandidates.empty());
	markerCandidates.clear();

	PointDetector::IndexDistancePairs<4> indexDistancePairs;

	ContinuousLineIndices continuousLineIndices;

	for (size_t pointIndex = 0; pointIndex < points.size(); ++pointIndex)
	{
		if (usedPoints[pointIndex] != 0u)
		{
			continue;
		}

		const Point& point = points[pointIndex];

		indexDistancePairs.weakClear();

		PointDetector::closestPoints<4, false>(pointsDistributionArray, pointIndex, points, indexDistancePairs, Numeric::sqr(maximalDistance));

		for (size_t nClosest = 0; nClosest < indexDistancePairs.size(); ++nClosest)
		{
			const Index32& closestIndex = indexDistancePairs[nClosest].first;
			ocean_assert(pointIndex != closestIndex);

			if (usedPoints[closestIndex] != 0u)
			{
				continue;
			}

			ocean_assert(closestIndex < points.size());
			const Point& closestPoint = points[closestIndex];

			if (point.sign() != closestPoint.sign())
			{
				continue;
			}

			continuousLineIndices.clear();
			if (determineContinuousLine(pointIndex, closestIndex, points, usedPoints, pointsDistributionArray, continuousLineIndices, maxDistancePercentage))
			{
				if (continuousLineIndices.size() == 5)
				{
					if (determineClosedRectangle(continuousLineIndices, points, usedPoints, pointsDistributionArray, maxDistancePercentage))
					{
						ocean_assert(continuousLineIndices.size() == 16);

						for (const Index32 continuousLineIndex : continuousLineIndices)
						{
							ocean_assert(usedPoints[continuousLineIndex] == 0u);
							usedPoints[continuousLineIndex] = 1u;
						}

						const Indices32 borderIndices(continuousLineIndices.cbegin(), continuousLineIndices.cend());

						markerCandidates.emplace_back(borderIndices, points);

						break;
					}
				}
			}
		}
	}

	CalibrationBoardDetector::determineMarkerCandidateNeighborhood(markerCandidates, points, maxDistancePercentage * Scalar(1.1));

	return true;
}

bool CalibrationBoardDetector::determineClosedRectangle(ContinuousLineIndices& lineIndices, const Points& points, const UsedPoints& usedPoints, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maxDistancePercentage)
{
	ocean_assert(lineIndices.size() == 5);
	if (lineIndices.size() != 5)
	{
		return false;
	}

	ocean_assert(points.size() == usedPoints.size());

	const bool sign = points[lineIndices.front()].sign();

#ifdef OCEAN_DEBUG
	for (const Index32 index : lineIndices)
	{
		ocean_assert(sign == points[index].sign());
	}
#endif

	bool allowReplacingInitialLine = true;

	ContinuousLineIndices perpendicularLineIndices;

	for (size_t nIteration = 1; nIteration < 4; ++nIteration)
	{
		const Index32 lastPointIndex = lineIndices[lineIndices.size() - 1];
		const Index32 secondLastPointIndex = lineIndices[lineIndices.size() - 2];

		ocean_assert(usedPoints[lastPointIndex] == 0u);
		ocean_assert(usedPoints[secondLastPointIndex] == 0u);

		const Vector2& lastPoint = points[lastPointIndex].observation();
		const Vector2& secondLastPoint = points[secondLastPointIndex].observation();

		const Vector2 endVector = lastPoint - secondLastPoint;

		bool foundNextLine = false;

		for (const Scalar leftRight : {1, -1})
		{
			perpendicularLineIndices.clear();

			const Vector2 predictedPoint = lastPoint + endVector.perpendicular() * leftRight;

			const Scalar maxSqrDistance = endVector.sqr() * Numeric::sqr(maxDistancePercentage);
			ocean_assert(Numeric::isWeakEqual(Numeric::sqrt(maxSqrDistance), endVector.length() * maxDistancePercentage));

			const size_t predictedPointIndex = PointDetector::closestPoint(predictedPoint, sign, pointsDistributionArray, points, maxSqrDistance);

			if (predictedPointIndex == size_t(-1))
			{
				continue;
			}

			if (usedPoints[predictedPointIndex] != 0u)
			{
				continue;
			}

			if (!determineContinuousLine(lastPointIndex, predictedPointIndex, points, usedPoints, pointsDistributionArray, perpendicularLineIndices, maxDistancePercentage))
			{
				continue;
			}

			if (perpendicularLineIndices.size() != 5)
			{
				continue;
			}

			if (perpendicularLineIndices.front() == lastPointIndex)
			{
				lineIndices.insert(lineIndices.cend(), perpendicularLineIndices.cbegin() + 1, perpendicularLineIndices.cend()); // the first point is already the last point

				foundNextLine = true;
				break;
			}
			else
			{
				// we have a T-like intersection

				if (nIteration == 1 && allowReplacingInitialLine)
				{
					// this first line is intersecting the second line in the middle of the second line
					// we replace the first line with the second line and start over again

					lineIndices = std::move(perpendicularLineIndices);
					nIteration = 0;

					foundNextLine = true;
					break;
				}
			}
		}

		allowReplacingInitialLine = false;

		if (!foundNextLine)
		{
			return false;
		}
	}

	ocean_assert(lineIndices.size() == 17); // the first and last point should be identical now

	if (lineIndices.front() == lineIndices.back())
	{
		lineIndices.pop_back();
		return true;
	}

	return false;
}

bool CalibrationBoardDetector::determineInitialCameraFieldOfView(const unsigned int width, const unsigned height, const Points& points, const MarkerCandidates& markerCandidates, const AnyCameraType anyCameraType, Scalar& fovX, const Scalar minFovX, const Scalar maxFovX, const size_t steps)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(!points.empty());
	ocean_assert(!markerCandidates.empty());

	ocean_assert(minFovX > Numeric::eps() && minFovX < maxFovX && maxFovX < Numeric::pi());
	ocean_assert(steps >= 2);

	if (minFovX <= Numeric::eps() || minFovX >= maxFovX || maxFovX >= Numeric::pi() || steps < 2)
	{
		return false;
	}

	RandomGenerator randomGenerator;

	const Vectors3 markerBorderObjectPoints =
	{
		Vector3(-2, 0, -2),
		Vector3(-1, 0, -2),
		Vector3(0, 0, -2),
		Vector3(1, 0, -2),
		Vector3(2, 0, -2),
		Vector3(2, 0, -1),
		Vector3(2, 0, 0),
		Vector3(2, 0, 1),
		Vector3(2, 0, 2),
		Vector3(1, 0, 2),
		Vector3(0, 0, 2),
		Vector3(-1, 0, 2),
		Vector3(-2, 0, 2),
		Vector3(-2, 0, 1),
		Vector3(-2, 0, 0),
		Vector3(-2, 0, -1)
	};

	SharedAnyCameras cameras;
	cameras.reserve(steps);

	for (size_t n = 0; n < steps; ++n)
	{
		const Scalar stepFovX = minFovX + (maxFovX - minFovX) * Scalar(n) / Scalar(steps - 1);

		if (anyCameraType == AnyCameraType::FISHEYE)
		{
			cameras.emplace_back(std::make_shared<AnyCameraFisheye>(FisheyeCamera(width, height, stepFovX)));
		}
		else
		{
			ocean_assert(anyCameraType == AnyCameraType::PINHOLE);
			cameras.emplace_back(std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, stepFovX)));
		}
	}

	using Vote = std::pair<unsigned int, Scalar>;
	using Votes = std::vector<Vote>;

	Votes votes(cameras.size(), Vote(0u, Scalar(0)));

	constexpr Scalar sqrPixelError = Numeric::sqr(15);

	Vectors3 objectPoints;
	Vectors2 imagePoints;

	objectPoints.reserve(markerBorderObjectPoints.size() * 5);
	imagePoints.reserve(markerBorderObjectPoints.size() * 5);

	for (size_t markerCandidateIndex = 0; markerCandidateIndex < markerCandidates.size(); ++markerCandidateIndex)
	{
		const MarkerCandidate& markerCandidate = markerCandidates[markerCandidateIndex];
		ocean_assert(markerCandidate.isValid());

		objectPoints.clear();
		objectPoints.insert(objectPoints.cend(), markerBorderObjectPoints.cbegin(), markerBorderObjectPoints.cend());

		imagePoints.clear();

		for (size_t borderIndex = 0; borderIndex < 16; ++borderIndex)
		{
			const Index32 pointIndex = markerCandidate.borderIndex(borderIndex);

			ocean_assert(pointIndex < points.size());

			const Vector2& markerBorderImagePoint = points[pointIndex].observation();

			imagePoints.push_back(markerBorderImagePoint);
		}

		if (markerCandidate.hasNeighbor())
		{
			const Vector2 directions[4] =
			{
				Vector2(0, -1), // north, 0 deg
				Vector2(-1, 0), // west, 90 deg
				Vector2(0, 1), // south, 180 deg
				Vector2(1, 0) // east, 270 deg
			};

			for (const MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighbors())
			{
				const CV::PixelDirection localEdge = neighborPair.first;
				const size_t neighborMarkerCandidateIndex = neighborPair.second;
				ocean_assert(neighborMarkerCandidateIndex != markerCandidateIndex);

				const MarkerCandidate& neighborMarkerCandidate = markerCandidates[neighborMarkerCandidateIndex];

				const CV::PixelDirection neighborEdge = neighborMarkerCandidate.neighborDirection(markerCandidateIndex);
				ocean_assert(neighborEdge != CV::PixelDirection::PD_INVALID);

				ocean_assert(localEdge >= 0 && localEdge <= 270);
				if (localEdge < 0 || localEdge > 270)
				{
					return false;
				}

				constexpr Scalar neighborDistance = 5;

				const Vector2 neighborOffset = directions[localEdge / 90] * neighborDistance;

				const int neighborRotationAngle = modulo<int>(int(localEdge) + 180 - int(neighborEdge), 360);

				MarkerCandidate rotatedNeighborhoodMarkerCandidate = neighborMarkerCandidate.rotatedClockWise(-neighborRotationAngle);

				for (const Vector3& objectPoint : markerBorderObjectPoints)
				{
					objectPoints.emplace_back(Vector3(neighborOffset.x(), 0, neighborOffset.y()) + objectPoint);
				}

				for (size_t indexInBorder = 0; indexInBorder < 16; ++indexInBorder)
				{
					const Index32 pointIndex = rotatedNeighborhoodMarkerCandidate.borderIndex(indexInBorder);

					const Vector2& markerBorderImagePoint = points[pointIndex].observation();

					imagePoints.emplace_back(markerBorderImagePoint);
				}
			}
		}

		ocean_assert(objectPoints.size() >= 16 && objectPoints.size() % 16 == 0);
		ocean_assert(objectPoints.size() == imagePoints.size());

		for (size_t cameraIndex = 0; cameraIndex < cameras.size(); ++cameraIndex)
		{
			const AnyCamera& camera = *(cameras[cameraIndex]);

			HomogenousMatrix4 world_T_camera;

			// we determine the initial camera pose only on the first 16 point correspondences (on marker)

			Scalar sqrAccuracy;
			if (Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints.data(), 16), ConstArrayAccessor<Vector2>(imagePoints.data(), 16), randomGenerator, world_T_camera, 16u, false /*refine*/, 20u, sqrPixelError, nullptr, &sqrAccuracy))
			{
				// now, let's optimized the pose, but we use all correspondences (in case we have more than one marker)

				const HomogenousMatrix4 flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

				HomogenousMatrix4 optimizedFlippedCamera_T_world(false);
				Scalar finalError = Numeric::maxValue();
				if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(camera, flippedCamera_T_world, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), optimizedFlippedCamera_T_world, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10), nullptr, &finalError))
				{
					ocean_assert(false && "This should never happen!");

					return false;
				}

				sqrAccuracy = finalError;

				Vote& vote = votes[cameraIndex];

				++vote.first;
				vote.second += sqrAccuracy;
			}
		}
	}

	size_t bestCameraIndex = size_t(-1);
	Scalar bestError = Numeric::maxValue();

	for (size_t cameraIndex = 0; cameraIndex < cameras.size(); ++cameraIndex)
	{
		const Vote& vote = votes[cameraIndex];

		const Scalar averageError = Numeric::ratio(vote.second, Scalar(vote.first), Numeric::maxValue()); // TODO check minimal number of votes?

		if (averageError < bestError)
		{
			bestCameraIndex = cameraIndex;
			bestError = averageError;
		}
	}

	if (bestCameraIndex >= cameras.size())
	{
		return false;
	}

	fovX = cameras[bestCameraIndex]->fovX();

	return true;
}

bool CalibrationBoardDetector::determineCameraPoseForMarker(const AnyCamera& camera, const MarkerCandidate& markerCandidate, const Points& points, HomogenousMatrix4& markerCandidate_T_camera, const Scalar maximalProjectionError)
{
	ocean_assert(camera.isValid());

	ocean_assert(markerCandidate.isValid());
	ocean_assert(markerCandidate.hasValidBorderIndices());

	RandomGenerator randomGenerator;

	Vectors3 objectPoints;
	Vectors2 imagePoints;

	objectPoints.reserve(25);
	imagePoints.reserve(25);

	size_t indexInMarker = 0;

	for (int32_t z = -2; z <= 2; ++z)
	{
		for (int32_t x = -2; x <= 2; ++x)
		{
			const Index32 pointIndex = markerCandidate.pointIndex(indexInMarker++);

			if (pointIndex == Index32(-1))
			{
				continue;
			}

			ocean_assert(pointIndex < points.size());

			objectPoints.emplace_back(Scalar(x), 0, Scalar(z));
			imagePoints.emplace_back(points[pointIndex].observation());
		}
	}

	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(objectPoints.size() >= 16 && objectPoints.size() <= 25);

	return Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, markerCandidate_T_camera, (unsigned int)(objectPoints.size()), true /*refine*/, 20u, Numeric::sqr(maximalProjectionError));
}

bool CalibrationBoardDetector::determineRemainingMarkerPointIndices(const AnyCamera& camera, const HomogenousMatrix4& markerCandidate_T_camera, MarkerCandidate& markerCandidate, const Points& points, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Scalar maximalProjectionError)
{
	ocean_assert(camera.isValid());
	ocean_assert(markerCandidate_T_camera.isValid());

	ocean_assert(markerCandidate.isValid());
	ocean_assert(markerCandidate.hasValidBorderIndices());

	ocean_assert(!markerCandidate.hasValidIndices());
	ocean_assert(!markerCandidate.hasMarkerId());
	ocean_assert(!markerCandidate.hasMarkerCoordinate());

	if (!markerCandidate.isValid())
	{
		return false;
	}

	UnorderedIndexSet32 pointIndexSet;
	pointIndexSet.reserve(25);

	for (size_t indexInBorder = 0; indexInBorder < 16; ++indexInBorder)
	{
		const Index32 pointIndex = markerCandidate.borderIndex(indexInBorder);

		pointIndexSet.insert(pointIndex);
	}

	if (pointIndexSet.size() != 16)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_markerCandidate = Camera::standard2InvertedFlipped(markerCandidate_T_camera);

	Index32 yIndexInMarker = 1u;

	for (int z = -1; z <= 1; ++z)
	{
		Index32 xIndexInMarker = 1u;

		for (int x = -1; x <= 1; ++x)
		{
			const Vector3 markerObjectPoint(Scalar(x), 0, Scalar(z));

			const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_markerCandidate, markerObjectPoint);

			if (!camera.isInside(projectedObjectPoint))
			{
				return false;
			}

			Index32 closestPointIndex = Index32(-1);
			Index32 secondClosestPointIndex = Index32(-1);

			Scalar closestSqrDistance = Numeric::maxValue();
			Scalar secondClosestSqrDistance = Numeric::maxValue();

			if (!PointDetector::closestPoints(projectedObjectPoint, pointsDistributionArray, points, closestPointIndex, secondClosestPointIndex, closestSqrDistance, secondClosestSqrDistance))
			{
				return false;
			}

			ocean_assert(closestPointIndex < points.size());

			if (closestSqrDistance > Numeric::sqr(maximalProjectionError))
			{
				return false;
			}

			if (secondClosestSqrDistance <= closestSqrDistance * Numeric::sqr(Scalar(2))) // we want a unique match
			{
				return false;
			}

			if (!pointIndexSet.emplace(closestPointIndex).second)
			{
				// we are re-using a point from the border
				return false;
			}

			// we cannot yet verify the sign of the point, as we do not yet know the id of the marker candidate

			const Index32 indexInMarker = yIndexInMarker * 5u + xIndexInMarker;

			markerCandidate.setPointIndex(indexInMarker, closestPointIndex);

			++xIndexInMarker;
		}

		++yIndexInMarker;
	}

	ocean_assert(pointIndexSet.size() == 25);

	return true;
}

bool CalibrationBoardDetector::determineStartMarkerCandidates(const unsigned int width, const unsigned int height, const MarkerCandidates& markerCandidates, const Points& points, Indices32& bestMarkerCandidateIndices)
{
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(!markerCandidates.empty());
	if (markerCandidates.empty())
	{
		return false;
	}

	Index32 bestMarkerIndex = Index32(-1);
	StaticVector<Index32, 4> bestNeighborIndices;
	Scalar bestSqrDistanceToCenter = Numeric::maxValue();

	StaticVector<Index32, 4> candidateNeighborIndices;

	const Vector2 imageCenter = Vector2(Scalar(width), Scalar(height)) / Scalar(2);

	for (size_t candidateIndex = 0; candidateIndex < markerCandidates.size(); ++candidateIndex)
	{
		const MarkerCandidate& markerCandidate = markerCandidates[candidateIndex];
		ocean_assert(markerCandidate.isValid());

		if (!markerCandidate.hasMarkerId())
		{
			continue;
		}

		if (!markerCandidate.hasMarkerCoordinate())
		{
			continue;
		}

		candidateNeighborIndices.weakClear();
		for (const MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighbors())
		{
			const size_t neighborMarkerCandidateIndex = neighborPair.second;
			ocean_assert(neighborMarkerCandidateIndex < markerCandidates.size());

			if (markerCandidates[neighborMarkerCandidateIndex].hasMarkerId())
			{
				if (markerCandidates[neighborMarkerCandidateIndex].hasMarkerCoordinate())
				{
					candidateNeighborIndices.pushBack(Index32(neighborMarkerCandidateIndex));
				}
			}
		}

		if (bestNeighborIndices.size() <= candidateNeighborIndices.size())
		{
			const Scalar candidateSqrDistanceToCenter = imageCenter.sqrDistance(markerCandidate.center(points));

			if (bestNeighborIndices.size() < candidateNeighborIndices.size() || candidateSqrDistanceToCenter < bestSqrDistanceToCenter)
			{
				// we either found more neighbors, or we have the same number of neighbors but more in the center of the image

				bestMarkerIndex = Index32(candidateIndex);
				bestNeighborIndices = candidateNeighborIndices;
				bestSqrDistanceToCenter = candidateSqrDistanceToCenter;
			}
		}
	}

	if (bestMarkerIndex == Index32(-1))
	{
		return false;
	}

	ocean_assert(bestMarkerCandidateIndices.empty());
	bestMarkerCandidateIndices.clear();
	bestMarkerCandidateIndices.reserve(bestNeighborIndices.size() + 1);

	bestMarkerCandidateIndices.push_back(bestMarkerIndex);

	for (size_t n = 0; n < bestNeighborIndices.size(); ++n)
	{
		bestMarkerCandidateIndices.push_back(bestNeighborIndices[n]);
	}

	return true;
}

bool CalibrationBoardDetector::detectCalibrationBoard(const AnyCamera& camera, const Frame& yFrame, const MetricCalibrationBoard& calibrationBoard, CalibrationBoardObservation& observation, const Scalar maximalProjectionError, Worker* worker)
{
	ocean_assert(camera.isValid());
	if (!camera.isValid())
	{
		return false;
	}

	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(yFrame.width() == camera.width() && yFrame.height() == camera.height());
	ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	if (!yFrame.isFrameTypeCompatible(FrameType(camera.width(), camera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), false))
	{
		return false;
	}

	ocean_assert(calibrationBoard.isValid());
	if (!calibrationBoard.isValid())
	{
		return false;
	}

	PointDetector pointDetector;
	if (!pointDetector.detectPoints(yFrame, worker))
	{
		return false;
	}

	const Points& points = pointDetector.points();
	const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray = pointDetector.pointsDistributionArray();

	constexpr Scalar maximalDistance = Scalar(50);

	constexpr Scalar maxDistancePercentage = Scalar(0.25);

	MarkerCandidates markerCandidates;
	if (!CalibrationBoardDetector::determineMarkerCandidates(points, pointsDistributionArray, maximalDistance, maxDistancePercentage, markerCandidates))
	{
		return false;
	}

	if (markerCandidates.empty())
	{
		return false;
	}

	for (size_t nMarkerCandidate = 0; nMarkerCandidate < markerCandidates.size(); /*noop*/)
	{
		MarkerCandidate& markerCandidate = markerCandidates[nMarkerCandidate];

		HomogenousMatrix4 markerCandidate_T_camera(false);
		if (CalibrationBoardDetector::determineCameraPoseForMarker(camera, markerCandidate, points, markerCandidate_T_camera, Scalar(maximalProjectionError)))
		{
			if (CalibrationBoardDetector::determineRemainingMarkerPointIndices(camera, markerCandidate_T_camera, markerCandidate, points, pointsDistributionArray, maximalProjectionError))
			{
				if (markerCandidate.determineMarkerId(points))
				{
					++nMarkerCandidate;
					continue;
				}
			}
		}

		MarkerCandidate::removeMarkerCandidate(markerCandidates, nMarkerCandidate);
	}

	if (markerCandidates.empty())
	{
		return false;
	}

	// now let's determine the correctness of the neighborhood of marker candidates


	Indices32 markerCandidatesWithCoordinates;
	markerCandidatesWithCoordinates.reserve(markerCandidates.size());


	for (size_t markerCandidateIndex = 0; markerCandidateIndex < markerCandidates.size(); /*noop*/)
	{
		bool keepMarkerCandidate = false;

		CV::Calibration::MarkerCandidate& markerCandidate = markerCandidates[markerCandidateIndex];

		if (markerCandidate.hasMarkerId())
		{
			ocean_assert(markerCandidate.neighbors().size() <= 4);

			if (markerCandidate.hasNeighborWithMarkerId(markerCandidates))
			{
				CalibrationBoard::MarkerCoordinate markerCoordinate;
				CalibrationBoard::NeighborMarkerCoordinateMap neighborMarkerCoordinateMap;

				if (calibrationBoard.containsMarkerCandidateWithNeighborhood(markerCandidates, markerCandidateIndex, &markerCoordinate, &neighborMarkerCoordinateMap))
				{
					if (neighborMarkerCoordinateMap.size() >= 1) // TODO expect more neighbors?
					{
						markerCandidate.setMarkerCoordinate(markerCoordinate);

						markerCandidatesWithCoordinates.emplace_back(Index32(markerCandidateIndex));

						keepMarkerCandidate = true;
					}
				}
			}
		}

		if (keepMarkerCandidate)
		{
			++markerCandidateIndex;
		}
		else
		{
			MarkerCandidate::removeMarkerCandidate(markerCandidates, markerCandidateIndex);
		}
	}

	if (markerCandidatesWithCoordinates.empty())
	{
		return false;
	}

	RandomGenerator randomGenerator;
	HomogenousMatrix4 board_T_camera(false);

	CalibrationBoard::ObjectPointIds usedObjectPointIds;
	Vectors3 usedObjectPoints;
	Vectors2 usedImagePoints;

	if (!calibrationBoard.determineCameraPose(camera, ConstArrayAccessor<MarkerCandidate>(markerCandidates), points, randomGenerator, board_T_camera, maximalProjectionError, nullptr, &usedObjectPointIds, &usedObjectPoints, &usedImagePoints))
	{
		return false;
	}

	observation = CalibrationBoardObservation(0, camera.clone(), board_T_camera, std::move(usedObjectPointIds), std::move(usedObjectPoints), std::move(usedImagePoints));

	return true;
}

bool CalibrationBoardDetector::determineContinuousLine(const size_t pointIndexA, const size_t pointIndexB, const Points& points, const UsedPoints& usedPoints, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, ContinuousLineIndices& lineIndices, const Scalar maxDistancePercentage)
{
	ocean_assert(pointIndexA != pointIndexB);
	ocean_assert(pointIndexA < points.size() && pointIndexB < points.size());

	ocean_assert(points.size() == usedPoints.size());
	ocean_assert(usedPoints[pointIndexA] == 0u);
	ocean_assert(usedPoints[pointIndexB] == 0u);

	ocean_assert(lineIndices.empty());

	const Point& pointA = points[pointIndexA];
	const Point& pointB = points[pointIndexB];

	const bool sign = pointA.sign();

	ocean_assert(sign == pointB.sign());

	const Vector2 offset = pointB.observation() - pointA.observation();

	constexpr Scalar minDistance = Scalar(5);

	if (offset.sqr() < Numeric::sqr(minDistance))
	{
		return false;
	}

	lineIndices.push_back(Index32(pointIndexA));
	lineIndices.push_back(Index32(pointIndexB));

	Vector2 updatedOffset = offset;

	for (unsigned int nPositive = 0u; nPositive < 4u; ++nPositive)
	{
		const Vector2& previousPoint = points[lineIndices.back()].observation();

		const Vector2 precitedPoint = previousPoint + updatedOffset;

		const Scalar maxSqrDistance = updatedOffset.sqr() * Numeric::sqr(maxDistancePercentage);
		ocean_assert(Numeric::isWeakEqual(Numeric::sqrt(maxSqrDistance), updatedOffset.length() * maxDistancePercentage));

		const size_t index = PointDetector::closestPoint(precitedPoint, sign, pointsDistributionArray, points, maxSqrDistance);

		if (index == size_t(-1))
		{
			break;
		}

		if (usedPoints[index] != 0u)
		{
			break;
		}

		if (Index32(index) == lineIndices.back())
		{
			break;
		}

		const Vector2& nextPoint = points[index].observation();

		updatedOffset = nextPoint - previousPoint;
		ocean_assert(updatedOffset * offset > 0);

		lineIndices.push_back(Index32(index));
	}

	updatedOffset = -offset;

	for (unsigned int nNegative = 0u; nNegative < 4u; ++nNegative)
	{
		const Vector2& previousPoint = points[lineIndices.front()].observation();

		const Vector2 precitedPoint = previousPoint + updatedOffset;

		const Scalar maxSqrDistance = updatedOffset.sqr() * Numeric::sqr(maxDistancePercentage);
		ocean_assert(Numeric::isWeakEqual(Numeric::sqrt(maxSqrDistance), updatedOffset.length() * maxDistancePercentage));

		const size_t index = PointDetector::closestPoint(precitedPoint, sign, pointsDistributionArray, points, maxSqrDistance);

		if (index == size_t(-1))
		{
			break;
		}

		if (usedPoints[index] != 0u)
		{
			break;
		}

		if (Index32(index) == lineIndices.front())
		{
			break;
		}

		const Vector2& nextPoint = points[index].observation();

		updatedOffset = nextPoint - previousPoint;
		ocean_assert(updatedOffset * -offset > 0);

		lineIndices.push_front(Index32(index));
	}

	return lineIndices.size() > 2;
}

void CalibrationBoardDetector::determineMarkerCandidateNeighborhood(MarkerCandidates& markerCandidates, const Points& points, const Scalar maxDistancePercentage)
{
	if (markerCandidates.size() <= 1)
	{
		return;
	}

	for (size_t nOuter = 0; nOuter < markerCandidates.size(); ++nOuter)
	{
		MarkerCandidate& outerMarkerCandidate = markerCandidates[nOuter];

		for (size_t nInner = nOuter + 1; nInner < markerCandidates.size(); ++nInner)
		{
			MarkerCandidate& innerMarkerCandidate = markerCandidates[nInner];

			CV::PixelDirection localEdge = CV::PD_INVALID;
			CV::PixelDirection neighborEdge = CV::PD_INVALID;

			if (outerMarkerCandidate.isNeighbor(innerMarkerCandidate, points, localEdge, neighborEdge, maxDistancePercentage))
			{
				outerMarkerCandidate.addNeighbor(localEdge, nInner);
				innerMarkerCandidate.addNeighbor(neighborEdge, nOuter);
			}
		}
	}
}

}

}

}
