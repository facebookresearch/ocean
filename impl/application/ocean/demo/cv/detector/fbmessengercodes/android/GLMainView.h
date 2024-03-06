// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_ANDROID_GL_MAIN_VIEW_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/cv/detector/fbmessengercodes/FBMessengerCodesWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * This class implements the main view of the FB Messenger codes demo application for android platforms.
 * @ingroup applicationdemocvdetectorfbmessengercodesandroid
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
		 * Initializes the detector for the FB Messenger codes.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 * @param pixelFormat The pixel format to use internally
		 * @param badge The filename of image containing the marker image that identifies the app (this should be the badge of the Messenger app)
		 * @param bullseye The filename of image containing the marker that is used to detect and locate the actual Messenger code.
		 * @param classifier The configuration filename of the OpenCV cascade classifier
		 */
		void initializeFBMessengerCodes(const std::string& inputMedium, const std::string& resolution, const std::string& pixelFormat, const std::string& badge, const std::string bullseye, const std::string& classifier);

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
		const static bool viewInstanceRegistered;

		/// The pixel image that will forward the image result from the FB Messenger code detector to the renderer.
		Ocean::Media::PixelImageRef viewPixelImage;

		/// The platform-independent implementation of this demo.
		FBMessengerCodesWrapper viewFBMessengerCodesWrapper;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the detector for FB Messenger codes.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @param pixelFormat The pixel format to use internally
 * @param badge The filename of image containing the marker image that identifies the app (this should be the badge of the Messenger app)
 * @param bullseye The filename of image containing the marker that is used to detect and locate the actual Messenger code.
 * @param classifier The configuration filename of the OpenCV cascade classifier
 * @return True, if succeeded
 * @ingroup applicationdemocvdetectorfbmessengercodesandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_detector_fbmessengercodes_android_FBMessengerCodesActivity_initializeFBMessengerCodes(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution, jstring pixelFormat, jstring badge, jstring bullseye, jstring classifier);

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_ANDROID_GL_MAIN_VIEW_H
