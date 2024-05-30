/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_LIVE_AUDIO_H
#define META_OCEAN_MEDIA_LIVE_AUDIO_H

#include "ocean/media/Media.h"
#include "ocean/media/ConfigMedium.h"
#include "ocean/media/LiveMedium.h"
#include "ocean/media/SoundMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class LiveAudio;

/**
 * Definition of a smart medium reference holding a live audio object.
 * @see SmartMediumRef, LiveAudio.
 * @ingroup media
 */
typedef SmartMediumRef<LiveAudio> LiveAudioRef;

/**
 * This class is the base class for all live audios.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT LiveAudio :
	public virtual LiveMedium,
	public virtual SoundMedium
{
	public:

		/**
		 * Definition of individual sample types.
		 */
		enum SampleType : uint32_t
		{
			/// An invalid sample type
			ST_INVALID = 0u,
			/// A sample with 16bit signed integer precision as mono channel and 48kHz
			ST_INTEGER_16_MONO_48,
			/// A sample with 16bit signed integer precision as stereo channel and 48kHz
			ST_INTEGER_16_STEREO_48
		};

	public:

		/**
		 * Adds new samples in case this audio object receives the audio data from a buffer/stream.
		 * @param sampleType The type of the samples, must be valid
		 * @param data The sample data, must be valid
		 * @param size The size of the sample data, in bytes, with range [1, infinity)
		 * @return True, if succeeded
		 */
		virtual bool addSamples(const SampleType sampleType, const void* data, const size_t size);

		/**
		 * Returns whether a new samples needs to be added.
		 * @return True, if so
		 */
		virtual bool needNewSamples() const = 0;

		/**
		 * Writes a WAV file with given sample data.
		 * @param filename The name of the WAV file to create, must be valid
		 * @param sampleType The type of the samples, must be valid
		 * @param data The sample data, must be valid
		 * @param size The size of the sample data, in bytes, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool writeWavFile(const std::string& filename, const SampleType sampleType, const void* data, const size_t size);

	protected:

		/**
		 * Creates a new live audio source by a given url.
		 * @param url Url of the live audio source
		 */
		explicit LiveAudio(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_LIVE_AUDIO_H
