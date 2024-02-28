// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceMapCreatorExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

OnDeviceMapCreatorExperience::OnDeviceMapCreatorExperience()
{
	// nothing to do here
}

OnDeviceMapCreatorExperience::~OnDeviceMapCreatorExperience()
{
	// nothing to do here
}

bool OnDeviceMapCreatorExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	if (anchoredContentManager_)
	{
		Log::warning() << "The experience is already loaded";
		return false;
	}

	mapCreatorTracker6DOF_ = Devices::Manager::get().device("On-Device Map Creator 6DOF Tracker");

	if (!mapCreatorTracker6DOF_)
	{
		Log::error() << "Failed to access relocalizer tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(mapCreatorTracker6DOF_);
	if (visualTracker && !engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			const Media::FrameMediumRef frameMedium(undistortedBackground->medium());

			if (frameMedium)
			{
				visualTracker->setInput(frameMedium);
			}
		}
	}

	const Devices::Tracker::ObjectId objectId = mapCreatorTracker6DOF_->objectId("Map");

	if (objectId == Devices::Tracker::invalidObjectId())
	{
		Log::error() << "Failed to register map";
		return false;
	}

	ocean_assert(!worldTrackerSampleSubscribe_);
	worldTrackerSampleSubscribe_ = mapCreatorTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &OnDeviceMapCreatorExperience::onWorldTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&OnDeviceMapCreatorExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingGroup_ = engine->factory().createGroup();

	const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(renderingGroup_, mapCreatorTracker6DOF_, objectId, Scalar(500), Scalar(10000));

	if (contentId == AnchoredContentManager::invalidContentId())
	{
		Log::error() << "Failed to register content";
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool OnDeviceMapCreatorExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stop();

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		unloadUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	anchoredContentManager_.release();

	return true;
}

Timestamp OnDeviceMapCreatorExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	{
		TemporaryScopedLock scopdLock(lock_);

		if (!objectPoints_.empty())
		{
			ocean_assert(renderingGroup_);
			const Vectors3 objectPoints = std::move(objectPoints_);

			scopdLock.release();

			renderingGroup_->clear();
			renderingGroup_->addChild(Rendering::Utilities::createPoints(*engine, objectPoints, RGBAColor(1, 0, 0), Scalar(8)));
		}
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

bool OnDeviceMapCreatorExperience::start()
{
	if (!mapCreatorTracker6DOF_->start())
	{
		return false;
	}

	return true;
}

bool OnDeviceMapCreatorExperience::stop()
{
	mapCreatorTracker6DOF_->stop();

	return true;
}

bool OnDeviceMapCreatorExperience::save(const std::string& filename, const bool optimized)
{
	ocean_assert(!filename.empty());

	std::ofstream outputStream(filename.c_str(), std::ios::binary);

	if (!outputStream.is_open())
	{
		return false;
	}

	const std::string options = optimized ? "optimized" : "";

	if (!mapCreatorTracker6DOF_->exportSceneElements("ocean_map", outputStream, options))
	{
		Log::error() << "Failed to export map";
	}

	return true;
}

std::unique_ptr<XRPlaygroundExperience> OnDeviceMapCreatorExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new OnDeviceMapCreatorExperience());
}

void OnDeviceMapCreatorExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Should never happen!");
}

void OnDeviceMapCreatorExperience::onWorldTrackerSample(const Devices::Measurement* /*measurement*/, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTracker6DOFSample(sample);

	ocean_assert(sceneTracker6DOFSample);
	ocean_assert(sceneTracker6DOFSample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);

	if (sceneTracker6DOFSample->objectIds().empty())
	{
		return;
	}

	for (const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement : sceneTracker6DOFSample->sceneElements())
	{
		if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS)
		{
			const Devices::SceneTracker6DOF::SceneElementObjectPoints& sceneElementObjectPoints = (const Devices::SceneTracker6DOF::SceneElementObjectPoints&)(*sceneElement);

			const ScopedLock scopdLock(lock_);

			objectPoints_ = sceneElementObjectPoints.objectPoints();

			return;
		}
	}
}

}

}
