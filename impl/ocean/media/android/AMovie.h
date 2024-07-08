/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_MOVIE_H
#define META_OCEAN_MEDIA_ANDROID_A_MOVIE_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/AMedium.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/base/ScopedObject.h"
#include "ocean/base/Thread.h"

#include "ocean/media/Movie.h"

#include <media/NdkMediaExtractor.h>

#include <atomic>

#include <unistd.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements a movie recorder class for Android.
 * NOTE: The supported pixel formats for recording may vary from device to device.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT AMovie final :
	virtual public AMedium,
	virtual public Movie,
	protected Thread
{
	friend class ALibrary;

	protected:

		/**
		 * Definition of a scoped file based on a file descriptor.
		 */
		using ScopedFileDescriptor = ScopedObjectCompileTimeT<int, int, int, close, 0, true, -1>;

	public:

		/**
		 * Clones this movie medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the duration of the finite medium.
		 * @see FiniteMedium::duration().
		 */
		double duration() const override;

		/**
		 * Returns the duration without speed consideration.
		 * @see FiniteMedium::normalDuration().
		 */
		double normalDuration() const override;

		/**
		 * Returns the recent position of the finite medium.
		 * @see FiniteMedium::position().
		 */
		double position() const override;

		/**
		 * Sets the recent position of the finite medium.
		 * @see FiniteMedium::setPosition().
		 */
		bool setPosition(const double position) override;

		/**
		 * Returns the speed of the finite medium.
		 * @see FiniteMedium::speed().
		 */
		float speed() const override;

		/**
		 * Sets the speed of the finite medium.
		 * @see FiniteMedium::setSpeed().
		 */
		bool setSpeed(const float speed) override;

		/**
		 * Returns the volume of the sound in db.
		 * @see SoundMedium::soundVolume().
		 */
		float soundVolume() const override;

		/**
		 * Returns whether the sound medium is in a mute state.
		 * @see SoundMedium::soundMute().
		 */
		bool soundMute() const override;

		/**
		 * Sets the volume of the sound in db.
		 * @see SoundMedium::setSoundVolume().
		 */
		bool setSoundVolume(const float volume) override;

		/**
		 * Sets or un-sets the sound medium to a mute state.
		 * @see SoundMedium::setSoundMute().
		 */
		bool setSoundMute(const bool mute) override;

		/**
		 * Returns whether the sound of this movie is enabled or disabled.
		 * @see Movie::useSound().
		 */
		bool useSound() const override;

		/**
		 * Enables or disables the audio in this movie (has no effect if the movie does not have audio).
		 * @see Movie::setUseSound()
		 */
		bool setUseSound(const bool state) override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new movie object.
		 * @param url The URL of the movie, must be valid
		 */
		explicit AMovie(const std::string& url);

		/**
		 * Destroys a movie object.
		 */
		~AMovie() override;

		/**
		 * Releases the movie and all associated resources.
		 */
		void release();

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Creates and initializes the media extractor for the medium's url.
		 * @return True, if succeeded
		 */
		bool initializeMediaExtractor();

		/**
		 * Creates and initializes the media codecs for the medium.
		 * @return True, if at least the video codec could be initialized successfully
		 */
		bool initializeMediaCodecs();

		/**
		 * Creates the media codec for a specific track.
		 * @param trackFormat The format of the track for which the code will be created, must be valid
		 * @param mime The mime of the format, must be valid
		 * @param trackIndex The index of the track for which the codec will be created, must be valid
		 * @return The media codec if succeeded; nullptr otherwise
		 */
		AMediaCodec* createCodecForTrack(AMediaFormat* trackFormat, const char* mime, const size_t trackIndex);

		/**
		 * Processes the input buffer of a media codec.
		 * @param mediaCodec The media codec which input buffer will be processed, must be valid
		 * @param normalRelativePresentationTime The resulting relative presentation time in seconds for normal speed
		 * @return False, if no more buffers need to be processed
		 */
		bool processInputBuffer(AMediaCodec* const mediaCodec, double& normalRelativePresentationTime);

		/**
		 * Extracts the audio samples from an output buffer of a audio codec.
		 * @param mediaCodec The media codec to which the output buffer belongs, must be valid
		 * @return The resulting extracted frame, invalid if the frame could not be extracted
		 */
		static bool extractAudioSamplesFromCodecOutputBuffer(AMediaCodec* const mediaCodec);

	protected:

		/// Optional file description if used.
		ScopedFileDescriptor fileDescriptor_;

		/// The media extractor to be used t
		AMediaExtractor* mediaExtractor_ = nullptr;

		/// The media codecs for the video track.
		AMediaCodec* videoMediaCodec_ = nullptr;

		/// The media codecs for the audio track.
		AMediaCodec* audioMediaCodec_ = nullptr;

		/// The timestamp when the medium started.
		Timestamp startTimestamp_ = Timestamp(false);

		/// The timestamp when the medium paused.
		Timestamp pauseTimestamp_ = Timestamp(false);

		/// The timestamp when the medium stopped.
		Timestamp stopTimestamp_ = Timestamp(false);

		/// The playback speed of this movie.
		std::atomic<float> speed_ = 1.0f;

		/// The duration of the movie with normal speed.
		double normalDuration_ = -1.0;

		/// True, to enable audio in the movie; False, to disable audio, currently AMovie does not support audio.
		bool useSound_ = false;
};

}

}

}

#endif // __ANDROID_API__

#endif // META_OCEAN_MEDIA_ANDROID_A_MOVIE_H
