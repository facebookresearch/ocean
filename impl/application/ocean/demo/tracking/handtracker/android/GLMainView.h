/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_HANDTRACKER_ANDROID_GL_MAIN_VIEW_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_HANDTRACKER_ANDROID_GL_MAIN_VIEW_H

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/ScopedJNIObject.h"

#include "ocean/platform/android/application/GLFrameView.h"

#include "ocean/rendering/Text.h"

/**
 * This class implements the main view of the hand tracker demo application for android platforms.
 * @ingroup applicationdemotrackinghandtrackerandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
	public:

		/**
		 * Initializes the hand tracker.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 */
		void initializeHandTracker(const std::string& inputMedium, const std::string& resolution);

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
		 * Initializes the view.
		 * @see GLFrameView::initialize().
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @see GLFrameView::release().
		 */
		bool release() override;

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// The input frame medium which is used to track the hand.
		Ocean::Media::FrameMediumRef inputFrameMedium_;

		/// The pixel image that will forward the image result from the hand tracker to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The rendering text object holding the text information.
		Ocean::Rendering::TextRef renderingText_;

		/// The JNI class of the activity.
		Ocean::Platform::Android::ScopedJClass jActivityClass_;

		/// The JNI method id of the tracking function.
		jmethodID jMethodId_ = nullptr;

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the hand tracker.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_tracking_handtracker_android_HandTrackerActivity_initializeHandTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution);

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_HANDTRACKER_ANDROID_GL_MAIN_VIEW_H
