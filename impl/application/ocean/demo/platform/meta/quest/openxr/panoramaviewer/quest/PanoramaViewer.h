/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_PANORAMAVIEWER_QUEST_PANORAMA_VIEWER_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_PANORAMAVIEWER_QUEST_PANORAMA_VIEWER_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/io/File.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

using namespace Ocean;
using namespace Platform::Meta::Quest::OpenXR::Application;

/**
 * Implements a specialization of the VRNativeApplicationAdvanced.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class PanoramaViewer final : public VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit PanoramaViewer(struct android_app* androidApp);

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
		 * @see VRApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Creates a textured sphere and inserts the sphere into the rendering pipeline.
		 * @param engine The rendering engine used to render the sphere, must be valid
		 * @param imageFile The file of the image which will be used as texture, must exist
		 * @return The resulting rendering Transform node which will contain the sphere, invalid if something went wrong
		 */
		static Rendering::TransformRef createTexturedSphere(const Rendering::EngineRef& engine, const IO::File& imageFile);

	protected:

		/// The rendering object for the image credits.
		Rendering::TransformRef renderingTextImageCredits_;

		/// The timestamp until the image credits will shown.
		Timestamp imageCreditsVisibleTimestamp_;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_PANORAMAVIEWER_QUEST_PANORAMA_VIEWER_H
