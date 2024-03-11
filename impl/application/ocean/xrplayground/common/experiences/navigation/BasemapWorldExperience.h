// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_WORLD_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_WORLD_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/io/maps/Basemap.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an advanced experience based on basemap.
 * OSM data will be downloaded for the current GPS location and buildings are visualized in 3D space.
 * @ingroup xrplayground
 */
class BasemapWorldExperience : public XRPlaygroundExperience
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

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~BasemapWorldExperience() override;

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
		BasemapWorldExperience();

		/**
		 * Checks whether a map tile needs to be downloaded or needs to be processed.
		 * @param engine Rendering engine to be used
		 * @param timestamp The current timestamp, must be valid
		 */
		void downloadAndProcessTile(const Rendering::Engine& engine, const Timestamp& timestamp);

		/**
		 * Adds a new rendering objects at a specified GPS location.
		 * @param transform The transform object to be added, must be valid
		 * @param location The GPS location at which the content will be anchored, must be valid
		 * @return True, if succeeded
		 */
		bool addContent(const Rendering::TransformRef& transform, const Devices::GPSTracker::Location& location);

		/**
		 * Removes several content objects from this experience.
		 * @param contentObjects The objects which will be removed from this experience
		 */
		void removeContent(const AnchoredContentManager::SharedContentObjectSet& contentObjects);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The GPS tracker providing access to the current GPS location.
		Devices::GPSTrackerRef gpsTracker_;

		/// The 6-DOF tracker providing access to the individual Geo Anchors.
		Devices::Tracker6DOFRef anchorTracker6DOF_;

		/// The timestamp when the tile will be tried to be downloaded the next time.
		Timestamp nextTileDownloadTimestamp_ = Timestamp(false);

		/// The index pair of the map tile.
		TileIndexPair tileIndexPair_;

		/// The queue of response of HTTP GET request.
		std::future<Network::Tigon::TigonRequest::TigonResponse> futureHttpResponse_;

		/// The content objects which the content manager has recently removed (e.g., because the objects left the radius of engagement).
		AnchoredContentManager::SharedContentObjectSet removedObjects_;

		/// The lock for removed content objects.
		Lock removedObjectsLock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_BASEMAP_EXPERIENCE_WORLD_H
