/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/TrackingCorrespondences.h"
#include "ocean/tracking/slam/SLAMDebugElements.h"
#include "ocean/tracking/slam/Tracker.h"

#include "ocean/base/StaticVector.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"

#include "ocean/math/Camera.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

TrackingCorrespondences::TrackingCorrespondences()
{
	currentImagePoints_.reserve(256);
	previousImagePoints_.reserve(256);

	pointIds_.reserve(256);
	validCorrespondences_.reserve(256);

	objectPoints_.reserve(128);
	objectPointPrecisions_.reserve(128);
}

void TrackingCorrespondences::update(const Index32 previousFrameIndex, const Index32 mapVersion, const LocalizedObjectPointMap& localizedObjectPointMap, PointTrackMap& pointTrackMap, const LocalizedObjectPoint::LocalizationPrecision minimalFrontPrecision)
{
	previousImagePoints_.clear();
	currentImagePoints_.clear();

	pointIds_.clear();
	pointIdSet_.clear();
	validCorrespondences_.clear();

	objectPoints_.clear();
	objectPointPrecisions_.clear();

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap) // TODO iterate only over visible object points
	{
		const Index32& objectPointId = objectPointPair.first;
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

		if (localizedObjectPoint.lastObservationFrameIndex() == previousFrameIndex)
		{
			const Observation lastObservation = localizedObjectPoint.lastObservation();

			previousImagePoints_.push_back(lastObservation.imagePoint());
			pointIds_.push_back(objectPointId);

			objectPoints_.push_back(localizedObjectPoint.position());
			objectPointPrecisions_.push_back(localizedObjectPoint.localizationPrecision());

			ocean_assert(!pointIdSet_.contains(objectPointId));
			pointIdSet_.insert(objectPointId);
		}
	}

	// let's sort the object points based on their precision in-place, medium and high precision points go to the front

	size_t nextHighPrecisionObjectPoint = 0;

	for (size_t nObjectPoint = 0; nObjectPoint < objectPoints_.size(); ++nObjectPoint)
	{
		if (objectPointPrecisions_[nObjectPoint] >= minimalFrontPrecision)
		{
			if (nObjectPoint != nextHighPrecisionObjectPoint)
			{
				std::swap(previousImagePoints_[nObjectPoint], previousImagePoints_[nextHighPrecisionObjectPoint]);
				std::swap(pointIds_[nObjectPoint], pointIds_[nextHighPrecisionObjectPoint]);

				std::swap(objectPoints_[nObjectPoint], objectPoints_[nextHighPrecisionObjectPoint]);
				std::swap(objectPointPrecisions_[nObjectPoint], objectPointPrecisions_[nextHighPrecisionObjectPoint]);
			}

			++nextHighPrecisionObjectPoint;
		}
	}

#ifdef OCEAN_DEBUG
	{
		bool reachedEndOfPrecisePoints = false;

		UnorderedIndexSet32 pointIdSet;

		for (size_t n = 0; n < objectPointPrecisions_.size(); ++n)
		{
			if (objectPointPrecisions_[n] >= minimalFrontPrecision)
			{
				ocean_assert(!reachedEndOfPrecisePoints);
			}
			else
			{
				reachedEndOfPrecisePoints = true;
			}

			ocean_assert(!pointIdSet.contains(pointIds_[n]));
			pointIdSet.insert(pointIds_[n]);
		}

		ocean_assert(pointIdSet_.size() == pointIdSet.size());
	}
#endif // OCEAN_DEBUG

	for (const PointTrackMap::value_type& pointPair : pointTrackMap)
	{
		const Index32& objectPointId = pointPair.first;

		if (!pointIdSet_.contains(objectPointId)) // TODO should not be necessary once point tracks and unlocalized are merged
		{
			const PointTrack& pointTrack = pointPair.second;
			ocean_assert(pointTrack.isValid());

			ocean_assert(pointTrack.lastFrameIndex() == previousFrameIndex);
			if (pointTrack.lastFrameIndex() == previousFrameIndex)
			{
				previousImagePoints_.push_back(pointTrack.lastImagePoint());
				pointIds_.push_back(objectPointId);
			}
		}
	}

	ocean_assert(UnorderedIndexSet32(pointIds_.cbegin(), pointIds_.cend()).size() == pointIds_.size());

	ocean_assert(objectPoints_.size() == objectPointPrecisions_.size());

	previousFrameIndex_ = previousFrameIndex;
	mapVersion_ = mapVersion;
}

bool TrackingCorrespondences::optimizePreviousCameraPose(const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const size_t minimalCorrespondences, HomogenousMatrix4& world_T_optimizedPreviousCamera, const Geometry::Estimator::EstimatorType estimatorType, const Geometry::GravityConstraints* gravityConstraints) const
{
	ocean_assert(camera.isValid());
	ocean_assert(world_T_previousCamera.isValid());
	ocean_assert(minimalCorrespondences >= 4);

	if (objectPoints_.size() < minimalCorrespondences)
	{
		return false;
	}

	ocean_assert(objectPoints_.size() <= previousImagePoints_.size());

	const Vector3* objectPoints = objectPoints_.data();
	const Vector2* previousImagePoints = previousImagePoints_.data();

	const size_t correspondences = objectPoints_.size();

	Scalar debugInitialError = Numeric::maxValue();
	Scalar debugFinalError = Numeric::maxValue();

	if (!Geometry::NonLinearOptimizationPose::optimizePose(camera, world_T_previousCamera, ConstArrayAccessor<Vector3>(objectPoints, correspondences), ConstArrayAccessor<Vector2>(previousImagePoints, correspondences), world_T_optimizedPreviousCamera, 20u, estimatorType, Scalar(0.001), Scalar(5), &debugInitialError, &debugFinalError, nullptr, gravityConstraints))
	{
		return false;
	}

	Log::info() << "POST pose improvement: " << debugInitialError << " -> " << debugFinalError;

	return true;
}

size_t TrackingCorrespondences::countValidCorrespondences() const
{
	size_t count = 0;

	for (const uint8_t validCorrespondence : validCorrespondences_)
	{
		if (validCorrespondence != uint8_t(0))
		{
			++count;
		}
	}

	return count;
}

void TrackingCorrespondences::trackImagePoints(const Index32 currentFrameIndex, const AnyCamera& camera, const HomogenousMatrix4& world_T_previousCamera, const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, const Tracker::TrackingParameters& trackingParameters, const Quaternion& previousCamera_Q_currentCamera, const LocalizedObjectPoint::LocalizationPrecision minimalFrontPrecision)
{
	ocean_assert(currentFrameIndex >= 1u);

	ocean_assert(camera.isValid());

	ocean_assert(yPreviousFramePyramid.isValid() && yCurrentFramePyramid.isValid());
	ocean_assert(yPreviousFramePyramid.frameType() == yCurrentFramePyramid.frameType());

	if (currentFrameIndex == 0u || !yPreviousFramePyramid.isValid())
	{
		return;
	}

	if (isEmpty())
	{
		return;
	}

	ocean_assert(trackingParameters.isValid());
	if (!trackingParameters.isValid())
	{
		return;
	}

	const Scalar border = Scalar(trackingParameters.patchSize_);

	const Box2 validArea(border, border, Scalar(camera.width()) - border, Scalar(camera.height()) - border);

	ocean_assert(currentImagePoints_.empty());
	ocean_assert(validCorrespondences_.empty());

	constexpr Scalar maximalSqrError = Scalar(0.9 * 0.9);
	constexpr unsigned int subPixelIterations = 4u;

	validCorrespondences_.resize(previousImagePoints_.size());

	StaticVector<CV::Advanced::AdvancedMotion::PointCorrespondences, 2> pointCorrespondences;

	constexpr Scalar strongMotionAngle = Numeric::deg2rad(0.5); // TODO move parameter to configuration

	if (previousCamera_Q_currentCamera.isValid())
	{
		currentImagePoints_.reserve(previousImagePoints_.size());

		if (world_T_previousCamera.isValid())
		{
			// we have a valid camera pose from the previous frame; thus, we can predict the updated camera pose and project precise 3D object points into the camera image
			// precise object points are at the front of all correspondences

			if (!objectPoints_.empty())
			{
				ocean_assert(objectPoints_.size() == objectPointPrecisions_.size());

				const HomogenousMatrix4 world_T_approximatedCurrentCamera = world_T_previousCamera * HomogenousMatrix4(previousCamera_Q_currentCamera);

				const HomogenousMatrix4 flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_approximatedCurrentCamera));

				for (size_t n = 0; n < objectPoints_.size(); ++n)
				{
					const LocalizedObjectPoint::LocalizationPrecision objectPointPrecision = objectPointPrecisions_[n];

					if (objectPointPrecision < minimalFrontPrecision)
					{
						// we have reached the group of localized object points which are below medium precise
						// they are not yet precise enough to be projected into the camera

#ifdef OCEAN_DEBUG
						for (size_t nDebug = n + 1; nDebug < objectPoints_.size(); ++nDebug)
						{
							ocean_assert(objectPointPrecisions_[nDebug] < minimalFrontPrecision);
						}
#endif // OCEAN_DEBUG

						break;
					}

					const Vector2& previousImagePoint = previousImagePoints_[n];

					const Vector3& objectPoint = objectPoints_[n];

					const Vector2 predictedImagePoint = camera.projectToImageIF(flippedCamera_T_world, objectPoint);

					if (validArea.isInside(previousImagePoint) && validArea.isInside(predictedImagePoint))
					{
						currentImagePoints_.push_back(predictedImagePoint);
					}
					else
					{
						currentImagePoints_.push_back(previousImagePoint);
					}
				}

				if (!currentImagePoints_.empty())
				{
					// we have at least one precise object point, so we can use the guided tracking approach

					const Tracker::TrackingParameterPair& parameterPair = trackingParameters.parameterPair(world_T_previousCamera, previousCamera_Q_currentCamera, strongMotionAngle);

					ocean_assert(currentImagePoints_.size() <= objectPoints_.size());
					pointCorrespondences.emplaceBack(previousImagePoints_.data(), currentImagePoints_.data(), validCorrespondences_.data(), currentImagePoints_.size(), parameterPair.layers_, parameterPair.coarsestLayerRadius_, maximalSqrError, subPixelIterations);
				}
			}

			if constexpr (Tracker::loggingEnabled_)
			{
				if (!currentImagePoints_.empty())
				{
					Log::info() << "Frame-to-frame tracking: Using " << currentImagePoints_.size() << " object point predictions";
				}
			}
		}

		if (currentImagePoints_.size() < previousImagePoints_.size())
		{
			const size_t imagePointStartIndex = currentImagePoints_.size();
			const size_t remainingImagePoints = previousImagePoints_.size() - currentImagePoints_.size();

			const Quaternion currentCamera_Q_previousCamera(previousCamera_Q_currentCamera.inverted());
			const Quaternion flippedCurrentCamera_Q_flippedPreviousCamera = Camera::flippedTransformationLeftAndRightSide(currentCamera_Q_previousCamera);

			for (size_t n = currentImagePoints_.size(); n < previousImagePoints_.size(); ++n)
			{
				const Vector2& previousImagePoint = previousImagePoints_[n];

				constexpr Scalar objectDistance = 1;

				const Vector3 previousObjectPointFlipped = camera.vectorIF(previousImagePoint, true /*makeUnitVector*/) * objectDistance;

				const Vector3 predictedObjectPointFlipped = flippedCurrentCamera_Q_flippedPreviousCamera * previousObjectPointFlipped;

				const Vector2 predictedImagePoint = camera.projectToImageIF(predictedObjectPointFlipped);

				if (validArea.isInside(previousImagePoint) && validArea.isInside(predictedImagePoint))
				{
					currentImagePoints_.push_back(predictedImagePoint);
				}
				else
				{
					currentImagePoints_.push_back(previousImagePoint);
				}
			}

			const Tracker::TrackingParameterPair& parameterPair = trackingParameters.parameterPair(HomogenousMatrix4(false), previousCamera_Q_currentCamera, strongMotionAngle);

			pointCorrespondences.emplaceBack(previousImagePoints_.data() + imagePointStartIndex, currentImagePoints_.data() + imagePointStartIndex, validCorrespondences_.data() + imagePointStartIndex, remainingImagePoints, parameterPair.layers_, parameterPair.coarsestLayerRadius_, maximalSqrError, subPixelIterations);
		}
	}
	else
	{
		currentImagePoints_.assign(previousImagePoints_.cbegin(), previousImagePoints_.cend());

		const Tracker::TrackingParameterPair& parameterPair = trackingParameters.parameterPair(HomogenousMatrix4(false), Quaternion(false), strongMotionAngle);

		pointCorrespondences.emplaceBack(previousImagePoints_.data(), currentImagePoints_.data(), validCorrespondences_.data(), previousImagePoints_.size(), parameterPair.layers_, parameterPair.coarsestLayerRadius_, maximalSqrError, subPixelIterations);
	}

#ifdef DEBUG_TRACKING_CORRESPONDENCES
	Vectors2 debugCopyCurrentImagePoints(currentImagePoints_);
#endif

	switch (trackingParameters.patchSize_)
	{
		case 7u:
		{
			if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<1u, 7u>(yPreviousFramePyramid, yCurrentFramePyramid, pointCorrespondences.data(), pointCorrespondences.size()))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			break;
		}

		case 31u:
		{
			if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<1u, 31u>(yPreviousFramePyramid, yCurrentFramePyramid, pointCorrespondences.data(), pointCorrespondences.size()))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			break;
		}

		default:
		{
			ocean_assert(trackingParameters.patchSize_ == 15u);

			if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<1u, 15u>(yPreviousFramePyramid, yCurrentFramePyramid, pointCorrespondences.data(), pointCorrespondences.size()))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			break;
		}
	}

	ocean_assert(previousImagePoints_.size() == currentImagePoints_.size());

#ifdef DEBUG_TRACKING_CORRESPONDENCES
	{
		static CV::Advanced::AdvancedMotion::TrackingStatistic trackingStatistic(yPreviousFramePyramid.finestWidth(), yPreviousFramePyramid.finestHeight());
		//trackingStatistic.addCorrespondences(previousImagePoints_.data(), debugCopyCurrentImagePoints.data(), validCorrespondences_.data(), previousImagePoints_.size());
		trackingStatistic.addCorrespondences(debugCopyCurrentImagePoints.data(), currentImagePoints_.data(), validCorrespondences_.data(), previousImagePoints_.size());

		if (trackingStatistic.measurements() % 20 == 0)
		{
			Log::info() << "TRACKING STATISTIC:";
			Log::info() << trackingStatistic.toString();
		}
	}
#endif

	if constexpr (SLAMDebugElements::allowDebugging_)
	{
		SLAMDebugElements::get().updateTrackedImagePoints(yCurrentFramePyramid.finestLayer(), previousImagePoints_.data(), currentImagePoints_.data(), validCorrespondences_.data(), previousImagePoints_.size());
	}
}

}

}

}
