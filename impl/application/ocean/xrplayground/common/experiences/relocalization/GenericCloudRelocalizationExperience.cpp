// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/GenericCloudRelocalizationExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Delaunay.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace XRPlayground
{

GenericCloudRelocalizationExperience::~GenericCloudRelocalizationExperience()
{
	// nothing to do here
}

bool GenericCloudRelocalizationExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	ocean_assert(engine);

	cloudRelocalizerTracker6DOF_ = Devices::Manager::get().device("Cloud Relocalizer 6DOF Tracker");

	if (!cloudRelocalizerTracker6DOF_)
	{
		Log::error() << "Generic Cloud Reloc Experience could not access cloud reloc tracker";
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

	cloudRelocTrackerSampleSubscription_ = cloudRelocalizerTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &GenericCloudRelocalizationExperience::onCloudRelocTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&GenericCloudRelocalizationExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef renderingAbsoluteTransformState = engine->factory().createAbsoluteTransform();
	renderingAbsoluteTransformState->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
	renderingAbsoluteTransformState->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(renderingAbsoluteTransformState);

	renderingAbsoluteTransformState->addChild(Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, "", "", &renderingTextState_));

	if (!properties.empty())
	{
		Tracking::Cloud::CloudRelocalizer::Configuration configuration;
		if (Tracking::Cloud::CloudRelocalizer::Configuration::parseConfiguration(properties, configuration))
		{
			Log::info() << "Starting relocalization with configuration '" << properties << "'";

			startRelocalization(properties);

			return true;
		}
		else
		{
			Log::error() << "Failed to parse cloud configuration '" << properties << "'";
		}
	}

	renderingTextState_->setText("Select the release environment...");

	const std::vector<std::pair<std::string, std::string>> configurations(selectableConfigurations());

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)

	// on iOS we use the Apple's UI functionality to show a menu
	showReleaseEnvironmentsIOS(userInterface, configurations);

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

bool GenericCloudRelocalizationExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	cloudRelocTrackerSampleSubscription_.release();

	renderingTextState_.release();
	renderingMenuTransform_.release();

	anchoredContentManager_.release();

	cloudRelocalizerTracker6DOF_.release();

	return true;
}

Timestamp GenericCloudRelocalizationExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!cloudRelocalizationVisualizationCreated_)
	{
		TemporaryScopedLock scopedLock(lock_);

		if (!firstRelocalizationObjectPoints_.empty())
		{
			const Vectors3 objectPoints = std::move(firstRelocalizationObjectPoints_);
			const Vectors2 imagePoints = std::move(firstRelocalizationImagePoints_);

			cloudRelocalizationVisualizationCreated_ = true;

			scopedLock.release();

			if (!createAndAnchorMesh(engine, objectPoints, imagePoints))
			{
				Log::error() << "Failed to create mesh visualization for relocalization result";
			}
		}
	}

	if (anchorObjectId_ != Devices::Tracker::invalidObjectId())
	{
		TemporaryScopedLock scopedLock(lock_);
			const Timestamp lastSuccessfulRelocalizationTimestamp(lastSuccessfulRelocalizationTimestamp_);
			const Timestamp lastFailedRelocalizationTimestamp(lastFailedRelocalizationTimestamp_);
			const float lastScale(lastScale_);
			const int numberFeaturesFrameToFrameTracking(numberFeaturesFrameToFrameTracking_);
			const Timestamp latestSampleTimestamp(latestSampleTimestamp_);
		scopedLock.release();

		std::string stateString;

		if (lastFailedRelocalizationTimestamp > lastSuccessfulRelocalizationTimestamp)
		{
			stateString = "reloc failed " + String::toAString(double(latestSampleTimestamp - lastFailedRelocalizationTimestamp), 1u) + " seconds ago";
		}
		else if (lastSuccessfulRelocalizationTimestamp.isValid())
		{
			stateString = "reloc succeeded " + String::toAString(double(latestSampleTimestamp - lastSuccessfulRelocalizationTimestamp), 1u) + " seconds ago";

			if (lastScale > 0.0f)
			{
				stateString += "\nscale: " + String::toAString(lastScale, 2u);
			}

			if (numberFeaturesFrameToFrameTracking > 0)
			{
				stateString += "\nf2f features: " + String::toAString(numberFeaturesFrameToFrameTracking);
			}
		}

		renderingTextState_->setText(stateString);
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

void GenericCloudRelocalizationExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
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

void GenericCloudRelocalizationExperience::startRelocalization(const std::string& jsonConfiguration)
{
	renderingTextState_->setText("Starting relocalization...");

	if (Devices::ObjectTrackerRef objectTracker = cloudRelocalizerTracker6DOF_)
	{
		anchorObjectId_ = objectTracker->registerObject(jsonConfiguration);
	}

	if (anchorObjectId_ != Devices::Tracker::invalidObjectId())
	{
		if (!cloudRelocalizerTracker6DOF_->start())
		{
			Log::error() << "Generic Cloud Relocalizer Experience could not start cloud tracker";
		}
	}
	else
	{
		Log::error() << "Failed to request cloud relocalization for object '" << jsonConfiguration << "'";
	}
}

std::unique_ptr<XRPlaygroundExperience> GenericCloudRelocalizationExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new GenericCloudRelocalizationExperience());
}

bool GenericCloudRelocalizationExperience::createAndAnchorMesh(const Rendering::EngineRef& engine, const Vectors3& objectPoints, const Vectors2& imagePoints)
{
	ocean_assert(anchorObjectId_ != Devices::Tracker::invalidObjectId());

	ocean_assert(engine);
	ocean_assert(objectPoints.size() >= 1);
	ocean_assert(objectPoints.size() == imagePoints.size());

	if (objectPoints.size() != imagePoints.size())
	{
		return false;
	}

	const Geometry::Delaunay::IndexTriangles indexTriangles = Geometry::Delaunay::triangulation(imagePoints);

	if (indexTriangles.empty())
	{
		return false;
	}

	constexpr Scalar radiusSphere = Scalar(0.08); // 8cm
	constexpr Scalar radiusCylinder = radiusSphere * Scalar(0.5);

	Rendering::MaterialRef sphereMaterial;
	Rendering::GeometryRef sphereGeometry;
	Rendering::Utilities::createSphere(engine, radiusSphere, RGBAColor(0.0f, 1.0f, 0.0f), nullptr, nullptr, &sphereMaterial, &sphereGeometry);
	if (!sphereMaterial || !sphereGeometry)
	{
		return false;
	}

	sphereMaterial->setEmissiveColor(RGBAColor(0.0f, 0.1f, 0.0f));

	Rendering::GroupRef group = engine->factory().createGroup();

	for (const Vector3& objectPoint : objectPoints)
	{
		const Rendering::TransformRef transform = engine->factory().createTransform();

		transform->setTransformation(HomogenousMatrix4(objectPoint));
		transform->addChild(sphereGeometry);

		group->addChild(transform);
	}

	Rendering::MaterialRef cylinderMaterial;
	Rendering::GeometryRef cylinderGeometry;
	Rendering::Utilities::createCylinder(engine, radiusCylinder, Scalar(1), RGBAColor(0.0f, 1.0f, 0.0f), nullptr, nullptr, &cylinderMaterial, &cylinderGeometry);
	if (!cylinderMaterial || !cylinderGeometry)
	{
		return false;
	}

	cylinderMaterial->setEmissiveColor(RGBAColor(0.0f, 0.1f, 0.0f));

	std::set<IndexPair32> existingEdges;

	for (const Geometry::Delaunay::IndexTriangle& indexTriangle : indexTriangles)
	{
		for (unsigned int edgeIndex = 0u; edgeIndex < 3u; ++edgeIndex)
		{
			unsigned int indexA = indexTriangle[edgeIndex];
			unsigned int indexB = indexTriangle[(edgeIndex + 1u) % 3u];
			Utilities::sortLowestToFront2(indexA, indexB);

			if (existingEdges.find(IndexPair32(indexA, indexB)) != existingEdges.cend())
			{
				// we have handled this edge already due to a neighboring triangles
				continue;
			}

			const Vector3 direction = objectPoints[indexB] - objectPoints[indexA];
			const Scalar length = direction.length();

			if (Numeric::isEqualEps(length))
			{
				continue;
			}

			const Vector3 translation = (objectPoints[indexA] + objectPoints[indexB]) * Scalar(0.5);
			const Rotation rotation(Vector3(0, 1, 0), direction / length); // the default cylinder is defined arount he y-axis
			const Vector3 scale = Vector3(1, length, 1);

			const Rendering::TransformRef transform = engine->factory().createTransform();

			transform->setTransformation(HomogenousMatrix4(translation, rotation, scale));
			transform->addChild(cylinderGeometry);

			group->addChild(transform);

			existingEdges.insert(IndexPair32(indexA, indexB));
		}
	}

	constexpr Scalar visibilityRadius = Scalar(1000); // 1km
	constexpr Scalar engagementRadius = Scalar(10000);

	const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(group, cloudRelocalizerTracker6DOF_, anchorObjectId_, visibilityRadius, engagementRadius);

	return contentId != AnchoredContentManager::invalidContentId();
}

void GenericCloudRelocalizationExperience::onCloudRelocTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const ScopedLock scopedLock(lock_);

	Devices::SceneTracker6DOF::Metadata::const_iterator iMetadata = sample->metadata().find("lastSuccessfulRelocalizationTimestamp");
	if (iMetadata != sample->metadata().cend())
	{
		lastSuccessfulRelocalizationTimestamp_ = iMetadata->second.float64Value();

		if (delayUntilFirstSuccessfulRelocalization_ < 0.0)
		{
			iMetadata = sample->metadata().find("firstRelocalizationTimestamp");
			if (iMetadata != sample->metadata().cend())
			{
				const Timestamp firstRelocalizationTimestamp(iMetadata->second.float64Value());

				delayUntilFirstSuccessfulRelocalization_ = double(sample->timestamp() - firstRelocalizationTimestamp);
				ocean_assert(delayUntilFirstSuccessfulRelocalization_ >= 0.0);

				Log::info() << "Generic Cloud Relocalization: Time to first successful relocalization: " << delayUntilFirstSuccessfulRelocalization_ << " seconds";
			}
		}
	}
	else
	{
		lastSuccessfulRelocalizationTimestamp_.toInvalid();
	}

	iMetadata = sample->metadata().find("lastFailedRelocalizationTimestamp");
	if (iMetadata != sample->metadata().cend())
	{
		lastFailedRelocalizationTimestamp_ = iMetadata->second.float64Value();
	}
	else
	{
		lastFailedRelocalizationTimestamp_.toInvalid();
	}

	iMetadata = sample->metadata().find("anchorWorldScale");
	if (iMetadata != sample->metadata().cend())
	{
		lastScale_ = iMetadata->second.floatValue();
	}
	else
	{
		lastScale_ = -1.0f;
	}

	iMetadata = sample->metadata().find("numberFeaturesFrameToFrameTracking");
	if (iMetadata != sample->metadata().cend())
	{
		numberFeaturesFrameToFrameTracking_ = iMetadata->second.intValue();
	}
	else
	{
		numberFeaturesFrameToFrameTracking_ = -1;
	}

	latestSampleTimestamp_ = sample->timestamp();

	if (cloudRelocalizationVisualizationCreated_)
	{
		return;
	}

	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (sceneTrackerSample && !sceneTrackerSample->sceneElements().empty())
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements().front();

		if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_FEATURE_CORRESPONDENCES)
		{
			const Devices::SceneTracker6DOF::SceneElementFeatureCorrespondences& featureCorrespondences = dynamic_cast<Devices::SceneTracker6DOF::SceneElementFeatureCorrespondences&>(*sceneElement);

			firstRelocalizationObjectPoints_ = featureCorrespondences.objectPoints();
			firstRelocalizationImagePoints_ = featureCorrespondences.imagePoints();
		}
	}
}

void GenericCloudRelocalizationExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

std::vector<std::pair<std::string, std::string>> GenericCloudRelocalizationExperience::selectableConfigurations()
{
	std::vector<std::pair<std::string, std::string>> configurations =
	{
		{"Production", "{\"releaseEnvironment\":\"Production\"}"},
		{"Staging", "{\"releaseEnvironment\":\"Staging\"}"},
		{"Testing", "{\"releaseEnvironment\":\"Testing\"}"},
		{"DINO", "{\"releaseEnvironment\":\"DINO\"}"},
		{"TEST_MPK20", "{\"releaseEnvironment\":\"TEST_MPK20\"}"},
		{"BYSM", "{\"releaseEnvironment\":\"BYSM\"}"}
	};

	return configurations;
}

}

}
