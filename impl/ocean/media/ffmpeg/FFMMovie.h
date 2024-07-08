/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FFM_MOVIE_H
#define META_OCEAN_MEDIA_FFM_MOVIE_H

#include "ocean/media/ffmpeg/FFmpeg.h"
#include "ocean/media/ffmpeg/FFMMedium.h"

#include "ocean/base/Thread.h"

#include "ocean/media/Movie.h"

// Forward declaration.
struct AVCodecContext;
struct AVFrame;
struct AVStream;

namespace Ocean
{

namespace Media
{

namespace FFmpeg
{

/**
 * This class implements an FFmpeg movie object.
 * @ingroup mediaffm
 */
class FFMMovie :
	virtual public FFMMedium,
	virtual public Movie,
	protected Thread
{
	friend class FFMLibrary;

	protected:

		/**
		 * Definition of an unordered map mapping frame indices to presentation timestamps.
		 */
		typedef std::unordered_map<int64_t, int64_t> PacketTimestampMap;

	public:

		/**
		 * Clones this movie medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Returns the duration of the movie medium.
		 * @see FiniteMedium::duration()
		 */
		double duration() const override;

		/**
		 * Returns the duration of the movie medium without speed consideration.
		 * @see FiniteMedium::normalDuration()
		 */
		double normalDuration() const override;

		/**
		 * Returns the recent position of the movie medium.
		 * @see FiniteMedium::position()
		 */
		double position() const override;

		/**
		 * Sets the recent position of the movie medium.
		 * @see FiniteMedium::setPosition()
		 */
		bool setPosition(const double position) override;

		/**
		 * Returns the speed of the movie medium.
		 * @see FiniteMedium::speed()
		 */
		float speed() const override;

		/**
		 * Sets the speed of the movie medium.
		 * @see FiniteMedium::setSpeed()
		 */
		bool setSpeed(const float speed) override;

		/**
		 * Returns the volume of the sound in db.
		 * @see SoundMedium::soundVolume()
		 */
		float soundVolume() const override;

		/**
		 * Returns whether the movie medium is in a mute state.
		 * @see SoundMedium::soundMute()
		 */
		bool soundMute() const override;

		/**
		 * Sets the volume of the sound in db.
		 * @see SoundMedium::setSoundVolume()
		 */
		bool setSoundVolume(const float volume) override;

		/**
		 * Sets or unsets the movie medium to a mute state.
		 * @see SoundMedium::setSoundMute()
		 */
		bool setSoundMute(const bool mute) override;

		/**
		 * Enables or disables the audio in this movie (has no effect if the movie does not have audio).
		 * @see Movie::setUseSound()
		 */
		bool setUseSound(const bool state) override;

	protected:

		/**
		 * Creates a new movie by a given url.
		 * @param url Url of the movie
		 */
		explicit FFMMovie(const std::string& url);

		/**
		 * Destructs a FFMMovie object.
		 */
		~FFMMovie() override;

		/**
		 * Internally starts the medium.
		 * @see AVFMedium::internalStart()
		 */
		bool internalStart() override;

		/**
		 * Internally pauses the medium.
		 * @see AVFMedium::internalPause()
		 */
		bool internalPause() override;

		/**
		 * Internally stops the medium.
		 * @see AVFMedium::internalStop()
		 */
		bool internalStop() override;

		/**
		 * Creates and opens the video codec.
		 * @return True, if succeeded
		 */
		bool createAndOpenVideoCodec();

		/**
		 * Releases the video codec.
		 */
		void releaseVideoCodec();

		/**
		 * Extracts the Ocean frame from a FFmpeg frame.
		 * @param avFrame The FFmpeg frame from which the data will be extracted, must be valid
		 * @param avPixelFormat The pixel format of the FFmpeg frame, provided as 'AVPixelFormat', must be valid
		 * @param avColorRange The FFmpeg color range associated with the pixel format, provided as 'AVColorRange'
		 * @return The resulting Ocean frame owning the memory, invalid otherwise
		 */
		static Frame extractFrame(AVFrame* avFrame, const int avPixelFormat, const int avColorRange);

		/**
		 * Translates a FFmpeg pixel format to an Ocean pixel format.
		 * @param avPixelFormat The FFmpeg pixel format to translate, provided as 'AVPixelFormat'
		 * @param avColorRange The FFmpeg color range associated with the pixel format, provided as 'AVColorRange'
		 * @return The corresponding Ocean pixel format, FORMAT_UNDEFINED if no corresponding pixel format exists
		 */
		static FrameType::PixelFormat translatePixelFormat(const int avPixelFormat, const int avColorRange);

	private:

		/**
		 * Thread run function.
		 */
		void threadRun() override;

	protected:

		/// The FFmpeg video codec context.
		AVCodecContext* avVideoCodecContext_ = nullptr;

		/// The FFmpeg video stream.
		AVStream* avVideoStream_ = nullptr;

		/// The index of the FFmpeg video stream.
		int avVideoStreamIndex_ = -1;

		/// The current playback position in seconds.
		std::atomic<double> position_ = 0.0;

		/// The seek playback position in seconds, -1 if not set.
		std::atomic<double> seekPosition_ = -1.0;

		/// The duration of the movie in seconds, for a default speed with 1.0, -1 if unknown
		double normalDuration_ = -1.0;

		/// The playback speed of the movie.
		std::atomic<float> speed_ = 1.0f;

		/// True, if the movie is paused.
		std::atomic<bool> isPaused_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_FFM_MOVIE_H
