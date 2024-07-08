/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_META_APPLICATION_DEMO_MEDIA_EXTERNALCAMERA_QUEST_EXTERNAL_CAMERA_APPLICATION_H
#define OCEAN_META_APPLICATION_DEMO_MEDIA_EXTERNALCAMERA_QUEST_EXTERNAL_CAMERA_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"
#include "ocean/platform/meta/quest/openxr/application/VRTableMenu.h"

#include "ocean/math/RateCalculator.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

using namespace Ocean;
using namespace Ocean::Platform::Meta::Quest;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class ExternalCameraApplication : public OpenXR::Application::VRNativeApplicationAdvanced
{
	protected:

		/**
		 * Definition of individual application states.
		 */
		enum ApplicationState
		{
			/// The application is idling.
			AS_IDLE,
			/// The application need to enumerate external cameras.
			AS_ENUMERATE_CAMERAS,
			/// The user needs to select a camera.
			AS_SELECT_CAMERA,
			/// The application is waiting for supported stream configurations.
			AS_WAITING_FOR_SUPPORTED_STREAMS,
			// The user needs to select a stream.
			AS_SELECT_STREAM
		};

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit ExternalCameraApplication(struct android_app* androidApp);

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
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * The event function which is called when all resources should finally be released.
		 * @see NativeApplication::onReleaseResources().
		 */
		void onReleaseResources() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Event functions for released buttons (e.g., from a tracked controller).
		 * @see VRNativeApplication::onButtonReleased().
		 */
		void onButtonReleased(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp) override;

		/**
		 * Enumerates all external cameras and dispalys them in a table menu.
		 */
		void enumerateCameras();

		/**
		 * Determines the supported stream configurations of the selected camera and displays them in a table menu.
		 * @return True, if at least one stream configuration could be found
		 */
		bool enumerateStreamConfigurations();

		/**
		 * Returns a string describing a live video stream type.
		 * @param streamType The live video stream type to translate, must be valid
		 * @param pixelFormat The pixel format associated with the stream type, can be invalid
		 * @param codecType The codec type associated with the stream type, can be invalid
		 * @return The translated string, 'Invalid' if the given stream type is invalid or unknown
		 */
		static std::string translateStreamType(const Media::LiveVideo::StreamType streamType, const FrameType::PixelFormat pixelFormat, const Media::LiveVideo::CodecType codecType);

	protected:

		/// The rendering Transform node holding the rendering content.
		Rendering::TransformRef renderingTransform_;

		/// The rendering Transform node holding the 3D box with the live texture of the external camera.
		Rendering::TransformRef renderingTransformCamera_;

		/// True, to anchor the camera transform in the world; False, to anchor the camera transform w.r.t. the headset.
		bool anchorCameraInWorld_ = true;

		/// The rendering Text node holding the text displaying information about the selected camera.
		Rendering::TextRef renderingText_;

		/// The external camera.
		Media::LiveVideoRef liveVideo_;

		/// The table menu allowing to select the external camera.
		OpenXR::Application::VRTableMenu vrTableMenu_;

		/// The application's state.
		ApplicationState applicationState_ = AS_IDLE;

		/// The individual stream configurations the external camera supports.
		Media::LiveVideo::StreamConfigurations streamConfigurations_;

		/// The rate for the video framerate.
		RateCalculator rateCalculator_;

		/// The timestamp of the last frame.
		Timestamp lastFrameTimestamp_;
};

#endif // OCEAN_META_APPLICATION_DEMO_MEDIA_EXTERNALCAMERA_QUEST_EXTERNAL_CAMERA_APPLICATION_H
