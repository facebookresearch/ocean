// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLATFORM_SDK_NETWORK_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLATFORM_SDK_NETWORK_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/rendering/Text.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
	#include "ocean/platform/meta/quest/platformsdk/Network.h"
	#include "ocean/platform/meta/quest/platformsdk/Room.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use network functions from PlatformSDK.
 * @ingroup xrplayground
 */
class PlatformSDKNetworkExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~PlatformSDKNetworkExperience() override;

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
		 * Event function for connection requests.
		 * @param remoteUserId The id of the remote user who wants to start the connection, must be valid
		 * @return True, to accept the connection; False, to reject the connection
		 */
		bool onConnection(const uint64_t remoteUserId);

		/**
		 * Event function for received data.
		 * @param senderUserId The id of the user sending the data, must be valid
		 * @param componentId The id of the component to which the data belongs, must be valid
		 * @param data The received data, must be valid
		 * @param size The size of the received data, in bytes, with range [1, infinity)
		 * @param connectionType The type of the connection
		 */
		void onReceive(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType);

	protected:

		/// The rendering Text object for some text output.
		Rendering::TextRef renderingTextA_;

		/// The rendering Text object for some text output.
		Rendering::TextRef renderingTextB_;

		/// The id of the remote user.
		uint64_t remoteUserId_ = 0ull;

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

		/// The users which can be invited.
		Platform::Meta::Quest::PlatformSDK::Room::Users invitableUsers_;

		/// True, to send data; False, to pause sending data.
		bool sendingActive_ = false;

		/// The timestamp the next package will be sent.
		Timestamp nextSendTimestamp_;

		/// The bytes already sent.
		size_t bytesSent_ = 0;

		/// The bytes already received.
		size_t bytesReceived_ = 0;

		/// The subscription for connection events.
		Platform::Meta::Quest::PlatformSDK::Network::ConnectionScopedSubscription connectionSubscription_;

		/// The subscription for data receive events.
		Platform::Meta::Quest::PlatformSDK::Network::ReceiveScopedSubscription receiveSubscription_;

		/// The experience's lock.
		Lock lock_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience.
 * @ingroup xrplayground
 */
class PlatformSDKNetworkExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~PlatformSDKNetworkExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLATFORM_SDK_NETWORK_EXPERIENCE_H
