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

size_t ALiveAudio::ChunkManager::StereoChunk::addElements(const SampleType sampleType, const int16_t* elements, const size_t size)
{
	ocean_assert(elements != nullptr && size > 0);

	ocean_assert(!buffer_.empty());
	ocean_assert(position_ <= buffer_.size());

	const size_t remainingElements = buffer_.size() - position_;
	ocean_assert(remainingElements % 2 == 0);

	if (remainingElements == 0 || remainingElements % 2 != 0)
	{
		return 0;
	}

	size_t elementsToCopy = 0;

	if (sampleType == ST_INTEGER_16_MONO_48)
	{
		elementsToCopy = std::min(size, remainingElements / 2);

		for (size_t n = 0; n < elementsToCopy; ++n)
		{
			buffer_[position_++] = elements[n];
			buffer_[position_++] = elements[n];
		}
	}
	else
	{
		ocean_assert(sampleType == ST_INTEGER_16_STEREO_48);

		ocean_assert(size % 2 == 0);

		elementsToCopy = std::min(size, remainingElements);

		memcpy(buffer_.data() + position_, elements, elementsToCopy * sizeof(int16_t));
		position_ += elementsToCopy;
	}

	ocean_assert(position_ <= buffer_.size());
	ocean_assert(elementsToCopy > 0);

	return elementsToCopy;
}

bool ALiveAudio::ChunkManager::addSamples(const SampleType sampleType, const void* data, const size_t size, SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	if (sampleType != ST_INTEGER_16_MONO_48 && sampleType != ST_INTEGER_16_STEREO_48)
	{
		ocean_assert(false && "Sample type is not supported");
		return false;
	}

	size_t remainingSourceElements = size / sizeof(int16_t);
	const int16_t* sourceElements = (const int16_t*)(data);

	while (remainingSourceElements != 0)
	{
		if (!fillingStereoChunk_)
		{
			const ScopedLock scopedLock(lock_);

			if (freeStereoChunks_.empty())
			{
				fillingStereoChunk_ = std::make_unique<StereoChunk>();
			}
			else
			{
				fillingStereoChunk_ = std::move(freeStereoChunks_.back());
				assert(fillingStereoChunk_->size() == 0);

				freeStereoChunks_.pop_back();
			}
		}

		ocean_assert(fillingStereoChunk_);
		ocean_assert(!fillingStereoChunk_->isFull());

		const size_t addedElements = fillingStereoChunk_->addElements(sampleType, sourceElements, remainingSourceElements);
		ocean_assert(addedElements > 0 && addedElements <= remainingSourceElements);

		sourceElements += addedElements;
		remainingSourceElements -= addedElements;

		if (fillingStereoChunk_->isFull())
		{
			// we have reached the end of the current chunk, so we switch to a fresh chunk

			ScopedLock scopedLock(lock_);

			pendingStereoChunks_.emplace(std::move(fillingStereoChunk_));
			fillingStereoChunk_ = nullptr;

			while (openslStereoChunkQueue_.size() < numberBuffers_ && !pendingStereoChunks_.empty())
			{
				// OpenSL's buffer queue has still a remaining spot left, so appending the current chunk to the OpenSL queue directly

				enqueueNextPendingChunk(bufferQueueInterface);
			}

			if (pendingStereoChunks_.size() >= numberBuffers_ * 3u)
			{
				Log::warning() << "ALiveAudio sample queue too long, skipping most pending buffers";

				while (pendingStereoChunks_.size() > numberBuffers_)
				{
					UniqueStereoChunk pendingChunk = std::move(pendingStereoChunks_.front());
					pendingStereoChunks_.pop();

					pendingChunk->reset();

					freeStereoChunks_.emplace_back(std::move(pendingChunk));
				}
			}
		}
	}

	return true;
}

bool ALiveAudio::ChunkManager::fillBufferQueue(SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	ocean_assert(bufferQueueInterface != nullptr);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!openslStereoChunkQueue_.empty());

	UniqueStereoChunk finishedChunk = std::move(openslStereoChunkQueue_.front());
	openslStereoChunkQueue_.pop();

	finishedChunk->reset();
	freeStereoChunks_.emplace_back(std::move(finishedChunk));

	if (pendingStereoChunks_.empty())
	{
		return false;
	}

	enqueueNextPendingChunk(bufferQueueInterface);

	return true;
}

void ALiveAudio::ChunkManager::enqueueNextPendingChunk(SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	// lock_ is already locked

	assert(!pendingStereoChunks_.empty());

	UniqueStereoChunk nextChunk = std::move(pendingStereoChunks_.front());
	pendingStereoChunks_.pop();

	if ((*bufferQueueInterface)->Enqueue(bufferQueueInterface, nextChunk->data(), SLuint32(nextChunk->size() * sizeof(int16_t))) == SL_RESULT_SUCCESS)
	{
		openslStereoChunkQueue_.emplace(std::move(nextChunk));
	}
	else
	{
		ocean_assert(false && "This should never happen!");
		Log::warning() << "ALiveAudio: Failed to enqueue sample";

		nextChunk->reset();
		freeStereoChunks_.emplace_back(std::move(nextChunk));
	}
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

		return float(millibel) * 0.01f;
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

bool ALiveAudio::setSoundVolume(const float decibels)
{
	const ScopedLock scopedLock(lock_);

	if (slVolumeInterface_ != nullptr)
	{
		if ((*slVolumeInterface_)->SetVolumeLevel(slVolumeInterface_, SLmillibel(decibels * 100.0f)) == SL_RESULT_SUCCESS)
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
		const SLint32 streamType = preferredStreamType();
		ocean_assert(streamType >= SL_ANDROID_STREAM_VOICE && streamType <= SL_ANDROID_STREAM_NOTIFICATION);

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

SLint32 ALiveAudio::preferredStreamType() const
{
	const std::string lowerURL(String::toLower(url_));

	if (lowerURL.find("/voice") != std::string::npos)
	{
		return SL_ANDROID_STREAM_VOICE;
	}
	else if (lowerURL.find("/system") != std::string::npos)
	{
		return SL_ANDROID_STREAM_SYSTEM;
	}
	else if (lowerURL.find("/ring") != std::string::npos)
	{
		return SL_ANDROID_STREAM_RING;
	}
	else if (lowerURL.find("/alarm") != std::string::npos)
	{
		return SL_ANDROID_STREAM_ALARM;
	}
	else if (lowerURL.find("/notification") != std::string::npos)
	{
		return SL_ANDROID_STREAM_NOTIFICATION;
	}

	return SL_ANDROID_STREAM_MEDIA;
}

void ALiveAudio::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue)
{
	ocean_assert(bufferQueue != nullptr);
	ocean_assert(bufferQueue == slBufferQueueInterface_);

	// the oldest buffer has been processed by OpenSL
	chunkManager_.fillBufferQueue(bufferQueue);
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
