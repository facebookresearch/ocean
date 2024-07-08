/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_DEVICE_PLAYER_H
#define META_OCEAN_DEVICES_DEVICE_PLAYER_H

#include "ocean/devices/Devices.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class DevicePlayer;

/**
 * Definition of a shared pointer holding a DevicePlayer object.
 * @see DevicePlayer.
 * @ingroup devices
 */
using SharedDevicePlayer = std::shared_ptr<DevicePlayer>;

/**
 * This class implements the abstract base class for all device players.
 * The device player can be used to re-play a previously captured data and exposing the data through devices.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT DevicePlayer
{
	public:

		/**
		 * Definition of a speed value for the stop-motion replay mode.
		 */
		static constexpr float SPEED_USE_STOP_MOTION = 0.0f;

		/**
		 * Definition of individual transformation results.
		 */
		enum TransformationResult : uint32_t
		{
			/// The transformation does not exist in the recording.
			TR_DOES_NOT_EXIST = 0u,
			/// The resulting transformation is valid but interpolated due to a not perfectly matching timestamp.
			TR_INTERPOLATED,
			/// The resulting transformation is valid and the timestamp matched perfectly with a transformation in the recording.
			TR_PRECISE
		};

	public:

		/**
		 * Creates a new device player.
		 */
		DevicePlayer() = default;

		/**
		 * Destructs the device player.
		 */
		virtual ~DevicePlayer();

		/**
		 * Initializes the player with a recording.
		 * @param filename The name of the file to be used in the player, must be valid
		 * @return True, if succeeded
		 */
		virtual bool initialize(const std::string& filename) = 0;

		/**
		 * Starts the replay.
		 * The recording can be payed with individual speed, e.g., real-time, slower than real-time, faster than real-time.<br>
		 * Further, the player support a stop-motion mode in which the player will play one frame by another.
		 * @param speed The speed at which the recording will be played, e.g., 2 means two times faster than normal, with range (0, infinity), 'SPEED_USE_STOP_MOTION' to play the recording in a stop-motion (frame by frame) mode
		 * @return True, if succeeded
		 * @see duration(), playNextFrame();
		 */
		virtual bool start(const float speed = 1.0f) = 0;

		/**
		 * Stops the replay.
		 * @return True, if succeeded
		 */
		virtual bool stop() = 0;

		/**
		 * Plays the next frame of the recording, the player must be started with stop-motion mode ('SPEED_USE_STOP_MOTION').
		 * In case the recording holds several media objects, the fist media objects is used to identify the next frame.<br>
		 * This function will read all records which have been recorded before or at the same time as the next frame of the first media object.<br>
		 * If the recording does not have any media object nothing happens.
		 * @return The timestamp of the frame which has been played, invalid if no additional frame exists
		 * @see start(), frameMediums().
		 */
		virtual Timestamp playNextFrame() = 0;

		/**
		 * Returns the duration of the content when played with default speed.
		 * @return The recording's default duration, in seconds, with range [0, infinity)
		 */
		virtual double duration() const = 0;

		/**
		 * Returns all frame media objects which have been created based on the recording.
		 * @return The media objects, empty if the recording does not contain any frame medium objects
		 */
		virtual Media::FrameMediumRefs frameMediums();

		/**
		 * Returns a specific transformation which is expected to be part of the recording.
		 * This function is intended as a helper function to simplify access to important transformations which otherwise would be accessed through the player's tracking devices.
		 * @param name The name of the transformation, must be valid
		 * @param timestamp The timestamp for which the transformation is evaluated, must be valid
		 * @param matrix The resulting transformation matrix
		 * @return The transformation result
		 */
		virtual TransformationResult transformation(const std::string& name, const Timestamp& timestamp, HomogenousMatrixD4& matrix) = 0;

		/**
		 * Returns whether this player is currently started.
		 * @return True, if so
		 */
		virtual bool isStarted() const = 0;

		/**
		 * Returns whether this player holds a valid recording.
		 * @return True, if so
		 */
		virtual bool isValid() const;

	protected:

		/// The filename of the recording which is used.
		std::string filename_;

		/// The player's lock.
		mutable Lock lock_;
};

} // namespace Devices

} // namespace Ocean

#endif // META_OCEAN_DEVICES_DEVICE_PLAYER_H
