// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_TRACKED_REMOTE_DEVICE_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_TRACKED_REMOTE_DEVICE_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/meta/quest/Device.h"

#include <VrApi_Input.h>

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

/**
 * This class implements a wrapper for VrApi tracked remote devices.
 * @ingroup platformmetaquestvrapi
 */
class TrackedRemoteDevice
{
	public:

		/**
		 * Definition of individual remote types.
		 */
		enum RemoteType : uint32_t
		{
			/// An undefined remote type.
			RT_UNDEFINED = 0u,
			/// A left remote.
			RT_LEFT = 1u << 0u,
			/// A right remote.
			RT_RIGHT = 1u << 1u,
			/// A left remote or a right remote.
			RT_LEFT_OR_RIGHT = RT_LEFT | RT_RIGHT
		};

		/**
		 * Definition of a vector holding remote types.
		 */
		typedef std::vector<RemoteType> RemoteTypes;

	protected:

		/// An invalid device id.
		static constexpr ovrDeviceID invalidDeviceId = 0u;

	public:

		/**
		 * Default constructor.
		 */
		TrackedRemoteDevice() = default;

		/**
		 * Copy constructor.
		 * @param trackedRemoteDevice The object to be copied
		 */
		inline TrackedRemoteDevice(const TrackedRemoteDevice& trackedRemoteDevice);

		/**
		 * Creates a new object with given OVR SDK object.
		 * @param ovrMobile The OVR SDK object, must be valid
		 * @param deviceType The device type that this object will be configured for, must not be `Device::DT_UNKNOWN`
		 */
		TrackedRemoteDevice(ovrMobile* ovrMobile, const Device::DeviceType deviceType);

		/**
		 * Updates the states of this object, must be called once for each new frame.
		 * @param timestamp The update timestamp, invalid to use the most recent tracking results
		 */
		void update(const Timestamp& timestamp);

		/**
		 * Returns all buttons which have been pressed recently (in the last frame).
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param remoteType The type of the remote for which the state will be returned
		 * @return The buttons recently pressed
		 * @see update().
		 */
		uint32_t buttonsPressed(const RemoteType remoteType = RT_LEFT_OR_RIGHT) const;

		/**
		 * Returns all buttons which have been released recently (in the last frame).
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param remoteType The type of the remote for which the state will be returned
		 * @return The buttons recently released
		 * @see update().
		 */
		uint32_t buttonsReleased(const RemoteType remoteType = RT_LEFT_OR_RIGHT) const;

		/**
		 * Returns all buttons which are currently pressed.
		 * Beware: Call update() once for each frame, before calling this function.
		 * @param remoteType The type of the remote for which the state will be returned
		 * @return The buttons crrently pressed
		 * @see update().
		 */
		uint32_t buttonsState(const RemoteType remoteType = RT_LEFT_OR_RIGHT) const;

		/**
		 * Returns the tilt of the joystick buttons
		 * The tilt values range from -1.0 (left/up) to +1.0 (right/down). The value is set to 0.0, if the magnitude of the vector is < 0.1.
		 * @param remoteType The type of the remote for which the joystick tilt will be returned, must be RT_LEFT or RT_RIGHT
		 * @return The tilt vector of the select joystick button, with range [-1, -1] x [1, 1]
		 */
		Vector2 joystickTilt(const RemoteType remoteType) const;

		/**
		 * Query the pose of a tracked remote device (controller)
		 * @param remoteType Indicates whether the pose of the left or the right controller should be queried, must be valid
		 * @param world_T_remoteDevice The optional resulting transformation that converts controller coordinates to world coordinates, valid only if the function returns true
		 * @param world_T_remoteDeviceRing The optional resulting transformation that converts coordinates defined at the ring center of a controller to world coordinates, valid only if the function returns true
		 * @param timestamp Optional parameter specifying the time at which the pose should be queried; invalid to use the timestamp of the last update call, 0 to use the most recent pose
		 * @return True, if pose transformation(s) could be determined
		 * @see getController_T_ringCenter().
		 */
		bool pose(const RemoteType remoteType, HomogenousMatrix4* world_T_remoteDevice = nullptr, HomogenousMatrix4* world_T_remoteDeviceRing = nullptr, const Timestamp& timestamp = Timestamp(false)) const;

		/**
		 * Set the haptic vibration intensity level for the next frame.
		 * @param remoteType The remote type for which the vibration level will be set, can be RT_LEFT, or RT_RIGHT, or RT_LEFT_OR_RIGHT
		 * @param intensity The vibration intensity level, with range [0, 1]
		 */
		void setVibrationLevel(const RemoteType remoteType, float intensity);

		/**
		 * Returns the remote types of all devices currently active and handheld (e.g., with tiny movements, button or joystick changes).
		 * If a dominant remote type is defined, the dominante remote type will be returned first followed by the other type if active.
		 * @param dominantRemoteType The dominant remote type, RT_RIGHT to treat left and right equal, RT_LEFT_OR_RIGHT to return types with an undefined order
		 * @return The types of devices currently active and handheld
		 */
		RemoteTypes activeHandheldDevices(const RemoteType dominantRemoteType = RT_RIGHT) const;

		/**
		 * Releases this tracked remote device object.
		 * The entire object will be set to an invalid state.
		 */
		void release();

		/**
		 * Returns whether this tracked remote device object has been initialized and can be used.
		 * @return True, if valid and ready to use; False, if the `ovrMobile` object has not yet been provided
		 */
		bool isValid() const;

		/**
		 * Assign operator.
		 * @param trackedRemoteDevice The object to be assign
		 * @return Reference to this object
		 */
		TrackedRemoteDevice& operator=(const TrackedRemoteDevice& trackedRemoteDevice);

		/**
		 * Returns the transformation from the ring center to the regular controller coordinate system
		 * The coordinate system at the ring center is defined as: x
		 *  x - points right and is in the plane defined by the front-facing part of the ring
		 *  y - points up and is in the plane defined by the front-facing part of the ring
		 *  z - is perpendicular to the plane defined by the front-facing part of the ring and points back (where the handles are)
		 * @param deviceType Indicates the model for which to return the pose, must be `Device::DT_QUEST` or `Device::DT_QUEST_2`
		 * @param remoteType Indicates whether it should be the left or the right controller, must be `RT_LEFT` or `RT_RIGHT`
		 * @param controller_T_ringCenter The resulting transformation that maps the coordinate system at the ring center to the coordinate system of the specified controller, will only be valid if this function returns `true`
		 * @return True, if the transformation could be retrieved
		 */
		static bool getController_T_ringCenter(const Device::DeviceType deviceType, const RemoteType remoteType, HomogenousMatrix4& controller_T_ringCenter);

		/**
		 * Translates the left and right button states to readable strings.
		 * @param buttonsLeft The left button states
		 * @param buttonsRight The right button states
		 * @param translatedButtons The resulting translated readable strings of the individual buttons
		 */
		static void translateButtons(const uint32_t buttonsLeft, const uint32_t buttonsRight, std::vector<std::string>& translatedButtons);

	protected:

		/**
		 * Returns all button states of a specific device.
		 * @param deviceId The id of the device for which the state will be returned
		 * @param buttons The current button states for the current frame (for the current update() call)
		 * @param joystickTilt The current tilt vector of joystick button for the current frame (for the current update() call)
		 * @return True on success
		 */
		bool buttonsState(const ovrDeviceID deviceId, uint32_t& buttons, Vector2& joystickTilt);

		/**
		 * Acquires a specific device id based on a specified device capability.
		 * @param ovrMobile The OVR SDK object, must be valid
		 * @param controllerCapability The controller capability for which the device will be acquired, must be a value from `ovrControllerCapabilties`
		 * @return The resulting device id if the device could be found, otherwise `invalidDeviceId`
		 */
		static ovrDeviceID acquireDevice(ovrMobile* ovrMobile, const uint32_t controllerCapability);

	protected:

		/// The OVR SDK object.
		ovrMobile* ovrMobile_ = nullptr;

		/// The device type that this object is configured for.
		Device::DeviceType deviceType_ = Device::DT_UNKNOWN;

		/// The timestamp of the last update call.
		Timestamp updateTimestamp_ = Timestamp(false);

		/// The id of the left remote device.
		ovrDeviceID deviceIdLeft_ = invalidDeviceId;

		/// The id of the right remote device.
		ovrDeviceID deviceIdRight_ = invalidDeviceId;

		/// The previous left button states.
		uint32_t previousButtonStateLeft_ = 0u;

		/// The previous right button states.
		uint32_t previousButtonStateRight_ = 0u;

		/// The current left button states.
		uint32_t currentButtonStateLeft_ = 0u;

		/// The current right button states.
		uint32_t currentButtonStateRight_ = 0u;

		/// The transformation between the left device and world at the last update call, invalid if unknown.
		HomogenousMatrix4 world_T_leftRemoteDevice_ = HomogenousMatrix4(false);

		/// The transformation between the right device and world at the last update call, invalid if unknown.
		HomogenousMatrix4 world_T_rightRemoteDevice_ = HomogenousMatrix4(false);

		/// The transformation from the ring center of the left controller to the origin of the controller
		HomogenousMatrix4 leftRemoteDevice_T_ringCenter_ = HomogenousMatrix4(false);

		/// The transformation from the ring center of the right controller to the origin of the controller
		HomogenousMatrix4 rightRemoteDevice_T_ringCenter_ = HomogenousMatrix4(false);

		/// The timestamp when the left remote device was active the last time.
		Timestamp leftRemoteDeviceActiveTimestamp_ = Timestamp(false);

		/// The timestamp when the right remote device was active the last time.
		Timestamp rightRemoteDeviceActiveTimestamp_ = Timestamp(false);

		/// The types of devices currently active and handheld.
		RemoteTypes activeHandheldDevices_;

		/// Tilt vector of the joystick button on the left controller
		Vector2 joystickTiltLeft_ = Vector2(0, 0);

		/// Tilt vector of the joystick button on the right controller
		Vector2 joystickTiltRight_ = Vector2(0, 0);

		/// The vibration levels which will be applied during the next update call, for left and right remote type.
		float nextVibrationLevel_[2] = {0.0f, 0.0f};

		/// The lock object.
		mutable Lock lock_;
};

inline TrackedRemoteDevice::TrackedRemoteDevice(const TrackedRemoteDevice& trackedRemoteDevice) :
	TrackedRemoteDevice()
{
	*this = trackedRemoteDevice;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_TRACKED_REMOTE_DEVICE_H
