// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_GLES_NATIVE_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_GLES_NATIVE_APPLICATION_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"
#include "ocean/platform/meta/quest/vrapi/application/NativeApplication.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

#include "ocean/platform/gles/EGLContext.h"

#include "ocean/platform/meta/quest/vrapi/GLESFramebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

/**
 * This class implements a basic Quest application with OpenGLES context using VrApi.
 * The class is not using Ocean's scene graph rendering pipeline (Rendering::GLESceneGraph) so that developers need to do all GLEScene calls manually.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT GLESNativeApplication : public NativeApplication
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit GLESNativeApplication(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		~GLESNativeApplication() override;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param glesNativeApplication Application object which would have been copied
		 */
		GLESNativeApplication(const GLESNativeApplication& glesNativeApplication) = delete;

		/**
		 * Main loop loop of the application.
		 * @see NativeApplication::applicationLoop().
		 */
		void applicationLoop() override;

		/**
		 * Event function which will be called before the EGL context is created.
		 * This function allows to customize the properties of the EGL context.
		 * @param configAttributePairs The resulting pairs of configuration attributes which will be used to configure the EGL context
		 * @return True, if valid configuration pairs could be provided
		 */
		virtual bool onPreConfigureEGLContext(Platform::GLES::EGLContext::ConfigAttributePairs& configAttributePairs);

		/**
		 * Event function which will be called before the OpenGL ES framebuffer is created.
		 * this function allows to customize the properties of the OpenGL ES framebuffer.
		 * @param colorFormat The resulting OpenGL ES color format of the framebuffer
		 * @param multisamples The resulting number of multipsamples of the framebuffer, with range [0, infinity)
		 * @return True, if valid parameter could be provided
		 */
		virtual bool onPreConfigureGLESFramebuffer(GLenum& colorFormat, unsigned int& multisamples);

		/**
		 * Event function which is called after the framebuffer(s) have been initialized successfully.
		 */
		virtual void onFramebufferInitialized();

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 * @see NativeApplication::onIdle().
		 */
		void onIdle() override;

		/**
		 * Renders the current image into one framebuffer.
		 * @param eyeIndex The index of the eye (framebuffer) for which the current image will be rendered
		 * @param view_T_world The current transformation between world and the current view (camera/eye), also known as viewing matrix, must be valid
		 * @param projection The projection matrix of the view (eye), must be valid
		 * @param predictedDisplayTime The predicted timestamp when the image will be display
		 */
		virtual void renderFramebuffer(const size_t eyeIndex, const HomogenousMatrix4& view_T_world, const SquareMatrix4& projection, const Timestamp& predictedDisplayTime);

		/**
		 * Returns the TrackedRemoteDevice object allowing access to event of remotes.
		 * @return The desired object
		 */
		inline const TrackedRemoteDevice& trackedRemoteDevice() const;

		/**
		 * Returns whether the VrApi VR mode is currently entered.
		 * @see NativeApplication::isVRModeEntered().
		 */
		bool isVRModeEntered() const override;

		/**
		 * Events function called when the VR mode has been entered.
		 */
		virtual void onVrModeEntered();

		/**
		 * Events function called when the VR mode has been left.
		 */
		virtual void onVrModeLeft();

		/**
		 * Event functions for pressed buttons (e.g., from a tracked remote device).
		 * @param buttons The buttons currently pressed either by the left or right remote device/controller
		 * @param buttonsLeft The buttons currently pressed by the left remote device/controller
		 * @param buttonsRight The buttons currently pressed by the right remote device/controller
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp);

		/**
		 * Event functions for released buttons (e.g., from a tracked remote device).
		 * @param buttons The buttons currently pressed either by the left or right remote device/controller
		 * @param buttonsLeft The buttons currently pressed by the left remote device/controller
		 * @param buttonsRight The buttons currently pressed by the right remote device/controller
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp);

		/**
		 * Registers the system fonts.
		 */
		void registerSystemFonts();

		/**
		 * Disabled copy operator.
		 * @param glesNativeApplication Application object which would have been copied
		 * @return Reference to this object
		 */
		GLESNativeApplication& operator=(const GLESNativeApplication& glesNativeApplication) = delete;

	protected:

		/// The ovrMobile object of this applicaiton.
		ovrMobile* ovrMobile_;

		/// The index of the current frame.
		unsigned int frameIndex_;

		/// The EGL Context.
		Platform::GLES::EGLContext eglContext_;

		/// The OpenGLES framebuffers (one for each eye).
		GLESFramebuffers glesFramebuffers_;

	private:

		/// The tracked remote device object providing events of remotes.
		TrackedRemoteDevice trackedRemoteDevice_;
};

inline const TrackedRemoteDevice& GLESNativeApplication::trackedRemoteDevice() const
{
	return trackedRemoteDevice_;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_GLES_NATIVE_APPLICATION_H
