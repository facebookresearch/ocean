// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_VERTS_NETWORK_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_VERTS_NETWORK_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/rendering/Text.h"

#include "ocean/network/verts/Driver.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to use network functions from VERTS.
 * @ingroup xrplayground
 */
class VertsNetworkExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~VertsNetworkExperience() override;

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
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Event function for received data on a channel.
		 * @param driver The VERTS driver sending the event
		 * @param channelIndex The index of the channel, with range [0, 7]
		 * @param buffer The received data, must be valid
		 * @param size The size of the received data, in bytes, with range [1, infinity)
		 */
		void onReceiveOnChannel(Network::Verts::Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size);

		/**
		 * Event function for received data on a channel.
		 * @param driver The VERTS driver sending the event
		 * @param sessionId The session id of the sender, will be valid
		 * @param userId The user id of the sender, will be valid
		 * @param identifier The identifier of the container, will be valid
		 * @param version The version of the container, with range [0, infinity)
		 * @param buffer The buffer of the container
		 */
		void onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer);

	protected:

		/// True, to send data on a channel.
		bool sendDataOnChannel_ = false;

		/// True, to send a container.
		bool sendDataAsContainer_ = false;

		/// The current container version.
		unsigned int containerVersion_ = 0u;

		/// The rendering Text object for some text output.
		Rendering::TextRef renderingTextA_;

		/// The rendering Text object for some text output.
		Rendering::TextRef renderingTextB_;

		/// The rendering Text object for some text output.
		Rendering::TextRef renderingTextC_;

		/// The bytes already sent on a channel.
		size_t bytesSentOnChannel_ = 0;

		/// The bytes already sent as a container.
		size_t bytesSentAsContainer_ = 0;

		/// The bytes already received on a channel.
		size_t bytesReceivedOnChannel_ = 0;

		/// The bytes already as container.
		size_t bytesReceivedAsContainer_ = 0;

		/// The VERTS driver.
		Network::Verts::SharedDriver vertsDriver_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveOnChannelScopedSubscription receiveOnChannelScopedSubscription_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveContainerScopedSubscription receiveContainerScopedSubscription_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_VERTS_NETWORK_EXPERIENCE_H
