/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_LIVE_AUDIO_H
#define META_OCEAN_MEDIA_ANDROID_A_LIVE_AUDIO_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/AMedium.h"

#include "ocean/media/LiveAudio.h"

#include <jni.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <queue>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements a LiveAudio class for Android.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT ALiveAudio final :
	virtual public AMedium,
	virtual public LiveAudio
{
	friend class ALibrary;
	friend class ALiveVideoManager;

	protected:

		/// The number of OpenSL buffers.
		static constexpr SLuint32 numberBuffers_ = 2u;

		/**
		 * This class implements a manager for sample chunks.
		 */
		class ChunkManager
		{
			protected:

				/**
				 * Definition of a chunk holding stereo samples.
				 */
				using StereoChunk = std::vector<int16_t>;

				/**
				 * Returns the size of one stereo chunk in elements.
				 * @return The size of one stereo chunk, in elements
				 */
				static constexpr size_t stereoChunkSize();

				/**
				 * Definition of shared pointer holding a stereo chunk.
				 */
				using SharedStereoChunk = std::shared_ptr<StereoChunk>;

				/**
				 * Definition of a vector holding stereo chunks.
				 */
				typedef std::vector<SharedStereoChunk> StereoChunks;

				/**
				 * Definition of a queue holding stereo chunks.
				 */
				typedef std::queue<SharedStereoChunk> StereoChunkQueue;

			public:

				/**
				 * Creates a new manager object.
				 */
				ChunkManager() = default;

				/**
				 * Adds new samples to manager.
				 * @param sampleType The type of the given samples, must be valid
				 * @param data The samples to add, must be valid
				 * @param size The size of the given samples, in bytes, with range [1, infinity)
				 * @param bufferQueueInterface The interface of the OpenSL buffer queue, must be valid
				 */
				bool addSamples(const SampleType sampleType, const void* data, const size_t size, SLAndroidSimpleBufferQueueItf bufferQueueInterface);

				/**
				 * Returns whether new samples need to be added (because the queue is running out of samples).
				 * @return True, if so
				 */
				inline bool needNewSamples() const;

				/**
				 * Enqueues the next pending chunk into OpenSL buffer.
				 * @param bufferQueueInterface The interface of the buffer queue in which the chunk will be queued, must be valid
				 * @return True, if succeeded; False, if there was no pending chunk
				 */
				bool enqueueNextPendingChunk(SLAndroidSimpleBufferQueueItf bufferQueueInterface);

			protected:

				/// The queue with stereo chunks which have been queued in OpenSL (OpenSL is working on these chunks).
				StereoChunkQueue openslStereoChunkQueue_;

				/// Pending stereo chunks which need to be queued in OpenSL (but OpenSL does not have a free buffer left).
				StereoChunkQueue pendingStereoChunks_;

				/// Free stereo chunks which can be queued again.
				StereoChunks freeStereoChunks_;

				/// The stereo chunk which is currently filled.
				SharedStereoChunk fillingStereoChunk_;

				/// The position within the current (not yet entirely filled) stereo chunk.
				size_t positionInFillingChunk_ = size_t(0);

				/// The lock for the chunks.
				mutable Lock lock_;
		};

	public:

		/**
		 * Clones this movie medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Adds a new sample in case this audio object receives the audio data from a buffer/stream.
		 * @see LiveAudio::addSample().
		 */
		bool addSamples(const SampleType sampleType, const void* data, const size_t size) override;

		/**
		 * Returns whether a new sample needs to be added.
		 * @see LiveAudio::needNewSamples().
		 */
		bool needNewSamples() const override;

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
		explicit ALiveAudio(const SLEngineItf& slEngineInterface, const std::string& url);

		/**
		 * Destructs the live video object.
		 */
		~ALiveAudio() override;

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

		/// The SL player object.
		SLObjectItf slPlayer_ = nullptr;

		/// The SL player interface.
		SLPlayItf slPlayInterface_ = nullptr;

		/// THe SL buffer queue interface.
		SLAndroidSimpleBufferQueueItf slBufferQueueInterface_ = nullptr;

		/// The SL output mix object.
		SLObjectItf slOutputMix_ = nullptr;

		/// The SL volume interface.
		SLVolumeItf slVolumeInterface_ = nullptr;

		/// The manager for chunks of samples.
		ChunkManager chunkManager_;

		/// The number of chunks which have to be queued manually before (not via the callback function).
		size_t remainingManuallyEnqueuedChunks_ = numberBuffers_;

		/// True, if the live audio has been stopped.
		std::atomic<bool> hasBeenStopped_ = true;
};

constexpr size_t ALiveAudio::ChunkManager::stereoChunkSize()
{
	constexpr size_t samplesPerSecondMono = 48000; // 48kHz
	constexpr size_t samplesPerSecondStereo = samplesPerSecondMono * 2;

	return samplesPerSecondStereo / 50; // 20ms
}

inline bool ALiveAudio::ChunkManager::needNewSamples() const
{
	const ScopedLock scopedLock(lock_);

	return pendingStereoChunks_.empty();
}

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_LIVE_AUDIO_H
