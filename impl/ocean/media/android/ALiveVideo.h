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

#include "ocean/base/Timestamp.h"
#include "ocean/base/Value.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/LiveVideo.h"

#include <jni.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

#ifdef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

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
		 * This class holds the relevant information of a selectable live video device.
		 */
		class Device
		{
			friend class ALiveVideo;

			public:

				/// Definition of an unordered map mapping metadata tags to their values.
				using MetadataMap = std::unordered_map<uint32_t, Value>;

			public:

				/**
				 * Default constructor.
				 */
				Device() = default;

				/**
				 * Returns the device's id.
				 * @return The id of the device
				 */
				inline const std::string& id() const;

				/**
				 * Returns the device's name.
				 * @return The name of the device
				 */
				inline const std::string& name() const;

				/**
				 * Returns the device's direction.
				 * @return The direction of the device
				 */
				inline acamera_metadata_enum_android_lens_facing_t lensFacing() const;

				/**
				 * Returns the optional metadata map of the device.
				 * @return The optional metadata map of the device
				 */
				inline const MetadataMap& metadataMap() const;

				/**
				 * Returns whether this object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns the readable string for a lens facing type.
				 * @param lensFacing The lens facing type to return the readable string for
				 * @return The readable string for the given lens facing type
				 */
				static std::string readableLensFacing(const acamera_metadata_enum_android_lens_facing_t lensFacing);

			protected:

				/**
				 * Creates a new device object.
				 * @param id The id of the device, must be valid
				 * @param name The human redable name of the device, must be valid
				 * @param lensFacing The direction of the device
				 * @param metadataMap The optional metadata map of the device
				 */
				inline Device(const std::string& id, const std::string& name_, const acamera_metadata_enum_android_lens_facing_t lensFacing, MetadataMap&& metadataMap = MetadataMap());

			protected:

				/// The device's id.
				std::string id_;

				/// The device's human readable name.
				std::string name_;

				/// The device's direction.
				acamera_metadata_enum_android_lens_facing_t lensFacing_ = acamera_metadata_enum_acamera_lens_facing(-1);

				/// The optional metadata map of the device.
				MetadataMap metadataMap_;
		};

		using Devices = std::vector<Device>;

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
		 * Returns the supported stream types.
		 * @see LiveVideo::supportedStreamTypes().
		 */
		StreamTypes supportedStreamTypes() const override;

		/**
		 * Returns the supported stream configurations for a given stream type.
		 * @see LiveVideo::supportedStreamConfigurations().
		 */
		StreamConfigurations supportedStreamConfigurations(const StreamType streamType = ST_INVALID) const override;

		/**
		 * Returns the current exposure duration of this device.
		 * @see exposureDuration().
		 */
		double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr, ControlMode* exposureMode = nullptr) const override;

		/**
		 * Returns the current ISO of this device.
		 * @see iso().
		 */
		float iso(float* minISO = nullptr, float* maxISO = nullptr, ControlMode* isoMode = nullptr) const override;

		/**
		 * Returns the current focus of this device.
		 * @see LiveVideo::focus().
		 */
		float focus(ControlMode* focusMode = nullptr) const override;

		/**
		 * Sets the preferred stream type.
		 * @see LiveVideo::setPreferredStreamConfiguration().
		 */
		bool setPreferredStreamType(const StreamType streamType) override;

		/**
		 * Sets the preferred stream configuration.
		 * @see LiveVideo::setPreferredStreamConfiguration().
		 */
		bool setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration) override;

		/**
		 * Sets the exposure duration of this device.
		 * Note: 'allowShorterExposure' is ignored
		 * @see setExposureDuration().
		 */
		bool setExposureDuration(const double duration, const bool allowShorterExposure = false) override;

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
		 * @param exposureMode The exposure mode of the camera, CM_INVALID if unknown
		 * @param exposureDuration The exposure duration of the camera, in seconds, -1 if unknown
		 * @param isoMode The ISO mode of the camera, CM_INVALID if unknown
		 * @param iso The ISO of the camera, -1 if unknown
		 * @param focusMode The focus mode of the camera, CM_INVALID if unknown
		 * @param focusValue The focus value of the camera, in the same domain as ACAMERA_LENS_INFO_MINIMUM_FOCUS_DISTANCE, -1 if unknown
		 */
		void feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera, const ControlMode exposureMode, const double exposureDuration, const ControlMode isoMode, const float iso, const ControlMode focusMode, const float focusValue);

		/**
		 * Forces a restart of the live video for situations in which the camera was used by an external resource.
		 * This function is intended for situations in which e.g., ARCore has used the borrowed the camera resource but does not use it anymore.
		 * Do not call this function in case the live video is still receiving pixel buffers from the Android media system.
		 * @return True, if succeeded
		 */
		bool forceRestart();

		/**
		 * Returns the list of currently selectable devices.
		 * @return The list of currently selectable devices
		 */
		static Devices selectableDevices();

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 * @param id Optional unique id of the medium
		 */
		explicit ALiveVideo(const std::string& url, const std::string& id);

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
		 * Sets or update a repeating capture request.
		 */
		bool setRepeatingRequest();

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
		 * @param id Optional known id of the camera, if provided the camera id will be verified again
		 * @param preferredFrameType The preferred frame type (may contain a valid image dimension, or just a pixel format), can be invalid
		 * @param frameType The resulting frame type matching with the resulting id
		 * @param streamConfigurations Optional resulting stream configurations
		 * @param device_T_camera Optional resulting transformation between camera and device
		 * @return The resulting id of the camera
		 */
		static std::string cameraIdForMedium(ACameraManager* cameraManager, const std::string& url, const std::string& id, const FrameType& preferredFrameType, FrameType& frameType, StreamConfigurations* streamConfigurations = nullptr, HomogenousMatrixD4* device_T_camera = nullptr);

		/**
		 * Determines the camera id and associated frame type for a given URL or/and id.
		 * @param cameraIdList The camera id list from which the camera id will be determined, must be valid
		 * @param url The URL for which the camera id will be determined
		 * @param id Optional known id of the camera, if provided the camera id will be verified again
		 * @return The resulting id of the camera, empty if no camera could be found
		 */
		static std::string cameraIdForMedium(ACameraIdList* cameraIdList, const std::string& url, const std::string& id);

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
		 * Determines the supported focus modes of the camera sensor.
		 * @param cameraManager The camera manager, must be valid
		 * @param cameraId The id of the camera, must be valid
		 * @param focusModes The resulting available focus modes
		 * @param minFocusPosition The minimal focus distance, which is the reciprocal of the focus distance in meters
		 * @return True, if succeeded
		 */
		static bool cameraAvailableFocusModes(ACameraManager* cameraManager, const std::string& cameraId, ControlModes& focusModes, float& minFocusPosition);

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
		 * @param timestampNs The resulting timestamp of the frame, as provided by AImage_getTimestamp(), in nanoseconds
		 * @return True, if succeeded
		 */
		static bool frameFromImage(AImage* image, Frame& frame, int64_t& timestampNs);

		/**
		 * Returns the transformation between camera and device (device_T_camera).
		 * @param cameraMetadata The camera metadata from which the transformation will be determined, must be valid
		 * @return The camera transformation with default camera pointing towards the negative z-space with y-axis upwards; invalid if the transformation could not be determined
		 */
		static HomogenousMatrixD4 determineCameraTransformation(ACameraMetadata* cameraMetadata);

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
		 * Determines the available stream configurations for the camera metadata from a camera.
		 * @param cameraMetadata The camera metadata from which the stream configurations will be determined, must be valid
		 * @return The resulting stream configurations
		 */
		static StreamConfigurations determineAvailableStreamConfigurations(const ACameraMetadata* cameraMetadata);

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

		/// The transformation between camera and device, with default camera pointing towards the negative z-space with y-axis up.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

		/// The timestamp converter to convert timestamps from the camera to unix timestamps.
		Timestamp::TimestampConverter timestampConverter_ = Timestamp::TimestampConverter(Timestamp::TimestampConverter::TD_BOOTTIME);

		/// The current exposure mode of this device.
		ControlMode exposureMode_ = CM_INVALID;

		/// The current exposure duration of this device.
		double exposureDuration_ = -1.0;

		/// Minimal duration to set, in seconds, with range (0, infinity), -1 if unknown.
		double exposureDurationMin_ = -1.0;

		/// Maximal duration to set, in seconds, with range [exposureDurationMin_, infinity), -1 if unknown
		double exposureDurationMax_ = -1.0;

		/// The current ISO mode of this device.
		ControlMode isoMode_ = CM_INVALID;

		/// The current ISO, with range [isoMin_, isoMax_], 0 for auto IOS, -1 if unknown.
		float iso_ = -1.0f;

		/// Minimal ISO to set, with range (0, infinity), -1 if unknown.
		float isoMin_ = -1.0f;

		/// Maximal ISO to set, with range (isoMin_, infinity), -1 if unknown.
		float isoMax_ = -1.0f;

		/// The current focus mode of this device.
		ControlMode focusMode_ = CM_INVALID;

		/// The focus position to be set, with range [0, 1] with 0 shortest distance and 1 furthest distance, -1 if unknown.
		float focusPosition_ = -1.0f;

		/// The minimal focus distance (reciprocal of the focus distance in meters), -1 if unknown.
		float focusPositionMin_ = -1.0f;

		/// The stream configurations available for this camera.
		StreamConfigurations availableStreamConfigurations_;

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;

		/// True, if the capture session is currently closing (a release has been invoked but not yet finished).
		std::atomic<bool> sessionCurrentlyClosing_ = false;
};

inline ALiveVideo::Device::Device(const std::string& id, const std::string& name, const acamera_metadata_enum_android_lens_facing_t lensFacing, MetadataMap&& metadataMap) :
	id_(id),
	name_(name),
	lensFacing_(lensFacing),
	metadataMap_(std::move(metadataMap))
{
	ocean_assert(!id_.empty());
	ocean_assert(!name_.empty());
}

inline const std::string& ALiveVideo::Device::id() const
{
	return id_;
}

inline const std::string& ALiveVideo::Device::name() const
{
	return name_;
}

inline acamera_metadata_enum_android_lens_facing_t ALiveVideo::Device::lensFacing() const
{
	return lensFacing_;
}

inline const ALiveVideo::Device::MetadataMap& ALiveVideo::Device::metadataMap() const
{
	return metadataMap_;
}

inline bool ALiveVideo::Device::isValid() const
{
	return !id_.empty();
}

#endif // OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_LIVE_VIDEO_H
