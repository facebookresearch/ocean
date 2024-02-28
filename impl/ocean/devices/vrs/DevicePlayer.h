// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_DEVICE_PLAYER_H
#define META_OCEAN_DEVICES_VRS_DEVICE_PLAYER_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSGPSTracker.h"
#include "ocean/devices/vrs/VRSTracker6DOF.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/media/PixelImage.h"

namespace vrs
{
	/// Forward declaration.
	class RecordFileReader;

	/// Forward declaration.
	class StreamPlayer;

	/// Forward declaration.
	class StreamId;
}

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * This class implements a VRS player for devices (and media objects).
 * VRS recordings containing sensor and media informations can be used for replay and debugging purposes.
 * Beware: Do not run two individual instances of the DevicePlayer at the same time.
 * @ingroup deviceios
 */
class OCEAN_DEVICES_VRS_EXPORT DevicePlayer : protected Thread
{
	protected:

		/// Forward declaration.
		class PlayableImage;

		/// Forward declaration.
		class PlayableTracker6DOFSample;

		/// Forward declaration.
		class PlayableGPSTrackerSample;

		/// Forward declaration.
		class PlayableSceneTracker6DOFSample;

		/// Forward declaration.
		class PlayableHomogenousMatrix4;

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

	public:

		/**
		 * Creates a new device player.
		 */
		DevicePlayer() = default;

		/**
		 * Destructs the device player.
		 */
		~DevicePlayer() override;

		/**
		 * Loads a new VRS file for replay.
		 * @param filename The VRS file to be loaded
		 * @return True, if succeeded
		 */
		bool loadRecording(const std::string& filename);

		/**
		 * Starts the replay.
		 * The recording can be payed with with individual speed, e.g., real-time, slower than real-time, faster than real-time.<br>
		 * Further, the player support a stop-motion mode in which the player will play one frame by another.
		 * @param speed The speed at which the VRS recording will be played, e.g., 2 means two times faster than normal, with range (0, infinity), 0 to play the recording in a stop-motion (frame by frame) mode
		 * @return True, if succeeded
		 * @see duration(), playNextFrame();
		 */
		bool start(const float speed = 1.0f);

		/**
		 * Stops the replay.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Playes the next frame of the recording, the player must be started with stop-motion mode.
		 * In case the recording holds several media objects, the fist media objects is used to identify the next frame.<br>
		 * This function will read all records which have been recorded before or at the same time as the next frame of the first media object.<br>
		 * If the recording does not have any media object nothing happens.
		 * @return The timestamp of the frame which has been played, invalid if no additional frame exists
		 * @see start(), frameMediums().
		 */
		Timestamp playNextFrame();

		/**
		 * Returns the duration of the VRS content when played with default speed.
		 * @return The recording's default duration, in seconds, with range [0, infinity)
		 */
		inline double duration() const;

		/**
		 * Returns all media objects which have been created based on the VRS recording.
		 * @return The media object
		 */
		inline Media::FrameMediumRefs frameMediums();

		/**
		 * Returns whether this player is currently started.
		 * @return True, if so
		 */
		inline bool isStarted() const;

		/**
		 * Returns whether this player holds a valid recording.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/**
		 * Analyzes all streams of the recording and setting up the necessary device and media objects.
		 * @return True, if succeeded
		 */
		bool analyzeRecording();

		/**
		 * The thread's run function in which the VRS records are read.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// The filename of the VRS file which is used.
		std::string filename_;

		/// The actual reader for the VRS content.
		std::shared_ptr<vrs::RecordFileReader> recordFileReader_;

		/// The playables used to extract the VRS content.
		std::vector<std::shared_ptr<vrs::StreamPlayer>> playables_;

		/// The stream id of the first PlayableImage object.
		std::shared_ptr<vrs::StreamId> firstPlayableImageStreamId_;

		/// The FrameMedium objects receiving image content from the VRS recording.
		Media::FrameMediumRefs frameMediums_;

		/// True, if the player is started.
		bool isStarted_ = false;

		/// The speed at which the recording will be played, with range (0, infinity), 0 to play the recording with stop-motion.
		float speed_ = 1.0f;

		/// The index of the next record in case the player is in stop motion mode.
		size_t nextStopMotionRecordIndex_ = 0;

		/// The duration of the VRS content when played with default speed, in seconds, with range [0, infinity)
		double duration_ = 0.0;

		/// Optional stream id of the only stream from which the timestamps will be used for replay (e.g., because other streams contain invalid timestamps).
		std::shared_ptr<vrs::StreamId> replayTimestampProviderStreamId_;

		/// The player's lock.
		mutable Lock lock_;
};

inline double DevicePlayer::duration() const
{
	const ScopedLock scopedLock(lock_);

	return duration_;
}

inline Media::FrameMediumRefs DevicePlayer::frameMediums()
{
	const ScopedLock scopedLock(lock_);

	return frameMediums_;
}

inline bool DevicePlayer::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return isStarted_;
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_DEVICE_RECORDER_H
