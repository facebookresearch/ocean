// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CROSSPLATFORMHANGOUT_CROSS_PLATFORM_HANGOUT_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CROSSPLATFORMHANGOUT_CROSS_PLATFORM_HANGOUT_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundSharedSpaceExperience.h"

#include "ocean/rendering/Text.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"

	#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience implements an experience in which users can hangout together across individual platforms.
 * @ingroup xrplayground
 */
class CrossPlatformHangoutExperience : protected XRPlaygroundSharedSpaceExperience
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
			/// The user is inviting someone to their session.
			AS_INVITING_SOMEONE,
			/// The user is joining a private zone.
			AS_JOINING_PRIVATE_ZONE,
			/// The user is joining the public zone.
			AS_JOINING_PUBLIC_ZONE,
			/// The user is in the shared space.
			AS_IN_SHARED_SPACE
		};

		/**
		 * Definition of a pair combining a content and timestamp.
		 */
		typedef std::pair<std::string, Timestamp> ContentPair;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~CrossPlatformHangoutExperience() override;

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
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Mouse press event function.
		 * @see Experience::onMousePress().
		 */
		void onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Mouse release event function.
		 * @see Experience::onMouseRelease().
		 */
		void onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * The event function called to define the initial user location in shared space.
		 * @see XRPlaygroundSharedSpaceExperience::onInitializeLocalUser().
		 */
		void onInitializeLocalUser(HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar) override;

		/**
		 * Initializes the VERTS network driver.
		 * @see XRPlaygroundSharedSpaceExperience::initializeNetworkZone().
		 */
		bool initializeNetworkZone(const std::string& zoneName) override;

		/**
		 * Switches the content of this experience
		 * @param content The content to used, "DEFAULT" to use the default environment
		 * @param engine The rendering engine to be used, must be valid
		 */
		void switchContent(const std::string& content, const Rendering::EngineRef& engine);

		/**
		 * The event function for new entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		void onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity);

		/**
		 * Returns the zone name for a given session id.
		 * @param sessionId The session id for which the zone name will be returned, must be valid
		 * @return The name of the zone
		 */
		static std::string sessionIdZoneName(const std::string& sessionId);

		/**
		 * Creates a simple default environment.
		 * @param engine The rendering engine to be used, must be valid
		 * @return The resulting Transform node holding the environment
		 */
		static Rendering::TransformRef createDefaultEnvironment(const Rendering::EngineRef& engine);

	protected:

		/// The application's state.
		ApplicationState applicationState_ = AS_IDLE;

		/// The rendering Text node for general information.
		Rendering::TextRef renderingText_;

		/// The rendering Text node for movement information.
		Rendering::TextRef renderingTextUserMovement_;

		/// The screen position when the user pushed the screen.
		Vector2 screenPositionPressed_ = Vector2(Numeric::minValue(), Numeric::minValue());

		/// The unique session id, in case the user is not in a public zone.
		std::string sessionId_;

		/// The VERTS node containing the content to be loaded.
		Network::Verts::SharedNode vertsContentNode_;

		/// The subscription object for new entity events.
		Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;

		/// The content pair holding the content to be loaded next.
		ContentPair contentPair_;

		/// The name of the public verts zone.
		const std::string vertsPublicZoneName_ = "XRPlayground://CROSS_PLATFORM_HANGOUT_EXPERIENCE_PUBLIC_ZONE";

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

		/// The future containing the scoped group presence object.
		std::future<Platform::Meta::Quest::PlatformSDK::Multiplayer::ScopedGroupPresence> scopedGroupPresenceFuture_;

		/// The scoped group presence object.
		Platform::Meta::Quest::PlatformSDK::Multiplayer::ScopedGroupPresence scopedGroupPresence_;

		/// The future containing whether an invite was sent successfully.
		std::future<bool> sentInvitesFuture_;

		/// True, to show the content menu.
		bool showContentMenu_ = false;

#else

		/// True to toggle between AR and VR mode.
		bool toggleMode_ = false;

#endif
};

#else // OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience shows how to use Avatars in an experience on a Quest.
 * @ingroup xrplayground
 */
class CrossPlatformHangoutExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~CrossPlatformHangoutExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CROSSPLATFORMHANGOUT_CROSS_PLATFORM_HANGOUT_EXPERIENCE_H
