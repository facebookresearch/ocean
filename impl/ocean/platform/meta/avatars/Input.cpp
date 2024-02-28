// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Input.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

Input::SharedBodyTrackingData Input::createBodyTrackingData(const HomogenousMatrix4& world_T_head)
{
	SharedBodyTrackingData bodyTrackingData = std::make_shared<BodyTrackingData>();

	ovrAvatar2InputTrackingState& avatarInputTrackingState = bodyTrackingData->avatarInputTrackingState_;

	avatarInputTrackingState.headsetActive = world_T_head.isValid();

	if (world_T_head.isValid())
	{
		const VectorF3 translation(world_T_head.translation());
		const QuaternionF orientation(world_T_head.rotation());

		avatarInputTrackingState.headset.position = {translation.x(), translation.y(), translation.z()};
		avatarInputTrackingState.headset.orientation = {orientation.x(), orientation.y(), orientation.z(), orientation.w()};
		avatarInputTrackingState.headset.scale = {1.0f, 1.0f, 1.0f};

		for (unsigned int controllerIndex = 0u; controllerIndex < ovrAvatar2Side_Count; ++controllerIndex)
		{
			ocean_assert(int(controllerIndex) < ovrAvatar2Side_Count);
			avatarInputTrackingState.controllerActive[controllerIndex] = false;
			avatarInputTrackingState.controllerVisible[controllerIndex] = false;

			ovrAvatar2Transform controllerPose;
			controllerPose.position = {0.0f, 0.0f, 0.0f}; // important: always set a default pose to avoid getting Avatars SDK into a bad state
			const QuaternionF defaultQuaternion(true);
			controllerPose.orientation = {defaultQuaternion.x(), defaultQuaternion.y(), defaultQuaternion.z(), defaultQuaternion.w()};
			controllerPose.scale = {1.0f, 1.0f, 1.0f};

			avatarInputTrackingState.controller[controllerIndex] = controllerPose;
		}
	}

	ovrAvatar2InputControlState& avatarInputControlState = bodyTrackingData->avatarInputControlState_;

	avatarInputControlState.type = ovrAvatar2ControllerType_Touch;

	for (unsigned int controllerIndex = 0u; controllerIndex < ovrAvatar2Side_Count; ++controllerIndex)
	{
		ocean_assert(int(controllerIndex) < ovrAvatar2Side_Count);
		ovrAvatar2ControllerState& controllerState = avatarInputControlState.controller[controllerIndex];

		controllerState.joystickX = 0.0f;
		controllerState.joystickY = 0.0f;

		controllerState.handTrigger = 0.0f;
		controllerState.indexTrigger = 0.0f;
	}

	ovrAvatar2HandTrackingState& avatarHandTrackingState = bodyTrackingData->avatarHandTrackingState_;

	for (unsigned int handIndex = 0u; handIndex < ovrAvatar2Side_Count; ++handIndex)
	{
		ovrAvatar2Transform handPose;
		handPose.position = {0.0f, 0.0f, 0.0f}; // important: always set a default pose to avoid getting Avatars SDK into a bad state
		const QuaternionF defaultQuaternion(true);
		handPose.orientation = {defaultQuaternion.x(), defaultQuaternion.y(), defaultQuaternion.z(), defaultQuaternion.w()};
		handPose.scale = {1.0f, 1.0f, 1.0f};

		avatarHandTrackingState.wristPose[handIndex] = handPose;
		avatarHandTrackingState.handScale[handIndex] = 1.0f;

		avatarHandTrackingState.isTracked[handIndex] = false;
		avatarHandTrackingState.isConfident[handIndex] = false;
	}

	return bodyTrackingData;
}

}

}

}

}
