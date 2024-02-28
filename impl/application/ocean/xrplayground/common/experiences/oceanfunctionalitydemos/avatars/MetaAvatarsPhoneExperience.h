// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_PHONE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_PHONE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/math/Plane3.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/network/verts/Driver.h"

#include "ocean/rendering/Text.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE
	#include <facetracker/manager/FaceTrackerManager_Ocean.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE

/**
 * This experience shows how to use Avatars in an experience on a phone.
 * @ingroup xrplayground
 */
class MetaAvatarsPhoneExperience :
	public MetaAvatarsExperience,
	protected Thread
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * Definition of an unordered map mapping user ids to rendering Transform nodes.
		 */
		typedef std::unordered_map<uint64_t, Rendering::TransformRef> AvatarRenderingMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaAvatarsPhoneExperience() override;

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
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Initializes the facetracker.
		 * @return True, if succeeded
		 */
		bool initializeFacetracker();

		/**
		 * The event function for a new rendering instance of an avatar.
		 * @param userId The id of the user of the new instance
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform) override;

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * Thread run function for the standalone face tracker, used on Android.
		 */
		void threadRun() override;

		/**
		 * Updates the group plane.
		 * @return True, if succeeded
		 */
		bool updateGroundPlane();

	protected:

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The rendering Group object holding all objects anchored with SLAM.
		Rendering::GroupRef renderingGroupAnchored_;

		/// The map mapping user ids to rendering Transform nods of avatars.
		AvatarRenderingMap avatarRenderingMap_;

		/// True, if a remote user exists already.
		bool remoteUserExists_ = false;

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The 6-DOF SLAM tracker.
		Devices::Tracker6DOFRef slamTracker6DOF_;

		/// The 6-DOF Plane tracker.
		Devices::SceneTracker6DOFRef planeTracker6DOF_;

		/// The 6-DOF face tracker, used on iOS.
		Devices::Tracker6DOFRef faceTracker6DOF_;

		/// The media device of the user-facing camera.
		Media::FrameMediumRef userFacingFrameMedium_;

		/// The implementation of the face tracker (using Ocean-based pipeline).
		std::shared_ptr<facebook::facetracker::FaceTrackerManagerOcean> facetrackerManager_;

		/// The most recent transformation between head and world.
		HomogenousMatrix4 world_T_head_ = HomogenousMatrix4(false);

		/// The group plane defined in world, invalid if unknown.
		Plane3 groundPlane_;

		/// The timestamp when the ground plane was updated the last time.
		Timestamp groupPlaneTimestamp_;

		/// The experience's lock.
		Lock lock_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

/**
 * This experience shows how to use Avatars in an experience on a phone.
 * @ingroup xrplayground
 */
class MetaAvatarsPhoneExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaAvatarsPhoneExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_PHONE_EXPERIENCE_H
