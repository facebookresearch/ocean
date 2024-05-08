/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_MEDIUM_H
#define META_OCEAN_MEDIA_AVF_MEDIUM_H

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/base/Singleton.h"

#include "ocean/media/Medium.h"

#include <AVFoundation/AVFoundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This is the base class for all AVFoundation mediums.
 * Beware: When using AV foundation objects within a console application on macOS, the main loop needs to be executed regularly.<br>
 * The main loop is handling events neccessary in AVFoundation, however a console application commonly is a blocking application and therefore never invokes the main loop.
 * @see Platform::Apple::System::runMainLoop().
 * @ingroup mediaavf
 */
class AVFMedium : virtual public Medium
{
	protected:

		/**
		 * This class implements a manager for audio sessions.
		 */
		class AudioSessionManager : public Singleton<AudioSessionManager>
		{
			friend class Singleton<AudioSessionManager>;

			public:

#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS

				/**
				 * Initializes the audio session.
				 * @param avAudioSessionCategory The preferred category to be used
				 * @param avAudioSessionMode The preferred mode to be used
				 */
				void initialize(const AVAudioSessionCategory avAudioSessionCategory, const AVAudioSessionMode avAudioSessionMode);

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS

				/**
				 * Starts an audio session.
				 * Each successful call of start() needs to be balanced with a call of stop().
				 * @return True, if the session was started successfully; False, if the session could not be started
				 * @see stop().
				 */
				bool start();

				/**
				 * Stops an audio session after is has been started.
				 * @see start().
				 */
				void stop();

				/**
				 * Requests record permission.
				 * The session must have been started already.
				 */
				void requestRecordPermission();

			protected:

				/**
				 * Default constructor.
				 */
				AudioSessionManager() = default;

			protected:

#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS

				/// The usage counter.
				unsigned int usageCounter_ = 0u;

				/// The audio session for iOS platforms.
				AVAudioSession* avAudioSession_ = nullptr;

				/// The category which is used.
				AVAudioSessionCategory avAudioSessionCategory_ = AVAudioSessionCategoryAmbient;

				/// The mode which is used.
				AVAudioSessionMode avAudioSessionMode_ = AVAudioSessionModeDefault;

				/// The manager's lock.
				Lock lock_;

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS
		};

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted()
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @return Start timestamp
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @return Pause timestamp
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @return Stop timestamp
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit AVFMedium(const std::string& url);

		/**
		 * Destructs a AVFMedium object.
		 */
		~AVFMedium() override;

		/**
		 * Starts the medium.
		 * The internalStart() function will be called inside.
		 * @see Medium::start()
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * The internalPause() function will be called inside.
		 * @see Medium::pause()
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * The internalStop() function will be called inside.
		 * @see Medium::stop()
		 */
		bool stop() override;

		/**
		 * Internally starts the medium.
		 * @return True, if succeeded or if the medium is already started
		 */
		virtual bool internalStart() = 0;

		/**
		 * Internally pauses the medium.
		 * @return True, if succeeded or if the medium is already paused
		 */
		virtual bool internalPause() = 0;

		/**
		 * Internally stops the medium.
		 * @return True, if succeeded or if the medium is already stopped
		 */
		virtual bool internalStop() = 0;

	protected:

		/// Start timestamp
		Timestamp startTimestamp_;

		/// Pause timestamp
		Timestamp pauseTimestamp_;

		/// Stop timestamp
		Timestamp stopTimestamp_;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_MEDIUM_H
