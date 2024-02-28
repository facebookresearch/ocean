// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/quest/InputQuest.h"

#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

using namespace Quest::VrApi;

namespace Avatars
{

namespace Quest
{

InputQuest::SharedBodyTrackingData InputQuest::createBodyTrackingData(ovrMobile* ovr, const Meta::Quest::Device::DeviceType deviceType, const TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp)
{
	ocean_assert(ovr != nullptr);

	SharedBodyTrackingData bodyTrackingData = std::make_shared<BodyTrackingData>();

	bodyTrackingData->avatarInputTrackingState_ = inputTrackingState(ovr, trackedRemoteDevice, timestamp);
	bodyTrackingData->avatarInputControlState_ = inputControlState(deviceType, trackedRemoteDevice);
	bodyTrackingData->avatarHandTrackingState_ = handTrackingState(bodyTrackingData->avatarInputTrackingState_, ovr, timestamp);

    return bodyTrackingData;
}

ovrAvatar2InputTrackingState InputQuest::inputTrackingState(ovrMobile* ovr, const TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp)
{
	ocean_assert(ovr != nullptr);
	ocean_assert(timestamp.isValid());

	ovrAvatar2InputTrackingState avatarInputTrackingState = {};

	const HomogenousMatrix4 world_T_device = HeadsetPoses::world_T_device(ovr, timestamp);

	avatarInputTrackingState.headsetActive = world_T_device.isValid();

	if (avatarInputTrackingState.headsetActive)
	{
		const Vector3 headsetTranslation = world_T_device.translation();
		const Quaternion headsetRotation = world_T_device.rotation();

		ovrAvatar2Transform headsetPose;
		headsetPose.position = {float(headsetTranslation.x()), float(headsetTranslation.y()), float(headsetTranslation.z())};
		headsetPose.orientation = {float(headsetRotation.x()), float(headsetRotation.y()), float(headsetRotation.z()), float(headsetRotation.w())};
		headsetPose.scale = {1.0f, 1.0f, 1.0f};

		avatarInputTrackingState.headsetActive = true;
		avatarInputTrackingState.headset = headsetPose;

		const TrackedRemoteDevice::RemoteType remoteTypes[2] = {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT};

		for (unsigned int controllerIndex = 0u; controllerIndex < 2u; ++controllerIndex)
		{
			ocean_assert(int(controllerIndex) < ovrAvatar2Side_Count);
			avatarInputTrackingState.controllerActive[controllerIndex] = false;
			avatarInputTrackingState.controllerVisible[controllerIndex] = false;

			ovrAvatar2Transform controllerPose;
			controllerPose.position = {0.0f, 0.0f, 0.0f}; // important: always set a default pose to avoid getting Avatars SDK into a bad state
			const QuaternionF defaultQuaternion(true);
			controllerPose.orientation = {defaultQuaternion.x(), defaultQuaternion.y(), defaultQuaternion.z(), defaultQuaternion.w()};
			controllerPose.scale = {1.0f, 1.0f, 1.0f};

			const TrackedRemoteDevice::RemoteType remoteType = remoteTypes[controllerIndex];

			HomogenousMatrix4 world_T_remoteDevice(false);
			if (trackedRemoteDevice.pose(remoteType, &world_T_remoteDevice, nullptr, timestamp))
			{
				ocean_assert(world_T_remoteDevice.isValid());

				Vector3 controllerTranslation = world_T_remoteDevice.translation();
				const Quaternion controllerRotation = world_T_remoteDevice.rotation();

				if (controllerTranslation.sqrDistance(headsetTranslation) > Numeric::sqr(Scalar(0.7)))
				{
					// HACK: temporary workaround until the underyling Avatars system comes with a better handling in case controllers are too far away from the user

					controllerTranslation = headsetTranslation + (controllerTranslation - headsetTranslation).normalized() * Scalar(0.7);
				}

				avatarInputTrackingState.controllerActive[controllerIndex] = true;
				avatarInputTrackingState.controllerVisible[controllerIndex] = true;

				controllerPose.position = {float(controllerTranslation.x()), float(controllerTranslation.y()), float(controllerTranslation.z())};
				controllerPose.orientation = {float(controllerRotation.x()), float(controllerRotation.y()), float(controllerRotation.z()), float(controllerRotation.w())};
			}

			avatarInputTrackingState.controller[controllerIndex] = controllerPose;
		}
	}

	return avatarInputTrackingState;
}

ovrAvatar2InputControlState InputQuest::inputControlState(const Meta::Quest::Device::DeviceType deviceType, const TrackedRemoteDevice& trackedRemoteDevice)
{
	ovrAvatar2InputControlState avatarInputControlState = {};

	// **TODO** decouple controller types from device (e.g., Quest Pro controllers may be used on Quest 2)

	switch (deviceType)
	{
		case Meta::Quest::Device::DT_QUEST:
			avatarInputControlState.type = ovrAvatar2ControllerType_Touch;
			break;

		case Meta::Quest::Device::DT_QUEST_2:
			avatarInputControlState.type = ovrAvatar2ControllerType_Quest2;
			break;

		case Meta::Quest::Device::DT_QUEST_PRO:
			avatarInputControlState.type = ovrAvatar2ControllerType_QuestPro;
			break;

		case Meta::Quest::Device::DT_UNKNOWN:
		case Meta::Quest::Device::DT_QUEST_3:
		case Meta::Quest::Device::DT_VENTURA:
		{
			static bool first = true;

			if (first)
			{
				first = false;
				Log::warning() << "Avatar2 does not yet support '" << Meta::Quest::Device::deviceName(deviceType) << "' yet, initializing with Quest2 instead";
			}

			avatarInputControlState.type = ovrAvatar2ControllerType_Quest2;
			break;
		}
	}

	const TrackedRemoteDevice::RemoteType remoteTypes[2] = {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT};

	for (unsigned int controllerIndex = 0u; controllerIndex < 2u; ++controllerIndex)
	{
		ocean_assert(int(controllerIndex) < ovrAvatar2Side_Count);
		ovrAvatar2ControllerState& controllerState = avatarInputControlState.controller[controllerIndex];

		const TrackedRemoteDevice::RemoteType remoteType = remoteTypes[controllerIndex];

		const uint32_t controllerButtonsState = trackedRemoteDevice.buttonsState(remoteType);
		const Vector2 joystick = trackedRemoteDevice.joystickTilt(remoteType);

		controllerState.joystickX = joystick.x();
		controllerState.joystickY = joystick.y();

		if (controllerButtonsState & ovrButton_GripTrigger)
		{
			controllerState.handTrigger = 1.0f;
		}
		else
		{
			controllerState.handTrigger = 0.0f;
		}

		if (controllerButtonsState & ovrButton_Trigger)
		{
			controllerState.indexTrigger = 1.0f;
		}
		else
		{
			controllerState.indexTrigger = 0.0f;
		}

		if (controllerButtonsState & (ovrButton_A | ovrButton_X))
		{
			controllerState.buttonMask = ovrAvatar2Button(controllerState.buttonMask | ovrAvatar2Button_One);
		}

		if (controllerButtonsState & (ovrButton_B | ovrButton_Y))
		{
			controllerState.buttonMask = ovrAvatar2Button(controllerState.buttonMask | ovrAvatar2Button_Two);
		}

		if (controllerButtonsState & ovrButton_Joystick)
		{
			controllerState.buttonMask = ovrAvatar2Button(controllerState.buttonMask | ovrAvatar2Button_Joystick);
		}
	}

	return avatarInputControlState;
}

ovrAvatar2HandTrackingState InputQuest::handTrackingState(const ovrAvatar2InputTrackingState& inputTrackingState, ovrMobile* ovr, const Timestamp& timestamp)
{
	ocean_assert(ovr != nullptr);
	ocean_assert(timestamp.isValid());

	ovrAvatar2HandTrackingState avatarHandTrackingState = {};

	static_assert(ovrAvatar2Side_Count == 2, "Invalid parameter!");

	for (unsigned int handIndex = 0u; handIndex < ovrAvatar2Side_Count; ++handIndex)
	{
		avatarHandTrackingState.isTracked[handIndex] = false;
		avatarHandTrackingState.isConfident[handIndex] = false;

		ovrAvatar2Transform handPose;
		handPose.position = {0.0f, 0.0f, 0.0f}; // important: always set a default pose to avoid getting Avatars SDK into a bad state
		const QuaternionF defaultQuaternion(true);
		handPose.orientation = {defaultQuaternion.x(), defaultQuaternion.y(), defaultQuaternion.z(), defaultQuaternion.w()};
		handPose.scale = {1.0f, 1.0f, 1.0f};

		avatarHandTrackingState.wristPose[handIndex] = handPose;
		avatarHandTrackingState.handScale[handIndex] = 1.0f;
	}

	int deviceIndex = 0;
	ovrInputCapabilityHeader inputCapabilityHeader;

	while (vrapi_EnumerateInputDevices(ovr, deviceIndex++, &inputCapabilityHeader) >= 0)
	{
		if (inputCapabilityHeader.Type == ovrControllerType_Hand)
		{
			ovrInputHandCapabilities inputHandCapabilities;
			inputHandCapabilities.Header = inputCapabilityHeader;

			if (vrapi_GetInputDeviceCapabilities(ovr, &inputHandCapabilities.Header) >= 0)
			{
				const bool isLeftHand = (inputHandCapabilities.HandCapabilities & ovrHandCaps_LeftHand) == ovrHandCaps_LeftHand;

				ovrHandPose handPose = {};
				handPose.Header.Version = ovrHandVersion_1;

				const ovrAvatar2Side sideIndex = isLeftHand ? ovrAvatar2Side_Left : ovrAvatar2Side_Right;
				const unsigned int jointOffset = isLeftHand ? 0 : (ovrAvatar2HandTrackingBoneId_Count / 2);

				if (vrapi_GetHandPose(ovr, inputHandCapabilities.Header.DeviceID, double(timestamp), &handPose.Header) == ovrSuccess)
				{
					if ((handPose.Status & ovrHandTrackingStatus_Tracked) == ovrHandTrackingStatus_Tracked)
					{
						avatarHandTrackingState.isTracked[sideIndex] = true;
						avatarHandTrackingState.isConfident[sideIndex] = handPose.HandConfidence == ovrConfidence_HIGH;

						memcpy(&avatarHandTrackingState.wristPose[sideIndex].orientation, &handPose.RootPose.Orientation, sizeof(handPose.RootPose.Orientation));
						memcpy(&avatarHandTrackingState.wristPose[sideIndex].position, &handPose.RootPose.Position, sizeof(handPose.RootPose.Position));
						avatarHandTrackingState.wristPose[sideIndex].scale = {1.0f, 1.0f, 1.0f};

						static_assert(ovrAvatar2HandTrackingBoneId_Count == (ovrHandBone_Pinky3 - ovrHandBone_Thumb0 + 1) * 2);
						memcpy(avatarHandTrackingState.boneRotations + jointOffset, handPose.BoneRotations + ovrHandBone_Thumb0,	sizeof(ovrAvatar2Quatf) * (ovrAvatar2HandTrackingBoneId_Count / 2));
					}
				}
			}
		}
	}

	// HACK: need to adjust hand position due to head pose to avoid that Avatars SDK renders extremely long hands

	const ovrAvatar2Vector3f& avatarHeadsetPosition = inputTrackingState.headset.position;

	const Vector3 headsetPosition(avatarHeadsetPosition.x, avatarHeadsetPosition.y, avatarHeadsetPosition.z);

	for (unsigned int handIndex = 0u; handIndex < 2u; ++handIndex)
	{
		ovrAvatar2Vector3f& avatarHandPosition = avatarHandTrackingState.wristPose[handIndex].position;

		Vector3 handPosition(avatarHandPosition.x, avatarHandPosition.y, avatarHandPosition.z);

		if (handPosition.sqrDistance(headsetPosition) > Numeric::sqr(Scalar(0.7)))
		{
			handPosition = headsetPosition + (handPosition - headsetPosition).normalized() * Scalar(0.7);

			avatarHandPosition.x = float(handPosition.x());
			avatarHandPosition.y = float(handPosition.y());
			avatarHandPosition.z = float(handPosition.z());
		}
	}

	return avatarHandTrackingState;
}

ovrAvatar2Platform InputQuest::translateDeviceType(const Meta::Quest::Device::DeviceType deviceType)
{
    ovrAvatar2Platform ovrPlatform = ovrAvatar2Platform_EnumSize;

	switch (deviceType)
	{
		case Meta::Quest::Device::DT_QUEST:
			Log::debug() << "Using Avatar2 on Quest1";
			ovrPlatform = ovrAvatar2Platform_Quest;
			break;

		case Meta::Quest::Device::DT_QUEST_2:
			Log::debug() << "Using Avatar2 on Quest2";
			ovrPlatform = ovrAvatar2Platform_Quest2;
			break;

		case Meta::Quest::Device::DT_QUEST_PRO:
			Log::debug() << "Using Avatar2 on Quest Pro";
			ovrPlatform = ovrAvatar2Platform_QuestPro;
			break;

		case Meta::Quest::Device::DT_QUEST_3:
		case Meta::Quest::Device::DT_VENTURA:
			Log::warning() << "Avatar2 does not yet support '" << Meta::Quest::Device::deviceName(deviceType) << "' yet, initializing with Quest2 instead";
			ovrPlatform = ovrAvatar2Platform_Quest2;
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: '" << Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "'";
			break;

		// Intentionally no default case!
	}

    return ovrPlatform;
}

}

}

}

}

}
