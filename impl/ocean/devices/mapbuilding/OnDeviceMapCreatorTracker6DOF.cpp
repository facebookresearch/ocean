/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/mapbuilding/OnDeviceMapCreatorTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/ScopedValue.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/File.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/mapbuilding/MapMerging.h"
#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

using namespace Ocean::Tracking::MapBuilding;

OnDeviceMapCreatorTracker6DOF::OnDeviceMapCreatorTracker6DOF() :
	Device(deviceNameOnDeviceMapCreatorTracker6DOF(), deviceTypeOnDeviceMapCreatorTracker6DOF()),
	MapBuildingDevice(deviceNameOnDeviceMapCreatorTracker6DOF(), deviceTypeOnDeviceMapCreatorTracker6DOF()),
	Measurement(deviceNameOnDeviceMapCreatorTracker6DOF(), deviceTypeOnDeviceMapCreatorTracker6DOF()),
	Tracker(deviceNameOnDeviceMapCreatorTracker6DOF(), deviceTypeOnDeviceMapCreatorTracker6DOF()),
	OrientationTracker3DOF(deviceNameOnDeviceMapCreatorTracker6DOF()),
	PositionTracker3DOF(deviceNameOnDeviceMapCreatorTracker6DOF()),
	Tracker6DOF(deviceNameOnDeviceMapCreatorTracker6DOF()),
	SceneTracker6DOF(deviceNameOnDeviceMapCreatorTracker6DOF()),
	VisualTracker(deviceNameOnDeviceMapCreatorTracker6DOF(), deviceTypeOnDeviceMapCreatorTracker6DOF())
{
	const Strings deviceNames =
	{
#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		"ARCore 6DOF World Tracker",
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		"ARKit 6DOF World Tracker",
#endif
		"VRS ARCore 6DOF World Tracker",
		"VRS ARKit 6DOF World Tracker",
		"VRS Rover 6DOF World Tracker"
	};

	for (const std::string& name : deviceNames)
	{
		worldTracker_ = Manager::get().device(name);

		if (worldTracker_)
		{
			break;
		}
	}

	if (worldTracker_.isNull())
	{
		Log::warning() << "Failed to access world tracker tracker!";

		deviceIsValid = false;
		return;
	}

	mapObjectId_ = addUniqueObjectId("Map");

	ocean_assert(deviceIsValid);
}

OnDeviceMapCreatorTracker6DOF::~OnDeviceMapCreatorTracker6DOF()
{
	stopThreadExplicitly();
}

bool OnDeviceMapCreatorTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool OnDeviceMapCreatorTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		return false;
	}

	if (isThreadActive())
	{
		return true;
	}

	ocean_assert(worldTracker_);

	if (Devices::VisualTrackerRef visualWorldTracker = worldTracker_)
	{
		visualWorldTracker->setInput(Media::FrameMediumRefs(frameMediums_));
	}

	if (!worldTrackerSampleSubscription_)
	{
		worldTrackerSampleSubscription_ = worldTracker_->subscribeSampleEvent(SampleCallback::create(*this, &OnDeviceMapCreatorTracker6DOF::onWorldTrackerSample));
	}

	worldTracker_->start();

	startThread();

	Log::info() << "6DOF On-Device Relocalizer tracker started.";

	return true;
}

bool OnDeviceMapCreatorTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	worldTrackerSampleSubscription_.release();

	stopThread();

	return true;
}

bool OnDeviceMapCreatorTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	if (mapObjectId_ == objectId)
	{
		return isMapTracked_;
	}

	return false;
}

bool OnDeviceMapCreatorTracker6DOF::exportSceneElements(const std::string& format, std::ostream& outputStream, const std::string& options) const
{
	if (format != "ocean_map")
	{
		return false;
	}

	const ScopedLock scopedLock(deviceLock);

	if (isThreadActive())
	{
		sleep(100u);

		if (isThreadActive())
		{
			Log::error() << "Failed to export the data, optimization took too long";
			return false;
		}
	}

	if (lastDatabase_.isEmpty<false>() || !lastUnifiedDescriptorMap_)
	{
		return false;
	}

	if (options == "optimized")
	{
		const Timestamp optimizationStartTimestamp(true);

		// we removed all flaky object points from the database

		constexpr size_t minimalNumberObservations = 30;
		constexpr Scalar mimimalBoxDiagonal = Scalar(0.1);

		Indices32 removedObjectPointIds;
		Tracking::MapBuilding::PatchTracker::removeFlakyObjectPoints(lastDatabase_, minimalNumberObservations, mimimalBoxDiagonal, &removedObjectPointIds);

		for (const Index32& removedObjectPointId : removedObjectPointIds)
		{
			lastUnifiedDescriptorMap_->removeDescriptors(removedObjectPointId);
		}

		// we execute a bundle adjustment optimization

		RandomGenerator randomGenerator;

		if (lastAnyCamera_->name() == AnyCameraPinhole::WrappedCamera::name())
		{
			const PinholeCamera& pinholeCamera = ((const AnyCameraPinhole&)(*lastAnyCamera_)).actualCamera();

			Tracking::MapBuilding::MapMerging::bundleAdjustment(lastDatabase_, pinholeCamera, randomGenerator, 10u);

			// we remove all object points which are not located anymore (which lost their location during bundle adjustment)

			const Indices32 unlocatedObjectPointIds = lastDatabase_.objectPointIds<false, true>(Tracking::Database::invalidObjectPoint());

			for (const Index32& unlocatedObjectPointId : unlocatedObjectPointIds)
			{
				lastDatabase_.removeObjectPointAndAttachedImagePoints<false>(unlocatedObjectPointId);
				lastUnifiedDescriptorMap_->removeDescriptors(unlocatedObjectPointId);
			}
		}

		Log::info() << "Finished optimization after " << String::toAString(double(Timestamp(true) - optimizationStartTimestamp), 2u) << " seconds";

		Log::info() << "The final database contains " << lastDatabase_.objectPointIds<false>().size() << " feature points";
	}

	IO::OutputBitstream outputBitstream(outputStream);

	if (!Tracking::Utilities::writeDatabase(lastDatabase_, outputBitstream))
	{
		return false;
	}

	if (!Tracking::MapBuilding::Utilities::writeDescriptorMap(*lastUnifiedDescriptorMap_, outputBitstream))
	{
		return false;
	}

	lastAnyCamera_ = nullptr;
	lastDatabase_ = Tracking::Database();
	lastUnifiedDescriptorMap_ = nullptr;

	return true;
}

void OnDeviceMapCreatorTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	ocean_assert(mapObjectId_ != invalidObjectId());
	ocean_assert(isMapTracked_ == false);

	Tracking::MapBuilding::PatchTracker patchTracker(std::make_shared<Tracking::MapBuilding::UnifiedDescriptorExtractorFreakMultiDescriptor256>(), Tracking::MapBuilding::PatchTracker::Options::realtimeOptions());

	HighPerformanceStatistic performance;

	Index32 frameIndex = 0u;
	Index32 lastProcessedFrameIndex = Index32(-1);

	Timestamp lastFrameTimestamp(false);
	Timestamp lastSceneElementTimestamp(false);

	constexpr unsigned int pyramidLayers = 5u;

	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yPreviousFramePyramid;
	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yCurrentFramePyramid;

	SharedAnyCamera lastAnyCamera;

	while (shouldThreadStop() == false)
	{
		SharedAnyCamera frameAnyCamera;
		const FrameRef frame = frameMedium->frame(&frameAnyCamera);

		if (frame && frameAnyCamera)
		{
			ocean_assert(frame->isValid() && frameAnyCamera->isValid());

			const ScopedValueT<unsigned int> scopedFrameIndex(frameIndex, (frame->timestamp() == lastFrameTimestamp) ? frameIndex : (frameIndex + 1u));
			const ScopedValueT<Timestamp> scopedLastFrameTimestamp(lastFrameTimestamp, frame->timestamp());

			if (frameIndex == lastProcessedFrameIndex)
			{
				sleep(1u);
				continue;
			}

			const Devices::Tracker6DOF::Tracker6DOFSampleRef sample(worldTracker_->sample(frame->timestamp()));

			if (!sample || sample->timestamp() != frame->timestamp())
			{
				continue;
			}

			ocean_assert(!sample->objectIds().empty());

			if (yPreviousFramePyramid && (yPreviousFramePyramid->finestWidth() != frame->width() || yPreviousFramePyramid->finestHeight() != frame->height()))
			{
				ocean_assert(false && "The image resolution has changed!");
				break;
			}

			const ScopedValueT<Index32> scopedLastProcessedFrameIndex(lastProcessedFrameIndex, frameIndex);

			if (performance.measurements() >= 100u)
			{
				Log::info() << "Performance: " << performance.averageMseconds() << "ms";
				performance.reset();
			}

			HomogenousMatrix4 world_T_camera(sample->positions().front(), sample->orientations().front());

			if (sample->referenceSystem() == Devices::Tracker6DOF::RS_OBJECT_IN_DEVICE)
			{
				world_T_camera.invert();
			}

			Frame yFrame;
			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
			{
				ocean_assert(false && "This should never happen!");
				break;
			}

			while (yFrame.width() * yFrame.height() > 1280u * 720u)
			{
				CV::FrameShrinker::downsampleByTwo11(yFrame);
			}

			SharedAnyCamera anyCamera = frameAnyCamera;

			if (yFrame.width() != anyCamera->width() || yFrame.height() != anyCamera->height())
			{
				anyCamera = frameAnyCamera->clone(yFrame.width(), yFrame.height());

				if (!anyCamera)
				{
					ocean_assert(false && "Failed to scale camera profile!");
					break;
				}
			}

			if (!yCurrentFramePyramid)
			{
				yCurrentFramePyramid = std::make_shared<CV::FramePyramid>();
			}

			yCurrentFramePyramid->replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*copyFirstLayer*/, WorkerPool::get().scopedWorker()(), yFrame.pixelFormat(), yFrame.timestamp());

			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			patchTracker.trackFrame(frameIndex, *anyCamera, world_T_camera, yCurrentFramePyramid, yFrame.timestamp(), WorkerPool::get().scopedWorker()());

			lastAnyCamera = std::move(anyCamera);

			if (yPreviousFramePyramid.use_count() == 1)
			{
				// nobody is using the previous pyramid anymore
				std::swap(yPreviousFramePyramid, yCurrentFramePyramid);
			}
			else
			{
				yCurrentFramePyramid = nullptr;
			}

			constexpr double sceneElementInterval = 0.5;

			if (frame->timestamp() >= lastSceneElementTimestamp + sceneElementInterval)
			{
				lastSceneElementTimestamp = frame->timestamp();

				Indices32 objectPointIds;
				Vectors3 objectPoints = patchTracker.latestObjectPoints(&objectPointIds);

				if (!objectPoints.empty())
				{
					Indices64 objectPointIds64;
					objectPointIds64.reserve(objectPointIds.size());

					for (const Index32& objectPointId : objectPointIds)
					{
						objectPointIds64.emplace_back(Index64(objectPointId));
					}

					const ScopedLock scopedLock(pointLock_);

					recentObjectPoints_ = std::move(objectPoints);
					recentObjectPointIds_ = std::move(objectPointIds64);
				}
			}
		}
		else
		{
			sleep(1u);
		}
	}

	if (isMapTracked_)
	{
		postLostTrackerObjects({mapObjectId_}, lastFrameTimestamp);

		isMapTracked_ = false;
	}

	// we move the database and descriptor map out of the tracker

	lastAnyCamera_ = std::move(lastAnyCamera);

	patchTracker.reset(&lastDatabase_, &lastUnifiedDescriptorMap_);
}

void OnDeviceMapCreatorTracker6DOF::onWorldTrackerSample(const Measurement* /*measurement*/, const SampleRef& sample)
{
	const Tracker6DOFSampleRef tracker6DOFSample(sample);

	ocean_assert(tracker6DOFSample);
	ocean_assert(tracker6DOFSample->referenceSystem() == RS_DEVICE_IN_OBJECT);

	if (tracker6DOFSample->objectIds().empty())
	{
		return;
	}

	const Timestamp& timestamp = tracker6DOFSample->timestamp();

	ocean_assert(tracker6DOFSample->objectIds().size() == 1); // world tracker is always providing one world transformation

	SharedSceneElements sceneElements(1, nullptr);

	TemporaryScopedLock scopedLock(pointLock_);
		if (!recentObjectPoints_.empty())
		{
			sceneElements[0] = std::make_shared<SceneElementObjectPoints>(std::move(recentObjectPoints_), std::move(recentObjectPointIds_));
		}
	scopedLock.release();

	HomogenousMatrix4 world_T_camera(tracker6DOFSample->positions().front(), tracker6DOFSample->orientations().front());
	if (tracker6DOFSample->referenceSystem() == Devices::Tracker6DOF::RS_OBJECT_IN_DEVICE)
	{
		world_T_camera.invert();
	}

	if (!isMapTracked_)
	{
		postFoundTrackerObjects({mapObjectId_}, timestamp);

		scopedLock.relock(deviceLock);

		isMapTracked_ = true;
	}

	postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, {mapObjectId_}, {world_T_camera.rotation()}, {world_T_camera.translation()}, std::move(sceneElements))));
}

}

}

}
