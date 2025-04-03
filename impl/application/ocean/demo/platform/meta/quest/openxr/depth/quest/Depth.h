/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_DEPTH_QUEST_DEPTH_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_DEPTH_QUEST_DEPTH_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/rendering/Group.h"

using namespace Ocean;
using namespace Ocean::Platform::Meta::Quest;
using namespace Ocean::Platform::Meta::Quest::OpenXR::Application;

/**
 * Implements a specialization of the VRNativeApplicationAdvanced.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class Depth final : public VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit Depth(struct android_app* androidApp);

	protected:

		/**
		 * Returns the names of the necessary OpenXR extensions the application needs.
		 * @see VRNativeApplicationAdvanced::necessaryOpenXRExtensionNames().
		 */
		StringSet necessaryOpenXRExtensionNames() const override;

		/**
		 * Event function call when an Android (or Oculus) permission is granted
		 * @see VRNativeApplication::onAndroidPermissionGranted().
		 */
		void onAndroidPermissionGranted(const std::string& permission) override;

		/**
		 * Event function called whenever the session is ready, when the session state changed to XR_SESSION_STATE_READY.
		 * @see VRNativeApplicationAdvanced::onOpenXRSessionReady().
		 */
		void onOpenXRSessionReady() override;

		/**
		 * Event function called whenever the session is stopping, when the session state changed to XR_SESSION_STATE_STOPPING.
		 * @see VRNativeApplicationAdvanced::onOpenXRSessionStopping().
		 */
		void onOpenXRSessionStopping() override;

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
		 * Event functions for pressed buttons (e.g., from a tracked controller).
		 * @see VRNativeApplication::onButtonPressed().
		 */
		void onButtonPressed(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp) override;

		/**
		 * Initializes the depth API.
		 * @return True, if succeeded
		 */
		bool initializeDepth();

		/**
		 * Extracts both depth maps from a OpenGLES texture array.
		 * @param depthTexture The depth texture array to extract the depth maps from, must be valid
		 * @param width The width of the depth texture array, with range [1, infinity)
		 * @param height The height of the depth texture array, with range [1, infinity)
		 * @param depthFrameA The resulting first depth frame, must be valid
		 * @param depthFrameB The resulting second depth frame, must be valid
		 * @return True, if succeeded
		 */
		bool extract(const GLuint depthTexture, const uint32_t width, const uint32_t height, Frame& depthFrameA, Frame& depthFrameB);

		/**
		 * Initializes the function pointer of an OpenXR function.
		 * @param xrInstance The instance of the OpenXR session, must be valid
		 * @param functionName The name of the function to be initialized, must be valid
		 * @param fucntion The resulting function pointer
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool initializeFunctionPointer(const XrInstance& xrInstance, const char* functionName, T& function);

	protected:

		/// The rendering Group object for all rendering objects.
		Rendering::GroupRef renderingGroup_;

		/// True, if the depth provider is running.
		bool isDepthProviderRunning_ = false;

		/// The OpenXR environment depth provider.
		XrEnvironmentDepthProviderMETA xrEnvironmentDepthProvider_ = XR_NULL_HANDLE;

		/// The OpenXR environment depth swapchain.
		XrEnvironmentDepthSwapchainMETA xrEnvironmentDepthSwapchainMETA_ = XR_NULL_HANDLE;

		/// The OpenXR environment depth swapchain state.
		XrEnvironmentDepthSwapchainStateMETA xrEnvironmentDepthSwapchainStateMETA_ = {};

		/// The OpenXR swapchain images using OpenGLES.
		std::vector<XrSwapchainImageOpenGLESKHR> swapChainImages_;

		/// The Meta specific OpenXR extension function pointers.
		PFN_xrCreateEnvironmentDepthProviderMETA xrCreateEnvironmentDepthProviderMETA_ = nullptr;
		PFN_xrDestroyEnvironmentDepthProviderMETA xrDestroyEnvironmentDepthProviderMETA_ = nullptr;
		PFN_xrStartEnvironmentDepthProviderMETA xrStartEnvironmentDepthProviderMETA_ = nullptr;
		PFN_xrStopEnvironmentDepthProviderMETA xrStopEnvironmentDepthProviderMETA_ = nullptr;
		PFN_xrCreateEnvironmentDepthSwapchainMETA xrCreateEnvironmentDepthSwapchainMETA_ = nullptr;
		PFN_xrDestroyEnvironmentDepthSwapchainMETA xrDestroyEnvironmentDepthSwapchainMETA_ = nullptr;
		PFN_xrEnumerateEnvironmentDepthSwapchainImagesMETA xrEnumerateEnvironmentDepthSwapchainImagesMETA_ = nullptr;
		PFN_xrGetEnvironmentDepthSwapchainStateMETA xrGetEnvironmentDepthSwapchainStateMETA_ = nullptr;
		PFN_xrAcquireEnvironmentDepthImageMETA xrAcquireEnvironmentDepthImageMETA_ = nullptr;
		PFN_xrSetEnvironmentDepthHandRemovalMETA xrSetEnvironmentDepthHandRemovalMETA_ = nullptr;

		/// The texture framebuffer which will be used as an intermediate helper framebuffer to be able copy the depth map to memory.
		Rendering::TextureFramebufferRef textureFramebuffer_;

		/// The shader program for rendering the texture texture to the intermediate framebuffer.
		Rendering::ShaderProgramRef shaderProgram_;

		/// The vertex set object holding the vertices of the two triangles when rendering the depth textures to the depth texture framebuffer.
		Rendering::VertexSetRef vertexSet_;

		/// The triangles object holding two triangles when rendering the depth textures to the depth texture framebuffer.
		Rendering::TrianglesRef triangles_;

		/// Indicates if the current device supports hand removal.
		bool isHandRemovalSupported_ = false;

		/// Indicates if hand removal is enabled, if applicable.
		bool isHandRemovalEnabled_ = false;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_DEPTH_QUEST_DEPTH_H
