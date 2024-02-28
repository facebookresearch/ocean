// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"
#include "ocean/platform/meta/quest/vrapi/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

TrackedRemoteDevice::TrackedRemoteDevice(ovrMobile* ovrMobile, const Device::DeviceType deviceType) :
	ovrMobile_(ovrMobile),
	deviceType_(deviceType)
{
	// nothing to do here
}

void TrackedRemoteDevice::update(const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	ocean_assert(ovrMobile_ != nullptr);
	ocean_assert(deviceType_ != Device::DT_UNKNOWN);

	updateTimestamp_ = timestamp;

	previousButtonStateLeft_ = currentButtonStateLeft_;
	previousButtonStateRight_ = currentButtonStateRight_;

	if (deviceIdLeft_ == invalidDeviceId)
	{
		deviceIdLeft_ = acquireDevice(ovrMobile_, ovrControllerCaps_LeftHand);
	}

	if (deviceIdRight_ == invalidDeviceId)
	{
		deviceIdRight_ = acquireDevice(ovrMobile_, ovrControllerCaps_RightHand);
	}

	if (deviceType_ == Device::DT_QUEST || deviceType_ == Device::DT_QUEST_2)
	{
		if (!leftRemoteDevice_T_ringCenter_.isValid())
		{
			if (!getController_T_ringCenter(deviceType_, RT_LEFT, leftRemoteDevice_T_ringCenter_) || !leftRemoteDevice_T_ringCenter_.isValid())
			{
				ocean_assert(false && "This should never happen!");
			}
		}

		if (!rightRemoteDevice_T_ringCenter_.isValid())
		{
			if (!getController_T_ringCenter(deviceType_, RT_RIGHT, rightRemoteDevice_T_ringCenter_) || !rightRemoteDevice_T_ringCenter_.isValid())
			{
				ocean_assert(false && "This should never happen!");
			}
		}
	}

	bool leftDeviceActive = false;
	bool rightDeviceActive = false;

	if (deviceIdLeft_ != invalidDeviceId)
	{
		const uint32_t oldCurrentButtonStateLeft = currentButtonStateLeft_;
		const Vector2 oldJoystickTiltLeft = joystickTiltLeft_;

		buttonsState(deviceIdLeft_, currentButtonStateLeft_, joystickTiltLeft_);

		if (oldCurrentButtonStateLeft != currentButtonStateLeft_ || oldJoystickTiltLeft != joystickTiltLeft_)
		{
			leftDeviceActive = true;
		}
	}

	if (deviceIdRight_ != invalidDeviceId)
	{
		const uint32_t oldCurrentButtonStateRight = currentButtonStateRight_;
		const Vector2 oldJoystickTiltRight = joystickTiltRight_;

		buttonsState(deviceIdRight_, currentButtonStateRight_, joystickTiltRight_);

		if (oldCurrentButtonStateRight != currentButtonStateRight_ || oldJoystickTiltRight != joystickTiltRight_)
		{
			rightDeviceActive = true;
		}
	}

	const HomogenousMatrix4 world_T_oldLeftRemoteDevice = world_T_leftRemoteDevice_;
	const HomogenousMatrix4 world_T_oldRightRemoteDevice = world_T_rightRemoteDevice_;

	for (const RemoteType remoteType : {RT_LEFT, RT_RIGHT})
	{
		const ovrDeviceID deviceId = remoteType == RT_LEFT ? deviceIdLeft_ : deviceIdRight_;
		HomogenousMatrix4& world_T_remoteDevice = remoteType == RT_LEFT ? world_T_leftRemoteDevice_ : world_T_rightRemoteDevice_;

		if (deviceId != invalidDeviceId)
		{
			ovrTracking trackingState;
			memset(&trackingState, 0, sizeof(ovrTracking));

			if (vrapi_GetInputTrackingState(ovrMobile_, deviceId, double(updateTimestamp_), &trackingState) == ovrSuccess)
			{
				if ((trackingState.Status & VRAPI_TRACKING_STATUS_ORIENTATION_VALID) == VRAPI_TRACKING_STATUS_ORIENTATION_VALID
						&& (trackingState.Status & VRAPI_TRACKING_STATUS_POSITION_VALID) == VRAPI_TRACKING_STATUS_POSITION_VALID)
				{
					world_T_remoteDevice = Utilities::toHomogenousMatrix4<Scalar>(trackingState.HeadPose.Pose);
					ocean_assert(world_T_remoteDevice.isValid());
					continue;
				}
			}
		}

		world_T_remoteDevice.toNull();
	}

	constexpr Scalar translationThreshold = Numeric::sqr(Scalar(0.0001)); // 0.01mm
	constexpr Scalar angleThreshold = Numeric::deg2rad(Scalar(0.1)); // 0.1deg

	if (world_T_leftRemoteDevice_.isValid() && world_T_oldLeftRemoteDevice.isValid())
	{
		const HomogenousMatrix4 world_T_oldWorld = world_T_leftRemoteDevice_ * world_T_oldLeftRemoteDevice.inverted();

		if (world_T_oldWorld.translation().sqr() >= translationThreshold || world_T_oldWorld.rotation().angle() >= angleThreshold)
		{
			leftDeviceActive = true;
		}
	}

	if (world_T_rightRemoteDevice_.isValid() && world_T_oldRightRemoteDevice.isValid())
	{
		const HomogenousMatrix4 world_T_oldWorld = world_T_rightRemoteDevice_ * world_T_oldRightRemoteDevice.inverted();

		if (world_T_oldWorld.translation().sqr() >= translationThreshold || world_T_oldWorld.rotation().angle() >= angleThreshold)
		{
			rightDeviceActive = true;
		}
	}

	// setting vibration levels for this frame

	ocean_assert(nextVibrationLevel_[0] >= 0.0f && nextVibrationLevel_[0] <= 1.0f);
	ocean_assert(nextVibrationLevel_[1] >= 0.0f && nextVibrationLevel_[1] <= 1.0f);

	if (deviceIdLeft_ != invalidDeviceId)
	{
		vrapi_SetHapticVibrationSimple(ovrMobile_, deviceIdLeft_, nextVibrationLevel_[0]);
	}

	if (deviceIdRight_ != invalidDeviceId)
	{
		vrapi_SetHapticVibrationSimple(ovrMobile_, deviceIdRight_, nextVibrationLevel_[1]);
	}

	nextVibrationLevel_[0] = 0.0f;
	nextVibrationLevel_[1] = 0.0f;

	activeHandheldDevices_.clear();

	if (leftDeviceActive || leftRemoteDeviceActiveTimestamp_ + 0.25 >= timestamp)
	{
		activeHandheldDevices_.emplace_back(RT_LEFT);
	}

	if (rightDeviceActive || rightRemoteDeviceActiveTimestamp_ + 0.25 >= timestamp)
	{
		activeHandheldDevices_.emplace_back(RT_RIGHT);
	}

	if (leftDeviceActive)
	{
		leftRemoteDeviceActiveTimestamp_ = timestamp;
	}

	if (rightDeviceActive)
	{
		rightRemoteDeviceActiveTimestamp_ = timestamp;
	}
}

uint32_t TrackedRemoteDevice::buttonsPressed(const RemoteType remoteType) const
{
	ocean_assert(ovrMobile_ != nullptr);

	const ScopedLock scopedLock(lock_);

	uint32_t state = 0u;

	if (remoteType & RT_LEFT)
	{
		// a button which is currently pressed which was not pressed before
		state |= ~previousButtonStateLeft_ & currentButtonStateLeft_;
	}

	if (remoteType & RT_RIGHT)
	{
		// a button which is currently pressed which was not pressed before
		state |= ~previousButtonStateRight_ & currentButtonStateRight_;
	}

	return state;
}

uint32_t TrackedRemoteDevice::buttonsReleased(const RemoteType remoteType) const
{
	const ScopedLock scopedLock(lock_);

	uint32_t state = 0u;

	if (remoteType & RT_LEFT)
	{
		// a button which is currently not pressed which was pressed before
		state |= previousButtonStateLeft_ & ~currentButtonStateLeft_;
	}

	if (remoteType & RT_RIGHT)
	{
		// a button which is currently not pressed which was pressed before
		state |= previousButtonStateRight_ & ~currentButtonStateRight_;
	}

	return state;
}

uint32_t TrackedRemoteDevice::buttonsState(const RemoteType remoteType) const
{
	const ScopedLock scopedLock(lock_);

	uint32_t state = 0u;

	if (remoteType & RT_LEFT)
	{
		// a button which is currently not pressed which was pressed before
		state |= currentButtonStateLeft_;
	}

	if (remoteType & RT_RIGHT)
	{
		// a button which is currently not pressed which was pressed before
		state |= currentButtonStateRight_;
	}

	return state;
}

Vector2 TrackedRemoteDevice::joystickTilt(const RemoteType remoteType) const
{
	const ScopedLock scopedLock(lock_);

	if (remoteType != RT_LEFT && remoteType != RT_RIGHT)
	{
		ocean_assert(false && "Invalid input");
		return Vector2(0, 0);
	}

	return remoteType == RT_LEFT ? joystickTiltLeft_ : joystickTiltRight_;
}

bool TrackedRemoteDevice::pose(const RemoteType remoteType, HomogenousMatrix4* world_T_remoteDevice, HomogenousMatrix4* world_T_remoteDeviceRing, const Timestamp& timestamp) const
{
	if (deviceType_ == Device::DT_UNKNOWN)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (remoteType != RT_LEFT && remoteType != RT_RIGHT)
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	HomogenousMatrix4 localWorld_T_remoteDevice = remoteType == RT_LEFT ? world_T_leftRemoteDevice_ : world_T_rightRemoteDevice_;

	if (timestamp.isValid() && timestamp != updateTimestamp_)
	{
		const ovrDeviceID deviceId = remoteType == RT_LEFT ? deviceIdLeft_ : deviceIdRight_;

		if (deviceId == invalidDeviceId)
		{
			return false;
		}

		ovrTracking trackingState;
		memset(&trackingState, 0, sizeof(ovrTracking));

		const double timestampD = timestamp.isValid() ? (double)timestamp : /* use most recent sensor reading */ 0.0;

		if (vrapi_GetInputTrackingState(ovrMobile_, deviceId, timestampD, &trackingState) != ovrSuccess)
		{
			return false;
		}

		// now we check the tracking state of the controller

		if ((trackingState.Status & VRAPI_TRACKING_STATUS_ORIENTATION_VALID) != VRAPI_TRACKING_STATUS_ORIENTATION_VALID
				|| (trackingState.Status & VRAPI_TRACKING_STATUS_POSITION_VALID) != VRAPI_TRACKING_STATUS_POSITION_VALID)
		{
			return false;
		}

		localWorld_T_remoteDevice = Utilities::toHomogenousMatrix4<Scalar>(trackingState.HeadPose.Pose);
		ocean_assert(localWorld_T_remoteDevice.isValid());
	}

	if (!localWorld_T_remoteDevice.isValid())
	{
		return false;
	}

	if (world_T_remoteDevice != nullptr)
	{
		*world_T_remoteDevice = localWorld_T_remoteDevice;
	}

	if (world_T_remoteDeviceRing != nullptr)
	{
		const HomogenousMatrix4& remoteDevice_T_ringCenter = remoteType == RT_LEFT ? leftRemoteDevice_T_ringCenter_ : rightRemoteDevice_T_ringCenter_;

		if (remoteDevice_T_ringCenter.isValid())
		{
			*world_T_remoteDeviceRing = localWorld_T_remoteDevice * remoteDevice_T_ringCenter;
			ocean_assert(world_T_remoteDeviceRing->isValid());
		}
		else
		{
			Log::error() << "The ring center is unknown on this device";

			world_T_remoteDeviceRing->toNull();
		}
	}

	return true;
}

void TrackedRemoteDevice::setVibrationLevel(const RemoteType remoteType, float intensity)
{
	ocean_assert(intensity >= 0.0f && intensity <= 1.0f);

	const ScopedLock scopedLock(lock_);

	switch (remoteType)
	{
		case RT_LEFT:
			nextVibrationLevel_[0] = intensity;
			break;

		case RT_RIGHT:
			nextVibrationLevel_[1] = intensity;
			break;

		case RT_LEFT_OR_RIGHT:
			nextVibrationLevel_[0] = intensity;
			nextVibrationLevel_[1] = intensity;
			break;

		default:
			ocean_assert(false && "Invalid remote type!");
			break;
	}
}

TrackedRemoteDevice::RemoteTypes TrackedRemoteDevice::activeHandheldDevices(const RemoteType dominantRemoteType) const
{
	ocean_assert(activeHandheldDevices_.size() <= 2);
	ocean_assert(activeHandheldDevices_.size() != 2 || (activeHandheldDevices_[0] == RT_LEFT && activeHandheldDevices_[1] == RT_RIGHT));

	if (activeHandheldDevices_.size() == 2)
	{
		if (dominantRemoteType == RT_RIGHT)
		{
			return {RT_RIGHT, RT_LEFT};
		}
	}

	return activeHandheldDevices_;
}

void TrackedRemoteDevice::release()
{
	const ScopedLock scopedLock(lock_);

	*this = TrackedRemoteDevice();
}

bool TrackedRemoteDevice::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return ovrMobile_ != nullptr;
}

TrackedRemoteDevice& TrackedRemoteDevice::operator=(const TrackedRemoteDevice& object)
{
	ovrMobile_ = object.ovrMobile_;

	deviceType_ = object.deviceType_;

	deviceIdLeft_ = object.deviceIdLeft_;
	deviceIdRight_ = object.deviceIdRight_;

	previousButtonStateLeft_ = object.previousButtonStateLeft_;
	previousButtonStateRight_ = object.previousButtonStateRight_;

	currentButtonStateLeft_ = object.currentButtonStateLeft_;
	currentButtonStateRight_ = object.currentButtonStateRight_;

	world_T_leftRemoteDevice_ = object.world_T_leftRemoteDevice_;
	world_T_rightRemoteDevice_ = object.world_T_rightRemoteDevice_;

	leftRemoteDevice_T_ringCenter_ = object.leftRemoteDevice_T_ringCenter_;
	rightRemoteDevice_T_ringCenter_ = object.rightRemoteDevice_T_ringCenter_;

	leftRemoteDeviceActiveTimestamp_ = object.leftRemoteDeviceActiveTimestamp_;
	rightRemoteDeviceActiveTimestamp_ = object.rightRemoteDeviceActiveTimestamp_;

	activeHandheldDevices_ = object.activeHandheldDevices_;

	joystickTiltLeft_ = object.joystickTiltLeft_;
	joystickTiltRight_ = object.joystickTiltRight_;

	nextVibrationLevel_[0] = object.nextVibrationLevel_[0];
	nextVibrationLevel_[1] = object.nextVibrationLevel_[1];

	return *this;
}

bool TrackedRemoteDevice::getController_T_ringCenter(const Device::DeviceType deviceType, const RemoteType remoteType, HomogenousMatrix4& controller_T_ringCenter)
{
	if ((deviceType != Device::DT_QUEST && deviceType != Device::DT_QUEST_2) || (remoteType != RT_LEFT && remoteType != RT_RIGHT))
	{
		ocean_assert(false && "Invalid input");

		controller_T_ringCenter = HomogenousMatrix4(false);

		return false;
	}

	// Quaternion values (w, x, y, z) to rotate from the coordinate system defined at
	// the center of the rings to the one defined at the origin of the controller.
	// TODO These values can be refined using a more robust sampling method
	constexpr Scalar rotation[16] =
	{
		// Quest, left and right
		Scalar(0.93927), Scalar(0.34316), Scalar(0.00443), Scalar(0),
		Scalar(0.94034), Scalar(0.34023), Scalar(-0.00070), Scalar(0),

		// Miramar, left and right
		Scalar(0.92477), Scalar(0.36681), Scalar(-0.10118), Scalar(0),
		Scalar(0.92392), Scalar(0.36734), Scalar(0.10692), Scalar(0),
	};

	// Translation (x, y, z) from the ring center to the origin of the controller
	// TODO These values can be refined using a more robust sampling method
	constexpr Scalar translation[12] =
	{
		// Quest, left and right
		Scalar(-0.00949), Scalar(0.03269), Scalar(-0.00791),
		Scalar(0.00949), Scalar(0.03269), Scalar(-0.00791),

		// Miramar, left and right
		Scalar(0.01069), Scalar(0.03106), Scalar(-0.00583),
		Scalar(-0.01069), Scalar(0.03106), Scalar(-0.00583),
	};

	if (deviceType == Device::DT_QUEST)
	{
		if (remoteType == RT_LEFT)
		{
			controller_T_ringCenter = HomogenousMatrix4(Vector3(translation), Quaternion(rotation));
			return true;
		}
		else if (remoteType == RT_RIGHT)
		{
			controller_T_ringCenter = HomogenousMatrix4(Vector3(translation + 3), Quaternion(rotation + 4));
			return true;
		}
	}
	else if (deviceType == Device::DT_QUEST_2)
	{
		if (remoteType == RT_LEFT)
		{
			controller_T_ringCenter = HomogenousMatrix4(Vector3(translation + 6), Quaternion(rotation + 8));
			return true;
		}
		else if (remoteType == RT_RIGHT)
		{
			controller_T_ringCenter = HomogenousMatrix4(Vector3(translation + 9), Quaternion(rotation + 12));
			return true;
		}
	}

	controller_T_ringCenter = HomogenousMatrix4(false);

	return false;
}

void TrackedRemoteDevice::translateButtons(const uint32_t buttonsLeft, const uint32_t buttonsRight, std::vector<std::string>& translatedButtons)
{
	const uint32_t buttons = buttonsLeft | buttonsRight;

	translatedButtons.clear();

	if (buttons & ovrButton_A)
	{
		translatedButtons.emplace_back("A");
	}
	if (buttons & ovrButton_B)
	{
		translatedButtons.emplace_back("B");
	}

	if (buttons & ovrButton_RThumb)
	{
		translatedButtons.emplace_back("RThumb");
	}
	if (buttons & ovrButton_RShoulder)
	{
		translatedButtons.emplace_back("RShoulder");
	}

	if (buttons & ovrButton_X)
	{
		translatedButtons.emplace_back("X");
	}
	if (buttons & ovrButton_Y)
	{
		translatedButtons.emplace_back("Y");
	}

	if (buttons & ovrButton_LThumb)
	{
		translatedButtons.emplace_back("LThumb");
	}
	if (buttons & ovrButton_LShoulder)
	{
		translatedButtons.emplace_back("LShoulder");
	}

	if (buttons & ovrButton_Up)
	{
		translatedButtons.emplace_back("Up");
	}
	if (buttons & ovrButton_Down)
	{
		translatedButtons.emplace_back("Down");
	}
	if (buttons & ovrButton_Left)
	{
		translatedButtons.emplace_back("Left");
	}
	if (buttons & ovrButton_Right)
	{
		translatedButtons.emplace_back("Right");
	}
	if (buttons & ovrButton_Enter)
	{
		translatedButtons.emplace_back("Enter");
	}

	if (buttons & ovrButton_Back)
	{
		translatedButtons.emplace_back("Back");
	}

	if (buttons & ovrButton_GripTrigger)
	{
		if (buttonsLeft & ovrButton_GripTrigger)
		{
			translatedButtons.emplace_back("LGripTrigger");
		}

		if (buttonsRight & ovrButton_GripTrigger)
		{
			translatedButtons.emplace_back("RGripTrigger");
		}
	}
	if (buttons & ovrButton_Trigger)
	{
		if (buttonsLeft & ovrButton_Trigger)
		{
			translatedButtons.emplace_back("LTrigger");
		}

		if (buttonsRight & ovrButton_Trigger)
		{
			translatedButtons.emplace_back("RTrigger");
		}
	}
	if (buttons & ovrButton_Joystick)
	{
		if (buttonsLeft & ovrButton_Joystick)
		{
			translatedButtons.emplace_back("LJoystick");
		}

		if (buttonsRight & ovrButton_Joystick)
		{
			translatedButtons.emplace_back("RJoystick");
		}
	}
}

bool TrackedRemoteDevice::buttonsState(const ovrDeviceID deviceId, uint32_t& buttons, Vector2& joystickTilt)
{
	ovrInputStateTrackedRemote trackedRemoteState;
	trackedRemoteState.Header.ControllerType = ovrControllerType_TrackedRemote;

	if (vrapi_GetCurrentInputState(ovrMobile_, deviceId, &trackedRemoteState.Header) != ovrSuccess)
	{
		buttons = 0u;
		joystickTilt = Vector2(0, 0);

		return false;
	}

	buttons = trackedRemoteState.Buttons;
	joystickTilt = Vector2(Scalar(trackedRemoteState.Joystick.x), Scalar(trackedRemoteState.Joystick.y));

	return true;
}

ovrDeviceID TrackedRemoteDevice::acquireDevice(ovrMobile* ovrMobile, const uint32_t controllerCapability)
{
	ocean_assert(ovrMobile != nullptr);

	uint32_t deviceIndex = 0u;

	ovrInputCapabilityHeader inputCapabilityHeader;

	while (vrapi_EnumerateInputDevices(ovrMobile, deviceIndex, &inputCapabilityHeader) == ovrSuccess)
	{
		if (inputCapabilityHeader.Type == ovrControllerType_TrackedRemote)
		{
			ovrInputTrackedRemoteCapabilities inputTrackedRemoteCapabilities;
			inputTrackedRemoteCapabilities.Header = inputCapabilityHeader;

			if (vrapi_GetInputDeviceCapabilities(ovrMobile, &inputTrackedRemoteCapabilities.Header) == ovrSuccess)
			{
				if ((inputTrackedRemoteCapabilities.ControllerCapabilities & controllerCapability) == controllerCapability)
				{
					return inputCapabilityHeader.DeviceID;
				}
			}
		}

		++deviceIndex;
	}

	return invalidDeviceId;
}

}

}

}

}

}
