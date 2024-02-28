// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/android/ALiveAudio.h"

#include "ocean/base/Thread.h"

#include "ocean/media/Manager.h"

#include <SLES/OpenSLES_AndroidConfiguration.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

bool ALiveAudio::ChunkManager::addSamples(const SampleType sampleType, const void* data, const size_t size)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(fillingStereoChunk_);

	if (sampleType == ST_INTEGER_16_MONO_48 || sampleType == ST_INTEGER_16_STEREO_48)
	{
		const size_t numberSourceElements = size / sizeof(int16_t);
		const int16_t* sourceElements = (const int16_t*)(data);

		const int16_t* sourceElementsEnd = sourceElements + numberSourceElements;

		while (sourceElements != sourceElementsEnd)
		{
			ocean_assert(sourceElements < sourceElementsEnd);

			StereoChunk& currentChunk = *fillingStereoChunk_;

			ocean_assert(positionInWriteChunk_ % 2 == 0);
			int16_t* stereoElementsInChunk = currentChunk.data();

			if (sampleType == ST_INTEGER_16_MONO_48)
			{
				while (sourceElements != sourceElementsEnd && positionInWriteChunk_ < stereoChunkSize())
				{
					ocean_assert(sourceElements < sourceElementsEnd);

					stereoElementsInChunk[positionInWriteChunk_++] = *sourceElements;
					stereoElementsInChunk[positionInWriteChunk_++] = *sourceElements++;
				}
			}
			else
			{
				ocean_assert(sampleType == ST_INTEGER_16_STEREO_48);

				while (sourceElements != sourceElementsEnd && positionInWriteChunk_ < stereoChunkSize())
				{
					ocean_assert(sourceElements < sourceElementsEnd);

					stereoElementsInChunk[positionInWriteChunk_++] = *sourceElements++;
				}
			}

			if (positionInWriteChunk_ == stereoChunkSize())
			{
				// we have reached the end of the current chunk, so we switch to a fresh chunk

				positionInWriteChunk_ = 0;

				pendingStereoChunks_.emplace(std::move(fillingStereoChunk_));

				if (pendingStereoChunks_.size() >= numberBuffers_ * 3u)
				{
					Log::warning() << "Pending VOIP queue too long, skipping most pending buffers";

					while (pendingStereoChunks_.size() > numberBuffers_)
					{
						freeStereoChunks_.emplace_back(std::move(pendingStereoChunks_.front()));
						pendingStereoChunks_.pop();
					}
				}

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
		}

		return true;
	}

	ocean_assert(false && "Sample type is not supported");
	return false;
}

bool ALiveAudio::ChunkManager::enqueue(SLAndroidSimpleBufferQueueItf bufferQueueInterface)
{
	ocean_assert(bufferQueueInterface != nullptr);

	const ScopedLock scopedLock(lock_);

	if (pendingStereoChunks_.empty())
	{
		return false;
	}

	stereoChunkQueue_.emplace(std::move(pendingStereoChunks_.front()));
	pendingStereoChunks_.pop();

	if ((*bufferQueueInterface)->Enqueue(bufferQueueInterface, stereoChunkQueue_.back()->data(), SLuint32(stereoChunkQueue_.back()->size() * sizeof(int16_t))) != SL_RESULT_SUCCESS)
	{
		Log::warning() << "Failed to add dummy sample";
	}

	return true;
}

void ALiveAudio::ChunkManager::unqueue()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!stereoChunkQueue_.empty());

	freeStereoChunks_.emplace_back(std::move(stereoChunkQueue_.front()));
	stereoChunkQueue_.pop();
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

	const ScopedLock scopedLock(lock_);

	if (!startTimestamp_.isValid())
	{
		return false;
	}

	if (!chunkManager_.addSamples(sampleType, data, size))
	{
		return false;
	}

	if (remainingManuallyEnqueuedChunks_ != 0)
	{
		chunkManager_.enqueue(slBufferQueueInterface_);

		--remainingManuallyEnqueuedChunks_;
	}

	return true;
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

	if (slPlayer_)
	{
		(*slPlayer_)->Destroy(slPlayer_);
		slPlayer_ = nullptr;
	}

	if (slOutputMix_)
	{
		(*slOutputMix_)->Destroy(slOutputMix_);
		slOutputMix_ = nullptr;
	}

	return true;
}

void ALiveAudio::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue)
{
	ocean_assert(bufferQueue != nullptr);

	// the oldst buffer has been processed by OpenSL
	chunkManager_.unqueue();

	{
		const ScopedLock scopedLock(lock_);

		if (remainingManuallyEnqueuedChunks_ != 0)
		{
			// first, we will manually enqueue enough buffers
			return;
		}
	}

	while (!hasBeenStopped_)
	{
		if (chunkManager_.enqueue(bufferQueue))
		{
			return;
		}

		Thread::sleep(1u);
	}
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
