/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/PoseCorrespondences.h"
#include "ocean/tracking/slam/PoseQualityCalculator.h"

#include "ocean/base/Subset.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Camera.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

PoseCorrespondences::PoseCorrespondences()
{
	reserve(256);
}

void PoseCorrespondences::reset(const TrackingCorrespondences& trackingCorrespondences)
{
	clear();

	mapVersion_ = trackingCorrespondences.mapVersion();

	const Vectors3& trackedObjectPoints = trackingCorrespondences.objectPoints();
	const LocalizedObjectPoint::LocalizationPrecisions trackedObjectPointPrecisions = trackingCorrespondences.objectPointPrecisions();

	const Vectors2& trackedPreviousImagePoints = trackingCorrespondences.previousImagePoints();
	const Vectors2& trackedCurrentImagePoints = trackingCorrespondences.currentImagePoints();
	ocean_assert(trackedPreviousImagePoints.size() == trackedCurrentImagePoints.size());

	const Indices32& trackedObjectPointIds = trackingCorrespondences.pointIds();

	const TrackingCorrespondences::ValidCorrespondences& trackedValidCorrespondences = trackingCorrespondences.validCorrespondences();

	ocean_assert(trackedObjectPoints.size() <= trackedValidCorrespondences.size());

	for (size_t nTrackedObjectPoint = 0; nTrackedObjectPoint < trackedObjectPoints.size(); ++nTrackedObjectPoint)
	{
		if (trackedValidCorrespondences[nTrackedObjectPoint])
		{
			const Vector3& trackedObjectPoint = trackedObjectPoints[nTrackedObjectPoint];

			const Vector2& trackedPreviousImagePoint = trackedPreviousImagePoints[nTrackedObjectPoint];
			const Vector2& trackedCurrentImagePoint = trackedCurrentImagePoints[nTrackedObjectPoint];

			const Index32& trackedObjectPointId = trackedObjectPointIds[nTrackedObjectPoint];

			const LocalizedObjectPoint::LocalizationPrecision& localizationPrecision = trackedObjectPointPrecisions[nTrackedObjectPoint];

			addCorrespondence(trackedObjectPoint, trackedCurrentImagePoint, trackedObjectPointId, localizationPrecision, trackedPreviousImagePoint.sqrDistance(trackedCurrentImagePoint));
		}
	}
}

SharedCameraPose PoseCorrespondences::determinePose(const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const unsigned int minimalNumberCorrespondences, RandomGenerator& randomGenerator, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType, const Geometry::GravityConstraints* gravityConstraints, Scalar* robustError)
{
	ocean_assert(camera.isValid());
	ocean_assert(minimalNumberCorrespondences >= 5u);
	ocean_assert(maximalProjectionError >= Scalar(0));

	ocean_assert(objectPoints_.size() >= minimalNumberCorrespondences);

	ocean_assert(inlierIndices_.empty());

	preciseObjectPointIds_.clear();
	impreciseObjectPointIds_.clear();

#ifdef OCEAN_DEBUG
	const size_t debugCorrespondenceCandidates = objectPoints_.size();
#endif

	HomogenousMatrix4 world_T_camera(false);
	HomogenousMatrix4 flippedCamera_T_world(false);

	if (!world_T_previousCamera.isValid())
	{
		// we do not have a previous camera pose, so we need to determine the camera pose based on a RANSAC approach

		constexpr unsigned int iterations = 50u;

		if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints_), ConstArrayAccessor<Vector2>(imagePoints_), randomGenerator, world_T_camera, 5u, true /*refine*/, iterations, Numeric::sqr(maximalProjectionError), nullptr, nullptr, gravityConstraints))
		{
			return nullptr;
		}
	}
	else
	{
		world_T_camera = world_T_previousCamera;
	}

	// now we have a valid rough camera pose so that we can apply a non-linear optimization to improve the pose

	ocean_assert(world_T_camera.isValid());

	// we have a previous camera pose, so we can simply try to use the updated 2D/3D correspondences to determine the new camera pose based on a non-linear optimization

	constexpr unsigned int iterations = 20u;

	Scalar optimizedRobustError = Numeric::maxValue();

	ocean_assert(estimatorType != Geometry::Estimator::ET_SQUARE);
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(camera, Camera::standard2InvertedFlipped(world_T_camera), ConstArrayAccessor<Vector3>(objectPoints_), ConstArrayAccessor<Vector2>(imagePoints_), flippedCamera_T_world, iterations, estimatorType, Scalar(0.001), Scalar(10), nullptr, &optimizedRobustError, nullptr, gravityConstraints))
	{
		return nullptr;
	}

	// let's determine which of the 2D/3D correspondences were actually valid

	Scalar sqrError = 0;

	for (size_t nCorrespondence = 0; nCorrespondence < objectPoints_.size(); ++nCorrespondence)
	{
		const Vector3& objectPoint = objectPoints_[nCorrespondence];
		const Vector2& imagePoint = imagePoints_[nCorrespondence];

		const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, objectPoint);

		const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

		if (sqrDistance <= Numeric::sqr(maximalProjectionError))
		{
			inlierIndices_.push_back(Index32(nCorrespondence));

			sqrError += sqrDistance;
		}
	}

	world_T_camera = Camera::invertedFlipped2Standard(flippedCamera_T_world);

	if (robustError != nullptr)
	{
		*robustError = Numeric::ratio(sqrError, Scalar(inlierIndices_.size()), Numeric::maxValue());
	}

	if (inlierIndices_.size() < minimalNumberCorrespondences)
	{
		return nullptr;
	}

	for (const Index32 validIndex : inlierIndices_)
	{
		ocean_assert(validIndex < objectPointIds_.size());

		const Index32 objectPointId = objectPointIds_[validIndex];

		preciseObjectPointIds_.insert(objectPointId);
	}

	ocean_assert(preciseObjectPointIds_.size() == inlierIndices_.size());

	for (const Index32 objectPointId : objectPointIds_)
	{
		if (!preciseObjectPointIds_.contains(objectPointId))
		{
			impreciseObjectPointIds_.emplace(objectPointId);
		}
	}

#ifdef OCEAN_DEBUG
	const size_t debugValidCorrespondences = inlierIndices_.size();
	ocean_assert(outlierObjectPointIds_.empty());
#endif

	applyInlierSubset();

#ifdef OCEAN_DEBUG
	ocean_assert(outlierObjectPointIds_.size() == debugCorrespondenceCandidates - debugValidCorrespondences);
	ocean_assert(objectPoints_.size() == debugValidCorrespondences);
	ocean_assert(precisions_.size() == debugValidCorrespondences);
#endif

	PoseQualityCalculator poseQualityCalculator;

	for (const LocalizedObjectPoint::LocalizationPrecision localizationPrecision : precisions_)
	{
		poseQualityCalculator.addObjectPoint(localizationPrecision);
	}

	const CameraPose::PoseQuality poseQuality = poseQualityCalculator.poseQuality();

	if (poseQuality == CameraPose::PQ_INVALID)
	{
		Log::debug() << "Lost tracking, with " << objectPoints_.size() << " correspondences";
		outlierObjectPointIds_.clear();

		return nullptr;
	}

	// let's determine the amount of optical flow for an indication of camera translation

	const CameraPose::EstimatedMotion estimatedMotion = CameraPose::motionFromOpticalFlow(imagePointSqrDistances_.data(), imagePointSqrDistances_.size(), camera.width(), camera.height());

	return std::make_shared<CameraPose>(world_T_camera, poseQuality, estimatedMotion);
}

void PoseCorrespondences::clear()
{
	objectPoints_.clear();
	imagePoints_.clear();
	objectPointIds_.clear();
	precisions_.clear();
	imagePointSqrDistances_.clear();

	inlierIndices_.clear();
	outlierObjectPointIds_.clear();
	preciseObjectPointIds_.clear();
	impreciseObjectPointIds_.clear();
}

void PoseCorrespondences::applyInlierSubset()
{
	ocean_assert(!inlierIndices_.empty());

#ifdef OCEAN_DEBUG
	// the inlier indices are expected to be sorted
	for (size_t n = 1; n < inlierIndices_.size(); ++n)
	{
		ocean_assert(inlierIndices_[n - 1] < inlierIndices_[n]);
	}
#endif

	ocean_assert(inlierIndices_.size() <= objectPointIds_.size());

	if (inlierIndices_.size() != objectPointIds_.size())
	{
		ocean_assert(outlierObjectPointIds_.empty());
		outlierObjectPointIds_.reserve(objectPointIds_.size() - inlierIndices_.size());

		Indices32::const_iterator iInlier = inlierIndices_.cbegin();

		for (size_t nId = 0; nId < objectPointIds_.size(); ++nId)
		{
			if (iInlier == inlierIndices_.end())
			{
				while (nId < objectPointIds_.size())
				{
					outlierObjectPointIds_.emplace_back(objectPointIds_[nId]);
					++nId;
				}

				break;
			}

			ocean_assert(Index32(nId) <= *iInlier);

			if (Index32(nId) < *iInlier)
			{
				outlierObjectPointIds_.emplace_back(objectPointIds_[nId]);
			}
			else
			{
				++iInlier;
			}
		}

		ocean_assert(UnorderedIndexSet32(inlierIndices_.cbegin(), inlierIndices_.cend()).size() + UnorderedIndexSet32(outlierObjectPointIds_.cbegin(), outlierObjectPointIds_.cend()).size() == objectPointIds_.size());

		Subset::applySubset(objectPoints_, inlierIndices_);
		Subset::applySubset(imagePoints_, inlierIndices_);
		Subset::applySubset(objectPointIds_, inlierIndices_);
		Subset::applySubset(precisions_, inlierIndices_);

		if (!imagePointSqrDistances_.empty())
		{
			Subset::applySubset(imagePointSqrDistances_, inlierIndices_);
		}
	}

	inlierIndices_.clear();
}

void PoseCorrespondences::reserve(const size_t capacity)
{
	objectPoints_.reserve(capacity);
	imagePoints_.reserve(capacity);
	objectPointIds_.reserve(capacity);
	precisions_.reserve(capacity);
	imagePointSqrDistances_.reserve(capacity);
}

}

}

}
