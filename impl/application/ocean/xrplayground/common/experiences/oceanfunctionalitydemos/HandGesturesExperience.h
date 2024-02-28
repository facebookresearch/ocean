// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_HAND_GESTURES_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_HAND_GESTURES_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience is showing XRPlayground's supported hand gestures.
 * @ingroup xrplayground
 */
class HandGesturesExperience : public XRPlaygroundExperience
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
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Creates a new AvatarMirrorExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/// The rendering text node for the left hand.
		Rendering::TextRef renderingTextHandLeft_;

		/// The rendering text node for the right hand.
		Rendering::TextRef renderingTextHandRight_;

		/// The rendering transform node for the left hand.
		Rendering::TransformRef renderingTransformHandLeft_;

		/// The rendering transform node for the right hand.
		Rendering::TransformRef renderingTransformHandRight_;

		/// The rendering group node holding hand joint indices.
		Rendering::GroupRef renderingGroupHandJoints_;

		/// The rendering group node holding hand gestures.
		Rendering::GroupRef renderingGroupHandGestures_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience on a phone.
 * @ingroup xrplayground
 */
class HandGesturesExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Creates a new HandGesturesExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_HAND_GESTURES_EXPERIENCE_H
