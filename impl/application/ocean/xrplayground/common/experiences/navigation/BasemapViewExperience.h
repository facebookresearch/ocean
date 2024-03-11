// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_VIEW_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/io/maps/Basemap.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Transform.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements a simple experience based on Basemap.
 * When starting the experience, Basemap tiles for the current GPS locations are downloaded on the fly and visualized at a fixed location in the view.<br>
 * The orientation of the visualized map is updated for each frame.<br>
 * When the user moves, the map moves as well.
 * @ingroup xrplayground
 */
class BasemapViewExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * The level of the tiles to display, with range [1, 22]
		 */
		constexpr static unsigned int tileLevel_ = 16u;

	protected:

		/**
		 * Re-definition of a tile index pair.
		 */
		typedef IO::Maps::Basemap::TileIndexPair TileIndexPair;

		/**
		 * Re-definition of a tile index pairs.
		 */
		typedef IO::Maps::Basemap::TileIndexPairs TileIndexPairs;

		/**
		 * Definition of an unordered map mapping tile index pairs to rendering objects.
		 */
		typedef std::unordered_map<TileIndexPair, Rendering::TransformRef, TileIndexPair> TileToRenderingTransformMap;

		/**
		 * Definition of a queue combining tile index pairs and tigon request responses.
		 */
		typedef std::queue<std::pair<TileIndexPair, std::future<Network::Tigon::TigonRequest::TigonResponse>>> FutureTigonQueue;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
    	~BasemapViewExperience() override;

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

		/**
		 * Allow other experiences using this experience to place the map at the users feet.
		 * This places the map at a fixed constant below the screen requiring the user to look down.
		 * It does not use any tracking method to put the map at a precise location.
		 */
		void shouldPlaceMapAtFeet(bool state);

    protected:

		/**
		 * Protected default constructor.
		 */
		BasemapViewExperience();

		/**
		 * Checks whether new tiles need to be downloaded due to the current tile.
		 * @param currentTileIndexPair The current tile, must be valid
		 */
		void downloadTiles(const TileIndexPair& currentTileIndexPair);

		/**
		 * Processes the downloaded map tiles and add the rendering objects to the Transform holding all tiles.
		 * @param engine The rendering engine to be used
		 * @param targetTileSize The target size of the tile in the rendering domain, with range (0, infinity)
		 */
		void processDownloadedTiles(const Rendering::Engine& engine, const Scalar targetTileSize);

		/**
		 * Updates the orientation of the map in the screen based on gravity and heading.
		 * @param timestamp The current timestamp
		 */
		void updateMapOrientation(const Timestamp& timestamp);

		/**
		 * Checks whether we need to shuffle the tiles as we have moved to a new tile.
		 * @param currentTileIndexPair The current tile, must be valid
		 * @param latitudeTileFraction The fraction in latitude direction providing the precise location within the tile, with range [0, 1), 0 for the north edge of the tile, 1 for the south edge of the tile
		 * @param longitudeTileFraction The fraction in longitude direction providing the precise location within the tile, with range [0, 1), 0 for the west edge of the tile, 1 for the east edge of the tile
		 * @param targetTileSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return True, if the tiles were reshuffled
		 */
		bool shuffleTiles(const TileIndexPair& currentTileIndexPair, const double latitudeTileFraction, double longitudeTileFraction, const Scalar targetTileSize);

		/**
		 * Moves all tiles so that they are centered at the user's location.
		 */
		void moveTilesToUser();

	protected:

		/// The GPS tracker providing access to the current GPS location.
		Devices::GPSTrackerRef gpsTracker_;

		/// The heading tracker providing access to the device heading (and gravity).
		Devices::OrientationTracker3DOFRef headingTracker_;

		/// The queue of response of HTTP GET request.
		FutureTigonQueue futureHttpResponseQueue_;

		/// The rendering absolute transform allowing to place content in relation to the view.
		Rendering::AbsoluteTransformRef renderingAbsoluteTransform_;

		/// The rendering transform holding all tiles.
		Rendering::TransformRef renderingTransformTilesOrientation_;

		/// The rendering transform holding all tiles.
		Rendering::TransformRef renderingTransformTilesTranslation_;

		/// The rendering transform holding the user's position.
		Rendering::TransformRef renderingTransformUser_;

		/// The timestamp at which the next tile update will happen.
		Timestamp nextTileUpdateTimestamp_ = Timestamp(false);

		/// The smoothed transformation of the GPS translation.
		Tracking::SmoothedTransformation smoothedGPSTranslation_;

		/// The map mapping tile index pairs to rendering objects.
		TileToRenderingTransformMap tileToRenderingTransformMap_;

		/// The tile indices of the current tile in the center.
		TileIndexPair centerTileIndexPair_ = TileIndexPair();

		/// Whether or not to place the map at the users feet
		bool shouldPlaceMapAtFeet_ = false;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_VIEW_H
