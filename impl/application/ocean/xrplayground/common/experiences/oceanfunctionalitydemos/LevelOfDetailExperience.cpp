// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/LevelOfDetailExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/LOD.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

LevelOfDetailExperience::~LevelOfDetailExperience()
{
	// nothing to do here
}

bool LevelOfDetailExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

	Rendering::LODRef renderingLOD = engine->factory().createLOD();

	renderingLOD->addChild(Rendering::Utilities::createBox(engine, Vector3(Scalar(0.2), Scalar(0.2), Scalar(0.2)), RGBAColor(1.0f, 0.0f, 0.0f))); // red box
	renderingLOD->addChild(Rendering::Utilities::createBox(engine, Vector3(Scalar(0.2), Scalar(0.2), Scalar(0.2)), RGBAColor(0.0f, 1.0f, 0.0f))); // green box

	renderingLOD->addChild(engine->factory().createGroup()); // an empty group to avoid displaying anything

	renderingLOD->addChild(Rendering::Utilities::createSphere(engine, Scalar(0.1), RGBAColor(1.0f, 0.0f, 0.0f))); // red sphere
	renderingLOD->addChild(Rendering::Utilities::createSphere(engine, Scalar(0.1), RGBAColor(0.0f, 1.0f, 0.0f))); // green sphere

	const Rendering::LOD::DistanceRanges ranges =
	{
		Scalar(0.0), // red box for [0, 0.5]
		Scalar(0.5), // green box for [0.5, 1.0]
		Scalar(1.0), // nothing for [1.0, 0.5]
		Scalar(1.5), // red sphere [1.5, 2.0]
		Scalar(2.0), // green sphere [2.0, 4.0]
		Scalar(3.0)  // nothing [4.0, infinity)
	};

	renderingLOD->setDistanceRanges(ranges);

	Rendering::TransformRef renderingTransform = engine->factory().createTransform();
	renderingTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2))); // moving 2 meters in front of the user
	renderingTransform->addChild(renderingLOD);

	Rendering::TransformRef renderingTransformIndicator = Rendering::Utilities::createArrow(engine, Scalar(0.9), Scalar(0.1), Scalar(0.05), RGBAColor(0.7f, 0.7f, 0.7f));
	renderingTransformIndicator->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(1.5), 0), Quaternion(Vector3(1, 0, 0), Numeric::pi())));
	renderingTransform->addChild(renderingTransformIndicator);

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		slamTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF World Tracker");
	#else
		slamTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF World Tracker");
	#endif

	if (slamTracker6DOF_.isNull())
	{
		Log::error() << "Failed to access SLAM tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(slamTracker6DOF_);
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

	slamTracker6DOF_->start();

	if (!anchoredContentManager_.initialize(std::bind(&LevelOfDetailExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Devices::Tracker6DOF::ObjectId trackerObjectId = slamTracker6DOF_->objectId("World");

	if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
	{
		constexpr Scalar visibilityRadius = Scalar(10);
		constexpr Scalar engagementRadius = Scalar(1000);
		anchoredContentManager_.addContent(renderingTransform, slamTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);
	}

#else

	experienceScene()->addChild(renderingTransform);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return true;
}

bool LevelOfDetailExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	anchoredContentManager_.release();

	slamTracker6DOF_.release();

	return true;
}

Timestamp LevelOfDetailExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	const Timestamp updatedTimestamp = anchoredContentManager_.preUpdate(engine, view, timestamp);

	return updatedTimestamp;
}

std::unique_ptr<XRPlaygroundExperience> LevelOfDetailExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new LevelOfDetailExperience());
}

void LevelOfDetailExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

}

}
