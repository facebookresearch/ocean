// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AVATAR_MIRROR_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AVATAR_MIRROR_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE
	#include "metaonly/ocean/platform/meta/avatars/Manager.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE

/**
 * This experience is implementing a mirrored avatar located in the view without any World/SLAM tracking.
 * @ingroup xrplayground
 */
class AvatarMirrorExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	public:

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
		 * Creates a new AvatarMirrorExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * The event function for a new rendering instance of an avatar.
		 * @param userId The id of the user of the new instance
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform);

		/**
		 * Thread run fucntion.
		 */
		void threadRun() override;

	protected:

		/// The input medium for the face tracker.
		Media::FrameMediumRef faceTrackerFrameMedium_;

		/// The subscription object for the avatar.
		Platform::Meta::Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription_;

		/// The latest transformation between head and user facing camera, invalid if unknown.
		HomogenousMatrix4 userFacingCamera_T_head_ = HomogenousMatrix4(false);

		/// The experience's lock.
		Lock lock_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

/**
 * This experience shows how to use Avatars in an experience on a phone.
 * @ingroup xrplayground
 */
class AvatarMirrorExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~AvatarMirrorExperience() override;

		/**
		 * Creates a new AvatarMirrorExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AVATAR_MIRROR_EXPERIENCE_H
