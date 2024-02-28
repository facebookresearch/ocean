// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QRCodeWifiExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector3D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/media/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/android/Resource.h"
	#include "ocean/platform/android/Utilities.h"

	#include "ocean/platform/meta/quest/Device.h"

	#include "ocean/platform/meta/quest/platformsdk/Application.h"

	#include "ocean/platform/meta/quest/vrapi/HandPoses.h"
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include "ocean/rendering/Utilities.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	using namespace Ocean::Platform::Meta::Quest::VrApi;
	using namespace Ocean::Platform::Meta::Quest::VrApi::Application;
#endif

namespace Ocean
{

namespace XRPlayground
{

QRCodeWifiExperience::~QRCodeWifiExperience()
{
	// nothing to do here
}

bool QRCodeWifiExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
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
			mediumUrls =
			{
				"ColorCameraId:0",
				"ColorCameraId:1",
			};
			break;

		default:
			Log::error() << "Missing case for \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			return false;
	}

	// Choose the location and size of the detection box based on the device type
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			detectionBoxSize_ = Vector3(Scalar(0.4), Scalar(0.4), Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.1), Scalar(-0.25));
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			detectionBoxSize_ = Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.05), Scalar(-0.3));
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
			detectionBoxSize_ = Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.001));
			detectionBoxTranslation_ = Vector3(Scalar(0), Scalar(-0.05), Scalar(-0.5));
			break;

		default:
			Log::error() << "Missing case for \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
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

	helpTextTransform_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &helpText_);

	if (!allCamerasAccessed)
	{
		helpTextTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));
		helpText_->setText(" Failed to access all cameras \n see https://fburl.com/access_cameras ");
	}

	experienceScene()->addChild(helpTextTransform_);

	currentSsidTextTransform_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.05), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &currentSsidText_);
	experienceScene()->addChild(currentSsidTextTransform_);


	detectionBoxTransform_ = Rendering::Utilities::createBox(engine, detectionBoxSize_, RGBAColor(1.0f, 1.0f, 1.0f, 0.5f), &detectionBox_, /* attributeSet */ nullptr, &detectionBoxMaterial_);
	detectionBoxTransform_->setVisible(true);
	experienceScene()->addChild(detectionBoxTransform_);

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	startThread();

	return true;
}

bool QRCodeWifiExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	bool allSucceeded = true;

	stopThread();

	if (!joinThread())
	{
		Log::error() << "Failed to stop the tracking thread!";
		allSucceeded = false;
	}

	frameMediums_.clear();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	vrTableMenu_.release();
#endif

	detectionBox_.release();
	detectionBoxMaterial_.release();
	detectionBoxTransform_.release();

	currentSsidText_.release();
	currentSsidTextTransform_.release();

	helpText_.release();
	helpTextTransform_.release();

	return allSucceeded;
}

Timestamp QRCodeWifiExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(!detectionBoxTransform_.isNull() && !detectionBox_.isNull() && !detectionBoxMaterial_.isNull());

	TemporaryScopedLock scopedResultLock(resultLock_);

		const std::string detectedSsid = detectedSsid_;
		const std::string detectedPassword = detectedPassword_;
		ocean_assert(currentStage_ != ST_INITIATE_WIFI_CONNECTION || (!detectedSsid.empty() && !detectedPassword.empty()));

	scopedResultLock.release();

#ifdef OCEAN_DEBUG
	static Timestamp printStageTimestamp(false);
	if (!printStageTimestamp.isValid() || printStageTimestamp + 1.0 < Timestamp(true))
	{
		Log::info() << "Current stage: " << translateStageType(currentStage_);
		printStageTimestamp = Timestamp(true);
	}
#endif

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);
#else
		const HomogenousMatrix4 world_T_device(false);
		Log::error() << "Unsupported platform";
		return timestamp;
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST


#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

	if (!currentSsidTimestamp_.isValid() || currentSsidTimestamp_ + 1.0 < Timestamp(true))
	{
		// Update the currently connected Wi-Fi network once a second
		JNIEnv* env = PlatformSpecific::get().environment();
		jobject rootActivity = PlatformSpecific::get().currentActivity(); //Platform::Android::NativeInterfaceManager::get().currentActivity();
		ocean_assert(env != nullptr && rootActivity != nullptr);

		unsigned int minSdkVersion = 0u;
		unsigned int targetSdkVersion = 0u;
		if (!Platform::Android::Utilities::manifestSdkVersions(env, rootActivity, minSdkVersion, targetSdkVersion) || std::max(minSdkVersion, targetSdkVersion) < 29u)
		{
			if (!Platform::Android::Utilities::currentWifiSsid(env, rootActivity, currentSsid_))
			{
				Log::error() << "Failed to query the SSID of the currently connect Wi-Fi network.";
				currentSsid_ = "Failed to query SSID";
			}
		}
		else
		{
			Log::error() << "Failed to query the SSID of the currently connect Wi-Fi network; Android SDK version 28 or lower is required.";
			currentSsid_ = "Failed to query SSID";
		}

		currentSsidTimestamp_ = Timestamp(true);
	}

	if (resetMenu_)
	{
		resetMenu_ = false;

		constexpr Scalar menuDistance = Scalar(-0.5);

		switch (currentStage_)
		{
			case ST_START:
			{
				const std::string label = "Scan QR code?";
				VRTableMenu::Entries entries =
				{
					{"Yes", "YES"},
				};

				const VRTableMenu::Groups groups =
				{
					{label, std::move(entries)}
				};

				vrTableMenu_.setMenuEntries(groups, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), Scalar(0.02), Scalar(0), Scalar(0));

				if (!world_T_deviceMenu_.isValid())
				{
					world_T_deviceMenu_ = world_T_device;
				}

				vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, menuDistance)), world_T_deviceMenu_);

				break;
			}

			case ST_INITIATE_WIFI_CONNECTION:
			{
				ocean_assert(!detectedSsid.empty());
				const std::string label = "Connect to <" + detectedSsid + ">?";
				VRTableMenu::Entries entries =
				{
					{"Yes", "YES"},
					{"No", "NO"},
				};

				const VRTableMenu::Groups groups =
				{
					{label, std::move(entries)}
				};

				vrTableMenu_.setMenuEntries(groups, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), Scalar(0.02), Scalar(0), Scalar(0));

				if (!world_T_deviceMenu_.isValid())
				{
					world_T_deviceMenu_ = world_T_device;
				}

				vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, menuDistance)), world_T_deviceMenu_);

				break;
			}

			case ST_QRCODE_DETECTION:
			case ST_WAIT_FOR_CONNECTION_UPDATE:
			case ST_COMPLETE_SUCCESS:
			case ST_COMPLETE_FAILURE:
				vrTableMenu_.hide();
				break;
		}
	}
	else
	{
		std::string url;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, url))
		{
			if (url.empty())
			{
				// do nothing - the user selected an entry without any defined action
			}
			else
			{
				if (currentStage_ == ST_START)
				{
					if (url == "YES")
					{
						currentStage_ = ST_QRCODE_DETECTION;
						vrTableMenu_.hide();
					}
				}
				else if (currentStage_ == ST_QRCODE_DETECTION)
				{
					// Nothing else to do.
				}
				else if (currentStage_ == ST_INITIATE_WIFI_CONNECTION)
				{
					if (url == "YES")
					{
						{
							ocean_assert(!detectedSsid.empty() && !detectedPassword.empty());

							JNIEnv* env = PlatformSpecific::get().environment();
							jobject activity = PlatformSpecific::get().currentActivity();

							unsigned int minSdkVersion = 0u;
							unsigned int targetSdkVersion = 0u;
							if (!Platform::Android::Utilities::manifestSdkVersions(env, activity, minSdkVersion, targetSdkVersion) || std::max(minSdkVersion, targetSdkVersion) < 29u)
							{
								if (Platform::Android::Utilities::connectToWifi(env, activity, detectedSsid, detectedPassword))
								{
									Log::info() << "Joining Wi-Fi network...";

									currentStage_ = ST_WAIT_FOR_CONNECTION_UPDATE;
								}
								else
								{
									Log::error() << "Failed to join WiFi network";

									currentStage_ = ST_COMPLETE_FAILURE;
								}
							}
							else
							{
								if (Platform::Android::Utilities::sendIntentToConnectToWifi(env, activity, detectedSsid, detectedPassword))
								{
									Log::info() << "Sent intent to join WiFi network...";

									currentStage_ = ST_COMPLETE_SUCCESS;
								}
								else
								{
									Log::error() << "Failed to join WiFi network";

									currentStage_ = ST_COMPLETE_FAILURE;
								}
							}

							vrTableMenu_.hide();
						}
					}
					else if (url == "NO")
					{
						currentStage_ = ST_START;
						resetMenu_ = true;
					}
				}
				else if (currentStage_ == ST_WAIT_FOR_CONNECTION_UPDATE)
				{
					// Nothing else to do.
				}
				else if (currentStage_ == ST_COMPLETE_SUCCESS || currentStage_ == ST_COMPLETE_FAILURE)
				{
					// Nothing else to do.
				}
				else
				{
					Log::error() << "Unknown menu entry";
					ocean_assert(false && "Invalid menu entry");
				}
			}
		}
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	constexpr Scalar textDistance = Scalar(-0.5);
	constexpr Scalar textHeight = Scalar(0.03);

	if (currentStage_ == ST_QRCODE_DETECTION)
	{
		const HomogenousMatrix4 device_T_detectionBox = HomogenousMatrix4(detectionBoxTranslation_);
		const HomogenousMatrix4 world_T_detectionBox = world_T_device * device_T_detectionBox;

		detectionBoxTransform_->setTransformation(world_T_detectionBox);
		detectionBoxTransform_->setVisible(true);

		const HomogenousMatrix4 device_T_helpText = HomogenousMatrix4(Vector3(Scalar(0), detectionBox_->size().y() * Scalar(0.5) + (textHeight * Scalar(0.5)), detectionBox_->size().z() * Scalar(0.5)) + device_T_detectionBox.translation());
		helpTextTransform_->setTransformation(world_T_device * device_T_helpText);
		helpText_->setSize(Scalar(0), Scalar(0), textHeight);

		detectionBoxMaterial_->setAmbientColor(RGBAColor(1.0f, 1.0f, 0.0f, 0.5f));
		detectionBoxMaterial_->setDiffuseColor(RGBAColor(1.0f, 1.0f, 0.0f, 0.5f));
		detectionBoxMaterial_->setTransparency(0.5f);

		helpText_->setText("  Scanning ...  ");
		helpTextTransform_->setVisible(true);

		currentSsidTextTransform_->setVisible(false);
	}
	else if (currentStage_ == ST_WAIT_FOR_CONNECTION_UPDATE)
	{
		const HomogenousMatrix4 world_T_helpText = world_T_device * HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.10), textDistance));

		std::string helpText;
		if (currentSsid_ != detectedSsid)
		{
			// It may take the OS a couple of second to update the Wi-Fi network.
			static Timestamp waitingToUpdateTimestamp(false);

			if (!waitingToUpdateTimestamp.isValid())
			{
				waitingToUpdateTimestamp = Timestamp(true);
			}

			const double secondsWaitingToUpdate = double(Timestamp(true) - waitingToUpdateTimestamp);

			helpText = "Updating Wi-Fi status (" + String::toAString(secondsWaitingToUpdate, 1u) + " s) ...";

			if (secondsWaitingToUpdate > 25.0)
			{
				// Something else went wrong. Let's abort.
				currentStage_ = ST_COMPLETE_FAILURE;
			}
		}
		else
		{
			currentStage_ = ST_COMPLETE_SUCCESS;
		}

		helpText_->setText(helpText);
		helpText_->setSize(Scalar(0), Scalar(0), textHeight);
		helpTextTransform_->setTransformation(world_T_helpText);
		helpTextTransform_->setVisible(true);

		detectionBoxTransform_->setVisible(false);

		currentSsidTextTransform_->setVisible(false);
	}
	else if (currentStage_ == ST_COMPLETE_SUCCESS || currentStage_ == ST_COMPLETE_FAILURE)
	{
		std::string helpText;

		if (currentStage_ == ST_COMPLETE_SUCCESS)
		{
			ocean_assert(!currentSsid_.empty() && currentSsid_ == detectedSsid);

			const HomogenousMatrix4 world_T_currentSsidText = world_T_device * HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.10), textDistance));

			std::string currentSsidText = "Wi-Fi: " + currentSsid_;
			helpText = "Success!";

			currentSsidText_->setText(currentSsidText);
			currentSsidText_->setSize(Scalar(0), Scalar(0), textHeight);
			currentSsidTextTransform_->setTransformation(world_T_currentSsidText);
			currentSsidTextTransform_->setVisible(true);
		}
		else
		{
			helpText = "Failed to join Wi-Fi network!";

			currentSsidTextTransform_->setVisible(false);
		}

		const HomogenousMatrix4 world_T_helpText = world_T_device * HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.0), textDistance));

		helpText_->setText(helpText);
		helpText_->setSize(Scalar(0), Scalar(0), textHeight);
		helpTextTransform_->setTransformation(world_T_helpText);
		helpTextTransform_->setVisible(true);

		detectionBoxTransform_->setVisible(false);
	}
	else
	{
		const HomogenousMatrix4 world_T_currentSsidText = world_T_device * HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.1), textDistance));

		std::string currentSsidText;
		if (!currentSsid_.empty())
		{
			currentSsidText = "Wi-Fi: " + currentSsid_;
		}
		else
		{
			currentSsidText = "No Wi-Fi connection";
		}

		currentSsidText_->setText(currentSsidText);
		currentSsidTextTransform_->setTransformation(world_T_currentSsidText);
		currentSsidTextTransform_->setVisible(true);

		detectionBoxTransform_->setVisible(false);
		helpTextTransform_->setVisible(false);
	}

	return timestamp;
}

void QRCodeWifiExperience::threadRun()
{
	Timestamp previousTimestamp(false);

	HighPerformanceStatistic statistic;

	while (!shouldThreadStop())
	{
		if (currentStage_ != ST_QRCODE_DETECTION)
		{
			Thread::sleep(1u);
			continue;
		}

		const HomogenousMatrix4 device_T_detectionBox = HomogenousMatrix4(detectionBoxTranslation_);

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

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(frameTimestamp);

#else

		const HomogenousMatrix4 world_T_device(false);
		Log::error() << "Unsupported platform";
		return;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		Frames yFrames;
		yFrames.reserve(frameRefs.size());

		for (size_t frameIndex = 0; frameIndex < frameRefs.size(); ++frameIndex)
		{
			const Frame& frame = *frameRefs[frameIndex];
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

			ocean_assert(yFrame.isValid());

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
			switch (Platform::Meta::Quest::Device::deviceType())
			{
				case Platform::Meta::Quest::Device::DT_QUEST:
				case Platform::Meta::Quest::Device::DT_QUEST_2:
					CV::FrameTransposer::Comfort::rotate90(yFrame, /* clockwise */ false, WorkerPool::get().scopedWorker()());
					break;

				default:
					// Nothing to do.
					break;
			}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

			yFrame.setTimestamp(frameTimestamp);

			yFrames.emplace_back(std::move(yFrame));
		}

		CV::Detector::QRCodes::QRCodes codes;
		HomogenousMatrices4 world_T_codes;
		Scalars codeSizes;

		statistic.start();
		const bool detectionSuccessful = CV::Detector::QRCodes::QRCodeDetector3D::detectQRCodes(anyCameras, yFrames, world_T_device, device_T_cameras, codes, world_T_codes, codeSizes);
		statistic.stop();

		if (!detectionSuccessful)
		{
			Log::error() << "Failed to run the QR code detection";
			ocean_assert(false && "This should never happen!");
		}

		if (statistic.measurements() % 60u == 0u)
		{
			Log::info() << "Detector performance: " << String::toAString(statistic.averageMseconds(), 2u) << " ms";
			statistic.reset();
		}

		previousTimestamp = frameTimestamp;

		if (codes.empty())
		{
			static Timestamp nextLogTimestamp = frameTimestamp;
			if (nextLogTimestamp <= frameTimestamp)
			{
				Log::info() << "No QR codes detected";
				nextLogTimestamp = frameTimestamp + 1.0;
			}
		}

		// Check if any of the detected QR codes contains Wi-Fi credentials.
		for (const CV::Detector::QRCodes::QRCode& code : codes)
		{
			const std::string data = code.dataString();

			std::string ssid;
			std::string password;
			std::string encryption;
			CV::Detector::QRCodes::Utilities::ParsingStatus parsingStatus = CV::Detector::QRCodes::Utilities::parseWifiConfig(data, ssid, password, &encryption, /* isSsidHidden */ nullptr);
			if (parsingStatus == CV::Detector::QRCodes::Utilities::PS_SUCCESS)
			{
				ocean_assert(!ssid.empty() && !password.empty());

				if (encryption == "WPA")
				{
					TemporaryScopedLock scopedLock(resultLock_);

						detectedSsid_ = ssid;
						detectedPassword_ = password;

					scopedLock.release();

					// Stop the detector and continue with the initiation of connection to the detected Wi-Fi network
					currentStage_ = ST_INITIATE_WIFI_CONNECTION;
					resetMenu_ = true;

					break;
				}
			}
			else
			{
				Log::info() << "Failed to parse wifi credentials from QR code data: " << data;
				Log::info() << "Reason for parse failure: " << CV::Detector::QRCodes::Utilities::parsingStatusToString(parsingStatus);
			}
		}
	}
}

std::unique_ptr<XRPlaygroundExperience> QRCodeWifiExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new QRCodeWifiExperience());
}

std::string QRCodeWifiExperience::translateStageType(const StageType stageType)
{
	switch (stageType)
	{
		case ST_START:
			return "START";

		case ST_QRCODE_DETECTION:
			return "QRCODE_DETECTION";

		case ST_INITIATE_WIFI_CONNECTION:
			return "INITIATE_WIFI_CONNECTION";

		case ST_WAIT_FOR_CONNECTION_UPDATE:
			return "WAIT_FOR_CONNECTION_UPDATE";

		case  ST_COMPLETE_SUCCESS:
			return "COMPLETE_SUCCESS";

		case ST_COMPLETE_FAILURE:
			return "COMPLETE_FAILURE";
	}

	ocean_assert(false && "Invalid stage type");
	return "INVALID";
}

}

}
