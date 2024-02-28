// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_QUEST_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_QUEST_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsExperience.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/network/verts/Driver.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"

	#include "ocean/platform/meta/avatars/Manager.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience on a Quest.
 * @ingroup xrplayground
 */
class MetaAvatarsQuestExperience : public MetaAvatarsExperience
{
	protected:

		/**
		 * Definition of individual application states.
		 */
		enum ApplicationState
		{
			/// The application is idling.
			AS_IDLE,
			/// The user has to decide which mode is used.
			AS_USER_SELECTING_MODE,
			/// The user is staying alone.
			AS_STAYING_ALONE,
			/// The user wants a clone of himself/herself
			AS_MAKE_CLONE,
			/// The user is inviting someone to their room.
			AS_INVITING_SOMEONE,
			/// The user is joining a private zone.
			AS_JOINING_PRIVATE_ZONE,
			/// The user is joining the public zone.
			AS_JOINING_PUBLIC_ZONE,
			/// The user joined a zone.
			AS_JOINED_ZONE
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaAvatarsQuestExperience() override;

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
		 * The event function for a new rendering instance of an avatar.
		 * @param userId The id of the user of the new instance
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform) override;

		/**
		 * Returns the zone name for a given session id.
		 * @param sessionId The session id for which the zone name will be returned, must be valid
		 * @return The name of the zone
		 */
		static std::string sessionIdZoneName(const std::string& sessionId);

	protected:

		/// The application's state.
		ApplicationState applicationState_ = AS_IDLE;

		/// The unique session id, in case the user is not in a public zone.
		std::string sessionId_;

		/// The future containing the scoped group presence object.
		std::future<Platform::Meta::Quest::PlatformSDK::Multiplayer::ScopedGroupPresence> scopedGroupPresenceFuture_;

		/// The scoped group presence object.
		Platform::Meta::Quest::PlatformSDK::Multiplayer::ScopedGroupPresence scopedGroupPresence_;

		/// The future containing whether an invite was sent successfully.
		std::future<bool> sentInvitesFuture_;

		/// The 6-DOF floor tracker.
		Devices::Tracker6DOFRef floorTracker6DOF_;

		/// The elevation of the floor within the y-axis.
		Scalar yFloorElevation_ = Numeric::minValue();

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience on a Quest.
 * @ingroup xrplayground
 */
class MetaAvatarsQuestExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaAvatarsQuestExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_QUEST_EXPERIENCE_H
