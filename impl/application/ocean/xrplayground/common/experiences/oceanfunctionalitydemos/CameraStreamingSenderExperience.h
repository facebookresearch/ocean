// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_SENDER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_SENDER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/media/FrameMedium.h"

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
class CameraStreamingSenderExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~CameraStreamingSenderExperience() override;

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
		 * Creates a new CameraStreamingSenderExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		CameraStreamingSenderExperience() = default;

		/**
		 * The event function for changed users.
		 * @param driver The driver sending the event
		 * @param addedUsers The ids of all added users
		 * @param removedUsers The ids of all removed users
		 */
		void onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers);

	protected:

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The frame medium which will be used as source for the camera stream.
		Media::FrameMediumRef frameMedium_;

		/// The timestamp of the last camera frame which has been streamed.
		Timestamp lastCameraTimestamp_;

		/// The counter for camera frames.
		unsigned int cameraFrameCounter_ = 0u;

		/// The network driver.
		Network::Verts::SharedDriver vertsDriver_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CAMERA_STREAMING_SENDER_EXPERIENCE_H
