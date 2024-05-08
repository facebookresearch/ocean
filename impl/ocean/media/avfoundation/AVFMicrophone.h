/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_MICROPHONE_H
#define META_OCEAN_MEDIA_AVF_MICROPHONE_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFMedium.h"

#include "ocean/media/Microphone.h"


namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements an AVFoundation microphone object.
 * @ingroup mediaavf
 */
class AVFMicrophone :
	virtual public AVFMedium,
	virtual public Microphone
{
	friend class AVFLibrary;

	protected:

		/**
		 * Definition of a vector holding sample elements.
		 */
		typedef std::vector<int16_t> Buffer;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit AVFMicrophone(const std::string& url);

		/**
		 * Destructs the media object.
		 */
		~AVFMicrophone() override;

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

		/// Optional convert from external audio format to internal audio format.
		AVAudioConverter* avAudioConverter_ = nullptr;

		/// Optional audio format for int16 samples.
		AVAudioFormat* avAudioFormatInternalInt16_ = nullptr;

		/// Optional audio buffer for int16 samples.
		AVAudioPCMBuffer* avAudioPCMBufferInternalInt16_ = nullptr;

		/// True, if the audio session was started.
		bool audioSessionStarted_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_MICROPHONE_H
