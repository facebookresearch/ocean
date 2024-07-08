/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_AUDIO_H
#define META_OCEAN_MEDIA_ANDROID_A_AUDIO_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/AMedium.h"

#include "ocean/media/Audio.h"

#include <jni.h>

#include <SLES/OpenSLES.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements an Audio class for Android.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT AAudio final :
	virtual public AMedium,
	virtual public Audio
{
	friend class ALibrary;

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
		 * Sets whether the medium has to be played in a loop.
		 * @see FiniteMedium::setLoop().
		 */
		bool setLoop(const bool value) override;

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
		 * Creates a new medium by a given url.
		 * @param slEngineInterface The interface of the SL engine, must be valid
		 * @param url Url of the medium
		 */
		explicit AAudio(const SLEngineItf& slEngineInterface, const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~AAudio() override;

		/**
		 * Initializes the audio and all corresponding resources.
		 * @param slEngineInterface The interface of the SL engine, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(const SLEngineItf& slEngineInterface);

		/**
		 * Releases the audio and all corresponding resources.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * The callback function for position events.
		 * @param slCaller The caller of the function
		 * @param slEvent The actual event
		 */
		void callbackFunction(SLPlayItf slCaller, SLuint32 slEvent);

		/**
		 * The static callback function for position events.
		 * @param slCaller The caller of the function
		 * @param context The callback's context
		 * @param slEvent The actual event
		 */
		static void staticCallbackFunction(SLPlayItf slCaller, void* context, SLuint32 slEvent);

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_ = Timestamp(false);

		/// Pause timestamp.
		Timestamp pauseTimestamp_ = Timestamp(false);

		/// Stop timestamp.
		Timestamp stopTimestamp_ = Timestamp(false);

		/// The SL player object.
		SLObjectItf slPlayer_ = nullptr;

		/// The SL player interface.
		SLPlayItf slPlayInterface_ = nullptr;

		/// The SL output mix object.
		SLObjectItf slOutputMix_ = nullptr;

		/// The SL seek interface.
		SLSeekItf slSeekInterface_ = nullptr;

		/// The SL volume interface.
		SLVolumeItf slVolumeInterface_ = nullptr;
};

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_AUDIO_H
