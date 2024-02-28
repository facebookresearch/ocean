// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/GenericCloudPlacementsExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace XRPlayground
{

GenericCloudPlacementsExperience::~GenericCloudPlacementsExperience()
{
	// nothing to do here
}

bool GenericCloudPlacementsExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	ocean_assert(engine);

	cloudRelocalizerTracker6DOF_ = Devices::Manager::get().device("Cloud Placements 6DOF Tracker");

	if (!cloudRelocalizerTracker6DOF_)
	{
		Log::error() << "Generic Cloud Placements Experience could not access cloud tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(cloudRelocalizerTracker6DOF_);
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

	trackerObjectEventSubscription_ = cloudRelocalizerTracker6DOF_->subscribeTrackerObjectEvent(Devices::Tracker::TrackerObjectCallback::create(*this, &GenericCloudPlacementsExperience::onTrackerObjectEvent));

	if (!anchoredContentManager_.initialize(std::bind(&GenericCloudPlacementsExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingCoordinateSystem_ = Rendering::Utilities::createCoordinateSystem(engine, Scalar(1), Scalar(0.05), Scalar(0.05));

	const Rendering::AbsoluteTransformRef renderingAbsoluteTransformState = engine->factory().createAbsoluteTransform();
	renderingAbsoluteTransformState->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
	renderingAbsoluteTransformState->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(renderingAbsoluteTransformState);

	renderingAbsoluteTransformState->addChild(Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, "", "", &renderingTextState_));

	if (!properties.empty() && properties.front() == '{' && properties.back() == '}')
	{
		Tracking::Cloud::CloudRelocalizer::Configuration testConfiguration;
		std::string errorMessage;

		if (Tracking::Cloud::CloudRelocalizer::Configuration::parseConfiguration(properties, testConfiguration, &errorMessage))
		{
			Log::info() << "Starting relocalization given configuration";

			startRelocalization(properties);

			return true;
		}
		else
		{
			Log::error() << "Failed to parse cloud configuration: " << errorMessage;
		}
	}

	renderingTextState_->setText("Select the release environment...");

	const std::vector<std::pair<std::string, std::string>> configurations(selectableConfigurations());

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)

	// on iOS we use the Apple's UI functionality to show a menu
	showConfigurationsIOS(userInterface, configurations);

#elif defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)

	// on macOS we show a menu entry for which the user needs to press a number key

	renderingMenuTransform_ = engine->factory().createAbsoluteTransform();
	renderingMenuTransform_->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
	renderingMenuTransform_->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.5)));

	std::string menuText = " Make a key selection: \n";

	for (size_t n = 0; n < configurations.size(); ++n)
	{
		menuText += "\n " + String::toAString(n) + ": " + configurations[n].first + " ";
	}

	const Rendering::TransformRef renderingMenuText = Rendering::Utilities::createText(*engine, menuText, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.0003), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
	renderingMenuTransform_->addChild(renderingMenuText);
	experienceScene()->addChild(renderingMenuTransform_);

#endif

	return true;
}

bool GenericCloudPlacementsExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	trackerObjectEventSubscription_.release();

	renderingCoordinateSystem_.release();
	renderingTextState_.release();
	renderingMenuTransform_.release();

	anchoredContentManager_.release();

	cloudRelocalizerTracker6DOF_.release();

	return true;
}

Timestamp GenericCloudPlacementsExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

void GenericCloudPlacementsExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (renderingMenuTransform_ && key.size() == 1)
	{
		const char value = key[0];

		if (value >= '0' && value <= '9')
		{
			const size_t selection = size_t(value - '0');

			if (selection < selectableConfigurations().size())
			{
				startRelocalization(selectableConfigurations()[selection].second);

				renderingMenuTransform_->clear();
				renderingMenuTransform_.release();
			}
		}
	}
}

void GenericCloudPlacementsExperience::startRelocalization(const std::string& jsonConfiguration)
{
	renderingTextState_->setText("Starting relocalization...");

	if (Devices::ObjectTrackerRef objectTracker = cloudRelocalizerTracker6DOF_)
	{
		const Devices::Tracker::ObjectId anchorObjectId = objectTracker->registerObject(jsonConfiguration);

		if (anchorObjectId != Devices::Tracker::invalidObjectId())
		{
			if (cloudRelocalizerTracker6DOF_->start())
			{
				mainAnchorObjectId_ = anchorObjectId;

				return;
			}
		}
	}

	Log::error() << "Generic Cloud Placements Experience could not start tracker";
}

std::unique_ptr<XRPlaygroundExperience> GenericCloudPlacementsExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new GenericCloudPlacementsExperience());
}

void GenericCloudPlacementsExperience::onTrackerObjectEvent(const Devices::Tracker* sender, const bool found, const Devices::Measurement::ObjectIdSet& objectIds, const Timestamp& timestamp)
{
	if (found)
	{
		renderingTextState_->setText("Received " + String::toAString(objectIds.size()) + " placements");

		for (const Devices::Tracker::ObjectId& objectId : objectIds)
		{
			if (objectId == mainAnchorObjectId_)
			{
				// we do not handle the main object of the tracker
				continue;
			}

			std::string description = sender->objectDescription(objectId);
			ocean_assert(!description.empty());

			ocean_assert(renderingCoordinateSystem_);

			constexpr Scalar visibilityRadius = Scalar(1000); // 1km
			constexpr Scalar engagementRadius = Scalar(10000);

			const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(renderingCoordinateSystem_, cloudRelocalizerTracker6DOF_, objectId, visibilityRadius, engagementRadius);

			if (contentId != AnchoredContentManager::invalidContentId())
			{
				Log::info() << "Anchored placement with fbid: " << description;
			}
			else
			{
				Log::error() << "Failed to anchor placement with fbid: " << description;
			}
		}
	}
	else
	{
		Log::info() << "Lost " << objectIds.size() << " objects - we do not handle this case";
	}
}

void GenericCloudPlacementsExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

std::vector<std::pair<std::string, std::string>> GenericCloudPlacementsExperience::selectableConfigurations()
{
	std::vector<std::pair<std::string, std::string>> configurations =
	{
		{"DINO with spatial anchor discovery", "{\"releaseEnvironment\":\"DINO\", \"mode\":\"SpatialAnchorDiscovery\", \"coordinateProviderLevel\":\"l1\"}"}
	};

	return configurations;
}

}

}
