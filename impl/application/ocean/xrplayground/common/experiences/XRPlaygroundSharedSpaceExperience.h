// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_SHARED_SPACE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_SHARED_SPACE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/media/FrameMedium.h"

#include "metaonly/ocean/network/verts/Driver.h"

#include "ocean/rendering/Transform.h"

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	#include "metaonly/ocean/platform/meta/avatars/Manager.h"
#endif

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE
	#include <facetracker/manager/FaceTrackerManager_Ocean.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience is the base class for all Avatars experiences.
 * @ingroup xrplayground
 */
class XRPlaygroundSharedSpaceExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * This class implements a floor tracker.
		 */
		class FloorTracker
		{
			public:

				/**
				 * Creates a new floor tracker.
				 * @param frameMedium The frame medium which is used on mobile platforms as tracker input; can be invalid on Quest platforms
				 */
				explicit FloorTracker(const Media::FrameMediumRef& frameMedium = Media::FrameMediumRef());

				/**
				 * Returns the latest floor plane.
				 * @return The tracker's latest floor plane
				 */
				Plane3 plane() const;

			protected:

				/**
				 * Event function for new tracking samples from the scene tracker.
				 * @param measurement The measurement object sending the sample
				 * @param sample The new samples with resulting from scene tracker
				 */
				void onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

			protected:

				/// The 6-DOF floor tracker.
				Devices::Tracker6DOFRef tracker6DOF_;

				/// The subscription object for sample events from the tracker.
				Devices::Measurement::SampleEventSubscription trackerSampleEventSubscription_;

				/// The timestamp when the floor plane was updated the last time.
				Timestamp planeTimestamp_;

				/// The floor plane defined in world, invalid if unknown.
				Plane3 plane_;

				/// The tracker's lock.
				mutable Lock lock_;
		};

		/**
		 * Definition of an unordered map mapping user ids to rendering Transform nodes.
		 */
		typedef std::unordered_map<uint64_t, Rendering::TransformRef> AvatarRenderingMap;

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * This class implements a face tracker.
		 */
		class FaceTracker : protected Thread
		{
			public:

				/**
				 * Creates a new face tracker.
				 * @param slamTracker The SLAM world tracker to be used, must be valid
				 * @param slamFrameMedium The frame medium the SLAM tracker is using, must be valid
				 */
				explicit FaceTracker(const Devices::Tracker6DOFRef& slamTracker, const Media::FrameMediumRef& slamFrameMedium);

				/**
				 * Destructs the face tracker.
				 */
				~FaceTracker() override;

				/**
				 * Returns the latest transformation between head and world.
				 */
				HomogenousMatrix4 world_T_head() const;

			protected:

				/**
				 * Initializes the facetracker.
				 * @return True, if succeeded
				 */
				bool initializeFacetracker();

				/**
				 * Thread run function.
				 */
				void threadRun() override;

			protected:

				/// The 6-DOF SLAM world tracker.
				Devices::Tracker6DOFRef slamTracker6DOF_;

				/// The 6-DOF face tracker, used on iOS.
				Devices::Tracker6DOFRef faceTracker6DOF_;

				/// The frame medium used to track the face.
				Media::FrameMediumRef frameMedium_;

				/// The implementation of the face tracker (using Ocean-based pipeline).
				std::shared_ptr<facebook::facetracker::FaceTrackerManagerOcean> facetrackerManager_;

				/// The most recent transformation between head and world.
				HomogenousMatrix4 world_T_head_ = HomogenousMatrix4(false);

				/// The tracker's lock.
				mutable Lock lock_;
		};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	protected:

		/**
		 * Creates a new experience.
		 * @param avatarsAtFloorLevel True, to place avatars wrt to the floor level; False, to place avatars without any adjustments
		 */
		explicit XRPlaygroundSharedSpaceExperience(const bool floorLevelAvatars = true);

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
		 * Returns whether the floor has been detected.
		 * @return True, if so
		 */
		bool hasValidFloor() const;

		/**
		 * Initializes the VERTS network driver.
		 * @param zoneName The name of the VERTS zone, must be valid
		 * @return True, if succeeded
		 */
		virtual bool initializeNetworkZone(const std::string& zoneName);

		/**
		 * Loads content for the shared space.
		 * @param content The content to be loaded, can be a local scene description file or a cloud asset id, must be valid
		 * @param floorLevel True, if the content's origin is located on the floor level (y=0 plane); False, if the content is freely located in the environment
		 * @return True, if succeeded
		 */
		bool loadContent(const std::string& content, const bool floorLevel = true);

		/**
		 * Loads content for the shared space.
		 * The content is given as an existing rendering node.
		 * @param node The rendering node containing the content, must be valid
		 * @param floorLevel True, if the content's origin is located on the floor level (y=0 plane); False, if the content is freely located in the environment
		 * @return True, if succeeded
		 */
		bool loadContent(const Rendering::NodeRef& node, const bool floorLevel = true);

		/**
		 * Returns the visibility of the local avatar.
		 * @return True, if the local avatar is currently rendered; False, if the local avatar is currently not rendered
		 */
		bool localAvatarVisible() const;

		/**
		 * Sets the visibility of the local avatar.
		 * @param state True, to render the local avatar; False, to hide the local avatar
		 */
		void setLocalAvatarVisible(const bool state);

		/**
		 * Enables or disables user movements.
		 * @param enable True, to enable user movements; False, to disable user movements
		 */
		virtual void setUserMovementEnabled(const bool enable);

		/**
		 * Returns whether user movements are currently enabled.
		 * @return True, if so
		 */
		virtual bool isUserMovementEnabled() const;

		/**
		 * Handles the user's movement.
		 * @param timestamp The current timestamp, must be valid
		 */
		virtual void handleUserMovement(const Timestamp& timestamp);

		/**
		 * The event function called to define the initial user location in shared space.
		 * @param remoteHeadsetWorld_T_remoteAvatar The initial user location to be used, must be valid
		 */
		virtual void onInitializeLocalUser(HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar);

		/**
		 * The event function for changed users.
		 * @param zoneName The name of the zone to which the avatars belong
		 * @param addedAvatars The avatars which have been added since the last event
		 * @param removedAvatars The avatars which have been removed since the last event
		 */
		void onChangedAvatars(const std::string& zoneName, const Platform::Meta::Avatars::Manager::UserPairs& addedAvatars, const Indices64& removedAvatars);

		/**
		 * The event function for a new rendering instance of an avatar.
		 * @param userId The id of the user of the new instance
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		virtual void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform);

		/**
		 * The event function called when an avatar has been removed.
		 * @param userId The id of the user associated with the avatar
		 */
		virtual void onAvatarRemoved(const uint64_t userId);

		/**
		 * Event callback function for handled content.
		 * @param content The content which has been handled
		 * @param succeeded True, if the content could be handled; False, if the content could not be handled
		 * @param scenes The rendering scenes that have been created when handling the content
		 * @param floorLevel True, if the content's origin is located on the floor level (y=0 plane); False, if the content is freely located in the environment
		 */
		virtual void onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes, const bool floorLevel);

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

#endif

	protected:

		/// The rendering Transform node holding the shared space.
		Rendering::TransformRef renderingTransformSharedSpace_;

		/// The rendering Group node holding the local avatar.
		Rendering::GroupRef renderingGroupLocalAvatar_;

		/// The rendering Group node holding the remote avatars.
		Rendering::GroupRef renderingGroupRemoteAvatars_;

		/// The rendering Transform node holding the environment content of the shared space.
		Rendering::TransformRef renderingTransformEnvironmentContent_;

		/// The rendering Transform node holding the environment content of the shared space, at floor level.
		Rendering::TransformRef renderingTransformEnvironmentContentFloorLevel_;

		/// True, if the floor has been detected.
		bool hasValidFloor_ = false;

		/// The unique id of the local user.
		uint64_t localUserId_ = 0ull;

		/// The subscription objects for the zone.
		Platform::Meta::Avatars::Manager::ZoneScopedSubscription zoneScopedSubscription_;

		/// The subscription objects for the avatars.
		Platform::Meta::Avatars::Manager::AvatarScopedSubscriptionMap avatarScopedSubscriptionMap_;

		/// The subscription object for changed avatars events.
		Platform::Meta::Avatars::Manager::ChangedAvatarsScopedSubscription changedAvatarsScopedSubscription_;

		/// True, to place avatars wrt to the floor level; False, to place avatars without any adjustments.
		bool avatarsAtFloorLevel_ = true;

		/// The VERTS driver for the public zone.
		Network::Verts::SharedDriver vertsDriver_;

		/// The floor tracker.
		std::shared_ptr<FloorTracker> floorTracker_;

		/// True, if user movements are currently enabled.
		bool userMovementEnabled_ = false;

		/// The content thas is currently loaded.
		std::string currentContent_;

		/// The map mapping user ids to rendering Transform nods of avatars, needed for shadows.
		AvatarRenderingMap avatarRenderingMap_;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The timestamp when the user started a jump interaction.
		Timestamp jumpStartTimestamp_;

#else

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The 6-DOF SLAM world tracker.
		Devices::Tracker6DOFRef slamTracker_;

		/// The face tracker to be used.
		std::shared_ptr<FaceTracker> faceTracker_;

		/// The transformation between SLAM camera and SLAM world at the start of a user movement.
		HomogenousMatrix4 world_T_slamCameraStart_ = HomogenousMatrix4(false);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

};

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_SHARED_SPACE_EXPERIENCE_H
