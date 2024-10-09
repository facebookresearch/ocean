/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_HANDGESTURES_QUEST_HAND_GESTURES_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_HANDGESTURES_QUEST_HAND_GESTURES_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

/**
 * Implements a specialization of the VRNativeApplicationAdvanced.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class HandGestures final : public Ocean::Platform::Meta::Quest::OpenXR::Application::VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit HandGestures(struct android_app* androidApp);

	protected:

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplicationAdvanced::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Ocean::Timestamp& predictedDisplayTime) override;

		/**
		 * Event functions for pressed buttons (e.g., from a tracked controller).
		 * @see VRNativeApplication::onButtonPressed().
		 */
		void onButtonPressed(const Ocean::Platform::Meta::Quest::OpenXR::TrackedController::ButtonType buttons, const Ocean::Timestamp& timestamp) override;

	protected:

		/// The rendering text node for the left hand.
		Ocean::Rendering::TextRef renderingTextHandLeft_;

		/// The rendering text node for the right hand.
		Ocean::Rendering::TextRef renderingTextHandRight_;

		/// The rendering transform node for the left hand.
		Ocean::Rendering::TransformRef renderingTransformHandLeft_;

		/// The rendering transform node for the right hand.
		Ocean::Rendering::TransformRef renderingTransformHandRight_;

		/// The rendering group node holding hand joint indices.
		Ocean::Rendering::GroupRef renderingGroupHandJoints_;

		/// The rendering group node holding hand gestures.
		Ocean::Rendering::GroupRef renderingGroupHandGestures_;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_HANDGESTURES_QUEST_HAND_GESTURES_H
