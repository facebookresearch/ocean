/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_ANDROID_GL_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/tracking/featuretracker/FeatureTrackerWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * This class implements the main view of the feature tracker demo application for android platforms.
 * @ingroup applicationdemotrackingfeaturetrackerandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
	public:

		/**
		 * Initializes the feature tracker.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param pattern The filename of the pattern to be used for tracking
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 */
		void initializeFeatureTracker(const std::string& inputMedium, const std::string& pattern, const std::string& resolution);

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

		/// The pixel image that will forward the image result from the feature tracker to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The actual implementation of the feature tracker.
		FeatureTrackerWrapper featureTrackerWrapper_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the feature tracker.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param pattern The filename of the pattern to be used for tracking
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_tracking_featuretracker_android_FeatureTrackerActivity_initializeFeatureTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring pattern, jstring resolution);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_ANDROID_GL_MAIN_VIEW_H
