// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_NATIVE_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_NATIVE_MAIN_VIEW_H

#include "application/ocean/xrplayground/android/XRPlaygroundAndroid.h"

#include "application/ocean/xrplayground/common/XRPlaygroundCommon.h"

#include "ocean/platform/android/application/GLView.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Android
{

/**
 * This class implements the main view of the XRPlayground viewer for android platforms.
 * @ingroup xrplaygroundandroid
 */
class XRPlaygroundNativeMainView : public Platform::Android::Application::GLView
{
	public:

		/**
		 * Initializes the view.
		 * @see GLView::initialize().
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @see GLView::release().
		 */
		bool release() override;

		/**
		 * View resize event function.
		 * @see GLView::release().
		 */
		bool resize(const int width, const int height) override;

		/**
		 * Renders the next frame.
		 * @see GLView::render().
		 */
		bool render() override;

		/**
		 * Loads or adds a new content.
		 * @param filename Filename of the content to be added
		 * @param replace State determining whether already existing content will be replaced or not
		 * @return True, if succeeded
		 */
		bool loadContent(const std::string& filename, const bool replace);

		/**
		 * Unload all permanent scenes/content.
		 */
		bool unloadContent();

		/**
		 * Stars the VRS recording.
		 * @return True, if succeeded
		 */
		bool startVRSRecording();

		/**
		 * Stops the VRS recording.
		 * @return True, if succeeded
		 */
		bool stopVRSRecording();

		/**
		 * Creates an instance of this object.
		 * @return The instance
		 */
		static inline Platform::Android::Application::GLView* createInstance();

	private:

		/**
		 * Creates a new main view object.
		 */
		XRPlaygroundNativeMainView();

		/**
		 * Destructs a main view object.
		 */
		~XRPlaygroundNativeMainView() override;

		/**
		 * The event function for granted permissions.
		 * @see GLView::onPermissionGranted().
		 */
		void onPermissionGranted(const std::string& permission) override;

		/**
		 * Touch down event function.
		 * @see GLView::touchDown().
		 */
		void onTouchDown(const float x, const float y) override;

		/**
		 * Touch move event function.
		 * @see GLView::touchMove().
		 */
		void onTouchMove(const float x, const float y) override;

		/**
		 * Touch move event function.
		 * @see GLView::touchUp().
		 */
		void onTouchUp(const float x, const float y) override;

		/**
		 * Setups the camera for the background.
		 * @return True, if succeeded
		 */
		bool setupCamera();

	private:

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;

		/// The platform independent XRPlayground code.
		XRPlaygroundCommon xrPlaygroundCommon_;

		/// True, if the camera permission is already granted.
		bool cameraPermissionGranted_;
};

inline Platform::Android::Application::GLView* XRPlaygroundNativeMainView::createInstance()
{
	return new XRPlaygroundNativeMainView();
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_XR_PLAYGROUND_NATIVE_MAIN_VIEW_H
