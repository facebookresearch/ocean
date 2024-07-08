/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_MOVIE_H
#define META_OCEAN_MEDIA_AVF_MOVIE_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFFrameMedium.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Thread.h"

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements an AVFoundation movie object.
 * @ingroup mediaavf
 */
class AVFMovie :
	virtual public AVFFrameMedium,
	virtual public Movie,
	protected Thread
{
	friend class AVFLibrary;

	public:

		/**
		 * Definition of a callback function for finished playing.
		 */
		typedef Callback<void> FinishedPlayingCallback;

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
		explicit AVFMovie(const std::string& url);

		/**
		 * Destructs a AVFMovie object.
		 */
		~AVFMovie() override;

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
		 * Event function for
		 */
		void onFinishedPlaying();

		/**
		 * Creates a new asset reader by a given start time.
		 * Ensure that the current asset reader and track output are unset before.
		 * @param startTime The start time of the asset reader.
		 * @return True, if succeeded
		 */
		bool createNewAssetReader(const double startTime);

		/**
		 * Creates a new player.
		 * @return True, if succeeded
		 */
		bool createNewPlayer();

	private:

		/**
		 * Thread run function.
		 */
		void threadRun() override;

	protected:

		/// True to indicate that the player should be started.
		bool playerShouldStart_ = false;

		/// The speed of the movie medium
		float speed_ = 1.0f;

		/// The asset that holds the movie file
		AVURLAsset* asset_ = nullptr;

		/// The asset reader for non playback time respect
		AVAssetReader* assetReader_ = nullptr;

		/// The asset reader output
		AVAssetReaderTrackOutput* assetReaderTrackOutput_ = nullptr;

		/// The player for playback time respect
		AVPlayer* player_ = nullptr;

		/// The player item that is played
		AVPlayerItem* playerItem_ = nullptr;

		/// The player output
		AVPlayerItemVideoOutput* playerItemVideoOutput_ = nullptr;

		/// The player observer
		NSObject* observer_ = nullptr;

};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_MOVIE_H
