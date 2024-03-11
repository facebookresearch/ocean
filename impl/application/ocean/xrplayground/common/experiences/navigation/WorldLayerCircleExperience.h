// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_WORLD_LAYER_CIRCLE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_WORLD_LAYER_CIRCLE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/Transform.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience based on WorldLayerCircle GraphQL queries.
 * WorldLayerCircle provides access to locations of interest associated with GPS locations.
 *
 * This experience runs GraphQL queries on a regular basis and converts the GraphQL responses into anchored content which shows up via the rendering engine.
 * This experience is realized based on the following services:<br>
 * - GPS Tracker (necessary to create a valid GraphQL query)<br>
 * - 6-DOF SLAM tracker with Geo Anchor support<br>
 * - GraphQL client
 * @ingroup xrplayground
 */
class WorldLayerCircleExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * This class implements a container hodling the relevant information of a WorldLayer renderable.
		 */
		class WorldLayerRenderable
		{
			public:

				/**
				 * Definition of individual renderable types.
				 */
				enum RenderableType : uint32_t
				{
					/// The renderable type is unknown.
					RT_UNKNOWN = 0u,
					/// The renderable is a street.
					RT_STREET,
					/// The renderable is a place.
					RT_PLACE
				};

			public:

				/**
				 * Creates a new WorldLayerRenderable object.
				 * @param worldLayerCircleId The unique WorldLayerCircle id of this object, must be valid
				 * @param renderableType The renderable type of this object, must be valid
				 * @param text The text of this object, must not be empty
				 * @param latitude The latitude GPS position of this renderable, in degree, with range [-90, 90]
				 * @param longitude The longitude GPS position of this renderable, in degree, with range [-180, 180]
				 * @param heading The heading of this renderable, in degree, with range [-180, 180]
				 */
				inline WorldLayerRenderable(std::string&& worldLayerCircleId, const RenderableType renderableType, std::string&& text, const double latitude, const double longitude, const double heading);

			public:

				/// The unique WorldLayerCircle id of this object.
				std::string worldLayerCircleId_;

				/// The renderable type of this object.
				RenderableType renderableType_;

				/// The text of this renderable.
				std::string text_;

				/// The latitude GPS position of this renderable, in degree, with range [-90, 90].
				const double latitude_;

				/// The longitude GPS position of this renderable, in degree, with range [-180, 180].
				const double longitude_;

				/// The heading of this renderable, in degree, with range [-180, 180].
				const double heading_;
		};

		/**
		 * Definition of a vector holding WorldLayerRenderable objects.
		 */
		typedef std::vector<WorldLayerRenderable> WorldLayerRenderables;

		/**
		 * Definition of an unordered set holding unique WorldLayerCircle ids.
		 */
		typedef std::unordered_set<std::string> WorldLayerCircleIdSet;

		/**
		 * Definition of an unordered map mapping content ids to unique WorldLayerCircle ids.
		 */
		typedef std::unordered_map<AnchoredContentManager::ContentId, std::string> ContentIdToWorldLayerCircleIdMap;

		/**
		 * Definition of an unordered map mapping tracker object ids to usage counters.
		 */
		typedef std::unordered_map<Devices::Measurement::ObjectId, unsigned int> ObjectIdToCounterMap;

    public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
    	~WorldLayerCircleExperience() override;

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
		WorldLayerCircleExperience();

		/**
		 * Invokes a WorldLayerCircle GraphQL query for a specific GPS location.
		 * The response will eventually show up in 'futureGraphQLResponse_'
		 * @param location The GPS location for which the query will be invoked
		 * @return True, if succeeded
		 */
		bool invokeGraphQLRequest(const Devices::GPSTracker::Location& location);

		/**
		 * Adds several WorldLayer renderables which have been received via GraphQL to this experience.
		 * All renderables which are already part of this experience (determined by the unique WorldLayerCircle id) will be skipped.<br>
		 */
		void addContent(const Rendering::Engine& engine, const WorldLayerRenderables& worldLayerRenderables);

		/**
		 * Removes several content objects from this experience.
		 * @param contentObjects The objects which will be removed from this experience
		 */
		void removeContent(const AnchoredContentManager::SharedContentObjectSet& contentObjects);

		/**
		 * Creates a new rendering object for a given WorldLayer renderable.
		 * Depending on the type of the renderable, a different rendering object will be created
		 * @param engine The rendering engine to be used
		 * @param worldLayerRenderable The WorldLayer renderable for which the rendering object will be created
		 * @return The rendering object, will be a transform node containing the actual geometry to render
		 */
		Rendering::TransformRef createRenderingObject(const Rendering::Engine& engine, const WorldLayerRenderable& worldLayerRenderable) const;

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * Parses the WorldLayerCircle GraphQL response and extracts the relevant information.
		 * @param response The response to be parsed
		 * @param worldLayerRenderables The resulting extracted information
		 * @return True, if succeeded
		 */
		static bool parseWorldLayerCircleResponse(const std::string& response, WorldLayerRenderables& worldLayerRenderables);

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The GPS tracker providing access to the current GPS location.
		Devices::GPSTrackerRef gpsTracker_;

		/// The 6-DOF tracker providing access to the individual Geo Anchors.
		Devices::Tracker6DOFRef anchorTracker6DOF_;

		/// The timestamp when the next GraphQL query should be invoked, invalid to start a query immediately.
		Timestamp nextGraphQLQueryTimestamp_ = Timestamp(false);

		/// The response of the GraphQL query.
		std::future<Network::Tigon::TigonRequest::TigonResponse> futureGraphQLResponse_;

		/// The set holding the unique ids of currently managed WorldLayerCircle renderables.
		WorldLayerCircleIdSet worldLayerCircleIdSet_;

		/// The map mapping content ids to WorldLayerCircle ids.
		ContentIdToWorldLayerCircleIdMap contentIdToWorldLayerCircleIdMap_;

		/// The map mapping tracker object ids to a usage counter.
		ObjectIdToCounterMap objectIdToCounterMap_;

		/// The modifier for a transform allowing to apply a simple animation to anchored content.
		Rendering::Transform::SharedTransformModifier renderingSharedTransformModifier_;

		/// The content objects which the content manager has recently removed (e.g., because the objects left the radius of engagement).
		AnchoredContentManager::SharedContentObjectSet removedObjects_;

		/// The lock for removed content objects.
		Lock removedObjectsLock_;
};

inline WorldLayerCircleExperience::WorldLayerRenderable::WorldLayerRenderable(std::string&& worldLayerCircleId, const RenderableType renderableType, std::string&& text, const double latitude, const double longitude, const double heading) :
	worldLayerCircleId_(std::move(worldLayerCircleId)),
	renderableType_(renderableType),
	text_(std::move(text)),
	latitude_(latitude),
	longitude_(longitude),
	heading_(heading)
{
	// nothing to do here
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_WORLD_LAYER_CIRCLE_EXPERIENCE_H
