// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QuestQRCodeTrackerExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/media/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/meta/quest/Device.h"

	#include "ocean/platform/meta/quest/platformsdk/Application.h"
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include "ocean/rendering/Utilities.h"

using namespace Ocean::CV::Detector::QRCodes;
using namespace Ocean::Tracking::QRCodes;

namespace Ocean
{

namespace XRPlayground
{

void QuestQRCodeTrackerExperience::CodeVisualization::visualizeInWorld(const HomogenousMatrix4& world_T_code, const Scalar codeSize, const unsigned int codeObjectId, const std::string& payload, const unsigned int codeVersion, const Scalar codeDistance, const Scalar codeTiltAngle, const Scalar codeViewAngle, const Scalar codePixelsPerModule, const unsigned int codeContrast)
{
	ocean_assert(world_T_code.isValid());
	ocean_assert(codeSize > Scalar(0));
	ocean_assert(!payload.empty());
	ocean_assert(codeVersion >= 1u && codeVersion <= 40u);
	ocean_assert(codeDistance > Scalar(0));
	ocean_assert(codeTiltAngle >= Scalar(0) && codeTiltAngle <= Numeric::pi_2());
	ocean_assert(codeViewAngle >= Scalar(0) && codeViewAngle <= Numeric::pi());
	ocean_assert(codePixelsPerModule > Scalar(0));

	const RGBAColor valueSupportTypeColors[3] =
	{
		// VST_UNSUPPORTED
		RGBAColor(1.0f, 0.0f, 0.0f, 1.0f),

		// VST_MARGINAL
		RGBAColor(1.0f, 1.0f, 0.0f, 1.0f),

		// VST_NOMINAL
		RGBAColor(0.0f, 1.0f, 0.0f, 1.0f),
	};

	ValueSupportType minValueSupportType = VST_NOMINAL;

	const Scalar codeSize_2 = Scalar(0.5) * codeSize;

	std::string sizeLimitedPayload;

	if (payload.size() > 50)
	{
		sizeLimitedPayload = payload.substr(0, 25) + "..." + payload.substr(payload.size() - 25, 25);
	}
	else
	{
		sizeLimitedPayload = payload;
	}

	// Top-side of the code

	textPayload_->setText(sizeLimitedPayload);
	textPayloadTransform_->setTransformation(HomogenousMatrix4(Vector3(0, codeSize_2 + Scalar(0.01), 0)));

	// The object ID is internal and mostly used for development/debugging, so hide by default/
	textObjectIdTransform_->setVisible(false);

	// Left side of the code

	const Scalar leftAnchor = -codeSize_2 - Scalar(0.005);

	if (Rendering::MaterialRef material = textCodeVersion_->backgroundMaterial())
	{
		ValueSupportType valueSupportType;

		unsigned int maxCodeVersionNominal = (unsigned int)(-1);
		unsigned int maxCodeVersionMarginal = (unsigned int)(-1);

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			maxCodeVersionNominal = 2u;
			maxCodeVersionMarginal = 3u;
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			maxCodeVersionNominal = 10u;
			maxCodeVersionMarginal = 12u;
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
			maxCodeVersionNominal = 10u;
			maxCodeVersionMarginal = 12u;
			break;

		case Platform::Meta::Quest::Device::DT_VENTURA:
			maxCodeVersionNominal = 10u;
			maxCodeVersionMarginal = 12u;
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			break;

		// Intentionally no default case!
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		if (codeVersion <= maxCodeVersionNominal)
		{
			valueSupportType = VST_NOMINAL;
		}
		else if (codeVersion <= maxCodeVersionMarginal)
		{
			valueSupportType = VST_MARGINAL;
		}
		else
		{
			valueSupportType = VST_UNSUPPORTED;
		}

		if (minValueSupportType > valueSupportType)
		{
			minValueSupportType = valueSupportType;
		}

		material->setAmbientColor(valueSupportTypeColors[valueSupportType]);
	}

	textCodeVersion_->setText(std::string("version: V") + String::toAString(codeVersion));
	textCodeVersionTransform_->setTransformation(HomogenousMatrix4(Vector3(leftAnchor, codeSize_2 - Scalar(0.005), 0)));

	textCodeSize_->setText(std::string("size: ") + String::toAString(codeSize * Scalar(100), 1u) + " cm");
	textCodeSizeTransform_->setTransformation(HomogenousMatrix4(Vector3(leftAnchor, codeSize_2 - Scalar(0.02), 0)));

	std::string codeContrastString = "contrast: ";

	if (codeContrast <= 255u)
	{
		if (Rendering::MaterialRef material = textCodeConstrast_->backgroundMaterial())
		{
			ValueSupportType valueSupportType;

			if (codeContrast >= 35u)
			{
				valueSupportType = VST_NOMINAL;
			}
			else if (codeVersion >= 30u)
			{
				valueSupportType = VST_MARGINAL;
			}
			else
			{
				valueSupportType = VST_UNSUPPORTED;
			}

			if (minValueSupportType > valueSupportType)
			{
				minValueSupportType = valueSupportType;
			}

			material->setAmbientColor(valueSupportTypeColors[valueSupportType]);
		}

		codeContrastString += String::toAString(codeContrast);
	}
	else
	{
		codeContrastString += "---";
	}

	textCodeConstrast_->setText(codeContrastString);
	textCodeContrastTransform_->setTransformation(HomogenousMatrix4(Vector3(leftAnchor, codeSize_2 - Scalar(0.035), 0)));

	// Right side of the code

	const Scalar rightAnchor = codeSize_2 + Scalar(0.005);

	if (Rendering::MaterialRef material = textCodeTiltAngle_->backgroundMaterial())
	{
		ValueSupportType valueSupportType;

		const Scalar codeTiltAngleDeg = Numeric::rad2deg(codeTiltAngle);

		if (codeTiltAngleDeg < Scalar(35))
		{
			valueSupportType = VST_NOMINAL;
		}
		else if (codeTiltAngleDeg < Scalar(40))
		{
			valueSupportType = VST_MARGINAL;
		}
		else
		{
			valueSupportType = VST_UNSUPPORTED;
		}

		if (minValueSupportType > valueSupportType)
		{
			minValueSupportType = valueSupportType;
		}

		material->setAmbientColor(valueSupportTypeColors[valueSupportType]);
	}

	textCodeTiltAngle_->setText(std::string("Tilt angle: ") + String::toAString(Numeric::rad2deg(codeTiltAngle), 1u) + " deg");
	textCodeTiltAngleTransform_->setTransformation(HomogenousMatrix4(Vector3(rightAnchor, codeSize_2 - Scalar(0.005), 0)));

	if (Rendering::MaterialRef material = textCodeViewAngle_->backgroundMaterial())
	{
		ValueSupportType valueSupportType;

		const Scalar codeViewAngleDeg = Numeric::rad2deg(codeViewAngle);

		if (codeViewAngleDeg < Scalar(45))
		{
			valueSupportType = VST_NOMINAL;
		}
		else if (codeViewAngleDeg < Scalar(55))
		{
			valueSupportType = VST_MARGINAL;
		}
		else
		{
			valueSupportType = VST_UNSUPPORTED;
		}

		if (minValueSupportType > valueSupportType)
		{
			minValueSupportType = valueSupportType;
		}

		material->setAmbientColor(valueSupportTypeColors[valueSupportType]);
	}

	textCodeViewAngle_->setText(std::string("View angle: ") + String::toAString(Numeric::rad2deg(codeViewAngle), 1u) + " deg");
	textCodeViewAngleTransform_->setTransformation(HomogenousMatrix4(Vector3(rightAnchor, codeSize_2 - Scalar(0.02), 0)));

	std::string pixelPerModuleString = "pixels per module: ";

	if (codePixelsPerModule > Scalar(0))
	{
		pixelPerModuleString += String::toAString(Numeric::sqrt(codePixelsPerModule), 1u) + " x " + String::toAString(Numeric::sqrt(codePixelsPerModule), 1u) + " (" + String::toAString(codePixelsPerModule, 1u) + ")";
	}
	else
	{
		pixelPerModuleString += "---";
	}

	{
		ValueSupportType valueSupportType;

		const Scalar sqrtCodePixelsPerModule = Numeric::sqrt(codePixelsPerModule);

		if (sqrtCodePixelsPerModule >= Scalar(4)) // KPI actually defines 8 here
		{
			valueSupportType = VST_NOMINAL;
		}
		else if (sqrtCodePixelsPerModule >= Scalar(3.5))
		{
			valueSupportType = VST_MARGINAL;
		}
		else
		{
			valueSupportType = VST_UNSUPPORTED;
		}

		if (minValueSupportType > valueSupportType)
		{
			minValueSupportType = valueSupportType;
		}

		for (Rendering::MaterialRef material : { textCodePixelsPerModule_->backgroundMaterial(), textCodeDistance_->backgroundMaterial() } )
		{
			if (!material.isNull())
			{
				material->setAmbientColor(valueSupportTypeColors[valueSupportType]);
			}
		}
	}

	textCodePixelsPerModule_->setText(pixelPerModuleString);
	textCodePixelsPerModuleTransform_->setTransformation(HomogenousMatrix4(Vector3(rightAnchor, codeSize_2 - Scalar(0.035), 0)));

	textCodeDistance_->setText(std::string("distance: ") + String::toAString(codeDistance * Scalar(100), 1u) + " cm");
	textCodeDistanceTransform_->setTransformation(HomogenousMatrix4(Vector3(rightAnchor, codeSize_2 - Scalar(0.05), 0)));

	// Inside the code area or above it

	textObjectId_->setText(String::toAString(codeObjectId));
	textObjectIdTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, /* 1 mm */ Scalar(0.001))));

	box_->setSize(Vector3(codeSize, codeSize, /* height = 1 mm */ Scalar(0.002)));
	boxMaterial_->setAmbientColor(valueSupportTypeColors[minValueSupportType]);
	boxMaterial_->setEmissiveColor(valueSupportTypeColors[minValueSupportType]);
	boxTransform_->setTransformation(world_T_code);

	boxTransform_->setVisible(true);
}

void QuestQRCodeTrackerExperience::CodeVisualization::hide()
{
	boxTransform_->setVisible(false);
}

QuestQRCodeTrackerExperience::CodeVisualization QuestQRCodeTrackerExperience::CodeVisualization::create(const Rendering::EngineRef& engine, const Rendering::SceneRef& parentScene)
{
	ocean_assert(!engine.isNull());
	ocean_assert(!parentScene.isNull());

	CodeVisualization codeVisualization;

	// Inside the code area or above it

	codeVisualization.boxTransform_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), RGBAColor(1.0f, 1.0f, 0.0f, 0.75f), &codeVisualization.box_, /* attributeSet */ nullptr, &codeVisualization.boxMaterial_);
	codeVisualization.boxTransform_->setVisible(false);

	parentScene->addChild(codeVisualization.boxTransform_);

	codeVisualization.textObjectIdTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.05), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textObjectId_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textObjectIdTransform_);

	// Top-side of the code

	codeVisualization.textPayloadTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textPayload_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textPayloadTransform_);

	// Left side of the code

	codeVisualization.textCodeVersionTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_RIGHT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeVersion_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeVersionTransform_);

	codeVisualization.textCodeSizeTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_RIGHT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeSize_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeSizeTransform_);

	codeVisualization.textCodeContrastTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_RIGHT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeConstrast_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeContrastTransform_);

	// Right side of the code

	codeVisualization.textCodePixelsPerModuleTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodePixelsPerModule_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodePixelsPerModuleTransform_);

	codeVisualization.textCodeTiltAngleTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeTiltAngle_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeTiltAngleTransform_);

	codeVisualization.textCodeViewAngleTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeViewAngle_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeViewAngleTransform_);

	codeVisualization.textCodeDistanceTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &codeVisualization.textCodeDistance_);
	codeVisualization.boxTransform_->addChild(codeVisualization.textCodeDistanceTransform_);

	return codeVisualization;
}

QuestQRCodeTrackerExperience::~QuestQRCodeTrackerExperience()
{
	// nothing to do here
}

bool QuestQRCodeTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	bool allCamerasAccessed = true;

	std::vector<std::string> mediumUrls;

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			mediumUrls =
			{
				"StereoCamera0Id:0",
				"StereoCamera0Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			mediumUrls =
			{
				"StereoCamera0Id:0",
				"StereoCamera0Id:1",
				// "StereoCamera2Id:0", // Enable this again once the reason for lag this creates has been identified.
				// "StereoCamera2Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
		case Platform::Meta::Quest::Device::DT_VENTURA:
			mediumUrls =
			{
				"StereoCamera3Id:0",
				"StereoCamera3Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			return false;
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	for (const std::string& mediumUrl : mediumUrls)
	{
		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium)
		{
			frameMedium->start();
			frameMediums_.emplace_back(std::move(frameMedium));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	const Rendering::TransformRef transform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);
	transform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));
	experienceScene()->addChild(transform);

	if (!allCamerasAccessed)
	{
		renderingText_->setText(" Failed to access all cameras \n see https://fburl.com/access_cameras ");
	}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

	if (properties == "onlyScanAndLaunch")
	{
		Log::info() << "Running QRCode Tracker experience in 'onlyScanAndLaunch' mode";
		onlyScanAndLaunch_ = true;

		if (renderingText_->text().empty())
		{
			renderingText_->setText(" Scan a QR Code ");
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const Rendering::TransformRef trackingModeTextTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &trackingModeText_);
	trackingModeTextTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0.5, -0.8)));
	experienceScene()->addChild(trackingModeTextTransform);

	number2DCodesTransform_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.05), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &number2DCodesText_);
	number2DCodesTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0.4, -0.8)));
	number2DCodesTransform_->setVisible(forceDetectionOnlyAndAllow2DCodes_);
	experienceScene()->addChild(number2DCodesTransform_);

	startThread();

	return true;
}

bool QuestQRCodeTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	bool allSuceeded = true;

	stopThread();

	if (!joinThread())
	{
		Log::error() << "Failed to stop the tracking thread!";
		allSuceeded = false;
	}

	frameMediums_.clear();

	renderingText_.release();

	return allSuceeded;
}

Timestamp QuestQRCodeTrackerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedResultLock(resultLock_);

		const bool haveResults = haveResults_;
		haveResults_ = false;

		const QRCodeTracker3D::TrackedQRCodesMap trackedQRCodesMap = std::move(trackedQRCodesMap_);

		std::vector<QRCodeTracker3D::ObjectId> objectIds;
		QRCodes codes;
		HomogenousMatrices4 world_T_codes;
		Scalars codeSizes;

		codes.reserve(trackedQRCodesMap.size());
		world_T_codes.reserve(trackedQRCodesMap.size());
		codeSizes.reserve(trackedQRCodesMap.size());

		for (QRCodeTracker3D::TrackedQRCodesMap::const_iterator iter = trackedQRCodesMap.cbegin(); iter != trackedQRCodesMap.cend(); ++iter)
		{
			if (iter->second.trackingState() == QRCodeTracker3D::TS_TRACKING)
			{
				objectIds.emplace_back(iter->first);
				codes.emplace_back(iter->second.code());
				world_T_codes.emplace_back(iter->second.world_T_code());
				codeSizes.emplace_back(iter->second.codeSize());
			}
		}

		const Frames yFrames = std::move(yFrames_);
		const SharedAnyCameras anyCameras = std::move(anyCameras_);
		ocean_assert(yFrames.size() == anyCameras.size());

		const HomogenousMatrices4 device_T_cameras = std::move(device_T_cameras_);
		const HomogenousMatrix4 world_T_device = std::move(world_T_device_);

	scopedResultLock.release();

	if (haveResults)
	{
#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		if (onlyScanAndLaunch_)
		{
			if (onlyScanAndLaunchDetectionApp_ == 0ull)
			{
				for (const QRCode& code : codes)
				{
					const std::string prefix = "launch:";

					if (code.dataString().find(prefix) == 0)
					{
						std::string payload = code.dataString().substr(prefix.size());

						if (!payload.empty())
						{
							uint64_t appId = 0ull;
							if (String::isUnsignedInteger64(payload, &appId))
							{
								Log::info() << "Detected QR Code with launch for app '" << appId << "'";

								onlyScanAndLaunchDetectionTimestamp_ = timestamp;
								onlyScanAndLaunchDetectionApp_ = appId;

								// we show the text for a short time

								renderingText_->setText(" QR Code detected \n launching now ... ");
							}
						}
					}
				}
			}
			else if (onlyScanAndLaunchDetectionTimestamp_.isValid() && timestamp >= onlyScanAndLaunchDetectionTimestamp_ + 1.0)
			{
				Log::info() << "Launching app '" << onlyScanAndLaunchDetectionApp_ << "'";

				const std::string deepLink = "";
				Platform::Meta::Quest::PlatformSDK::Application::get().launchAnotherApplication(onlyScanAndLaunchDetectionApp_, deepLink);

				onlyScanAndLaunchDetectionTimestamp_.toInvalid();
			}
		}
		else
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		{
			while (codeVisualizations_.size() < codes.size())
			{
				codeVisualizations_.emplace_back(CodeVisualization::create(engine, experienceScene()));
			}

			unsigned int number2DCodes = 0u;
			unsigned int number3DCodes = 0u;

			for (size_t iCode = 0; iCode < codes.size(); ++iCode)
			{
				ocean_assert(iCode < world_T_codes.size());
				ocean_assert(iCode < codeSizes.size());

				ocean_assert(number3DCodes <= codeVisualizations_.size());

				const HomogenousMatrix4& world_T_code = world_T_codes[iCode];
				const Scalar codeSize = codeSizes[iCode];

				ocean_assert(codeSize > Scalar(0) == world_T_code.isValid());
				const bool is2DCode = codeSize <= Scalar(0) || !world_T_code.isValid();

				if (is2DCode)
				{
					number2DCodes++;

					// The 2D codes don't have a 6-DOF pose and cannot be rendered in 3D.
					continue;
				}
				else
				{
					number3DCodes++;
				}

				const QRCodeTracker3D::ObjectId objectId = objectIds[iCode];

				const QRCode& code = codes[iCode];

				Scalar maxCodeTiltAngle = Numeric::minValue();
				Scalar maxCodeViewAngle = Numeric::minValue();
				Scalar maxCodeDistance = Numeric::minValue();

				Scalar minNumberPixelsPerModule = Numeric::maxValue();

				unsigned int minCodeConstrast = (unsigned int)(-1);

				for (size_t iCamera = 0; iCamera < anyCameras.size(); ++iCamera)
				{
					ocean_assert(anyCameras[iCamera] != nullptr && anyCameras[iCamera]->isValid());

					const AnyCamera& anyCamera = *anyCameras[iCamera];

					const HomogenousMatrix4 world_T_camera = world_T_device * device_T_cameras[iCamera];

					Scalar codeTiltAngle = Numeric::minValue();
					Scalar codeViewAngle = Numeric::minValue();
					Scalar codeDistance = Numeric::minValue();

					if (CV::Detector::QRCodes::Utilities::computeCodeTiltAndViewAngles(world_T_camera, world_T_code, codeTiltAngle, codeViewAngle, &codeDistance))
					{
						maxCodeTiltAngle = std::max(maxCodeTiltAngle, codeTiltAngle);
						maxCodeViewAngle = std::max(maxCodeViewAngle, codeViewAngle);
						maxCodeDistance = std::max(maxCodeDistance, codeDistance);
					}

					Scalar numberPixelsPerModule = Numeric::minValue();

					if (CV::Detector::QRCodes::Utilities::computeNumberPixelsPerModule(anyCamera, world_T_camera, world_T_code, codeSizes[iCode], code.version(), /* minNumberPixelsPerModule */ nullptr, /* maxNumberPixelsPerModule */ nullptr, /* medianNumberPixelsPerModule */ nullptr, &numberPixelsPerModule))
					{
						minNumberPixelsPerModule = std::min(minNumberPixelsPerModule, numberPixelsPerModule);
					}

					unsigned int codeConstrast = (unsigned int)(-1);

					if (CV::Detector::QRCodes::Utilities::computeContrast(anyCamera, yFrames[iCamera], world_T_camera, code, world_T_code, codeSizes[iCode], /* medianContrast */ nullptr, /* averageContrast */ &codeConstrast))
					{
						minCodeConstrast = std::min(minCodeConstrast, codeConstrast);
					}
				}

				if (minNumberPixelsPerModule == Numeric::maxValue())
				{
					// Number of pixel per modules has not been updated; change value to something negative so that it won't be displayed.
					minNumberPixelsPerModule = Scalar(-1);
				}

				ocean_assert(number3DCodes != 0u && size_t(number3DCodes) <= codeVisualizations_.size());
				codeVisualizations_[number3DCodes - 1u].visualizeInWorld(world_T_code, codeSizes[iCode], objectId, code.dataString(), code.version(), maxCodeDistance, maxCodeTiltAngle, maxCodeViewAngle, minNumberPixelsPerModule, minCodeConstrast);
			}

			for (size_t iCode = number3DCodes; iCode < codeVisualizations_.size(); ++iCode)
			{
				codeVisualizations_[iCode].hide();
			}

			number2DCodesText_->setText("Found " + String::toAString(number2DCodes) + " 2D codes");
		}
	}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	if (PlatformSpecific::get().trackedRemoteDevice().buttonsPressed() & ovrButton_X)
	{
		forceDetectionOnlyAndAllow2DCodes_ = !forceDetectionOnlyAndAllow2DCodes_;

		number2DCodesTransform_->setVisible(forceDetectionOnlyAndAllow2DCodes_);
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (forceDetectionOnlyAndAllow2DCodes_)
	{
		trackingModeText_->setText(" Detection only + 2D codes ");
	}
	else
	{
		trackingModeText_->setText(" Normal Tracking ");
	}

	return timestamp;
}

void QuestQRCodeTrackerExperience::threadRun()
{
	const Tracking::QRCodes::QRCodeTracker3D::Parameters parameters;
	Tracking::QRCodes::QRCodeTracker3D tracker(parameters, CV::Detector::QRCodes::QRCodeDetector3D::detectQRCodes, /* callbackNewQRCode */ nullptr, forceDetectionOnlyAndAllow2DCodes_);

	Timestamp previousTimestamp(false);

	HighPerformanceStatistic statistic;

	while (!shouldThreadStop())
	{
		if (forceDetectionOnlyAndAllow2DCodes_ != tracker.isForceDetectionOnlyAndAllow2DCodesEnabled())
		{
			// Toggle between the modes "normal tracking" and "detection-only + 2D codes"
			tracker = Tracking::QRCodes::QRCodeTracker3D(parameters, CV::Detector::QRCodes::QRCodeDetector3D::detectQRCodes, /* callbackNewQRCode */ nullptr, forceDetectionOnlyAndAllow2DCodes_);
		}

		FrameRefs frameRefs;
		SharedAnyCameras anyCameras;
		HomogenousMatricesD4 device_T_camerasD;

		bool timedOut = false;
		if (!Media::FrameMedium::syncedFrames(frameMediums_, previousTimestamp, frameRefs, anyCameras, /* waitTime */ 2u, &timedOut, &device_T_camerasD))
		{
			if (timedOut)
			{
				Log::warning() << "Failed to access synced camera frames for timestamp";
			}

			Thread::sleep(1u);
			continue;
		}

		HomogenousMatrices4 device_T_cameras;
		device_T_cameras.reserve(device_T_camerasD.size());

		for (const HomogenousMatrixD4& device_T_cameraD : device_T_camerasD)
		{
			device_T_cameras.emplace_back(HomogenousMatrix4(device_T_cameraD));
		}

		ocean_assert(!frameRefs.empty());
		ocean_assert(frameRefs.size() == anyCameras.size());
		ocean_assert(frameRefs.size() == device_T_cameras.size());

		ocean_assert(!frameRefs[0].isNull() && frameRefs[0]->isValid());
		const Timestamp frameTimestamp = frameRefs[0]->timestamp();

		if (previousTimestamp.isValid() && previousTimestamp >= frameTimestamp)
		{
			// Only process each frame once.
			Thread::sleep(1u);
			continue;
		}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(frameTimestamp);

#else

		const HomogenousMatrix4 world_T_device(false);
		Log::error() << "Unsupported platform";
		return;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		if (!world_T_device.isValid())
		{
			Log::warning() << "Invalid transformation from world_T_device - skipping this frame.";
			continue;
		}

		Frames yFrames;
		yFrames.reserve(frameRefs.size());

		for (size_t iFrame = 0; iFrame < frameRefs.size(); ++iFrame)
		{
			const Frame& frame = *frameRefs[iFrame];
			ocean_assert(frame.isValid());

			Frame yFrame;

			CV::FrameConverter::Options options;

			if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10) ||
				FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10_PACKED))
			{
				options = CV::FrameConverter::Options(/* gamma */ 0.6f, /* allowApproximations */ true);
			}

			if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()(), options))
			{
				ocean_assert(false && "This should never happen!");
			}

			yFrame.setTimestamp(frameTimestamp);

			yFrames.emplace_back(std::move(yFrame));
		}

		QRCodeTracker3D::TrackedQRCodesMap trackedQRCodesMap;

		if (onlyScanAndLaunch_)
		{
			for (size_t cameraIndex = 0; cameraIndex < anyCameras.size(); ++cameraIndex)
			{
				const QRCodes codes = QRCodeDetector2D::detectQRCodes(*anyCameras[cameraIndex], yFrames[cameraIndex], /* observations */ nullptr, WorkerPool::get().scopedWorker()());

				// Save the results
				for (const QRCode& code : codes)
				{
					bool alreadyAdded = false;

					for (QRCodeTracker3D::TrackedQRCodesMap::const_iterator iter = trackedQRCodesMap.begin(); iter != trackedQRCodesMap.cend(); ++iter)
					{
						if (code == iter->second.code())
						{
							alreadyAdded = true;
							break;
						}
					}

					if (!alreadyAdded)
					{
						// Because this is a 2D detection, all information except for the actual QR code needs to be filled with dummy information.
						// This is fine because with `onlyScanAndLaunch_` there won't be any visualization
						const QRCodeTracker3D::TrackedQRCodesMap::key_type dummyObjectId(trackedQRCodesMap.size());

						constexpr QRCodeTracker3D::TrackingState dummyTrackingState = QRCodeTracker3D::TS_TRACKING;
						const HomogenousMatrix4 dummyWorld_T_code = HomogenousMatrix4(true);
						constexpr Scalar dummyCodeSize = Scalar(1);
						const Vectors3 dummyObjectPoints =
						{
							Vector3(Scalar(0), Scalar(0), Scalar(0)),
							Vector3(Scalar(0), Scalar(-1), Scalar(0)),
							Vector3(Scalar(1), Scalar(0), Scalar(0))
						};

						trackedQRCodesMap.emplace(std::make_pair(dummyObjectId, QRCodeTracker3D::TrackedQRCode(code, dummyWorld_T_code, dummyCodeSize, dummyObjectPoints, dummyTrackingState, yFrames[cameraIndex].timestamp())));
					}
				}
			}
		}
		else
		{
			statistic.start();
			trackedQRCodesMap = tracker.trackQRCodes(anyCameras, yFrames, world_T_device, device_T_cameras, WorkerPool::get().scopedWorker()());
			statistic.stop();
		}

		double averageDetectionTimeMs = -1.0;
		if (statistic.measurements() % 15u == 0)
		{
			averageDetectionTimeMs = statistic.averageMseconds();
			statistic.reset();
		}

		previousTimestamp = frameTimestamp;

		const ScopedLock scopedLock(resultLock_);

		trackedQRCodesMap_ = std::move(trackedQRCodesMap);

		yFrames_ = std::move(yFrames);
		anyCameras_ = std::move(anyCameras);

		device_T_cameras_ = std::move(device_T_cameras);
		world_T_device_ = std::move(world_T_device);

		detectionTimestamp_ = frameTimestamp;

		if (averageDetectionTimeMs != -1.0)
		{
			averageDetectionTimeMs_ = averageDetectionTimeMs;
		}

		haveResults_ = true;
	}
}

std::unique_ptr<XRPlaygroundExperience> QuestQRCodeTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new QuestQRCodeTrackerExperience());
}

}

}
