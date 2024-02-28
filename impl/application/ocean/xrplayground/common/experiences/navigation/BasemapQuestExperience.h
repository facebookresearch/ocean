// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_QUEST_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_QUEST_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"

#include "ocean/io/maps/Basemap.h"

#include "ocean/network/tigon/TigonClient.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience based on basemap for Quest.
 * @ingroup xrplayground
 */
class BasemapQuestExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * The level of the tiles to display, with range [1, 22]
		 */
		constexpr static unsigned int tileLevel_ = 16u;

		/**
		 * Re-definition of a tile index pair.
		 */
		typedef IO::Maps::Basemap::TileIndexPair TileIndexPair;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~BasemapQuestExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Creates a new WorldLayerCircleExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		BasemapQuestExperience();

		/**
		 * Checks whether a map tile needs to be downloaded or needs to be processed.
		 * @param engine Rendering engine to be used
		 * @param timestamp The current timestamp, must be valid
		 */
		void downloadAndProcessTile(const Rendering::Engine& engine, const Timestamp& timestamp);

	protected:

		/// The timestamp when the experience was started.
		Timestamp startTimestamp_ = Timestamp(false);

		/// The GPS tracker providing access to the current GPS location.
		Devices::GPSTrackerRef gpsTracker_;

		/// The timestamp when the tile will be tried to be downloaded the next time.
		Timestamp nextTileDownloadTimestamp_ = Timestamp(false);

		/// The timestamp when a hard-coded GPS location is forced.
		Timestamp useCustomLocationTimestamp_ = Timestamp(false);

		/// The GPS location to be used.
		Devices::GPSTracker::Location location_;

		/// The index pair of the map tile.
		TileIndexPair tileIndexPair_;

		/// The queue of response of HTTP GET request.
		std::future<Network::Tigon::TigonRequest::TigonResponse> futureHttpResponse_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_QUEST_EXPERIENCE_H
