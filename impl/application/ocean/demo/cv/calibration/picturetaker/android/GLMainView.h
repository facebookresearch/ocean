/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_ANDROID_GL_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/demo/cv/calibration/ApplicationDemoCVCalibration.h"

#include "ocean/io/Directory.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/platform/android/application/GLFrameView.h"

/**
 * @ingroup applicationdemocvcalibrationpicturetaker
 * @defgroup applicationdemocvcalibrationpicturetakerandroid PictureTaker (Android)
 * @{
 * The demo application allows to take pictures from a live video stream and to store the pictures in a file.<br>
 * This application here is intended for Android platforms only.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the main view of the picture taker demo application for android platforms.
 * @ingroup applicationdemocvcalibrationpicturetakerandroid
 */
class GLMainView : public Ocean::Platform::Android::Application::GLFrameView
{
	public:

		/**
		 * Selects the camera which will be used for taking pictures.
		 * @param cameraName The name of the camera to be used
		 * @return True, if succeeded
		 */
		bool selectCamera(const std::string& cameraName);

		/**
		 * Starts the selected camera with a preferred image resolution.
		 * @param resolution The preferred image resolution, e.g., "640x480", "1280x720", "1920x1080"
		 * @param directory The directory in which the pictures will be stored, must be valid
		 * @return True, if succeeded
		 */
		bool startCamera(const std::string& resolution, const std::string& directory);

		/**
		 * Sets or changes the focus of the camera.
		 * @param focus The focus to be set, with range [0, 1]
		 * @return True, if succeeded
		 */
		bool setFocus(const float focus);

		/**
		 * Sets or changes the video stabilization mode.
		 * @param enabled True to enable stabilization, false to disable
		 * @return True, if succeeded
		 */
		bool setVideoStabilization(const bool enabled);

		/**
		 * Returns the current video stabilization status.
		 * @return True if stabilization is enabled
		 */
		bool videoStabilization() const;

		/**
		 * Takes a picture.
		 * @return True, if succeeded
		 */
		bool takePicture();

		/**
		 * Returns the available image resolution of the selected camera.
		 * The camera must have been selected before.
		 * @return The available image resolutions as strings, e.g., {"640x480", "1280x720", "1920x1080"}
		 */
		std::vector<std::string> availableResolutions();

		/**
		 * Creates an instance of this object.
		 * @return The instance
		 */
		static inline Ocean::Platform::Android::Application::GLView* createInstance();

		/**
		 * Returns the names of all available cameras in the device.
		 * @return The camera names
		 */
		static std::vector<std::string> availableCameras();

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
		 * Renders the next frame.
		 * @see GLFrameView::render().
		 */
		bool render() override;

	protected:

		/// The direcotry in which the pictures will be stored.
		IO::Directory directory_;

		/// The live video which will be used to take the pictures.
		Media::LiveVideoRef liveVideo_;

		/// The counter for the pictures.
		unsigned int pictureCounter_ = 0u;

		/// The selected camera name.
		std::string selectedCameraName_;

		/// The selected resolution.
		std::string selectedResolution_;

		/// The current focus value.
		float currentFocus_ = 0.85f;

		/// The current video stabilization state, 0 if disabled, 1 if enabled, -1 if unknown.
		int videoStabilization_ = -1;

		/// True, if the settings file has been written.
		bool settingsFileWritten_ = false;

	private:

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;
};

inline Ocean::Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

/**
 * Java native interface function to start the selected camera.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_startCamera(JNIEnv* env, jobject javaThis, jstring resolution);

/**
 * Java native interface function to take a picture.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_takePicture(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to get available cameras.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return Array of available camera names
 */
extern "C" jobjectArray Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_availableCameras(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to select a camera.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param cameraName The name of the camera to select
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_selectCamera(JNIEnv* env, jobject javaThis, jstring cameraName);

/**
 * Java native interface function to get available resolutions for the selected camera.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return Array of available resolutions
 */
extern "C" jobjectArray Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_availableResolutions(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to set the focus of the camera.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param focus The focus value between [0, 1]
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_setFocus(JNIEnv* env, jobject javaThis, jfloat focus);

/**
 * Java native interface function to set video stabilization.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param enabled True to enable stabilization, false to disable
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_setVideoStabilization(JNIEnv* env, jobject javaThis, jboolean enabled);

/**
 * Java native interface function to get current video stabilization status.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True if stabilization is enabled
 */
extern "C" jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_videoStabilization(JNIEnv* env, jobject javaThis);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_ANDROID_GL_MAIN_VIEW_H
