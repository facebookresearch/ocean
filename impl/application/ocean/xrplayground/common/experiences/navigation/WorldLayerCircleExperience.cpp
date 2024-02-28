// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/WorldLayerCircleExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/JSONConfig.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

WorldLayerCircleExperience::WorldLayerCircleExperience()
{
	// nothing to do here
}

WorldLayerCircleExperience::~WorldLayerCircleExperience()
{
	// nothing to do here
}

bool WorldLayerCircleExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	if (anchoredContentManager_)
	{
		Log::warning() << "The experience is already loaded";
		return false;
	}

	gpsTracker_ = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

	if (!gpsTracker_)
	{
		Log::error() << "WorldLayerCircle Experience could not access a GPS tracker";
		return false;
	}

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	anchorTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Geo Anchors Tracker");
#endif

	if (!anchorTracker6DOF_)
	{
		// on Android, or as backup if ARKit's Geo Anchors are not available in the current location
		anchorTracker6DOF_ = Devices::Manager::get().device("GeoAnchor 6DOF Tracker");
	}

	if (!anchorTracker6DOF_)
	{
		Log::error() << "WorldLayerCircle Experience could not access anchor tracker";
		return false;
	}

	if (!gpsTracker_->start())
	{
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(anchorTracker6DOF_);
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

	if (!anchorTracker6DOF_->start())
	{
		return false;
	}

	if (!anchoredContentManager_.initialize(std::bind(&WorldLayerCircleExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	if (anchorTracker6DOF_->name() != "ARKit 6DOF Geo Anchors Tracker")
	{
		const Rendering::AbsoluteTransformRef absoluteTransformation = engine->factory().createAbsoluteTransform();
		ocean_assert(absoluteTransformation);

		absoluteTransformation->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
		absoluteTransformation->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));

		const Rendering::TransformRef text = Rendering::Utilities::createText(*engine, "ARKit's Geo Anchors not available", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0.005, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
		absoluteTransformation->addChild(text);
		experienceScene()->addChild(absoluteTransformation);
	}

	renderingSharedTransformModifier_ = Rendering::Transform::createTransformModifier();

	return true;
}

bool WorldLayerCircleExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	anchoredContentManager_.release();

	renderingSharedTransformModifier_ = nullptr;

	return true;
}

Timestamp WorldLayerCircleExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	constexpr double queryInterval = 30.0; // 30 seconds

	// first we check whether we have to remove content which we have added previously
	// content is removed when the content left the radius of engagement

	TemporaryScopedLock temporaryScopedLock(removedObjectsLock_);
		const AnchoredContentManager::SharedContentObjectSet removedObjects(std::move(removedObjects_));
	temporaryScopedLock.release();

	removeContent(removedObjects);

	if (futureGraphQLResponse_.valid() && futureGraphQLResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		// we have a new GraphQL response

		const Network::Tigon::TigonRequest::TigonResponse graphQLResponse(futureGraphQLResponse_.get());

		if (graphQLResponse.succeeded())
		{
			WorldLayerRenderables worldLayerRenderables;
			if (parseWorldLayerCircleResponse(graphQLResponse.response(), worldLayerRenderables))
			{
				addContent(*engine, worldLayerRenderables);
			}

			nextGraphQLQueryTimestamp_ = timestamp + queryInterval;
		}
		else
		{
			Log::warning() << "GraphQL request failed: " << graphQLResponse.error();

			nextGraphQLQueryTimestamp_ = timestamp;
		}

		futureGraphQLResponse_ = std::future<Network::Tigon::TigonRequest::TigonResponse>();
		ocean_assert(!futureGraphQLResponse_.valid());
	}

	if (nextGraphQLQueryTimestamp_.isInvalid() || timestamp >= nextGraphQLQueryTimestamp_)
	{
		ocean_assert(!futureGraphQLResponse_.valid());

		Devices::GPSTracker::GPSTrackerSampleRef sample(gpsTracker_->sample()); // accessing the latest GPS sample available

		if (sample && !sample->objectIds().empty())
		{
			if (invokeGraphQLRequest(sample->locations().front()))
			{
				nextGraphQLQueryTimestamp_ = timestamp + queryInterval;
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	// a rotation around the y-axis based on the timestamp (one rotation in ~6.2 seconds)
	const HomogenousMatrix4 rotationAnimationTransformation(Quaternion(Vector3(0, 1, 0), -Scalar(NumericD::angleAdjustNull(double(timestamp)))));

	ocean_assert(renderingSharedTransformModifier_);
	renderingSharedTransformModifier_->setTransformation(rotationAnimationTransformation);

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

std::unique_ptr<XRPlaygroundExperience> WorldLayerCircleExperience::WorldLayerCircleExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new WorldLayerCircleExperience());
}

bool WorldLayerCircleExperience::invokeGraphQLRequest(const Devices::GPSTracker::Location& location)
{
	// invoking a WorldLayerCircle query https://fburl.com/graphiql/6u2nt8nz

	std::string parameters = R"_REQUEST_(
	{
		"query_params": {
			"location": {
				"longitude": <LONGITUDE>,
				"latitude": <LATITUDE>
			},
			"radius_meters": 100,
			"max_results": 100,
			"layers": [
				{
				"type": "MAPS"
				},
				{
				"type": "PLACES"
				}
			]
		}
	})_REQUEST_";

	parameters = String::replace(std::move(parameters), "<LONGITUDE>", String::toAString(location.longitude(), 10u), true);
	parameters = String::replace(std::move(parameters), "<LATITUDE>", String::toAString(location.latitude(), 10u), true);

	ocean_assert(!futureGraphQLResponse_.valid());
	futureGraphQLResponse_ = Network::Tigon::TigonClient::get().graphQLRequest("5207491415988790", true, parameters);

	return futureGraphQLResponse_.valid();
}

void WorldLayerCircleExperience::addContent(const Rendering::Engine& engine, const WorldLayerRenderables& worldLayerRenderables)
{
	ocean_assert(anchoredContentManager_);

	const Devices::ObjectTrackerRef objectTracker(anchorTracker6DOF_);

	for (const WorldLayerRenderable& worldLayerRenderable : worldLayerRenderables)
	{
		if (worldLayerCircleIdSet_.find(worldLayerRenderable.worldLayerCircleId_) != worldLayerCircleIdSet_.cend())
		{
			// this world layer renderable is not new
			continue;
		}

		const Rendering::TransformRef renderingTextTransform = createRenderingObject(engine, worldLayerRenderable);

		if (!renderingTextTransform)
		{
			// this world layer renderable is currently not supported
			continue;
		}

		const std::string trackerObjectDescription = "GPS Location " + String::toAString(worldLayerRenderable.latitude_, 10u) + ", " + String::toAString(worldLayerRenderable.longitude_, 10u);

		const Devices::ObjectTracker::ObjectId trackerObjectId = objectTracker->registerObject(trackerObjectDescription);

		if (trackerObjectId == Devices::ObjectTracker::invalidObjectId())
		{
			continue;
		}

		// we may need to place several objects at the same location, therefore lifting the content with each now object in z-direction

		unsigned int& usageCounter = objectIdToCounterMap_[trackerObjectId];
		renderingTextTransform->setTransformation(renderingTextTransform->transformation() * HomogenousMatrix4(Vector3(0, Scalar(usageCounter) * Scalar(0.75), 0)));
		++usageCounter;

#ifdef OCEAN_DEBUG
		constexpr Scalar visibilityRadius = Scalar(100); // in meter
		constexpr Scalar engagementRadius = Scalar(200);
#else
		constexpr Scalar visibilityRadius = Scalar(50);
		constexpr Scalar engagementRadius = Scalar(200);
#endif

		const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(renderingTextTransform, anchorTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);

		if (contentId != AnchoredContentManager::invalidContentId())
		{
			ocean_assert(worldLayerCircleIdSet_.find(worldLayerRenderable.worldLayerCircleId_) == worldLayerCircleIdSet_.cend());
			worldLayerCircleIdSet_.insert(worldLayerRenderable.worldLayerCircleId_);

			ocean_assert(contentIdToWorldLayerCircleIdMap_.find(contentId) == contentIdToWorldLayerCircleIdMap_.cend());
			contentIdToWorldLayerCircleIdMap_.emplace(contentId, worldLayerRenderable.worldLayerCircleId_);
		}
	}
}

void WorldLayerCircleExperience::removeContent(const AnchoredContentManager::SharedContentObjectSet& contentObjects)
{
	ocean_assert(anchoredContentManager_);

	for (const AnchoredContentManager::SharedContentObject& contentObject : contentObjects)
	{
		ocean_assert(contentObject);
		const AnchoredContentManager::ContentId contentId = contentObject->contentId();

		ContentIdToWorldLayerCircleIdMap::const_iterator iWorldLayerCircle = contentIdToWorldLayerCircleIdMap_.find(contentId);

		if (iWorldLayerCircle != contentIdToWorldLayerCircleIdMap_.cend())
		{
			const std::string& worldLayerCircleId = iWorldLayerCircle->second;

			ocean_assert(worldLayerCircleIdSet_.find(worldLayerCircleId) != worldLayerCircleIdSet_.cend());
			worldLayerCircleIdSet_.erase(worldLayerCircleId);

			contentIdToWorldLayerCircleIdMap_.erase(iWorldLayerCircle);
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}
}

Rendering::TransformRef WorldLayerCircleExperience::createRenderingObject(const Rendering::Engine& engine, const WorldLayerRenderable& worldLayerRenderable) const
{
	if (worldLayerRenderable.text_.empty())
	{
		ocean_assert(false && "This should never happen!");
		return Rendering::TransformRef();
	}

	if (worldLayerRenderable.renderableType_ == WorldLayerRenderable::RT_STREET)
	{
		const RGBAColor foregroundColor(0.9f, 0.9f, 0.9f);
		const RGBAColor backgroundColor(0.0f, 0.4f, 0.0f);

		constexpr Scalar fixedLineHeight = Scalar(0.5);
		constexpr bool shaded = false;

		const std::string& text = worldLayerRenderable.text_;
		const double& heading = worldLayerRenderable.heading_;

		ocean_assert(!text.empty());

		const Scalar headingRotationAngle = NumericD::deg2rad(heading);

		const Rendering::TransformRef textTransform = Rendering::Utilities::createText(engine, "  " + text + " ", foregroundColor, backgroundColor, shaded, 0 /*fixedWidth*/, 0 /*fixedHeight*/, fixedLineHeight, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_BOTTOM);
		ocean_assert(textTransform);

		const Quaternion headingRotation(Vector3(0, 1, 0), -headingRotationAngle + Numeric::pi_2());
		textTransform->setTransformation(HomogenousMatrix4(headingRotation));

		return textTransform;
	}
	else if (worldLayerRenderable.renderableType_ == WorldLayerRenderable::RT_PLACE)
	{
		const RGBAColor foregroundColor(0.0f, 0.0f, 0.0f);
		const RGBAColor backgroundColor(0.6f, 0.6f, 0.6f);

		constexpr Scalar fixedLineHeight = Scalar(0.5);

		const Rendering::TextRef text = engine.factory().createText();
		text->setSize(0 /*fixedWidth*/, 0 /*fixedHeight*/, fixedLineHeight);

		std::string textString = "  ---  " + worldLayerRenderable.text_;
		text->setText(textString);

		Vector2 textBlockSize = text->size();
		ocean_assert(!textBlockSize.isNull());

		Scalar radius = textBlockSize.x() / Numeric::pi2();

		while (radius < Scalar(1)) // ensuring that the text ring has a radius of at least 1 meter
		{
			textString += "  ---  " + worldLayerRenderable.text_;

			text->setText(textString);
			textBlockSize = text->size();
			ocean_assert(!textBlockSize.isNull());

			radius = textBlockSize.x() / Numeric::pi2();
		};

		// we create a geometry lookup table to create a text ring

		LookupCorner2<Vector3> geometryLookupTable(100, 1, 100, 1);

		for (unsigned int binX = 0u; binX <= geometryLookupTable.binsX(); ++binX)
		{
			const Scalar angle = Scalar(binX) / Scalar(geometryLookupTable.binsX()) * Numeric::pi2();

			const Scalar x = Numeric::sin(angle) * radius;
			const Scalar z = Numeric::cos(angle) * radius;

			geometryLookupTable.setBinTopLeftCornerValue(binX, 0u, Vector3(x, textBlockSize.y(), z));
			geometryLookupTable.setBinTopLeftCornerValue(binX, 1u, Vector3(x, 0, z));
		}

		text->setGeometryLookupTable(geometryLookupTable);

		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef foregroundMaterial = engine.factory().createMaterial();
		foregroundMaterial->setDiffuseColor(foregroundColor);
		attributeSet->addAttribute(foregroundMaterial);

		const Rendering::MaterialRef backgroundMaterial = engine.factory().createMaterial();
		backgroundMaterial->setDiffuseColor(backgroundColor);

		text->setBackgroundMaterial(backgroundMaterial);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();

		geometry->addRenderable(text, attributeSet);

		const Rendering::TransformRef textTransform = engine.factory().createTransform();

		textTransform->addChild(geometry);
		textTransform->setTransformModifier(renderingSharedTransformModifier_);

		return textTransform;
	}

	ocean_assert(false && "This should never happen!");
	return Rendering::TransformRef();
}

void WorldLayerCircleExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(!removedObjects.empty());

	const ScopedLock scopedLock(removedObjectsLock_);

	if (removedObjects_.empty())
	{
		removedObjects_ = std::move(removedObjects);
	}
	else
	{
		removedObjects_.insert(removedObjects.cbegin(), removedObjects.cend());
	}
}

bool WorldLayerCircleExperience::parseWorldLayerCircleResponse(const std::string& response, WorldLayerRenderables& worldLayerRenderables)
{
	// parsing the WorldLayerCircle query response https://fburl.com/graphiql/6u2nt8nz

	IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(response)));

	if (!config["data"].exist("world_layer_circle_root_query"))
	{
		return false;
	}

	IO::JSONConfig::JSONValue& rootQuery = config["data"]["world_layer_circle_root_query"];

	const unsigned int numberRenderables = rootQuery.values("renderables");

	worldLayerRenderables.clear();
	worldLayerRenderables.reserve(numberRenderables);

	for (unsigned int nRenderable = 0u; nRenderable < numberRenderables; ++nRenderable)
	{
		IO::JSONConfig::JSONValue& renderable = rootQuery.value("renderables", nRenderable);

		std::string worldLayerCircleId = renderable["unique_id"](std::string());

		if (worldLayerCircleId.empty())
		{
			continue;
		}

		const std::string type = renderable["asset"]["type"](std::string());

		if (type.empty())
		{
			continue;
		}

		const double latitude = renderable["placement"]["geoanchor"]["coordinate"]["latitude"](NumericD::minValue());
		const double longitude = renderable["placement"]["geoanchor"]["coordinate"]["longitude"](NumericD::minValue());

		const double headingOffset = renderable["position"]["heading_offset"](NumericD::minValue());

		if (latitude == NumericD::minValue() || longitude == NumericD::minValue())
		{
			continue;
		}

		std::string text = renderable["text"]["text"](std::string());

		if (type.empty() || text.empty())
		{
			continue;
		}

		const double heading = headingOffset == NumericD::minValue() ? 0.0 : headingOffset;

		WorldLayerRenderable::RenderableType renderableType = WorldLayerRenderable::RT_UNKNOWN;

		if (type == "street_sign")
		{
			renderableType = WorldLayerRenderable::RT_STREET;
		}
		else if (type == "place_card")
		{
			renderableType = WorldLayerRenderable::RT_PLACE;
		}

		if (renderableType != WorldLayerRenderable::RT_UNKNOWN)
		{
			worldLayerRenderables.emplace_back(std::move(worldLayerCircleId), renderableType, std::move(text), latitude, longitude, heading);
		}
		else
		{
			Log::warning() << "Skipped renderable '" << type << "'";
		}
	}

	return true;
}

}

}
