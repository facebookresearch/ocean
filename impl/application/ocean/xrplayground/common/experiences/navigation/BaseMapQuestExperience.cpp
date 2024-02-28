// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/BasemapQuestExperience.h"

#include "application/ocean/xrplayground/common/MapsHandler.h"

#include "ocean/devices/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

using namespace Ocean::IO::Maps;

namespace XRPlayground
{

BasemapQuestExperience::BasemapQuestExperience()
{
	// nothing to do here
}

BasemapQuestExperience::~BasemapQuestExperience()
{
	// nothing to do here
}

bool BasemapQuestExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	gpsTracker_ = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

	if (!gpsTracker_ || !gpsTracker_->start())
	{
		Log::error() << "Basemap Experience could not access a GPS tracker";
	}

	nextTileDownloadTimestamp_ = timestamp;
	useCustomLocationTimestamp_ = timestamp + 5.0; // we will use a hard-coded GPS locaiton in 5 seconds if we do not receive the current GPS locaiton

	startTimestamp_ = timestamp;

	return true;
}

bool BasemapQuestExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	// nothing to do here

	return true;
}

Timestamp BasemapQuestExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(engine && view);
	ocean_assert(timestamp.isValid());

	downloadAndProcessTile(*engine, timestamp);

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> BasemapQuestExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new BasemapQuestExperience());
}

void BasemapQuestExperience::downloadAndProcessTile(const Rendering::Engine& engine, const Timestamp& timestamp)
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
					Rendering::TransformRef transform = MapsHandler::createTile(engine, *tile, 0.5, true /*volumetric*/);
					transform->setTransformation(HomogenousMatrix4(Vector3(0, -0.5, -0.5)));
					experienceScene()->addChild(transform);
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
		if (!location_.isValid() && gpsTracker_)
		{
			Devices::GPSTracker::GPSTrackerSampleRef sample = gpsTracker_->sample();

			if (sample && !sample->locations().empty())
			{
				location_ = sample->locations().front();

				Log::info() << "Received current GPS location " << location_.latitude() << ", " << location_.longitude() << " after " << String::toAString(double(timestamp - startTimestamp_), 1u) << " seconds";
			}
		}

		if (!location_.isValid() && timestamp >= useCustomLocationTimestamp_)
		{
			// using hard-coded GPS location in downtown San Francisco
			location_ = Devices::GPSTracker::Location(37.7866276, -122.3992617);

			showMessage(" Failed to determine the device's GPS location, \n using a hard-coded location in downtown San Francisco instead ");

			Log::warning() << "Failed to determine the device's GPS location, using a hard-coded location in downtown San Francisco instead";
		}

		if (location_.isValid())
		{
			tileIndexPair_ = IO::Maps::Basemap::Tile::calculateTile(tileLevel_, location_.latitude(), location_.longitude());
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

}

}
