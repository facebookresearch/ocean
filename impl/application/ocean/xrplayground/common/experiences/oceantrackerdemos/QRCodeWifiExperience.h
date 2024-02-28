// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_WIFI_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_WIFI_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"

#include "ocean/media/FrameMedium.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
#endif

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience demonstrates how to connect to a Wi-Fi network using QR codes.
 * @ingroup xrplayground
 */
class QRCodeWifiExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Identifier for the stages of the workflow of this experience
		 */
		enum StageType : uint32_t
		{
			/// The initial stage when the experience is started
			ST_START = 0u,
			/// The stage in which a QR code is searched
			ST_QRCODE_DETECTION,
			/// The stage in which the information from a detected QR code is used to initiate a Wi-Fi connection
			ST_INITIATE_WIFI_CONNECTION,
			/// The stage for waiting on the Wi-Fi change being reflected by the operating system
			ST_WAIT_FOR_CONNECTION_UPDATE,
			/// The stage in which establishing the Wi-Fi connection complete successfully
			ST_COMPLETE_SUCCESS,
			/// The stage in which establishing the Wi-Fi connection failed
			ST_COMPLETE_FAILURE,
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~QRCodeWifiExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Translates a stage type identifier to a human-readable string.
		 * @param stageType The stage type identifier, must be valid
		 * @return The translated string
		 */
		static std::string translateStageType(const StageType stageType);

	protected:

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		/// The table menu.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;
#endif

		/// True, to reset the menu.
		std::atomic_bool resetMenu_ = true;

		/// The transformation between device and world when the menu has been shown for the first time.
		HomogenousMatrix4 world_T_deviceMenu_ = HomogenousMatrix4(false);

		/// The current stage of the workflow.
		std::atomic<StageType> currentStage_ = ST_START;

		/// The name of the currently connected Wi-Fi network.
		std::string currentSsid_ = "";

		/// The time when the currently connected Wi-Fi network was last checked.
		Timestamp currentSsidTimestamp_ = Timestamp(false);

		/// The rendering text object to display usage help for the user.
		Rendering::TransformRef helpTextTransform_;
		/// The rendered help text.
		Rendering::TextRef helpText_;

		/// The rendering text object to display the name of the currently connected Wi-Fi network.
		Rendering::TransformRef currentSsidTextTransform_;
		/// The rendered help text.
		Rendering::TextRef currentSsidText_;

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		/// The transformation for the detection box.
		Rendering::TransformRef detectionBoxTransform_;
		/// The detection box that the card has to be placed in.
		Rendering::BoxRef detectionBox_;
		/// The material of the detection box.
		Rendering::MaterialRef detectionBoxMaterial_;

		/// The dimensions of the detection box for this device (in meters).
		Vector3 detectionBoxSize_;
		/// The location of the detection box for this device.
		Vector3 detectionBoxTranslation_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// The name of the Wi-Fi network from the detected QR code
		std::string detectedSsid_;

		/// The password of the Wi-Fi network from the detected QR code
		std::string detectedPassword_;

		/// The detected QR code
		CV::Detector::QRCodes::QRCode wifiCode_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_WIFI_EXPERIENCE_H
