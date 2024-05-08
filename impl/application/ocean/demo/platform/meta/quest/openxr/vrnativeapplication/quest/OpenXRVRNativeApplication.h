/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplication.h"

using namespace Ocean;

/**
 * Implements a specialization of the VRNativeApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class OpenXRVRNativeApplication : public Platform::Meta::Quest::OpenXR::Application::VRNativeApplication
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit OpenXRVRNativeApplication(struct android_app* androidApp);

	protected:

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_VRNATIVEAPPLICATION_OPEN_XR_VR_NATIVE_APPLICATION_H
