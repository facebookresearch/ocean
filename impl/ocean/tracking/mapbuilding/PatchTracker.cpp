/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/PatchTracker.h"

#include "ocean/base/Subset.h"
#include "ocean/base/ScopedValue.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/geometry/Octree.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/tracking/Solver3.h"

#ifdef _WINDOWS
	#include "ocean/platform/win/Utilities.h"
	#include "ocean/tracking/Utilities.h"
	#include "ocean/base/HighPerformanceTimer.h"
#endif

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool PatchTracker::TrackingData::isValid() const
{
	return objectPointIds_.size() == previousImagePoints_.size();
}

bool PatchTracker::LocatedTrackingData::isValid() const
{
	return TrackingData::isValid() && objectPointIds_.size() == objectPoints_.size() && objectPointIds_.size() == poseTranslationsWhenDeterminedObjectPoints_.size();
}

bool PatchTracker::trackFrame(const Index32 frameIndex, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const SharedFramePyramid& yCurrentFramePyramid, const Timestamp& frameTimestamp, Worker* worker, Frame* debugFrame)
{
	ocean_assert(anyCamera.isValid() && world_T_camera.isValid() && yCurrentFramePyramid && yCurrentFramePyramid->isValid() && frameTimestamp.isValid());

	needToUpdateFramePyramidForDescriptors_ = true;

	if (database_.hasPose<false>(frameIndex))
	{
		ocean_assert(false && "The frame has been tracked before!");
		return false;
	}

	database_.addPose<false>(frameIndex, world_T_camera);

	if (reusableOccupancyArray_.isValid())
	{
		if (reusableOccupancyArray_.width() != Scalar(anyCamera.width()) || reusableOccupancyArray_.height() != Scalar(anyCamera.height()))
		{
			ocean_assert(false && "The camera resolution has changed!");
			return false;
		}

		reusableOccupancyArray_.reset();
	}
	else
	{
		unsigned int horizontalBins = 0u;
		unsigned int verticalBins = 0u;
		Geometry::SpatialDistribution::idealBins(anyCamera.width(), anyCamera.height(), options_.maximalFeaturesPerFrame_, horizontalBins, verticalBins);

		// we will apply a 9 neighborhood, therefore, we need 3 times more bins
		horizontalBins *= 3u;
		verticalBins *= 3u;

		reusableOccupancyArray_ = Geometry::SpatialDistribution::OccupancyArray(Scalar(0), Scalar(0), Scalar(anyCamera.width()), Scalar(anyCamera.height()), horizontalBins, verticalBins);
	}

	ocean_assert(reusableOccupancyArray_.isValid());

	trackUnlocatedPoints(frameIndex, yCurrentFramePyramid, reusableOccupancyArray_, worker);
	trackLocatedPoints(frameIndex, anyCamera, world_T_camera, yCurrentFramePyramid, reusableOccupancyArray_, worker);

	addUnlocatedPoints(frameIndex, anyCamera, *yCurrentFramePyramid, frameTimestamp, reusableOccupancyArray_, worker);

	convertUnlocatedPointsToLocatedPoints(anyCamera, world_T_camera);

	if (debugFrame != nullptr)
	{
		Frame yFrame(yCurrentFramePyramid->finestLayer(), Frame::ACM_USE_KEEP_LAYOUT);
		yFrame.setPixelFormat(FrameType::FORMAT_Y8);

		if (CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, *debugFrame, CV::FrameConverter::CP_ALWAYS_COPY, worker, CV::FrameConverter::Options(true)))
		{
			const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

			const Vectors3 objectPoints = latestObjectPoints();

			for (const Vector3& objectPoint : objectPoints)
			{
				if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
				{
					const Vector2 projectedImagePoint = anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint);

					CV::Canvas::point8BitPerChannel<3u, 3u>(debugFrame->data<uint8_t>(), debugFrame->width(), debugFrame->height(), projectedImagePoint, CV::Canvas::blue(), debugFrame->paddingElements());
				}
			}

			CV::Canvas::points8BitPerChannel<3u, 3u>(debugFrame->data<uint8_t>(), debugFrame->width(), debugFrame->height(), unlocatedTrackingData_.previousImagePoints_, CV::Canvas::green(), debugFrame->paddingElements());
		}
	}

	return true;
}

Vectors3 PatchTracker::latestObjectPoints(Indices32* objectPointIds) const
{
	return database_.objectPoints<false, false>(Database::invalidObjectPoint(), objectPointIds);
}

void PatchTracker::reset(Database* database, std::shared_ptr<UnifiedDescriptorMap>* unifiedDescriptorMap)
{
	if (database != nullptr)
	{
		*database = std::move(database_);
	}

	if (unifiedDescriptorMap != nullptr)
	{
		*unifiedDescriptorMap = std::move(unifiedDescriptorMap_);
	}

	database_.clear<false>();
	unifiedDescriptorMap_ = unifiedDescriptorExtractor_->createUnifiedDescriptorMap();

	unlocatedTrackingData_ = UnlocatedTrackingData();
	locatedTrackingData_ = LocatedTrackingData();

	needToUpdateFramePyramidForDescriptors_ = true;
}

bool PatchTracker::trackRecording(Devices::DevicePlayer& devicePlayer, const std::vector<std::string>& worldTrackerNames, Database& database, SharedAnyCamera& anyCamera, std::shared_ptr<UnifiedDescriptorMap>& descriptorMap, const std::shared_ptr<UnifiedDescriptorExtractor>& unifiedDescriptorExtractor)
{
	ocean_assert(devicePlayer.isValid());
	if (!devicePlayer.isValid())
	{
		return false;
	}

	if (!devicePlayer.start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
	{
		Log::error() << "The recording could not be started";
		return false;
	}

	const Media::FrameMediumRefs frameMediums = devicePlayer.frameMediums();

	if (frameMediums.empty())
	{
		Log::error() << "The recording does not contain any frame mediums.";
		return false;
	}

	const Media::FrameMediumRef frameMedium = frameMediums.front();
	ocean_assert(frameMedium);

	frameMedium->start();

	Devices::Tracker6DOFRef worldTracker;

	for (const std::string& worldTrackerName : worldTrackerNames)
	{
		worldTracker = Devices::Manager::get().device(worldTrackerName);

		if (worldTracker)
		{
			break;
		}
	}

	if (worldTracker.isNull())
	{
		Log::error() << "The recording does not contain any World Tracker.";
		return false;
	}

	worldTracker->start();

	constexpr unsigned int pyramidLayers = 5u;

	PatchTracker patchTracker(unifiedDescriptorExtractor);

	SharedFramePyramid yPreviousFramePyramid;
	SharedFramePyramid yCurrentFramePyramid;

	unsigned int frameIndex = 0u;

	SharedAnyCamera currentAnyCamera;

	while (true)
	{
		const Timestamp timestamp = devicePlayer.playNextFrame();

		if (!timestamp.isValid())
		{
			break;
		}

		const FrameRef frame = frameMedium->frame(timestamp, &currentAnyCamera);

		if (frame.isNull() || !currentAnyCamera)
		{
			ocean_assert(false && "This should never happen!");
			return 1;
		}

		const ScopedValueT<unsigned int> scopedFrameIndex(frameIndex, frameIndex + 1u);

		if (yPreviousFramePyramid && (yPreviousFramePyramid->finestWidth() != frame->width() || yPreviousFramePyramid->finestHeight() != frame->height()))
		{
			ocean_assert(false && "The image resolution has changed!");
			return false;
		}

		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = worldTracker->sample(timestamp);

		if (sample.isNull() || sample->timestamp() != timestamp)
		{
#ifdef OCEAN_DEBUG
			Log::warning() << "<debug> No World Tracking pose for frame " << frameIndex << " with timestamp " << String::toAString(double(timestamp), 6u) << "'";
#endif

			continue;
		}

		if (sample->objectIds().size() != 1 || sample->referenceSystem() != Devices::Tracker6DOF::RS_DEVICE_IN_OBJECT)
		{
			ocean_assert(false && "Invalid sample!");
			return false;
		}

		const HomogenousMatrix4 world_T_camera(sample->positions().front(), sample->orientations().front());

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!yCurrentFramePyramid)
		{
			yCurrentFramePyramid = std::make_shared<CV::FramePyramid>();
		}

		yCurrentFramePyramid->replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*coypFirstLayer*/, WorkerPool::get().scopedWorker()(), yFrame.pixelFormat(), yFrame.timestamp());

		for (unsigned int n = 0u; n < yCurrentFramePyramid->layers(); ++n) // **TODO**
		{
			CV::FrameFilterGaussian::filter(yCurrentFramePyramid->layer(n), 5u, WorkerPool::get().scopedWorker()());
		}

		patchTracker.trackFrame(frameIndex, *currentAnyCamera, world_T_camera, yCurrentFramePyramid, yFrame.timestamp(), WorkerPool::get().scopedWorker()());

		if (yPreviousFramePyramid.use_count() == 1)
		{
			// nobody is using the previous pyramid anymore
			std::swap(yPreviousFramePyramid, yCurrentFramePyramid);
		}
		else
		{
			yCurrentFramePyramid = nullptr;
		}

#if 0
#ifdef _WINDOWS
		{
			Frame rgbFrame;
			CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGB24, rgbFrame, true, WorkerPool::get().scopedWorker()());

			Tracking::Utilities::visualizeDatabase(patchTracker.database_, frameIndex, rgbFrame, CV::Canvas::white(), CV::Canvas::red(), CV::Canvas::green(), 15u, 50u);
			Platform::Win::Utilities::desktopFrameOutput(0, 0, rgbFrame);
		}
#endif
#endif
	}

	anyCamera = std::move(currentAnyCamera);

	database = std::move(patchTracker.database_);
	descriptorMap = std::move(patchTracker.unifiedDescriptorMap_);

	return true;
}

size_t PatchTracker::removeFlakyObjectPoints(Database& database, const size_t minimalNumberObservations, const Scalar mimimalBoxDiagonal, Indices32* removedObjectPointIds)
{
	size_t removedPoints = 0;

	if (minimalNumberObservations >= 1)
	{
		removedPoints += Solver3::removeObjectPointsWithoutEnoughObservations(database, minimalNumberObservations, removedObjectPointIds);
	}

	removedPoints += Solver3::removeObjectPointsNotInFrontOfCamera(database, removedObjectPointIds);

	if (mimimalBoxDiagonal > 0)
	{
		removedPoints += Solver3::removeObjectPointsWithSmallBaseline(database, mimimalBoxDiagonal, removedObjectPointIds);
	}

	ocean_assert(removedObjectPointIds == nullptr || removedObjectPointIds->size() >= removedPoints);

	return removedPoints;
}

void PatchTracker::trackUnlocatedPoints(const Index32 frameIndex, const SharedFramePyramid& yCurrentFramePyramid, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker)
{
	ocean_assert(unlocatedTrackingData_.isValid());

	ocean_assert(yCurrentFramePyramid && occupancyArray.isValid());
	ocean_assert(Scalar(yCurrentFramePyramid->finestWidth()) == occupancyArray.width() && Scalar(yCurrentFramePyramid->finestHeight()) == occupancyArray.height());

	SharedFramePyramid& yPreviousFramePyramid = unlocatedTrackingData_.yPreviousFramePyramid_;
	Vectors2& previousImagePoints = unlocatedTrackingData_.previousImagePoints_;
	Indices32& objectPointIds = unlocatedTrackingData_.objectPointIds_;

	ocean_assert(database_.hasPose<false>(frameIndex));

	if (yPreviousFramePyramid && !previousImagePoints.empty())
	{
		constexpr unsigned int pyramidLayers = 3u;
		constexpr unsigned int coarsestLayerRadius = 8u;
		constexpr unsigned int subPixelIterations = 4u;

		constexpr unsigned int patchTrackingRadius = 16u * (1u << (pyramidLayers - 1u));
		OCEAN_SUPPRESS_UNUSED_WARNING(patchTrackingRadius);

		ocean_assert(yPreviousFramePyramid->layers() >= pyramidLayers);
		ocean_assert(yCurrentFramePyramid->layers() >= pyramidLayers);

		const CV::FramePyramid yReducedPreviousFramePyramid(*yPreviousFramePyramid, 0u, pyramidLayers, false /*copy*/);
		const CV::FramePyramid yReducedCurrentFramePyramid(*yCurrentFramePyramid, 0u, pyramidLayers, false /*copy*/);

		reusableCurrentImagePoints_.clear();
		reusableValidIndices_.clear();

		CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<1u, 15u>(yReducedPreviousFramePyramid, yReducedCurrentFramePyramid, coarsestLayerRadius, previousImagePoints, reusableCurrentImagePoints_, Scalar(0.9 * 0.9), worker, &reusableValidIndices_, subPixelIterations);

		for (const Index32& validIndex : reusableValidIndices_)
		{
			const Index32& objectPointId = objectPointIds[validIndex];
			const Vector2& currentImagePoint = reusableCurrentImagePoints_[validIndex];

			const Index32 imagePointId = database_.addImagePoint<false>(currentImagePoint);

			database_.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
			database_.attachImagePointToPose<false>(imagePointId, frameIndex);

			occupancyArray.addPoint(currentImagePoint);
		}

		if (!options_.keepUnlocatedFeatures_)
		{
			// we need to remove all object points from the database which we lost in this tracking iteration

			const Indices32 invalidIndices = Subset::invertedIndices(reusableValidIndices_, objectPointIds.size());

			for (const Index32 invalidIndex : invalidIndices)
			{
				const Index32 objectPointId = objectPointIds[invalidIndex];

				database_.removeObjectPointAndAttachedImagePoints<false>(objectPointId);

				unifiedDescriptorMap_->removeDescriptors(objectPointId);
			}
		}

		objectPointIds = Subset::subset(objectPointIds, reusableValidIndices_);
		reusableCurrentImagePoints_ = Subset::subset(reusableCurrentImagePoints_, reusableValidIndices_);

		std::swap(previousImagePoints, reusableCurrentImagePoints_);
	}

	yPreviousFramePyramid = yCurrentFramePyramid;

	ocean_assert(unlocatedTrackingData_.isValid());
}

void PatchTracker::trackLocatedPoints(const Index32 frameIndex, const AnyCamera& currentAnyCamera, const HomogenousMatrix4& world_T_currentCamera, const SharedFramePyramid& yCurrentFramePyramid, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker)
{
	ocean_assert(locatedTrackingData_.isValid());

	ocean_assert(currentAnyCamera.isValid() && world_T_currentCamera.isValid() && yCurrentFramePyramid);
	ocean_assert(currentAnyCamera.width() == yCurrentFramePyramid->finestWidth() && currentAnyCamera.height() == yCurrentFramePyramid->finestHeight());

	ocean_assert(Scalar(yCurrentFramePyramid->finestWidth()) == occupancyArray.width() && Scalar(yCurrentFramePyramid->finestHeight()) == occupancyArray.height());

	ocean_assert(database_.hasPose<false>(frameIndex));

	SharedFramePyramid& yPreviousFramePyramid = locatedTrackingData_.yPreviousFramePyramid_;
	Vectors2& previousImagePoints = locatedTrackingData_.previousImagePoints_;
	Vectors2& reusablePredictedCurrentImagePoints = locatedTrackingData_.reusablePredictedCurrentImagePoints_;
	Indices32& objectPointIds = locatedTrackingData_.objectPointIds_;
	Vectors3& objectPoints = locatedTrackingData_.objectPoints_;
	Vectors3& poseTranslationsWhenDeterminedObjectPoints = locatedTrackingData_.poseTranslationsWhenDeterminedObjectPoints_;

	unsigned int& framesWithoutGoodTracking = locatedTrackingData_.framesWithoutGoodTracking_;

	if (yPreviousFramePyramid && !previousImagePoints.empty())
	{
		constexpr unsigned int pyramidLayers = 1u;
		constexpr unsigned int coarsestLayerRadius = 8u;
		constexpr unsigned int subPixelIterations = 4u;

		ocean_assert(yPreviousFramePyramid->layers() >= pyramidLayers);
		ocean_assert(yCurrentFramePyramid->layers() >= pyramidLayers);

		const CV::FramePyramid yReducedPreviousFramePyramid(*yPreviousFramePyramid, 0u, pyramidLayers, false /*copy*/);
		const CV::FramePyramid yReducedCurrentFramePyramid(*yCurrentFramePyramid, 0u, pyramidLayers, false /*copy*/);

		reusablePredictedCurrentImagePoints.resize(objectPoints.size());

		currentAnyCamera.projectToImage(world_T_currentCamera, objectPoints.data(), objectPoints.size(), reusablePredictedCurrentImagePoints.data());

		for (Vector2& predictedCurrentImagePoint : reusablePredictedCurrentImagePoints)
		{
			predictedCurrentImagePoint.x() = minmax<Scalar>(0, predictedCurrentImagePoint.x(), Scalar(yCurrentFramePyramid->finestWidth() - 1u));
			predictedCurrentImagePoint.y() = minmax<Scalar>(0, predictedCurrentImagePoint.y(), Scalar(yCurrentFramePyramid->finestHeight() - 1u));
		}

		reusableCurrentImagePoints_.clear();
		reusableValidIndices_.clear();

		CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 15u>(yReducedPreviousFramePyramid, yReducedCurrentFramePyramid, coarsestLayerRadius, previousImagePoints, reusablePredictedCurrentImagePoints, reusableCurrentImagePoints_, Scalar(0.9 * 0.9), worker, &reusableValidIndices_, subPixelIterations);

		const float successRate = float(reusableValidIndices_.size()) / float(previousImagePoints.size());

		if (successRate < 0.75f && framesWithoutGoodTracking < 5u)
		{
			// the current frame may be hard for tracking e.g., because of strong motion blur, therefore we simply skip this frame and try to track the next frame (but using the same previous frame pyramid)

			for (const Vector2& predictedCurrentImagePoint : reusablePredictedCurrentImagePoints)
			{
				occupancyArray.addPoint(predictedCurrentImagePoint);
			}

			++framesWithoutGoodTracking;

			ocean_assert(locatedTrackingData_.isValid());

			return; // keeping the previous frame pyramid, previous image points, and object points untouched
		}
		else
		{
			framesWithoutGoodTracking = 0u;

			for (const Index32& validIndex : reusableValidIndices_)
			{
				const Index32& objectPointId = objectPointIds[validIndex];
				const Vector2& currentImagePoint = reusableCurrentImagePoints_[validIndex];

				const Index32 imagePointId = database_.addImagePoint<false>(currentImagePoint);

				database_.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
				database_.attachImagePointToPose<false>(imagePointId, frameIndex);

				const size_t numberObservations = database_.imagePointsFromObjectPoint<false>(objectPointId).size();

				ocean_assert(unifiedDescriptorMap_ && unifiedDescriptorExtractor_);
				const size_t numberDescriptors = unifiedDescriptorMap_->numberDescriptors(objectPointId);

				if (numberDescriptors * 60 < numberObservations && numberDescriptors < 10) // adding a new descriptor every 60th frame, at most 10
				{
					if (needToUpdateFramePyramidForDescriptors_)
					{
						replaceDescriptorPyramid(*yCurrentFramePyramid, yReusableFramePyramidForDescriptors_, worker);
						needToUpdateFramePyramidForDescriptors_ = false;
					}

					ocean_assert(yReusableFramePyramidForDescriptors_.isValid());

					unifiedDescriptorExtractor_->createAndAddDescriptor(yReusableFramePyramidForDescriptors_, currentAnyCamera, currentImagePoint, objectPointId, *unifiedDescriptorMap_);
				}
			}

			// for all feature points we lost in the current tracking iteration, we check whether the features have enough observations to stay in the database

			const Indices32 invalidIndices = Subset::invertedIndices(reusableValidIndices_, objectPointIds.size());

			for (const Index32 invalidIndex : invalidIndices)
			{
				const Index32 objectPointId = objectPointIds[invalidIndex];

				if (database_.numberImagePointsFromObjectPoint<false>(objectPointId) < options_.minimalNumberObservationsPerFeature_)
				{
					database_.removeObjectPointAndAttachedImagePoints<false>(objectPointId);

					unifiedDescriptorMap_->removeDescriptors(objectPointId);
				}
			}

			objectPointIds = Subset::subset(objectPointIds, reusableValidIndices_);
			objectPoints = Subset::subset(objectPoints, reusableValidIndices_);
			poseTranslationsWhenDeterminedObjectPoints = Subset::subset(poseTranslationsWhenDeterminedObjectPoints, reusableValidIndices_);

			reusableCurrentImagePoints_ = Subset::subset(reusableCurrentImagePoints_, reusableValidIndices_);

			for (const Vector2& currentImagePoint : reusableCurrentImagePoints_)
			{
				occupancyArray.addPoint(currentImagePoint);
			}

			previousImagePoints = std::move(reusableCurrentImagePoints_);
		}
	}

	// we update the 3D position of all object points

	for (size_t pointIndex = 0; pointIndex < objectPointIds.size(); /*noop*/)
	{
		const Index32& objectPointId = objectPointIds[pointIndex];

		const Vector3& poseTranslationsWhenDeterminedObjectPoint = poseTranslationsWhenDeterminedObjectPoints[pointIndex];

		if (poseTranslationsWhenDeterminedObjectPoint.sqrDistance(world_T_currentCamera.translation()) < Scalar(0.5 * 0.5) || database_.imagePointsFromObjectPoint<false>(objectPointId).size() >= 200)
		{
			// the camera hasn't moved enough since the last update, or the point has already enough observations

			++pointIndex;
			continue;
		}

		Vector3 objectPoint;

		const LocationResult locationResult = determineObjectPointLocation(currentAnyCamera, objectPointId, Geometry::Estimator::ET_HUBER, objectPoint);
		ocean_assert(locationResult != LR_NOT_YET);

		if (locationResult == LR_PRECISE)
		{
			objectPoints[pointIndex] = objectPoint;
			database_.setObjectPoint<false>(objectPointId, objectPoint);

			poseTranslationsWhenDeterminedObjectPoints[pointIndex] = world_T_currentCamera.translation();

			++pointIndex;
		}
		else
		{
			// the object point is flaky, we remove it

			objectPointIds[pointIndex] = objectPointIds.back();
			objectPointIds.pop_back();

			objectPoints[pointIndex] = objectPoints.back();
			objectPoints.pop_back();

			poseTranslationsWhenDeterminedObjectPoints[pointIndex] = poseTranslationsWhenDeterminedObjectPoints.back();
			poseTranslationsWhenDeterminedObjectPoints.pop_back();

			previousImagePoints[pointIndex] = previousImagePoints.back();
			previousImagePoints.pop_back();
		}
	}

	yPreviousFramePyramid = yCurrentFramePyramid;

	ocean_assert(locatedTrackingData_.isValid());
}

void PatchTracker::addUnlocatedPoints(const Index32 frameIndex, const AnyCamera& currentAnyCamera, const CV::FramePyramid& yCurrentFramePyramid, const Timestamp& frameTimestamp, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker)
{
	ocean_assert(unlocatedTrackingData_.isValid() && frameTimestamp.isValid());

	ocean_assert(currentAnyCamera.isValid() && yCurrentFramePyramid.isValid());
	ocean_assert(currentAnyCamera.width() == yCurrentFramePyramid.finestWidth() && currentAnyCamera.height() == yCurrentFramePyramid.finestHeight());

	ocean_assert(database_.hasPose<false>(frameIndex));

	if (frameTimestamp < unlocatedTrackingData_.lastNewFeaturesAddedTimestamp_ + options_.newFeaturesInterval_)
	{
		return;
	}

	CV::Detector::HarrisCorners& reusableHarrisCorners = unlocatedTrackingData_.reusableHarrisCorners_;
	Vectors2& previousImagePoints = unlocatedTrackingData_.previousImagePoints_;
	Indices32& objectPointIds = unlocatedTrackingData_.objectPointIds_;

	constexpr unsigned int detectorThreshold = 15u;

	reusableHarrisCorners.clear();
	CV::Detector::HarrisCornerDetector::detectCorners(yCurrentFramePyramid.finestLayer(), detectorThreshold, true, reusableHarrisCorners, true, worker);

	std::sort(reusableHarrisCorners.begin(), reusableHarrisCorners.end());

	for (const CV::Detector::HarrisCorner& newHarrisCorner : reusableHarrisCorners)
	{
		if (!occupancyArray.isOccupiedNeighborhood9(newHarrisCorner.observation()))
		{
			occupancyArray.addPoint(newHarrisCorner.observation());

			const Vector2& imagePoint = newHarrisCorner.observation();

			previousImagePoints.emplace_back(imagePoint);

			const Index32 objectPointId = database_.addObjectPoint<false>(Database::invalidObjectPoint());
			const Index32 imagePointId = database_.addImagePoint<false>(imagePoint);

			database_.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
			database_.attachImagePointToPose<false>(imagePointId, frameIndex);

			objectPointIds.emplace_back(objectPointId);

			if (needToUpdateFramePyramidForDescriptors_)
			{
				replaceDescriptorPyramid(yCurrentFramePyramid, yReusableFramePyramidForDescriptors_, worker);
				needToUpdateFramePyramidForDescriptors_ = false;
			}

			ocean_assert(unifiedDescriptorMap_ && unifiedDescriptorExtractor_);

			unifiedDescriptorExtractor_->createAndAddDescriptor(yReusableFramePyramidForDescriptors_, currentAnyCamera, imagePoint, objectPointId, *unifiedDescriptorMap_);
		}
	}

	ocean_assert(unlocatedTrackingData_.isValid());

	unlocatedTrackingData_.lastNewFeaturesAddedTimestamp_ = frameTimestamp;
}

void PatchTracker::convertUnlocatedPointsToLocatedPoints(const AnyCamera& currentAnyCamera, const HomogenousMatrix4& world_T_currentCamera)
{
	ocean_assert(unlocatedTrackingData_.isValid());
	ocean_assert(locatedTrackingData_.isValid());

	for (size_t pointIndex = 0; pointIndex < unlocatedTrackingData_.objectPointIds_.size(); /*noop*/)
	{
		const Index32& objectPointId = unlocatedTrackingData_.objectPointIds_[pointIndex];

		if (database_.imagePointsFromObjectPoint<false>(objectPointId).size() >= 20)
		{
			Vector3 objectPoint;
			if (determineObjectPointLocation(currentAnyCamera, objectPointId, Geometry::Estimator::ET_SQUARE, objectPoint) == LR_PRECISE)
			{
				ocean_assert(database_.objectPoint<false>(objectPointId) == Database::invalidObjectPoint());
				database_.setObjectPoint<false>(objectPointId, objectPoint);

				locatedTrackingData_.objectPoints_.emplace_back(objectPoint);
				locatedTrackingData_.poseTranslationsWhenDeterminedObjectPoints_.emplace_back(world_T_currentCamera.translation());

				locatedTrackingData_.objectPointIds_.emplace_back(objectPointId);
				unlocatedTrackingData_.objectPointIds_[pointIndex] = unlocatedTrackingData_.objectPointIds_.back();
				unlocatedTrackingData_.objectPointIds_.pop_back();

				locatedTrackingData_.previousImagePoints_.emplace_back(unlocatedTrackingData_.previousImagePoints_[pointIndex]);
				unlocatedTrackingData_.previousImagePoints_[pointIndex] = unlocatedTrackingData_.previousImagePoints_.back();
				unlocatedTrackingData_.previousImagePoints_.pop_back();

				continue;
			}
		}

		++pointIndex;
	}

	ocean_assert(unlocatedTrackingData_.isValid());
	ocean_assert(locatedTrackingData_.isValid());
}

PatchTracker::LocationResult PatchTracker::determineObjectPointLocation(const AnyCamera& currentAnyCamera, const Index32 objectPointId, const Geometry::Estimator::EstimatorType estimatorType, Vector3& objectPoint)
{
	constexpr Scalar ransacMaximalSqrError = Scalar(4 * 4);

	reusabledPoseIds_.clear();
	reusableImagePointIds_.clear();
	reusableImagePoints_.clear();
	database_.observationsFromObjectPoint<false>(objectPointId, reusabledPoseIds_, reusableImagePointIds_, &reusableImagePoints_);

	const unsigned int minimalValidObservations = (unsigned int)(reusabledPoseIds_.size() * 80 / 100);

	const HomogenousMatrices4 world_T_cameras = database_.poses<false>(reusabledPoseIds_.data(), reusabledPoseIds_.size());

	if (options_.minimalBoxDiagonalForLocatedFeature_ > 0)
	{
		Box3 boundingBox;

		for (const HomogenousMatrix4& world_T_camera : world_T_cameras)
		{
			boundingBox += world_T_camera.translation();
		}

		if (boundingBox.diagonal() < options_.minimalBoxDiagonalForLocatedFeature_)
		{
			return LR_NOT_YET;
		}
	}

	if (Geometry::RANSAC::objectPoint(currentAnyCamera, ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), ConstArrayAccessor<Vector2>(reusableImagePoints_), randomGenerator_, objectPoint, 5u, ransacMaximalSqrError, minimalValidObservations, true, estimatorType))
	{
		return LR_PRECISE;
	}

	return LR_FLAKY;
}

}

}

}
