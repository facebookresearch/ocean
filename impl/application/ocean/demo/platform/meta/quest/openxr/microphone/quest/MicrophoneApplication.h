/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_MICROPHONE_OPEN_XR_MICROPHONE_APPLICATION_H
#define OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_MICROPHONE_OPEN_XR_MICROPHONE_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplication.h"

#include "ocean/platform/meta/quest/platformsdk/Microphone.h"

#include "ocean/rendering/LineStrips.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/VertexSet.h"

using namespace Ocean;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class MicrophoneApplication : public Platform::Meta::Quest::OpenXR::Application::VRNativeApplication
{
	protected:

		/// The size of the frequency historty which will be visualized, with range [1, infinity)
		constexpr static size_t historySize_ = 50;

		/**
		 * Definition of a vector holding float values.
		 */
		using Values = std::vector<float>;

		/**
		 * Definition of a vector holding float vectors.
		 */
		using ValuesGroup = std::vector<Values>;

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit MicrophoneApplication(struct android_app* androidApp);

	protected:

		/**
		 * Returns the application's base space.
		 * @see VRNativeApplication::baseSpace().
		 */
		XrSpace baseSpace() const override;

		/**
		 * Event function call when an Android (or Oculus) permission is granted.
		 * @see NativeApplication::onAndroidPermissionGranted().
		 */
		void onAndroidPermissionGranted(const std::string& permission) override;

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Updates the rendering information of the microphone spectrum.
		 */
		void updateMicrophoneSpectrum();

		/**
		 * The event function for new microphone samples.
		 * @see PlatformSDK::Microphone::SampleCallback.
		 */
		void onMicrophoneSample(const int16_t* elements, const size_t size);

	protected:

		/// The rendering Transform node holding the rendering content.
		Rendering::TransformRef renderingTransform_;

		/// The rendering VertexSet object holding the microphone spectrum.
		Rendering::VertexSetRef renderingVertexSet_;

		/// The indicies of the line strips associated with the vertices.
		Rendering::LineStripsRef renderingLineStrips_;

		/// The subscription object for the microphone.
		Platform::Meta::Quest::PlatformSDK::Microphone::ScopedSubscription microphoneSubscription_;

		/// The recenty history of the microphone frequencies.
		ValuesGroup frequenciesHistory_;

		/// The new mirophone sample, as frequencies, empty if no new sample is available.
		Values newSample_;

		/// Reusable memory for vertices.
		Vectors3 reusableVertices_;

		/// Reusable memory for colors.
		RGBAColors reusableColorsPerVertex_;

		/// Reusable memory for indices.
		Rendering::VertexIndexGroups lineIndexGroups_;

		/// The counter for microphone samples.
		size_t sampleCounter_ = 0;

		/// The lock for new microphone samples.
		Lock lock_;
};

#endif // OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_MICROPHONE_OPEN_XR_MICROPHONE_APPLICATION_H
