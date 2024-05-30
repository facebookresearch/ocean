/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_LIVE_AUDIO_H
#define META_OCEAN_MEDIA_AVF_LIVE_AUDIO_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFMedium.h"

#include "ocean/media/LiveAudio.h"


namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements an AVFoundation live audio object.
 * @ingroup mediaavf
 */
class AVFLiveAudio :
	virtual public AVFMedium,
	virtual public LiveAudio
{
	friend class AVFLibrary;

	public:

		/**
		 * Adds new samples in case this audio object receives the audio data from a buffer/stream.
		 * @param sampleType The type of the samples, must be valid
		 * @param data The sample data, must be valid
		 * @param size The size of the sample data, in bytes, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool addSamples(const SampleType sampleType, const void* data, const size_t size) override;

		/**
		 * Returns whether a new sample needs to be added.
		 * @see LiveAudio::needNewSamples().
		 */
		bool needNewSamples() const override;

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

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit AVFLiveAudio(const std::string& url);

		/**
		 * Destructs the media object.
		 */
		~AVFLiveAudio() override;

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
		 * Extracts and sends mono samles form a PCM buffer.
		 * @param avAudioPCMBuffer The PCM buffer from which the samples will be extracted, must be valid
		 * @return True, if succeeded
		 */
		bool sendSamplesMono(AVAudioPCMBuffer* avAudioPCMBuffer);

	protected:

		/// The audio engine.
		AVAudioEngine* avAudioEngine_ = nullptr;

		/// The audio player node.
		AVAudioPlayerNode* avAudioPlayerNode_ = nullptr;

		/// The audio mixer node e.g., allowing to control the volume.
		AVAudioMixerNode* avAudioMixerNode_ = nullptr;

		/// The audio format of the audio player node.
		AVAudioFormat* avAudioFormat_ = nullptr;

		/// Optional convert from external audio format to internal audio format.
		AVAudioConverter* avAudioConverter_ = nullptr;

		/// Optional audio format for int16 samples.
		AVAudioFormat* avAudioFormatInternalInt16_ = nullptr;

		/// Optional audio buffer for int16 samples.
		AVAudioPCMBuffer* avAudioPCMBufferInternalInt16_ = nullptr;

		/// The previous volume before the medium was muted; -1 if the medium is not muted.
		float previousVolume_ = -1.0f;

		/// True, if the audio session was started.
		bool audioSessionStarted_ = false;

		/// True, if the audio object needs a new samples.
		std::atomic_bool needNewSamples_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_LIVE_AUDIO_H
