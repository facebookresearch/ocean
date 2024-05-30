/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/ALiveAudio.h"

#include "ocean/media/Manager.h"

#include <SLES/OpenSLES_AndroidConfiguration.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

bool ALiveAudio::ChunkManager::addSamples(const SampleType sampleType, const void* data, const size_t size, SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	if (sampleType != ST_INTEGER_16_MONO_48 && sampleType != ST_INTEGER_16_STEREO_48)
	{
		ocean_assert(false && "Sample type is not supported");
		return false;
	}

	const size_t numberSourceElements = size / sizeof(int16_t);
	const int16_t* sourceElements = (const int16_t*)(data);

	const int16_t* sourceElementsEnd = sourceElements + numberSourceElements;

	while (sourceElements != sourceElementsEnd)
	{
		TemporaryScopedLock scopedLock(lock_);

			if (!fillingStereoChunk_)
			{
				if (freeStereoChunks_.empty())
				{
					fillingStereoChunk_ = std::make_shared<StereoChunk>(stereoChunkSize());
				}
				else
				{
					fillingStereoChunk_ = std::move(freeStereoChunks_.back());
					freeStereoChunks_.pop_back();
				}
			}

		scopedLock.release();

		ocean_assert(sourceElements < sourceElementsEnd);

		ocean_assert(fillingStereoChunk_);
		StereoChunk& currentChunk = *fillingStereoChunk_;

		ocean_assert(positionInFillingChunk_ % 2 == 0);
		int16_t* stereoElementsInChunk = currentChunk.data();

		if (sampleType == ST_INTEGER_16_MONO_48)
		{
			while (sourceElements != sourceElementsEnd && positionInFillingChunk_ < stereoChunkSize())
			{
				ocean_assert(sourceElements < sourceElementsEnd);

				stereoElementsInChunk[positionInFillingChunk_++] = *sourceElements;
				stereoElementsInChunk[positionInFillingChunk_++] = *sourceElements++;
			}
		}
		else
		{
			ocean_assert(sampleType == ST_INTEGER_16_STEREO_48);

			const size_t remainingSourceElements = sourceElementsEnd - sourceElements;
			const size_t remainingTargetElements = stereoChunkSize() - positionInFillingChunk_;

			const size_t elements = std::min(remainingSourceElements, remainingTargetElements);

			memcpy(stereoElementsInChunk, sourceElements, elements * sizeof(int16_t));

			positionInFillingChunk_ += elements;
			sourceElements += elements;
		}

		ocean_assert(positionInFillingChunk_ <= stereoChunkSize());
		if (positionInFillingChunk_ == stereoChunkSize())
		{
			// we have reached the end of the current chunk, so we switch to a fresh chunk

			scopedLock.relock(lock_);

			positionInFillingChunk_ = 0;

			ocean_assert(bufferQueueInterface != nullptr);

			if (openslStereoChunkQueue_.size() < numberBuffers_)
			{
				ocean_assert(pendingStereoChunks_.empty());

				// OpenSL's buffer queue is not yet full, so appending the current chunk to the OpenSL queue directly

				openslStereoChunkQueue_.emplace(fillingStereoChunk_);

				if ((*bufferQueueInterface)->Enqueue(bufferQueueInterface, fillingStereoChunk_->data(), SLuint32(fillingStereoChunk_->size() * sizeof(int16_t))) != SL_RESULT_SUCCESS)
				{
					ocean_assert(false && "This should never happen!");
					Log::warning() << "ALiveAudio: Failed to enqueue sample";
				}

				fillingStereoChunk_ = nullptr;
			}
			else
			{
				// OpenSL's buffer queue is full, so we need to add the current chunk to the pending queue

				pendingStereoChunks_.emplace(std::move(fillingStereoChunk_));

				if (pendingStereoChunks_.size() >= numberBuffers_ * 3u)
				{
					Log::warning() << "ALiveAudio sample queue too long, skipping most pending buffers";

					while (pendingStereoChunks_.size() > numberBuffers_)
					{
						freeStereoChunks_.emplace_back(std::move(pendingStereoChunks_.front()));
						pendingStereoChunks_.pop();
					}
				}
			}
		}
	}

	return true;
}

bool ALiveAudio::ChunkManager::enqueueNextPendingChunk(SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	ocean_assert(bufferQueueInterface != nullptr);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!openslStereoChunkQueue_.empty());

	freeStereoChunks_.emplace_back(std::move(openslStereoChunkQueue_.front()));
	openslStereoChunkQueue_.pop();

	if (pendingStereoChunks_.empty())
	{
		return false;
	}

	SharedStereoChunk nextChunk = std::move(pendingStereoChunks_.front());
	pendingStereoChunks_.pop();

	openslStereoChunkQueue_.emplace(nextChunk);

	if ((*bufferQueueInterface)->Enqueue(bufferQueueInterface, nextChunk->data(), SLuint32(nextChunk->size() * sizeof(int16_t))) != SL_RESULT_SUCCESS)
	{
		ocean_assert(false && "This should never happen!");
		Log::warning() << "ALiveAudio: Failed to enqueue sample";
	}

	return true;
}

ALiveAudio::ALiveAudio(const SLEngineItf& slEngineInterface, const std::string& url) :
	Medium(url),
	AMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	SoundMedium(url),
	LiveAudio(url)
{
	libraryName_ = nameAndroidLibrary();

	isValid_ = initialize(slEngineInterface);
}

ALiveAudio::~ALiveAudio()
{
	release();
}

MediumRef ALiveAudio::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return Manager::get().newMedium(url_, libraryName_, LIVE_AUDIO, true);
	}

	return MediumRef();
}

bool ALiveAudio::addSamples(const SampleType sampleType, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size != 0);

	TemporaryScopedLock scopedLock(lock_);

		if (!startTimestamp_.isValid())
		{
			return false;
		}

	scopedLock.release();

	if (!chunkManager_.addSamples(sampleType, data, size, slBufferQueueInterface_))
	{
		return false;
	}

	return true;
}

bool ALiveAudio::needNewSamples() const
{
	TemporaryScopedLock scopedLock(lock_);

		if (!startTimestamp_.isValid())
		{
			return false;
		}

	scopedLock.release();

	return chunkManager_.needNewSamples();
}

bool ALiveAudio::start()
{
	const ScopedLock scopedLock(lock_);

	hasBeenStopped_ = false;

	if (startTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toNow();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toInvalid();

			return true;
		}
	}

	return false;
}

bool ALiveAudio::pause()
{
	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_PAUSED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toNow();
			stopTimestamp_.toInvalid();

			return true;
		}
	}

	return false;
}

bool ALiveAudio::stop()
{
	const ScopedLock scopedLock(lock_);

	hasBeenStopped_ = true;

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	if (slPlayInterface_ != nullptr)
	{
		if ((*slPlayInterface_)->SetPlayState(slPlayInterface_, SL_PLAYSTATE_STOPPED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toNow();

			return true;
		}
	}

	return false;
}

bool ALiveAudio::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

float ALiveAudio::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		SLmillibel millibel = 0;
		if ((*slVolumeInterface_)->GetVolumeLevel(slVolumeInterface_, &millibel) != SL_RESULT_SUCCESS)
		{
			return -1.0f;
		}

		return float(millibel) * 0.1f;
	}

	return -1.0f;
}

bool ALiveAudio::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		SLboolean mute = SL_BOOLEAN_FALSE;
		if ((*slVolumeInterface_)->GetMute(slVolumeInterface_, &mute) != SL_RESULT_SUCCESS)
		{
			return false;
		}

		return mute == SL_BOOLEAN_TRUE ? true : false;
	}

	return false;
}

bool ALiveAudio::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		if ((*slVolumeInterface_)->SetVolumeLevel(slVolumeInterface_, SLmillibel(volume * 10.0f)) == SL_RESULT_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

bool ALiveAudio::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		if ((*slVolumeInterface_)->SetMute(slVolumeInterface_, mute ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE) == SL_RESULT_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

Timestamp ALiveAudio::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp ALiveAudio::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp ALiveAudio::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

bool ALiveAudio::initialize(const SLEngineItf& slEngineInterface)
{
	bool noError = true;

	SLEngineItf slEngineInterface_(slEngineInterface);

	ocean_assert(slOutputMix_ == nullptr);
	if (noError && (*slEngineInterface_)->CreateOutputMix(slEngineInterface_, &slOutputMix_, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL output mix";
		noError = false;
	}

	if (noError && (*slOutputMix_)->Realize(slOutputMix_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL output mix";
		noError = false;
	}

	SLDataLocator_AndroidSimpleBufferQueue slDataLocatorBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, numberBuffers_};

	constexpr SLuint32 numberChannels = 2u;
	SLDataFormat_PCM slDataFormatPCM = {SL_DATAFORMAT_PCM, numberChannels, SL_SAMPLINGRATE_48, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};
	SLDataSource slAudioSource = {&slDataLocatorBufferQueue, &slDataFormatPCM};

	ocean_assert(slOutputMix_ != nullptr);
	SLDataLocator_OutputMix dataLocatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX, slOutputMix_};
	SLDataSink slAudioSink = {&dataLocatorOutputMix, nullptr};

	const SLInterfaceID interfaceIds[3] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME, SL_IID_ANDROIDCONFIGURATION};
	const SLboolean interfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE};

	ocean_assert(slPlayer_ == nullptr);
	if (noError && (*slEngineInterface_)->CreateAudioPlayer(slEngineInterface_, &slPlayer_, &slAudioSource, &slAudioSink, 3u, interfaceIds, interfaceRequired) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio player";
		noError = false;
	}

	SLAndroidConfigurationItf slPlayerConfiguration = nullptr;
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_ANDROIDCONFIGURATION, &slPlayerConfiguration) != SL_RESULT_SUCCESS)
	{
		Log::warning() << "Failed to create SL player configuration interface";
	}

	if (noError && slPlayerConfiguration != nullptr)
	{
		constexpr bool useMediaStream = true;

		const SLint32 streamType = useMediaStream ? SL_ANDROID_STREAM_MEDIA : SL_ANDROID_STREAM_VOICE;
		if ((*slPlayerConfiguration)->SetConfiguration(slPlayerConfiguration, SL_ANDROID_KEY_STREAM_TYPE, &streamType, sizeof(SLint32)) != SL_RESULT_SUCCESS)
		{
			Log::warning() << "Failed to set SL player's stream type";
		}
	}

	if (noError && (*slPlayer_)->Realize(slPlayer_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL audio player";
		noError = false;
	}

	ocean_assert(slPlayInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_PLAY, &slPlayInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio player interface";
		noError = false;
	}

	ocean_assert(slBufferQueueInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &slBufferQueueInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio player interface";
		noError = false;
	}

	if (noError && (*slBufferQueueInterface_)->RegisterCallback(slBufferQueueInterface_, onFillBufferQueueCallback, this) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to register callback";
		noError = false;
	}

	ocean_assert(slVolumeInterface_ == nullptr);
	if (noError && (*slPlayer_)->GetInterface(slPlayer_, SL_IID_VOLUME, &slVolumeInterface_) != SL_RESULT_SUCCESS)
	{
		Log::warning() << "Failed to create SL volume interface";
	}

	if (noError == false)
	{
		release();
	}

	return noError;
}

bool ALiveAudio::release()
{
	stop();

	slVolumeInterface_ = nullptr;
	slPlayInterface_ = nullptr;
	slBufferQueueInterface_ = nullptr;

	if (slPlayer_ != nullptr)
	{
		(*slPlayer_)->Destroy(slPlayer_);
		slPlayer_ = nullptr;
	}

	if (slOutputMix_ != nullptr)
	{
		(*slOutputMix_)->Destroy(slOutputMix_);
		slOutputMix_ = nullptr;
	}

	return true;
}

void ALiveAudio::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue)
{
	ocean_assert(bufferQueue != nullptr);
	ocean_assert(bufferQueue == slBufferQueueInterface_);

	// the oldst buffer has been processed by OpenSL
	chunkManager_.enqueueNextPendingChunk(bufferQueue);
}

void ALiveAudio::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
	ocean_assert(bufferQueue != nullptr && context != nullptr);

	ALiveAudio* liveAudio = static_cast<ALiveAudio*>(context);
	ocean_assert(liveAudio != nullptr);

	liveAudio->onFillBufferQueueCallback(bufferQueue);
}

}

}

}
