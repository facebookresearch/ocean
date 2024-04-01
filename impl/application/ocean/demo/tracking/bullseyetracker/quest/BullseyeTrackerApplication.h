// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/detector/MessengerCodeDetector.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/platform/meta/quest/sensors/FrameProvider.h"

#include "ocean/rendering/Sphere.h"
#include "ocean/rendering/Utilities.h"

using namespace Ocean;
using namespace Ocean::CV::Detector;
using namespace Ocean::Platform::Meta::Quest::OpenXR::Application;
using namespace Ocean::Rendering;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemotracking
 */
class BullseyeTrackerApplication :
	public VRNativeApplicationAdvanced,
	protected Thread
{
	protected:

		/**
		 * Definition of a vector holding camera frame types (i.e. the stream type) and the sub-selection cameras from the stream.
		 */
		typedef std::vector<std::pair<OSSDK::Sensors::v3::FrameType, Platform::Meta::Quest::Sensors::FrameProvider::CameraType>> CameraFrameTypes;

		/**
		 * Definition of individual text visualization ids.
		 */
		enum TextVisualizations : unsigned int
		{
			/// The version of the app
			TV_MANIFEST_VERSION_CODE = 0u,
			/// The average time spent on detection of bullseyes
			TV_DETECTION_TIME,
			/// The name of the camera that is currently used
			TV_CAMERA_NAME,
			/// Popup messages that are shown to the user for a short time.
			TV_TIMED_POPUP_MESSAGES,
			/// time since last detection
			TV_DETECTION_AGE,
			/// Indicates the last pre-assigned ID; this element must be the last one in this list; dynamically created text visualizations should use indices starting with this value
			TV_INDICES_END
		};

		/// Definition of a queue of popup messages: actual message, time after which they should disappear again
		typedef std::vector<std::pair<std::string, Timestamp>> PopupMessageQueue;

		/**
		 * Helper class for bulleye detection.
		 */
		class BullseyeDetector : public CV::Detector::MessengerCodeDetector
		{
			public:

				/**
				 * Detects a bullseye in a grayscale image.
				 * @param frame The frame in which the square will be detected, pixel pixel format FORMAT_Y8, must be valid
				 * @param bullseye The resulting bullseye within the given image.
				 * @return True, if succeeded
				 */
				static bool detectBullseye(const Frame& frame, Bullseye& bullseye);

				/**
				 * Determines the 3D location of a bullseye visible in two stereo images.
				 * @param sharedAnyCameraA The camera profile of the first stereo camera, must be valid
				 * @param sharedAnyCameraB The camera profile of the second stereo camera, must be valid
				 * @param world_T_device The transformation between device and world, must be valid
				 * @param device_T_cameraA The transformation between the first camera and the device, must be valid
				 * @param device_T_cameraB The transformation between the second camera and the device, must be valid
				 * @param bullseyeA The known location of the bullseye in the first stereo image, must be valid
				 * @param bullseyeB The known location of the bullseye in the second stereo image, must be valid
				 * @param worldObjectPoint The resulting 3D location of the bullseye
				 * @param projectionErrorA Optional resulting projection error in the first camera, in pixel
				 * @param projectionErrorB Optional resulting projection error in the second camera, in pixel
				 * @return True, if succeeded
				 */
				static bool determineBullseyeObjectPoint(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Vector2& bullseyeA, const Vector2& bullseyeB, Vector3& worldObjectPoint, Scalar* projectionErrorA = nullptr, Scalar* projectionErrorB = nullptr);
		};

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit BullseyeTrackerApplication(struct android_app* androidApp);

	protected:

		/**
		 * Returns the application's base space.
		 * @see VRNativeApplication::baseSpace().
		 */
		XrSpace baseSpace() const override;

		/**
		 * Event function call when an Android (or Oculus) permission is granted.
		 * @see NativeApplication::onAndroidPermissionGranted().
		 */
		void onAndroidPermissionGranted(const std::string& permission) override;

		/**
		 * Event functions for pressed buttons (e.g., from a tracked remote device).
		 * @see VRNativeApplication::onButtonPressed().
		 */
		void onButtonPressed(const Platform::Meta::Quest::OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp) override;

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Thread function for the keyboard tracker.
		 */
		void threadRun() override;

		/**
		 * Initializes the cameras through the frame provider depending on the current device type
		 * @return True if a camera stream has been started, otherwise false
		 */
		bool initializeCameras();

		/**
		 * Switches the frame provider to the available camera stream
		 * @return True if the camera streams have been switched successfully, otherwise false
		 */
		bool switchToNextCameraType();

		/**
		 * Displays a message to the user for a short time
		 * Duplicate messages will only shown once but the display time will be extended, if applicable.
		 * @param message The message that will be displayed, must be valid
		 * @param durationInSeconds The number seconds that the message will be displayed, range: (0, infinity)
		 */
		void displayPopupMessage(const std::string& message, const double durationInSeconds = 1.5);

		/**
		 * Returns all popup messages that should be displayed now
		 * @return The list of messages
		 */
		std::vector<std::string> getPopupMessages();

		/**
		 * Converts a frame to a grayscale frame with pixel format FORMAT_Y8
		 * @param frame The frame to convert, must be valid
		 * @param yFrame The resulting grayscale frame
		 * @return True, if succeeded
		 */
		static bool convertFrameToY8(const Frame& frame, Frame& yFrame);

	protected:

		/// The frame provider for the cameras.
		Platform::Meta::Quest::Sensors::FrameProvider frameProvider_;

		/// The lock for changing the parameters of this app.
		Lock parameterLock_;

		/// All available frame types.
		CameraFrameTypes cameraFrameTypes_;

		/// The index of the current camera frame type.
		size_t cameraFrameTypeIndex_ = size_t(-1);

		/// The scene object of the renderer;
		Rendering::SceneRef scene_;

		/// The marker transformation that will be used as an overlay for the bullseye displayed in Passthrough.
		Rendering::TransformRef markerTransform_;

		/// Current marker size
		Scalar markerSize_;

		/// for settting marker color during operation
		MaterialRef markerMaterial_;

		/// current marker color
		RGBAColor markerColor_;

		/// If true then stop detection and keep rendering marker at current World position
		bool lockedPosition_;

		/// timestamp of last detection
		Timestamp displayDetectionTime_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// Indicates if new results are available for displaying. Protected by resultLock_.
		bool haveResults_ = false;

		/// The world location of the bullseye detected by `MessengerCodeDetector`. Protected by resultLock_.
		Vector3 bullseyeCenter_;

		/// Time of bullseye detection. Protected by resultLock_.
		Timestamp detectionTime_;

		/// The 6DOF pose if the device origin in world coordinates
		HomogenousMatrix4 world_T_device_;

		/// The queue of all popup messages.
		PopupMessageQueue popupMessageQueue_;
};
