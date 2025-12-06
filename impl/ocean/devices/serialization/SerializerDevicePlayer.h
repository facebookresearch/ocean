/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_PLAYER_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_PLAYER_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/devices/Device.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/DevicePlayer.h"

#include "ocean/io/serialization/InputDataSerializer.h"
#include "ocean/io/serialization/MediaSerializer.h"

#include "ocean/media/PixelImage.h"

#include <deque>

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

/**
 * This class implements a player for device objects based on Ocean's DataSerializer.
 * Serialized recordings containing sensor and media information can be used for replay and debugging purposes.
 * Beware: Do not run two individual instances of the SerializerDevicePlayer at the same time.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializerDevicePlayer :
	public DevicePlayer,
	protected Thread
{
	protected:

		/**
		 * This class implements a simple helper to ensure that there cannot be more than one active DevicePlayer at the same time.
		 */
		class UsageManager : public Singleton<UsageManager>
		{
			friend class Singleton<UsageManager>;

			public:

				/**
				 * Informs the manager that the player will be used.
				 * @return True, if the player can be used
				 */
				bool registerUsage();

				/**
				 * Informs the manager that the player is not longer used.
				 */
				void unregisterUsage();

			protected:

				/**
				 * Default constructor.
				 */
				UsageManager() = default;

			protected:

				/// True, if the player is currently used.
				bool isUsed_ = false;

				/// The manager's lock.
				Lock lock_;
		};

		/**
		 * Alias for DeviceRef.
		 */
		using DeviceRef = Devices::DeviceRef;

		/**
		 * Definition of a map mapping channel ids to device objects.
		 */
		using ChannelDeviceMap = std::unordered_map<IO::Serialization::DataSerializer::ChannelId, DeviceRef>;

		/**
		 * Definition of a map mapping channel ids to PixelImage objects.
		 */
		using ChannelPixelImageMap = std::unordered_map<IO::Serialization::DataSerializer::ChannelId, Media::PixelImageRef>;

		/**
		 * Definition of a function pointer for specialized sample processing functions.
		 */
		using SampleProcessorFunction = void (SerializerDevicePlayer::*)(const IO::Serialization::DataSerializer::ChannelId, IO::Serialization::UniqueDataSample&&);

		/**
		 * Definition of a map mapping sample type strings to specialized processing functions.
		 */
		using SampleTypeMap = std::unordered_map<std::string, SampleProcessorFunction>;

		/**
		 * Definition of a map mapping channel ids to specialized processing functions.
		 */
		using ChannelProcessorMap = std::unordered_map<IO::Serialization::DataSerializer::ChannelId, SampleProcessorFunction>;

		/**
		 * This class holds data associated with a frame medium channel.
		 */
		class FrameMediumData
		{
			public:

				FrameMediumData() = default;

				/**
				 * Creates a new frame medium data object.
				 * @param pixelImage The pixel image to be used, must be valid
				 */
				explicit inline FrameMediumData(const Media::PixelImageRef& pixelImage);

				/**
				 * Updates the frame medium data with a new frame sample.
				 * @param frameSample The frame sample to update from
				 * @return True, if succeeded
				 */
				bool update(const IO::Serialization::MediaSerializer::DataSampleFrame& frameSample);

			public:

				/// The pixel image object.
				Media::PixelImageRef pixelImage_;

				/// The camera profile.
				SharedAnyCamera camera_;

				/// The transformation between device and camera.
				HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);
		};

		/**
		 * Definition of a map mapping channel ids to FrameMediumData objects.
		 */
		using ChannelFrameMediumDataMap = std::unordered_map<IO::Serialization::DataSerializer::ChannelId, FrameMediumData>;

		/// Definition of a pair holding a channel id and a unique data sample.
		using SamplePair = std::pair<IO::Serialization::DataSerializer::ChannelId, IO::Serialization::UniqueDataSample>;

		/// Definition of a queue holding sample pairs.
		using SampleQueue = std::deque<SamplePair>;

	public:

		/**
		 * Creates a new device player.
		 */
		SerializerDevicePlayer() = default;

		/**
		 * Destructs the device player.
		 */
		~SerializerDevicePlayer() override;

		/**
		 * Loads a new serialized file for replay.
		 * @param filename The serialized file to be loaded
		 * @return True, if succeeded
		 */
		bool initialize(const std::string& filename) override;

		/**
		 * Starts the replay.
		 * The recording can be played with individual speed, e.g., real-time, slower than real-time, faster than real-time.<br>
		 * Further, the player supports a stop-motion mode in which the player will play one frame by another.
		 * @param speed The speed at which the recording will be played, e.g., 2 means two times faster than normal, with range (0, infinity), 0 to play the recording in a stop-motion (frame by frame) mode
		 * @return True, if succeeded
		 * @see duration(), playNextFrame();
		 */
		bool start(const float speed = 1.0f) override;

		/**
		 * Stops the replay.
		 * @return True, if succeeded
		 */
		bool stop() override;

		/**
		 * Plays the next frame of the recording, the player must be started with stop-motion mode.
		 * In case the recording holds several media objects, the first media object is used to identify the next frame.<br>
		 * This function will read all samples which have been recorded before or at the same time as the next frame of the first media object.<br>
		 * If the recording does not have any media object nothing happens.
		 * @return The timestamp of the frame which has been played, invalid if no additional frame exists
		 * @see start(), frameMediums().
		 */
		Timestamp playNextFrame() override;

		/**
		 * Returns the duration of the content when played with default speed.
		 * @return The recording's default duration, in seconds, with range [0, infinity)
		 */
		inline double duration() const override;

		/**
		 * Returns all media objects which have been created based on the recording.
		 * @return The media objects
		 */
		inline Media::FrameMediumRefs frameMediums() override;

		/**
		 * Returns a specific transformation which is expected to be part of the recording.
		 * This function is intended as a helper function to simplify access to important transformations which otherwise would be accessed through the player's tracking devices.
		 * @param name The name of the transformation, must be valid
		 * @param timestamp The timestamp for which the transformation is evaluated, must be valid
		 * @param matrix The resulting transformation matrix
		 * @return The transformation result
		 */
		TransformationResult transformation(const std::string& name, const Timestamp& timestamp, HomogenousMatrixD4& matrix) override;

		/**
		 * Returns whether this player is currently started.
		 * @return True, if so
		 */
		inline bool isStarted() const override;

		/**
		 * Sets the tolerance for stop-motion playback mode.
		 * The tolerance defines a time window beyond the current frame's timestamp within which additional samples will be processed together with the frame.
		 * This is useful when sensor samples (e.g., tracker data) have timestamps slightly newer than the corresponding video frame but should still be associated with it.
		 * The tolerance must have the same value type (double or int64) as the data timestamps in the recording.
		 * @param stopMotionTolerance The tolerance to add to each frame's timestamp, must be valid
		 * @return True, if the tolerance was set successfully
		 * @see playNextFrame().
		 */
		bool setStopMotionTolerance(const IO::Serialization::DataTimestamp& stopMotionTolerance);

		/**
		 * Returns whether this player is currently playing.
		 * @return True, if so
		 */
		bool isPlaying() const;

		/**
		 * Stops playback and releases the player.
		 */
		void release();

		/**
		 * Returns whether this player holds a valid recording.
		 * @return True, if so
		 */
		bool isValid() const override;

	protected:

		/**
		 * Initializes the device factories by registering factory functions for all supported sample types.
		 * @return True, if succeeded
		 */
		bool initializeDeviceFactories();

		/**
		 * Processes a sample and forwards it to the appropriate device.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processSample(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleFrame sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleFrame(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleOrientationTracker3DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleOrientationTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleAccelerationSensor3DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleAccelerationSensor3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleGyroSensor3DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleGyroSensor3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleGravityTracker3DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleGravityTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSamplePositionTracker3DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSamplePositionTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleTracker6DOF sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleTracker6DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes a DataSampleGPSTracker sample.
		 * @param channelId The channel id of the sample
		 * @param sample The sample to process, will be moved
		 */
		void processDataSampleGPSTracker(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Processes samples within the lookahead window.
		 * This function processes queued samples and reads new samples from the input serializer that fall within the specified playback timestamp.
		 * @param dataTimestamp The data timestamp to process samples up to
		 * @param maxPlaybackTimestamp The maximum playback timestamp defining the lookahead window
		 */
		void processLookaheadSamples(const IO::Serialization::DataTimestamp& dataTimestamp, const double maxPlaybackTimestamp);

		/**
		 * Creates or retrieves a device for a specific channel.
		 * @param channelId The channel id
		 * @param deviceName The name of the device
		 * @param deviceType The type of the device
		 * @return The device reference, empty if creation failed
		 */
		DeviceRef ensureDevice(const IO::Serialization::DataSerializer::ChannelId channelId, const std::string& deviceName, const Device::DeviceType& deviceType);

		/**
		 * The thread's run function in which the samples are read.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Factory function for creating SerializationOrientationTracker3DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationAccelerationSensor3DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationGyroSensor3DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createGyroSensor3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationGravityTracker3DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationPositionTracker3DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createPositionTracker3DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationTracker6DOF devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);

		/**
		 * Factory function for creating SerializationGPSTracker devices.
		 * @param name The name of the new device, must be valid
		 * @param deviceType The device type of the device, must be valid
		 * @return The new device, nullptr if the device could not be created
		 */
		static Device* createGPSTracker(const std::string& name, const Device::DeviceType& deviceType);

	protected:

		/// The input data serializer for reading the content.
		std::unique_ptr<IO::Serialization::FileInputDataSerializer> inputSerializer_;

		/// The id of the channel which is the first channel with media content.
		IO::Serialization::DataSerializer::ChannelId firstMediaFrameChannelId_ = IO::Serialization::DataSerializer::invalidChannelId();

		/// The map associating channel ids with device objects.
		ChannelDeviceMap channelDeviceMap_;

		/// The map associating channel ids with FrameMediumData objects.
		ChannelFrameMediumDataMap channelFrameMediumDataMap_;

		/// The FrameMedium objects receiving image content from the recording.
		Media::FrameMediumRefs frameMediums_;

		/// The map associating channel ids with specialized processing functions.
		ChannelProcessorMap channelProcessorMap_;

		/// True, if the player is started.
		std::atomic_bool isStarted_ = false;

		/// The speed at which the recording will be played, with range (0, infinity), 0 to play the recording with stop-motion.
		float speed_ = 1.0f;

		/// Map associating sample type strings with specialized processing functions.
		SampleTypeMap sampleTypeMap_;

		/// The sample queue holding pending samples for the stop-motion mode.
		SampleQueue stopMotionSampleQueue_;

		/// The tolerance for stop-motion playback defining a time window beyond the current frame's timestamp for sample processing.
		IO::Serialization::DataTimestamp stopMotionTolerance_;
};

inline SerializerDevicePlayer::FrameMediumData::FrameMediumData(const Media::PixelImageRef& pixelImage) :
	pixelImage_(pixelImage)
{
	ocean_assert(pixelImage_);
}

inline double SerializerDevicePlayer::duration() const
{
	ocean_assert(false && "This function is not supported");
	return -1.0;
}

inline Media::FrameMediumRefs SerializerDevicePlayer::frameMediums()
{
	const ScopedLock scopedLock(lock_);

	return frameMediums_;
}

inline bool SerializerDevicePlayer::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return isStarted_;
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_PLAYER_H
