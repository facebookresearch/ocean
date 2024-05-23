/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_H

#include "ocean/platform/meta/quest/openxr/application/Application.h"
#include "ocean/platform/meta/quest/openxr/application/NativeApplication.h"

#include "ocean/platform/gles/EGLContext.h"

#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"
#include "ocean/platform/meta/quest/application/VRTextVisualizer.h"

#include "ocean/platform/meta/quest/openxr/TrackedController.h"

#include "ocean/platform/openxr/ActionSet.h"
#include "ocean/platform/openxr/Session.h"
#include "ocean/platform/openxr/Utilities.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/StereoView.h"

#include "ocean/rendering/glescenegraph/quest/openxr/GLESWindowFramebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

/**
 * This class implements a basic OpenXR (VR) application using Ocean's scene graph rendering pipline (Rendering::GLESceneGraph).
 * @ingroup platformmetaquestopenxrapplication
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT VRNativeApplication : public NativeApplication
{
	protected:

		/// The maximal number of supported eyes.
		static constexpr size_t maximalNumberEyes_ = 2;

		/**
		 * Definition of a union allowing to define individual composition layers.
		 */
		union XrCompositorLayerUnion
		{
			/// Composition layer for projection.
			XrCompositionLayerProjection xrCompositionLayerProjection_;

			/// composition layer for a quad.
			XrCompositionLayerQuad xrCompositionLayerQuad_;

			/// Composition layer for a cylinder.
			XrCompositionLayerCylinderKHR xrCompositionLayerCylinderKHR_;

			/// Composition layer for a cube map.
			XrCompositionLayerCubeKHR xrCompositionLayerCubeKHR_;

			/// Composition layer for a equirectangular projection.
			XrCompositionLayerEquirectKHR xrCompositionLayerEquirectKHR_;

			/// A composition layer for passthrough.
			XrCompositionLayerPassthroughFB xrCompositionLayerPassthroughFB_;
		};

		/**
		 * Definition of a vector holding xrCompositorLayerUnion objects.
		 */
		typedef std::vector<XrCompositorLayerUnion> XrCompositorLayerUnions;

		/**
		 * Definition of a vector holding XrCompositionLayerBaseHeader objects.
		 */
		typedef std::vector<const XrCompositionLayerBaseHeader*> XrCompositionLayerBaseHeaders;

		/**
		 * Definition of a vector holding XrActionSet handles.
		 */
		typedef std::vector<XrActionSet> XrActionSets;

		/**
		 * Definition of a vector holding XrActiveActionSet objects.
		 */
		typedef std::vector<XrActiveActionSet> XrActiveActionSets;

	public:

		/**
		 * Destructs this object.
		 */
		~VRNativeApplication() override;

	protected:

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit VRNativeApplication(struct android_app* androidApp);

#endif // OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Disabled copy constructor.
		 */
		VRNativeApplication(const NativeApplication&) = delete;

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
		 * Main loop loop of the application.
		 * @see NativeApplication::applicationLoop().
		 */
		void applicationLoop() override;

		/**
		 * Returns the names of the necessary OpenXR extensions the application needs.
		 * @see NativeApplication::necessaryOpenXRExtensionNames().
		 */
		StringSet necessaryOpenXRExtensionNames() const override;

		/**
		 * Registers the system fonts.
		 */
		void registerSystemFonts();

		/**
		 * Renders a new frame.
		 * @param shouldRender True, if the frame should actually be rendered; False, to frame should actually be skipped
		 * @param xrPredictedDisplayTime The predicted display time as provided, with range (0, infinity)
		 * @param renderTimestamp The timestamp of the render call, must be valid
		 */
		virtual void render(const bool shouldRender, const XrTime& xrPredictedDisplayTime, const Timestamp& renderTimestamp);

		/**
		 * Returns the application's base space.
		 * @return The base space which is currently used
		 */
		virtual XrSpace baseSpace() const;

		/**
		 * Locates a space in relation to the applications' base space.
		 * @param xrSpace The space to locate, must be valid
		 * @param xrTime The time for which the pose will be determined, must be valid
		 * @param xrSpaceLocationFlags Optional resulting location flags, nullptr if not of interest
		 * @return The resulting transformation between the located space and the base space, will be baseSpace_T_space, invalid in case of an error
		 * @see baseSpace().
		 */
		template <typename T = Scalar>
		HomogenousMatrixT4<T> locateSpace(const XrSpace& xrSpace, const XrTime& xrTime, XrSpaceLocationFlags* xrSpaceLocationFlags = nullptr);

		/**
		 * Locates a space in relation to the applications' base space.
		 * @param xrSpace The space to locate, must be valid
		 * @param timestamp The time for which the pose will be determined, must be valid
		 * @param xrSpaceLocationFlags Optional resulting location flags, nullptr if not of interest
		 * @return The resulting transformation between the located space and the base space, will be baseSpace_T_space, invalid in case of an error
		 * @see baseSpace().
		 */
		template <typename T = Scalar>
		HomogenousMatrixT4<T> locateSpace(const XrSpace& xrSpace, const Timestamp& timestamp, XrSpaceLocationFlags* xrSpaceLocationFlags = nullptr);

		/**
		 * Returns the TrackedController object allowing access to controller events.
		 * @return The desired object
		 */
		inline TrackedController& trackedController();

		/**
		 * Event function called whenever the session is ready, when the session state changed to XR_SESSION_STATE_READY.
		 * @see NativeApplication::onOpenXRSessionReady().
		 */
		void onOpenXRSessionReady() override;

		/**
		 * Event function called whenever the session is stopping, when the session state changed to XR_SESSION_STATE_STOPPING.
		 * @see NativeApplication::onOpenXRSessionStopping().
		 */
		void onOpenXRSessionStopping() override;

		/**
		 * Event function called to configure the action sets.
		 * @param actionSets The action sets to configure
		 */
		virtual void onConfigureActionSets(Platform::OpenXR::SharedActionSets& actionSets);

		/**
		 * Event function called to release the action sets or/and resources associated with action sets.
		 */
		virtual void onReleaseActionSets();

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 * @see NativeApplication::onIdle().
		 */
		void onIdle() override;

		/**
		 * The event function which is called when all resources should finally be released.
		 */
		virtual void onReleaseResources();

		/**
		 * Event function called after the framebuffer has been initialized.
		 * This event function can be used to configure the framebuffer or to add rendering content.
		 */
		virtual void onFramebufferInitialized();

		/**
		 * Event function called before the framebuffer will be released.
		 * This event function can be used to release depending resources.
		 */
		virtual void onFramebufferReleasing();

		/**
		 * Event function allows to add custom compositor layers at the very back.
		 * @param xrCompositorLayerUnions The compositor layers to which new layers can be added
		 */
		virtual void onAddCompositorBackLayers(XrCompositorLayerUnions& xrCompositorLayerUnions);

		/**
		 * Event function allows to add custom compositor layers at the very front.
		 * @param xrCompositorLayerUnions The compositor layers to which new layers can be added
		 */
		virtual void onAddCompositorFrontLayers(XrCompositorLayerUnions& xrCompositorLayerUnions);

		/**
		 * Events function called before the scene is rendered.
		 * @param xrPredictedDisplayTime The OpenXR timestamp which will be used for rendering, must be valid
		 * @param predictedDisplayTime The timestamp which will be used for rendering, must be valid
		 */
		virtual void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime);

		/**
		 * Event functions for pressed buttons (e.g., from a tracked controller).
		 * @param buttons The buttons currently pressed
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonPressed(const TrackedController::ButtonType buttons, const Timestamp& timestamp);

		/**
		 * Event functions for released buttons (e.g., from a tracked controller).
		 * @param buttons The buttons currently pressed
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonReleased(const TrackedController::ButtonType buttons, const Timestamp& timestamp);

	protected:

		/// True, to use the stencil buffer.
		bool useStencilBuffer_ = false;

		/// The near distance used for clipping in the projection matrix.
		float nearDistance_ = 0.1f;

		/// The far distance used for clipping in the projection matrix.
		float farDistance_ = 100.0f;

		/// The application's EGL context
		Platform::GLES::EGLContext eglContext_;

		/// The application's OpenXR session.
		Platform::OpenXR::Session xrSession_;

		/// The OpenXR view space.
		Platform::OpenXR::ScopedXrSpace xrSpaceView_;

		/// The OpenXR local space (the headset's world, which can change with the reset button).
		Platform::OpenXR::ScopedXrSpace xrSpaceLocal_;

		/// The OpenXR stage space (the headsets world).
		Platform::OpenXR::ScopedXrSpace xrSpaceStage_;

		/// True, if the OpenXR session is currently running and ready for rendering.
		bool xrSessionIsRunning_ = false;

		/// The OpenXR compositor layer unions (the rendering layers).
		XrCompositorLayerUnions xrCompositorLayerUnions_;

		/// The OpenXR composition layer base headers.
		XrCompositionLayerBaseHeaders xrCompositionLayerBaseHeaders_;

		/// The rendering engine to be used in the VR application.
		Rendering::EngineRef engine_;

		/// The rendering framebuffer to be used in the VR application.
		Rendering::WindowFramebufferRef framebuffer_;

		/// The stereo view to be used in the VR Application.
		Rendering::StereoViewRef stereoView_;

		/// The application's action sets.
		Platform::OpenXR::SharedActionSets actionSets_;

		/// The active action sets.
		XrActiveActionSets xrActiveActionSets_;

		/// The visualizer for images (mainly a helper class creating textured SceneGraph objects to display image content).
		Quest::Application::VRImageVisualizer vrImageVisualizer_;

		/// The visualizer for text (mainly a helper class creating textured SceneGraph objects to display text).
		Quest::Application::VRTextVisualizer vrTextVisualizer_;

	private:

		/// The explicit pointer to the Quest-specialized framebuffer (identical to framebuffer_, but pointing to the derived class).
		Rendering::GLESceneGraph::Quest::OpenXR::GLESWindowFramebuffer* questFramebuffer_ = nullptr;

		/// The tracked controller object providing controller events and poses.
		TrackedController trackedController_;
};

template <typename T>
HomogenousMatrixT4<T> VRNativeApplication::locateSpace(const XrSpace& xrSpace, const XrTime& xrTime, XrSpaceLocationFlags* xrSpaceLocationFlags)
{
	return Platform::OpenXR::Utilities::determinePose(xrSpace, baseSpace(), xrTime, xrSpaceLocationFlags);
}

template <typename T>
HomogenousMatrixT4<T> VRNativeApplication::locateSpace(const XrSpace& xrSpace, const Timestamp& timestamp, XrSpaceLocationFlags* xrSpaceLocationFlags)
{
	const XrTime xrTime(timestamp.nanoseconds());

	return locateSpace(xrSpace, xrTime, xrSpaceLocationFlags);
}

inline TrackedController& VRNativeApplication::trackedController()
{
	return trackedController_;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_H
