/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_ANDROID_GL_MAIN_VIEW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * @ingroup applicationdemotrackingslamslamtracker
 * @defgroup applicationdemotrackingslamslamtrackerandroid SLAM Tracker (Android)
 * @{
 * The demo application shows the capabilities of Ocean's SLAM tracker.<br>
 * This application here is intended for Android platforms only.
 * @}
 */

/**
 * This class implements the main view of the SLAM tracker demo application for android platforms.
 * @ingroup applicationdemotrackingslamslamtrackerandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
	public:

		/**
		 * Initializes the SLAM tracker.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 * @param deviceModel The device model on which the app is running
		 */
		void initializeSLAMTracker(const std::string& inputMedium, const std::string& resolution, const std::string& deviceModel);

		/**
		 * Starts recording.
		 * @return True, if succeeded
		 */
		bool startRecording();

		/**
		 * Stops recording.
		 * @return True, if succeeded
		 */
		bool stopRecording();

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
		~GLMainView() override;

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;

		/// The pixel image that will forward the image result from the SLAM tracker to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The actual implementation of the SLAM tracker.
		std::unique_ptr<SLAMTrackerWrapper> slamTracker_;

		/// Position of the most recent user interaction.
		Vector2 recentTouchPosition_ = Vector2(Numeric::minValue(), Numeric::minValue());
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the SLAM tracker.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_initializeSLAMTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution);

/**
 * Java native interface function to start recording.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_startRecording(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to stop recording.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_stopRecording(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_ANDROID_GL_MAIN_VIEW_H
