/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_AUDIO_H
#define META_OCEAN_MEDIA_AVF_AUDIO_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFMedium.h"

#include "ocean/base/Callback.h"

#include "ocean/media/Audio.h"

/**
 * Delegate for audio playing events.
 */
@interface OceanMediaAVFAudioAVAudioPlayerDelegate : NSObject<AVAudioPlayerDelegate>
@end

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements an AVFoundation audio object.
 * @ingroup mediaavf
 */
class AVFAudio :
	virtual public AVFMedium,
	virtual public Audio
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
		 * Sets whether the medium has to be played in a loop.
		 * @see FiniteMedium::setLoop().
		 */
		bool setLoop(const bool value) override;

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

	protected:

		/**
		 * Creates a new audio by a given url.
		 * @param url Url of the audio
		 */
		explicit AVFAudio(const std::string& url);

		/**
		 * Destructs a AVFMovie object.
		 */
		~AVFAudio() override;

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

	protected:

		/// The actual audio player.
		AVAudioPlayer* avAudioPlayer_ = nullptr;

		/// The delegate for audio playing events.
		OceanMediaAVFAudioAVAudioPlayerDelegate* oceanMediaAVFAudioAVAudioPlayerDelegate_ = nullptr;

		/// The volume before mute, -1 if invalid.
		float volumeBeforeMute_ = -1.0f;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_AUDIO_H
