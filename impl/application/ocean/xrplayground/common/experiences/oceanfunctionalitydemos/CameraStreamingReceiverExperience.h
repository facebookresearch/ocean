// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_RECEIVER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_RECEIVER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/media/PixelImage.h"

#include "ocean/network/verts/Driver.h"

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to stream camera data from one device to another devices.
 * @ingroup xrplayground
 */
class CameraStreamingReceiverExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~CameraStreamingReceiverExperience() override;

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
		 * Creates a new CameraStreamingReceiverExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		CameraStreamingReceiverExperience() = default;

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

		/// The rendering Text object holding the instruction text.
		Rendering::TextRef renderingText_;

		/// The rendering Transform node holding the box showing the camera stream.
		Rendering::TransformRef renderingTransformBox_;

		/// The pixel image which will hold the received camera stream.
		Media::PixelImageRef pixelImage_;

		/// The most recent frame if any.
		Frame recentFrame_;

		/// The network driver.
		Network::Verts::SharedDriver vertsDriver_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveContainerScopedSubscription receiveContainerScopedSubscription_;

		/// Experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_RECEIVER_EXPERIENCE_H
