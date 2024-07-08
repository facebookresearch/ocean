/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_ANDROID_A_LIVE_VIDEO_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/AMedium.h"
#include "ocean/media/android/NativeCameraLibrary.h"
#include "ocean/media/android/NativeMediaLibrary.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/LiveVideo.h"

#include <jni.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

/**
 * This class implements an live video class for Android.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT ALiveVideo final :
	virtual public AMedium,
	virtual public LiveVideo
{
	friend class ALibrary;
	friend class ALiveVideoManager;

	private:

		/**
		 * Definition of Android image formats that are missing from the NDK
		 * These values are used but they are missing from the NDK header, media/NdkImage.h.
		 * @see https://developer.android.com/reference/kotlin/android/graphics/ImageFormat
		 */
		enum AIMAGE_FORMATS_MISSING_FROM_NDK : int32_t
		{
			// Android YUV P010 format.
			AIMAGE_FORMAT_YCBCR_P010 = 54,
		};

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

		/**
		 * Returns the transformation between the camera and device.
		 * @see FrameMedium::device_T_camera().
		 */
		HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Returns the current exposure duration of this device.
		 * @see exposureDuration().
		 */
		double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr) const override;

		/**
		 * Returns the current ISO of this device.
		 * @see iso().
		 */
		float iso(float* minISO = nullptr, float* maxISO = nullptr) const override;

		/**
		 * Returns the current focus of this device.
		 * @see focus().
		 */
		float focus() const override;

		/**
		 * Sets the exposure duration of this device.
		 * @see setExposureDuration().
		 */
		bool setExposureDuration(const double duration) override;

		/**
		 * Sets the ISO of this device.
		 * @see setISO().
		 */
		bool setISO(const float iso) override;

		/**
		 * Sets the focus of this device.
		 * @see setFocus().
		 */
		bool setFocus(const float position) override;

		/**
		 * Explicitly feeds a new external frame this live video.
		 * This function is intended for situations in which this live video does not receive the frame anymore from the system (e.g., when ARCore is accessing the video stream).<br>
		 * Do not call this function in case the live video is still receiving pixel buffers from the Android media system.
		 * @param frame The frame to be fed, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 */
		inline void feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera);

		/**
		 * Forces a restart of the live video for situations in which the camera was used by an external resource.
		 * This function is intended for situations in which e.g., ARCore has used the borrowed the camera resource but does not use it anymore.
		 * Do not call this function in case the live video is still receiving pixel buffers from the Android media system.
		 * @return True, if succeeded
		 */
		bool forceRestart();

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit ALiveVideo(const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~ALiveVideo() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Initializes the camera and all corresponding resources.
		 * @return True, if succeeded
		 */
		bool initialize();

		/**
		 * Releases the camera and all corresponding resources.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Creates an Android camera for the URL of this medium.
		 * @param frameType The resulting frame type matching with the Android camera
		 * @return True, if succeeded
		 */
		bool createCamera(FrameType& frameType);

		/**
		 * Releases the Android camera.
		 * @return True, if succeeded
		 */
		bool releaseCamera();

		/**
		 * Creates a new image reader based on a given frame type, needs a valid camera.
		 * @param frameType The frame type of the image reader, must be valid
		 * @return True, if succeeded
		 */
		bool createImageReader(const FrameType& frameType);

		/**
		 * Releases the image reader.
		 */
		void releaseImageReader();

		/**
		 * Creates a new capture session, needs a valid camera and image reader.
		 * @return True, if succeeded
		 */
		bool createCaptureSession();

		/**
		 * Releases the capture session.
		 */
		void releaseCaptureSession();

		/**
		 * The callback function for camera images.
		 * @param imageReader The image reader of the event, must be valid
		 * @see onCameraImageCallbackStatic().
		 */
		void onCameraImageCallback(AImageReader* imageReader);

		/**
		 * The callback function for a disconnected camera.
		 * @param cameraDevice The camera device of the event, must be valid
		 * @see onCameraDisconnectedStatic().
		 */
		void onCameraDisconnected(ACameraDevice* cameraDevice);

		/**
		 * The callback function for errors.
		 * @param cameraDevice The camera device of the event, must be valid
		 * @param error The error value of the event
		 * @see onCameraErrorStatic().
		 */
		void onCameraError(ACameraDevice* cameraDevice, int error);

		/**
		 * The callback function for an active session.
		 * @param session The session, must be valid
		 */
		void onSessionActive(ACameraCaptureSession* session);

		/**
		 * The callback function for a closed session.
		 * @param session The session, must be valid
		 */
		void onSessionClosed(ACameraCaptureSession* session);

		/**
		 * The callback function for a started capture.
		 * @param session The session, must be valid
		 * @param request The capture request, must be valid
		 * @param timestamp The capture results, must be valid
		 */
		void onCaptureStarted(ACameraCaptureSession* session, const ACaptureRequest* request, int64_t timestamp);

		/**
		 * The callback function for a completed capture.
		 * @param session The session, must be valid
		 * @param request The capture request, must be valid
		 * @param result The capture results, must be valid
		 */
		void onCaptureCompleted(ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result);

		/**
		 * Event function for new camera frames.
		 * @param frame The new camera frame, must be valid
		 * @param anyCamera The camera profile defining the projection, invalid if unknown
		 * @return True, if succeeded
		 */
		bool onNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera);

		/**
		 * Determines the camera id and associated frame type for a given URL.
		 * @param cameraManager The camera manager from which the camera id will be determined, must be valid
		 * @param url The URL for which the camera id will be determined
		 * @param preferredFrameType The preferred frame type (may contain a valid image dimension, or just a pixel format), can be invalid
		 * @param frameType The resulting frame type matching with the resulting id
		 * @return The resulting id of the camera
		 */
		static std::string cameraIdForMedium(ACameraManager* cameraManager, const std::string& url, const FrameType& preferredFrameType, FrameType& frameType);

		/**
		 * Determines the range of the exposure duration of the camera sensor.
		 * @param cameraManager The camera manager, must be valid
		 * @param cameraId The id of the camera, must be valid
		 * @param minExposureDuration The minimal exposure duration, in seconds
		 * @param maxExposureDuration The maximal exposure duration, in seconds
		 * @return True, if succeeded
		 */
		static bool cameraExposureDurationRange(ACameraManager* cameraManager, const std::string& cameraId, double& minExposureDuration, double& maxExposureDuration);

		/**
		 * Determines the range of the ISO of the camera sensor.
		 * @param cameraManager The camera manager, must be valid
		 * @param cameraId The id of the camera, must be valid
		 * @param minISO The minimal ISO
		 * @param maxISO The maximal ISO
		 * @return True, if succeeded
		 */
		static bool cameraISORange(ACameraManager* cameraManager, const std::string& cameraId, float& minISO, float& maxISO);

		/**
		 * Determines the range of the focus distance of the camera sensor.
		 * @param cameraManager The camera manager, must be valid
		 * @param cameraId The id of the camera, must be valid
		 * @param minFocusPosition The minimal focus distance
		 * @param maxFocusPosition The maximal focus distance
		 * @return True, if succeeded
		 */
		static bool cameraFocusRange(ACameraManager* cameraManager, const std::string& cameraId, float& minFocusPosition, float& maxFocusPosition);

		/**
		 * Determines the physical size of the camera sensor.
		 * @param cameraManager The camera manager, must be valid
		 * @param cameraId The id of the camera, must be valid
		 * @param cameraSensorPhysicalSizeX Resulting horizontal physical size of the sensor
		 * @return True, if succeeded
		 */
		static bool cameraSensorPysicalSize(ACameraManager* cameraManager, const std::string& cameraId, float& cameraSensorPhysicalSizeX);

		/**
		 * Determines the horizontal field of view from the camera's metadata based on `ACAMERA_LENS_FOCAL_LENGTH`.
		 * @param cameraSensorPhysicalSizeX The horizontal physical size of camera sensor, as provided by `ACAMERA_SENSOR_INFO_PHYSICAL_SIZE`.
		 * @param cameraMetadata The camera's metadata for a recent frame, must be valid
		 * @param fovX The resulting horizontal field of view of the camera, in radian
		 * @return True, if succeeded
		 */
		static bool horizontalFieldOfView(const float cameraSensorPhysicalSizeX, const ACameraMetadata* cameraMetadata, float& fovX);

		/**
		 * Extracts the next frame from an Android image object.
		 * @param image The Android image object from which the next frame will be extracted
		 * @param frame The resulting frame containing the extracted image
		 * @return True, if succeeded
		 */
		static bool frameFromImage(AImage* image, Frame& frame);

		/**
		 * Translates an Android pixel format to an Ocean pixel format.
		 * @param androidFormat The Android pixel format to translate
		 * @return The corresponding Ocean pixel format, FORMAT_UNDEFINED if no corresponding Ocean pixel format exists
		 */
		static FrameType::PixelFormat androidFormatToPixelFormat(const int32_t androidFormat);

		/**
		 * Translates Android formats to readable strings.
		 * @param androidFormat The Android pixel format to translate
		 * @return The corresponding readable string of the Android format
		 */
		static std::string androidFormatToString(const int32_t androidFormat);

		/**
		 * Translates an Ocean pixel format to an Android pixel format.
		 * @param pixelFormat The Ocean pixel format to translate
		 * @return The corresponding Android pixel format, 0 if no corresponding Android pixel format exists
		 */
		static int32_t pixelFormatToAndroidFormat(const FrameType::PixelFormat pixelFormat);

		/**
		 * The static callback function for camera images.
		 * @param context The event's context, must be valid
		 * @param imageReader The image reader of the event, must be valid
		 * @see onCameraImageCallback().
		 */
		static void onCameraImageCallbackStatic(void* context, AImageReader* imageReader);

		/**
		 * The static callback function for a disconnected camera.
		 * @param context The event's context, must be valid
		 * @param cameraDevice The camera device of the event, must be valid
		 * @see onCameraDisconnected().
		 */
		static void onCameraDisconnectedStatic(void* context, ACameraDevice* cameraDevice);

		/**
		 * The static callback function for errors.
		 * @param context The event's context, must be valid
		 * @param cameraDevice The camera device of the event, must be valid
		 * @param error The error value of the event
		 * @see onCameraError().
		 */
		static void onCameraErrorStatic(void* context, ACameraDevice* cameraDevice, int error);

		/**
		 * The static callback function for an active session.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 */
		static void onSessionActiveStatic(void* context, ACameraCaptureSession* session);

		/**
		 * The static callback function for a ready session.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 */
		static void onSessionReadyStatic(void* context, ACameraCaptureSession* session);

		/**
		 * The static callback function for a closed session.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 */
		static void onSessionClosedStatic(void* context, ACameraCaptureSession* session);

		/**
		 * The static callback function for a failed capture.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 * @param request The capture request, must be valid
		 * @param failure The failure, must be valid
		 */
		static void onCaptureFailedStatic(void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraCaptureFailure* failure);

		/**
		 * The static callback function for a completed capture sequence.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 * @param sequenceId The sequence id
		 * @param frameNumber The number of frames
		 */
		static void onCaptureSequenceCompletedStatic(void* context, ACameraCaptureSession* session, int sequenceId, int64_t frameNumber);

		/**
		 * The static callback function for an aborted sequence.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 * @param sequenceId The sequence id
		 */
		static void onCaptureSequenceAbortedStatic(void* context, ACameraCaptureSession* session, int sequenceId);

		/**
		 * The static callback function for a started capture.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 * @param request The capture request, must be valid
		 * @param timestamp The capture timestamp
		 */
		static void onCaptureStartedStatic(void *context, ACameraCaptureSession *session, const ACaptureRequest *request, int64_t timestamp);

		/**
		 * The static callback function for a completed capture.
		 * @param context The event's context, must be valid
		 * @param session The session, must be valid
		 * @param request The capture request, must be valid
		 * @param result The capture results, must be valid
		 */
		static void onCaptureCompletedStatic(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result);

		/**
		 * Translates a camera status to a readable strings.
		 * @param cameraStatus The camera status
		 * @return The readable camera status.
		 */
		static std::string translateCameraStatus(const camera_status_t cameraStatus);

	protected:

		/// Unique camera id defined by Android, may be valid even if 'cameraDevice_' is not yet valid.
		std::string cameraId_;

		/// The camera device.
		ACameraDevice* cameraDevice_ = nullptr;

		/// The image reader.
		AImageReader* imageReader_ = nullptr;

		/// The native Android window associated with the video.
		ANativeWindow* nativeWindow_ = nullptr;

		/// The session output.
		NativeCameraLibrary::ScopedACaptureSessionOutput sessionOutput_;

		/// The session output container.
		NativeCameraLibrary::ScopedACaptureSessionOutputContainer sessionOutputContainer_;

		/// The capture session.
		ACameraCaptureSession* captureSession_ = nullptr;

		/// The capture request.
		ACaptureRequest* captureRequest_ = nullptr;

		/// The output target.
		ACameraOutputTarget* outputTarget_ = nullptr;

		/// The horizontal size of the physical camera sensor, if known; -1 if unknown.
		float cameraSensorPhysicalSizeX_ = -1.0f;

		/// Recent frame type.
		FrameType imageRecentFrameType;

		/// The current exposure duration of this device.
		double exposureDuration_ = -1.0;

		/// Minimal duration to set, in seconds, with range (0, infinity), -1 if unknown.
		double exposureDurationMin_ = -1.0;

		/// Maximal duration to set, in seconds, with range [exposureDurationMin_, infinity), -1 if unknown
		double exposureDurationMax_ = -1.0;

		/// The current ISO, with range [isoMin_, isoMax_], 0 for auto IOS, -1 if unknown.
		float iso_ = -1.0f;

		/// Minimal ISO to set, with range (0, infinity), -1 if unknown.
		float isoMin_ = -1.0f;

		/// Maximal ISO to set, with range (isoMin_, infinity), -1 if unknown.
		float isoMax_ = -1.0f;

		/// The focus position to be set, with range [0, 1] with 0 shortest distance and 1 furthest distance, -1 for auto focus.
		float focusPosition_ = -1.0f;

		/// The minimal focus distance.
		float focusPositionMin_ = -1.0f;

		/// The maximal focus distance.
		float focusPositionMax_ = -1.0f;

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;
};

inline void ALiveVideo::feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera)
{
	onNewFrame(std::move(frame), std::move(anyCamera));
}

#else // #if __ANDROID_API__ >= 24

/**
 * Java native interface function for new camera framebuffer events.
 * @param env JNI environment object
 * @param javaThis JNI caller object
 * @param objectId Id of the camera object
 * @param imageBuffer New camera image buffer
 * @param width The width of the new camera image buffer in pixel
 * @param height The height of the new camera image buffer in pixel
 * @param format Format of the camera image buffer
 * @param fovX Horizontal field of view in radian
 * @param bufferSize The size of the given image buffer in bytes
 * @param timestamp Unix timestamp of the frame in seconds (time since 1970)
 * @return True, if succeeded
 * @ingroup mediaandroid
 */
extern "C" jboolean Java_com_meta_ocean_media_android_MediaAndroidJni_onNewCameraFrame(JNIEnv* env, jobject javaThis, jint objectId, jbyteArray imageBuffer, jint width, jint height, jint format, jfloat fovX, jint bufferSize, jdouble timestamp);

/**
 * This class implements an live video class for Android.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT ALiveVideo final :
	virtual public AMedium,
	virtual public LiveVideo
{
	friend class ALibrary;
	friend class ALiveVideoManager;

	public:

		/**
		 * This class implements a camera manager.
		 */
		class ALiveVideoManager : public Singleton<ALiveVideoManager>
		{
			friend class Singleton<ALiveVideoManager>;
			friend class ALiveVideo;

			private:

				/**
				 * Definition of a map mapping camera id to ocean camera objects.
				 */
				typedef std::map<int, ALiveVideo*> ObjectMap;

			public:

				/**
				 * Event function for a new camera frame.
				 * @param cameraId Id of the camera the new frame belongs to
				 * @param frame The new camera frame, must be valid
				 * @param anyCamera The camera profile defining the projection, can be invalid
				 * @return True, if succeeded
				 */
				bool onNewFrame(const int cameraId, Frame&& frame, SharedAnyCamera&& anyCamera);

			private:

				/**
				 * Creates a new camera manager.
				 */
				ALiveVideoManager();

				/**
				 * Destructs a camera manager.
				 */
				virtual ~ALiveVideoManager();

				/**
				 * Creates a new camera object.
				 * @param object Ocean video object to connect with the android camera object
				 * @param url The camera url to create the object for
				 * @param preferredWidth Preferred frame width in pixel
				 * @param preferredHeight Preferred frame height in pixel
				 * @param preferredFrequency Preferred frame frequency in Hz
				 * @return Unique camera id if succeeded, -1 otherwise
				 */
				int createCamera(ALiveVideo* object, const std::string& url, const unsigned int preferredWidth, const unsigned int preferredHeight, const float preferredFrequency);

				/**
				 * Releases a camera object.
				 * @param cameraId Unique camera id of the camera to be released
				 */
				void releaseCamera(const int cameraId);

				/**
				 * Starts a specified camera object.
				 * @param cameraId Id of the camera to be started
				 * @return True, if succeeded
				 */
				bool startCamera(const int cameraId);

				/**
				 * Pauses a specified camera object.
				 * @param cameraId Id of the camera to be paused
				 * @return True, if succeeded
				 */
				bool pauseCamera(const int cameraId);

				/**
				 * Stops a specified camera object.
				 * @param cameraId Id of the camera to be stopped
				 * @return True, if succeeded
				 */
				bool stopCamera(const int cameraId);

			private:

				/// The camera object map.
				ObjectMap managerObjectMap;

				/// Manager lock.
				Lock managerLock;
		};

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

		/**
		 * Returns the transformation between the camera and device.
		 * @see FrameMedium::device_T_camera().
		 */
		HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Explicitly feeds a new external frame this live video.
		 * This function is intended for situations in which this live video does not receive the frame anymore from the system (e.g., when ARCore is accessing the video stream).<br>
		 * Do not call this function in case the live video is still receiving pixel buffers from the Android media system.
		 * @param frame The frame to be fed, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 */
		inline void feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera);

		/**
		 * Forces a restart of the live video for situations in which the camera was used by an external resource.
		 * This function is intended for situations in which e.g., ARCore has used the borrowed the camera resource but does not use it anymore.
		 * Do not call this function in case the live video is still receiving pixel buffers from the Android media system.
		 * @return True, if succeeded
		 */
		bool forceRestart();

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit ALiveVideo(const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~ALiveVideo() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Event function for new camera frames.
		 * @param frame The new camera frame, must be valid
		 * @param anyCamera The camera profile defining the projection, invalid if unknown
		 * @return True, if succeeded
		 */
		bool onNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera);

	protected:

		/// Unique camera id received by the android system.
		int mediumCameraId = -1;

		/// Recent frame type.
		FrameType imageRecentFrameType;

		/// State determining whether the camera is currently started.
		bool mediumIsStarted = false;

		/// Start timestamp.
		Timestamp mediumStartTimestamp;

		/// Pause timestamp.
		Timestamp mediumPauseTimestamp;

		/// Stop timestamp.
		Timestamp mediumStopTimestamp;
};

inline void ALiveVideo::feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera)
{
	onNewFrame(std::move(frame), std::move(anyCamera));
}

#endif // __ANDROID_API__ >= 24

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_LIVE_VIDEO_H
