// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/BasemapViewExperience.h"

#include "application/ocean/xrplayground/common/MapsHandler.h"

#include "ocean/devices/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

BasemapViewExperience::BasemapViewExperience()
{
	// nothing to do here
}

BasemapViewExperience::~BasemapViewExperience()
{
	// nothing to do here
}

bool BasemapViewExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	gpsTracker_ = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

	if (!gpsTracker_ || !gpsTracker_->start())
	{
		Log::error() << "Basemap Experience could not access a GPS tracker";
		return false;
	}

#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
	headingTracker_ = Devices::Manager::get().device("Android 3DOF Heading Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	headingTracker_ = Devices::Manager::get().device("IOS 3DOF Heading Tracker");
#endif

	if (!headingTracker_ || !headingTracker_->start())
	{
		Log::error() << "Basemap Experience could not access a heading tracker";
		return false;
	}

	ocean_assert(!renderingAbsoluteTransform_ && !renderingTransformTilesTranslation_ && !renderingTransformTilesOrientation_ && !renderingTransformUser_);

	renderingTransformUser_ = Rendering::Utilities::createSphere(engine, Scalar(0.005), RGBAColor(1.0f, 0.0f, 0.0f, 0.5f));
	renderingTransformUser_->setVisible(false);

	renderingTransformTilesTranslation_ = engine->factory().createTransform();

	renderingTransformTilesOrientation_ = engine->factory().createTransform();
	renderingTransformTilesOrientation_->addChild(renderingTransformTilesTranslation_);
	renderingTransformTilesOrientation_->addChild(renderingTransformUser_);

	renderingAbsoluteTransform_ = engine->factory().createAbsoluteTransform();
	renderingAbsoluteTransform_->setTransformationType(Rendering::AbsoluteTransform::TT_VIEW);
	renderingAbsoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0), Scalar(-0.2))));
	renderingAbsoluteTransform_->addChild(renderingTransformTilesOrientation_);

	experienceScene()->addChild(renderingAbsoluteTransform_);

	return true;
}

bool BasemapViewExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingTransformUser_.release();
	renderingTransformTilesTranslation_.release();
	renderingTransformTilesOrientation_.release();
	renderingAbsoluteTransform_.release();

	return true;
}

Timestamp BasemapViewExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(gpsTracker_);

	constexpr Scalar targetTileSize = Scalar(0.2);

	const Devices::GPSTracker::GPSTrackerSampleRef gpsSample(gpsTracker_->sample(timestamp));

	Devices::GPSTracker::Location currentLocation;
	TileIndexPair currentTileIndexPair;

	if (gpsSample)
	{
		ocean_assert(!gpsSample->locations().empty());

		currentLocation = gpsSample->locations().front();

		currentTileIndexPair = IO::Maps::Basemap::Tile::calculateTile(tileLevel_, currentLocation.latitude(), currentLocation.longitude());

		if (!centerTileIndexPair_.isValid())
		{
			centerTileIndexPair_ = currentTileIndexPair;
		}

		if (nextTileUpdateTimestamp_.isInvalid() || timestamp >= nextTileUpdateTimestamp_)
		{
			// we may need to download new tiles as we may have moved
			downloadTiles(currentTileIndexPair);

			nextTileUpdateTimestamp_ = timestamp + 10.0;
		}
	}

	// we check whether new tiles have been downloaded which need to be converted to rendering objects
	processDownloadedTiles(*engine, targetTileSize);

	// we update the orientation of the map based on gravity and heading
	updateMapOrientation(timestamp);

	bool userVisible = false;

	if (currentLocation.isValid() && centerTileIndexPair_.isValid() && currentTileIndexPair.isValid())
	{
		double latitudeTileFraction = -1.0;
		double longitudeTileFraction = -1.0;

		const TileIndexPair unusedTileIndexPair = IO::Maps::Basemap::Tile::calculateTile(tileLevel_, currentLocation.latitude(), currentLocation.longitude(), &latitudeTileFraction, &longitudeTileFraction);
		ocean_assert_and_suppress_unused(unusedTileIndexPair == currentTileIndexPair, unusedTileIndexPair);

		if (currentTileIndexPair != centerTileIndexPair_)
		{
			if (shuffleTiles(currentTileIndexPair, latitudeTileFraction, longitudeTileFraction, targetTileSize))
			{
				centerTileIndexPair_ = currentTileIndexPair;

				// all tiles have been re-shuffeled (around the origin), so the smoothed GPS translation need to be reset
				smoothedGPSTranslation_.reset();
			}

			// we have to determine the fractions in relation to the center tile (which may have changed or not)
			IO::Maps::Basemap::Tile::calculateTileFractions(tileLevel_, currentLocation.latitude(), currentLocation.longitude(), centerTileIndexPair_, latitudeTileFraction, longitudeTileFraction);
		}

		const TileToRenderingTransformMap::const_iterator i = tileToRenderingTransformMap_.find(currentTileIndexPair);

		if (i != tileToRenderingTransformMap_.cend() && i->second)
		{
			const Scalar latitudeCenterPosition = Scalar(latitudeTileFraction - 0.5);
			const Scalar longitudeCenterPosition = Scalar(longitudeTileFraction - 0.5);

			smoothedGPSTranslation_.setTransformation(HomogenousMatrix4(Vector3(-longitudeCenterPosition, 0, -latitudeCenterPosition) * targetTileSize), timestamp);

			ocean_assert(renderingTransformTilesTranslation_);
			renderingTransformTilesTranslation_->setTransformation(smoothedGPSTranslation_.transformation(timestamp));

			userVisible = true;
		}
	}

	ocean_assert(renderingTransformUser_);
	renderingTransformUser_->setVisible(userVisible);

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> BasemapViewExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new BasemapViewExperience());
}

void BasemapViewExperience::shouldPlaceMapAtFeet(bool state)
{
	shouldPlaceMapAtFeet_ = state;
}

void BasemapViewExperience::downloadTiles(const TileIndexPair& currentTileIndexPair)
{
	ocean_assert(currentTileIndexPair.isValid());

	constexpr unsigned int numberTilesOnLevel = IO::Maps::Basemap::Tile::numberTiles(tileLevel_);

	for (const TileIndexPair& tileIndexPair : TileIndexPair::createNeighborhoodTiles(currentTileIndexPair, numberTilesOnLevel, 1u))
	{
		if (tileToRenderingTransformMap_.find(tileIndexPair) == tileToRenderingTransformMap_.cend())
		{
			std::future<Network::Tigon::TigonRequest::TigonResponse> futureResponse = MapsHandler::downloadTile(tileLevel_, tileIndexPair.latitudeIndex(), tileIndexPair.longitudeIndex());

			if (futureResponse.valid())
			{
				futureHttpResponseQueue_.emplace(tileIndexPair, std::move(futureResponse));

				tileToRenderingTransformMap_.emplace(tileIndexPair, Rendering::TransformRef());
			}
			else
			{
				Log::error() << "Failed to make a HTTP request to download a map tile";
			}
		}
	}
}

void BasemapViewExperience::processDownloadedTiles(const Rendering::Engine& engine, const Scalar targetTileSize)
{
	while (!futureHttpResponseQueue_.empty())
	{
		const TileIndexPair& tileIndexPair = futureHttpResponseQueue_.front().first;
		std::future<Network::Tigon::TigonRequest::TigonResponse>& futureResponse = futureHttpResponseQueue_.front().second;

		ocean_assert(futureResponse.valid());
		if (futureResponse.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			Network::Tigon::TigonRequest::TigonResponse tigonResponse = futureResponse.get();

			if (tigonResponse.succeeded())
			{
				const IO::Maps::Basemap::SharedTile tile = IO::Maps::Basemap::newTileFromPBFData(tileLevel_, tileIndexPair, tigonResponse.response().data(), tigonResponse.response().size());

				if (tile)
				{
					Rendering::TransformRef transform = MapsHandler::createTile(engine, *tile, targetTileSize, true /*volumetric*/);

					ocean_assert(centerTileIndexPair_.isValid());

					const Scalar tileIndexOffsetLatitude = Scalar(int(tileIndexPair.latitudeIndex() - centerTileIndexPair_.latitudeIndex()));
					const Scalar tileIndexOffsetLongitude = Scalar(int(tileIndexPair.longitudeIndex() - centerTileIndexPair_.longitudeIndex()));

					transform->setTransformation(HomogenousMatrix4(Vector3(Scalar(tileIndexOffsetLongitude), 0, Scalar(tileIndexOffsetLatitude)) * targetTileSize));

					TileToRenderingTransformMap::iterator i = tileToRenderingTransformMap_.find(tileIndexPair);
					ocean_assert(i != tileToRenderingTransformMap_.cend());

					ocean_assert(renderingTransformTilesTranslation_);
					renderingTransformTilesTranslation_->addChild(transform);

					ocean_assert(i->second.isNull());
					i->second = std::move(transform);
				}
			}
			else
			{
				Log::warning() << "Failed to download map tile, retrying again...";

				// we try to reload the data again
				tileToRenderingTransformMap_.erase(tileIndexPair);
			}

			futureHttpResponseQueue_.pop();
		}
		else
		{
			break;
		}
	}
}

void BasemapViewExperience::updateMapOrientation(const Timestamp& timestamp)
{
	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef headingSample(headingTracker_->sample(timestamp, Devices::OrientationTracker3DOF::IS_TIMESTAMP_INTERPOLATE));

	if (headingSample)
	{
		ocean_assert(!headingSample->orientations().empty());
		ocean_assert(headingSample->referenceSystem() == Devices::OrientationTracker3DOF::RS_DEVICE_IN_OBJECT);

		const Quaternion& object_Q_device = headingSample->orientations().front();
		Quaternion device_Q_object = object_Q_device.inverted();

		// we want to place the map at the bottom of the screen,
		// therefore, we need to know which screen edge is closer to ground

		const Vector3 gravity = device_Q_object * Vector3(0, -1, 0);

		constexpr Scalar cosValue45 = Scalar(0.707106781186548);
		Scalar edgeOffset = shouldPlaceMapAtFeet_ ? Scalar(0) : Scalar(0.04);
		Scalar zOffset = shouldPlaceMapAtFeet_ ? Scalar(-0.5) : Scalar(-0.2);

		if (gravity.z() >= 0)
		{
			// the user is looking from the bottom of the map, we ensure that we do not see the map from the bottom

			Vector3 heading = device_Q_object * Vector3(0, 0, -1);

			const Vector3 clampedGravity = Vector3(gravity.xy(), 0).normalizedOrZero();

			const Vector3 xAxis = -heading.cross(clampedGravity).normalizedOrZero();
			heading = xAxis.cross(clampedGravity).normalizedOrZero();

			const SquareMatrix3 device_R_object(xAxis, -clampedGravity, -heading);
			ocean_assert(device_R_object.isOrthonormal());

			device_Q_object = Quaternion(device_R_object);
		}

		renderingTransformTilesOrientation_->setTransformation(HomogenousMatrix4(device_Q_object));

		if (gravity * Vector3(-1, 0, 0) >= cosValue45)
		{
			// landscape mode, home button to the right
			if (shouldPlaceMapAtFeet_) edgeOffset += gravity.x();
			renderingAbsoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(-edgeOffset, 0, zOffset)));
		}
		else if (gravity * Vector3(1, 0, 0) >= cosValue45)
		{
			if (shouldPlaceMapAtFeet_) edgeOffset -= gravity.x();
			// landscape mode, home button to the left
			renderingAbsoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(edgeOffset, 0, zOffset)));
		}
		else if (gravity * Vector3(0, 1, 0) >= cosValue45)
		{
			// portrait mode, home button at the top
			if (shouldPlaceMapAtFeet_) edgeOffset += gravity.y();
			renderingAbsoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(0, edgeOffset, zOffset)));
		}
		else
		{
			// portrait mode, home button at the bottom
			if (shouldPlaceMapAtFeet_) edgeOffset -= gravity.y();
			renderingAbsoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(0, -edgeOffset, zOffset)));
		}
	}
}

bool BasemapViewExperience::shuffleTiles(const TileIndexPair& currentTileIndexPair, const double latitudeTileFraction, double longitudeTileFraction, const Scalar targetTileSize)
{
	constexpr unsigned int numberTilesOnLevel = IO::Maps::Basemap::Tile::numberTiles(tileLevel_);

	constexpr double fractionThreshold = 0.2;

	if (centerTileIndexPair_.isLocationClose(currentTileIndexPair, latitudeTileFraction, longitudeTileFraction, fractionThreshold))
	{
		return false;
	}

	// we are not close to the current center tile anymore, we need to make the current tile the new center tile, but first we need to ensure that all "new" neighboring tiles are already downloaded

	bool allNeighborTilesExist = true;

	const TileIndexPairs newNeibhoringTiles = TileIndexPair::createNeighborhoodTiles(currentTileIndexPair, numberTilesOnLevel, 1u);

	for (const TileIndexPair& tileIndexPair : newNeibhoringTiles)
	{
		TileToRenderingTransformMap::const_iterator i = tileToRenderingTransformMap_.find(tileIndexPair);

		if (i == tileToRenderingTransformMap_.cend() || i->second.isNull())
		{
			allNeighborTilesExist = false;
			break;
		}
	}

	if (!allNeighborTilesExist)
	{
		// we wait until all "new" neibhoring tiles are downloaded and rendered
		return false;
	}

	// we update remove all rendering objects of tiles which are part of the "new" neibhorhood anymore, and we update the positions of all remaining tiles

	const IO::Maps::Basemap::TileIndexPairSet newNeibhoringTileSet(newNeibhoringTiles.cbegin(), newNeibhoringTiles.cend());

	for (TileToRenderingTransformMap::iterator i = tileToRenderingTransformMap_.begin(); i != tileToRenderingTransformMap_.end(); /* noop */)
	{
		ocean_assert(i->second);

		if (newNeibhoringTileSet.find(i->first) == newNeibhoringTileSet.cend())
		{
			// we do not need the tile anymore

			renderingTransformTilesTranslation_->removeChild(i->second);

			i = tileToRenderingTransformMap_.erase(i);
		}
		else
		{
			const TileIndexPair& tileIndexPair = i->first;

			const Scalar tileIndexOffsetLatitude = Scalar(int(tileIndexPair.latitudeIndex() - currentTileIndexPair.latitudeIndex()));
			const Scalar tileIndexOffsetLongitude = Scalar(int(tileIndexPair.longitudeIndex() - currentTileIndexPair.longitudeIndex()));

			i->second->setTransformation(HomogenousMatrix4(Vector3(Scalar(tileIndexOffsetLongitude), 0, Scalar(tileIndexOffsetLatitude)) * targetTileSize));

			++i;
		}
	}

	ocean_assert(tileToRenderingTransformMap_.size() == newNeibhoringTiles.size());

	return true;
}

}

}
