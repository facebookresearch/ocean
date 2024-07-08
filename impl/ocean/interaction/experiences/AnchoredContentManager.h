/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EXPERIENCES_ANCHORED_CONTENT_MANAGER_H
#define META_OCEAN_INTERACTION_EXPERIENCES_ANCHORED_CONTENT_MANAGER_H

#include "ocean/interaction/experiences/Experiences.h"
#include "ocean/interaction/experiences/Experience.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Triple.h"

#include "ocean/devices/Measurement.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"

#include <functional>

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

/**
 * This class implements a manager for anchored content and allows to simplify creating experiences intensively using virtual content anchored based on 6-DOF trackers.
 * The manager handles visibility of anchored content and updates the underlying rendering transformations to ensure that virtual content shows up at the intended location.
 * @ingroup interactionexperiences
 */
class OCEAN_INTERACTION_EXPERIENCES_EXPORT AnchoredContentManager
{
	public:

		/**
		 * Definition of an id defining anchored content.
		 */
		typedef unsigned int ContentId;

		/**
		 * Returns an invalid content id.
		 * @return Invalid content id
		 */
		static constexpr inline ContentId invalidContentId();

		/**
		 * This class implements a container for anchored content.
		 * Anchored content is mainly defined by the actual rendering object (e.g., a geometry/shape/mesh etc.), the tracker used to anchor the content, and the tracker's object id associated with the content.
		 * Further, a visibility radius specifies within which radius the anchored content is visible.<br>
		 * If the anchor leaves the engagement radius, the content will be removed automatically from the manger to improve resource management.
		 */
		class OCEAN_INTERACTION_EXPERIENCES_EXPORT ContentObject
		{
			friend class AnchoredContentManager;

			public:

				/**
				 * Returns the unique id of this content object.
				 * @return The object's unique content id
				 */
				inline ContentId contentId() const;

				/**
				 * Returns the rendering node holding the content to be rendered.
				 * This object needs to be valid.
				 * @return The object's rendering node
				 */
				inline Rendering::NodeRef renderingObjectNode() const;

				/**
				 * Returns whether the anchor is currently tracked.
				 * This object needs to be valid.
				 * @return True, if so
				 */
				inline bool isTracked() const;

				/**
				 * Returns the current square distance to the anchor.
				 * This object needs to be valid.
				 * @return The square distance to the anchor when the anchor was tracked the last time, with range [0, infinity), -1 if unknown
				 */
				inline Scalar sqrDistance() const;

				/**
				 * Returns the transformation between object and world.
				 * This object needs to be valid.
				 * @return The object's transformation
				 */
				HomogenousMatrix4 world_T_object() const;

				/**
				 * Updates or changes the rendering node holding the content to be rendered
				 * @param renderingObjectNode The new rendering object node, must be valid
				 */
				void setRenderingObjectNode(Rendering::NodeRef renderingObjectNode);

				/**
				 * Returns whether this content object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Creates a new content object.
				 * @param renderingObjectNode The rendering object to take care of, will be added to the manager's rendering scene, must be valid
				 * @param devicesTracker The 6-DOF tracker which provides the anchor to which the rendering object is connected, must be valid
				 * @param devicesObjectId The tracker object id of the anchor to which the rendering object is connected, must be valid
				 * @param contentId The object's unique content id, must be valid
				 * @param visibilityRadius The radius in which the new content will be visible (maximal distance between anchor and user), in meter, with range (0, infinity)
				 * @param engagementRadius The radius defining the engagement area of the new content, whenever the user leaves this area, the content will be removed from this manager automatically, with range [visibilityRadius, infinity)
				 */
				ContentObject(const Rendering::NodeRef& renderingObjectNode, const Devices::Tracker6DOFRef& devicesTracker, const Devices::Tracker6DOF::ObjectId& devicesObjectId, const ContentId contentId, const Scalar visibilityRadius, const Scalar engagementRadius);

			protected:

				/// The object's unique content id.
				ContentId contentId_ = invalidContentId();

				/// The rendering node holding the content to be rendered.
				Rendering::NodeRef renderingObjectNode_;

				/// The transform between scene and rendering node which will receive constant transformation updates from the tracker.
				Rendering::TransformRef renderingAnchorTransform_;

				/// The tracker providing the pose updates for the anchor.
				Devices::Tracker6DOFRef devicesTracker_;

				/// The tracker object id of the anchor.
				Devices::Tracker6DOF::ObjectId devicesObjectId_;

				/// The radius in which the content is visible, in meter.
				Scalar visibilityRadius_ = -1;

				/// The radius in which the content is managed by this owning manager.
				Scalar engagementRadius_ = -1;

				/// True, if the content is currently tracked (e.g., visible if the anchor is based on a visual tracker); False, if the content is currently not tracked.
				bool isTracked_ = false;

				/// The current square distance to the anchor.
				Scalar sqrDistance_ = -1;

				/// The content's lock.
				mutable Lock lock_;
		};

		/**
		 * Definition of a shared content object.
		 */
		typedef std::shared_ptr<ContentObject> SharedContentObject;

		/**
		 * Definition of an unordered set holding content objects.
		 */
		typedef std::unordered_set<SharedContentObject> SharedContentObjectSet;

		/**
		 * Definition of a callback function for removed object events.
		 */
		typedef std::function<void(SharedContentObjectSet&&)> RemovedContentCallbackFunction;

	protected:

		/**
		 * Definition of an unordered map mapping content ids to content objects.
		 */
		typedef std::unordered_map<ContentId, SharedContentObject> ContentMap;

		/**
		 * Definition of an unordered multimap mapping object ids to content objects.
		 */
		typedef std::unordered_multimap<Devices::Measurement::ObjectId, SharedContentObject> ObjectIdToContentObjectMultiMap;

		/**
		 * Definition of an unordered map mapping trackers to content objects.
		 */
		typedef std::unordered_map<const Devices::Tracker6DOF*, SharedContentObject> TrackerToContentObjectMap;

		/**
		 * Definition of pair combining a subscription id and a usage counter.
		 */
		typedef std::pair<Devices::Tracker::TrackerObjectEventSubscription, unsigned int> SubscriptionPair;

		/**
		 * Definition of an unordered map mapping trackers to subscription pairs.
		 */
		typedef std::unordered_map<Devices::Tracker6DOF*, SubscriptionPair> SubscriptionMap;

	public:

		/**
		 * Creates a new manager object.
		 * Experiences can create several independent manager if necessary.<br>
		 * A manager needs to be initialized before it can be used.
		 * @see initialize().
		 */
		AnchoredContentManager();

		/**
		 * Destructs the manager and releases all resources.
		 * @see release().
		 */
		~AnchoredContentManager();

		/**
		 * Initializes the manager.
		 * @param removedContentCallbackFunction The callback function which will be called whenever the manager removes a content object e.g., because the object left the engagement area, must be valid
		 * @param scene The rendering scene to which the manager will add all rendering elements of the content objects, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(RemovedContentCallbackFunction removedContentCallbackFunction, const Rendering::SceneRef& scene);

		/**
		 * Releases this manager explicitly before.
		 */
		void release();

		/**
		 * Adds a new anchored content so that the manager will take care of visibility and pose updates.
		 * @param renderingObjectNode The rendering object to take care of, will be added to the manager's rendering scene, must be valid
		 * @param devicesTracker The 6-DOF tracker which provides the anchor to which the rendering object is connected, must be valid
		 * @param devicesObjectId The tracker object id of the anchor to which the rendering object is connected, must be valid
		 * @param visibilityRadius The radius in which the new content will be visible (maximal distance between anchor and user), in meter, with range (0, infinity)
		 * @param engagementRadius The radius defining the engagement area of the new content, whenever the user leaves this area, the content will be removed from this manager automatically, with range [visibilityRadius, infinity)
		 * @return The id of the new content
		 */
		ContentId addContent(const Rendering::NodeRef& renderingObjectNode, const Devices::Tracker6DOFRef& devicesTracker, const Devices::Tracker6DOF::ObjectId& devicesObjectId, const Scalar visibilityRadius, const Scalar engagementRadius);

		/**
		 * Removes a content object from this manager.
		 * The manager will remove the associated rendering object from the scene and will not take care of visibility and pose updates anymore.
		 * @return True, if succeeded
		 */
		bool removeContent(const ContentId contentId);

		/**
		 * Removes all content object currently managed by this manager.
		 * The manager will remove all associated rendering objects from the scene and will not take care of visibility and pose updates anymore.
		 * @return True, if succeeded
		 */
		bool removeAllContent();

		/**
		 * Returns a specific content.
		 * @param contentId The id of the content to return, must be valid
		 * @return The resulting content, nullptr if the content is currently not managed
		 */
		SharedContentObject content(const ContentId contentId) const;

		/**
		 * Returns all contents which are currently visible.
		 * @return All visible content objects
		 */
		SharedContentObjectSet visibleContents() const;

		/**
		 * Returns all contents which is closer than a given distance.
		 * @param maxSqrDistance The maximal square distance to the anchor so that the object will be returned, with range [0, infinity)
		 * @return All visible content objects closer than the specified distance
		 */
		SharedContentObjectSet closeContents(const Scalar maxSqrDistance) const;

		/**
		 * Pre update event function in which all visibility and pose updates will be handled.
		 * @see Library::preUpdate().
		 */
		Timestamp preUpdate(const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Returns the scene to which the rendering objects of new content will be added.
		 * @return The manager's scene, invalid if not yet initialized
		 */
		inline Rendering::SceneRef scene() const;

		/**
		 * Returns whether this manager is initialized and ready to use.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Internal event function for tracker object events.
		 * @param tracker The sender of the event, will be valid
		 * @param found True, if all provided objects have been found at the specified timestamp; False, if all provided objects have been lost
		 * @param objectIds The ids of all tracker objects for which the found/lost event is intended
		 * @param timestamp The event timestamp
		 */
		void onTrackerObjects(const Devices::Tracker* tracker, const bool found, const Devices::Measurement::ObjectIdSet& objectIds, const Timestamp& timestamp);

	protected:

		/// The map containing the currently managed content objects.
		ContentMap contentMap_;

		/// The multi map mapping tracker object ids to content objects.
		ObjectIdToContentObjectMultiMap objectIdToContentObjectMultiMap_;

		/// The tracker mapping trackers to content objects.
		TrackerToContentObjectMap trackerToContentObjectMap_;

		/// The map mapping trackers to subscription ids.
		SubscriptionMap subscriptionMap_;

		/// The counter for unique content ids.
		unsigned int contentIdCounter_ = 0u;

		/// The scene to which all rendering nodes will be added.
		Rendering::SceneRef renderingScene_;

		/// The callback function for removed content events.
		RemovedContentCallbackFunction removedContentCallbackFunction_;

		/// The manager's lock.
		mutable Lock lock_;
};

constexpr inline AnchoredContentManager::ContentId AnchoredContentManager::invalidContentId()
{
	return ContentId(-1);
}

inline AnchoredContentManager::ContentId AnchoredContentManager::ContentObject::contentId() const
{
	return contentId_;
}

inline Rendering::NodeRef AnchoredContentManager::ContentObject::renderingObjectNode() const
{
	ocean_assert(isValid());

	return renderingObjectNode_;
}

inline bool AnchoredContentManager::ContentObject::isTracked() const
{
	ocean_assert(isValid());

	return isTracked_;
}

inline Scalar AnchoredContentManager::ContentObject::sqrDistance() const
{
	ocean_assert(isValid());

	return sqrDistance_;
}

inline bool AnchoredContentManager::ContentObject::isValid() const
{
	return bool(renderingObjectNode_);
}

inline Rendering::SceneRef AnchoredContentManager::scene() const
{
	return renderingScene_;
}

inline AnchoredContentManager::operator bool() const
{
	return removedContentCallbackFunction_ && renderingScene_;
}

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_EXPERIENCES_ANCHORED_CONTENT_MANAGER_H
