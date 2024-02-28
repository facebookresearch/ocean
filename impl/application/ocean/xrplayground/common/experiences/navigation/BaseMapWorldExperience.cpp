// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/BasemapWorldExperience.h"

#include "application/ocean/xrplayground/common/MapsHandler.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

using namespace Ocean::IO::Maps;

namespace XRPlayground
{

BasemapWorldExperience::BasemapWorldExperience()
{
	// nothing to do here
}

BasemapWorldExperience::~BasemapWorldExperience()
{
	// nothing to do here
}

bool BasemapWorldExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	gpsTracker_ = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

	if (!gpsTracker_ || !gpsTracker_->start())
	{
		Log::error() << "Basemap Experience could not access a GPS tracker";
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
		Log::error() << "BasemapWorld Experience could not access anchor tracker";
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
		Log::error() << "Basemap Experience could not access anchor tracker";
		return false;
	}

	if (!anchoredContentManager_.initialize(std::bind(&BasemapWorldExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
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

	nextTileDownloadTimestamp_ = timestamp;

	return true;
}

bool BasemapWorldExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	anchoredContentManager_.release();

	return true;
}

Timestamp BasemapWorldExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(engine && view);
	ocean_assert(timestamp.isValid());

	// first we check whether we have to remove content which we have added previously
	// content is removed when the content left the radius of engagement

	TemporaryScopedLock temporaryScopedLock(removedObjectsLock_);
		const AnchoredContentManager::SharedContentObjectSet removedObjects(std::move(removedObjects_));
	temporaryScopedLock.release();

	removeContent(removedObjects);

	downloadAndProcessTile(*engine, timestamp);

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

std::unique_ptr<XRPlaygroundExperience> BasemapWorldExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new BasemapWorldExperience());
}

void BasemapWorldExperience::downloadAndProcessTile(const Rendering::Engine& engine, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (tileIndexPair_.isValid() && futureHttpResponse_.valid())
	{
		if (futureHttpResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			Network::Tigon::TigonRequest::TigonResponse tigonResponse = futureHttpResponse_.get();

			futureHttpResponse_ = std::future<Network::Tigon::TigonRequest::TigonResponse>();
			ocean_assert(!futureHttpResponse_.valid());

			if (tigonResponse.succeeded())
			{
				const Basemap::SharedTile tile = Basemap::newTileFromPBFData(tileLevel_, tileIndexPair_, tigonResponse.response().data(), tigonResponse.response().size());

				if (tile)
				{
					constexpr double metricNormalization = 1.0;

					const double tileMetricExtent = tile->metricExtent();

					const Rendering::AttributeSetRef attributeSetBuilding = engine.factory().createAttributeSet();

					const Rendering::MaterialRef materialBuilding = engine.factory().createMaterial();
					materialBuilding->setDiffuseColor(RGBAColor(0.5f, 0.5f, 0.5f));
					materialBuilding->setTransparency(0.20f);
					attributeSetBuilding->addAttribute(materialBuilding);

					attributeSetBuilding->addAttribute(engine.factory().createBlendAttribute());

					for (const Basemap::SharedObject& object : tile->objects())
					{
						ocean_assert(object);
						if (object->objectType() == Basemap::Object::OT_BUILDING)
						{
							const Basemap::Building& building = reinterpret_cast<const Basemap::Building&>(*object);

							const CV::PixelBoundingBoxI boundingBox = building.boundingBox();

							if (boundingBox.isValid())
							{
								// we create a rendering object with triangles, centered at the center of the building

								const CV::PixelPositionI buildingOrigin(boundingBox.left() + int(boundingBox.width() / 2u), boundingBox.top() + int(boundingBox.height() / 2u));

								const double pixelPositionNormalization = tileMetricExtent / double(building.layerExtent());

								const Rendering::TrianglesRef triangles = MapsHandler::createBuilding(engine, building, buildingOrigin, pixelPositionNormalization, metricNormalization, true /*volumetric*/);

								const Rendering::GeometryRef geometry = engine.factory().createGeometry();
								geometry->addRenderable(triangles, attributeSetBuilding);

								const Rendering::TransformRef transform = engine.factory().createTransform();
								transform->setTransformation(HomogenousMatrix4(Vector3(0, -1.5, 0))); // **TODO** geo anchors are currently ~ 1.5 meter above ground

								transform->addChild(geometry);

								double latitude = NumericD::minValue();
								double longitude = NumericD::maxValue();
								tile->tileCoordinate2GPSLocation(buildingOrigin, building.layerExtent(), latitude, longitude);

								const Devices::GPSTracker::Location location(latitude, longitude);
								ocean_assert(location.isValid());

								addContent(transform, location);
							}
						}
					}
				}

				nextTileDownloadTimestamp_.toInvalid(); // we do not need to try downloading the tile anymore

				return;
			}

			Log::warning() << "Failed to download map tile, we will retry in a few seconds";

			nextTileDownloadTimestamp_ = timestamp + 2.0;
		}
	}
	else if (nextTileDownloadTimestamp_.isValid() && timestamp >= nextTileDownloadTimestamp_)
	{
		ocean_assert(gpsTracker_);

		const Devices::GPSTracker::GPSTrackerSampleRef gpsSample(gpsTracker_->sample(timestamp));

		if (gpsSample)
		{
			ocean_assert(!gpsSample->locations().empty());

			const Devices::GPSTracker::Location& currentLocation = gpsSample->locations().front();

			tileIndexPair_ = IO::Maps::Basemap::Tile::calculateTile(tileLevel_, currentLocation.latitude(), currentLocation.longitude());
			ocean_assert(tileIndexPair_.isValid());

			futureHttpResponse_ = MapsHandler::downloadTile(tileLevel_, tileIndexPair_.latitudeIndex(), tileIndexPair_.longitudeIndex());

			if (!futureHttpResponse_.valid())
			{
				Log::warning() << "Failed to download map tile, we will retry in a few seconds";

				nextTileDownloadTimestamp_ = timestamp + 2.0;
			}
		}
	}
}

bool BasemapWorldExperience::addContent(const Rendering::TransformRef& transform, const Devices::GPSTracker::Location& location)
{
	ocean_assert(transform);

	const Devices::ObjectTrackerRef objectTracker(anchorTracker6DOF_);
	ocean_assert(objectTracker);

	const std::string trackerObjectDescription = "GPS Location " + String::toAString(location.latitude(), 10u) + ", " + String::toAString(location.longitude(), 10u);

	const Devices::ObjectTracker::ObjectId trackerObjectId = objectTracker->registerObject(trackerObjectDescription);

	if (trackerObjectId == Devices::ObjectTracker::invalidObjectId())
	{
		return false;
	}

	constexpr Scalar visibilityRadius = Scalar(1000); // in meter
	constexpr Scalar engagementRadius = Scalar(2000);

	const AnchoredContentManager::ContentId contentId = anchoredContentManager_.addContent(transform, anchorTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);

	if (contentId == AnchoredContentManager::invalidContentId())
	{
		return false;
	}

	return true;
}

void BasemapWorldExperience::removeContent(const AnchoredContentManager::SharedContentObjectSet& contentObjects)
{
	for (const AnchoredContentManager::SharedContentObject& contentObject : contentObjects)
	{
		ocean_assert(contentObject);
		experienceScene()->removeChild(contentObject->renderingObjectNode());
	}
}

void BasemapWorldExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
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

}

}
