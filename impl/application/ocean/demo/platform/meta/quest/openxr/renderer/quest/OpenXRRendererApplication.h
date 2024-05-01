// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_RENDERER_OPEN_XR_RENDERER_APPLICATION_H
#define OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_RENDERER_OPEN_XR_RENDERER_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplication.h"

using namespace Ocean;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class OpenXRRendererApplication : public Platform::Meta::Quest::OpenXR::Application::VRNativeApplication
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit OpenXRRendererApplication(struct android_app* androidApp);

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
		 * Optional explicit callback function for pre-render events.
		 * @param eyeIndex The index of the eye (framebuffer) for which the current image will be rendered
		 * @param view_T_world The current transformation between world and the current view (camera/eye), also known as viewing matrix, must be valid
		 * @param projection The projection matrix of the view (eye), must be valid
		 * @param predictedDisplayTime The predicted timestamp when the image will be display
		 */
		void onPreRenderFramebuffer(const size_t eyeIndex, const HomogenousMatrix4 view_T_world, const SquareMatrix4 projection, const Timestamp predictedDisplayTime);

		/**
		 * Optional explicit callback function for post-render events.
		 * @param eyeIndex The index of the eye (framebuffer) for which the current image will be rendered
		 * @param view_T_world The current transformation between world and the current view (camera/eye), also known as viewing matrix, must be valid
		 * @param projection The projection matrix of the view (eye), must be valid
		 * @param predictedDisplayTime The predicted timestamp when the image will be display
		 */
		void onPostRenderFramebuffer(const size_t eyeIndex, const HomogenousMatrix4 view_T_world, const SquareMatrix4 projection, const Timestamp predictedDisplayTime);
};

#endif // OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_RENDERER_OPEN_XR_RENDERER_APPLICATION_H