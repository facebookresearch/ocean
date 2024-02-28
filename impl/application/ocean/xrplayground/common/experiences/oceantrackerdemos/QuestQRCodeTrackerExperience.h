// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_QRCODE_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_QRCODE_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Text.h"

#include "ocean/tracking/qrcodes/QRCodeTracker3D.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to access the cameras on Quest platforms.
 * @ingroup xrplayground
 */
class QuestQRCodeTrackerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Definition of a helper class to visualize QR codes in VR.
		 */
		class CodeVisualization
		{
			protected:

				/**
				 * Definition of an indicator for the support of specific metric value (e.g. as defined by key-performance indicators, KPI)
				 */
				enum ValueSupportType : size_t
				{
					/// Indicates that a value is outside the supported value range
					VST_UNSUPPORTED = 0,

					/// Indicates that a value is in the marginally supported value range
					VST_MARGINAL,

					/// Indicates that a value is inside the fully the supported value range
					VST_NOMINAL,
				};

			public:

				/**
				 * Visualizes a QR code in the world coordinate frame
				 * @param world_T_code The 6DOF pose of the code mapping from object space to world space, must be valid.
				 * @param codeSize The size of the code in the physical world, in meters, range: (0, infinity)
				 * @param payload The payload of the code as a string, must be valid
				 * @param codeVersion The version number of the code that will be displayed, range: [1, 40]
				 * @param codeDistance The distance to the code, e.g. distance from device center, range: (0, infinity)
				 * @param codeTiltAngle The angle at which the code normal is tilted from view direction of the device, in radian, range: [0, PI/2]
				 * @param codeViewAngle The angle relative to the view direction of the device at which the code has been observed, in radian, range: [0, PI)
				 * @param codePixelsPerModule The number of pixel per modules, range: (0, infinity)
				 * @param codeContrast The pixel contrast between fore- and background modules, range: [0, 255]
				 */
				void visualizeInWorld(const HomogenousMatrix4& world_T_code, const Scalar codeSize, const unsigned int codeObjectId, const std::string& payload, const unsigned int codeVersion, const Scalar codeDistance, const Scalar codeTiltAngle, const Scalar codeViewAngle, const Scalar codePixelsPerModule, const unsigned int codeContrast);

				/**
				 * Hides this visualization.
				 */
				void hide();

				/**
				 * Creates a visualization instance.
				 * @param engine The rendering engine that should be used to create this new instance, must be valid.
				 * @param parentScene The scene in which the visualization instance should be registered, must be valid.
				 * @return The instance that has been created.
				 */
				static CodeVisualization create(const Rendering::EngineRef& engine, const Rendering::SceneRef& parentScene);

			protected:

				/**
				 * Constructs an empty instance.
				 */
				CodeVisualization() = default;

			protected:

				/// The box transformation that will be used as an overlay for the QR codes displayed in Passthrough.
				Rendering::TransformRef boxTransform_;

				/// The box that will be used as an overlay for the QR codes displayed in Passthrough and of which the sizes need to be adjusted to sizes of the QR codes they are representing.
				Rendering::BoxRef box_;

				/// The material configuration of the box.
				Rendering::MaterialRef boxMaterial_;

				/// The location there the payload of the code should be visualized.
				Rendering::TransformRef textPayloadTransform_;

				/// The text element that visualizes the payload of a code.
				Rendering::TextRef textPayload_;

				/// The location there the object ID of the code should be visualized.
				Rendering::TransformRef textObjectIdTransform_;

				/// The text element that visualizes the object ID of a code.
				Rendering::TextRef textObjectId_;

				/// The location there the display size of the code should be visualized.
				Rendering::TransformRef textCodeSizeTransform_;

				/// The text element that visualizes the display size of a code.
				Rendering::TextRef textCodeSize_;

				/// The location there the version number of the code should be visualized.
				Rendering::TransformRef textCodeVersionTransform_;

				/// The text element that visualizes the version number of a code.
				Rendering::TextRef textCodeVersion_;

				/// The location there the distance to the code should be visualized.
				Rendering::TransformRef textCodeDistanceTransform_;

				/// The text element that visualizes the distance to the code.
				Rendering::TextRef textCodeDistance_;

				/// The location there the number of pixels per module of the code should be visualized.
				Rendering::TransformRef textCodePixelsPerModuleTransform_;

				/// The text element that visualizes the number of pixels per module of a code.
				Rendering::TextRef textCodePixelsPerModule_;

				/// The location there the tilt angle of the code should be visualized.
				Rendering::TransformRef textCodeTiltAngleTransform_;

				/// The text element that visualizes the tilt angle of a code.
				Rendering::TextRef textCodeTiltAngle_;

				/// The location there the view angle of the code should be visualized.
				Rendering::TransformRef textCodeViewAngleTransform_;

				/// The text element that visualizes the view angle of a code.
				Rendering::TextRef textCodeViewAngle_;

				/// The location there the contrast value of the code should be visualized.
				Rendering::TransformRef textCodeContrastTransform_;

				/// The text element that visualizes the constrast value of a code.
				Rendering::TextRef textCodeConstrast_;
		};

		/// Definition of a vector of code visualizations.
		typedef std::vector<CodeVisualization> CodeVisualizations;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~QuestQRCodeTrackerExperience() override;

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

		/// The rendering text object holding general text instructions.
		Rendering::TextRef renderingText_;

		/// The rendering text object displaying the current tracking mode.
		Rendering::TextRef trackingModeText_;

		/// The rendering text object displaying the current number of detected 2D codes.
		Rendering::TextRef number2DCodesText_;

		/// The transform for the text object displaying the current number of detected 2D codes.
		Rendering::TransformRef number2DCodesTransform_;

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		/// The helpers for the visualizations of QR codes.
		CodeVisualizations codeVisualizations_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// Indicates if new results are available for displaying.
		bool haveResults_ = false;

		/// The currently tracked QR codes.
		Tracking::QRCodes::QRCodeTracker3D::TrackedQRCodesMap trackedQRCodesMap_;

		/// The frames in which the QR codes have been detected.
		Frames yFrames_;

		/// The cameras that belong to the frames above.
		SharedAnyCameras anyCameras_;

		/// The poses of the device cameras relative to the device origin
		HomogenousMatrices4 device_T_cameras_;

		/// The 6DOF pose if the device origin in world coordinates
		HomogenousMatrix4 world_T_device_;

		/// The timestamp from when the code were detected
		Timestamp detectionTimestamp_;

		/// Average detection time (in milliseconds) for all processed frames together.
		double averageDetectionTimeMs_ = -1.0;

		/// True, to only scan a code and launch the code immediately
		std::atomic_bool onlyScanAndLaunch_ = false;

		/// The timestamp when the app id of the launch QR code was detected the first time.
		Timestamp onlyScanAndLaunchDetectionTimestamp_;

		/// The id of the app of the launch QR code.
		uint64_t onlyScanAndLaunchDetectionApp_ = 0ull;

		/// When true, it will disable tracking and allow 2D codes when a 6-DOF pose is not available; visualization of codes will be limited when enabled.
		std::atomic_bool forceDetectionOnlyAndAllow2DCodes_ = false;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_QRCODE_TRACKER_EXPERIENCE_H
