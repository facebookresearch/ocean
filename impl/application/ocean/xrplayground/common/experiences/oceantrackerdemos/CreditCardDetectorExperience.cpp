// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/CreditCardDetectorExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/android/Resource.h"

	#include "ocean/platform/meta/quest/Device.h"

	#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"

	#include "ocean/platform/meta/quest/platformsdk/Application.h"

	#include "ocean/platform/meta/quest/vrapi/HandPoses.h"
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include "ocean/rendering/Utilities.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	using facebook::identity_integrity::smart_capture::cc_scanner::CCScannerConfig;
	using facebook::identity_integrity::smart_capture::cc_scanner::CCScanner;

	typedef facebook::identity_integrity::smart_capture::Frame CCFrame;
	typedef facebook::identity_integrity::smart_capture::Image CCImage;
	typedef facebook::identity_integrity::smart_capture::ImageFormat CCImageFormat;
	typedef facebook::identity_integrity::smart_capture::PixelFormat CCPixelFormat;
	typedef facebook::identity_integrity::smart_capture::Point CCPoint;

	typedef facebook::identity_integrity::smart_capture::docauth::Corners CCCorners;
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

namespace Ocean
{

namespace XRPlayground
{

CreditCardDetectorExperience::~CreditCardDetectorExperience()
{
	// nothing to do here
}

bool CreditCardDetectorExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
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
				"ColorCameraId:0",
			};
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
		case Platform::Meta::Quest::Device::DT_VENTURA:
			mediumUrls =
			{
				"ColorCameraId:0",
				"ColorCameraId:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			return false;

		// Intentionally no default case!
	}

	// Choose the location and size of the detection box based on the device type
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			detectionBoxSize_ = Vector3(Scalar(3.5) * creditCardWidth_, Scalar(3.5) * creditCardHeight_, Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.1), Scalar(-0.25));
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			detectionBoxSize_ = Vector3(Scalar(2.5) * creditCardWidth_, Scalar(2.5) * creditCardHeight_, Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.05), Scalar(-0.3));
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
		case Platform::Meta::Quest::Device::DT_VENTURA:
			detectionBoxSize_ = Vector3(Scalar(2.5) * creditCardWidth_, Scalar(2.5) * creditCardHeight_, Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.05), Scalar(-0.3));
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			return false;

		// Intentionally no default case!
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


	helpTextTransform_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &helpText_);

	if (!allCamerasAccessed)
	{
		helpTextTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));
		helpText_->setText(" Failed to access all cameras \n see https://fburl.com/access_cameras ");
	}

	experienceScene()->addChild(helpTextTransform_);

	disclaimerTextTransform_ = Rendering::Utilities::createText(*engine, "  NO CREDIT CARD INFORMATION  \n  IS STORED OR TRANSMITTED!  ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(1.0f, 0.0f, 0.0f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "");
	disclaimerTextTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0.5, -2)));
	experienceScene()->addChild(disclaimerTextTransform_);

	creditCardInfoTextTransform_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &creditCardInfoText_);
	creditCardInfoTextTransform_->setVisible(false);
	experienceScene()->addChild(creditCardInfoTextTransform_);

	detectionBoxTransform_ = Rendering::Utilities::createBox(engine, detectionBoxSize_, RGBAColor(1.0f, 1.0f, 1.0f, 0.5f), &detectionBox_, /* attributeSet */ nullptr, &detectionBoxMaterial_);
	detectionBoxTransform_->setVisible(true);
	experienceScene()->addChild(detectionBoxTransform_);

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

	PlatformSpecific::get().vrHandVisualizer().setTransparency(Scalar(0.2));
	PlatformSpecific::get().vrHandVisualizer().hide();

	vrImageVisualizer_ = Platform::Meta::Quest::Application::VRImageVisualizer(engine, engine->framebuffers().front());

	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const std::string externalDirectoryName = Platform::Android::ResourceManager::get().externalFilesDirectory();

	const std::string temporaryModelDirectory = externalDirectoryName + "mobile_vision/ocr2go/credit_card_reader/";
	if (Platform::Android::ResourceManager::get().copyAssets(temporaryModelDirectory, /* createDirectory */ true, "cxx-resources/application/ocean/xrplayground/mobile_vision/ocr2go/credit_card_reader")
			&& Platform::Android::ResourceManager::get().copyAssets(temporaryModelDirectory, /* createDirectory */ true, "cxx-resources/ocean/res/application/ocean/xrplayground/mobile_vision/ocr2go/credit_card_reader"))
	{
		const IO::File ocrConfigFile = IO::File(temporaryModelDirectory + "ocr_config.json");
		const IO::File detectionModelFile = IO::File(temporaryModelDirectory + "detection_model.ptl");
		const IO::File recognitionModelFile = IO::File(temporaryModelDirectory + "recognition_model.ptl");

		if (!ocrConfigFile.exists() || !detectionModelFile.exists() || !recognitionModelFile.exists())
		{
			showMessage("Failed to load necessary model files");

			Log::debug() << "At least one file is missing:";
			Log::debug() << ocrConfigFile();
			Log::debug() << detectionModelFile();
			Log::debug() << recognitionModelFile();

			return true;
		}

		CCScannerConfig ccScannerConfig = CCScannerConfig();
		ccScannerConfig.ocrConfigPath = ocrConfigFile();
		ccScannerConfig.detModelPath = detectionModelFile();
		ccScannerConfig.recModelPath = recognitionModelFile();

		ccScanner_ = std::make_unique<CCScanner>(ccScannerConfig);
	}

	if (ccScanner_ == nullptr)
	{
		const std::string errorText = "Failed to initialize the detector";

		Log::error() << errorText;
		helpText_->setText(errorText);
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	startThread();

	return true;
}

bool CreditCardDetectorExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	bool allSucceeded = true;

	stopThread();

	if (!joinThread())
	{
		Log::error() << "Failed to stop the tracking thread!";
		allSucceeded = false;
	}

	frameMediums_.clear();

	detectionBox_.release();
	detectionBoxMaterial_.release();
	detectionBoxTransform_.release();

	disclaimerTextTransform_.release();

	helpText_.release();
	helpTextTransform_.release();

	creditCardInfoText_.release();
	creditCardInfoTextTransform_.release();

	return allSucceeded;
}

Timestamp CreditCardDetectorExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(!detectionBoxTransform_.isNull() && !detectionBox_.isNull() && !detectionBoxMaterial_.isNull());

	TemporaryScopedLock scopedResultLock(resultLock_);

		const Frames detectionFrames = std::move(detectionFrames_);
		const CreditCardInfo creditCardInfo = std::move(creditCardInfo_);
		const double averageDetectionTimeMs = averageDetectionTimeMs_;

	scopedResultLock.release();

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);
#else
		const HomogenousMatrix4 world_T_device(false);
		Log::error() << "Unsupported platform";
		return timestamp;
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const HomogenousMatrix4 device_T_detectionBox = HomogenousMatrix4(detectionBoxTranslation_);
	const HomogenousMatrix4 world_T_detectionBox = world_T_device * device_T_detectionBox;

	detectionBoxTransform_->setTransformation(world_T_detectionBox);

	const Vector3 detectionBoxCenterInWorld = world_T_detectionBox * Vector3(Scalar(0), Scalar(0), Scalar(0));
	const Scalar maxTipDistance = Scalar(0.6) * std::max(detectionBox_->size().x(), detectionBox_->size().y());
	isDetectionBoxOccupied_ = isAHandNearPoint(detectionBoxCenterInWorld, maxTipDistance);

	const HomogenousMatrix4 device_T_helpText = HomogenousMatrix4(Vector3(Scalar(0), detectionBox_->size().y() * Scalar(0.5) + Scalar(0.015), detectionBox_->size().z() * Scalar(0.5)) + device_T_detectionBox.translation());
	helpTextTransform_->setTransformation(world_T_device * device_T_helpText);
	helpText_->setSize(Scalar(0), Scalar(0), Scalar(0.01));

	if (isDetectionBoxOccupied_)
	{
		detectionBoxMaterial_->setAmbientColor(RGBAColor(1.0f, 1.0f, 0.0f, 0.5f));
		detectionBoxMaterial_->setDiffuseColor(RGBAColor(1.0f, 1.0f, 0.0f, 0.5f));
		detectionBoxMaterial_->setTransparency(0.5f);

		std::string averageDetectionTimeMsString = "";
		if (averageDetectionTimeMs > 0.0)
		{
			averageDetectionTimeMsString = "(" + String::toAString(averageDetectionTimeMs, 3u) + "ms)";
		}

		helpText_->setText("  Detector is running " + averageDetectionTimeMsString);
		helpTextTransform_->setVisible(true);
	}
	else
	{
		detectionBoxMaterial_->setAmbientColor(RGBAColor(1.0f, 1.0f, 1.0f, 0.5f));
		detectionBoxMaterial_->setDiffuseColor(RGBAColor(1.0f, 1.0f, 1.0f, 0.5f));
		detectionBoxMaterial_->setTransparency(0.5f);

		helpText_->setText("  Hold a credit card into the box below  ");
		helpTextTransform_->setVisible(true);
	}

	const HomogenousMatrix4 device_T_disclaimerText = HomogenousMatrix4(Vector3(Scalar(0), -detectionBox_->size().y() * Scalar(0.5) - Scalar(0.015), detectionBox_->size().z() * Scalar(0.5)) + device_T_detectionBox.translation());
	disclaimerTextTransform_->setTransformation(world_T_device * device_T_disclaimerText);

	const HomogenousMatrix4 device_T_creditCardInfoText = HomogenousMatrix4(Vector3(Scalar(0), detectionBox_->size().y() * Scalar(0.5) + Scalar(0.04), detectionBox_->size().z() * Scalar(0.5)) + device_T_detectionBox.translation());
	creditCardInfoTextTransform_->setTransformation(world_T_device * device_T_creditCardInfoText);

	if (creditCardInfo.isValid())
	{
		creditCardInfoText_->setText("Number: " + creditCardInfo.number() + "\nName: " + creditCardInfo.name() + "\nDate: " + creditCardInfo.expirationDate());
		creditCardInfoTextTransform_->setVisible(true);

		displayCreditCardInfoTimestamp_ = Timestamp(true);
	}

	if (displayCreditCardInfoTimestamp_.isInvalid() || displayCreditCardInfoTimestamp_ + Scalar(2) < Timestamp(true))
	{
		creditCardInfoTextTransform_->setVisible(false);
	}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	if (PlatformSpecific::get().trackedRemoteDevice().buttonsPressed() & ovrButton_A)
	{
		displayDetectionFrames_ = !displayDetectionFrames_;
	}

	ocean_assert(detectionFrames.empty() || detectionFrames.size() == 2);

	if (displayDetectionFrames_)
	{
		PlatformSpecific::get().vrHandVisualizer().show();
	}
	else
	{
		PlatformSpecific::get().vrHandVisualizer().hide();
	}

	if (displayDetectionFrames_)
	{
		ocean_assert(detectionFrames.size() <= 2);

		for (unsigned int frameIndex = 0u; frameIndex < detectionFrames.size(); ++frameIndex)
		{
			const Frame& detectionFrame = detectionFrames[frameIndex];
			ocean_assert(detectionFrame.isValid() && FrameType::arePixelFormatsCompatible(detectionFrame.pixelFormat(), FrameType::FORMAT_RGB24));

			const Scalar visualizationSize = Scalar(0.4);
			Scalar xTranslation = Scalar(0);

			if (detectionFrames.size() == 2)
			{
				xTranslation = Scalar(0.5) * (frameIndex == 0u ? -visualizationSize : visualizationSize);
			}

			const HomogenousMatrix4 view_T_frame = HomogenousMatrix4(Vector3(xTranslation, Scalar(0), Scalar(-0.5)));

			vrImageVisualizer_.visualizeImageInView(frameIndex, view_T_frame, detectionFrame, visualizationSize);
		}
	}
	else
	{
		for (unsigned int frameIndex : {0u, 1u})
		{
			vrImageVisualizer_.visualizeImageInView(frameIndex, HomogenousMatrix4(false), Frame(), Scalar(0));
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return timestamp;
}

void CreditCardDetectorExperience::threadRun()
{
	Timestamp previousTimestamp(false);

	HighPerformanceStatistic statistic;

	while (!shouldThreadStop())
	{
		const HomogenousMatrix4 device_T_detectionBox = HomogenousMatrix4(detectionBoxTranslation_);

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		if (ccScanner_ == nullptr)
		{
			Log::error() << "CCScanner is not initialized.";
			Thread::sleep(1u);
			continue;
		}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META

		if (!device_T_detectionBox.isValid() || detectionBoxSize_.x() <= Scalar(0) || detectionBoxSize_.y() <= Scalar(0))
		{
			Thread::sleep(1u);
			continue;
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

		Boxes2 detectionBoxesInImages(anyCameras.size(), Box2());
		bool haveBoundingBoxes = true;

		for (size_t cameraIndex = 0; cameraIndex < anyCameras.size(); ++cameraIndex)
		{
			if (!computeDetectionBoxInImage(*anyCameras[cameraIndex], device_T_cameras[cameraIndex], device_T_detectionBox, detectionBoxSize_.x(), detectionBoxSize_.y(), detectionBoxesInImages[cameraIndex]))
			{
				haveBoundingBoxes = false;
				break;
			}
		}

		if (!haveBoundingBoxes)
		{
			continue;
		}

		Frames detectionFrames;
		detectionFrames.reserve(frameRefs.size());

		for (size_t frameIndex = 0; frameIndex < frameRefs.size(); ++frameIndex)
		{
			const Frame& frame = *frameRefs[frameIndex];
			ocean_assert(frame.isValid());

			Frame rgbFrame;

			CV::FrameConverter::Options options;

			if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10) ||
				FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10_PACKED))
			{
				options = CV::FrameConverter::Options(/* gamma */ 0.6f, /* allowApproximations */ true);
			}

			if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_RGB24), rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()(), options))
			{
				ocean_assert(false && "This should never happen!");
			}

			ocean_assert(rgbFrame.isValid());

			const unsigned int left = (unsigned int)(detectionBoxesInImages[frameIndex].left());
			const unsigned int top = (unsigned int)(detectionBoxesInImages[frameIndex].top());
			const unsigned int detectionFrameWidth = (unsigned int)(detectionBoxesInImages[frameIndex].width());
			const unsigned int detectionFrameHeight = (unsigned int)(detectionBoxesInImages[frameIndex].height());
			ocean_assert(left + detectionFrameWidth <= frameRefs.front()->width());
			ocean_assert(top + detectionFrameHeight <= frameRefs.front()->height());

			Frame detectionFrame = rgbFrame.subFrame(left, top, detectionFrameWidth, detectionFrameHeight, Frame::CM_COPY_REMOVE_PADDING_LAYOUT);
			detectionFrame.setTimestamp(frameTimestamp);

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
			switch (Platform::Meta::Quest::Device::deviceType())
			{
				case Platform::Meta::Quest::Device::DT_QUEST:
				case Platform::Meta::Quest::Device::DT_QUEST_2:
					CV::FrameTransposer::Comfort::rotate90(detectionFrame, /* clockwise */ false, WorkerPool::get().scopedWorker()());
					break;

				case Platform::Meta::Quest::Device::DT_VENTURA:
					CV::FrameTransposer::Comfort::rotate90(detectionFrame, /* clockwise */ true, WorkerPool::get().scopedWorker()());
					break;

				default:
					// Nothing to do.
					break;
			}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

			Log::debug() << "detectionFrame[" << frameIndex << "]: " << detectionFrame.width() << " x " << detectionFrame.height();

			detectionFrames.emplace_back(std::move(detectionFrame));
		}

		CreditCardInfos creditCardInfos;

		if (isDetectionBoxOccupied_)
		{
#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
			for (size_t cameraIndex = 0; cameraIndex < anyCameras.size(); ++cameraIndex)
			{
				HighPerformanceStatistic::ScopedStatistic scopedStatistic(statistic);

				CCFrame ccFrame;
				if (!toSmartCaptureFrame(detectionFrames[cameraIndex], ccFrame))
				{
					Log::error() << "Failed to create an input image for the CCScanner";
					continue;
				}

				// The frames have been cropped already
				const int left = 0;
				const int top = 0;
				const int right = int(detectionFrames[cameraIndex].width());
				const int bottom = int(detectionFrames[cameraIndex].height());

				const CCCorners ccCorners{false, false, CCPoint{left, top}, CCPoint{right, top}, CCPoint{right, bottom}, CCPoint{left, bottom}};

				ocean_assert(ccScanner_ != nullptr);
				const facebook::identity_integrity::smart_capture::cc_scanner::CCScannerResult ccScannerResult = ccScanner_->processFrameWithCorners(std::move(ccFrame), ccCorners);

				if (ccScannerResult.isFound)
				{
					Log::debug() << "camera " << cameraIndex << ": number: " << ccScannerResult.number << ", name:   " << ccScannerResult.name << ", exp. date:   " << ccScannerResult.expiryDate;

					creditCardInfos.emplace_back(ccScannerResult.number, ccScannerResult.name, ccScannerResult.expiryDate);
				}

				if (!creditCardInfos.empty() && !enforceIdenticalDetectionsInStereo)
				{
					break;
				}
			}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		}

		double averageDetectionTimeMs = statistic.averageMseconds();
		if (statistic.measurements() % 15u == 0u)
		{
			statistic.reset();
		}

		previousTimestamp = frameTimestamp;

		const ScopedLock scopedLock(resultLock_);

		detectionFrames_ = std::move(detectionFrames);

		if (averageDetectionTimeMs != -1.0)
		{
			averageDetectionTimeMs_ = averageDetectionTimeMs;
		}

		if (!creditCardInfos.empty())
		{
			if (enforceIdenticalDetectionsInStereo)
			{
				bool areDetectedCardsIdentical = true;

				for (size_t cardIndex = 1; cardIndex < creditCardInfos.size(); ++cardIndex)
				{
					if (creditCardInfos.front() != creditCardInfos[cardIndex])
					{
						areDetectedCardsIdentical = false;
						break;
					}
				}

				if (areDetectedCardsIdentical)
				{
					creditCardInfo_ = creditCardInfos.front();
				}
			}
			else
			{
				creditCardInfo_ = creditCardInfos.front();
			}
		}
	}
}

std::unique_ptr<XRPlaygroundExperience> CreditCardDetectorExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new CreditCardDetectorExperience());
}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

bool CreditCardDetectorExperience::toSmartCaptureFrame(const Frame& frame, facebook::identity_integrity::smart_capture::Frame& ccFrame)
{
	static_assert(std::is_same<facebook::identity_integrity::smart_capture::pixel_byte_t, uint8_t>::value, "Type mismatch for pixels");

	if (!frame.isValid() || !frame.isContinuous() || frame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT || !FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24))
	{
		return false;
	}

	const size_t frameDataSize = frame.strideBytes() * frame.height();

	std::vector<uint8_t> frameData;
	frameData.resize(frameDataSize);

	memcpy(frameData.data(), frame.constdata<uint8_t>(), frameDataSize);

	// Anything except BGR causes a crash. Internally in the CCScanner, BGR is later replaced by RGB (without conversion).
	// This is likely a bug in the CCScanner, which we are going to ignore.
	constexpr CCPixelFormat ccPixelFormat = CCPixelFormat::BGR;

	CCImageFormat ccImageFormat;
	ccImageFormat.size.width = frame.width();
	ccImageFormat.size.height = frame.height();
	ccImageFormat.bytesPerRow = frame.strideBytes();
	ccImageFormat.pixelFormat = ccPixelFormat;

	CCFrame localCCFrame;
	localCCFrame.image = CCImage(ccImageFormat, frameData);

	ccFrame = std::move(localCCFrame);

	return true;
}

bool CreditCardDetectorExperience::isAHandNearPoint(const Vector3& worldPoint, const Scalar maxTipDistance, const Scalar minTipDistance)
{
#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	ocean_assert(maxTipDistance >= minTipDistance);
	ocean_assert(minTipDistance >= 0);

	const Scalar maxTipDistance2 = maxTipDistance * maxTipDistance;
	const Scalar minTipDistance2 = minTipDistance * minTipDistance;

	for (const bool left : {true, false})
	{
		Vectors3 jointPointsInWorld;
		if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, jointPointsInWorld))
		{
			return false;
		}

		constexpr ovrHandBoneIndex handBoneIndices[5] =
		{
			ovrHandBone_ThumbTip,
			ovrHandBone_IndexTip,
			ovrHandBone_MiddleTip,
			ovrHandBone_RingTip,
			ovrHandBone_PinkyTip
		};

		for (size_t index = 0; index < 5; ++index)
		{
			const Scalar distance2 = worldPoint.sqrDistance(jointPointsInWorld[handBoneIndices[index]]);

			if (distance2 <= maxTipDistance2 && distance2 >= minTipDistance2)
			{
				return true;
			}
		}
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return false;
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

bool CreditCardDetectorExperience::computeDetectionBoxInImage(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_detectionBox, const Scalar detectionBoxWidth, const Scalar detectionBoxHeight, Box2& detectionBoxInImage)
{
	ocean_assert(camera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(world_T_detectionBox.isValid());
	ocean_assert(detectionBoxWidth > Scalar(0) && detectionBoxHeight > Scalar(0));

	const Scalar detectionBoxWidth_2 = detectionBoxWidth * Scalar(0.5);
	const Scalar detectionBoxHeight_2 = detectionBoxHeight * Scalar(0.5);
	ocean_assert(detectionBoxWidth_2 > Scalar(0) && detectionBoxHeight_2 > Scalar(0));

	const Vectors3 worldPoints =
	{
		world_T_detectionBox * Vector3(-detectionBoxWidth_2, detectionBoxHeight_2, Scalar(0)), // TL
		world_T_detectionBox * Vector3(-detectionBoxWidth_2, -detectionBoxHeight_2, Scalar(0)), // BL
		world_T_detectionBox * Vector3(detectionBoxWidth_2, -detectionBoxHeight_2, Scalar(0)), // BR
		world_T_detectionBox * Vector3(detectionBoxWidth_2, detectionBoxHeight_2, Scalar(0)), // TR
	};

	Vectors2 imagePoints;
	imagePoints.reserve(worldPoints.size());

	const HomogenousMatrix4 flippedCamera_T_world = AnyCamera::standard2InvertedFlipped(world_T_camera);

	for (const Vector3& worldPoint : worldPoints)
	{
		const Vector2 imagePoint = camera.projectToImageIF(flippedCamera_T_world, worldPoint);

		if (camera.isInside(imagePoint))
		{
			imagePoints.emplace_back(imagePoint);
		}
		else
		{
			// The projected detection box is not fully inside the image.
			return false;
		}
	}

	ocean_assert(imagePoints.size() == worldPoints.size());

	detectionBoxInImage = Box2(imagePoints.data(), imagePoints.size());
	ocean_assert(detectionBoxInImage.left() >= Scalar(0) && (unsigned int)(detectionBoxInImage.left() + detectionBoxInImage.width()) <= camera.width());

	return true;
}

}

}
