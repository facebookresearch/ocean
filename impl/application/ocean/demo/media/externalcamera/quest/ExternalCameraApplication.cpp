/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/externalcamera/quest/ExternalCameraApplication.h"

#include "ocean/media/Manager.h"

#include "ocean/media/usb/USB.h"

#include "ocean/rendering/Utilities.h"

using namespace Platform::Meta::Quest;

static std::string getUsbCameraPermissionName(struct android_app* androidApp)
{
	if (androidApp->activity->sdkVersion >= 34) 
	{
		return "horizonos.permission.USB_CAMERA";
	}

	return "android.permission.CAMERA";
}

ExternalCameraApplication::ExternalCameraApplication(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	Media::USB::registerUSBLibrary();

	requestAndroidPermission(getUsbCameraPermissionName(androidApp));
}

XrSpace ExternalCameraApplication::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void ExternalCameraApplication::onAndroidPermissionGranted(const std::string& permission)
{
	VRNativeApplicationAdvanced::onAndroidPermissionGranted(permission);

	if (permission == getUsbCameraPermissionName(androidApp_))
	{
		Log::info() << "Camera permission granted";

		applicationState_ = AS_ENUMERATE_CAMERAS;
	}
}

void ExternalCameraApplication::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	ocean_assert(engine_ && framebuffer_);

	// initializing the VR table menu
	vrTableMenu_ = OpenXR::Application::VRTableMenu(engine_, framebuffer_);

	const Rendering::SceneRef scene = engine_->factory().createScene();
	framebuffer_->addScene(scene);

	renderingTransform_ = engine_->factory().createTransform();
	scene->addChild(renderingTransform_);

	Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.05) /*fixedLineHeight*/, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_BOTTOM, "", "", &renderingTextCamera_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.55), Scalar(0.01))));
	renderingTransform_->addChild(std::move(textTransform));

	textTransform = Rendering::Utilities::createText(*engine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.05) /*fixedLineHeight*/, Rendering::Text::AM_LEFT, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, "", "", &renderingTextProperties_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.55), Scalar(0.01))));
	renderingTransform_->addChild(std::move(textTransform));
}

void ExternalCameraApplication::onFramebufferReleasing()
{
	vrTableMenu_.release();

	renderingTransform_.release();
	renderingTransformCamera_.release();
	renderingTextCamera_.release();
	renderingTextProperties_.release();

	liveVideo_.release();

	framebuffer_->clearScenes();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void ExternalCameraApplication::onReleaseResources()
{
	Media::USB::unregisterUSBLibrary();

	VRNativeApplicationAdvanced::onReleaseResources();
}

void ExternalCameraApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	ocean_assert(renderingTextCamera_);

	switch (applicationState_)
	{
		case AS_IDLE:
		{
			break;
		}

		case AS_ENUMERATE_CAMERAS:
		{
			enumerateCameras();

			applicationState_ = AS_SELECT_CAMERA;

			break;
		}

		case AS_SELECT_CAMERA:
		{
			if (vrTableMenu_.isShown())
			{
				std::string menuEntryUrl;
				if (vrTableMenu_.onPreRender(trackedController(), predictedDisplayTime, menuEntryUrl))
				{
					if (!menuEntryUrl.empty())
					{
						if (menuEntryUrl == "REFRESH")
						{
							applicationState_ = AS_ENUMERATE_CAMERAS;
						}
						else
						{
							Log::info() << "Selected external device: " << menuEntryUrl;

							liveVideo_ = Media::Manager::get().newMedium(menuEntryUrl, Media::Medium::LIVE_VIDEO);

							if (liveVideo_)
							{
								applicationState_ = AS_WAITING_FOR_SUPPORTED_STREAMS;
							}
							else
							{
								Log::error() << "Failed to create the live camera";
								renderingTextCamera_->setText("Failed to create the live camera");
							}
						}

						vrTableMenu_.hide();
					}
				}
			}

			break;
		}

		case AS_WAITING_FOR_SUPPORTED_STREAMS:
		{
			if (enumerateStreamConfigurations())
			{
				applicationState_ = AS_SELECT_STREAM;
			}
			else
			{
				// the user may not have yet granted access, so we will wait until we show a warning

				static Timestamp firstTimestamp(true);

				if (firstTimestamp.hasTimePassed(10.0))
				{
					Log::warning() << "Failed to enumerate available stream configurations, permission granted?";
					renderingTextCamera_->setText("Failed to enumerate available stream configurations, permission granted?");
				}
			}

			break;
		}

		case AS_SELECT_STREAM:
		{
			if (vrTableMenu_.isShown())
			{
				std::string menuEntryUrl;
				if (vrTableMenu_.onPreRender(trackedController(), predictedDisplayTime, menuEntryUrl))
				{
					int configurationIndex = -1;
					if (String::isInteger32(menuEntryUrl, &configurationIndex))
					{
						ocean_assert(configurationIndex >= 0 && configurationIndex < int(streamConfigurations_.size()));

						Log::info() << "Selected stream configuration: " << streamConfigurations_[configurationIndex].toString();

						ocean_assert(liveVideo_);

						if (liveVideo_->setPreferredStreamConfiguration(streamConfigurations_[configurationIndex]))
						{
							if (liveVideo_->start())
							{
								if (renderingTransformCamera_)
								{
									renderingTransform_->removeChild(renderingTransformCamera_);
									renderingTransformCamera_.release();
								}

								/// let's create a 3D box (with 1meter each side), and let's use the live video as a texture
								/// the rendering engine will update the texture automatically
								renderingTransformCamera_ = Rendering::Utilities::createBox(engine_, Vector3(1, 1, 1), liveVideo_);
								renderingTransformCamera_->setVisible(false);

								renderingTransform_->addChild(renderingTransformCamera_);

								applicationState_ = AS_IDLE;
							}
							else
							{
								Log::error() << "Failed to start live video";
								renderingTextCamera_->setText("Failed to start live video");
							}
						}
						else
						{
							Log::error() << "Failed to set preferred stream configuration";
							renderingTextCamera_->setText("Failed to set preferred stream configuration");
						}
					}

					vrTableMenu_.hide();
				}
			}
		}
	}

	const Vector3 offset = Vector3(0, 0, -2); // 2 meter in front of the user

	if (anchorCameraInWorld_)
	{
		// the camera should be locked with the world, so simply placing the camera at the origin of the world

		renderingTransform_->setTransformation(HomogenousMatrix4(offset));
	}
	else
	{
		// the camera should be locked with the headset, so we need to determine the current headset pose
		// as an alternative, we could have used Rendering::AbsoluteTransform with type TT_VIEW

		const HomogenousMatrix4 world_T_device = locateSpace(xrSpaceView_.object(), xrPredictedDisplayTime);

		if (world_T_device.isValid())
		{
			renderingTransform_->setTransformation(world_T_device * HomogenousMatrix4(offset));
		}
	}

	if (liveVideo_ && liveVideo_->isStarted())
	{
		const FrameRef frame = liveVideo_->frame();

		if (frame && frame->isValid() && frame->timestamp() != lastFrameTimestamp_)
		{
			if (renderingTransformCamera_ && !renderingTransformCamera_->visible())
			{
				rateCalculator_.clear();

				// the camera screen is not yet visible, we can show the screen once we know the resolution of the camera stream

				// we have the very first frame, we could not access the pixel information e.g., via frame->constdata(), however we just need the aspect ratio

				Log::info() << "Received first camera frame, with resolution " << frame->width() << "x" << frame->height();

				const Scalar aspectRatio = Scalar(frame->width()) / Scalar(frame->height());

				const Vector3 scale = Vector3(aspectRatio, 1, 0.01); // we scale the box to get a nice flat screen with height 1 meter

				renderingTransformCamera_->setTransformation(HomogenousMatrix4(Vector3(0, 0, 0), scale));
				renderingTransformCamera_->setVisible(true);
			}

			rateCalculator_.addOccurance(Timestamp(true));

			renderingTextCamera_->setText(" Video resolution: " + String::toAString(frame->width()) + "x" + String::toAString(frame->height()) + " \n " + String::toAString(rateCalculator_.rate(Timestamp(true)), 1u) + " fps ");

			{
				std::string text;

				double minExposureDuration = 0.0;
				double maxExposureDuration = 0.0;
				Media::LiveVideo::ControlMode exposureMode = Media::LiveVideo::CM_INVALID;
				const double exposureDuration = liveVideo_->exposureDuration(&minExposureDuration, &maxExposureDuration, &exposureMode);

				if (exposureDuration >= 0.0)
				{
					text += " Exposure duration: \n " + String::toAString(exposureDuration * 1000.0, 1u) + " ms \n Mode: " + Media::LiveVideo::translateControlMode(exposureMode) + " \n Range: [" + String::toAString(minExposureDuration * 1000.0, 1u) + ", " + String::toAString(maxExposureDuration * 1000.0, 1u) + "] ms ";
				}
				else if (exposureMode != Media::LiveVideo::CM_INVALID)
				{
					text += " Exposure mode: " + Media::LiveVideo::translateControlMode(exposureMode) + " \n Manual exposure is not supported ";
				}
				else
				{
					text += " Exposure control not available ";
				}

				text += "\n\n";

				Media::LiveVideo::ControlMode focusMode = Media::LiveVideo::CM_INVALID;
				const float focus = liveVideo_->focus(&focusMode);

				if (focus >= 0.0)
				{
					text += " Focus: \n " + String::toAString(focus, 2u) + " \n Mode: " + Media::LiveVideo::translateControlMode(focusMode) + " \n Range: [0.0, 1.0] ";
				}
				else if (focusMode != Media::LiveVideo::CM_INVALID)
				{
					text += " Focus mode: " + Media::LiveVideo::translateControlMode(focusMode) + " \n Manual focus is not supported ";
				}
				else
				{
					text += " Focus control not available ";
				}

				renderingTextProperties_->setText(text);
			}

			lastFrameTimestamp_ = frame->timestamp();
		}

		if (predictedDisplayTime > disabledCustomFocusTimestamp_)
		{
			const Vector2 leftJoystickTilt = trackedController().joystickTilt(OpenXR::TrackedController::CT_LEFT);

			if (Numeric::abs(leftJoystickTilt.x()) > Scalar(0.1))
			{
				const float focus = liveVideo_->focus();

				if (focus >= 0.0f)
				{
					const float newFocus = minmax(0.0f, focus + float(leftJoystickTilt.x()) * 0.01f, 1.0f);

					if (!liveVideo_->setFocus(newFocus))
					{
						Log::warning() << "Failed to set new focus " << newFocus << ", was " << focus << " range is [0, 1]";
					}
				}
			}
		}

		if (predictedDisplayTime > disabledCustomExposureDurationTimestamp_)
		{
			const Vector2 rightJoystickTilt = trackedController().joystickTilt(OpenXR::TrackedController::CT_RIGHT);

			if (Numeric::abs(rightJoystickTilt.x()) > Scalar(0.1))
			{
				double minDurtation = -1.0f;
				double maxDurtation = -1.0f;
				const double duration = liveVideo_->exposureDuration(&minDurtation, &maxDurtation);

				if (duration >= 0.0)
				{
					const double newDuration = minmax(minDurtation, duration + double(rightJoystickTilt.x()) * 0.001, maxDurtation);

					if (!liveVideo_->setExposureDuration(newDuration))
					{
						Log::warning() << "Failed to set new exposure duration " << newDuration << ", was " << duration << " range is [" << minDurtation << ", " << maxDurtation << "]";
					}
				}
			}
		}
	}
}

void ExternalCameraApplication::onButtonPressed(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp)
{
	if (buttons & OpenXR::TrackedController::BT_LEFT_JOYSTICK)
	{
		// let's ensure that the focus does not change due to an unintended joystick movement

		disabledCustomFocusTimestamp_ = timestamp + 0.5;

		if (!liveVideo_->setFocus(-1.0f))
		{
			Log::warning() << "Failed to enable auto focus mode";
		}
	}

	if (buttons & OpenXR::TrackedController::BT_RIGHT_JOYSTICK)
	{
		// let's ensure that the exposure rate does not change due to an unintended joystick movement

		disabledCustomExposureDurationTimestamp_ = timestamp + 0.5;

		if (!liveVideo_->setExposureDuration(-1.0))
		{
			Log::warning() << "Failed to enable auto exposure mode";
		}
	}
}

void ExternalCameraApplication::onButtonReleased(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp)
{
	if ((buttons & OpenXR::TrackedController::BT_LEFT_Y) == OpenXR::TrackedController::BT_LEFT_Y
			|| (buttons & OpenXR::TrackedController::BT_RIGHT_B) == OpenXR::TrackedController::BT_RIGHT_B)
	{
		if (liveVideo_)
		{
			liveVideo_->stop();
			liveVideo_.release();
		}

		if (renderingTransformCamera_)
		{
			renderingTransformCamera_->clear();
		}

		renderingTextCamera_->setText("");
		renderingTextProperties_->setText("");

		applicationState_ = AS_ENUMERATE_CAMERAS;
	}
	else if ((buttons & OpenXR::TrackedController::BT_LEFT_X) == OpenXR::TrackedController::BT_LEFT_X
			|| (buttons & OpenXR::TrackedController::BT_RIGHT_A) == OpenXR::TrackedController::BT_RIGHT_A)
	{
		anchorCameraInWorld_ = !anchorCameraInWorld_;
	}
}

void ExternalCameraApplication::enumerateCameras()
{
	Media::Library::Definitions selectableMedia = Media::Manager::get().selectableMedia(Media::Medium::LIVE_VIDEO);

	OpenXR::Application::VRTableMenu::Entries menuGroupEntries;

	for (const Media::Library::Definition& definition : selectableMedia)
	{
		menuGroupEntries.emplace_back(definition.url(), definition.url());
	}

	std::string deviceGroupName = menuGroupEntries.empty() ? "No external camera found" : "Select an external camera";

	if (!menuGroupEntries.empty())
	{
		// let's place an empty entry as separation
		menuGroupEntries.emplace_back("", "");
	}
	menuGroupEntries.emplace_back("Refresh device list", "REFRESH");

	const OpenXR::Application::VRTableMenu::Groups menuGroups =
	{
		{std::move(deviceGroupName), std::move(menuGroupEntries)},
	};

	vrTableMenu_.setMenuEntries(menuGroups);

	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1))); // place the menu 1 meter in front of the user
}

bool ExternalCameraApplication::enumerateStreamConfigurations()
{
	ocean_assert(liveVideo_);

	const Media::LiveVideo::StreamTypes streamTypes = liveVideo_->supportedStreamTypes();

	if (streamTypes.empty())
	{
		return false;
	}

	streamConfigurations_.clear();

	OpenXR::Application::VRTableMenu::Groups menuGroups;

	Log::info() << "Supported stream types: " << streamTypes.size();

	for (const Media::LiveVideo::StreamType streamType : streamTypes)
	{
		Log::info() << "Supported stream type: " << Media::LiveVideo::translateStreamType(streamType);

		Media::LiveVideo::StreamConfigurations streamConfigurations = liveVideo_->supportedStreamConfigurations(streamType);

		Log::info() << streamConfigurations.size() << " configurations:";

		OpenXR::Application::VRTableMenu::Entries menuGroupEntries;
		menuGroupEntries.reserve(streamConfigurations.size());

		FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;
		Media::LiveVideo::CodecType codecType = Media::LiveVideo::CT_INVALID;

		size_t configurationsSelectedPerStreamType = 0;

		for (size_t n = 0; n < streamConfigurations.size(); ++n)
		{
			const Media::LiveVideo::StreamConfiguration& streamConfiguration = streamConfigurations[n];

			// let's ensure that we don't display too many streams (could be handled with a sub-menu otherwise)

			if (configurationsSelectedPerStreamType >= 3 && streamConfiguration.width_ != 640u && streamConfiguration.width_ != 1920u && streamConfiguration.width_ != 1280u)
			{
				Log::info() << n << ": " << streamConfiguration.toString() + " (skipping)";
				continue;
			}

			Log::info() << n << ": " << streamConfiguration.toString();

			ocean_assert(!streamConfiguration.frameRates_.empty());
			const double frameRate = streamConfiguration.frameRates_.empty() ? 0.0 : streamConfiguration.frameRates_.front();

			if (pixelFormat == FrameType::FORMAT_UNDEFINED)
			{
				pixelFormat = streamConfiguration.framePixelFormat_;
			}

			if (codecType == Media::LiveVideo::CT_INVALID)
			{
				codecType = streamConfiguration.codecType_;
			}

			ocean_assert(pixelFormat == streamConfiguration.framePixelFormat_); // we don't expect that a pixel format or a codec type changes for a stream type
			ocean_assert(codecType == streamConfiguration.codecType_);

			std::string menuEntryText = String::toAString(streamConfiguration.width_) + "x" + String::toAString(streamConfiguration.height_) + ", " + String::toAString(frameRate, 1u) + "fps";
			std::string menuEntryUrl = String::toAString(streamConfigurations_.size());

			menuGroupEntries.emplace_back(std::move(menuEntryText), std::move(menuEntryUrl));

			streamConfigurations_.emplace_back(streamConfiguration);

			++configurationsSelectedPerStreamType;
		}

		std::string menuGroupName = translateStreamType(streamType, pixelFormat, codecType);

		if (streamConfigurations.size() != configurationsSelectedPerStreamType)
		{
			menuGroupName += " (only a subset)";
		}

		menuGroups.emplace_back(std::move(menuGroupName), std::move(menuGroupEntries));
	}

	vrTableMenu_.setMenuEntries(menuGroups);

	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1))); // place the menu 1 meter in front of the user

	return true;
}

std::string ExternalCameraApplication::translateStreamType(const Media::LiveVideo::StreamType streamType, const FrameType::PixelFormat pixelFormat, const Media::LiveVideo::CodecType codecType)
{
	switch (streamType)
	{
		case Media::LiveVideo::ST_INVALID:
			return "Invalid";

		case Media::LiveVideo::ST_FRAME:
		{
			std::string result = "Uncompressed Stream";

			if (pixelFormat != FrameType::FORMAT_UNDEFINED)
			{
				result += ", " + FrameType::translatePixelFormat(pixelFormat);
			}

			return result;
		}

		case Media::LiveVideo::ST_MJPEG:
			return "MotionJPEG stream";

		case Media::LiveVideo::ST_CODEC:
		{
			std::string result = "Compressed Stream";

			if (codecType != Media::LiveVideo::CT_INVALID)
			{
				result += ", " + Media::LiveVideo::translateCodecType(codecType);
			}

			return result;
		}
	}

	ocean_assert(false && "Invalid stream type!");
	return "Invalid";
}
