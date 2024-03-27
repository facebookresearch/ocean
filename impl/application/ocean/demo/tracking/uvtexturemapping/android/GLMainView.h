// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_ANDROID_GL_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/tracking/uvtexturemapping/UVTextureMappingWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * This class implements the main view of the UV texture mapping demo application for android platforms.
 * @ingroup applicationdemotrackinguvtexturemappingandroid
 */
class GLMainView :
	public Ocean::Platform::Android::Application::GLFrameView,
	protected Ocean::Thread
{
	public:

		/**
		 * Initializes the UV texture mapping.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param pattern The filename of the pattern to be used for tracking
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 */
		void initializeUVTextureMapping(const std::string& inputMedium, const std::string& pattern, const std::string& resolution);

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

		/// The pixel image that will forward the image result from the UV texture mapping to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The actual implementation of the UV texture mapping.
		UVTextureMappingWrapper uvTextureMappingWrapper_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the UV texture mapping.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param pattern The filename of the pattern to be used for tracking
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_facebook_ocean_app_demo_tracking_uvtexturemapping_android_UVTextureMappingActivity_initializeUVTextureMapping(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring pattern, jstring resolution);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_ANDROID_GL_MAIN_VIEW_H