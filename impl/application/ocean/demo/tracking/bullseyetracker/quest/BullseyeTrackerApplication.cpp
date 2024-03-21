// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/bullseyetracker/quest/BullseyeTrackerApplication.h"

#include "ocean/base/Base.h"
#include "ocean/base/Median.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Numeric.h"

#include "ocean/platform/android/Resource.h"
#include "ocean/platform/android/Utilities.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Utilities.h"

#include <vros/sys/sensors/FrameType.h>

using namespace Ocean::Media;
using namespace Ocean::Platform::Meta;
using namespace Ocean::Platform::Meta::Quest;
using namespace Ocean::Platform::Meta::Quest::Application;
using namespace Ocean::Platform::Meta::Quest::OpenXR;
using namespace Ocean::Platform::Meta::Quest::Sensors;

bool BullseyeTrackerApplication::BullseyeDetector::detectBullseye(const Frame& frame, Bullseye& bullseye)
{
	ocean_assert(frame.isValid() && frame.pixelFormat() == FrameType::FORMAT_Y8);

	if (frame.pixelFormat() != FrameType::FORMAT_Y8)
	{
		return false;
	}

	const CV::Detector::MessengerCodeDetector::Bullseyes bullseyes = CV::Detector::MessengerCodeDetector::detectBullseyes(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), nullptr);

	if (bullseyes.empty())
	{
		return false;
	}

	// do some naive non-maximum suprression

	Scalars xBullseyes;
	Scalars yBullseyes;

	for (size_t n = 0; n < bullseyes.size(); ++n)
	{
		xBullseyes.push_back(bullseyes[n].position().x());
		yBullseyes.push_back(bullseyes[n].position().y());
	}

	const Vector2 bullseyeCandidate = Vector2(Median::median(xBullseyes.data(), xBullseyes.size()), Median::median(yBullseyes.data(), yBullseyes.size()));

	for (size_t n = 0; n < bullseyes.size(); ++n)
	{
		if (bullseyeCandidate.sqrDistance(bullseyes[n].position()) <= Numeric::sqr(2))
		{
			bullseye = bullseyes[n];
			return true;
		}
	}

	return false;
}

bool BullseyeTrackerApplication::BullseyeDetector::determineBullseyeObjectPoint(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Vector2& bullseyeA, const Vector2& bullseyeB, Vector3& worldObjectPoint, Scalar* projectionErrorA, Scalar* projectionErrorB)
{
	ocean_assert(sharedAnyCameraA->isValid() && sharedAnyCameraB->isValid());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameraA.isValid() && device_T_cameraB.isValid());

	ocean_assert(sharedAnyCameraA->isInside(bullseyeA));
	ocean_assert(sharedAnyCameraB->isInside(bullseyeB));

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

	Vector3 objectPoint;

	// two rays starting at the center of projections and intersecting the bullseyes in the top views

	const Line3 rayA = sharedAnyCameraA->ray(bullseyeA, world_T_cameraA);
	const Line3 rayB = sharedAnyCameraB->ray(bullseyeB, world_T_cameraB);

	if (rayA.nearestPoint(rayB, objectPoint))
	{
		if (AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), objectPoint)
			&& AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), objectPoint))
		{
			// we have a valid 3D object point

			if (projectionErrorA)
			{
				const Vector2 projectedObjectPointA = sharedAnyCameraA->projectToImage(world_T_cameraA, objectPoint);
				*projectionErrorA = projectedObjectPointA.distance(bullseyeA);
			}

			if (projectionErrorB)
			{
				const Vector2 projectedObjectPointB = sharedAnyCameraB->projectToImage(world_T_cameraB, objectPoint);
				*projectionErrorB = projectedObjectPointB.distance(bullseyeB);
			}

			worldObjectPoint = objectPoint;

			return true;
		}
	}

	return false;
}

BullseyeTrackerApplication::BullseyeTrackerApplication(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	requestAndroidPermission("com.oculus.permission.ACCESS_MR_SENSOR_DATA");
}

XrSpace BullseyeTrackerApplication::baseSpace() const
{
	return xrSpaceLocal_.object();
}

void BullseyeTrackerApplication::onAndroidPermissionGranted(const std::string& permission)
{
	VRNativeApplicationAdvanced::onAndroidPermissionGranted(permission);

	if (permission == "com.oculus.permission.ACCESS_MR_SENSOR_DATA")
	{
		if (!initializeCameras())
		{
			ocean_assert(false && "This shouldn't happen!");
		}
	}
}

void BullseyeTrackerApplication::onButtonPressed(const TrackedController::ButtonType buttons, const Timestamp& timestamp)
{
	VRNativeApplicationAdvanced::onButtonPressed(buttons, timestamp);

	if (buttons & TrackedController::BT_LEFT_X)
	{
		if (!switchToNextCameraType())
		{
			Log::error() << "Failed to switch cameras";
		}
	}
}

void BullseyeTrackerApplication::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	ocean_assert(engine_);
	ocean_assert(framebuffer_);

	scene_ = engine_->factory().createScene();
	framebuffer_->addScene(scene_);

	markerTransform_ = Rendering::Utilities::createSphere(engine_, Scalar(0.025), RGBAColor(0.85f, 1.0f, 0.0f, 0.8f));
	markerTransform_->setTransformation(HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.35), Scalar(-0.8))));
	markerTransform_->setVisible(false);

	Rendering::TransformRef markerCenterTransform = Rendering::Utilities::createSphere(engine_, Scalar(0.003), RGBAColor(3.0f, 0.0f, 0.0f, 1.0f));
	markerTransform_->addChild(markerCenterTransform);

	scene_->addChild(markerTransform_);

	int manifestVersionCode = -1;
	if (Platform::Android::Utilities::manifestVersionCode(androidApp_->activity->vm, androidApp_->activity->clazz, manifestVersionCode))
	{
		ocean_assert(manifestVersionCode >= 0);
		const std::string manifestVersionCodeString = "App version: " + (manifestVersionCode >= 0 ? String::toAString(manifestVersionCode) : "unknown");
		vrTextVisualizer_.visualizeTextInWorld(TV_MANIFEST_VERSION_CODE, HomogenousMatrix4(Vector3(0, 0.4, -0.8)), manifestVersionCodeString, VRVisualizer::ObjectSize(0, 0.025));
	}
}

void BullseyeTrackerApplication::onFramebufferReleasing()
{
	// Stop the bullseye detection thread
	stopThread();

	markerTransform_.release();
	scene_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void BullseyeTrackerApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (passthrough_.isValid() && !passthrough_.isStarted())
	{
		if (!passthrough_.start())
		{
			Log::error() << "Failed to start passthrough";
		}
	}

	ocean_assert(scene_);

	if (cameraFrameTypeIndex_ >= cameraFrameTypes_.size())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const OSSDK::Sensors::v3::FrameType cameraFrameType = cameraFrameTypes_[cameraFrameTypeIndex_].first;

	TemporaryScopedLock scopedResultLock(resultLock_);

	const bool haveResults = haveResults_;
	haveResults_ = false;

	const Vector3 bullseyeCenter = std::move(bullseyeCenter_);

	scopedResultLock.release();

	// Display the name of the current camera
	const std::string cameraNameString = "Camera: " + FrameProvider::translateCameraFrameType(cameraFrameType);
	vrTextVisualizer_.visualizeTextInWorld(TV_CAMERA_NAME, HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.25), Scalar(-0.8))), cameraNameString, VRVisualizer::ObjectSize(Scalar(0), Scalar(0.025)));

	// Display pop-up messages
	const std::vector<std::string> popupMessages = getPopupMessages();
	std::string popupMessageString;
	Scalar popupMessageHeight = Scalar(0);

	if (!popupMessages.empty())
	{
		popupMessageHeight = Scalar(0.025) * Scalar(popupMessages.size());

		popupMessageString = popupMessages.front();
		for (size_t i = 1; i < popupMessages.size(); ++i)
		{
			popupMessageString += "\n" + popupMessages[i];
		}
	}

	vrTextVisualizer_.visualizeTextInWorld(TV_TIMED_POPUP_MESSAGES, HomogenousMatrix4(Vector3(Scalar(0), Scalar(0.0), Scalar(-0.7))), popupMessageString, VRVisualizer::ObjectSize(Scalar(0), popupMessageHeight));

	if (haveResults)
	{
		markerTransform_->setTransformation(HomogenousMatrix4(bullseyeCenter));
		markerTransform_->setVisible(true);
	}
}

void BullseyeTrackerApplication::threadRun()
{
	ocean_assert(frameProvider_.isValid());

	ocean_assert(androidApp_ != nullptr);

	Frames frames;
	SharedAnyCamerasD camerasD;
	HomogenousMatrixD4 ossdkWorld_T_deviceD;
	HomogenousMatricesD4 device_T_camerasD;

	SharedAnyCameras cameras;

	Frames yFrames;

	while (androidApp_->destroyRequested == 0 && !shouldThreadStop())
	{
		if (!frameProvider_.latestFrames(frames, &camerasD, &ossdkWorld_T_deviceD, &device_T_camerasD))
		{
			Thread::sleep(1u);
			continue;
		}

		ocean_assert(!camerasD.empty());
		ocean_assert(camerasD.size() == frames.size());
		ocean_assert(camerasD.size() == device_T_camerasD.size());
		ocean_assert(ossdkWorld_T_deviceD.isValid());

		const Timestamp frameTimestamp = frames.front().timestamp();

		// hack: workaround as the FrameProvider's ossdkWorld_T_device transformation is slightly different from OpenXR's local space
		// needs to be addressed once the we understand how to determine the offset between OpenXR's local space and OSSDK's head tracker pose
		const HomogenousMatrix4 world_T_device = locateSpace(xrSpaceView_.object(), frameTimestamp);

		if (!world_T_device.isValid())
		{
			Log::warning() << "Invalid OpennXR device pose";
		}

		yFrames.resize(frames.size());
		cameras.resize(camerasD.size());

		HomogenousMatrices4 device_T_cameras(camerasD.size());

		for (size_t i = 0; i < frames.size(); ++i)
		{
			if (!convertFrameToY8(frames[i], yFrames[i]))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			cameras[i] = AnyCamera::convert(camerasD[i]);
			ocean_assert(cameras[i] != nullptr && cameras[i]->isValid());

			device_T_cameras[i] = HomogenousMatrix4(device_T_camerasD[i]);
		}

		BullseyeDetector::Bullseye bullseyeA;
		if (!BullseyeDetector::detectBullseye(yFrames[0], bullseyeA))
		{
			continue;
		}

		BullseyeDetector::Bullseye bullseyeB;
		if (!BullseyeDetector::detectBullseye(yFrames[1], bullseyeB))
		{
			continue;
		}

		Vector3 bullseyeCenter;
		Scalar pixelErrorA;
		Scalar pixelErrorB;
		if (!BullseyeDetector::determineBullseyeObjectPoint(cameras[0], cameras[1], world_T_device, device_T_cameras[0], device_T_cameras[1], bullseyeA.position(), bullseyeB.position(), bullseyeCenter, &pixelErrorA, &pixelErrorB))
		{
			continue;
		}

		const ScopedLock scopedLock(resultLock_);

		bullseyeCenter_ = std::move(bullseyeCenter);

		haveResults_ = true;
	}
}

bool BullseyeTrackerApplication::initializeCameras()
{
	frameProvider_.initialize(FrameProvider::FCM_MAKE_COPY);

	// Select the cameras based on the current device
	CameraFrameTypes cameraFrameTypes;
	switch (deviceType())
	{
		case Quest::Device::DT_QUEST:
		case Quest::Device::DT_QUEST_2:
			cameraFrameTypes = CameraFrameTypes
			{
				{OSSDK::Sensors::v3::FrameType::Headset, Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS}
			};
			break;

		case Quest::Device::DT_QUEST_PRO:
			cameraFrameTypes = CameraFrameTypes
			{
				{OSSDK::Sensors::v3::FrameType::GenericSlot0, Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS},
				{OSSDK::Sensors::v3::FrameType::GenericSlot1, Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS},
				{OSSDK::Sensors::v3::FrameType::GenericSlot2, Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS}
			};
			break;

		case Quest::Device::DT_QUEST_3:
		case Quest::Device::DT_VENTURA:
			cameraFrameTypes = CameraFrameTypes
			{
				{OSSDK::Sensors::v3::FrameType::Color, Platform::Meta::Quest::Sensors::FrameProvider::CT_ALL_CAMERAS},
				{OSSDK::Sensors::v3::FrameType::GenericSlot0, Platform::Meta::Quest::Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS}
			};
			break;

		case Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown device";
			ocean_assert(false && "Never be here!");

			return false;

		// no default case
	}

	ocean_assert(!cameraFrameTypes.empty());

	cameraFrameTypes_.clear();

	for (size_t n = 0; n < cameraFrameTypes.size(); ++n)
	{
		if (frameProvider_.isCameraFrameTypeAvailable(cameraFrameTypes[n].first))
		{
			cameraFrameTypes_.emplace_back(cameraFrameTypes[n]);
		}
	}

	if (cameraFrameTypes_.empty())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// Start the camera streams using the first available camera

	frameProvider_.stopReceivingCameraFrames();

	cameraFrameTypeIndex_ = 0;

	if (cameraFrameTypeIndex_ < cameraFrameTypes_.size())
	{
		if (frameProvider_.startReceivingCameraFrames(cameraFrameTypes_[cameraFrameTypeIndex_].first, cameraFrameTypes_[cameraFrameTypeIndex_].second, /* useRealtimeCalibration */ true ))
		{
			Log::info() << "Receiving camera stream: " << FrameProvider::translateCameraFrameType(cameraFrameTypes_[cameraFrameTypeIndex_].first);

			// Start the bullseye detection thread
			startThread();

			return true;
		}
	}

	ocean_assert(false && "This should never happen!");
	Log::error() << "Failed to start the camera stream for " << FrameProvider::translateCameraFrameType(cameraFrameTypes_[cameraFrameTypeIndex_].first);

	return false;
}

bool BullseyeTrackerApplication::switchToNextCameraType()
{
	if (cameraFrameTypes_.empty() || !frameProvider_.isValid())
	{
		Log::error() << "Cannot switch - no cameras available! Required permission may be missing.";
		return false;
	}

	ocean_assert(cameraFrameTypeIndex_  < cameraFrameTypes_.size());
	cameraFrameTypeIndex_ = (cameraFrameTypeIndex_ + 1u) % cameraFrameTypes_.size();

	frameProvider_.stopReceivingCameraFrames();

	if (cameraFrameTypeIndex_ < cameraFrameTypes_.size())
	{
		if (frameProvider_.startReceivingCameraFrames(cameraFrameTypes_[cameraFrameTypeIndex_].first, cameraFrameTypes_[cameraFrameTypeIndex_].second, /* useRealtimeCalibration */ true ))
		{
			return true;
		}
	}

	ocean_assert(false && "Never be here!");
	return false;
}

void BullseyeTrackerApplication::displayPopupMessage(const std::string& message, const double durationInSeconds)
{
	ocean_assert(false && message.empty());
	ocean_assert(durationInSeconds > 0.0);

	const Timestamp validUntilTimestamp(Timestamp(true) + durationInSeconds);

	bool foundDuplicate = false;
	PopupMessageQueue::iterator iter = popupMessageQueue_.begin();

	while (iter != popupMessageQueue_.end())
	{
		// Avoid duplicated messages but extent their display time, if applicable
		if (iter->first == message)
		{
			if (iter->second < validUntilTimestamp)
			{
				iter->second = validUntilTimestamp;
			}

			foundDuplicate = true;
			break;
		}

		++iter;
	}

	if (!foundDuplicate)
	{
		popupMessageQueue_.emplace_back(message, validUntilTimestamp);
	}
}

std::vector<std::string> BullseyeTrackerApplication::getPopupMessages()
{
	if (popupMessageQueue_.empty())
	{
		return std::vector<std::string>();
	}

	const Timestamp deletionTimestamp(true);

	std::vector<std::string> popupMessages;
	popupMessages.reserve(popupMessageQueue_.size());

	PopupMessageQueue::iterator iter = popupMessageQueue_.begin();

	while (iter != popupMessageQueue_.end())
	{
		if (iter->second < deletionTimestamp)
		{
			iter = popupMessageQueue_.erase(iter);
		}
		else
		{
			popupMessages.emplace_back(iter->first);

			++iter;
		}
	}

	return popupMessages;
}

bool BullseyeTrackerApplication::convertFrameToY8(const Frame& frame, Frame& yFrame)
{
	ocean_assert(frame.isValid());

	if (frame.pixelFormat() == FrameType::FORMAT_RGGB10_PACKED)
	{
		const CV::FrameConverter::Options options(/* black level */ 64u, /* white balance red */ 2.0f, /* green */ 1.0f, /* blue */ 2.0f, /* gamma */ 1.0f / 2.4f);

		Frame intermediateFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_RGB24), intermediateFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, options))
		{
			return false;
		}

		return convertFrameToY8(intermediateFrame, yFrame);
	}

	CV::FrameConverter::Options options;

	if (frame.pixelFormat() == FrameType::FORMAT_Y10 || frame.pixelFormat() == FrameType::FORMAT_Y10_PACKED)
	{
		options = CV::FrameConverter::Options(Scalar(0.5));
	}

	return CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, options);
}
