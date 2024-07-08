/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/Detector2DWrapper.h"

#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * @ingroup applicationdemocvdetectorqrcodesdetector2d
 * @defgroup applicationdemocvdetectorqrcodesdetector2dandroid QR code detector demo on Android
 * @{
 * The demo application demonstrates the 2D detector for QR codes<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * This class implements the main view of the QR code detector demo application for android platforms.
 * @ingroup applicationdemocvdetectorqrcodesdetector2dandroid
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
		 * Initializes the platform-independent part of the QR code detector demo.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 */
		void initializeQRCode(const std::string& inputMedium, const std::string& resolution);

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

		/// The pixel image that will forward the image result from the QR code detector to the renderer.
		Ocean::Media::PixelImageRef pixelImage_;

		/// The platform-independent implementation of this demo.
		Detector2DWrapper detector2DWrapper_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to initialize the detector for QR codes.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup applicationdemocvdetectorqrcodesdetector2dandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_detector_qrcodes_detector2d_android_Detector2DActivity_initializeQRCode(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution);
