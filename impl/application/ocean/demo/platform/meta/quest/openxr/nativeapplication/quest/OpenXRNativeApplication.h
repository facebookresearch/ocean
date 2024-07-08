/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_NATIVEAPPLICATION_OPEN_XR_NATIVE_APPLICATION_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_NATIVEAPPLICATION_OPEN_XR_NATIVE_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/NativeApplication.h"

#include <android_native_app_glue.h>

using namespace Ocean;

/**
 * This application shows how to implement a custom native application which a custom OpenXR session and rendering engine.
 * The relevant functions to be implemented are createOpenXRSession(), releaseOpenXRSession(), onIdle().
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class OpenXRNativeApplication : public Platform::Meta::Quest::OpenXR::Application::NativeApplication
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit OpenXRNativeApplication(struct android_app* androidApp);

	protected:

		/**
		 * Creates the OpenXR session.
		 * @see NativeApplication::createOpenXRSession().
		 */
		bool createOpenXRSession(const XrViewConfigurationViews& xrViewConfigurationViews) override;

		/**
		 * Releases the OpenXR session.
		 * @see NativeApplication::releaseOpenXRSession().
		 */
		void releaseOpenXRSession() override;

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 * @see NativeApplication::onIdle().
		 */
		void onIdle() override;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_NATIVEAPPLICATION_OPEN_XR_NATIVE_APPLICATION_H
