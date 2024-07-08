/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_ADVANCED_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_ADVANCED_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

using namespace Ocean;
using namespace Platform::Meta::Quest::OpenXR::Application;

/**
 * Implements a specialization of the VRNativeApplicationAdvanced.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class OpenXRVRNativeApplicationAdvanced final : public VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit OpenXRVRNativeApplicationAdvanced(struct android_app* androidApp);

	protected:

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

	protected:

		/// The timestamp when the hand visualization mode changes the next time.
		Timestamp handVisualizationModeTimestamp_;

		/// The current index of the hand visualization render mode.
		size_t renderModeIndex_ = 0;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_ADVANCED_H
