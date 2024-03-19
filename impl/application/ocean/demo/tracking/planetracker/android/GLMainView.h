// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_ANDROID_GL_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/tracking/planetracker/PlaneTrackerWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * This class implements the main view of the plane tracker demo application for android platforms.
 * @ingroup applicationdemotrackingplanetrackerandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
	public:

		/**
		 * Initializes the plane tracker.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 */
		void initializePlaneTracker(const std::string& inputMedium, const std::string& resolution);

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

		/**
		 * Touch down event function.
		 * @see GLFrameView::onTouchDown().
		 */
		void onTouchDown(const float x, const float y) override;

	private:

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;

		/// The pixel image that will forward the image result from the plane tracker to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The actual implementation of the plane tracker.
		PlaneTrackerWrapper planeTrackerWrapper_;

		/// Position of the most recent user interaction.
		Vector2 recentTouchPosition_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the plane tracker.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_facebook_ocean_app_demo_tracking_planetracker_android_PlaneTrackerActivity_initializePlaneTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_ANDROID_GL_MAIN_VIEW_H
