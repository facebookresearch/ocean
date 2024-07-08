/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_ANDROID_GL_MAIN_VIEW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/cv/detector/messengercode/MessengerCodeWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * This class implements the main view of the Messenger code demo application for android platforms.
 * @ingroup applicationdemocvdetectormessengercodeandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
/// \cond DOXYGEN_DO_NOT_DOCUMENT
	friend class Ocean::Singleton<GLMainView>;
/// \endcond

	public:
		/**
		 * Initializes the detector for the Messenger code.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 * @param pixelFormat The pixel format to use internally
		 */
		void initializeMessengerCode(const std::string& inputMedium, const std::string& resolution, const std::string& pixelFormat);

		/**
		 * Creates an instance of this object.
		 * @return The instance
		 */
		static inline Ocean::Platform::Android::Application::GLView* createInstance();

	private:

		/**
		 * Creates a new main view object.
		 */
		GLMainView();

		/**
		 * Destructs a main view object.
		 */
		virtual ~GLMainView();

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;

		/// The pixel image that will forward the image result from the Messenger code detector to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The platform-independent implementation of this demo.
		MessengerCodeWrapper messengerCodeWrapper_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the detector for Messenger code.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @param pixelFormat The pixel format to use internally
 * @return True, if succeeded
 * @ingroup applicationdemocvdetectormessengercodeandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_detector_messengercode_android_MessengerCodeActivity_initializeMessengerCode(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution, jstring pixelFormat);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_ANDROID_GL_MAIN_VIEW_H
