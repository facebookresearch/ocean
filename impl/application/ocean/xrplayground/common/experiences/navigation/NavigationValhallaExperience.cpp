// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/NavigationValhallaExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/JSONConfig.h"

#include "ocean/math/Random.h"
#include "ocean/math/Sphere3.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

NavigationValhallaExperience::NavigationValhallaExperience(const ExperienceMode experienceMode) :
	experienceMode_(experienceMode)
{
	// nothing to do here
}

NavigationValhallaExperience::~NavigationValhallaExperience()
{
	// nothing to do here
}

bool NavigationValhallaExperience::setTargetLocation(const Devices::GPSTracker::Location& targetLocation)
{
	if (experienceMode_ != EM_CUSTOM_TARGET)
	{
		ocean_assert(false && "The experience is in the wrong mode!");
		return false;
	}

	if (!targetLocation.isValid())
	{
		return false;
	}

	if (customTargetLocation_.isValid())
	{
		// **TODO** we need to restart the navigation for the new target location

		ocean_assert(false && "Missing implementation!");
		return false;
	}

	customTargetLocation_ = targetLocation;
	return true;
}

bool NavigationValhallaExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
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
		Log::error() << "NavigationValhalla Experience could not access a GPS tracker";
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
		Log::error() << "NavigationValhalla Experience could not access anchor tracker";
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

	if (!anchoredContentManager_.initialize(std::bind(&NavigationValhallaExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingSharedTransformModifier_ = Rendering::Transform::createTransformModifier();

	renderingArrow_ = Rendering::Utilities::createArrow(engine, Scalar(0.05), Scalar(0.015), Scalar(0.005), RGBAColor(0.7f, 0.0f, 0.0f));
	renderingArrow_->setVisible(false);

	experienceScene()->addChild(renderingArrow_);

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

	return true;
}

bool NavigationValhallaExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingBoxPassive_.release();
	renderingBoxActive_.release();
	renderingArrow_.release();

	anchoredContentManager_.release();

	renderingSharedTransformModifier_ = nullptr;

	return true;
}

Timestamp NavigationValhallaExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(gpsTracker_);

	if (!activeTargetLocation_.isValid())
	{
		Devices::GPSTracker::GPSTrackerSampleRef sample(gpsTracker_->sample()); // accessing the latest GPS sample available

		if (sample && !sample->objectIds().empty())
		{
			const Devices::GPSTracker::Location& currentLocation = sample->locations().front();
			ocean_assert(currentLocation.isValid());

			if (experienceMode_ == EM_RANDOM_TARGET)
			{
				// let's create a random target location with fixed distance to the current location

				constexpr Scalar distance = 1000; // in meter

				activeTargetLocation_ = createRandomLocation(currentLocation, distance);
				ocean_assert(activeTargetLocation_.isValid());
			}
			else
			{
				ocean_assert(experienceMode_ == EM_CUSTOM_TARGET);

				if (customTargetLocation_.isValid())
				{
					activeTargetLocation_ = customTargetLocation_;
				}
			}

			if (activeTargetLocation_.isValid())
			{
				invokeGraphQLRequest(currentLocation, activeTargetLocation_);
			}
		}
	}

	if (futureGraphQLResponse_.valid() && futureGraphQLResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		// we have a new GraphQL response

		const Network::Tigon::TigonRequest::TigonResponse graphQLResponse(futureGraphQLResponse_.get());

		if (graphQLResponse.succeeded())
		{
			NavigationWaypoints navigationWaypoints;
			if (parseNavigationValhallaResponse(graphQLResponse.response(), navigationWaypoints))
			{
				addContent(*engine, navigationWaypoints);
			}
		}
		else
		{
			Log::warning() << "GraphQL request failed: " << graphQLResponse.error();

			// we reset the target location
			activeTargetLocation_ = Devices::GPSTracker::Location();
		}

		futureGraphQLResponse_ = std::future<Network::Tigon::TigonRequest::TigonResponse>();
		ocean_assert(!futureGraphQLResponse_.valid());
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	// a rotation around the y-axis based on the timestamp (one rotation in ~3.1 seconds)
	const HomogenousMatrix4 rotationAnimationTransformation(Quaternion(Vector3(0, 1, 0), Scalar(NumericD::angleAdjustNull(double(timestamp) * 2.0))));

	ocean_assert(renderingSharedTransformModifier_);
	renderingSharedTransformModifier_->setTransformation(rotationAnimationTransformation);

	const Timestamp updatedTimestamp = anchoredContentManager_.preUpdate(engine, view, timestamp);

	constexpr Scalar maxSqrDistance = Numeric::sqr(5); // meter

	const AnchoredContentManager::SharedContentObjectSet closeContents = anchoredContentManager_.closeContents(maxSqrDistance);

	for (const AnchoredContentManager::SharedContentObject& contentObject : closeContents)
	{
		ocean_assert(contentObject);

		const ContentIdSet::const_iterator iCurrentContent = notYetVisitedWaypointContentIdSet_.find(contentObject->contentId());

		if (iCurrentContent != notYetVisitedWaypointContentIdSet_.cend())
		{
			// this is the first time we are close to the object, so we can "disable" this content (and all content objects with smaller id)

			for (ContentIdSet::iterator iContentNotYetVisited = notYetVisitedWaypointContentIdSet_.begin(); iContentNotYetVisited != notYetVisitedWaypointContentIdSet_.cend(); /* noop*/)
			{
				const AnchoredContentManager::ContentId contentId = *iContentNotYetVisited;

				iContentNotYetVisited = notYetVisitedWaypointContentIdSet_.erase(iContentNotYetVisited);

				const AnchoredContentManager::SharedContentObject object = anchoredContentManager_.content(contentId);
				ocean_assert(object);

				if (object)
				{
					object->setRenderingObjectNode(renderingBoxPassive_);
				}

				if (contentId == *iCurrentContent)
				{
					// we have reach our current object
					break;
				}
			}
		}
	}

	ocean_assert(renderingArrow_);
	bool showArrow = false;

	if (!notYetVisitedWaypointContentIdSet_.empty())
	{
		// we place the arrow so that it points towards the first waypoint which we have not yet visited
		const AnchoredContentManager::SharedContentObject object = anchoredContentManager_.content(*notYetVisitedWaypointContentIdSet_.cbegin());
		ocean_assert(object);

		if (object && object->isTracked())
		{
			const HomogenousMatrix4 world_T_object = object->world_T_object();
			const HomogenousMatrix4 world_T_view = view->transformation();

			Vector3 direction = world_T_object.translation() - world_T_view.translation();

			if (direction.normalize())
			{
				const HomogenousMatrix4 world_T_arrow = world_T_view * HomogenousMatrix4(Vector3(0, -0.075, -0.2)) * HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), direction)) * HomogenousMatrix4(Vector3(0, -0.025, 0));

				renderingArrow_->setTransformation(world_T_arrow);

				showArrow = true;
			}
		}
	}

	renderingArrow_->setVisible(showArrow);

	return updatedTimestamp;
}

std::unique_ptr<NavigationValhallaExperience> NavigationValhallaExperience::createExperience(const ExperienceMode experienceMode)
{
    return std::unique_ptr<NavigationValhallaExperience>(new NavigationValhallaExperience(experienceMode));
}

bool NavigationValhallaExperience::invokeGraphQLRequest(const Devices::GPSTracker::Location& startLocation, const Devices::GPSTracker::Location& targetLocation)
{
	// invoking a WorldLayerCircle query https://fburl.com/graphiql/6u2nt8nz

	std::string parameters = R"_REQUEST_(
	{
		"query_params": {
			"locations": [
				{
					"latitude": <START_LATITUDE>,
					"longitude": <START_LONGITUDE>,
					"type": "BREAK"
				},
				{
					"latitude": <TARGET_LATITUDE>,
					"longitude": <TARGET_LONGITUDE>,
					"type": "BREAK"
				}
			],
			"costing": "PEDESTRIAN",
			"directions_options": {
				"language": "EN_US",
				"units": "KILOMETERS"
			}
		}
	})_REQUEST_";

	parameters = String::replace(std::move(parameters), "<START_LATITUDE>", String::toAString(startLocation.latitude(), 10u), true);
	parameters = String::replace(std::move(parameters), "<START_LONGITUDE>", String::toAString(startLocation.longitude(), 10u), true);
	parameters = String::replace(std::move(parameters), "<TARGET_LATITUDE>", String::toAString(targetLocation.latitude(), 10u), true);
	parameters = String::replace(std::move(parameters), "<TARGET_LONGITUDE>", String::toAString(targetLocation.longitude(), 10u), true);

	ocean_assert(!futureGraphQLResponse_.valid());
	futureGraphQLResponse_ = Network::Tigon::TigonClient::get().graphQLRequest("4194389007342140", true, parameters);

	return futureGraphQLResponse_.valid();
}

void NavigationValhallaExperience::addContent(const Rendering::Engine& engine, const NavigationWaypoints& navigationWaypoints)
{
	ocean_assert(anchoredContentManager_);

	constexpr Scalar engagementRadius = Scalar(10000);

#ifdef OCEAN_DEBUG
		constexpr Scalar visibilityRadius = Scalar(100); // in meter
#else
		constexpr Scalar visibilityRadius = Scalar(50);
#endif

	if (navigationWaypoints.locations_.empty())
	{
		Log::warning() << "The navigation dos not contain any location";
		return;
	}

	const Devices::ObjectTrackerRef objectTracker(anchorTracker6DOF_);

	for (const Devices::GPSTracker::Location& location : navigationWaypoints.locations_)
	{
		const std::string trackerObjectDescription = "GPS Location " + String::toAString(location.latitude(), 10u) + ", " + String::toAString(location.longitude(), 10u);
		const Devices::ObjectTracker::ObjectId trackerObjectId = objectTracker->registerObject(trackerObjectDescription);

		if (trackerObjectId == Devices::ObjectTracker::invalidObjectId())
		{
			continue;
		}

		const Rendering::TransformRef boxTransform(createRenderingObject(engine, location));
		ocean_assert(boxTransform);

		const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(boxTransform, anchorTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);

		if (contentId != AnchoredContentManager::invalidContentId())
		{
			ocean_assert(notYetVisitedWaypointContentIdSet_.find(contentId) == notYetVisitedWaypointContentIdSet_.cend());
			notYetVisitedWaypointContentIdSet_.emplace(contentId);
		}
	}

	for (const NavigationWaypoints::Maneuver& maneuver : navigationWaypoints.maneuvers_)
	{
		const size_t locationIndex = maneuver.beginShapeIndex_;

		if (locationIndex > navigationWaypoints.locations_.size())
		{
			Log::warning() << "Maneuver '" << maneuver.instruction_ << "' is associated with not existing shape";
			continue;
		}

		const Devices::GPSTracker::Location& location = locationIndex < navigationWaypoints.locations_.size() ? navigationWaypoints.locations_[locationIndex] : navigationWaypoints.locations_.back();

		const std::string trackerObjectDescription = "GPS Location " + String::toAString(location.latitude(), 10u) + ", " + String::toAString(location.longitude(), 10u);
		const Devices::ObjectTracker::ObjectId trackerObjectId = objectTracker->registerObject(trackerObjectDescription);

		if (trackerObjectId == Devices::ObjectTracker::invalidObjectId())
		{
			continue;
		}

		Rendering::TransformRef transform = engine.factory().createTransform();

		Rendering::TransformRef textTransform(createRenderingObject(engine, maneuver, location));
		ocean_assert(textTransform);
		transform->addChild(textTransform);

		textTransform = createRenderingObject(engine, maneuver, location);
		textTransform->setTransformation(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::pi())));
		transform->addChild(textTransform);

		transform->setTransformation(HomogenousMatrix4(Vector3(0, 2, 0))); // lifting by 2 meters

		const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(transform, anchorTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);

		ocean_assert_and_suppress_unused(contentId != AnchoredContentManager::invalidContentId(), contentId);
	}
}

Rendering::TransformRef NavigationValhallaExperience::createRenderingObject(const Rendering::Engine& engine, const Devices::GPSTracker::Location& location)
{
	if (renderingBoxActive_.isNull())
	{
		renderingBoxActive_ = engine.factory().createGeometry();

		const Rendering::BoxRef box = engine.factory().createBox();
		box->setSize(Vector3(Scalar(0.5), Scalar(0.5), Scalar(0.5)));

		const Rendering::MaterialRef materialGreen = engine.factory().createMaterial();
		materialGreen->setDiffuseColor(RGBAColor(1.0f, 0.0f, 0.0f));

		const Rendering::AttributeSetRef attributeSetGreen = engine.factory().createAttributeSet();
		attributeSetGreen->addAttribute(materialGreen);

		renderingBoxActive_->addRenderable(box, attributeSetGreen);

		ocean_assert(renderingBoxPassive_.isNull());
		renderingBoxPassive_ = engine.factory().createGeometry();

		const Rendering::MaterialRef materialTransparent = engine.factory().createMaterial();
		materialTransparent->setDiffuseColor(RGBAColor(0.7f, 0.7f, 0.7f));
		materialTransparent->setTransparency(0.5f);

		const Rendering::AttributeSetRef attributeSetTransparent = engine.factory().createAttributeSet();
		attributeSetTransparent->addAttribute(materialTransparent);
		attributeSetTransparent->addAttribute(engine.factory().createBlendAttribute());

		renderingBoxPassive_->addRenderable(box, attributeSetTransparent);
	}

	ocean_assert(renderingBoxActive_ && renderingBoxPassive_);

	Rendering::TransformRef boxTransform = engine.factory().createTransform();

	boxTransform->addChild(renderingBoxActive_);
	boxTransform->setTransformModifier(renderingSharedTransformModifier_);

	return boxTransform;
}

Rendering::TransformRef NavigationValhallaExperience::createRenderingObject(const Rendering::Engine& engine, const NavigationWaypoints::Maneuver& maneuver, const Devices::GPSTracker::Location& location)
{
	const RGBAColor foregroundColor(0.0f, 0.0f, 0.0f);
	const RGBAColor backgroundColor(0.6f, 0.2f, 0.2f);

	constexpr Scalar fixedLineHeight = Scalar(0.5);
	constexpr bool shaded = true;

	std::string text = maneuver.instruction_;

	const std::string::size_type pos = text.find('.'); // separate two sentences into two lines
	if (pos != std::string::npos && pos != text.size() - 1)
	{
		text = String::replace(std::move(text), ".", ".\n", true /*onlyFirstOccurrence*/);
	}

	Rendering::TransformRef textTransform = Rendering::Utilities::createText(engine, "  " + text + " ", foregroundColor, backgroundColor, shaded, 0 /*fixedWidth*/, 0 /*fixedHeight*/, fixedLineHeight, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_BOTTOM);
	ocean_assert(textTransform);

	return textTransform;
}

void NavigationValhallaExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

bool NavigationValhallaExperience::parseNavigationValhallaResponse(const std::string& response, NavigationWaypoints& navigationWaypoints)
{
	// parsing the WorldLayerCircle query response https://fburl.com/graphiql/x45wfa0w

	IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(response)));

	if (!config["data"].exist("navigation_route_query"))
	{
		return false;
	}

	IO::JSONConfig::JSONValue& rootQuery = config["data"]["navigation_route_query"];

	const unsigned int numberLegs = rootQuery.values("legs");

	if (numberLegs != 1u)
	{
		return false;
	}

	IO::JSONConfig::JSONValue& leg = rootQuery.value("legs", 0u);

	std::string shape = leg["shape"](std::string());
	shape = String::replace(std::move(shape), "\\u0040", "@", false /*onlyFirstOccurrence*/);

	Devices::GPSTracker::Locations locations;
	if (!Devices::GPSTracker::decodePolyline(shape, 6u, locations, true /*unescapeBackslash*/))
	{
		Log::error() << "Received invalid shape data";
		return false;
	}

	const unsigned int numberManeuvers = leg.values("maneuvers");

	if (numberManeuvers > 20000u)
	{
		return false;
	}

	NavigationWaypoints::Maneuvers manuevers;
	manuevers.reserve(numberManeuvers);

	for (unsigned int n = 0u; n < numberManeuvers; ++n)
	{
		IO::JSONConfig::JSONValue& maneuver = leg.value("maneuvers", n);

		std::string instruction = maneuver["instruction"](std::string());

		const int beginShapeIndex = maneuver["begin_shape_index"](-1);
		const int endShapeIndex = maneuver["end_shape_index"](-1); // including

		if (instruction.empty() || beginShapeIndex < 0 || beginShapeIndex > endShapeIndex || endShapeIndex >= int(locations.size()))
		{
			Log::error() << "Received invalid navigation instructions";
			return false;
		}

		Log::debug() << "Maneuver " << n << ": '" << instruction;

		manuevers.emplace_back(std::move(instruction), size_t(beginShapeIndex), size_t(endShapeIndex));
	}

	navigationWaypoints = NavigationWaypoints(std::move(manuevers), std::move(locations));

	return true;
}

Devices::GPSTracker::Location NavigationValhallaExperience::createRandomLocation(const Devices::GPSTracker::Location& currentLocation, const Scalar distance)
{
	ocean_assert(currentLocation.isValid());
	ocean_assert(distance > 0);

	constexpr double earthRadius = 6378135.0; // in meter
	constexpr double earthCircumference = earthRadius * NumericD::pi2();

	RandomGenerator randomGenerator(RandomI::timeBasedSeed());

	const VectorD3 currentVector(SphereD3::coordinateToVector(NumericD::deg2rad(currentLocation.latitude()), NumericD::deg2rad(currentLocation.longitude())));

	const double radDistance = distance / earthCircumference * NumericD::pi2();

	// we determine a rotation which rotates 'currentVector' by 'radDistance'

	const VectorD3 randomAxis = QuaternionD(currentVector, RandomD::scalar(randomGenerator, 0, NumericD::pi2())) * currentVector.perpendicular().normalized();
	ocean_assert(randomAxis.isOrthogonal(currentVector));
	ocean_assert(randomAxis.isUnit());

	const VectorD3 targetVector = QuaternionD(randomAxis, radDistance) * currentVector;
	ocean_assert(targetVector.isUnit());

	double targetLatitude;
	double targetLongitude;
	SphereD3::vectorToCoordinate(targetVector, targetLatitude, targetLongitude);

	return Devices::GPSTracker::Location(NumericD::rad2deg(targetLatitude), NumericD::rad2deg(targetLongitude));
}

}

}
