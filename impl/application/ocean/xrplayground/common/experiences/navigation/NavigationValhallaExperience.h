// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_NAVIGATION_VALHALLA_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_NAVIGATION_VALHALLA_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience based on NavigationVallhalla GraphQL queries.
 *
 * This experience runs a GraphQL query to determine a navigation path when the experience is loaded.
 * The navigation path is then converted into virtual rendering objects.
 * This experience is realized based on the following services:<br>
 * - GPS Tracker (necessary to create a valid GraphQL query)<br>
 * - 6-DOF SLAM tracker with Geo Anchor support<br>
 * - GraphQL client
 * @ingroup xrplayground
 */
class NavigationValhallaExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Definition of individual experience modes.
		 */
		enum ExperienceMode : uint32_t
		{
			/// The experience is using a random target location.
			EM_RANDOM_TARGET = 0u,
			/// The experience is using a custom target loation.
			EM_CUSTOM_TARGET
		};

	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * This class implements a container hodling the relevant information of a Navigation leg.
		 */
		class NavigationWaypoints
		{
			public:

				/**
				 * This class stores a navigation maneuver.
				 */
				class Maneuver
				{
					public:

						/**
						 * Creates a new Maneuver object.
						 * @param instruction The manuever instruction, must be valid
						 * @param beginShapeIndex The index of the first shape (location) at which this maneuver takes place
						 * @param endShapeIndex The (exclusive) index of the shape at which the maneuver ends
						 */
						inline Maneuver(std::string&& instruction, const size_t beginShapeIndex, const size_t endShapeIndex);

					public:

						/// The manuever instruction.
						std::string instruction_;

						/// The index of the first shape (location) at which this maneuver takes place
						const size_t beginShapeIndex_;

						/// The (exclusive) index of the shape at which the maneuver ends
						const size_t lastShapeIndex_;
				};

				/**
				 * Definition of a vector holding maneuvers.
				 */
				typedef std::vector<Maneuver> Maneuvers;

  public:

				/**
				 * Default constructor creating an invalid object.
				 */
				NavigationWaypoints() = default;

				/**
				 * Creates a new NavigationWaypoints object with given maneuvers and locations.
				 * @param maneuvers The sparse maneuvers for the nagivation, must fit to the provided locations
				 * @param locations The dense locations for the navigation, must fit to the provided maneuvers
				 */
				inline NavigationWaypoints(Maneuvers&& maneuvers, Devices::GPSTracker::Locations&& locations);

			public:

				/// The sparse maneuvers for the nagivation.
				Maneuvers maneuvers_;

				/// The dense locations for the navigation.
				Devices::GPSTracker::Locations locations_;
		};

		/**
		 * Definition of an ordered set holding ontent ids.
		 */
		typedef std::set<AnchoredContentManager::ContentId> ContentIdSet;

    public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~NavigationValhallaExperience() override;

		/**
		 * Sets or changes the target location.
		 * In case a navigation is active, the navigation will be changed to the new target location.
		 * The experience must be in mode EM_CUSTOM_TARGET.
		 * @param targetLocation The new target location to be set, must be valid
		 * @return True, if succeeded
		 */
		bool setTargetLocation(const Devices::GPSTracker::Location& targetLocation);

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
		 * @param experienceMode The mode the experience will be using
		 * @return The new experience
		 */
		static std::unique_ptr<NavigationValhallaExperience> createExperience(const ExperienceMode experienceMode = EM_RANDOM_TARGET);

	protected:

		/**
		 * Protected default constructor.
		 * @param experienceMode The mode the experience will be using
		 */
		explicit NavigationValhallaExperience(const ExperienceMode experienceMode);

		/**
		 * Invokes a WorldLayerCircle GraphQL query for a specific GPS location.
		 * The response will eventually show up in 'futureGraphQLResponse_'
		 * @param startLocation The GPS start location for which the query will be invoked
		 * @param targetLocation The GPS target location for which the query will be invoked
		 * @return True, if succeeded
		 */
		bool invokeGraphQLRequest(const Devices::GPSTracker::Location& startLocation, const Devices::GPSTracker::Location& targetLocation);

		/**
		 * Adds the content of all waypoints which have been received via GraphQL to this experience.
		 * @param navigationWaypoints The waypoints to be added
		 */
		void addContent(const Rendering::Engine& engine, const NavigationWaypoints& navigationWaypoints);

		/**
		 * Creates a new rendering object for a given waypoint.
		 * @param engine The rendering engine to be used
		 * @param location The GPS location of the waypoint for which a rendering object will be created
		 * @return The rendering object, will be a transform node containing the actual geometry to render
		 */
		Rendering::TransformRef createRenderingObject(const Rendering::Engine& engine, const Devices::GPSTracker::Location& location);

		/**
		 * Creates a new rendering object for a given maneuver.
		 * @param engine The rendering engine to be used
		 * @param maneuver The maneuver for which the rendering object will be created
		 * @param location The GPS location of the maneuver for which a rendering object will be created
		 * @return The rendering object, will be a transform node containing the actual geometry to render
		 */
		Rendering::TransformRef createRenderingObject(const Rendering::Engine& engine, const NavigationWaypoints::Maneuver& maneuver, const Devices::GPSTracker::Location& location);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * Parses the WorldLayerCircle GraphQL response and extracts the relevant information.
		 * @param response The response to be parsed
		 * @param navigationWaypoints The resulting extracted information
		 * @return True, if succeeded
		 */
		static bool parseNavigationValhallaResponse(const std::string& response, NavigationWaypoints& navigationWaypoints);

		/**
		 * Creates a random GPS location close to a given location.
		 * @param currentLocation The current location for which a random location will be created
		 * @param distance The distance between the current location and the random location, in meters, with range (0, infinity)
		 */
		static Devices::GPSTracker::Location createRandomLocation(const Devices::GPSTracker::Location& currentLocation, const Scalar distance);

	protected:

		/// The mode of the experience.
		ExperienceMode experienceMode_;

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The GPS tracker providing access to the current GPS location.
		Devices::GPSTrackerRef gpsTracker_;

		/// The 6-DOF tracker providing access to the individual Geo Anchors.
		Devices::Tracker6DOFRef anchorTracker6DOF_;

		/// The response of the GraphQL query.
		std::future<Network::Tigon::TigonRequest::TigonResponse> futureGraphQLResponse_;

		/// The custom target location to navigate to, invalid if not set.
		Devices::GPSTracker::Location customTargetLocation_;

		/// The currently active target location to navigate to, invalid if not yet decided.
		Devices::GPSTracker::Location activeTargetLocation_;

		/// The rendering object of an active box.
		Rendering::GeometryRef renderingBoxActive_;

		/// The rendering object of a passive box.
		Rendering::GeometryRef renderingBoxPassive_;

		/// The rendering object of the direction arrow.
		Rendering::TransformRef renderingArrow_;

		/// The modifier for a transform allowing to apply a simple animation to anchored content.
		Rendering::Transform::SharedTransformModifier renderingSharedTransformModifier_;

		/// The set holding the waypoint locations which we have not yet visited.
		ContentIdSet notYetVisitedWaypointContentIdSet_;

		/// The lock for removed content objects.
		Lock removedObjectsLock_;
};

inline NavigationValhallaExperience::NavigationWaypoints::Maneuver::Maneuver(std::string&& instruction, const size_t beginShapeIndex, const size_t lastShapeIndex) :
	instruction_(std::move(instruction)),
	beginShapeIndex_(beginShapeIndex),
	lastShapeIndex_(lastShapeIndex)
{
	// nothing to do here
}

inline NavigationValhallaExperience::NavigationWaypoints::NavigationWaypoints(Maneuvers&& maneuvers, Devices::GPSTracker::Locations&& locations) :
	maneuvers_(std::move(maneuvers)),
	locations_(std::move(locations))
{
	// nothing to do here
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_NAVIGATION_VALHALLA_EXPERIENCE_H
