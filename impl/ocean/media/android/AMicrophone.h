/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_MICROPHONE_H
#define META_OCEAN_MEDIA_ANDROID_A_MICROPHONE_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/AMedium.h"

#include "ocean/media/Microphone.h"

#include <SLES/OpenSLES_Android.h>

#include <queue>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements a microphone class for Android.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT AMicrophone final :
	virtual public AMedium,
	virtual public Microphone
{
	friend class ALibrary;

	protected:

		/// The number of buffers in the queue.
		static constexpr SLuint32 numberBuffers_ = 4u;

		/**
		 * Definition of a vector holding sample elements.
		 */
		typedef std::vector<int16_t> Buffer;

	public:

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

		/**
		 * Returns the size of one mono chunk in elements.
		 * @return The size of one mono chunk, in elements
		 */
		static constexpr size_t monoChunkElements();

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param slEngineInterface The interface of the SL engine, must be valid
		 * @param url Url of the medium
		 */
		AMicrophone(const SLEngineItf& slEngineInterface, const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~AMicrophone() override;

		/**
		 * Initializes the audio and all corresponding resources.
		 * @param slEngineInterface The interface of the SL engine, must be valid
		 * @param microphoneTypes The types of the microphone to create
		 * @param microphoneConfigurations The configurations of the microphone to create
		 * @return True, if succeeded
		 */
		bool initialize(const SLEngineItf& slEngineInterface, const MicrophoneTypes microphoneTypes, const MicrophoneConfigurations microphoneConfigurations);

		/**
		 * Releases the audio and all corresponding resources.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Event callback function to fill the OpenSL buffer queue.
		 * @param bufferQueue The buffer queue to fill with the next samples, must be valid
		 */
		void onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue);

		/**
		 * Static event callback function to fill the OpenSL buffer queue.
		 * @param bufferQueue The buffer queue to fill with the next samples, must be valid
		 * @param context The context of the event (the ALiveAudio object), must be valid
		 */
		static void onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context);

	protected:

		/// Start timestamp.
		Timestamp startTimestamp_ = Timestamp(false);

		/// Pause timestamp.
		Timestamp pauseTimestamp_ = Timestamp(false);

		/// Stop timestamp.
		Timestamp stopTimestamp_ = Timestamp(false);

		/// The SL recorder object.
		SLObjectItf slRecorder_ = nullptr;

		/// The SL recorder interface.
		SLRecordItf slRecordInterface_ = nullptr;

		/// THe SL buffer queue interface.
		SLAndroidSimpleBufferQueueItf slBufferQueueInterface_ = nullptr;

		/// The buffer for the sample queue.
		std::queue<Buffer> bufferQueue_;

		/// The SL effect interface for the acoustic echo cancel effect.
		SLAndroidEffectItf slAcousticEchoCancelEffect_ = nullptr;

		/// The SL implementation id of the acoustic echo cancel effect.
		SLInterfaceID slAcousticEchoCancelerEffectImplementationId_ = nullptr;
};

constexpr size_t AMicrophone::monoChunkElements()
{
	constexpr size_t samplesPerSecondMono = 48000; // 48kHz

	return samplesPerSecondMono / 50; // 20ms
}

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_MICROPHONE_H
