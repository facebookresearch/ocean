/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_THEREMIN_OPEN_XR_THEREMIN_APPLICATION_H
#define OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_THEREMIN_OPEN_XR_THEREMIN_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/media/LiveAudio.h"

#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

using namespace Ocean;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class ThereminApplication : public Platform::Meta::Quest::OpenXR::Application::VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit ThereminApplication(struct android_app* androidApp);

	protected:

		/**
		 * Returns the application's base space.
		 * @see VRNativeApplication::baseSpace().
		 */
		XrSpace baseSpace() const override;

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Renders the next audio sample.
		 * @param frequency The frequency of the sample, in Hz, with range [20, 20000]
		 */
		void renderAudioSample(const float frequency);

	protected:

		/// The rendering Transform node holding the pitch antenna.
		Rendering::TransformRef renderingTransformPitchAntenna_;

		/// The rendering Transform node holding the volume antenna.
		Rendering::TransformRef renderingTransformVolumeAntenna_;

		/// The rendering Text node for the frequency.
		Rendering::TextRef renderingTextFrequency_;

		/// The rendering Text node for the volume.
		Rendering::TextRef renderingTextVolume_;

		/// The speaker of this experience.
		Media::LiveAudioRef liveAudio_;

		/// The reusable buffer holding an audio sample.
		std::vector<int16_t> reusableSample_;

		/// The frequency of the last sample.
		float previousFrequency_ = -1.0f;

		/// The phase for the next sample.
		float phase_ = 0.0f;
};

#endif // OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_THEREMIN_OPEN_XR_THEREMIN_APPLICATION_H
