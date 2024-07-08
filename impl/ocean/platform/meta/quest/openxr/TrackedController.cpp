/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/TrackedController.h"

#include "ocean/platform/openxr/Utilities.h"

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

TrackedController::~TrackedController()
{
	release();
}

bool TrackedController::initialize(const XrInstance& xrInstance, const XrSession& xrSession)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSession != XR_NULL_HANDLE);

	if (xrInstance == XR_NULL_HANDLE || xrSession == XR_NULL_HANDLE)
	{
		return false;
	}

	if (actionSet_)
	{
		ocean_assert(false && "This TrackedController has already been initialized");
		return true;
	}

	SharedActionSet actionSet = std::make_shared<ActionSet>();

	constexpr uint32_t priority = 0u;

	if (!actionSet->initialize(xrInstance, "actionset_ocean_trackedcontroller", "ActionSet TrackedController", priority))
	{
		Log::error() << "OpenXR TrackedController: Failed to initialize action set";
		return false;
	}

	bool allSucceeded = true;

	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_x", "Left X", "/user/hand/left/input/x/click", BT_LEFT_X);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_y", "Left Y", "/user/hand/left/input/y/click", BT_LEFT_Y);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_trigger", "Left Trigger", "/user/hand/left/input/trigger/value", BT_LEFT_TRIGGER);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_grip", "Left Grip", "/user/hand/left/input/squeeze/value", BT_LEFT_GRIP);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_menu", "Left Menu", "/user/hand/left/input/menu/click", BT_LEFT_MENU);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "left_joystick", "Left Joystick", "/user/hand/left/input/thumbstick/click", BT_LEFT_JOYSTICK);

	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_a", "Right A", "/user/hand/right/input/a/click", BT_RIGHT_A);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_b", "Right B", "/user/hand/right/input/b/click", BT_RIGHT_B);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_trigger", "Right Trigger", "/user/hand/right/input/trigger/value", BT_RIGHT_TRIGGER);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_grip", "Right Grip", "/user/hand/right/input/squeeze/value", BT_RIGHT_GRIP);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_system", "Right Menu", "/user/hand/right/input/system/click", BT_RIGHT_SYSTEM);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_BOOLEAN_INPUT, "right_joystick", "Right Joystick", "/user/hand/right/input/thumbstick/click", BT_RIGHT_JOYSTICK);

	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_POSE_INPUT, "left_pose_aim", "Left Pose Aim", "/user/hand/left/input/aim/pose", AT_LEFT_AIM);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_POSE_INPUT, "left_pose_grip", "Left Pose Grip", "/user/hand/left/input/grip/pose", AT_LEFT_GRIP);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_POSE_INPUT, "right_pose_aim", "Right Pose Aim", "/user/hand/right/input/aim/pose", AT_RIGHT_AIM);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_POSE_INPUT, "right_pose_grip", "Right Pose Grip", "/user/hand/right/input/grip/pose", AT_RIGHT_GRIP);

	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_VECTOR2F_INPUT, "left_joystick_vector", "Left Joystick Vector", "/user/hand/left/input/thumbstick", AT_LEFT_JOYSTICK_VECTOR);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_VECTOR2F_INPUT, "right_joystick_vector", "Right Joystick Vector", "/user/hand/right/input/thumbstick", AT_RIGHT_JOYSTICK_VECTOR);

	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_VIBRATION_OUTPUT, "left_vibration", "Left Vibration", "/user/hand/left/output/haptic", AT_LEFT_VIBRATION);
	allSucceeded = allSucceeded && actionSet->createActionWithBinding(XR_ACTION_TYPE_VIBRATION_OUTPUT, "right_vibration", "Right Vibration", "/user/hand/right/output/haptic", AT_RIGHT_VIBRATION);

	if (!allSucceeded)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!actionSet->suggestActionBindings(xrInstance, "/interaction_profiles/oculus/touch_controller"))
	{
		Log::error() << "OpenXR VRNativeApplicationAdvanced: Failed to suggest action bindings";

		return false;
	}

	spaceLeftAim_ = Session::createActionSpace(xrInstance, xrSession, actionSet->action(AT_LEFT_AIM));
	spaceLeftGrip_ = Session::createActionSpace(xrInstance, xrSession, actionSet->action(AT_LEFT_GRIP));

	spaceRightAim_ = Session::createActionSpace(xrInstance, xrSession, actionSet->action(AT_RIGHT_AIM));
	spaceRightGrip_ = Session::createActionSpace(xrInstance, xrSession, actionSet->action(AT_RIGHT_GRIP));

	xrSession_ = xrSession;

	actionSet_ = std::move(actionSet);

	xrActionStateMap_ =
	{
		{BT_LEFT_X, {}},
		{BT_LEFT_Y, {}},
		{BT_LEFT_TRIGGER, {}},
		{BT_LEFT_GRIP, {}},
		{BT_LEFT_MENU, {}},
		{BT_LEFT_JOYSTICK, {}},

		{BT_RIGHT_A, {}},
		{BT_RIGHT_B, {}},
		{BT_RIGHT_TRIGGER, {}},
		{BT_RIGHT_GRIP, {}},
		{BT_RIGHT_SYSTEM, {}},
		{BT_RIGHT_JOYSTICK, {}}
	};

	return true;
}

bool TrackedController::update(const XrSpace& baseSpace, const XrTime& xrPredictedDisplayTime)
{
	ocean_assert(baseSpace != XR_NULL_HANDLE);

	const ScopedLock scopedLock(lock_);

	ocean_assert(xrSession_ != XR_NULL_HANDLE);
	if (xrSession_ == XR_NULL_HANDLE)
	{
		return false;
	}

	ocean_assert(actionSet_);

	// first, we update the states of the individual buttons

	buttonsState_ = BT_NONE;
	buttonsPressed_ = BT_NONE;
	buttonsReleased_ = BT_NONE;

	ButtonType buttonsState = BT_NONE;
	ButtonType buttonsPressed = BT_NONE;
	ButtonType buttonsReleased = BT_NONE;

	for (XrActionStateMap::value_type& actionState : xrActionStateMap_)
	{
		const ButtonType buttonType = actionState.first;
		XrActionStateBoolean& actionStateBoolean = actionState.second;

		if  (!actionSet_->actionState(xrSession_, buttonType, actionStateBoolean))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (actionStateBoolean.isActive)
		{
			buttonsState = ButtonType(buttonsState | buttonType);

			if (actionStateBoolean.changedSinceLastSync)
			{
				if (actionStateBoolean.currentState)
				{
					buttonsPressed = ButtonType(buttonsPressed | buttonType);
				}
				else
				{
					buttonsReleased = ButtonType(buttonsPressed | buttonType);
				}
			}
		}
	}

	buttonsState_ = buttonsState;
	buttonsPressed_ = buttonsPressed;
	buttonsReleased_ = buttonsReleased;


	// now, we update the individual poses

	static constexpr XrSpaceLocationFlags expectedLocationFlags = XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;

	const HomogenousMatrix4 baseSpace_T_previousLeftAim(baseSpace_T_leftAim_);
	const HomogenousMatrix4 baseSpace_T_previousRightAim(baseSpace_T_rightAim_);

	constexpr size_t numberPoses = 4;

	const XrSpace spaces[numberPoses] = {spaceLeftAim_.object(), spaceLeftGrip_.object(), spaceRightAim_.object(), spaceRightGrip_.object()};
	HomogenousMatrix4* const baseSpace_T_poses[numberPoses] = {&baseSpace_T_leftAim_, &baseSpace_T_leftGrip_, &baseSpace_T_rightAim_, &baseSpace_T_rightGrip_};

	XrSpaceLocation xrSpaceLocation{XR_TYPE_SPACE_LOCATION};

	for (size_t n = 0; n < numberPoses; ++n)
	{
		if (xrLocateSpace(spaces[n], baseSpace, xrPredictedDisplayTime, &xrSpaceLocation) == XR_SUCCESS)
		{
			if ((xrSpaceLocation.locationFlags & expectedLocationFlags) == expectedLocationFlags)
			{
				*baseSpace_T_poses[n] = Utilities::toHomogenousMatrix4<Scalar>(xrSpaceLocation.pose);
				continue;
			}
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		baseSpace_T_poses[n]->toNull();
	}


	// now, we update the joystick vectors

	for (const bool left : {false, true})
	{
		const ActionType actionType = left ? AT_LEFT_JOYSTICK_VECTOR : AT_RIGHT_JOYSTICK_VECTOR;
		Vector2& joystickVector = left ? joystickVectorLeft_ : joystickVectorRight_;

		XrActionStateVector2f actionStateVector = {};

		if (!actionSet_->actionState(xrSession_, actionType, actionStateVector))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (actionStateVector.isActive)
		{
			joystickVector = Vector2(Scalar(actionStateVector.currentState.x), Scalar(actionStateVector.currentState.y));
		}
		else
		{
			joystickVector = Vector2(0, 0);
		}
	}


	// now, we update the vibration

	for (size_t controllerIndex = 0; controllerIndex < numberControllers_; ++controllerIndex)
	{
		if (xrHapticVibrations_[controllerIndex].type == XR_TYPE_HAPTIC_VIBRATION)
		{
			XrHapticActionInfo xrHapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
			xrHapticActionInfo.action = actionSet_->action(controllerIndex == 0 ? AT_LEFT_VIBRATION : AT_RIGHT_VIBRATION);

			if (xrApplyHapticFeedback(xrSession_, &xrHapticActionInfo, (const XrHapticBaseHeader*)(xrHapticVibrations_ + controllerIndex)) != XR_SUCCESS)
			{
				Log::error() << "Failed to set haptic feedback";
			}

			xrHapticVibrations_[controllerIndex].type = XR_TYPE_UNKNOWN;
		}
	}


	// finally, we determine which controller is currently active

	bool leftControllerActive = ((buttonsPressed_ | buttonsReleased_) & BT_LEFT_ALL) != BT_NONE;
	bool rightControllerActive = ((buttonsPressed_ | buttonsReleased_) & BT_RIGHT_ALL) != BT_NONE;

	constexpr Scalar translationThreshold = Numeric::sqr(Scalar(0.0001)); // 0.01mm
	constexpr Scalar angleThreshold = Numeric::deg2rad(Scalar(0.1)); // 0.1deg

	if (!leftControllerActive && baseSpace_T_leftAim_.isValid() && baseSpace_T_previousLeftAim.isValid())
	{
		const HomogenousMatrix4 leftAim_T_previousLeftAim = baseSpace_T_leftAim_ * baseSpace_T_previousLeftAim.inverted();

		if (leftAim_T_previousLeftAim.translation().sqr() >= translationThreshold || leftAim_T_previousLeftAim.rotation().angle() >= angleThreshold)
		{
			leftControllerActive = true;
		}
	}

	if (!rightControllerActive && baseSpace_T_rightAim_.isValid() && baseSpace_T_previousRightAim.isValid())
	{
		const HomogenousMatrix4 rightAim_T_previousRightAim = baseSpace_T_rightAim_ * baseSpace_T_previousRightAim.inverted();

		if (rightAim_T_previousRightAim.translation().sqr() >= translationThreshold || rightAim_T_previousRightAim.rotation().angle() >= angleThreshold)
		{
			rightControllerActive = true;
		}
	}

	activeHandheldControllers_.clear();

	const Timestamp timestamp(Timestamp::nanoseconds2seconds(xrPredictedDisplayTime));

	if (leftControllerActive || leftControllerActiveTimestamp_ + 0.25 >= timestamp)
	{
		activeHandheldControllers_.emplace_back(CT_LEFT);
	}

	if (rightControllerActive || rightControllerActiveTimestamp_ + 0.25 >= timestamp)
	{
		activeHandheldControllers_.emplace_back(CT_RIGHT);
	}

	if (leftControllerActive)
	{
		leftControllerActiveTimestamp_ = timestamp;
	}

	if (rightControllerActive)
	{
		rightControllerActiveTimestamp_ = timestamp;
	}

	return true;
}

TrackedController::ButtonType TrackedController::buttonsPressed(const ControllerType controllerType) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	ButtonType state = BT_NONE;

	if (controllerType & CT_LEFT)
	{
		state = ButtonType(state | (buttonsPressed_ & BT_LEFT_ALL));
	}

	if (controllerType & CT_RIGHT)
	{
		state = ButtonType(state | (buttonsPressed_ & BT_RIGHT_ALL));
	}

	return state;
}

TrackedController::ButtonType TrackedController::buttonsReleased(const ControllerType controllerType) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	ButtonType state = BT_NONE;

	if (controllerType & CT_LEFT)
	{
		state = ButtonType(state | (buttonsReleased_ & BT_LEFT_ALL));
	}

	if (controllerType & CT_RIGHT)
	{
		state = ButtonType(state | (buttonsReleased_ & BT_RIGHT_ALL));
	}

	return state;
}

TrackedController::ButtonType TrackedController::buttonsState(const ControllerType controllerType) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	ButtonType state = BT_NONE;

	if (controllerType & CT_LEFT)
	{
		state = ButtonType(state | (buttonsState_ & BT_LEFT_ALL));
	}

	if (controllerType & CT_RIGHT)
	{
		state = ButtonType(state | (buttonsState_ & BT_RIGHT_ALL));
	}

	return state;
}

Vector2 TrackedController::joystickTilt(const ControllerType controllerType) const
{
	const ScopedLock scopedLock(lock_);

	if (controllerType != CT_LEFT && controllerType != CT_RIGHT)
	{
		ocean_assert(false && "Invalid input");
		return Vector2(0, 0);
	}

	return controllerType == CT_LEFT ? joystickVectorLeft_ : joystickVectorRight_;

	return Vector2(0, 0);
}

bool TrackedController::pose(const ControllerType controllerType, HomogenousMatrix4* baseSpace_T_controllerAim, HomogenousMatrix4* baseSpace_T_controllerGrap) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid())
	{
		return false;
	}

	bool result = true;

	if (controllerType == CT_LEFT)
	{
		if (baseSpace_T_controllerAim != nullptr)
		{
			*baseSpace_T_controllerAim = baseSpace_T_leftAim_;

			result = result && baseSpace_T_leftAim_.isValid();
		}

		if (baseSpace_T_controllerGrap != nullptr)
		{
			*baseSpace_T_controllerGrap = baseSpace_T_leftGrip_;

			result = result && baseSpace_T_leftGrip_.isValid();
		}
	}
	else
	{
		ocean_assert(controllerType == CT_RIGHT);

		if (baseSpace_T_controllerAim != nullptr)
		{
			*baseSpace_T_controllerAim = baseSpace_T_rightAim_;

			result = result && baseSpace_T_rightAim_.isValid();
		}

		if (baseSpace_T_controllerGrap != nullptr)
		{
			*baseSpace_T_controllerGrap = baseSpace_T_rightGrip_;

			result = result && baseSpace_T_rightGrip_.isValid();
		}
	}

	return result;
}

bool TrackedController::setVibration(const ControllerType controllerType, const double duration, const float frequency, const float intensity)
{
	ocean_assert(duration >= 0.0);
	ocean_assert(frequency >= 0.0f);
	ocean_assert(intensity >= 0.0f && intensity <= 1.0f);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid())
	{
		return false;
	}

	for (size_t controllerIndex = 0; controllerIndex < numberControllers_; ++controllerIndex)
	{
		XrHapticVibration& xrHapticVibration = xrHapticVibrations_[controllerIndex];

		if ((controllerIndex == 0 && (controllerType & CT_LEFT) != CT_LEFT)
				|| (controllerIndex == 1 && (controllerType & CT_RIGHT) != CT_RIGHT))
		{
			xrHapticVibration = {};
			continue;
		}

		xrHapticVibration = XrHapticVibration{XR_TYPE_HAPTIC_VIBRATION};

		if (duration == 0.0)
		{
			xrHapticVibration.duration = XR_MIN_HAPTIC_DURATION;
		}
		else
		{
			xrHapticVibration.duration = XrDuration(Timestamp::seconds2nanoseconds(duration));
		}

		xrHapticVibration.frequency = frequency;
		xrHapticVibration.amplitude = intensity;
	}

	return true;
}

TrackedController::ControllerTypes TrackedController::activeHandheldControllers(const ControllerType dominantControllerType) const
{
	ocean_assert(activeHandheldControllers_.size() <= 2);
	ocean_assert(activeHandheldControllers_.size() != 2 || (activeHandheldControllers_[0] == CT_LEFT && activeHandheldControllers_[1] == CT_RIGHT));

	if (activeHandheldControllers_.size() == 2)
	{
		if (dominantControllerType == CT_RIGHT)
		{
			return {CT_RIGHT, CT_LEFT};
		}
	}

	return activeHandheldControllers_;
}

void TrackedController::release()
{
	const ScopedLock scopedLock(lock_);

	xrSession_ = XR_NULL_HANDLE;

	actionSet_ = nullptr;

	spaceLeftAim_.release();
	spaceLeftGrip_.release();

	spaceRightAim_.release();
	spaceRightGrip_.release();

	xrActionStateMap_.clear();

	buttonsState_ = BT_NONE;
	buttonsPressed_ = BT_NONE;
	buttonsReleased_ = BT_NONE;

	baseSpace_T_leftAim_.toNull();
	baseSpace_T_leftGrip_.toNull();
	baseSpace_T_rightAim_.toNull();
	baseSpace_T_rightGrip_.toNull();

	joystickVectorLeft_ = Vector2(0, 0);
	joystickVectorRight_ = Vector2(0, 0);

	xrHapticVibrations_[0] = {};
	xrHapticVibrations_[1] = {};

	activeHandheldControllers_.clear();

	leftControllerActiveTimestamp_.toInvalid();
	rightControllerActiveTimestamp_.toInvalid();
}

bool TrackedController::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrSession_ != XR_NULL_HANDLE;
}

TrackedController& TrackedController::operator=(TrackedController&& trackedController)
{
	if (this != &trackedController)
	{
		release();

		xrSession_ = trackedController.xrSession_;
		trackedController.xrSession_ = XR_NULL_HANDLE;

		actionSet_ = std::move(trackedController.actionSet_);

		spaceLeftAim_ = std::move(trackedController.spaceLeftAim_);
		spaceLeftGrip_ = std::move(trackedController.spaceLeftGrip_);
		spaceRightAim_ = std::move(trackedController.spaceRightAim_);
		spaceRightGrip_ = std::move(trackedController.spaceRightGrip_);

		xrActionStateMap_ = std::move(trackedController.xrActionStateMap_);

		buttonsState_ = trackedController.buttonsState_;
		buttonsPressed_ = trackedController.buttonsPressed_;
		buttonsReleased_ = trackedController.buttonsReleased_;
		trackedController.buttonsState_ = BT_NONE;
		trackedController.buttonsPressed_ = BT_NONE;
		trackedController.buttonsReleased_ = BT_NONE;

		baseSpace_T_leftAim_ = trackedController.baseSpace_T_leftAim_;
		baseSpace_T_leftGrip_ = trackedController.baseSpace_T_leftGrip_;
		baseSpace_T_rightAim_ = trackedController.baseSpace_T_rightAim_;
		baseSpace_T_rightGrip_ = trackedController.baseSpace_T_rightGrip_;
		trackedController.baseSpace_T_leftAim_.toNull();
		trackedController.baseSpace_T_leftGrip_.toNull();
		trackedController.baseSpace_T_rightAim_.toNull();
		trackedController.baseSpace_T_rightGrip_.toNull();

		joystickVectorLeft_ = trackedController.joystickVectorLeft_;
		joystickVectorRight_ = trackedController.joystickVectorRight_;
		trackedController.joystickVectorLeft_ = Vector2(0, 0);
		trackedController.joystickVectorRight_ = Vector2(0, 0);

		xrHapticVibrations_[0] = trackedController.xrHapticVibrations_[0];
		xrHapticVibrations_[1] = trackedController.xrHapticVibrations_[1];
		trackedController.xrHapticVibrations_[0] = {};
		trackedController.xrHapticVibrations_[1] = {};

		activeHandheldControllers_ = std::move(trackedController.activeHandheldControllers_);

		leftControllerActiveTimestamp_ = trackedController.leftControllerActiveTimestamp_;
		rightControllerActiveTimestamp_ = trackedController.rightControllerActiveTimestamp_;
		trackedController.leftControllerActiveTimestamp_.toInvalid();
		trackedController.rightControllerActiveTimestamp_.toInvalid();
	}

	return *this;
}

void TrackedController::translateButtons(const ButtonType buttons, std::vector<std::string>& translatedButtons)
{
	translatedButtons.clear();

	if (buttons & BT_LEFT_X)
	{
		translatedButtons.emplace_back("X");
	}
	if (buttons & BT_LEFT_Y)
	{
		translatedButtons.emplace_back("Y");
	}
	if (buttons & BT_LEFT_TRIGGER)
	{
		translatedButtons.emplace_back("LeftTrigger");
	}
	if (buttons & BT_LEFT_GRIP)
	{
		translatedButtons.emplace_back("LeftGrip");
	}
	if (buttons & BT_LEFT_MENU)
	{
		translatedButtons.emplace_back("LeftMenu");
	}
	if (buttons & BT_LEFT_JOYSTICK)
	{
		translatedButtons.emplace_back("LeftJoystick");
	}

	if (buttons & BT_RIGHT_A)
	{
		translatedButtons.emplace_back("A");
	}
	if (buttons & BT_RIGHT_B)
	{
		translatedButtons.emplace_back("B");
	}
	if (buttons & BT_RIGHT_TRIGGER)
	{
		translatedButtons.emplace_back("RightTrigger");
	}
	if (buttons & BT_RIGHT_GRIP)
	{
		translatedButtons.emplace_back("RightGrip");
	}
	if (buttons & BT_RIGHT_SYSTEM)
	{
		translatedButtons.emplace_back("RightSystem");
	}
	if (buttons & BT_RIGHT_JOYSTICK)
	{
		translatedButtons.emplace_back("RightJoystick");
	}
}

}

}

}

}

}
