// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceRelocalizerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

OnDeviceRelocalizerExperience::OnDeviceRelocalizerExperience()
{
	// nothing to do here
}

OnDeviceRelocalizerExperience::~OnDeviceRelocalizerExperience()
{
	// nothing to do here
}

bool OnDeviceRelocalizerExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	if (anchoredContentManager_)
	{
		Log::warning() << "The experience is already loaded";
		return false;
	}

	relocalizerTracker6DOF_ = Devices::Manager::get().device("On-Device Relocalizer 6DOF Tracker");

	if (!relocalizerTracker6DOF_)
	{
		Log::error() << "Failed to access relocalizer tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(relocalizerTracker6DOF_);
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

	ocean_assert(!worldTrackerSampleSubscribe_);
	worldTrackerSampleSubscribe_ = relocalizerTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &OnDeviceRelocalizerExperience::onWorldTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&OnDeviceRelocalizerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingGroup_ = engine->factory().createGroup();

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface);
	}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return true;
}

bool OnDeviceRelocalizerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	worldTrackerSampleSubscribe_.release();

	renderingGroup_.release();

	anchoredContentManager_.release();

	return true;
}

Timestamp OnDeviceRelocalizerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
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

bool OnDeviceRelocalizerExperience::start(const std::string& filename)
{
	Devices::ObjectTrackerRef objectTracker = relocalizerTracker6DOF_;

	if (!objectTracker)
	{
		return false;
	}

	const Devices::Tracker::ObjectId objectId = objectTracker->registerObject(filename);

	if (objectId == Devices::Tracker::invalidObjectId())
	{
		Log::error() << "Failed to register map";
		return false;
	}

	if (!relocalizerTracker6DOF_->start())
	{
		return false;
	}

	const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(renderingGroup_, relocalizerTracker6DOF_, objectId, Scalar(500), Scalar(10000));

	if (contentId == AnchoredContentManager::invalidContentId())
	{
		Log::error() << "Failed to register content";

		return false;
	}

	return true;
}

bool OnDeviceRelocalizerExperience::stop()
{
	relocalizerTracker6DOF_->stop();

	return true;
}

std::unique_ptr<XRPlaygroundExperience> OnDeviceRelocalizerExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new OnDeviceRelocalizerExperience());
}

void OnDeviceRelocalizerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Should never happen!");
}

void OnDeviceRelocalizerExperience::onWorldTrackerSample(const Devices::Measurement* /*measurement*/, const Devices::Measurement::SampleRef& sample)
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
