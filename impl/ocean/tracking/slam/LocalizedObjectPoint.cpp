/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/PoseQualityCalculator.h"

#include "ocean/base/Median.h"
#include "ocean/base/Subset.h"

#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

void LocalizedObjectPoint::CorrespondenceData::reset()
{
	objectPoints_.clear();
	imagePoints_.clear();
	objectPointIds_.clear();
	localizationPrecisions_.clear();
	imagePointSqrDistances_.clear();

	usedIndices_.clear();

	badObjectPointIds_.clear();

	posePreciseObjectPointIds_.clear();
	poseNotPreciseObjectPointIds_.clear();
}

void LocalizedObjectPoint::CorrespondenceData::applySubset()
{
#ifdef OCEAN_DEBUG
	// the used indices are expected to be sorted
	for (size_t n = 1; n < usedIndices_.size(); ++n)
	{
		ocean_assert(usedIndices_[n - 1] < usedIndices_[n]);
	}
#endif

	ocean_assert(usedIndices_.size() <= objectPointIds_.size());

	if (usedIndices_.size() != objectPointIds_.size())
	{
		ocean_assert(badObjectPointIds_.empty());
		badObjectPointIds_.reserve(objectPointIds_.size() - usedIndices_.size());

		Indices32::const_iterator iUsed = usedIndices_.cbegin();

		for (size_t nId = 0; nId < objectPointIds_.size(); ++nId)
		{
			if (iUsed == usedIndices_.end())
			{
				while (nId < objectPointIds_.size())
				{
					badObjectPointIds_.emplace_back(objectPointIds_[nId]);
					++nId;
				}

				break;
			}

			ocean_assert(Index32(nId) <= *iUsed);

			if (Index32(nId) < *iUsed)
			{
				badObjectPointIds_.emplace_back(objectPointIds_[nId]);
			}
			else
			{
				++iUsed;
			}
		}

		ocean_assert(UnorderedIndexSet32(usedIndices_.cbegin(), usedIndices_.cend()).size() + UnorderedIndexSet32(badObjectPointIds_.cbegin(), badObjectPointIds_.cend()).size() == objectPointIds_.size());

		Subset::applySubset(objectPoints_, usedIndices_);
		Subset::applySubset(imagePoints_, usedIndices_);
		Subset::applySubset(objectPointIds_, usedIndices_);
		Subset::applySubset(localizationPrecisions_, usedIndices_);

		if (!imagePointSqrDistances_.empty())
		{
			Subset::applySubset(imagePointSqrDistances_, usedIndices_);
		}
	}

	usedIndices_.clear();
}

LocalizedObjectPoint::LocalizedObjectPoint(const PointTrack& pointTrack)
{
	ocean_assert(pointTrack.isValid());

	const Vectors2& imagePoints = pointTrack.imagePoints();
	ocean_assert(imagePoints.size() >= 2);

	observationMap_.reserve(imagePoints.size());

	for (size_t nImagePoint = 0; nImagePoint < imagePoints.size(); ++nImagePoint)
	{
		const Index32 frameIndex = pointTrack.firstFrameIndex() + Index32(nImagePoint);
		const Vector2& imagePoint = imagePoints[nImagePoint];

		ocean_assert(observationMap_.find(frameIndex) == observationMap_.cend());

		observationMap_.emplace(frameIndex, imagePoint);
	}

	lastObservationFrameIndex_ = pointTrack.lastFrameIndex();
}

void LocalizedObjectPoint::addObservations(const PointTrack& pointTrack)
{
	const Index32 firstFrameIndex = pointTrack.firstFrameIndex();
	const Vectors2& imagePoints = pointTrack.imagePoints();

	for (size_t nObservation = 0; nObservation < imagePoints.size(); ++nObservation)
	{
		const Index32 frameIndex = firstFrameIndex + Index32(nObservation);
		const Vector2& imagePoint = imagePoints[nObservation];

		ocean_assert(!hasObservation(frameIndex));

		addObservation(frameIndex, imagePoint);
	}
}

LocalizedObjectPoint::OptimizationResult LocalizedObjectPoint::optimizedObjectPoint(const Index32 mapVersion, const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 currentFrameIndex, const size_t minimalNumberObservations, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType, Vector3& optimizedPosition) const
{
	ocean_assert(camera.isValid());
	ocean_assert(cameraPoses.size() >= 2);
	ocean_assert(minimalNumberObservations >= 2);
	ocean_assert(maximalProjectionError >= Scalar(0));

	if (cameraPoses.size() < minimalNumberObservations)
	{
		return OR_NOT_ENOUGH_OBSERVATIONS;
	}

	ocean_assert_and_suppress_unused(hasObservation(currentFrameIndex), currentFrameIndex);

	Vectors2 imagePoints; // TODO make reusable
	HomogenousMatrices4 flippedCameras_T_world;

	imagePoints.reserve(observationMap_.size());
	flippedCameras_T_world.reserve(observationMap_.size());

	for (const ObservationMap::value_type& observationPair : observationMap_)
	{
		const Index32& frameIndex = observationPair.first;

		SharedCameraPose cameraPose;
		if (!cameraPoses.hasPose(Index32(frameIndex), cameraPose))
		{
			continue;
		}

		if (cameraPose->mapVersion() != mapVersion)
		{
			continue;
		}

		const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();

		if (!Camera::isObjectPointInFrontIF(flippedCamera_T_world, position_))
		{
			return OR_INACCURATE;
		}

		ocean_assert(cameraPose != nullptr);

		const Vector2& imagePoint = observationPair.second;

		imagePoints.push_back(imagePoint);
		flippedCameras_T_world.push_back(cameraPose->flippedCamera_T_world());
	}

	if (imagePoints.size() < minimalNumberObservations)
	{
		return OR_NOT_ENOUGH_OBSERVATIONS;
	}

	Vector3 internalOptimizedPosition;

	Scalar initialError = Numeric::maxValue();
	Scalar finalError = Numeric::maxValue();
	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(camera, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), position_, ConstArrayAccessor<Vector2>(imagePoints), internalOptimizedPosition, 10u, estimatorType, Scalar(0.001), Scalar(5), true, &initialError, &finalError))
	{
		return OR_INACCURATE;
	}

	for (size_t nCorrespondence = 0; nCorrespondence < imagePoints.size(); ++nCorrespondence)
	{
		const Vector2& imagePoint = imagePoints[nCorrespondence];
		const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[nCorrespondence];

		if (!Camera::isObjectPointInFrontIF(flippedCamera_T_world, internalOptimizedPosition))
		{
			// may happen due to robust estimator
			return OR_INACCURATE;
		}

		const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, internalOptimizedPosition);

		const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

		if (sqrDistance > Numeric::sqr(maximalProjectionError))
		{
			return OR_INACCURATE;
		}
	}

	optimizedPosition = internalOptimizedPosition;

	return OR_SUCCEEDED;
}

bool LocalizedObjectPoint::updateLocalizedObjectPointUncertainty(const AnyCamera& camera, const CameraPoses& cameraPoses)
{
	ocean_assert(position_ != Vector3::minValue());

	ocean_assert(camera.isValid());
	ocean_assert(!cameraPoses.isEmpty());

	if (localizationPrecision_ >= LP_HIGH)
	{
		ocean_assert(localizationPrecision_ == LP_HIGH);

		// the precision is already high, we don't expect that it can drop
		return false;
	}

	SquareMatrix3 covarianceMatrix(false);

	size_t numberPoses = 0;

	Scalar jacobian[6]; // 2x3 point Jacobian

	for (const ObservationMap::value_type& observationPair : observationMap_)
	{
		const Index32& frameIndex = observationPair.first;

		SharedCameraPose cameraPose;
		if (!cameraPoses.hasPose(frameIndex, cameraPose))
		{
			continue;
		}

		const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();
		ocean_assert(flippedCamera_T_world.isValid());

		const Vector3 cameraObjectPoint = flippedCamera_T_world * position_;

		camera.pointJacobian2x3IF(cameraObjectPoint, jacobian, jacobian + 3);

		// Jacobian J is 2x3
		// covarianceMatrix = sum (J^T * J)

		// 0 3
		// 1 4    *  0 1 2
		// 2 5       3 4 5

		covarianceMatrix(0, 0) += jacobian[0] * jacobian[0] + jacobian[3] * jacobian[3];
		covarianceMatrix(0, 1) += jacobian[0] * jacobian[1] + jacobian[3] * jacobian[4];
		covarianceMatrix(0, 2) += jacobian[0] * jacobian[2] + jacobian[3] * jacobian[5];

		//covarianceMatrix(1, 0) += jacobian[1] * jacobian[0] + jacobian[4] * jacobian[3]; // the covariance matrix is symmetric
		covarianceMatrix(1, 1) += jacobian[1] * jacobian[1] + jacobian[4] * jacobian[4];
		covarianceMatrix(1, 2) += jacobian[1] * jacobian[2] + jacobian[4] * jacobian[5];

		//covarianceMatrix(2, 0) += jacobian[2] * jacobian[0] + jacobian[5] * jacobian[3];
		//covarianceMatrix(2, 1) += jacobian[2] * jacobian[1] + jacobian[5] * jacobian[4];
		covarianceMatrix(2, 2) += jacobian[2] * jacobian[2] + jacobian[5] * jacobian[5];

		++numberPoses;
	}

	ocean_assert(numberPoses >= 2);
	if (numberPoses <= 1) // TODO, is this case really possible?
	{
		constexpr LocalizationPrecision newPrecision = LP_UNKNOWN;

		const bool result = localizationPrecision_ != newPrecision;
		localizationPrecision_ = newPrecision;

		return result;
	}

	constexpr size_t lowNumberPosesThreshold = 5; // TODO tweak threshold

	if (numberPoses <= lowNumberPosesThreshold)
	{
		constexpr LocalizationPrecision newPrecision = LP_LOW;

		const bool result = localizationPrecision_ != newPrecision;
		localizationPrecision_ = newPrecision;

		return result;
	}

	ocean_assert(!covarianceMatrix.isNull());

	const Scalar maxAbsValue = std::max(Numeric::abs(covarianceMatrix(0, 0)), std::max(Numeric::abs(covarianceMatrix(0, 1)), std::max(Numeric::abs(covarianceMatrix(0, 2)), std::max(Numeric::abs(covarianceMatrix(1, 1)), std::max(Numeric::abs(covarianceMatrix(1, 2)), Numeric::abs(covarianceMatrix(2, 2)))))));

	if (Numeric::isNotEqualEps(maxAbsValue))
	{
		const Scalar normalization = Scalar(1) / maxAbsValue;

		covarianceMatrix(0, 0) *= normalization;
		covarianceMatrix(0, 1) *= normalization;
		covarianceMatrix(0, 2) *= normalization;
		covarianceMatrix(1, 1) *= normalization;
		covarianceMatrix(1, 2) *= normalization;
		covarianceMatrix(2, 2) *= normalization;
	}

	covarianceMatrix(1, 0) = covarianceMatrix(0, 1);
	covarianceMatrix(2, 0) = covarianceMatrix(0, 2);
	covarianceMatrix(2, 1) = covarianceMatrix(1, 2);

	if (!covarianceMatrix.invert())
	{
		ocean_assert(localizationPrecision_ <= LP_LOW);

		constexpr LocalizationPrecision newPrecision = LP_UNKNOWN;

		const bool result = localizationPrecision_ != newPrecision;
		localizationPrecision_ = newPrecision;

		return result;
	}

	const LocalizationPrecision newPrecision = determineLocalizedObjectPointUncertainty(covarianceMatrix);

	const bool result = localizationPrecision_ != newPrecision;
	localizationPrecision_ = newPrecision;

	return result;
}

Scalar LocalizedObjectPoint::determineMedianViewingAngle(const CameraPoses& cameraPoses) const
{
	if (position_ == Vector3::minValue() || observationMap_.size() < 2)
	{
		return Scalar(0);
	}

	Vectors3 rays;
	rays.reserve(observationMap_.size());

	Vector3 meanDirection(0, 0, 0);

	for (const ObservationMap::value_type& observationPair : observationMap_)
	{
		const Index32 frameIndex = observationPair.first;

		SharedCameraPose cameraPose;
		if (cameraPoses.hasPose(frameIndex, cameraPose))
		{
			ocean_assert(cameraPose != nullptr);
			const Vector3 ray = (cameraPose->world_T_camera().translation() - position_).normalizedOrZero();

			if (!ray.isNull())
			{
				rays.emplace_back(ray);
				meanDirection += ray;
			}
		}
	}

	if (rays.size() < 2)
	{
		return Scalar(0);
	}

	if (!meanDirection.normalize())
	{
		return Scalar(0);
	}

	Scalars angles;
	angles.reserve(rays.size());

	for (const Vector3& ray : rays)
	{
		angles.push_back(meanDirection.angle(ray));
	}

	if (angles.empty())
	{
		return Scalar(0);
	}

	return Median::median(angles.data(), angles.size());
}

size_t LocalizedObjectPoint::determineCameraPoseQuality(const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, Scalar& minError, Scalar& averageError, Scalar& maxError)
{
	ocean_assert(camera.isValid());

	SharedCameraPose cameraPose;
	if (!cameraPoses.hasPose(frameIndex, cameraPose))
	{
		return 0;
	}

	ocean_assert(cameraPose != nullptr);

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();

	return determineCameraPoseQualityIF(camera, flippedCamera_T_world, frameIndex, localizedObjectPointMap, minError, averageError, maxError);
}

size_t LocalizedObjectPoint::determineCameraPoseQualityIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, Scalar& minError, Scalar& averageError, Scalar& maxError)
{
	ocean_assert(camera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());

	minError = Numeric::maxValue();
	averageError = Scalar(0);
	maxError = Numeric::minValue();

	size_t numberObservations = 0;

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		const Vector3& position = localizedObjectPoint.position();
		ocean_assert(position != Vector3::minValue());

		Vector2 imagePoint;
		if (localizedObjectPoint.hasObservation(frameIndex, &imagePoint))
		{
			if (!AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
			{
				Log::debug() << "LocalizedObjectPoint::determineCameraPoseQualityIF(): Object point is not in front of the camera, this should never happen!";
				ocean_assert(false && "This should never happen!");

				continue;
			}

			const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, position);

			const Scalar distance = imagePoint.distance(projectedObjectPoint);

			minError = std::min(minError, distance);
			averageError += distance;
			maxError = std::max(maxError, distance);

			++numberObservations;
		}
	}

	if (numberObservations > 0)
	{
		averageError /= Scalar(numberObservations);
	}

	return numberObservations;
}

void LocalizedObjectPoint::determineObjectPointQualityIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const Scalar maximalProjectionError, Indices32& validObjectPointIds, Indices32& invalidObjectPointIds)
{
	ocean_assert(camera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(maximalProjectionError >= 0);

	validObjectPointIds.clear();
	invalidObjectPointIds.clear();

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const Index32 objectPointId = objectPointPair.first;
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		const Vector3& position = localizedObjectPoint.position();
		ocean_assert(position != Vector3::minValue());

		Vector2 imagePoint;
		if (localizedObjectPoint.hasObservation(frameIndex, &imagePoint))
		{
			if (!AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
			{
				ocean_assert(false && "This should never happen!");
				invalidObjectPointIds.push_back(objectPointId);
			}
			else
			{
				const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, position);

				const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

				if (sqrDistance <= Numeric::sqr(maximalProjectionError))
				{
					validObjectPointIds.push_back(objectPointId);
				}
				else
				{
					invalidObjectPointIds.push_back(objectPointId);
				}
			}
		}
	}
}

size_t LocalizedObjectPoint::determineBundleAdjustmentQuality(const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, size_t& bundleAdjustedObjectPoints)
{
	size_t totalObjectPoints = 0;
	bundleAdjustedObjectPoints = 0;

	for (const LocalizedObjectPointMap::value_type& pointPair : localizedObjectPointMap)
	{
		const LocalizedObjectPoint& localizedObjectPoint = pointPair.second;

		if (localizedObjectPoint.hasObservation(frameIndex))
		{
			++totalObjectPoints;

			if (localizedObjectPoint.isBundleAdjusted())
			{
				++bundleAdjustedObjectPoints;
			}
		}
	}

	return totalObjectPoints;
}

Scalar LocalizedObjectPoint::determineMedianLocalizedObjectDistances(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const bool onlyTrackedObjectPoints)
{
	SharedCameraPose cameraPose;
	if (!cameraPoses.hasPose(frameIndex, cameraPose))
	{
		return Scalar(-1);
	}

	ocean_assert(cameraPose != nullptr);

	const HomogenousMatrix4& world_T_camera = cameraPose->world_T_camera();
	const Vector3 cameraPosition = world_T_camera.translation();

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();

	Scalars sqrDistances; // TODO make re-usable
	sqrDistances.reserve(localizedObjectPointMap.size());

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		const Vector3& position = localizedObjectPoint.position();
		ocean_assert(position != Vector3::minValue());

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
		{
			if (onlyTrackedObjectPoints)
			{
				if (!localizedObjectPoint.hasObservation(frameIndex))
				{
					continue;
				}
			}

			sqrDistances.push_back(cameraPosition.sqrDistance(position));
		}
	}

	if (sqrDistances.empty())
	{
		return Scalar(-1);
	}

	return Numeric::sqrt(Median::median(sqrDistances.data(), sqrDistances.size()));
}

size_t LocalizedObjectPoint::determineNumberTrackedObjectPoints(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap)
{
	SharedCameraPose cameraPose;
	if (!cameraPoses.hasPose(frameIndex, cameraPose))
	{
		return 0;
	}

	ocean_assert(cameraPose != nullptr);

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();

	size_t numberTrackedObjectPoints = 0;

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		const Vector3& position = localizedObjectPoint.position();
		ocean_assert(position != Vector3::minValue());

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
		{
			if (localizedObjectPoint.hasObservation(frameIndex))
			{
				++numberTrackedObjectPoints;
			}
		}
	}

	return numberTrackedObjectPoints;
}

size_t LocalizedObjectPoint::determineNumberTrackedObjectPoints(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const UnorderedIndexSet32& objectPointIds)
{
	SharedCameraPose cameraPose;
	if (!cameraPoses.hasPose(frameIndex, cameraPose))
	{
		return 0;
	}

	ocean_assert(cameraPose != nullptr);

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose->flippedCamera_T_world();

	size_t numberTrackedObjectPoints = 0;

	for (const Index32 objectPointId : objectPointIds)
	{
		const LocalizedObjectPointMap::const_iterator iObjectPoint = localizedObjectPointMap.find(objectPointId);

		ocean_assert(iObjectPoint != localizedObjectPointMap.cend());
		if (iObjectPoint != localizedObjectPointMap.cend())
		{
			const LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

			const Vector3& position = localizedObjectPoint.position();
			ocean_assert(position != Vector3::minValue());

			if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
			{
				if (localizedObjectPoint.hasObservation(frameIndex))
				{
					++numberTrackedObjectPoints;
				}
			}
		}
	}

	return numberTrackedObjectPoints;
}

LocalizedObjectPoint::LocalizationPrecision LocalizedObjectPoint::determineLocalizedObjectPointUncertaintyIF(const AnyCamera& camera, const HomogenousMatrices4& flippedCameras_T_world, const Vector3& objectPoint)
{
	ocean_assert(camera.isValid());

	ocean_assert(flippedCameras_T_world.size() >= 2);
	if (flippedCameras_T_world.size() < 2)
	{
		return LP_INVALID;
	}

	constexpr size_t lowNumberPosesThreshold = 5;

	if (flippedCameras_T_world.size() <= lowNumberPosesThreshold)
	{
		return LP_LOW;
	}

	SquareMatrix3 covarianceMatrix(false);

	Scalar jacobian[6]; // 2x3 point Jacobian

	for (const HomogenousMatrix4& flippedCamera_T_world : flippedCameras_T_world)
	{
		const Vector3 cameraObjectPoint = flippedCamera_T_world * objectPoint;

		ocean_assert(cameraObjectPoint.z() > Numeric::eps()); // the object point should always be in front of the camera
		if (cameraObjectPoint.z() <= Numeric::eps())
		{
			continue;
		}

		camera.pointJacobian2x3IF(cameraObjectPoint, jacobian, jacobian + 3);

		// Jacobian J is 2x3
		// covarianceMatrix = sum (J^T * J)

		// 0 3
		// 1 4    *  0 1 2
		// 2 5       3 4 5

		covarianceMatrix(0, 0) += jacobian[0] * jacobian[0] + jacobian[3] * jacobian[3];
		covarianceMatrix(0, 1) += jacobian[0] * jacobian[1] + jacobian[3] * jacobian[4];
		covarianceMatrix(0, 2) += jacobian[0] * jacobian[2] + jacobian[3] * jacobian[5];

		//covarianceMatrix(1, 0) += jacobian[1] * jacobian[0] + jacobian[4] * jacobian[3]; // the covariance matrix is symmetric
		covarianceMatrix(1, 1) += jacobian[1] * jacobian[1] + jacobian[4] * jacobian[4];
		covarianceMatrix(1, 2) += jacobian[1] * jacobian[2] + jacobian[4] * jacobian[5];

		//covarianceMatrix(2, 0) += jacobian[2] * jacobian[0] + jacobian[5] * jacobian[3];
		//covarianceMatrix(2, 1) += jacobian[2] * jacobian[1] + jacobian[5] * jacobian[4];
		covarianceMatrix(2, 2) += jacobian[2] * jacobian[2] + jacobian[5] * jacobian[5];
	}

	ocean_assert(!covarianceMatrix.isNull());

	const Scalar maxAbsValue = std::max(Numeric::abs(covarianceMatrix(0, 0)), std::max(Numeric::abs(covarianceMatrix(0, 1)), std::max(Numeric::abs(covarianceMatrix(0, 2)), std::max(Numeric::abs(covarianceMatrix(1, 1)), std::max(Numeric::abs(covarianceMatrix(1, 2)), Numeric::abs(covarianceMatrix(2, 2)))))));

	if (Numeric::isNotEqualEps(maxAbsValue))
	{
		const Scalar normalization = Scalar(1) / maxAbsValue;

		covarianceMatrix(0, 0) *= normalization;
		covarianceMatrix(0, 1) *= normalization;
		covarianceMatrix(0, 2) *= normalization;
		covarianceMatrix(1, 1) *= normalization;
		covarianceMatrix(1, 2) *= normalization;
		covarianceMatrix(2, 2) *= normalization;
	}

	covarianceMatrix(1, 0) = covarianceMatrix(0, 1);
	covarianceMatrix(2, 0) = covarianceMatrix(0, 2);
	covarianceMatrix(2, 1) = covarianceMatrix(1, 2);

	if (!covarianceMatrix.invert())
	{
		return LP_UNKNOWN;
	}

	return determineLocalizedObjectPointUncertainty(covarianceMatrix);
}

LocalizedObjectPoint::LocalizationPrecision LocalizedObjectPoint::determineLocalizedObjectPointUncertainty(const SquareMatrix3& covarianceMatrix)
{
	ocean_assert(!covarianceMatrix.isSingular());

	Scalar eigenValues[3];
	if (!covarianceMatrix.eigenValues(eigenValues))
	{
		return LP_UNKNOWN;
	}

	ocean_assert(eigenValues[0] >= eigenValues[1] && eigenValues[1] >= eigenValues[2]);

	if (eigenValues[2] <= Numeric::eps())
	{
		return LP_UNKNOWN;
	}

	// axis uncertainty is equivalent to the root of the eigen values

	const Scalar& sqrAxisUncertainty0 = eigenValues[0];
	const Scalar& sqrAxisUncertainty1 = eigenValues[1];

	if (Numeric::isEqualEps(sqrAxisUncertainty1))
	{
		return LP_UNKNOWN;
	}

	const Scalar sqrRatio = sqrAxisUncertainty0 / sqrAxisUncertainty1;

	constexpr Scalar highPrecisionThreshold = Scalar(7); // ~20-30 deg
	constexpr Scalar mediumPrecisionThreshold = highPrecisionThreshold * 2; // ~10-15 deg

	if (sqrRatio <= Scalar(Numeric::sqr(highPrecisionThreshold)))
	{
		return LP_HIGH;
	}

	if (sqrRatio <= Scalar(Numeric::sqr(mediumPrecisionThreshold)))
	{
		return LP_MEDIUM;
	}

	return LP_LOW;
}

SharedCameraPose LocalizedObjectPoint::determineCameraPose(const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, RandomGenerator& randomGenerator, const Geometry::Estimator::EstimatorType estimatorType, CorrespondenceData& correspondenceData, const Geometry::GravityConstraints* gravityConstraints)
{
	ocean_assert(camera.isValid());
	ocean_assert(localizedObjectPointMap.size() >= 4);

	ocean_assert(frameIndex <= cameraPoses.frameIndex());

	correspondenceData.reset();

	Vectors3& objectPoints = correspondenceData.objectPoints_;
	Vectors2& imagePoints = correspondenceData.imagePoints_;
	Indices32& objectPointIds = correspondenceData.objectPointIds_;
	Indices32& usedIndices = correspondenceData.usedIndices_;
	LocalizedObjectPoint::LocalizationPrecisions& localizationPrecisions = correspondenceData.localizationPrecisions_;

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const Index32& objectPointId = objectPointPair.first;
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		Vector2 imagePoint;
		if (localizedObjectPoint.hasObservation(frameIndex, &imagePoint))
		{
			ocean_assert(localizedObjectPoint.position_ != Vector3::minValue());

			objectPoints.push_back(localizedObjectPoint.position_);
			imagePoints.push_back(imagePoint);

			objectPointIds.push_back(objectPointId);
			localizationPrecisions.push_back(localizedObjectPoint.localizationPrecision());
		}
	}

	if (objectPoints.size() < 20)
	{
		return nullptr;
	}

#ifdef OCEAN_DEBUG
	const size_t debugCorrespondenceCandidates = objectPoints.size();
#endif

	PoseQualityCalculator poseQualityCalculator;

	constexpr Scalar maximalProjectionError = Scalar(3.5);

	HomogenousMatrix4 world_T_camera(false);

	SharedCameraPose previousCameraPose;
	if (frameIndex >= 1u && cameraPoses.hasPose(frameIndex - 1u, previousCameraPose))
	{
		ocean_assert(previousCameraPose != nullptr);

		constexpr unsigned int iterations = 20u;
		Scalar debugInitialError = Numeric::maxValue();
		Scalars debugIntermediateError;
		Scalar debugFinalError = Numeric::maxValue();

		if (!Geometry::NonLinearOptimizationPose::optimizePose(camera, previousCameraPose->world_T_camera(), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), world_T_camera, iterations, estimatorType, Scalar(0.001), Scalar(10), &debugInitialError, &debugFinalError, &debugIntermediateError, gravityConstraints))
		{
			return nullptr;
		}

		const HomogenousMatrix4 flippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_camera);

		for (size_t nCorrespondence = 0; nCorrespondence < objectPoints.size(); ++nCorrespondence)
		{
			const Vector3& objectPoint = objectPoints[nCorrespondence];
			const Vector2& imagePoint = imagePoints[nCorrespondence];

			const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, objectPoint);

			if (imagePoint.sqrDistance(projectedObjectPoint) <= Numeric::sqr(maximalProjectionError))
			{
				usedIndices.push_back(Index32(nCorrespondence));
			}
		}
	}
	else
	{
		if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, world_T_camera, 5u, true /*refine*/, 50u /*iterations*/, Numeric::sqr(maximalProjectionError), &usedIndices, nullptr, gravityConstraints))
		{
			return nullptr;
		}
	}

#ifdef OCEAN_DEBUG
	const size_t debugValidCorrespondences = usedIndices.size();
	ocean_assert(correspondenceData.badObjectPointIds_.empty());
#endif

	correspondenceData.applySubset();

#ifdef OCEAN_DEBUG
	ocean_assert(correspondenceData.badObjectPointIds_.size() == debugCorrespondenceCandidates - debugValidCorrespondences);
	ocean_assert(correspondenceData.objectPoints_.size() == debugValidCorrespondences);
	ocean_assert(correspondenceData.localizationPrecisions_.size() == debugValidCorrespondences);
	ocean_assert(correspondenceData.imagePointSqrDistances_.empty() || correspondenceData.imagePointSqrDistances_.size() == debugValidCorrespondences);
#endif

	for (const LocalizedObjectPoint::LocalizationPrecision localizationPrecision : correspondenceData.localizationPrecisions_)
	{
		poseQualityCalculator.addObjectPoint(localizationPrecision);
	}

	const CameraPose::PoseQuality poseQuality = poseQualityCalculator.poseQuality();

	if (poseQuality == CameraPose::PQ_INVALID)
	{
		return nullptr;
	}

	return std::make_shared<CameraPose>(world_T_camera, poseQuality);
}

bool LocalizedObjectPoint::determineCameraPose(const AnyCamera& camera, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, HomogenousMatrix4& world_T_camera, RandomGenerator& randomGenerator, const size_t minimalCorrespondences, const Scalar maximalProjectionError, Indices32& usedObjectPointIds, Scalar& sqrError, const Geometry::GravityConstraints* gravityConstraints)
{
	ocean_assert(camera.isValid());

	Vectors3 objectPoints;
	objectPoints.reserve(128);

	Vectors2 imagePoints;
	imagePoints.reserve(128);

	usedObjectPointIds.clear();
	usedObjectPointIds.reserve(128);

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const Index32 objectPointId = objectPointPair.first;
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		Vector2 imagePoint;
		if (localizedObjectPoint.hasObservation(frameIndex, &imagePoint))
		{
			objectPoints.push_back(localizedObjectPoint.position());
			imagePoints.push_back(imagePoint);

			usedObjectPointIds.push_back(objectPointId);
		}
	}

	if (objectPoints.size() < minimalCorrespondences)
	{
		return false;
	}

	Indices32 usedIndices;

	if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, world_T_camera, (unsigned int)(minimalCorrespondences), true /*refine*/, 50u /*iterations*/, Numeric::sqr(maximalProjectionError), &usedIndices, &sqrError, gravityConstraints))
	{
		return false;
	}

	ocean_assert(usedIndices.size() >= minimalCorrespondences);

	Subset::applySubset(usedObjectPointIds, usedIndices);

	return true;
}

std::string LocalizedObjectPoint::translateLocalizationPrecision(const LocalizationPrecision localizationPrecision)
{
	switch (localizationPrecision)
	{
		case LP_INVALID:
			return std::string("Invalid");

		case LP_UNKNOWN:
			return std::string("Unknown");

		case LP_LOW:
			return std::string("Low");

		case LP_MEDIUM:
			return std::string("Medium");

		case LP_HIGH:
			return std::string("High");
	}

	ocean_assert(false && "Invalid precision!");
	return std::string("Invalid");
}

bool LocalizedObjectPoint::serialize(const LocalizedObjectPointMap& localizedObjectPointMap, IO::OutputBitstream& outputBitstream)
{
	constexpr uint32_t version = 1u;

	if (!outputBitstream.write<uint32_t>(version))
	{
		return false;
	}

	if (!outputBitstream.write<uint32_t>(uint32_t(localizedObjectPointMap.size())))
	{
		return false;
	}

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const Index32& objectPointId = objectPointPair.first;

		if (!outputBitstream.write<uint32_t>(objectPointId))
		{
			return false;
		}

		if (!serialize(objectPointPair.second, outputBitstream))
		{
			return false;
		}
	}

	return true;
}

bool LocalizedObjectPoint::serialize(const LocalizedObjectPoint& localizedObjectPoint, IO::OutputBitstream & outputBitstream)
{
	const VectorF3 position(localizedObjectPoint.position());

	if (!outputBitstream.write<float>(position.x()) || !outputBitstream.write<float>(position.y()) || !outputBitstream.write<float>(position.z()))
	{
		return false;
	}

	if (!outputBitstream.write<Index32>(localizedObjectPoint.lastObservationFrameIndex()))
	{
		return false;
	}

	if (!outputBitstream.write<uint8_t>(localizedObjectPoint.localizationPrecision()))
	{
		return false;
	}

	ocean_assert(NumericT<uint32_t>::isInsideValueRange(localizedObjectPoint.observationMap_.size()));
	if (!outputBitstream.write<uint32_t>(uint32_t(localizedObjectPoint.observationMap_.size())))
	{
		return false;
	}

	for (const ObservationMap::value_type& observationPair : localizedObjectPoint.observationMap_)
	{
		const Index32 frameIndex = observationPair.first;

		const VectorF2 imagePoint(observationPair.second);

		if (!outputBitstream.write<Index32>(frameIndex))
		{
			return false;
		}

		if (!outputBitstream.write<float>(imagePoint.x()) || !outputBitstream.write<float>(imagePoint.y()))
		{
			return false;
		}
	}

	return true;
}

}

}

}
