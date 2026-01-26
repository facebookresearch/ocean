/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/TrackerMono.h"
#include "ocean/tracking/slam/PoseQualityCalculator.h"
#include "ocean/tracking/slam/SLAMDebugElements.h"

#include "ocean/base/Median.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/StereoscopicGeometry.h"

#include "ocean/tracking/PoseEstimationT.h"

#include <optional>

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

bool TrackerMono::FrameStatistics::isValid() const
{
	if (frameIndex_ == Index32(-1))
	{
		return false;
	}

	if (frameToFrameTrackingPossible_ < frameToFrameTrackingActual_)
	{
		return false;
	}

	if (frameToFrameTrackingActual_ < poseEstimationCorrespondences_)
	{
		return false;
	}

	ocean_assert(poseEstimationCorrespondences_ == 0 || (projectionError_ >= Scalar(0) && world_T_camera_.isValid()));

	return true;
}

void TrackerMono::ObjectPointOptimization::collectObjectPoints(const LocalizedObjectPointMap& localizedObjectPointMap, const UnorderedIndexSet32& previousBundleAdjustmentObjectPointIdSet)
{
	optimizationObjectMap_.clear();

	for (const LocalizedObjectPointMap::value_type& localizedObjectPointPair : localizedObjectPointMap)
	{
		const Index32& objectPointId = localizedObjectPointPair.first;

		if (previousBundleAdjustmentObjectPointIdSet.contains(objectPointId))
		{
			continue;
		}

		const LocalizedObjectPoint& localizedObjectPoint = localizedObjectPointPair.second;

		OptimizationObject* optimizationObject = nullptr;

		for (size_t nKeyFrameSubsetIndex = 0; nKeyFrameSubsetIndex < keyFrameIndices_.size(); ++nKeyFrameSubsetIndex)
		{
			ocean_assert(nKeyFrameSubsetIndex < keyFrameIndices_.size());
			const Index32& keyFrameIndex = keyFrameIndices_[nKeyFrameSubsetIndex];

			Vector2 imagePoint;
			if (localizedObjectPoint.hasObservation(keyFrameIndex, &imagePoint))
			{
				if (optimizationObject == nullptr)
				{
					ocean_assert(!optimizationObjectMap_.contains(objectPointId));
					optimizationObject = &optimizationObjectMap_[objectPointId];

					optimizationObject->objectPoint_ = localizedObjectPoint.position();
				}

				optimizationObject->imagePoints_.push_back(imagePoint);
				optimizationObject->keyFrameSubsetIndices_.push_back(nKeyFrameSubsetIndex);
			}
		}
	}
}

void TrackerMono::ObjectPointOptimization::optimizeObjectPointsIF(const AnyCamera& camera, const HomogenousMatrices4& optimizedFlippedCameras_T_world, const Geometry::Estimator::EstimatorType estimatorType, const Scalar maximalProjectionError, UnorderedIndexSet32& currentBundleAdjustmentObjectPointIdSet, Indices32& currentObjectPointIds, Vectors3& currentObjectPointPositions, Indices32& inaccurateObjectPointIds)
{
	HomogenousMatrices4 subsetFlippedCameras_T_world;
	Vectors2 subsetImagePoints;

	size_t numberOptimizedObjectPoints = 0;

	for (const OptimizationObjectMap::value_type& optimizationObjectPair : optimizationObjectMap_)
	{
		const Index32& objectPointId = optimizationObjectPair.first;
		const OptimizationObject& optimizationObject = optimizationObjectPair.second;

		ocean_assert(!currentBundleAdjustmentObjectPointIdSet.contains(objectPointId));

		if (optimizationObject.keyFrameSubsetIndices_.size() < 2)
		{
			continue;
		}

		const Vector3& objectPoint = optimizationObject.objectPoint_;

		subsetFlippedCameras_T_world.clear();
		subsetImagePoints.clear();

		ocean_assert(optimizationObject.imagePoints_.size() == optimizationObject.keyFrameSubsetIndices_.size());
		for (size_t nKeyFrameSubsetIndex = 0; nKeyFrameSubsetIndex < optimizationObject.keyFrameSubsetIndices_.size(); ++nKeyFrameSubsetIndex)
		{
			const size_t keyFrameSubsetIndex = optimizationObject.keyFrameSubsetIndices_[nKeyFrameSubsetIndex];
			ocean_assert(keyFrameSubsetIndex < optimizedFlippedCameras_T_world.size());

			ocean_assert(nKeyFrameSubsetIndex < optimizationObject.imagePoints_.size());

			subsetFlippedCameras_T_world.push_back(optimizedFlippedCameras_T_world[keyFrameSubsetIndex]);
			subsetImagePoints.push_back(optimizationObject.imagePoints_[nKeyFrameSubsetIndex]);
		}

		Vector3 optimizedObjectPoint;

		Scalar debugInitialError = Numeric::maxValue();
		Scalar finalRobustError = Numeric::maxValue();
		if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(camera, ConstArrayAccessor<HomogenousMatrix4>(subsetFlippedCameras_T_world), objectPoint, ConstArrayAccessor<Vector2>(subsetImagePoints), optimizedObjectPoint, 20u, estimatorType, Scalar(0.001), Scalar(5), true, &debugInitialError, &finalRobustError))
		{
			if (finalRobustError < Numeric::sqr(maximalProjectionError))
			{
#ifdef OCEAN_DEBUG
				for (const HomogenousMatrix4& subsetFlippedCamera_T_world : subsetFlippedCameras_T_world)
				{
					ocean_assert(Camera::isObjectPointInFrontIF(subsetFlippedCamera_T_world, optimizedObjectPoint));
				}
#endif // OCEAN_DEBUG

				currentBundleAdjustmentObjectPointIdSet.emplace(objectPointId);

				currentObjectPointIds.push_back(objectPointId);
				currentObjectPointPositions.push_back(optimizedObjectPoint);

				++numberOptimizedObjectPoints;

				continue;
			}
		}

		inaccurateObjectPointIds.push_back(objectPointId);
	}

	if constexpr (loggingEnabled_)
	{
		if (numberOptimizedObjectPoints != 0)
		{
			Log::info() << "    Background: Number optimized object points in addition to Bundle Adjustment: " << numberOptimizedObjectPoints;
		}
	}
	else
	{
		OCEAN_SUPPRESS_UNUSED_WARNING(numberOptimizedObjectPoints);
	}
}

std::string TrackerMono::PerformanceStatistics::toString() const
{
	if constexpr (!isEnabled_)
	{
		return "Performance measurements are disabled";
	}

	std::string result;

	result += "Main thread:";
	result += "\nHandle frame: " + handleFrame_.toString();
	result += "\nTrack image points: " + trackImagePoints_.toString();
	result += "\n   Update database: " + trackImagePointsDatabase_.toString();
	result += "\nDetermine camera pose: " + determineCameraPose_.toString();
	result += "\n\nBackground task (post handle frame):";
	result += "\nDetect new image points: " + detectNewImagePoints_.toString();
	result += "\n   Match unlocalized object points: " + matchCornersToLocalizedObjectPoints_.toString();
	result += "\n\nBackground thread:";
	result += "\nDetermine initial object points: " + determineInitialObjectPoints_.toString();
	result += "\nRecognize object points: " + relocalize_.toString();
	result += "\nOptimize bad object points: " + optimizeBadObjectPoints_.toString();
	result += "\nOptimize poses and object points: " + bundleAdjustment_.toString();

	return result;
}

void TrackerMono::DebugData::update(const Index32 frameIndex, const TrackingCorrespondences& trackingCorrespondences, const PoseCorrespondences& poseCorrespondences)
{
	const Indices32& pointIds = trackingCorrespondences.pointIds();
	const Vectors2& previousImagePoints = trackingCorrespondences.previousImagePoints();
	const Vectors2& currentImagePoints = trackingCorrespondences.currentImagePoints();
	const TrackingCorrespondences::ValidCorrespondences& validCorrespondences = trackingCorrespondences.validCorrespondences();

	ocean_assert(previousImagePoints.size() == currentImagePoints.size());
	ocean_assert(previousImagePoints.size() == pointIds.size());
	ocean_assert(previousImagePoints.size() == validCorrespondences.size());

	for (size_t n = 0; n < pointIds.size(); ++n)
	{
		const Index32 pointId = pointIds[n];

		if (validCorrespondences[n])
		{
			TracksMap::iterator iTrack = tracksMap_.find(pointId);

			if (iTrack != tracksMap_.cend())
			{
				TrackPair& trackPair = iTrack->second;

				Index32& trackFrameIndex = trackPair.first;
				Vectors2& trackImagePoints = trackPair.second;

				ocean_assert(trackFrameIndex + 1u == frameIndex);

				trackImagePoints.push_back(currentImagePoints[n]);

				trackFrameIndex = frameIndex;
			}
			else
			{
				tracksMap_.emplace(pointId, TrackPair(frameIndex, {previousImagePoints[n], currentImagePoints[n]}));
			}
		}
		else
		{
			tracksMap_.erase(pointId);
		}
	}

	for (TracksMap::iterator iTrack = tracksMap_.begin(); iTrack != tracksMap_.end(); /*noop*/)
	{
		Index32& trackFrameIndex = iTrack->second.first;

		if (trackFrameIndex != frameIndex)
		{
			// TODO we lost a track somehow - should be debugged

			iTrack = tracksMap_.erase(iTrack);
		}
		else
		{
			++iTrack;
		}
	}

	ocean_assert(poseCorrespondences.objectPointIds_.size() == poseCorrespondences.objectPoints_.size());
	ocean_assert(poseCorrespondences.objectPointIds_.size() == poseCorrespondences.precisions_.size());

	for (size_t n = 0; n < poseCorrespondences.objectPointIds_.size(); ++n)
	{
		const Index32 objectPointId = poseCorrespondences.objectPointIds_[n];

		const Vector3& objectPoint = poseCorrespondences.objectPoints_[n];
		const LocalizedObjectPoint::LocalizationPrecision& localizationPrecision = poseCorrespondences.precisions_[n];

		pointMap_[objectPointId] = Point(objectPoint, localizationPrecision);
	}

	inaccurateObjectPointIdSet_.clear();
	inaccurateObjectPointIdSet_.insert(poseCorrespondences.outlierObjectPointIds_.begin(), poseCorrespondences.outlierObjectPointIds_.end());
}

void TrackerMono::DebugData::update(DebugData&& debugData)
{
	tracksMap_ = std::move(debugData.tracksMap_);

	for (const PointMap::value_type& pair : debugData.pointMap_)
	{
		const Index32 objectPointId = pair.first;
		const Point& point = pair.second;

		pointMap_[objectPointId] = point;
	}

	posePreciseObjectPointIds_ = std::move(debugData.posePreciseObjectPointIds_);
	poseNotPreciseObjectPointIds_ = std::move(debugData.poseNotPreciseObjectPointIds_);

	inaccurateObjectPointIdSet_ = std::move(debugData.inaccurateObjectPointIdSet_);
}

void TrackerMono::DebugData::clear()
{
	tracksMap_.clear();
	pointMap_.clear();

	posePreciseObjectPointIds_.clear();
	poseNotPreciseObjectPointIds_.clear();
	inaccurateObjectPointIdSet_.clear();
}

TrackerMono::TrackerMono()
{
	ocean_assert(configuration_.isValid());

	harrisThreshold_ = configuration_.harrisThresholdMean();

	postHandleFrameTask_.setTask(std::bind(&TrackerMono::postHandleFrame, this));
}

TrackerMono::~TrackerMono()
{
	postHandleFrameTask_.release();

	stopThreadExplicitly();
}

bool TrackerMono::configure(const Configuration& configuration)
{
	if (!configuration.isValid())
	{
		return false;
	}

	if (!cameraPoses_.isEmpty())
	{
		return false;
	}

	const WriteLock writeLock(mutex_);

	configuration_ = configuration;

	harrisThreshold_ = configuration_.harrisThresholdMean();

	return true;
}

bool TrackerMono::handleFrame(const AnyCamera& camera, Frame&& yFrame, HomogenousMatrix4& world_T_camera, const Vector3& cameraGravity, const Quaternion& anyWorld_Q_camera, DebugData* debugData)
{
	const Timestamp unixTimestamp(true);

	handleFrameRateCalculator_.addOccurence(unixTimestamp);

	double frameRate = -1.0;
	if (handleFrameRateCalculator_.rateEveryNSeconds(unixTimestamp, frameRate, 10.0))
	{
		Log::info() << "TrackerMono:handleFrame() frame rate: " << String::toAString(frameRate, 1u) << "Hz";
	}

	ocean_assert(configuration_.isValid());
	if (!configuration_.isValid())
	{
		return false;
	}

	if (!camera_)
	{
		// we make a clone of the very first valid camera model, afterwards we assume that the model never changes

		camera_ = camera.clone();

		startThread();
	}

	ocean_assert(camera_->isEqual(camera));

	if (trackerState_ == TS_UNKNOWN)
	{
		ocean_assert(!cameraPoses_.isValid());
		trackerState_ = TS_INITIALIZING;
	}

	ocean_assert(camera.isValid());
	if (!camera.isValid())
	{
		return false;
	}

	ocean_assert(yFrame.width() == camera.width() && yFrame.height() == camera.height());
	if (yFrame.width() != camera.width() || yFrame.height() != camera.height())
	{
		return false;
	}

	ocean_assert(yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));
	if (!yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(yFrame.timestamp().isValid());
	if (!yFrame.timestamp().isValid())
	{
		return false;
	}

	ocean_assert(cameraGravity.isNull() || cameraGravity.isUnit());

	world_T_camera.toNull();

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.handleFrame_);

	// first, let's create a pyramid for the current frame
	// we store the new pyramid in a temporary variable until the background task has finished

	yFrame.makeOwner();

	const Index32 currentFrameIndex = cameraPoses_.nextFrameIndex();

	if (frameStatisticsEnabled_)
	{
		framesStatistics_.emplace_back(currentFrameIndex);
	}

	if (!cameraGravity.isNull())
	{
		gravities_.setGravity(currentFrameIndex, cameraGravity);
	}

	if (!trackingParameters_.isValid())
	{
		trackingParameters_ = TrackingParameters(yFrame.width(), yFrame.height(), configuration_);

		if (!trackingParameters_.isValid())
		{
			Log::error() << "Failed to determine tracking parameters";
		}
	}

	FramePyramidManager::ScopedPyramid tempCurrentPyramid = framePyramidManager_.newPyramid(currentFrameIndex);
	tempCurrentPyramid->replace(CV::FramePyramid::DM_FILTER_11, std::move(yFrame), CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, nullptr);

	// we need to wait until the background task has finished with post processing of the previous handleFrame() call

	DelayDebugger delayDebuggerWait;

	const BackgroundTask::WaitResult postHandleFrameResult = postHandleFrameTask_.wait();

	ocean_assert(postHandleFrameResult == BackgroundTask::WR_RELEASED || postHandleFrameResult == BackgroundTask::WR_PROCESSED);
	if (postHandleFrameResult == BackgroundTask::WR_RELEASED)
	{
		return false;
	}

	delayDebuggerWait.reportDelay("Wait");

	if constexpr (loggingEnabled_)
	{
		Log::info() << " ";
		Log::info() << "Frame index: " << currentFrameIndex << ", " << translateTrackerState(trackerState_);
	}

	cameraPoses_.nextFrame();
	ocean_assert(currentFrameIndex == cameraPoses_.frameIndex());

	// the background task has finished, we can now update the current pyramid

	previousPyramid_ = std::move(currentPyramid_);
	currentPyramid_ = std::move(tempCurrentPyramid);

	Quaternion previousCamera_Q_currentCamera(false);
	if (anyWorld_Q_previousCamera_.isValid() && anyWorld_Q_camera.isValid())
	{
		previousCamera_Q_currentCamera = anyWorld_Q_previousCamera_.inverted() * anyWorld_Q_camera;
	}

	SharedCameraPose cameraPose = trackImagePointsAndDeterminePose(camera, currentFrameIndex, randomGenerator_, previousCamera_Q_currentCamera);

	if (cameraPose)
	{
		const TrackerState previousTrackerState = trackerState_;
		TrackerState currentTrackerState = previousTrackerState;

		if (currentTrackerState == TS_INITIALIZING)
		{
			if (cameraPose->poseQuality() >= CameraPose::PQ_MEDIUM)
			{
				constexpr size_t minimalNumberObjectPoints = 40; // TODO tweak threshold, e.g., align with bins

				if (poseCorrespondences_.size() >= minimalNumberObjectPoints)
				{
					currentTrackerState = TS_TRACKING;
				}
			}
		}

#if 0
		if (currentTrackerState == TS_TRACKING) // TODO not during initialization?
#endif
		{
			// we do not report a camera pose until the tracker is initialized

			ocean_assert(cameraPose->isValid());
			world_T_camera = cameraPose->world_T_camera();
		}

		cameraPoses_.setPose(currentFrameIndex, std::move(cameraPose), poseCorrespondences_.mapVersion_);

		if (currentTrackerState != previousTrackerState)
		{
			// TODO ensure that we have enough described features before switching from intialization to tracking

			trackerState_ = currentTrackerState;

			if constexpr (loggingEnabled_)
			{
				Log::info() << "Changed state from " << translateTrackerState(previousTrackerState) << " to " << translateTrackerState(currentTrackerState);
			}
		}
	}

	if (debugData != nullptr)
	{
		debugData_.update(currentFrameIndex, trackingCorrespondences_, poseCorrespondences_);
		debugData_.posePreciseObjectPointIds_ = std::move(poseCorrespondences_.preciseObjectPointIds_);
		debugData_.poseNotPreciseObjectPointIds_ = std::move(poseCorrespondences_.impreciseObjectPointIds_);

		const ReadLock readLock(mutex_);

		// TOOD we should store the information in the pose/tracking correspondences to avoid a read lock in the handleFrame() function

		for (DebugData::PointMap::value_type& pointPair : debugData_.pointMap_)
		{
			const Index32 objectPointId = pointPair.first;
			DebugData::Point& point = pointPair.second;

			LocalizedObjectPointMap::const_iterator iLocalizedObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iLocalizedObjectPoint != localizedObjectPointMap_.cend())
			{
				point.isBundleAdjusted_ = iLocalizedObjectPoint->second.isBundleAdjusted();
			}
			else
			{
				point.isBundleAdjusted_ = false;
			}
		}

		*debugData = debugData_;
	}
	else
	{
		debugData_.clear();
	}

	DelayDebugger delayDebuggerExecute;

	postHandleFrameTask_.execute();

	delayDebuggerExecute.reportDelay("Execute");

	anyWorld_Q_previousCamera_ = anyWorld_Q_camera;

	return true;
}

bool TrackerMono::needsFrameStatistics(const Index32 frameIndex, FrameStatistics*& frameStatistics)
{
	frameStatistics = nullptr;

	if (!frameStatisticsEnabled_)
	{
		return false;
	}

	ocean_assert(!framesStatistics_.empty());
	ocean_assert(framesStatistics_.back().frameIndex_ == frameIndex);

	if (framesStatistics_.empty() || framesStatistics_.back().frameIndex_ != frameIndex)
	{
		return false;
	}

	frameStatistics = &framesStatistics_.back();

	return true;
}

SharedCameraPose TrackerMono::trackImagePointsAndDeterminePose(const AnyCamera& camera, const Index32 currentFrameIndex, RandomGenerator& randomGenerator, const Quaternion& previousCamera_Q_currentCamera)
{
	ocean_assert(camera.isValid());

	// first, we track points from the previous frame to the current frame, simple 2D/2D point tracking

	if (!previousPyramid_)
	{
		ocean_assert(currentFrameIndex == 0u);
		return nullptr;
	}

	ocean_assert(currentPyramid_.frameIndex() == currentFrameIndex);
	ocean_assert(previousPyramid_.frameIndex() + 1u == currentFrameIndex);

	const CV::FramePyramid& previousPyramid = *previousPyramid_;
	const CV::FramePyramid& currentPyramid = *currentPyramid_;

	HomogenousMatrix4 world_T_previousCamera(false);

	const Index32 previousFrameIndex = currentFrameIndex - 1u;

	if (trackerState_ == TS_TRACKING) // during initialization 3D locations and camera poses can be so far off that frame-to-frame tracking will fail if we use predictions based on projected object points
	{
		SharedCameraPose previousCameraPose = cameraPoses_.pose(previousFrameIndex);

		if (previousCameraPose)
		{
			if (previousCameraPose->mapVersion() == trackingCorrespondences_.mapVersion()) // TODO store previous pose in poseCorrespondences_ instead?
			{
				world_T_previousCamera = previousCameraPose->world_T_camera();
			}
		}
	}

	Quaternion previous_Q_current(false);

	if (configuration_.isInsideExpectedFrameInterval(previousPyramid.finestLayer().timestamp(), currentPyramid.finestLayer().timestamp()))
	{
		previous_Q_current = previousCamera_Q_currentCamera;
	}
	else
	{
		Log::warning() << "TrackerMono: Frame interval outside of expected frame interval: " << String::toAString(double(currentPyramid.finestLayer().timestamp() - previousPyramid.finestLayer().timestamp()) * 1000.0, 1u) << "ms";
	}

	performanceStatistics_.start(performanceStatistics_.trackImagePoints_);
		trackingCorrespondences_.trackImagePoints(currentFrameIndex, camera, world_T_previousCamera, previousPyramid, currentPyramid, trackingParameters_, previous_Q_current, minimalFrontPrecision_);
	performanceStatistics_.stop(performanceStatistics_.trackImagePoints_);

	if constexpr (loggingEnabled_)
	{
		const size_t validCounter = trackingCorrespondences_.countValidCorrespondences();
		const float validPercent = NumericF::ratio(float(validCounter), float(trackingCorrespondences_.previousImagePoints().size()), 0.0f);

		const Scalar imuDegree = previous_Q_current.isValid() ? Numeric::rad2deg(previous_Q_current.angle()) : 0;

		Log::info() << "Frame-to-frame tracking: IMU degree: " << imuDegree << ", valid correspondences: " << validCounter << "/" << trackingCorrespondences_.previousImagePoints().size() << " = " << String::toAString(validPercent * 100.0f, 1u) << "%";

		if (validPercent < 0.70f)
		{
			Log::warning() << "The frame-to-frame for the current frame is quite bad with, normally this should not happen";
		}
	}

	FrameStatistics* frameStatistics = nullptr;
	if (needsFrameStatistics(currentFrameIndex, frameStatistics))
	{
		frameStatistics->frameToFrameTrackingPossible_ = trackingCorrespondences_.previousImagePoints().size();
		frameStatistics->frameToFrameTrackingActual_ = trackingCorrespondences_.countValidCorrespondences();
		frameStatistics->mapVersion_ = trackingCorrespondences_.mapVersion();

		ocean_assert(frameStatistics->frameToFrameTrackingActual_ <= frameStatistics->frameToFrameTrackingPossible_);
	}

	if (taskDetermineInitialObjectPoints_)
	{
		// we still have an active task to re-initialize the object points, so we don't need to try to determine the camera pose

		if constexpr (loggingEnabled_)
		{
			Log::info() << "Pose estimation skipped, due to pending initialization";
		}

		return nullptr;
	}

	Geometry::GravityConstraints gravityConstraints;

	Vector3 cameraGravity(0, 0, 0);
	if (gravities_.hasGravity(currentFrameIndex, &cameraGravity))
	{
		gravityConstraints = Geometry::GravityConstraints(cameraGravity, configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_); // TODO locked access configuration
	}

	constexpr unsigned int minimalNumberCorrespondences = 20u;

	PerformanceStatistics::ScopedStatistic scopedPerformance(performanceStatistics_.determineCameraPose_);

	poseCorrespondences_.reset(trackingCorrespondences_);

	if (poseCorrespondences_.size() >= minimalNumberCorrespondences)
	{
		ocean_assert(trackingCorrespondences_.previousFrameIndex() == previousFrameIndex);

		const size_t initialNumberObjectPoints = poseCorrespondences_.size();

		const Scalar maximalProjectionError = configuration_.maximalProjectionError_;

		Scalar robustError = Numeric::maxValue();

		SharedCameraPose cameraPose = poseCorrespondences_.determinePose(camera, world_T_previousCamera, minimalNumberCorrespondences, randomGenerator, maximalProjectionError, Geometry::Estimator::ET_HUBER, gravityConstraints.conditionalPointer(), &robustError);

		if constexpr (loggingEnabled_)
		{
			if (cameraPose)
			{
				std::string gravityConstraintString;

				if (gravityConstraints.isValid())
				{
					const Scalar degree = Numeric::rad2deg(gravityConstraints.alignmentAngleIF(cameraPose->flippedCamera_T_world()));

					if (Numeric::isNotEqual(degree, Scalar(0), Scalar(0.1)))
					{
						gravityConstraintString += ", gravity alignment " + String::toAString(degree, 1u) + "deg";
					}
				}

				Log::info() << "Pose estimation succeeded, " << poseCorrespondences_.size() << " / " << initialNumberObjectPoints << " object points were used, previous pose: " << int(world_T_previousCamera.isValid()) << ", map version: " << poseCorrespondences_.mapVersion_ << ", with a projection error of " << robustError << "px" << gravityConstraintString;
			}
			else
			{
				Log::info() << "Pose estimation failed";
			}
		}

		if (cameraPose)
		{
			if (frameStatistics != nullptr)
			{
				frameStatistics->poseEstimationCorrespondences_ = poseCorrespondences_.size();
				frameStatistics->projectionError_ = robustError;
				frameStatistics->world_T_camera_ = cameraPose->world_T_camera();
			}

			inaccurateObjectPointIdSet_.add(poseCorrespondences_.outlierObjectPointIds_);

			poseEstimationFailedFrameIndex_ = Index32(-1);

			return cameraPose;
		}
		else
		{
			ocean_assert(poseCorrespondences_.outlierObjectPointIds_.empty());
		}
	}
	else
	{
		if constexpr (loggingEnabled_)
		{
			Log::info() << "Skipping pose estimation, not enough 3D object points in correspondence data";
		}
	}

	// pose estimation failed

	if (trackerState_ == TS_INITIALIZING)
	{
		// the tracking is still initializing, so let's restart the task to determine initial object points
		// however, in case we had no 3D object points, we delay the execution by one frame to ensure that the post-frame task was executed to collect all 3D object points

		if (poseCorrespondences_.isEmpty() && poseEstimationFailedFrameIndex_ == Index32(-1))
		{
			poseEstimationFailedFrameIndex_ = currentFrameIndex;
		}
		else
		{
			if (!poseCorrespondences_.isEmpty())
			{
				Log::warning() << "Tracking or pose estimation failure during initialization, resetting the tracker for frame index " << currentFrameIndex;
			}

			ocean_assert(!taskDetermineInitialObjectPoints_);
			taskDetermineInitialObjectPoints_ = true;

			poseEstimationFailedFrameIndex_ = Index32(-1);
		}
	}

	return nullptr;
}

std::string TrackerMono::performance() const
{
	return performanceStatistics_.toString();
}

TrackerMono::FramesStatistics TrackerMono::framesStatistics() const
{
	return framesStatistics_;
}

void TrackerMono::postHandleFrame()
{
	ocean_assert(camera_ && camera_->isValid());

	ocean_assert(currentPyramid_);
	ocean_assert(currentPyramid_.frameIndex() == cameraPoses_.frameIndex());

	const Index32 currentFrameIndex = currentPyramid_.frameIndex();

	const SharedCameraPose currentCameraPose = cameraPoses_.pose(currentFrameIndex);

	const unsigned int frameWidth = camera_->width();
	const unsigned int frameHeight = camera_->height();

	// first, let's initialize or clear the occupancy array

	if (!occupancyArray_.isValid())
	{
		unsigned int horizontalBins = 0u;
		unsigned int verticalBins = 0u;
		Geometry::SpatialDistribution::idealBins(frameWidth, frameHeight, configuration_.numberBins_, horizontalBins, verticalBins);

		ocean_assert(horizontalBins >= 1u && verticalBins >= 1u);
		if (horizontalBins == 0u || verticalBins == 0u)
		{
			return;
		}

		constexpr unsigned int neighborhoodSize = 3u;
		constexpr float minCoverageThreshold = 0.8f;

		horizontalBins *= neighborhoodSize;
		verticalBins *= neighborhoodSize;

		Log::debug() << "TrackerMono: Using " << horizontalBins << "x" << verticalBins << " bins for the occupancy array (with 3x3 neighborhood), and coverage threshold " << String::toAString(minCoverageThreshold * 100.0f, 1u) << "%";

		occupancyArray_ = OccupancyArray(Scalar(0), Scalar(0), frameWidth, frameHeight, horizontalBins, verticalBins, neighborhoodSize, minCoverageThreshold);
	}
	else
	{
		ocean_assert(occupancyArray_.width() == Scalar(frameWidth));
		ocean_assert(occupancyArray_.height() == Scalar(frameHeight));

		occupancyArray_.removePoints();
	}

	// add new observations to the unlocalized or localized object points maps, update the occupancy array
	processTrackingResults(currentFrameIndex, trackingCorrespondences_);

	framePyramidManager_.updateLatest(currentFrameIndex);

	if constexpr (SLAMDebugElements::allowDebugging_)
	{
		if (SLAMDebugElements::get().isElementActive(SLAMDebugElements::EI_OCCUPANCY_ARRAY))
		{
			// no read lock necessary, as the occupancy array is not modified in the post processing thread

			SLAMDebugElements::get().updateOccupancyArray(currentPyramid_->finestLayer(), occupancyArray_);
		}
	}

	constexpr bool tryMatchCornersToLocalizedObjectPoints = true; // allows to toggle between forward/backward matching

	if (!tryMatchCornersToLocalizedObjectPoints)
	{
		if (currentCameraPose)
		{
			matchLocalizedObjectPointsToCorners(*camera_, currentFrameIndex, *currentCameraPose, *currentPyramid_);
		}
	}

	detectNewImagePoints(*camera_, currentFrameIndex, *currentPyramid_, tryMatchCornersToLocalizedObjectPoints);

	ReadLock readLock(mutex_, "TrackerMono::postHandleFrame(), update correspondences");
		trackingCorrespondences_.update(currentFrameIndex, mapVersion_, localizedObjectPointMap_, pointTrackMap_, minimalFrontPrecision_);
	readLock.unlock();

	if constexpr (SLAMDebugElements::allowDebugging_)
	{
		if (SLAMDebugElements::get().isElementActive(SLAMDebugElements::EI_OBJECT_POINTS) || SLAMDebugElements::get().isElementActive(SLAMDebugElements::EI_IMAGE_POINTS))
		{
			readLock = ReadLock(mutex_, "TrackerMono::postHandleFrame(), debug elements");

			if (currentCameraPose)
			{
				SLAMDebugElements::get().updateObjectPoints(currentPyramid_->finestLayer(), *camera_, *currentCameraPose, currentFrameIndex, localizedObjectPointMap_);
			}

			SLAMDebugElements::get().updateImagePoints(currentPyramid_->finestLayer(), currentFrameIndex, pointTrackMap_);
		}
	}
}

void TrackerMono::resetLocalizedObjectPoints()
{
	// the localized object points are not precise enough, so we remove all of them
	// however, we will keep the observations of all currently visible object points and convert these object points to unlocalized object points

#ifdef OCEAN_DEBUG
	ocean_assert(WriteLock::debugIsLocked(mutex_));
#endif

	ocean_assert(trackerState_ == TS_INITIALIZING);

	localizedObjectPointMap_.clear();
	inaccurateObjectPointIdSet_.clear();

	cameraPoses_.removePoses();

	bundleAdjustmentKeyFrameIndices_.clear();
	bundleAdjustmentSqrBaseline_ = Numeric::minValue();
	bundleAdjustmentObjectPointIdSet_.clear();
}

void TrackerMono::threadRun()
{
	Log::debug() << "TrackerMono background thread started";

	Thread::setThreadPriority(Thread::PRIORTY_BELOW_NORMAL);

	Index32 latestFrameIndex = Index32(-1);

	while (!shouldThreadStop())
	{
		FramePyramidManager::ScopedPyramid latestFramePyramid(framePyramidManager_.latestPyramid());

		if (!latestFramePyramid || latestFramePyramid.frameIndex() == latestFrameIndex)
		{
			Thread::sleep(1u);
			continue;
		}

		latestFrameIndex = latestFramePyramid.frameIndex();

		ocean_assert(camera_ && camera_->isValid());

		if (taskDetermineInitialObjectPoints_)
		{
			// let's try to determine the initial positions of 3D object points

			if (determineInitialObjectPoints(*camera_, latestFrameIndex, randomGeneratorBackground_))
			{
				taskDetermineInitialObjectPoints_ = false;
			}
		}
		else
		{
			if (cameraPoses_.hasPose(latestFrameIndex))
			{
				const UnorderedIndexSet32 inaccurateObjectPointIds = inaccurateObjectPointIdSet_.objectPointIds();

				if (!inaccurateObjectPointIds.empty())
				{
					// let's try to improve the precision of existing localized 3D objec points

					updateInaccurateObjectPoints(*camera_, latestFrameIndex, inaccurateObjectPointIds);
				}
			}
			else
			{
				const TrackerState trackerState = trackerState_;

				if (trackerState == TS_TRACKING)
				{
					ocean_assert(trackerState == TS_TRACKING);

					if (relocalize(*camera_, latestFrameIndex, *latestFramePyramid))
					{
						// let's skip any additional post processing steps for this frame
						continue;
					}
				}
			}

			// let's try to execute a Bundle Adjustment

			bundleAdjustment(*camera_, latestFrameIndex);

			// let's try to create new localized 3D object points from unlocalized point tracks

			localizeUnlocalizedObjectPoints(*camera_, latestFrameIndex);

			// let's add more descriptors to localized 3D object points

			describeObjectPoints(*camera_, latestFrameIndex, *latestFramePyramid);
		}
	}

	Log::debug() << "TrackerMono background thread stopped";
}

bool TrackerMono::determineInitialObjectPoints(const AnyCamera& camera, const Index32 latestFrameIndex, RandomGenerator& randomGenerator)
{
	// first, let's get 2D/2D image correspondences between the 'latestFrameIndex' frame and a suitable previous frame 'firstFrameIndex'

	ReadLock readLock(mutex_, "TrackerMono::determineInitialObjectPoints()");

		ocean_assert(taskDetermineInitialObjectPoints_);
		ocean_assert(trackerState_ == TS_INITIALIZING);

		const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.determineInitialObjectPoints_);

		// let's determine the track lengths of all points tracks from their start all the way to the last frame index

		constexpr size_t minimalTracks = 30; // TODO tweak threshold

		const size_t trackLengthP95 = PointTrack::determineTracksLengthUntil(latestFrameIndex, pointTrackMap_, minimalTracks, 0.95);

		if (trackLengthP95 < 10) // ~0.3s for 30fps, TODO tweak threshold
		{
			return false;
		}

		const size_t trackLengthP95_2 = trackLengthP95 / 2; // we use all tracks which have at least half the length of the P95 track

		const Index32 firstFrameIndex = latestFrameIndex - Index32(trackLengthP95_2) + 1u;
		ocean_assert(firstFrameIndex + Index32(trackLengthP95_2) - 1u == latestFrameIndex);

		Vectors2 imagePoints0; // TODO make re-usable
		Vectors2 imagePoints1;
		Indices32 objectPointIds;

		PointTrack::extractCorrespondences(firstFrameIndex, latestFrameIndex, pointTrackMap_, imagePoints0, imagePoints1, objectPointIds);

	readLock.unlock();

	constexpr size_t minimalNumberLocalizedObjectPoints = 20; // TODO tweak threshold

	if (imagePoints0.size() < minimalNumberLocalizedObjectPoints)
	{
		return false;
	}

	Geometry::GravityConstraints gravityConstraints;

	Vector3 cameraGravity0(0, 0, 0);
	Vector3 cameraGravity1(0, 0, 0);
	if (gravities_.hasGravity(firstFrameIndex, &cameraGravity0) && gravities_.hasGravity(latestFrameIndex, &cameraGravity1))
	{
		gravityConstraints = Geometry::GravityConstraints(Vectors3({cameraGravity0, cameraGravity1}), configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_);
	}

	ocean_assert(firstFrameIndex < latestFrameIndex);

	Vectors3 objectPoints;
	Indices32 validIndices;

	HomogenousMatrix4 world_T_camera0(false);
	HomogenousMatrix4 world_T_camera1(false);

	constexpr Scalar maxRotationalSqrError = Scalar(1.5 * 1.5);
	constexpr Scalar maxArbitrarySqrError = Scalar(3.5 * 3.5);
	constexpr unsigned int iterations = 100u;
	constexpr Scalar rotationalMotionMinimalValidCorrespondencesPercent = Scalar(0.9);
	constexpr Scalar baselineDistance = Scalar(0.01);

	if (!Geometry::StereoscopicGeometry::cameraPose(camera, ConstArrayAccessor<Vector2>(imagePoints0), ConstArrayAccessor<Vector2>(imagePoints1), randomGenerator, world_T_camera0, world_T_camera1, gravityConstraints.conditionalPointer(), &objectPoints, &validIndices, maxRotationalSqrError, maxArbitrarySqrError, iterations, rotationalMotionMinimalValidCorrespondencesPercent, baselineDistance))
	{
		Log::debug() << "Failed to determine camera pose";
		return false;
	}

	ocean_assert(objectPoints.size() == validIndices.size());

	if (world_T_camera1.translation().isNull())
	{
		if constexpr (loggingEnabled_)
		{
			Log::info() << "    Background: Frame index " << latestFrameIndex << ", Initial 3D object points: FAILED due to pure rotational camera pose, with " << validIndices.size() << " correspondences";
		}

		return false;
	}

	const Scalar angleP75 = Numeric::rad2deg(PointTrack::determineViewingAngle(camera, world_T_camera0, world_T_camera1, imagePoints0, imagePoints1, validIndices, 0.75));

	if (angleP75 < Scalar(1.5)) // TODO tweak threshold
	{
		if constexpr (loggingEnabled_)
		{
			Log::info() << "    Background: Frame index " << latestFrameIndex << ", Initial 3D object points: FAILED due to median viewing ray angle: " << angleP75 << "deg, with " << validIndices.size() << " correspondences, used frames " << firstFrameIndex << ", " << latestFrameIndex;
		}

		return false;
	}

	const Scalar sqrBaseline = world_T_camera0.translation().sqrDistance(world_T_camera1.translation());

	// we convert all used unlocalized object points to localized object points

	WriteLock writeLock(mutex_, "TrackerMono::determineInitialObjectPoints()");

		resetLocalizedObjectPoints();

		for (size_t nIndex = 0; nIndex < validIndices.size(); ++nIndex)
		{
			const Vector3& position = objectPoints[nIndex];
			const Index32& validIndex = validIndices[nIndex];

			const Index32& objectPointId = objectPointIds[validIndex];

			const PointTrackMap::iterator iPoint = pointTrackMap_.find(objectPointId);

			if (iPoint == pointTrackMap_.cend())
			{
				// the unlocalized object point has been deleted during frame-to-frame tracking (in the main thread)

				continue;
			}

			PointTrack& pointTrack = iPoint->second;

			ocean_assert(!localizedObjectPointMap_.contains(objectPointId));

			constexpr LocalizedObjectPoint::LocalizationPrecision initialLocalizationPrecision = LocalizedObjectPoint::LP_UNKNOWN;

			localizedObjectPointMap_.emplace(objectPointId, LocalizedObjectPoint(pointTrack, position, initialLocalizationPrecision, true /*isBundleAdjusted*/));

			bundleAdjustmentObjectPointIdSet_.insert(objectPointId);
		}

		if constexpr (loggingEnabled_)
		{
			const HomogenousMatrix4 flippedCamera0_T_world = Camera::standard2InvertedFlipped(world_T_camera0);
			const HomogenousMatrix4 flippedCamera1_T_world = Camera::standard2InvertedFlipped(world_T_camera1);

			Scalar minError0 = Numeric::maxValue();
			Scalar averageError0 = Numeric::maxValue();
			Scalar maxError0 = Numeric::maxValue();
			const size_t correspondences0 = LocalizedObjectPoint::determineCameraPoseQualityIF(camera, flippedCamera0_T_world, firstFrameIndex, localizedObjectPointMap_, minError0, averageError0, maxError0);

			Scalar minError1 = Numeric::maxValue();
			Scalar averageError1 = Numeric::maxValue();
			Scalar maxError1 = Numeric::maxValue();
			const size_t correspondences1 = LocalizedObjectPoint::determineCameraPoseQualityIF(camera, flippedCamera1_T_world, latestFrameIndex, localizedObjectPointMap_, minError1, averageError1, maxError1);

			Log::info() << "    Background: Frame index " << latestFrameIndex << ", Initial 3D object points: Translational camera pose with median viewing ray angle: " << angleP75 << "deg, with " << bundleAdjustmentObjectPointIdSet_.size() << " correspondences, used frames " << firstFrameIndex << ", " << latestFrameIndex;

			std::string gravityConstraintString0;
			std::string gravityConstraintString1;

			if (gravityConstraints.isValid())
			{
				gravityConstraintString0 += ", gravity alignment " + String::toAString(Numeric::rad2deg(gravityConstraints.alignmentAngleIF(flippedCamera0_T_world, 0)), 1u) + "deg";
				gravityConstraintString1 += ", gravity alignment " + String::toAString(Numeric::rad2deg(gravityConstraints.alignmentAngleIF(flippedCamera1_T_world, 1)), 1u) + "deg";
			}

			Log::info() << "    Background: Initial 3D object points statistics: First image: " << correspondences0 << " correspondences, with projection errors: " << minError0 << ", " << averageError0 << ", " << maxError0 << gravityConstraintString0;
			Log::info() << "    Background: Initial 3D object points statistics: Second image: " << correspondences1 << " correspondences, with projection errors: " << minError1 << ", " << averageError1 << ", " << maxError1 << gravityConstraintString1;
		}

		const CameraPose::PoseQuality poseQuality = CameraPose::PQ_LOW; // low quality as we determine a very first pose

		ocean_assert(!cameraPoses_.hasPose(firstFrameIndex));

		++mapVersion_;

		cameraPoses_.setPose(firstFrameIndex, std::make_shared<CameraPose>(world_T_camera0, poseQuality), mapVersion_);
		cameraPoses_.setPose(latestFrameIndex, std::make_shared<CameraPose>(world_T_camera1, poseQuality), mapVersion_);

		bundleAdjustmentKeyFrameIndices_ = {firstFrameIndex, latestFrameIndex};
		bundleAdjustmentSqrBaseline_ = sqrBaseline;

	writeLock.unlock();

	// for now, we do not determine camera poses for all remaining frames (past or future)

	return true;
}

void TrackerMono::updateInaccurateObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const UnorderedIndexSet32& inaccurateObjectPointIdSet)
{
	ocean_assert(camera.isValid());
	ocean_assert(!inaccurateObjectPointIdSet.empty());

	const PerformanceStatistics::ScopedStatistic scopedStatisticOptimizeBadObjectPoints(performanceStatistics_.optimizeBadObjectPoints_);

	const Scalar maximalProjectionError = configuration_.maximalProjectionError_;

	ObjectPointIdPositionPairs& objectPointIdPositionPairs = reusableObjectPointIdPositionPairs_;
	objectPointIdPositionPairs.clear();
	objectPointIdPositionPairs.reserve(inaccurateObjectPointIdSet.size());

	for (const Index32& badObjectPointId : inaccurateObjectPointIdSet)
	{
		const ReadLock readLock(mutex_, "TrackerMono::updateBadObjectPoints()");

		LocalizedObjectPointMap::const_iterator iObjectPoint = localizedObjectPointMap_.find(badObjectPointId);

		if (iObjectPoint != localizedObjectPointMap_.cend())
		{
			const LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

			if (localizedObjectPoint.hasObservation(currentFrameIndex))
			{
				// the object point is currently visible, let's try to optimize the position

				constexpr size_t minimalNumberObservations = 10;

				Vector3 optimizedObjectPoint;
				const LocalizedObjectPoint::OptimizationResult optimizationResult = localizedObjectPoint.optimizedObjectPoint(mapVersion_, camera, cameraPoses_, currentFrameIndex, minimalNumberObservations, maximalProjectionError, Geometry::Estimator::ET_HUBER, optimizedObjectPoint);

				switch (optimizationResult)
				{
					case LocalizedObjectPoint::OR_SUCCEEDED:
						objectPointIdPositionPairs.emplace_back(badObjectPointId, optimizedObjectPoint);
						break;

					case LocalizedObjectPoint::OR_INACCURATE:
						objectPointIdPositionPairs.emplace_back(badObjectPointId, Vector3::minValue());
						break;

					case LocalizedObjectPoint::OR_NOT_ENOUGH_OBSERVATIONS:
						break;
				}
			}
		}
	}

	if (objectPointIdPositionPairs.empty())
	{
		return;
	}

	size_t numberOptimizedObjectPoints = 0;
	size_t numberFailedOptimizedObjectPoints = 0;

	{
		const WriteLock writeLock(mutex_, "TrackerMono::updateBadObjectPoints()");

		for (const ObjectPointIdPositionPair& objectPointIdPositionPair : objectPointIdPositionPairs)
		{
			const Index32& objectPointId = objectPointIdPositionPair.first;
			const Vector3& objectPointPosition = objectPointIdPositionPair.second;

			LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint != localizedObjectPointMap_.cend())
			{
				LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

				if (objectPointPosition != Vector3::minValue())
				{
					localizedObjectPoint.setPosition(objectPointPosition, false /*isBundleAdjusted*/);

					++numberOptimizedObjectPoints;
				}
				else
				{
					// we do not yet remove the object points, perhaps the object point can be fixed during the next Bundle Adjustment

					++numberFailedOptimizedObjectPoints;
				}
			}
		}

		if constexpr (loggingEnabled_)
		{
			Log::info() << "    Background: Frame index " << currentFrameIndex << ", Optimized object points: " << numberOptimizedObjectPoints << ", failed: " << numberFailedOptimizedObjectPoints;
		}
		else
		{
			OCEAN_SUPPRESS_UNUSED_WARNING(numberOptimizedObjectPoints);
			OCEAN_SUPPRESS_UNUSED_WARNING(numberFailedOptimizedObjectPoints);
		}
	}
}

void TrackerMono::bundleAdjustment(const AnyCamera& camera, const Index32 currentFrameIndex)
{
	ocean_assert(camera.isValid());

	const Scalar maximalProjectionError = configuration_.maximalProjectionError_;

	ReadLock readLock(mutex_, "TrackerMono::bundleAdjustment()");

		const Index32 necessaryMapVersion = mapVersion_;

		if (localizedObjectPointMap_.empty())
		{
			return;
		}

		const SharedCameraPose currentCameraPose = cameraPoses_.pose(currentFrameIndex);

		if (!currentCameraPose)
		{
			return;
		}

		if (currentCameraPose->mapVersion() != necessaryMapVersion)
		{
			ocean_assert(currentCameraPose->mapVersion() < necessaryMapVersion);

			// the current camera pose has been determined based on an older feature map, so let's skip this frame
			return;
		}

		if (currentCameraPose->estimatedMotion() != CameraPose::EM_TRANSLATIONAL)
		{
			// the user is currently not moving, so there is no reason to apply a bundle adjustment
			return;
		}

		if (!isBundleAdjustmentNeeded(camera, *currentCameraPose, currentFrameIndex, maximalProjectionError, necessaryMapVersion))
		{
			// the current map is precise enough
			return;
		}

		const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.bundleAdjustment_);

		constexpr size_t maximalNumberNewKeyFrames = 1; // we add one new key-frame with each new Bundle Adjustment execution
		constexpr size_t minimalNumberKeyFrames = 3;
		constexpr size_t maximalNumberKeyFrames = 15;

		Indices32 keyFrameIndices = bundleAdjustmentKeyFrameIndices_;

		ObjectPointToObservations objectPointToObservations;
		if (!determineBundleAdjustmentTopology(necessaryMapVersion, cameraPoses_, localizedObjectPointMap_, maximalNumberNewKeyFrames, maximalNumberKeyFrames, keyFrameIndices, objectPointToObservations, minimalNumberKeyFrames))
		{
			return;
		}

		UnorderedIndexSet32 bundleAdjustmentObjectPointIdSet(bundleAdjustmentObjectPointIdSet_); // we make a local copy because we may need to modify it

		ocean_assert(keyFrameIndices.size() >= minimalNumberKeyFrames);

		Vectors3 objectPoints; // TODO make re-usable
		Indices32 objectPointIds;
		HomogenousMatrices4 flippedCameras_T_world;
		Geometry::NonLinearOptimization::ObjectPointToPoseIndexImagePointCorrespondenceAccessor correspondenceGroups;

		flippedCameras_T_world.reserve(keyFrameIndices.size());

		for (const Index32& keyFrameIndex : keyFrameIndices)
		{
			const SharedCameraPose cameraPose = cameraPoses_.pose(keyFrameIndex);
			ocean_assert(cameraPose);

			ocean_assert(cameraPose->mapVersion() == necessaryMapVersion);

			flippedCameras_T_world.push_back(cameraPose->flippedCamera_T_world());
		}

		for (const ObjectPointToObservations::value_type& objectPointPair : objectPointToObservations)
		{
			const Index32& objectPointId = objectPointPair.first;
			const PoseIndexToImagePointPairs& poseIndexToImagePointPairs = objectPointPair.second;

			ocean_assert(poseIndexToImagePointPairs.size() >= 2);

			ocean_assert(localizedObjectPointMap_.contains(objectPointId));
			const LocalizedObjectPoint& localizedObjectPoint = localizedObjectPointMap_.find(objectPointId)->second;

			const Vector3& position = localizedObjectPoint.position();

			bool useForBundleAdjustment = true;

			if (bundleAdjustmentObjectPointIdSet.contains(objectPointId))
			{
				// the object point has been used during the last Bundle Adjustment phase, so we know that it is precise
			}
			else
			{
				// this 3D object point has not been used during Bundle Adjustment before, so we first need to verify the precision across all key frames

				for (const PoseIndexToImagePointPair& poseIndexToImagePointPair : poseIndexToImagePointPairs)
				{
					const unsigned int& keyFrameIndex = poseIndexToImagePointPair.first;
					const ImagePoint& imagePoint = poseIndexToImagePointPair.second;

					ocean_assert(keyFrameIndex < keyFrameIndices.size());

					const HomogenousMatrix4& flippedCamera_T_world = flippedCameras_T_world[keyFrameIndex];

					if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, position))
					{
						const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, position);

						const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

						if (sqrDistance > Numeric::sqr(maximalProjectionError))
						{
							useForBundleAdjustment = false;
							break;
						}
					}
					else
					{
						useForBundleAdjustment = false;
						break;
					}
				}

				if (useForBundleAdjustment)
				{
					bundleAdjustmentObjectPointIdSet.emplace(objectPointId);
				}
			}

			if (useForBundleAdjustment)
			{
				objectPointIds.push_back(objectPointId);
				objectPoints.push_back(position);

				correspondenceGroups.addObjectPoint(PoseIndexToImagePointPairs(poseIndexToImagePointPairs)); // TODO, use move operator
			}
			else
			{
				Log::debug() << "    Background: Object point " << objectPointId << " will not be used during Bundle Adjustment";
			}
		}

		// let's gather all object points which are visible in at least two key-frames but which will not be used during Bundle Adjustment

		ObjectPointOptimization objectPointOptimization(keyFrameIndices); // TODO make re-usable
		objectPointOptimization.collectObjectPoints(localizedObjectPointMap_, bundleAdjustmentObjectPointIdSet);

	readLock.unlock();

	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.size() < 10)
	{
		return;
	}

	if constexpr (loggingEnabled_)
	{
		Log::info() << "    Background: Starting bundle adjustment result: Frame index " << currentFrameIndex;
	}

	Vectors3 cameraGravities;
	cameraGravities.reserve(keyFrameIndices.size());

	for (const Index32& keyFrameIndex : keyFrameIndices)
	{
		Vector3 cameraGravity(0, 0, 0);
		if (gravities_.hasGravity(keyFrameIndex, &cameraGravity))
		{
			cameraGravities.push_back(cameraGravity);
		}
	}

	Geometry::GravityConstraints gravityConstraints;

	if (keyFrameIndices.size() == cameraGravities.size())
	{
		gravityConstraints = Geometry::GravityConstraints(std::move(cameraGravities), configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_);
	}

	HomogenousMatrices4 optimizedFlippedCameras_T_world(flippedCameras_T_world.size());
	Vectors3 optimizedObjectPoints(objectPoints.size());

	NonconstArrayAccessor<HomogenousMatrix4> accessorOptimizedPoses(optimizedFlippedCameras_T_world);
	NonconstArrayAccessor<Vector3> accessorOptimizedObjectPoints(optimizedObjectPoints);

	Scalar initialError = Numeric::maxValue();
	Scalar finalError = Numeric::maxValue();

	constexpr bool applyAbsolutePoseAlignment = true;

	constexpr Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_SQUARE;

	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPosesIF(camera, ConstArrayAccessor<HomogenousMatrix4>(flippedCameras_T_world), ConstArrayAccessor<Vector3>(objectPoints), correspondenceGroups, &accessorOptimizedPoses, &accessorOptimizedObjectPoints, 20u, estimatorType, Scalar(0.001), Scalar(5), true /*onlyFrontObjectPoints*/, &initialError, &finalError, nullptr, gravityConstraints.isValid() ? &gravityConstraints : nullptr, applyAbsolutePoseAlignment))
	{
		Log::warning() << "Failed to run Bundle Adjustment";
		return;
	}

	if constexpr (loggingEnabled_)
	{
		std::string gravityConstraintsString;

		if (gravityConstraints.isValid())
		{
			Scalar maxAlignmentAngle = Numeric::minValue();

			for (size_t n = 0; n < flippedCameras_T_world.size(); ++n)
			{
				const Scalar alignmentAngle = gravityConstraints.alignmentAngleIF(flippedCameras_T_world[n], n);

				if (alignmentAngle > maxAlignmentAngle)
				{
					maxAlignmentAngle = alignmentAngle;
				}
			}

			if (Numeric::isNotEqual(maxAlignmentAngle, Scalar(0), Scalar(0.1)))
			{
				gravityConstraintsString += ", max gravity alignment " + String::toAString(Numeric::rad2deg(maxAlignmentAngle), 1u) + "deg";
			}
		}

		Log::info() << "    Background: Bundle adjustment result: Frame index " << currentFrameIndex << ", using " << flippedCameras_T_world.size() << " cameras and " << objectPoints.size() << " object points: " << initialError << " -> " << finalError << gravityConstraintsString;

		Indices32 sortedFrameIndices(keyFrameIndices);
		std::sort(sortedFrameIndices.begin(), sortedFrameIndices.end());

		std::string frameIndicesString = String::toAString(sortedFrameIndices.front());

		for (size_t n = 1; n < sortedFrameIndices.size(); ++n)
		{
			frameIndicesString += ", " + String::toAString(sortedFrameIndices[n]);
		}

		Log::info() << "    Background: Bundle adjustment used the following frames: " << frameIndicesString;
	}


	// now we determine the ids of all bundle adjusted object points

	UnorderedIndexSet32 newBundleAdjustmentObjectPointIdSet;

	for (size_t nObjectPoint = 0; nObjectPoint < objectPointIds.size(); ++nObjectPoint)
	{
		const Index32 objectPointId = objectPointIds[nObjectPoint];
		const Vector3& objectPoint = optimizedObjectPoints[nObjectPoint];

		const size_t observations = correspondenceGroups.groupElements(nObjectPoint);

		bool objectPointIsPrecise = true;

		for (size_t nObservation = 0; nObservation < observations; ++nObservation)
		{
			Index32 keyFrameSubsetIndex = Index32(-1);
			Vector2 imagePoint;
			correspondenceGroups.element(nObjectPoint, nObservation, keyFrameSubsetIndex, imagePoint);

			ocean_assert(keyFrameSubsetIndex < optimizedFlippedCameras_T_world.size());

			const HomogenousMatrix4& optimizedFlippedCamera_T_world = optimizedFlippedCameras_T_world[keyFrameSubsetIndex];
			ocean_assert(AnyCamera::isObjectPointInFrontIF(optimizedFlippedCamera_T_world, objectPoint));

			const Vector2 projectedObjectPoint = camera.projectToImageIF(optimizedFlippedCamera_T_world, objectPoint);

			const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

			if (sqrDistance > Numeric::sqr(maximalProjectionError))
			{
				objectPointIsPrecise = false;
				break;
			}
		}

		if (objectPointIsPrecise)
		{
			newBundleAdjustmentObjectPointIdSet.emplace(objectPointId);
		}
	}


	// now let's try to determine the precise 3D position of all localized object points which were not used during Bundle Adjustments but which are visible at least in two key-frames

	Indices32 inaccurateObjectPointIds;
	objectPointOptimization.optimizeObjectPointsIF(camera, optimizedFlippedCameras_T_world, estimatorType, maximalProjectionError, newBundleAdjustmentObjectPointIdSet, objectPointIds, optimizedObjectPoints, inaccurateObjectPointIds);

	ocean_assert(UnorderedIndexSet32(objectPointIds.cbegin(), objectPointIds.cend()).size() == objectPointIds.size());


	// let's ensure that all 3D object points are within a resonable distance

	const HomogenousMatrices4 world_T_optimizedCameras = Camera::invertedFlipped2Standard(optimizedFlippedCameras_T_world);

	Geometry::NonLinearOptimizationObjectPoint::clampDistantObjectPoints(world_T_optimizedCameras, optimizedObjectPoints, Scalar(10000));


	// let's determine all baselines between all key-frames

	Scalars sqrBaselines;
	sqrBaselines.reserve(keyFrameIndices.size() * (keyFrameIndices.size() - 1) / 2);

	for (size_t nOuter = 0; nOuter < keyFrameIndices.size() - 1; ++nOuter)
	{
		const Vector3& outerKeyFrameTranslation = world_T_optimizedCameras[nOuter].translation();

		for (size_t nInner = nOuter + 1; nInner < keyFrameIndices.size(); ++nInner)
		{
			const Scalar sqrBaseline = outerKeyFrameTranslation.sqrDistance(world_T_optimizedCameras[nInner].translation());

			sqrBaselines.push_back(sqrBaseline);
		}
	}

	ocean_assert(!sqrBaselines.empty());
	const Scalar medianSqrBaseline = Median::percentile(sqrBaselines.data(), sqrBaselines.size(), 1.0);

	{
		const WriteLock writeLock(mutex_, "TrackerMono::bundleAdjustment()");

		if (taskDetermineInitialObjectPoints_)
		{
			resetLocalizedObjectPoints();
			return;
		}

		// first, let's increment the map version, then update all the camera poses which got optimized during Bundle Adjustment, and update the 3D object points

		++mapVersion_;

		ocean_assert(keyFrameIndices.size() == optimizedFlippedCameras_T_world.size());
		ocean_assert(keyFrameIndices.size() == world_T_optimizedCameras.size());

		for (size_t nKeyFrame = 0; nKeyFrame < keyFrameIndices.size(); ++nKeyFrame)
		{
			const Index32& frameIndex = keyFrameIndices[nKeyFrame];

			const HomogenousMatrix4& optimizedFlippedCamera_T_world = optimizedFlippedCameras_T_world[nKeyFrame];
			const HomogenousMatrix4& world_T_optimizedCamera = world_T_optimizedCameras[nKeyFrame];

			ocean_assert(cameraPoses_.hasPose(frameIndex));

			cameraPoses_.setPose(frameIndex, std::make_shared<CameraPose>(world_T_optimizedCamera, optimizedFlippedCamera_T_world, CameraPose::PQ_HIGH), mapVersion_);
		}

		ocean_assert(objectPointIds.size() == optimizedObjectPoints.size());
		for (size_t nObjectPoint = 0; nObjectPoint < objectPointIds.size(); ++nObjectPoint)
		{
			const Index32& objectPointId = objectPointIds[nObjectPoint];
			const Vector3& optimizedObjectPoint = optimizedObjectPoints[nObjectPoint];

			const LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint != localizedObjectPointMap_.cend())
			{
				LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

				localizedObjectPoint.setPosition(optimizedObjectPoint, true /*isBundleAdjusted*/);

				localizedObjectPoint.updateLocalizedObjectPointUncertainty(camera, cameraPoses_);
			}
		}

		// now let's remove all object points which have been determined as inaccurate during Bundle Adjustment

		if (!inaccurateObjectPointIds.empty())
		{
			for (const Index32 inaccurateObjectPointId : inaccurateObjectPointIds)
			{
				Log::debug() << "    Background: Bundle adjustment removed inaccurate object point " << inaccurateObjectPointId;

				localizedObjectPointMap_.erase(inaccurateObjectPointId);
			}

			if constexpr (loggingEnabled_)
			{
				Log::info() << "    Background: Bundle adjustment removed " << inaccurateObjectPointIds.size() << " inaccurate 3D object points";
			}
		}

		bundleAdjustmentObjectPointIdSet_ = std::move(newBundleAdjustmentObjectPointIdSet);

		bundleAdjustmentKeyFrameIndices_ = std::move(keyFrameIndices);
		bundleAdjustmentSqrBaseline_ = medianSqrBaseline;

		if constexpr (loggingEnabled_)
		{
			Log::info() << "    Background: Bundle adjustment integrated results based on frame index " << currentFrameIndex << ", at current frame index " << cameraPoses_.frameIndex() << ", map version: " << mapVersion_;

			for (size_t nKeyFrame = 0; nKeyFrame < bundleAdjustmentKeyFrameIndices_.size(); ++nKeyFrame)
			{
				const Index32 frameIndex = bundleAdjustmentKeyFrameIndices_[nKeyFrame];
				const HomogenousMatrix4& flippedCamera_T_world = optimizedFlippedCameras_T_world[nKeyFrame];

				Scalar minError = Numeric::maxValue();
				Scalar averageError = Numeric::maxValue();
				Scalar maxError = Numeric::maxValue();
				const size_t observations = LocalizedObjectPoint::determineCameraPoseQualityIF(camera, flippedCamera_T_world, frameIndex, localizedObjectPointMap_, minError, averageError, maxError);

				Log::info() << "    Background: Bundle adjustment result for frame index " << frameIndex << ": Observations: " << observations << ", errors: " << minError << ", " << averageError << ", " << maxError;
			}
		}
	}
}

void TrackerMono::localizeUnlocalizedObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex)
{
	ocean_assert(camera.isValid());

	SharedCameraPose currentCameraPose;
	if (!cameraPoses_.hasPose(currentFrameIndex, currentCameraPose))
	{
		// we do not have valid camera pose for the provided current frame, so there is no way that we can determine the locations of unlocalized object points
		return;
	}

	ocean_assert(currentCameraPose);
	if (currentCameraPose->estimatedMotion() != CameraPose::EM_TRANSLATIONAL)
	{
		// the user is currently not moving the device (estimation is based on optical flow, so we don't need to try determining a 3D location)
		return;
	}

	// first, we determine the ids of all object points which can be localized

	constexpr size_t minimalNumberObservations = 10; // TODO tweak threshold

	Indices32 objectPointIds; // TOOD re-usable
	objectPointIds.reserve(32);

	{
		const ReadLock readLock(mutex_, "TrackerMono::localizeUnlocalizedObjectPoints(), gather");

		for (const PointTrackMap::value_type& pointPair : pointTrackMap_)
		{
			const Index32& objectPointId = pointPair.first;

			// first let's ensure the point is not localized yet

			if (localizedObjectPointMap_.contains(objectPointId))
			{
				continue;
			}

			const PointTrack& pointTrack = pointPair.second;

			ocean_assert(pointTrack.lastFrameIndex() >= currentFrameIndex);

			const size_t numberObservations = pointTrack.numberObservationsUntil(currentFrameIndex);

			if (numberObservations < minimalNumberObservations)
			{
				continue;
			}

			objectPointIds.push_back(objectPointId);
		}
	}

	if (objectPointIds.empty())
	{
		return;
	}

	constexpr size_t maximalNumberInvalidObservations = 2;

	Vectors2 imagePoints; // TODO re-use
	HomogenousMatrices4 world_T_cameras;
	HomogenousMatrices4 flippedCameras_T_world;
	Indices32 validIndices;
	Indices32 keyFrames;

	imagePoints.reserve(16);
	world_T_cameras.reserve(16);
	keyFrames.reserve(16);

	Indices32 newLocalizedObjectPointIds; // TODO re-use
	LocalizedObjectPoint::LocalizationPrecisions newLocalizedObjectPointPrecisions;
	Vectors3 newLocalizedObjectPointPositions;

	newLocalizedObjectPointIds.reserve(64);
	newLocalizedObjectPointPrecisions.reserve(64);
	newLocalizedObjectPointPositions.reserve(64);

	for (const Index32 objectPointId : objectPointIds)
	{
		ReadLock readLock(mutex_, "TrackerMono::localizeUnlocalizedObjectPoints(), handle object point");

			ocean_assert(!localizedObjectPointMap_.contains(objectPointId));
			if (localizedObjectPointMap_.contains(objectPointId))
			{
				// TODO this should never happen, remove once verified

				continue;
			}

			const PointTrackMap::const_iterator iPoint = pointTrackMap_.find(objectPointId);

			if (iPoint == pointTrackMap_.cend())
			{
				// the track has been removed in the meantime
				continue;
			}

			const PointTrack& pointTrack = iPoint->second;

			ocean_assert(pointTrack.lastFrameIndex() >= currentFrameIndex);

			const size_t numberObservations = pointTrack.numberObservationsUntil(currentFrameIndex);

			if (numberObservations <= minimalNumberObservations)
			{
				continue;
			}

			Box3 boundingBox;

			imagePoints.clear();
			world_T_cameras.clear();
			flippedCameras_T_world.clear();
			keyFrames.clear();

			ocean_assert(pointTrack.firstFrameIndex() + Index32(numberObservations) - 1u == currentFrameIndex);

			constexpr size_t maximalNumberObservations = 100; // we skip earlier observations

			Index32 firstFrameIndex = pointTrack.firstFrameIndex();

			if (numberObservations > 100)
			{
				firstFrameIndex += Index32(numberObservations - maximalNumberObservations);
				ocean_assert(firstFrameIndex + Index32(maximalNumberObservations) - 1u == currentFrameIndex);
			}

			for (Index32 frameIndex = firstFrameIndex; frameIndex <= currentFrameIndex; ++frameIndex)
			{
				SharedCameraPose previousPose;
				if (!cameraPoses_.hasPose(frameIndex, previousPose))
				{
					continue;
				}

				// TODO, should we skip poses with too old map version?

				ocean_assert(previousPose != nullptr);
				const HomogenousMatrix4& world_T_camera = previousPose->world_T_camera();

				boundingBox += world_T_camera.translation();

				imagePoints.push_back(pointTrack.observation(frameIndex));

				world_T_cameras.push_back(world_T_camera);
				flippedCameras_T_world.push_back(previousPose->flippedCamera_T_world());

				keyFrames.push_back(frameIndex);
			}

		readLock.unlock();

		if (imagePoints.size() < minimalNumberObservations)
		{
			continue;
		}

		// let's make a quick check whether the object point can have enough precision

		constexpr Scalar minDiagonal = Scalar(0.001); // TODO, remove threshold due to missing metric dimension?

		const Scalar boundingSqrSize = boundingBox.sqrDiagonal();

		if (boundingSqrSize < minDiagonal * minDiagonal)
		{
			const SharedCameraPose firstCameraPose = cameraPoses_.pose(keyFrames.front());
			const SharedCameraPose lastCameraPose = cameraPoses_.pose(keyFrames.back());

			ocean_assert(firstCameraPose && lastCameraPose);

			const HomogenousMatrix4& world_T_firstCamera = firstCameraPose->world_T_camera();
			const HomogenousMatrix4& world_T_lastCamera = lastCameraPose->world_T_camera();

			const Vector3 firstViewingDirection = world_T_firstCamera.rotation() * camera.vector(imagePoints.front());
			const Vector3 lastViewingDirection = world_T_lastCamera.rotation() * camera.vector(imagePoints.back());

			const Scalar angle = firstViewingDirection.angle(lastViewingDirection);

			if (angle < Numeric::deg2rad(Scalar(1.5))) // TODO tweak threshold
			{
				continue;
			}
		}

		Vector3 objectPoint;

		validIndices.clear();
		Scalar finalError = Numeric::maxValue();
		if (!Geometry::RANSAC::objectPoint(camera, ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), ConstArrayAccessor<Vector2>(imagePoints), randomGeneratorBackground_, objectPoint, 20u, Scalar(3 * 3), 2u, true, Geometry::Estimator::ET_HUBER, &finalError, &validIndices))
		{
			continue;
		}

		if (validIndices.size() + maximalNumberInvalidObservations < imagePoints.size())
		{
			if constexpr (loggingEnabled_)
			{
				Log::debug() << "We skipped a 3D object point as it was not precise enough: " << validIndices.size() << " of " << imagePoints.size();
			}

			continue;
		}

		const LocalizedObjectPoint::LocalizationPrecision precision = LocalizedObjectPoint::determineLocalizedObjectPointUncertaintyIF(camera, flippedCameras_T_world, objectPoint);
		ocean_assert(precision != LocalizedObjectPoint::LP_INVALID);

		newLocalizedObjectPointIds.push_back(objectPointId);
		newLocalizedObjectPointPrecisions.push_back(precision);
		newLocalizedObjectPointPositions.push_back(objectPoint);
	}

	{
		const WriteLock writeLock(mutex_, "TrackerMono::localizeUnlocalizedObjectPoints()");

		if (taskDetermineInitialObjectPoints_)
		{
			resetLocalizedObjectPoints();
			return;
		}

		const bool trackerIsTracking = trackerState_ == TS_TRACKING;

		for (size_t nObjectPoint = 0; nObjectPoint < newLocalizedObjectPointIds.size(); ++nObjectPoint)
		{
			const Index32& objectPointId = newLocalizedObjectPointIds[nObjectPoint];

			const PointTrackMap::const_iterator iPointTrack = pointTrackMap_.find(objectPointId);

			if (iPointTrack == pointTrackMap_.cend())
			{
				// frame-to-frame tracking has failed in the meantime, so we don't want to do anything
				continue;
			}

			const LocalizedObjectPoint::LocalizationPrecision& precision = newLocalizedObjectPointPrecisions[nObjectPoint];
			const Vector3& position = newLocalizedObjectPointPositions[nObjectPoint];

			ocean_assert(!localizedObjectPointMap_.contains(objectPointId));
			localizedObjectPointMap_.emplace(objectPointId, LocalizedObjectPoint(iPointTrack->second, position, precision, false /*isBundleAdjusted*/));

			if (trackerIsTracking)
			{
				// we are not initializing anymore, so point tracks will be removed

				pointTrackMap_.erase(objectPointId);
			}
		}

		if constexpr (loggingEnabled_)
		{
			if (!newLocalizedObjectPointIds.empty())
			{
				Log::info() << "Converted " << newLocalizedObjectPointIds.size() << " / " << objectPointIds.size() << " unlocalized object points to localized object points";
			}
		}
	}
}

bool TrackerMono::relocalize(const AnyCamera& camera, const Index32 latestFrameIndex, const CV::FramePyramid& yFramePyramid)
{
	ocean_assert(camera.isValid());
	ocean_assert(yFramePyramid.isValid());
	ocean_assert(camera.width() == yFramePyramid.finestWidth() && camera.height() == yFramePyramid.finestHeight());

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.relocalize_);

	ReadLock readLock(mutex_, "TrackerMono::relocalize()");

		const Index32 mapVersion = mapVersion_;

		ocean_assert(trackerState_ == TS_TRACKING);

		Vectors2 imagePoints; // TODO make re-usable
		Indices32 unlocalizedObjectPointIds;
		Indices32 localizedObjectPointIds;
		std::vector<CV::Detector::FREAKDescriptors32> localizedObjectPointDescriptors;
		Vectors3 localizedObjectPointPositions;
		Indices32 firstObservationFrameIndexUnlocalizedObjectPoints;
		Indices32 lastObservationFrameIndexLocalizedObjectPoints;

		imagePoints.reserve(128);
		unlocalizedObjectPointIds.reserve(128);
		localizedObjectPointIds.reserve(128);
		localizedObjectPointDescriptors.reserve(128);
		localizedObjectPointPositions.reserve(128);
		firstObservationFrameIndexUnlocalizedObjectPoints.reserve(128);
		lastObservationFrameIndexLocalizedObjectPoints.reserve(128);

		for (const PointTrackMap::value_type& pointPair : pointTrackMap_)
		{
			const Index32& objectPointId = pointPair.first;
			const PointTrack& pointTrack = pointPair.second;

			Vector2 imagePoint;
			if (pointTrack.hasObservation(latestFrameIndex, &imagePoint))
			{
				unlocalizedObjectPointIds.push_back(objectPointId);
				imagePoints.emplace_back(imagePoint);

				firstObservationFrameIndexUnlocalizedObjectPoints.push_back(pointTrack.firstFrameIndex());
			}
		}

		constexpr size_t minimalNumberImagePoints = 10; // TODO tweak threshold

		if (imagePoints.size() < minimalNumberImagePoints)
		{
			return false;
		}

		// let's gather all localized 3D object points with descriptors

		// TODO, reduce search space

		constexpr size_t minimalNumberObjectPoints = 40;

		for (const LocalizedObjectPointMap::value_type& pointPair : localizedObjectPointMap_)
		{
			const Index32& objectPointId = pointPair.first;
			const LocalizedObjectPoint& localizedObjectPoint = pointPair.second;

			if (localizedObjectPoint.descriptors().empty())
			{
				// the object point does not have any descriptor, so we can not match it

				continue;
			}

			localizedObjectPointIds.push_back(objectPointId);
			localizedObjectPointDescriptors.emplace_back(localizedObjectPoint.descriptors());
			localizedObjectPointPositions.emplace_back(localizedObjectPoint.position());
			lastObservationFrameIndexLocalizedObjectPoints.push_back(localizedObjectPoint.lastObservationFrameIndex());
		}

		if (localizedObjectPointPositions.size() < minimalNumberObjectPoints)
		{
			ocean_assert(false && "This should never happen");
			return false;
		}

	readLock.unlock();

	CV::Detector::FREAKDescriptors32 freakDescriptors(imagePoints.size());
	CV::Detector::FREAKDescriptor32::computeDescriptors(camera.clone(), yFramePyramid, imagePoints.data(), imagePoints.size(), 0u /*pyramidLevel*/, freakDescriptors.data());

	// now, let's remove all descriptors which are invalid

	ocean_assert(imagePoints.size() == unlocalizedObjectPointIds.size());
	ocean_assert(imagePoints.size() == freakDescriptors.size());

	for (size_t nImagePoint = 0; nImagePoint < imagePoints.size(); /*noop*/)
	{
		if (freakDescriptors[nImagePoint].isValid())
		{
			++nImagePoint;
		}
		else
		{
			imagePoints[nImagePoint] = imagePoints.back();
			unlocalizedObjectPointIds[nImagePoint] = unlocalizedObjectPointIds.back();
			freakDescriptors[nImagePoint] = freakDescriptors.back();
			firstObservationFrameIndexUnlocalizedObjectPoints[nImagePoint] = firstObservationFrameIndexUnlocalizedObjectPoints.back();

			imagePoints.pop_back();
			unlocalizedObjectPointIds.pop_back();
			freakDescriptors.pop_back();
			firstObservationFrameIndexUnlocalizedObjectPoints.pop_back();
		}
	}

	ocean_assert(imagePoints.size() == unlocalizedObjectPointIds.size());
	ocean_assert(imagePoints.size() == freakDescriptors.size());
	ocean_assert(imagePoints.size() == firstObservationFrameIndexUnlocalizedObjectPoints.size());

	constexpr size_t minimalNumberCorrespondences = 20;

	if (imagePoints.size() < minimalNumberCorrespondences)
	{
		return false;
	}

	HomogenousMatrix4 world_T_camera;

	Geometry::GravityConstraints gravityConstraints;

	Vector3 cameraGravity(0, 0, 0);
	if (gravities_.hasGravity(latestFrameIndex, &cameraGravity))
	{
		gravityConstraints = Geometry::GravityConstraints(cameraGravity, configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_);
	}

	constexpr unsigned int maximalDescriptorDistance = descriptorThreshold();

	const Scalar maximalProjectionError = configuration_.maximalProjectionError_;

	constexpr Scalar inlierRate = Scalar(0.15);

	Indices32 usedObjectPointIndices;
	Indices32 usedImagePointIndices;

	if constexpr (loggingEnabled_)
	{
		Log::info() << "Trying re-localization with " << imagePoints.size() << " unlocalized object points and " << localizedObjectPointIds.size() << " localized object points";
	}

	if (!Tracking::PoseEstimationT::determinePoseBruteForce<CV::Detector::FREAKDescriptor32, CV::Detector::FREAKDescriptors32, unsigned int, CV::Detector::FREAKDescriptor32::calculateDistance>(camera, localizedObjectPointPositions.data(), localizedObjectPointDescriptors.data(), localizedObjectPointPositions.size(), imagePoints.data(), freakDescriptors.data(), imagePoints.size(), randomGeneratorBackground_, world_T_camera, minimalNumberCorrespondences, maximalDescriptorDistance, maximalProjectionError, inlierRate, &usedObjectPointIndices, &usedImagePointIndices, HomogenousMatrix4(false), nullptr, gravityConstraints.conditionalPointer()))
	{
		return false;
	}

	if constexpr (loggingEnabled_)
	{
		Log::info() << "Re-localization succeeded with " << usedImagePointIndices.size();
	}

	ocean_assert(usedObjectPointIndices.size() == usedImagePointIndices.size());

	// let's remove all corresponding which have an time-wise overlapping observation (e.g., localized object point was still visible while the unlocalized object point was already tracked - cannot be a valid match) // TODO add logic checking whether both tracks are pixel-wise close to eachother and merge them, TODO ADD similar rejection to LOGIC TO FEATURE MATCHING

	for (size_t nCorrespondence = 0; nCorrespondence < usedObjectPointIndices.size(); /*noop*/)
	{
		const Index32& usedObjectPointIndex = usedObjectPointIndices[nCorrespondence];
		const Index32& usedImagePointIndex = usedImagePointIndices[nCorrespondence];

		ocean_assert(usedImagePointIndex < firstObservationFrameIndexUnlocalizedObjectPoints.size());
		ocean_assert(usedObjectPointIndex < lastObservationFrameIndexLocalizedObjectPoints.size());

		if (lastObservationFrameIndexLocalizedObjectPoints[usedObjectPointIndex] >= firstObservationFrameIndexUnlocalizedObjectPoints[usedImagePointIndex])
		{
			// both object points were visible in the same frame, so they cannot match

			usedObjectPointIndices[nCorrespondence] = usedObjectPointIndices.back();
			usedImagePointIndices[nCorrespondence] = usedImagePointIndices.back();

			usedObjectPointIndices.pop_back();
			usedImagePointIndices.pop_back();
		}
		else
		{
			++nCorrespondence;
		}
	}

	ocean_assert(usedObjectPointIndices.size() == usedImagePointIndices.size());

	if (usedImagePointIndices.size() < minimalNumberCorrespondences)
	{
		return false;
	}

	ocean_assert(usedObjectPointIndices.size() >= minimalNumberCorrespondences);

	ocean_assert(usedObjectPointIndices.size() == usedImagePointIndices.size());

	WriteLock writeLock(mutex_, "TrackerMono::relocalize()");

		PoseQualityCalculator poseQualityCalculator;

		for (size_t nCorrespondence = 0; nCorrespondence < usedObjectPointIndices.size(); ++nCorrespondence)
		{
			const Index32& usedObjectPointIndex = usedObjectPointIndices[nCorrespondence];
			const Index32& usedImagePointIndex = usedImagePointIndices[nCorrespondence];

			const Index32& localizedObjectPointId = localizedObjectPointIds[usedObjectPointIndex];
			const Index32& unlocalizedObjectPointId = unlocalizedObjectPointIds[usedImagePointIndex];

			const PointTrackMap::iterator iPointTrack = pointTrackMap_.find(unlocalizedObjectPointId);

			if (iPointTrack == pointTrackMap_.cend())
			{
				// the unlocalized object point does not exist, e.g., frame-to-frame tracking has failed in the meantime
				continue;
			}

			const LocalizedObjectPointMap::iterator iLocalized = localizedObjectPointMap_.find(localizedObjectPointId);

			ocean_assert(iLocalized != localizedObjectPointMap_.cend()); // should always exist
			if (iLocalized == localizedObjectPointMap_.cend())
			{
				continue;
			}

			const PointTrack& pointTrack = iPointTrack->second;
			LocalizedObjectPoint& localizedObjectPoint = iLocalized->second;

			localizedObjectPoint.addObservations(pointTrack);

			poseQualityCalculator.addObjectPoint(localizedObjectPoint.localizationPrecision());

			pointTrackMap_.erase(iPointTrack);
		}

	writeLock.unlock();

	const CameraPose::PoseQuality poseQuality = poseQualityCalculator.poseQuality();

	// we may have lost too many features in frame-to-frame tracking, so that the pose quality is not valid (anymore)
	if (poseQuality == CameraPose::PQ_INVALID)
	{
		return false;
	}

	cameraPoses_.setPose(latestFrameIndex, std::make_shared<CameraPose>(world_T_camera, poseQuality), mapVersion);

	writeLock = WriteLock(mutex_, "TrackerMono::relocalize(), data preparation");

		determineCameraPoses<false>(camera, latestFrameIndex, Index32(-1), true /*stopAtValidPose*/);

		// finally, we need to prepare the frame-to-frame tracking data for the main thread

		const Index32 currentFrameIndex = cameraPoses_.frameIndex();

		if (!cameraPoses_.hasPose(currentFrameIndex))
		{
			determineCameraPoses<false>(camera, cameraPoses_.lastValidPoseFrameIndex(), Index32(-1), true /*stopAtValidPose*/);
		}

	return true;
}

template <bool tUseReadLock>
void TrackerMono::determineCameraPoses(const AnyCamera& camera, const Index32 startFrameIndex, const Index32 skipFrameIndex, const bool stopAtValidPose)
{
	ocean_assert(camera.isValid());
	ocean_assert(startFrameIndex != Index32(-1));

	LocalizedObjectPoint::CorrespondenceData correspondenceData; // TODO make re-usable

	constexpr Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_HUBER;

	const std::optional<ReadLock> readLock = tUseReadLock ? ReadLock(mutex_, "TrackerMono::determineCameraPoses()") : std::optional<ReadLock>();

#ifdef OCEAN_DEBUG
	ocean_assert(WriteLock::debugIsLocked(mutex_));
#endif

	if (localizedObjectPointMap_.empty())
	{
		// the feature map has been reset in the meantime
		return;
	}

	// first the backward pass

	for (Index32 frameIndex = startFrameIndex - 1u; frameIndex < startFrameIndex; --frameIndex)
	{
		if (frameIndex == skipFrameIndex)
		{
			// the first stereoscopic frame can be ignored
			continue;
		}

		ocean_assert(frameIndex != skipFrameIndex && frameIndex != startFrameIndex);

		if (cameraPoses_.hasPose(frameIndex))
		{
			if (stopAtValidPose)
			{
				break;
			}
		}

		Geometry::GravityConstraints gravityConstraints;

		Vector3 cameraGravity(0, 0, 0);
		if (gravities_.hasGravity(frameIndex, &cameraGravity))
		{
			gravityConstraints = Geometry::GravityConstraints(cameraGravity, configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_);
		}

		correspondenceData.reset();

		SharedCameraPose cameraPose = LocalizedObjectPoint::determineCameraPose(camera, cameraPoses_, frameIndex, localizedObjectPointMap_, randomGeneratorBackground_, estimatorType, correspondenceData, gravityConstraints.isValid() ? &gravityConstraints : nullptr);

		if (cameraPose)
		{
			cameraPoses_.setPose(frameIndex, std::move(cameraPose), mapVersion_);
		}
	}

	// now the forward pass

	for (Index32 frameIndex = startFrameIndex + 1u; true; ++frameIndex)
	{
		if (frameIndex == skipFrameIndex)
		{
			// the first stereoscopic frame can be ignore
			continue;
		}

		ocean_assert(frameIndex != skipFrameIndex && frameIndex != startFrameIndex);

		if (cameraPoses_.frameIndex() < frameIndex)
		{
			break;
		}

		if (cameraPoses_.hasPose(frameIndex))
		{
			if (stopAtValidPose)
			{
				break;
			}
		}

		Geometry::GravityConstraints gravityConstraints;

		Vector3 cameraGravity(0, 0, 0);
		if (gravities_.hasGravity(frameIndex, &cameraGravity))
		{
			gravityConstraints = Geometry::GravityConstraints(cameraGravity, configuration_.worldGravity_, Scalar(configuration_.gravityWeightFactor_), configuration_.gravityMaximalAngle_);
		}

		correspondenceData.reset();

		SharedCameraPose cameraPose = LocalizedObjectPoint::determineCameraPose(camera, cameraPoses_, frameIndex, localizedObjectPointMap_, randomGeneratorBackground_, estimatorType, correspondenceData, gravityConstraints.isValid() ? &gravityConstraints : nullptr);

		if (cameraPose)
		{
			cameraPoses_.setPose(frameIndex, std::move(cameraPose), mapVersion_);
		}
		else
		{
			Log::debug() << "Failed to determine initial camera pose for frame " << frameIndex;
		}
	}
}

void TrackerMono::processTrackingResults(const Index32 currentFrameIndex, const TrackingCorrespondences& trackingCorrespondences)
{
	const Vectors2& previousImagePoints = trackingCorrespondences.previousImagePoints();
	const Vectors2& currentImagePoints = trackingCorrespondences.currentImagePoints();
	const Indices32& pointIds = trackingCorrespondences.pointIds();
	const TrackingCorrespondences::ValidCorrespondences& validCorrespondences = trackingCorrespondences.validCorrespondences();

	ocean_assert(previousImagePoints.size() == currentImagePoints.size());
	ocean_assert(previousImagePoints.size() == pointIds.size());
	ocean_assert(previousImagePoints.size() == validCorrespondences.size());

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.trackImagePointsDatabase_);

	const WriteLock writeLock(mutex_, "TrackerMono::trackImagePoints()");

	for (size_t nCorrespondence = 0; nCorrespondence < previousImagePoints.size(); ++nCorrespondence)
	{
		const Vector2& previousImagePoint = previousImagePoints[nCorrespondence];
		const Vector2& currentImagePoint = currentImagePoints[nCorrespondence];
		const Index32 objectPointId = pointIds[nCorrespondence];
		const bool isValid = validCorrespondences[nCorrespondence] == uint8_t(1);

		if (isValid)
		{
			// the point correspondences were tracked successfully, so we add the point to the occpancy array

			occupancyArray_.addPoint(currentImagePoint);
		}

		{
			// now, let's extend the point track with the new observation; if tracking failed we remove the point track

			PointTrackMap::iterator iPointTrack = pointTrackMap_.find(objectPointId);

			if (iPointTrack != pointTrackMap_.cend())
			{
				if (isValid)
				{
					PointTrack& pointTrack = iPointTrack->second;

					ocean_assert(pointTrack.lastFrameIndex() == currentFrameIndex - 1u);
					ocean_assert_and_suppress_unused(pointTrack.lastImagePoint().sqrDistance(previousImagePoint) <= Numeric::sqr(5), previousImagePoint); // due to re-localization a previous point may be slightly off

					pointTrack.addObservation(currentFrameIndex, currentImagePoint);
				}
				else
				{
					pointTrackMap_.erase(iPointTrack);
				}
			}
		}

		{
			// now, let's extend the localized object point with the new observation; if tracking failed we might remove the point

			LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint != localizedObjectPointMap_.cend())
			{
				LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

				if (isValid)
				{
#ifdef OCEAN_DEBUG
					if (localizedObjectPoint.lastObservationFrameIndex() + 1u == currentFrameIndex)
					{
						ocean_assert_and_suppress_unused(localizedObjectPoint.lastObservation().imagePoint() == previousImagePoint, previousImagePoint);
					}
#endif // OCEAN_DEBUG

					localizedObjectPoint.addObservation(currentFrameIndex, currentImagePoint);
				}
				else
				{
					if (localizedObjectPoint.localizationPrecision() < LocalizedObjectPoint::LP_LOW)
					{
						// the point is not precise enough - we will never use this point again, so we can remove it

						Log::debug() << "Removed low precision object points for which frame-to-frame tracking was lost: " << iObjectPoint->first;

						localizedObjectPointMap_.erase(iObjectPoint);
					}
				}
			}
		}
	}
}

bool TrackerMono::detectNewImagePoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CV::FramePyramid& yFramePyramid, const bool tryMatchCornersToLocalizedObjectPoints)
{
	ocean_assert(camera.isValid());
	ocean_assert(yFramePyramid.isValid());
	ocean_assert(camera.width() == yFramePyramid.finestWidth() && camera.height() == yFramePyramid.finestHeight());
	ocean_assert(yFramePyramid.finestLayer().isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.detectNewImagePoints_);

	ocean_assert(occupancyArray_.isValid()); // no read lock necessary

	if (!occupancyArray_.needMorePoints())
	{
		// most of the bins are containing feature points, so we don't need to add new features

		return true;
	}

	// TODO detect points in a sub-region of the image only (empty bins)

	const Frame& yFrame = yFramePyramid.finestLayer();

	ocean_assert(configuration_.harrisThresholdMin_ <= harrisThreshold_ && harrisThreshold_ <= configuration_.harrisThresholdMax_);

	CV::Detector::HarrisCorners corners;
	if (!CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), harrisThreshold_, false /*frameIsUndistorted*/, corners, true /*determineExactPosition*/))
	{
		return false;
	}

	if (!corners.empty())
	{
		if (tryMatchCornersToLocalizedObjectPoints)
		{
			// first, let's check whether any of the detected corners can be matched to an existing localized object point (which is currently not visible)

			SharedCameraPose cameraPose;
			if (cameraPoses_.hasPose(currentFrameIndex, cameraPose))
			{
				ocean_assert(cameraPose);

				matchCornersToLocalizedObjectPoints(camera, currentFrameIndex, *cameraPose, yFramePyramid, corners);
			}
		}

		// now, we sort all corners by strength to ensure that we add the strongest corners first

		std::sort(corners.begin(), corners.end());
		ocean_assert(corners.empty() || corners.front().strength() >= corners.back().strength());

		// in case a corner is located in an empty bin, we add the corner as a new unlocalized object point

		const WriteLock writeLock(mutex_, "TrackerMono::detectNewImagePoints()");

		size_t newImagePointsCounter = 0;

		for (const CV::Detector::HarrisCorner& corner : corners)
		{
			if (occupancyArray_.addPointIfEmpty(corner.observation()))
			{
				const Index32 newUnlocalizedObjectPointId = uniqueObjectPointId();

				pointTrackMap_.emplace(newUnlocalizedObjectPointId, PointTrack(currentFrameIndex, corner.observation()));

				++newImagePointsCounter;
			}
		}

		if constexpr (loggingEnabled_)
		{
			if (newImagePointsCounter != 0)
			{
				Log::info() << "Added " << newImagePointsCounter << " new image points";
			}
		}
	}

	const size_t coveragePercent = size_t(occupancyArray_.coverage() * 100.0f + 0.5f);

	if (coveragePercent < 40) // target is 40%
	{
		if (harrisThreshold_ > configuration_.harrisThresholdMin_)
		{
			--harrisThreshold_;

			if constexpr (loggingEnabled_)
			{
				Log::info() << "Harris threshold decreased: " << harrisThreshold_;
			}
		}
	}
	else
	{
		if (harrisThreshold_ < configuration_.harrisThresholdMax_)
		{
			++harrisThreshold_;

			if constexpr (loggingEnabled_)
			{
				Log::info() << "Harris threshold increased: " << harrisThreshold_;
			}
		}
	}

	return true;
}

void TrackerMono::matchCornersToLocalizedObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CameraPose& cameraPose, const CV::FramePyramid& yFramePyramid, CV::Detector::HarrisCorners& corners)
{
	ocean_assert(!corners.empty());
	ocean_assert(cameraPose.isValid());
	ocean_assert(yFramePyramid.finestWidth() == camera.width() && yFramePyramid.finestHeight() == camera.height());
	ocean_assert(yFramePyramid.finestLayer().isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.matchCornersToLocalizedObjectPoints_);

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose.flippedCamera_T_world();

	// first let's gather all corners which are close to projected localized object points (which are currently not visible)

	using CornerIndexToObjectPointIdsMap = std::unordered_map<Index32, Indices32>;
	CornerIndexToObjectPointIdsMap cornerIndexToObjectPointsMap;

	const Scalar maximalProjectionError = configuration_.maximalProjectionError_;

	ReadLock readLock(mutex_, "TrackerMono::matchCornersToLocalizedObjectPoints(), pre description");

		for (LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap_)
		{
			const Index32& objectPointId = objectPointPair.first;
			LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

			if (localizedObjectPoint.localizationPrecision() < LocalizedObjectPoint::LP_LOW)
			{
				// not even a low precision, no reason to try matching the object point
				continue;
			}

			ocean_assert(localizedObjectPoint.lastObservationFrameIndex() <= currentFrameIndex);
			if (localizedObjectPoint.lastObservationFrameIndex() == currentFrameIndex)
			{
				// the localized object point is already/still visible
				continue;
			}

			if (localizedObjectPoint.descriptors().empty())
			{
				// the localized object point has not been described yet
				continue;
			}

			ocean_assert(localizedObjectPoint.position() != Vector3::minValue());

			if (Camera::isObjectPointInFrontIF(flippedCamera_T_world, localizedObjectPoint.position()))
			{
				const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, localizedObjectPoint.position());

				for (size_t cornerIndex = 0; cornerIndex < corners.size(); ++cornerIndex)
				{
					const CV::Detector::HarrisCorner& corner = corners[cornerIndex];

					if (projectedObjectPoint.sqrDistance(corner.observation()) < Numeric::sqr(maximalProjectionError))
					{
						// TODO add check whether 3D point can actually be visible (normal of point)

						cornerIndexToObjectPointsMap[Index32(cornerIndex)].push_back(objectPointId);
					}
				}
			}
		}

	readLock.unlock();

	if (cornerIndexToObjectPointsMap.empty())
	{
		// none of the new corners are close enough to any object point
		return;
	}

	// let's describe all corners with corresponding object point candidates so that we can match them afterwards

	using MatchedObjectPointIdToCornerIndexMap = std::unordered_map<Index32, Index32>;
	MatchedObjectPointIdToCornerIndexMap matchedObjectPointIdToCornerIndexMap; // TODO make reusable
	Indices32 matchedCornerIndices;
	Vectors2 imagePoints;

	for (const CornerIndexToObjectPointIdsMap::value_type& pair : cornerIndexToObjectPointsMap)
	{
		const Index32& cornerIndex = pair.first;
		const CV::Detector::HarrisCorner& corner = corners[cornerIndex];

		imagePoints.emplace_back(corner.observation());
	}

	CV::Detector::FREAKDescriptors32 freakDescriptors(imagePoints.size());
	CV::Detector::FREAKDescriptor32::computeDescriptors(camera.clone(), yFramePyramid, imagePoints.data(), imagePoints.size(), 0u /*pyramidLevel*/, freakDescriptors.data());

	readLock = ReadLock(mutex_, "TrackerMono::matchCornersToLocalizedObjectPoints() post description");

		// now, let's try to find valid matches between the existing object points and the described points

		size_t nIndex = 0;
		for (const CornerIndexToObjectPointIdsMap::value_type& pair : cornerIndexToObjectPointsMap)
		{
			const CV::Detector::FREAKDescriptor32& freakDescriptor = freakDescriptors[nIndex];
			++nIndex;

			if (!freakDescriptor.isValid())
			{
				continue;
			}

			const Index32& cornerIndex = pair.first;
			const Indices32& localizedObjectPointIds = pair.second;
			ocean_assert(!localizedObjectPointIds.empty());

			unsigned int bestDistance = (unsigned int)(-1);
			Index32 bestLocalizedObjectPointId = Index32(-1);

			for (const Index32& localizedObjectPointId : localizedObjectPointIds)
			{
				LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(localizedObjectPointId);

				ocean_assert(iObjectPoint != localizedObjectPointMap_.cend());
				if (iObjectPoint != localizedObjectPointMap_.cend())
				{
					const LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;
					ocean_assert(localizedObjectPoint.descriptors().size() >= 1);

					for (const CV::Detector::FREAKDescriptor32& objectPointDescriptor : localizedObjectPoint.descriptors())
					{
						const unsigned int distance = freakDescriptor.distance(objectPointDescriptor);

						if (distance < bestDistance)
						{
							bestDistance = distance;
							bestLocalizedObjectPointId = localizedObjectPointId;
						}
					}
				}
			}

			if (bestDistance < descriptorThreshold())
			{
				ocean_assert(bestLocalizedObjectPointId != Index32(-1));

				if (!matchedObjectPointIdToCornerIndexMap.emplace(bestLocalizedObjectPointId, cornerIndex).second)
				{
					Log::debug() << "Object point " << bestLocalizedObjectPointId << " already matched to new image point";
				}
			}
		}

	readLock.unlock();

	ocean_assert(nIndex == cornerIndexToObjectPointsMap.size());

	WriteLock writeLock(mutex_, "TrackerMono::matchCornersToLocalizedObjectPoints()");

		for (const MatchedObjectPointIdToCornerIndexMap::value_type& matchedPair : matchedObjectPointIdToCornerIndexMap)
		{
			const Index32& objectPointId = matchedPair.first;
			const Index32& cornerIndex = matchedPair.second;

			LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint != localizedObjectPointMap_.cend())
			{
				LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

				const Vector2& imagePoint = corners[cornerIndex].observation();

				ocean_assert(localizedObjectPoint.lastObservationFrameIndex() != currentFrameIndex);
				localizedObjectPoint.addObservation(currentFrameIndex, imagePoint);

				matchedCornerIndices.push_back(cornerIndex);
			}
		}

	writeLock.unlock();

	if (!matchedCornerIndices.empty())
	{
		// let's remove all matched corners

		std::sort(matchedCornerIndices.rbegin(), matchedCornerIndices.rend());
		ocean_assert(matchedCornerIndices.size() == 1 || matchedCornerIndices.front() > matchedCornerIndices.back());

		for (const Index32& matchedCornerIndex : matchedCornerIndices)
		{
			corners[matchedCornerIndex] = std::move(corners.back());
			corners.pop_back();
		}
	}

	if constexpr (loggingEnabled_)
	{
		if (!matchedCornerIndices.empty())
		{
			Log::info() << "Re-matched " << matchedCornerIndices.size() << " existing 3D object points";
		}
	}
}

void TrackerMono::matchLocalizedObjectPointsToCorners(const AnyCamera& camera, const Index32 currentFrameIndex, const CameraPose& cameraPose, const CV::FramePyramid& yFramePyramid)
{
	ocean_assert(cameraPose.isValid());
	ocean_assert(yFramePyramid.finestWidth() == camera.width() && yFramePyramid.finestHeight() == camera.height());
	ocean_assert(yFramePyramid.finestLayer().isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	const PerformanceStatistics::ScopedStatistic scopedStatistic(performanceStatistics_.matchLocalizedObjectPointsToCorners_);

	const HomogenousMatrix4& flippedCamera_T_world = cameraPose.flippedCamera_T_world();

	// we determine all untracked 3D object points which could be visible in the current frame

	Indices32 objectPointIds; // TODO re-use
	Vectors2 projectedObjectPoints;

	objectPointIds.reserve(32);
	projectedObjectPoints.reserve(32);

	ReadLock readLock(mutex_, "TrackerMono::matchLocalizedObjectPointsToCorners(), gathering");

		for (LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap_)
		{
			const Index32& objectPointId = objectPointPair.first;
			LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

			if (localizedObjectPoint.localizationPrecision() < LocalizedObjectPoint::LP_LOW)
			{
				// not even a low precision, no reason to try matching the object point
				continue;
			}

			ocean_assert(localizedObjectPoint.lastObservationFrameIndex() <= currentFrameIndex);
			if (localizedObjectPoint.lastObservationFrameIndex() == currentFrameIndex)
			{
				// the localized object point is already/still visible
				continue;
			}

			if (localizedObjectPoint.descriptors().empty())
			{
				// the localized object point has not been described yet
				continue;
			}

			// TODO add additional visibility check (e.g., similiar pose, distance to object, occlusion, time since last observation, etc.)

			ocean_assert(localizedObjectPoint.position() != Vector3::minValue());

			if (Camera::isObjectPointInFrontIF(flippedCamera_T_world, localizedObjectPoint.position()))
			{
				const Vector2 projectedObjectPoint = camera.projectToImageIF(flippedCamera_T_world, localizedObjectPoint.position());

				objectPointIds.push_back(objectPointId);
				projectedObjectPoints.push_back(projectedObjectPoint);
			}
		}

	readLock.unlock();

	ocean_assert(projectedObjectPoints.size() == objectPointIds.size());

	if (objectPointIds.empty())
	{
		return;
	}

	// now, we try to detect features in the direct neighborhood of the projected object points
	// then we try to match the features to the object points

	const Frame& yFrame = yFramePyramid.finestLayer();

	ocean_assert(configuration_.harrisThresholdMin_ <= harrisThreshold_ && harrisThreshold_ <= configuration_.harrisThresholdMax_);

	Vectors2 imagePoints; // TODO re-usable
	imagePoints.reserve(objectPointIds.size() * 4);

	CV::Detector::HarrisCorners subsetCorners;

	IndexPairs32 objectPointIdIndicesPairs; // indices of the first corner of each object point
	objectPointIdIndicesPairs.reserve(objectPointIds.size());

	for (size_t nObjectPoint = 0; nObjectPoint < objectPointIds.size(); ++nObjectPoint)
	{
		subsetCorners.clear();

		const Vector2& projectedObjectPoint = projectedObjectPoints[nObjectPoint];
		const Index32& objectPointId = objectPointIds[nObjectPoint];

		constexpr Scalar regionSize = Scalar(20); // min: 7 + 2 * radius

		const Box2 pointRegion(projectedObjectPoint, regionSize, regionSize);

		unsigned int subFrameLeft;
		unsigned int subFrameTop;
		unsigned int subFrameWidth;
		unsigned int subFrameHeight;
		if (!pointRegion.box2integer(yFrame.width(), yFrame.height(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight) || subFrameWidth < 10u || subFrameHeight < 10u)
		{
			// the overlapping/visible region is too small
			continue;
		}

		if (!CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, harrisThreshold_, false /*frameIsUndistorted*/, subsetCorners, true /*determineExactPosition*/))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		if (subsetCorners.empty())
		{
			continue;
		}

		objectPointIdIndicesPairs.emplace_back(objectPointId, Index32(imagePoints.size()));

		for (const CV::Detector::HarrisCorner& subsetCorner : subsetCorners)
		{
			imagePoints.emplace_back(subsetCorner.observation());
		}
	}

	if (objectPointIdIndicesPairs.empty())
	{
		return;
	}

	CV::Detector::FREAKDescriptors32 freakDescriptors(imagePoints.size());
	CV::Detector::FREAKDescriptor32::computeDescriptors(camera.clone(), yFramePyramid, imagePoints.data(), imagePoints.size(), 0u /*pyramidLevel*/, freakDescriptors.data());

	using MatchedObjectPointIdToImagePointMap = std::unordered_map<Index32, size_t>;
	MatchedObjectPointIdToImagePointMap matchedObjectPointIdToImagePointMap; // TODO make reusable

	readLock = ReadLock(mutex_, "TrackerMono::matchLocalizedObjectPointsToCorners() post description");

		objectPointIdIndicesPairs.emplace_back(Index32(-1), Index32(imagePoints.size())); // adding a final entry to make the processing simpler

		for (size_t nObjectPoint = 0; nObjectPoint < objectPointIdIndicesPairs.size() - 1; ++nObjectPoint)
		{
			const Index32& objectPointId = objectPointIdIndicesPairs[nObjectPoint].first;

			LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint == localizedObjectPointMap_.cend())
			{
				// the object point has been removed in the meantime
				continue;
			}

			const LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

			const CV::Detector::FREAKDescriptors32& objectPointDescriptors = localizedObjectPoint.descriptors();
			ocean_assert(!objectPointDescriptors.empty());

			const Index32& firstCornerIndex = objectPointIdIndicesPairs[nObjectPoint].second;
			const Index32& endCornerIndex = objectPointIdIndicesPairs[nObjectPoint + 1].second;
			ocean_assert(firstCornerIndex < endCornerIndex);

			unsigned int bestDistance = (unsigned int)(-1);
			size_t bestIndex = size_t(-1);

			for (size_t nImagePoint = firstCornerIndex; nImagePoint < endCornerIndex; ++nImagePoint)
			{
				const CV::Detector::FREAKDescriptor32& imagePointDescriptor = freakDescriptors[nImagePoint];

				if (!imagePointDescriptor.isValid())
				{
					continue;
				}

				for (const CV::Detector::FREAKDescriptor32& objectPointDescriptor : objectPointDescriptors)
				{
					const unsigned int distance = imagePointDescriptor.distance(objectPointDescriptor);

					if (distance < bestDistance)
					{
						bestDistance = distance;
						bestIndex = nImagePoint;
					}
				}
			}

			if (bestDistance < descriptorThreshold())
			{
				ocean_assert(bestIndex != size_t(-1));

				if (!matchedObjectPointIdToImagePointMap.emplace(objectPointId, bestIndex).second)
				{
					Log::debug() << "Object point " << objectPointId << " already matched to new image point";
				}
			}
		}

	readLock.unlock();

	WriteLock writeLock(mutex_, "TrackerMono::matchLocalizedObjectPointsToCorners()");

		size_t debugCounter = 0;

		for (const MatchedObjectPointIdToImagePointMap::value_type& matchedPair : matchedObjectPointIdToImagePointMap)
		{
			const Index32& objectPointId = matchedPair.first;
			const size_t& imagePointIndex = matchedPair.second;

			LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

			if (iObjectPoint == localizedObjectPointMap_.cend())
			{
				continue;
			}

			LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;

			const Vector2& imagePoint = imagePoints[imagePointIndex];

			ocean_assert(localizedObjectPoint.lastObservationFrameIndex() != currentFrameIndex);
			localizedObjectPoint.addObservation(currentFrameIndex, imagePoint);

			++debugCounter;

			occupancyArray_.addPoint(imagePoint);
		}

		Log::debug() << "Matched " << debugCounter << " new 3D object points";
		OCEAN_SUPPRESS_UNUSED_WARNING(debugCounter);

	writeLock.unlock();
}

bool TrackerMono::isBundleAdjustmentNeeded(const AnyCamera& camera, const CameraPose& currentCameraPose, const Index32 currentFrameIndex, const Scalar maximalProjectionError, const unsigned int necessaryMapVersion) const
{
#ifdef OCEAN_DEBUG
	ocean_assert(WriteLock::debugIsLocked(mutex_));
#endif

	{
		// first, let's see whether the average projection error exceeds a threshold

		Scalar currentMinError = Numeric::maxValue();
		Scalar currentAverageError = Numeric::maxValue();
		Scalar currentMaxError = Numeric::maxValue();
		LocalizedObjectPoint::determineCameraPoseQualityIF(camera, currentCameraPose.flippedCamera_T_world(), currentFrameIndex, localizedObjectPointMap_, currentMinError, currentAverageError, currentMaxError);

		if (currentAverageError >= maximalProjectionError)
		{
			if constexpr (loggingEnabled_)
			{
				Log::debug() << "    Background: Bundle Adjustment is necessary for frame " << currentFrameIndex << " due to projection error: " << currentAverageError << " > " << maximalProjectionError;
			}

			return true;
		}
	}

	{
		// now, let's see whether the ratio of bundle adjusted object points is too low

		size_t currentBundleAdjusted = 0;
		const size_t currentCorrespondences = LocalizedObjectPoint::determineBundleAdjustmentQuality(currentFrameIndex, localizedObjectPointMap_, currentBundleAdjusted);

		const float ratio = NumericF::ratio(float(currentBundleAdjusted), float(currentCorrespondences), 0.0f);

		if (ratio < 0.5f)
		{
			if constexpr (loggingEnabled_)
			{
				Log::debug() << "    Background: Bundle Adjustment is necessary for frame " << currentFrameIndex << " due to ratio of bundle adjusted object points: " << ratio << " < 0.5";
			}

			return true;
		}
	}

	{
		// let's determine the smallest baseline between the pose of the current frame and all other key-frame poses

		ocean_assert(bundleAdjustmentSqrBaseline_ >= 0);
		ocean_assert(!bundleAdjustmentKeyFrameIndices_.empty());

		const Vector3& currentTranslation = currentCameraPose.world_T_camera().translation();

		Scalar currentSqrBaseline = Numeric::maxValue();

		for (const Index32& bundleAdjustmentKeyFrameIndex : bundleAdjustmentKeyFrameIndices_)
		{
			const SharedCameraPose cameraPose = cameraPoses_.pose(bundleAdjustmentKeyFrameIndex);
			ocean_assert(cameraPose);

			ocean_assert_and_suppress_unused(cameraPose->mapVersion() == necessaryMapVersion, necessaryMapVersion);

			const Vector3& keyFrameTranslation = cameraPose->world_T_camera().translation();

			const Scalar sqrBaseline = currentTranslation.sqrDistance(keyFrameTranslation);

			if (sqrBaseline < currentSqrBaseline)
			{
				currentSqrBaseline = sqrBaseline;
			}
		}

		constexpr Scalar baselineFactor = Scalar(1.225); // TODO tweak threshold

		if (currentSqrBaseline >= bundleAdjustmentSqrBaseline_ * Numeric::sqr(baselineFactor))
		{
			if constexpr (loggingEnabled_)
			{
				Log::debug() << "    Background: Bundle Adjustment is necessary for frame " << currentFrameIndex << " due to baseline: " << Numeric::sqrt(currentSqrBaseline) << " > " << Numeric::sqrt(bundleAdjustmentSqrBaseline_ * Numeric::sqr(baselineFactor));
			}

			return true;
		}
	}

	return false;
}

bool TrackerMono::determineBundleAdjustmentTopology(const Index32 necessaryMapVersion, const CameraPoses& cameraPoses, const LocalizedObjectPointMap& localizedObjectPointMap, const size_t maximalNumberNewKeyFrames, const size_t maximalNumberKeyFrames, Indices32& keyFrameIndices, ObjectPointToObservations& objectPointToObservations, const size_t minimalNumberKeyFrames, const size_t maximalFrameHistory)
{
	// we select a subset of keyframes to be used in the bundle adjustment.
	// the selection strategy tries to maximize the spatial distribution of keyframes while ensuring sufficient feature overlap.

	ocean_assert(cameraPoses.size() >= 2);
	ocean_assert(!localizedObjectPointMap.empty());
	ocean_assert(maximalNumberNewKeyFrames >= 1);
	ocean_assert(minimalNumberKeyFrames >= 2);
	ocean_assert(minimalNumberKeyFrames <= maximalNumberKeyFrames);

	objectPointToObservations.clear();

	Index32 firstKeyFrameIndex = 0u;

	if (keyFrameIndices.size() >= maximalNumberKeyFrames)
	{
		firstKeyFrameIndex = Index32(std::max(0, int32_t(cameraPoses.frameIndex()) - int32_t(maximalFrameHistory) + 1));
	}

	while (keyFrameIndices.size() >= maximalNumberKeyFrames)
	{
		keyFrameIndices.erase(keyFrameIndices.begin());
	}

	constexpr size_t minimalNumberObjectPoints = 20; // TODO tweak threshold

	if (keyFrameIndices.empty())
	{
		const Index32 firstFrameIndex = frameIndexWithMostLocalizedObjectPoints(necessaryMapVersion, cameraPoses, localizedObjectPointMap, minimalNumberObjectPoints);

		if (firstFrameIndex == Index32(-1))
		{
			return false;
		}

		keyFrameIndices.push_back(firstFrameIndex);
	}

	Vectors3 keyFrameTranslations;
	keyFrameTranslations.reserve(keyFrameIndices.size());

	for (const Index32 & keyFrameIndex : keyFrameIndices)
	{
		const SharedCameraPose cameraPose = cameraPoses.pose(keyFrameIndex);
		ocean_assert(cameraPose);

		ocean_assert(cameraPose->mapVersion() == necessaryMapVersion);

		keyFrameTranslations.push_back(cameraPose->world_T_camera().translation());
	}

	using DistancePair = std::pair<Scalar, Index32>;
	using DistancePairs = std::vector<DistancePair>;

	DistancePairs distancePairs;
	distancePairs.reserve(cameraPoses.frameIndex() + 1u);

	for (Index32 frameIndex = firstKeyFrameIndex; frameIndex <= cameraPoses.frameIndex(); ++frameIndex)
	{
		SharedCameraPose cameraPose;
		if (cameraPoses.hasPose(frameIndex, cameraPose))
		{
			if (cameraPose->mapVersion() == necessaryMapVersion)
			{
				const Vector3 poseTranslation = cameraPose->world_T_camera().translation();

				Scalar sqrDistance = Numeric::maxValue();

				for (const Vector3& keyFrameTranslation : keyFrameTranslations)
				{
					sqrDistance = std::min(sqrDistance, poseTranslation.sqrDistance(keyFrameTranslation));
				}

				if (sqrDistance >= Numeric::weakEps())
				{
					distancePairs.emplace_back(sqrDistance, frameIndex);
				}
			}
		}
	}

	if (distancePairs.empty())
	{
		keyFrameIndices.clear();

		return false;
	}

	size_t numberNewKeyFrames = 0;

	while (!distancePairs.empty() && keyFrameIndices.size() < maximalNumberKeyFrames)
	{
		ocean_assert(numberNewKeyFrames < maximalNumberNewKeyFrames);

		// we determine a subset of candidate frames (bracket) that are furthest away from the current keyframe.
		// we only consider the top 10% of frames with the largest distance.
		const size_t bracketSize = std::max(size_t(1), (distancePairs.size() * 10 + 50) / 100); // 10%

		if (bracketSize < distancePairs.size())
		{
			// sort/partition to get the furthest candidates in the beginning of the vector
			std::nth_element(distancePairs.begin(), distancePairs.begin() + bracketSize, distancePairs.end(), [](const DistancePair& a, const DistancePair& b)
			{
				return a.first > b.first;
			});
		}

		// within the bracket of furthest frames, we find the one with the most visible object points
		// simultaneously, we track the absolute furthest frame and its number of object points for a potential heuristic switch
		size_t bestObjectPoints = 0;
		Index32 bestFrameIndex = Index32(-1);
		Scalar bestSqrDistance = Numeric::minValue();

		Scalar largestSqrDistance = Numeric::minValue();
		Index32 largestFrameIndex = Index32(-1);
		size_t largestFrameObjectPoints = 0;

		for (size_t nDistance = 0; nDistance < bracketSize; ++nDistance)
		{
			const DistancePair& distancePair = distancePairs[nDistance];

			if (distancePair.first < Numeric::weakEps())
			{
				continue;
			}

			const Index32 frameIndex = distancePair.second;

			size_t objectPoints = 0;

			for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
			{
				const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

				if (localizedObjectPoint.hasObservation(frameIndex))
				{
					++objectPoints;
				}
			}

			if (objectPoints > bestObjectPoints)
			{
				bestObjectPoints = objectPoints;
				bestFrameIndex = frameIndex;

				bestSqrDistance = distancePair.first;
			}

			if (distancePair.first > largestSqrDistance)
			{
				largestSqrDistance = distancePair.first;
				largestFrameIndex = frameIndex;
				largestFrameObjectPoints = objectPoints;
			}
		}

		if (bestObjectPoints < minimalNumberObjectPoints || bestFrameIndex == Index32(-1))
		{
			break;
		}

		ocean_assert(bestFrameIndex != Index32(-1));

		// heuristic: if the absolute furthest frame is significantly further away (sqrt(2) times) than the 'best' frame (with most points),
		// we prefer the spatial distribution and pick the furthest frame instead, provided it has enough object points.
		if (bestSqrDistance * Numeric::sqr(2) < largestSqrDistance)
		{
			if (largestFrameObjectPoints >= minimalNumberObjectPoints)
			{
				// the largest distance is so much larger that we need to use the largest distance instead

				bestFrameIndex = largestFrameIndex;
			}
		}

		keyFrameIndices.push_back(bestFrameIndex);

		++numberNewKeyFrames;

		if (numberNewKeyFrames >= maximalNumberNewKeyFrames)
		{
			// we have enough key-frames
			break;
		}

		// we update the distances of all remaining candidates relative to the newly selected keyframe.
		// candidates that are too close to the new keyframe are removed.

		const SharedCameraPose bestCameraPose = cameraPoses.pose(bestFrameIndex);
		ocean_assert(bestCameraPose);

		const Vector3& nextPoseTranslation = bestCameraPose->world_T_camera().translation();

		for (size_t nDistance = 0; nDistance < distancePairs.size(); /*noop*/)
		{
			DistancePair& distancePair = distancePairs[nDistance];

			const SharedCameraPose cameraPose = cameraPoses.pose(distancePair.second);
			ocean_assert(cameraPose);

			const Scalar newSqrDistance = cameraPose->world_T_camera().translation().sqrDistance(nextPoseTranslation);

			if (newSqrDistance < Numeric::weakEps())
			{
				distancePairs[nDistance] = distancePairs.back();
				distancePairs.pop_back();
			}
			else
			{
				if (newSqrDistance < distancePair.first)
				{
					distancePair.first = newSqrDistance;
				}

				++nDistance;
			}
		}
	}

	if (keyFrameIndices.size() < minimalNumberKeyFrames)
	{
		keyFrameIndices.clear();

		return false;
	}

	ocean_assert(UnorderedIndexSet32(keyFrameIndices.cbegin(), keyFrameIndices.cend()).size() == keyFrameIndices.size());

	// now, let's extract the topology for the Bundle Adjustment, for each 3D object points, we need the observations (frame index and 2D image point)

	size_t poseIndex = 0;

	for (const Index32& keyFrameIndex : keyFrameIndices)
	{
#ifdef OCEAN_DEBUG
		const HomogenousMatrix4 flippedCamera_T_world = cameraPoses.flippedCamera_T_world(keyFrameIndex);
		ocean_assert(flippedCamera_T_world.isValid());
#endif

		for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
		{
			const Index32& objectPointId = objectPointPair.first;
			const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

			Vector2 imagePoint;
			if (localizedObjectPoint.hasObservation(keyFrameIndex, &imagePoint))
			{
#ifdef OCEAN_DEBUG
				ocean_assert(Camera::isObjectPointInFrontIF(flippedCamera_T_world, localizedObjectPoint.position()));
#endif

				objectPointToObservations[objectPointId].emplace_back(Index32(poseIndex), imagePoint);
			}
		}

		++poseIndex;
	}

	// let's ensure that we remove all 3D object points which are only visible in one key-frame

	size_t singleObservationObjectPoints = 0;

#ifdef OCEAN_DEBUG
	UnorderedIndexSet32 debugPoseIndexSet;
#endif

	for (ObjectPointToObservations::const_iterator iObjectPoint = objectPointToObservations.cbegin(); iObjectPoint != objectPointToObservations.cend(); /*noop*/)
	{
		const PoseIndexToImagePointPairs& poseIndexToImagePointPairs = iObjectPoint->second;

		if (poseIndexToImagePointPairs.size() >= 2)
		{
#ifdef OCEAN_DEBUG
			for (const PoseIndexToImagePointPair& poseIndexToImagePointPair : poseIndexToImagePointPairs)
			{
				debugPoseIndexSet.emplace(poseIndexToImagePointPair.first);
			}
#endif
			++iObjectPoint;
		}
		else
		{
			iObjectPoint = objectPointToObservations.erase(iObjectPoint);

			++singleObservationObjectPoints;
		}
	}

	if (singleObservationObjectPoints != 0)
	{
		Log::debug() << "Removed " << singleObservationObjectPoints << " object points with only one observation, " << objectPointToObservations.size() << " object points left";
	}

#ifdef OCEAN_DEBUG
	ocean_assert(debugPoseIndexSet.size() == keyFrameIndices.size());
#endif

	return true;
}

void TrackerMono::describeObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CV::FramePyramid& yFramePyramid)
{
	ocean_assert(camera.isValid());
	ocean_assert(yFramePyramid.isValid());
	ocean_assert(yFramePyramid.finestWidth() == camera.width() && yFramePyramid.finestHeight() == camera.height());

	ReadLock readLock(mutex_, "TrackerMono::describeObjectPoints()");

		if (localizedObjectPointMap_.empty())
		{
			return;
		}

		Indices32 objectPointIds; // TODO reuse
		Vectors2 imagePoints;

		for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap_)
		{
			const Index32& objectPointId = objectPointPair.first;
			const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

			if (localizedObjectPoint.needDescriptor(currentFrameIndex))
			{
				if (localizedObjectPoint.lastObservationFrameIndex() == currentFrameIndex)
				{
					const Observation& lastObservation = localizedObjectPoint.lastObservation();

					const Vector2& imagePoint = lastObservation.imagePoint();

					objectPointIds.push_back(objectPointId);
					imagePoints.push_back(imagePoint);
				}
			}
		}

	readLock.unlock();

	if (objectPointIds.empty())
	{
		return;
	}

	CV::Detector::FREAKDescriptors32 freakDescriptors(imagePoints.size());
	CV::Detector::FREAKDescriptor32::computeDescriptors(camera.clone(), yFramePyramid, imagePoints.data(), imagePoints.size(), 0u /*pyramidLevel*/, freakDescriptors.data());

	ocean_assert(objectPointIds.size() == imagePoints.size());

	size_t numberAddedDescriptors = 0;

	const WriteLock writeLock(mutex_, "TrackerMono::describeObjectPoints()");

		for (size_t nImagePoint = 0; nImagePoint < objectPointIds.size(); ++nImagePoint)
		{
			CV::Detector::FREAKDescriptor32& freakDescriptor = freakDescriptors[nImagePoint];

			if (freakDescriptor.isValid())
			{
				const Index32& objectPointId = objectPointIds[nImagePoint];

				LocalizedObjectPointMap::iterator iObjectPoint = localizedObjectPointMap_.find(objectPointId);

				ocean_assert(iObjectPoint != localizedObjectPointMap_.cend());
				if (iObjectPoint != localizedObjectPointMap_.cend())
				{
					LocalizedObjectPoint& localizedObjectPoint = iObjectPoint->second;
					localizedObjectPoint.addDescriptors(currentFrameIndex, freakDescriptor);

					++numberAddedDescriptors;
				}
			}
		}

		if constexpr (loggingEnabled_)
		{
			if (numberAddedDescriptors != 0)
			{
				Log::info() << "    Background: Added " << numberAddedDescriptors << " descriptors";
			}
		}
		else
		{
			OCEAN_SUPPRESS_UNUSED_WARNING(numberAddedDescriptors);
		}
}

Index32 TrackerMono::frameIndexWithMostLocalizedObjectPoints(const Index32 necessaryMapVersion, const CameraPoses& cameraPoses, const LocalizedObjectPointMap& localizedObjectPointMap, const size_t minimalNumberObjectPoints, const UnorderedIndexSet32* ignoreFrameIndices)
{
	ocean_assert(!cameraPoses.isEmpty());
	ocean_assert(ignoreFrameIndices == nullptr || ignoreFrameIndices->size() <= cameraPoses.size());

	if (localizedObjectPointMap.size() < minimalNumberObjectPoints)
	{
		return Index32(-1);
	}

	size_t bestObjectPoints = 0;
	Index32 bestFrameIndex = Index32(-1);

	for (Index32 frameIndex = 0u; frameIndex <= cameraPoses.frameIndex(); ++frameIndex)
	{
		if (ignoreFrameIndices != nullptr && ignoreFrameIndices->contains(frameIndex))
		{
			continue;
		}

		SharedCameraPose cameraPose;
		if (!cameraPoses.hasPose(frameIndex, cameraPose))
		{
			continue;
		}

		ocean_assert(cameraPose);
		if (cameraPose->mapVersion() != necessaryMapVersion)
		{
			continue;
		}

		size_t objectPoints = 0;

		// TODO, this should be optimized by storing some information in CameraPose about which object points are used

		for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
		{
			const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;

			if (localizedObjectPoint.hasObservation(frameIndex))
			{
				++objectPoints;
			}
		}

		if (objectPoints > bestObjectPoints)
		{
			bestObjectPoints = objectPoints;

			bestFrameIndex = frameIndex;
		}
	}

	if (bestObjectPoints < minimalNumberObjectPoints)
	{
		return Index32(-1);
	}

	ocean_assert(bestFrameIndex != Index32(-1));
	return bestFrameIndex;
}

}

}

}
