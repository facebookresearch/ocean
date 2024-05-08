/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKDepthTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKDepthTracker6DOF::AKDepthTracker6DOF() :
	Device(deviceNameAKDepthTracker6DOF(), deviceTypeAKDepthTracker6DOF()),
	AKDevice(TrackerCapabilities(TC_SLAM | TC_DEPTH), deviceNameAKDepthTracker6DOF(), deviceTypeAKDepthTracker6DOF()),
	Measurement(deviceNameAKDepthTracker6DOF(), deviceTypeAKDepthTracker6DOF()),
	Tracker(deviceNameAKDepthTracker6DOF(), deviceTypeAKDepthTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKDepthTracker6DOF()),
	PositionTracker3DOF(deviceNameAKDepthTracker6DOF()),
	Tracker6DOF(deviceNameAKDepthTracker6DOF()),
	SceneTracker6DOF(deviceNameAKDepthTracker6DOF()),
	VisualTracker(deviceNameAKDepthTracker6DOF(), deviceTypeAKDepthTracker6DOF())
{
	worldObjectId_ = addUniqueObjectId("World");
}

AKDepthTracker6DOF::~AKDepthTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKDepthTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (frameMediums_.size() == 1 && frameMediums_.front())
	{
		isStarted_ = ARSessionManager::get().start(this, frameMediums_.front());
	}
	else
	{
		Log::error() << "AKDepthTracker6DOF needs one valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKDepthTracker6DOF::pause()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	ObjectIdSet lostObjects;

	if (worldIsTracked_)
	{
		lostObjects.emplace(worldObjectId_);
		worldIsTracked_ = false;
	}

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKDepthTracker6DOF::stop()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (!ARSessionManager::get().stop(this))
	{
		return false;
	}

	if (!worldIsTracked_)
	{
		return true;
	}

	ObjectIdSet lostObjects = {worldObjectId_};
	worldIsTracked_ = false;

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKDepthTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == worldObjectId_);

	return worldIsTracked_ && objectId == worldObjectId_;
}

void AKDepthTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() == 1 && frameMediums.front())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();

		if (frameMedium->library() == Media::AVFoundation::nameAVFLibrary() && (frameMedium->type() & Media::Medium::LIVE_VIDEO) == Media::Medium::LIVE_VIDEO)
		{
			if (frameMedium->url() == "Back Camera" || frameMedium->url() == "Front Camera")
			{
				VisualTracker::setInput(std::move(frameMediums));
				return;
			}
		}
	}

	ocean_assert(false && "Invalid frameMedium!");
}

void AKDepthTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, const SharedAnyCamera& camera, const HomogenousMatrix4& device_T_depth, ARFrame* arFrame)
{
	ocean_assert(camera);
	ocean_assert(arFrame != nullptr);

	SharedSceneElements sceneElements;
	sceneElements.reserve(2);

	if (@available(iOS 14.0, *))
	{
		if (arFrame.sceneDepth != nullptr)
		{
			const Media::AVFoundation::PixelBufferAccessor depthPixelBufferAccessor(arFrame.sceneDepth.depthMap, true /*readonly*/);

			if (depthPixelBufferAccessor)
			{
				const Frame& depthFrame = depthPixelBufferAccessor.frame();

				std::shared_ptr<Frame> depth = std::make_shared<Frame>(depthFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				SharedAnyCamera depthCamera = camera;

				if (depthCamera->width() != depthFrame.width() || depthCamera->height() != depthFrame.height())
				{
					depthCamera = depthCamera->clone(depthFrame.width(), depthFrame.height());
					if (!depthCamera)
					{
						Log::warning() << "Depth image has wrong image resolution!";
					}
				}

				std::shared_ptr<Frame> confidence;

				if (arFrame.sceneDepth.confidenceMap != nullptr)
				{
					const Media::AVFoundation::PixelBufferAccessor confidencePixelBufferAccessor(arFrame.sceneDepth.confidenceMap, true /*readonly*/);

					if (confidencePixelBufferAccessor)
					{
						confidence = std::make_shared<Frame>(confidencePixelBufferAccessor.frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
					}
				}

				if (depthCamera != nullptr)
				{
					sceneElements.emplace_back(std::make_shared<SceneElementDepth>(std::move(depthCamera), device_T_depth, std::move(depth), std::move(confidence)));
				}
			}
		}
	}

	if (sceneElements.empty())
	{
		sceneElements.emplace_back(nullptr); // adding a pure 6-DOF pose scene element
	}

	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid())
	{
		ocean_assert(worldObjectId_ != invalidObjectId());

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(1, worldObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_camera.rotation());

		if (!worldIsTracked_)
		{
			foundObjectIds = {worldObjectId_};
			worldIsTracked_ = true;
		}

		scopedLock.release();

		postFoundTrackerObjects(foundObjectIds, timestamp);

		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(sceneElements))));
	}
	else
	{
		ObjectIdSet lostObjectIds;

		if (worldIsTracked_)
		{
			lostObjectIds = {worldObjectId_};
			worldIsTracked_ = false;
		}

		scopedLock.release();

		postLostTrackerObjects(lostObjectIds, timestamp);
	}
}

}

}

}
