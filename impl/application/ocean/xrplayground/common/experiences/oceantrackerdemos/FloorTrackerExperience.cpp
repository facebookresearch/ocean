// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/FloorTrackerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

FloorTrackerExperience::~FloorTrackerExperience()
{
	// nothing to do here
}

bool FloorTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	floorTracker6DOF_ = Devices::Manager::get().device("XRPlayground Floor 6DOF Tracker");

	if (floorTracker6DOF_.isNull())
	{
		Log::error() << "Failed to access XRPlayground's floor tracker";
		return false;
	}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const Devices::VisualTrackerRef visualTracker(floorTracker6DOF_);
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

#endif

	if (!floorTracker6DOF_->start())
	{
		Log::error() << "Failed to start XRPlayground's floor tracker";
		return false;
	}

	renderingTransform_ = engine->factory().createTransform();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	floorTrackerSampleEventSubscription_ = floorTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &FloorTrackerExperience::onTrackerSample));

	renderingTransform_->setVisible(false);

	experienceScene()->addChild(renderingTransform_);

#else

	if (!anchoredContentManager_.initialize(std::bind(&FloorTrackerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Devices::Tracker6DOF::ObjectId objectId = floorTracker6DOF_->objectId("Floor");
	ocean_assert(objectId != Devices::Tracker6DOF::invalidObjectId());

	if (objectId != Devices::Tracker6DOF::invalidObjectId())
	{
		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingTransform_, floorTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

#endif

	Rendering::TransformRef transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), RGBAColor(0.7f, 0.0f, 0.0f));
	transform->setTransformation(HomogenousMatrix4(Vector3(-1, Scalar(0.05), -1)));
	renderingTransform_->addChild(std::move(transform));

	transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.1), Scalar(1), Scalar(0.1)), RGBAColor(0.7f, 0.7f, 0.7f));
	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.5), -1)));
	renderingTransform_->addChild(std::move(transform));

	transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.1), Scalar(0.5), Scalar(0.1)), RGBAColor(0.0f, 0.7f, 0.0f));
	transform->setTransformation(HomogenousMatrix4(Vector3(1, Scalar(0.25), -1)));
	renderingTransform_->addChild(std::move(transform));

	transform = Rendering::Utilities::createCone(engine, Scalar(0.1), Scalar(1), RGBAColor(0.7f, 0.7f, 0.7f));
	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.5), -2)));
	renderingTransform_->addChild(std::move(transform));

	return true;
}

bool FloorTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingTransform_.release();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	floorTrackerSampleEventSubscription_.release();
#else
	anchoredContentManager_.release();
#endif

	return true;
}

Timestamp FloorTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return timestamp;

#else

	return anchoredContentManager_.preUpdate(engine, view, timestamp);

#endif
}

std::unique_ptr<XRPlaygroundExperience> FloorTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new FloorTrackerExperience());
}

void FloorTrackerExperience::onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::Tracker6DOF::Tracker6DOFSampleRef trackerSample(sample);

	if (trackerSample && trackerSample->objectIds().size() == 1)
	{
		ocean_assert(renderingTransform_);

		const HomogenousMatrix4 world_T_floor(trackerSample->positions().front(), trackerSample->orientations().front());

		renderingTransform_->setTransformation(world_T_floor);
		renderingTransform_->setVisible(true);
	}
}

void FloorTrackerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

}

}
