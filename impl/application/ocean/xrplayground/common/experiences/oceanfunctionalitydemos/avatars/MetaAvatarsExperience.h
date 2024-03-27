// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	#include "metaonly/ocean/platform/meta/avatars/Manager.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience is the base class for all multi-user Avatars experiences allowing to share the same space.
 * @ingroup xrplayground
 */
class MetaAvatarsExperience : public XRPlaygroundExperience
{
	protected:

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
		virtual void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform) = 0;

	protected:

		/// The rendering Transform object holding the remote avatars.
		Rendering::TransformRef renderingTransformRemoteAvatars_;

		/// The unique id of the user.
		uint64_t userId_ = 0ull;

		/// The subscription objects for the zone.
		Platform::Meta::Avatars::Manager::ZoneScopedSubscription zoneScopedSubscription_;

		/// The subscription object for changed avatars events.
		Platform::Meta::Avatars::Manager::ChangedAvatarsScopedSubscription changedAvatarsScopedSubscription_;

		/// The subscription objects for the avatars.
		Platform::Meta::Avatars::Manager::AvatarScopedSubscriptions avatarScopedSubscriptions_;

		/// The name of the public verts zone.
		const std::string vertsPublicZoneName_ = "XRPlayground://AVATARS_EXPERIENCE_PUBLIC_ZONE";
};

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_META_AVATARS_EXPERIENCE_H
