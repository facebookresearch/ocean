/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_TRACKED_CONTROLLER_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_TRACKED_CONTROLLER_H

#include "ocean/platform/meta/quest/openxr/OpenXR.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/openxr/ActionSet.h"
#include "ocean/platform/openxr/Session.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

/**
 * This class implements a wrapper for tracker controllers.
 * @ingroup platformmetaquestopenxr
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT TrackedController
{
	public:

		/**
		 * Definition of individual button types, may be a combintation of several buttons.
		 */
		enum ButtonType : uint32_t
		{
			/// No button.
			BT_NONE = 0u,

			/// The left X button.
			BT_LEFT_X = 1u << 0u,
			/// The left Y button.
			BT_LEFT_Y = 1u << 1u,
			/// The left trigger button.
			BT_LEFT_TRIGGER = 1u << 2u,
			/// The left grip button.
			BT_LEFT_GRIP = 1u << 3u,
			/// The left menu button.
			BT_LEFT_MENU = 1u << 4u,
			/// The left joystick button.
			BT_LEFT_JOYSTICK = 1u << 5u,

			/// The right A button.
			BT_RIGHT_A = 1u << 6u,
			/// The right B button.
			BT_RIGHT_B = 1u << 7u,
			/// The right trigger button.
			BT_RIGHT_TRIGGER = 1u << 8u,
			/// The right grip button.
			BT_RIGHT_GRIP = 1u << 9u,
			/// The right system button (aka Oculus button).
			BT_RIGHT_SYSTEM = 1u << 10u,
			/// The right joystick button.
			BT_RIGHT_JOYSTICK = 1u << 11u,

			/// All left buttons.
			BT_LEFT_ALL = BT_LEFT_X | BT_LEFT_Y | BT_LEFT_TRIGGER | BT_LEFT_GRIP | BT_LEFT_MENU | BT_LEFT_JOYSTICK,

			/// All right buttons.
			BT_RIGHT_ALL = BT_RIGHT_A | BT_RIGHT_B | BT_RIGHT_TRIGGER | BT_RIGHT_GRIP | BT_RIGHT_SYSTEM | BT_RIGHT_JOYSTICK,

			/// The end button bit, must not be used
			BT_END = 1u << 12u,
		};

		/**
		 * Definition of individual controller types.
		 */
		enum ControllerType : uint32_t
		{
			/// An undefined controller type.
			CT_UNDEFINED = 0u,
			/// A left controller.
			CT_LEFT = 1u << 0u,
			/// A right controller.
			CT_RIGHT = 1u << 1u,
			/// A left controller or a right controller.
			CT_LEFT_OR_RIGHT = CT_LEFT | CT_RIGHT
		};

		/**
		 * Definition of a vector holding controller types.
		 */
		typedef std::vector<ControllerType> ControllerTypes;

	protected:

		/**
		 * Definition of internal action types e.g., for vector or pose states.
		 */
		enum ActionType : uint32_t
		{
			/// The left aim pose.
			AT_LEFT_AIM = BT_END,
			/// The left grip pose.
			AT_LEFT_GRIP,
			/// The right aim pose.
			AT_RIGHT_AIM,
			/// The right grip pose.
			AT_RIGHT_GRIP,

			/// The left joystick vector state.
			AT_LEFT_JOYSTICK_VECTOR,
			/// The right joystick vector state.
			AT_RIGHT_JOYSTICK_VECTOR,

			/// The left vibration output.
			AT_LEFT_VIBRATION,
			/// The right vibration output.
			AT_RIGHT_VIBRATION
		};

		/**
		 * Definition of an unordered map mapping button types to OpenXR boolean states.
		 */
		typedef std::unordered_map<ButtonType, XrActionStateBoolean> XrActionStateMap;

		/// The number of controllers.
		static constexpr size_t numberControllers_ = 2;

	public:

		/**
		 * Default constructor.
		 */
		TrackedController() = default;

		/**
		 * Destructs this tracked controller object and releases all associated resources.
		 */
		~TrackedController();

		/**
		 * Move constructor.
		 * @param trackedController The object to be moved
		 */
		inline TrackedController(TrackedController&& trackedController);

		/**
		 * Initializes this controller object.
		 * @param session The OpenXR session to be used, must be valid
		 * @return True, if succeeded
		 */
		inline bool initialize(const Platform::OpenXR::Session& session);

		/**
		 * Initializes this controller object.
		 * @param xrInstance The OpenXR instance to be used, must be valid
		 * @param xrSession The OpenXR session to be used, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(const XrInstance& xrInstance, const XrSession& xrSession);

		/**
		 * Updates the states of this object, must be called once for each new frame.
		 * @param baseSpace The base space in which the controller will be located, must be valid
		 * @param xrPredictedDisplayTime The predicted display time
		 * @return True, if succeeded
		 */
		bool update(const XrSpace& baseSpace, const XrTime& xrPredictedDisplayTime);

		/**
		 * Returns all buttons which have been pressed recently (in the last frame).
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param controllerType The type of the controller for which the state will be returned
		 * @return The buttons recently pressed
		 * @see update().
		 */
		ButtonType buttonsPressed(const ControllerType controllerType = CT_LEFT_OR_RIGHT) const;

		/**
		 * Returns all buttons which have been released recently (in the last frame).
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param controllerType The type of the controller for which the state will be returned
		 * @return The buttons recently released
		 * @see update().
		 */
		ButtonType buttonsReleased(const ControllerType controllerType = CT_LEFT_OR_RIGHT) const;

		/**
		 * Returns all buttons which are currently pressed.
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param controllerType The type of the controller for which the state will be returned
		 * @return The buttons crrently pressed
		 * @see update().
		 */
		ButtonType buttonsState(const ControllerType controllerType = CT_LEFT_OR_RIGHT) const;

		/**
		 * Returns the tilt of the joystick buttons
		 * The tilt values range from -1.0 (left/up) to +1.0 (right/down). The value is set to 0.0, if the magnitude of the vector is < 0.1.
		 * @param controllerType The type of the controller for which the state will be returned, must be CT_LEFT or CT_RIGHT
		 * @return The tilt vector of the select joystick button, with range [-1, -1] x [1, 1]
		 */
		Vector2 joystickTilt(const ControllerType controllerType) const;

		/**
		 * Queries the pose of a controller.
		 * @param controllerType The type of the controller for which the pose will be returned, must be CT_LEFT or CT_RIGHT
		 * @param baseSpace_T_controllerAim Optional resulting transformation between the controller's aim pose and the base space, nullptr if not of interest
		 * @param baseSpace_T_controllerGrap Optional resulting transformation between the controller's grap pose and the base space, nullptr if not of interest
		 * @return True, if pose transformation(s) could be determined
		 * @see update().
		 */
		bool pose(const ControllerType controllerType, HomogenousMatrix4* baseSpace_T_controllerAim = nullptr, HomogenousMatrix4* baseSpace_T_controllerGrap = nullptr) const;

		/**
		 * Set the haptic vibration for the next frame.
		 * @param controllerType The controller type for which the vibration level will be set, can be CT_LEFT, or CT_RIGHT, or CT_LEFT_OR_RIGHT
		 * @param duration The duration of the vibration, in seconds, with range (0, infinity), 0 to use a short haptics pulse of minimal supported duration
		 * @param frequency The frequency in Hz, with range (0, infinity), 0 to use the default frequency
		 * @param intensity The vibration intensity, with range [0, 1]
		 * @return True, if succeeded
		 */
		bool setVibration(const ControllerType controllerType, const double duration, const float frequency, const float intensity);

		/**
		 * Returns the controller types of all controllers currently active and handheld (e.g., with tiny movements, button or joystick changes).
		 * If a dominant controller type is defined, the dominante controller type will be returned first followed by the other type if active.
		 * @param dominantControllerType The dominant controller type, RCT_RIGHT to treat left and right equal, CT_LEFT_OR_RIGHT to return types with an undefined order
		 * @return The types of devices currently active and handheld
		 */
		ControllerTypes activeHandheldControllers(const ControllerType dominantControllerType = CT_RIGHT) const;

		/**
		 * The action set used to determine the tracked controller states.
		 * @return The action set
		 */
		inline Platform::OpenXR::SharedActionSet actionSet() const;

		/**
		 * Releases this tracked controller object.
		 * The entire object will be set to an invalid state.
		 */
		void release();

		/**
		 * Returns whether this tracked controller object has been initialized and can be used.
		 * @return True, if valid and ready to use
		 */
		bool isValid() const;

		/**
		 * Move operator.
		 * @param trackedController The object to be moved
		 * @return Reference to this object
		 */
		TrackedController& operator=(TrackedController&& trackedController);

		/**
		 * Translates button states to readable strings.
		 * @param buttons The button states
		 * @param translatedButtons The resulting translated readable strings of the individual buttons
		 */
		static void translateButtons(const ButtonType buttons, std::vector<std::string>& translatedButtons);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		TrackedController(const TrackedController&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		TrackedController& operator=(const TrackedController&) = delete;

	protected:

		/// The OpenXR session.
		XrSession xrSession_ = XR_NULL_HANDLE;

		/// The OpenXR action set used to determine the controller states.
		Platform::OpenXR::SharedActionSet actionSet_;

		/// The OpenXR space of the left aim pose.
		Platform::OpenXR::ScopedXrSpace spaceLeftAim_;

		/// The OpenXR space of the left grip pose.
		Platform::OpenXR::ScopedXrSpace spaceLeftGrip_;

		/// The OpenXR space of the right aim pose.
		Platform::OpenXR::ScopedXrSpace spaceRightAim_;

		/// The OpenXR space of the right grip pose.
		Platform::OpenXR::ScopedXrSpace spaceRightGrip_;

		/// The map mapping button types to boolean states.
		XrActionStateMap xrActionStateMap_;

		/// The current button states.
		ButtonType buttonsState_ = BT_NONE;

		/// The buttons pressed since the last update.
		ButtonType buttonsPressed_ = BT_NONE;

		/// The buttons released since the last update.
		ButtonType buttonsReleased_ = BT_NONE;

		/// The transformation between left aim pose and the base space, invalid if unknown.
		HomogenousMatrix4 baseSpace_T_leftAim_ = HomogenousMatrix4(false);

		/// The transformation between left grip pose and the base space, invalid if unknown.
		HomogenousMatrix4 baseSpace_T_leftGrip_ = HomogenousMatrix4(false);

		/// The transformation between right aim pose and the base space, invalid if unknown.
		HomogenousMatrix4 baseSpace_T_rightAim_ = HomogenousMatrix4(false);

		/// The transformation between right grip pose and the base space, invalid if unknown.
		HomogenousMatrix4 baseSpace_T_rightGrip_ = HomogenousMatrix4(false);

		/// Tilt vector of the joystick button on the left controller.
		Vector2 joystickVectorLeft_ = Vector2(0, 0);

		/// Tilt vector of the joystick button on the right controller.
		Vector2 joystickVectorRight_ = Vector2(0, 0);

		/// The haptic vibration objects for the left and right controller.
		XrHapticVibration xrHapticVibrations_[numberControllers_] = {};

		/// The types of controllers currently active and handheld.
		ControllerTypes activeHandheldControllers_;

		/// The timestamp when the left controller was active the last time.
		Timestamp leftControllerActiveTimestamp_;

		/// The timestamp when the right controller was active the last time.
		Timestamp rightControllerActiveTimestamp_;

		/// The lock object.
		mutable Lock lock_;
};

inline TrackedController::TrackedController(TrackedController&& trackedController)
{
	*this = std::move(trackedController);
}

bool TrackedController::initialize(const Platform::OpenXR::Session& session)
{
	ocean_assert(session.isValid());

	return initialize(session.xrInstance(), session);
}

inline Platform::OpenXR::SharedActionSet TrackedController::actionSet() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return actionSet_;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_TRACKED_CONTROLLER_H
