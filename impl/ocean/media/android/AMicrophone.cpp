/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/AMicrophone.h"

#include "ocean/base/Thread.h"
#include "ocean/base/Utilities.h"

#include "ocean/math/Numeric.h"

#include "ocean/media/Manager.h"

#include <SLES/OpenSLES_AndroidConfiguration.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

AMicrophone::AMicrophone(const SLEngineItf& slEngineInterface, const std::string& url) :
	Medium(url),
	AMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	Microphone(url)
{
	const std::string lowerUrl = String::toLower(url);

	ocean_assert(lowerUrl.find("microphone") != std::string::npos);

	libraryName_ = nameAndroidLibrary();

	MicrophoneTypes microphoneTypes = MT_ANY;
	MicrophoneConfigurations microphoneConfigurations = MC_DEFAULT;

	if (lowerUrl.find("external") != std::string::npos)
	{
		microphoneTypes = MT_EXTERNAL;
	}

	if (lowerUrl.find("voice") != std::string::npos)
	{
		microphoneConfigurations = MicrophoneConfigurations(microphoneConfigurations | MC_VOICE_COMMUNICATION);
	}

	if (lowerUrl.find("performance") != std::string::npos)
	{
		microphoneConfigurations = MicrophoneConfigurations(microphoneConfigurations | MC_PERFORMANCE);
	}

	if (lowerUrl.find("gained") != std::string::npos)
	{
		microphoneConfigurations = MicrophoneConfigurations(microphoneConfigurations | MC_GAINED);
	}

	isValid_ = initialize(slEngineInterface, microphoneTypes, microphoneConfigurations);
}

AMicrophone::~AMicrophone()
{
	release();
}

bool AMicrophone::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	if (slRecordInterface_ != nullptr)
	{
		ocean_assert(slBufferQueueInterface_ != nullptr);

		if ((*slBufferQueueInterface_)->Clear(slBufferQueueInterface_) != SL_RESULT_SUCCESS)
		{
			Log::error() << "Failed to clear SL buffer queue";
			return false;
		}

		bufferQueue_ = std::queue<Buffer>();

		for (SLuint32 n = 0u; n < numberBuffers_; ++n)
		{
			bufferQueue_.emplace(monoChunkElements());
			Buffer& buffer = bufferQueue_.back();

			if ((*slBufferQueueInterface_)->Enqueue(slBufferQueueInterface_, buffer.data(), SLuint32(buffer.size() * sizeof(int16_t))) != SL_RESULT_SUCCESS)
			{
				Log::error() << "Failed to enqueue SL buffer";
				return false;
			}
		}

		if ((*slRecordInterface_)->SetRecordState(slRecordInterface_, SL_RECORDSTATE_RECORDING) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toNow();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toInvalid();

			if (slAcousticEchoCancelEffect_ != nullptr)
			{
				if ((*slAcousticEchoCancelEffect_)->SetEnabled(slAcousticEchoCancelEffect_, slAcousticEchoCancelerEffectImplementationId_, true) == SL_RESULT_SUCCESS)
				{
					SLboolean isEnabled = false;
					if ((*slAcousticEchoCancelEffect_)->IsEnabled(slAcousticEchoCancelEffect_, slAcousticEchoCancelerEffectImplementationId_, &isEnabled) == SL_RESULT_SUCCESS)
					{
						Log::warning() << "Acoustic Echo Cancel effect may not be enabled";
					}
				}
				else
				{
					Log::error() << "Failed to enable Acoustic Echo Cancel effect";
				}
			}

			return true;
		}
	}

	return false;
}

bool AMicrophone::pause()
{
	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (slRecordInterface_ != nullptr)
	{
		if ((*slRecordInterface_)->SetRecordState(slRecordInterface_, SL_RECORDSTATE_STOPPED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toNow();
			stopTimestamp_.toInvalid();

			return true;
		}
	}

	return false;
}

bool AMicrophone::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	if (slRecordInterface_ != nullptr)
	{
		if ((*slRecordInterface_)->SetRecordState(slRecordInterface_, SL_RECORDSTATE_STOPPED) == SL_RESULT_SUCCESS)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toNow();

			return true;
		}
	}

	return false;
}

bool AMicrophone::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp AMicrophone::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp AMicrophone::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp AMicrophone::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

bool AMicrophone::initialize(const SLEngineItf& slEngineInterface, const MicrophoneTypes microphoneTypes, const MicrophoneConfigurations microphoneConfigurations)
{
	bool noError = true;

	SLEngineItf slEngineInterface_(slEngineInterface);

	SLDataLocator_IODevice slDataLocator = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
	SLDataSource slAudioSource = {&slDataLocator, nullptr};

	/// The number of OpenSL buffers.
	SLDataLocator_AndroidSimpleBufferQueue slDataLocatorBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, numberBuffers_};

	constexpr SLuint32 numberChannels = 1u;
	SLDataFormat_PCM slDataFormatPCM = {SL_DATAFORMAT_PCM, numberChannels, SL_SAMPLINGRATE_48, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_LEFT, SL_BYTEORDER_LITTLEENDIAN};

	SLDataSink slAudioSink = {&slDataLocatorBufferQueue, &slDataFormatPCM};

	std::vector<SLInterfaceID> interfaceIds = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION, SL_IID_ANDROIDEFFECTCAPABILITIES};
	std::vector<SLboolean> interfaceRequired = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};

	SLInterfaceID slAcousticEchoCancelerInterfaceId = nullptr;

	if ((microphoneConfigurations & MC_VOICE_COMMUNICATION) == MC_VOICE_COMMUNICATION)
	{
		ocean_assert(slAcousticEchoCancelerEffectImplementationId_ == nullptr);
		if (EffectManager::get().effectIds("Acoustic Echo Canceler", slAcousticEchoCancelerInterfaceId, slAcousticEchoCancelerEffectImplementationId_))
		{
			interfaceIds.emplace_back(slAcousticEchoCancelerInterfaceId);
			interfaceRequired.emplace_back(SL_BOOLEAN_FALSE);
		}
		else
		{
			Log::warning() << "The engine does not have provide an Acoustic Echo Canceler effect";
		}
	}

	ocean_assert(interfaceIds.size() == interfaceRequired.size());

	ocean_assert(slRecorder_ == nullptr);
	if (noError && (*slEngineInterface_)->CreateAudioRecorder(slEngineInterface_, &slRecorder_, &slAudioSource, &slAudioSink, SLuint32(interfaceIds.size()), interfaceIds.data(), interfaceRequired.data()) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio recorder";
		noError = false;
	}

	SLAndroidConfigurationItf slInputConfig = nullptr;
	if (noError && (*slRecorder_)->GetInterface(slRecorder_, SL_IID_ANDROIDCONFIGURATION, &slInputConfig) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL audio recorder";
		noError = false;
	}

	MicrophoneTypes resultingMicrophoneTypes = MT_UNKNOWN;
	MicrophoneConfigurations resultingMicrophoneConfigurations = MicrophoneConfigurations(microphoneConfigurations & MC_GAINED);

	/*
	 * Android recording presets (not officially documented):
	 *
	 * Value:                                                Type:                            Optimized for:
	 *                                                       Built-in    External             Voice communication    Performance                Comment
	 *
	 * SL_ANDROID_RECORDING_PRESET_UNPROCESSED               Yes         Yes                  No                     No                         No pre-processing, raw input
	 * SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION         Yes         Yes                  Yes                    No                         Almost no latency, no Acoustic Echo Canceler
	 * SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION       Yes         Yes                  No                     Yes                        Some latency, can include Acoustic Echo Canceler
	 * SL_ANDROID_RECORDING_PRESET_CAMCORDER                 No          Yes                  No                     No                         Using an external microphone (e.g., via a headphone jack)
	 * SL_ANDROID_RECORDING_PRESET_GENERIC                   Yes         Yes                  No                     No                         If other presets do not fit
	 */

	SLuint32 presetValue = SL_ANDROID_RECORDING_PRESET_NONE;

	if (microphoneTypes == MT_EXTERNAL)
	{
		presetValue = SL_ANDROID_RECORDING_PRESET_CAMCORDER;

		resultingMicrophoneTypes = MT_EXTERNAL;
	}
	else
	{
		resultingMicrophoneTypes = MT_ANY;

		if ((microphoneConfigurations & MC_VOICE_COMMUNICATION) == MC_VOICE_COMMUNICATION)
		{
			presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION;

			resultingMicrophoneConfigurations = MicrophoneConfigurations(resultingMicrophoneConfigurations | MC_VOICE_COMMUNICATION);
		}
		else if ((microphoneConfigurations & MC_PERFORMANCE) == MC_PERFORMANCE)
		{
			presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION;

			resultingMicrophoneConfigurations = MicrophoneConfigurations(resultingMicrophoneConfigurations | MC_PERFORMANCE);
		}
		else
		{
			// no specifc configuration, we use best audio quality

			presetValue = SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION;

			resultingMicrophoneConfigurations = MicrophoneConfigurations(resultingMicrophoneConfigurations | MC_VOICE_COMMUNICATION);
		}
	}

	ocean_assert(presetValue != SL_ANDROID_RECORDING_PRESET_NONE);

	if (noError && (*slInputConfig)->SetConfiguration(slInputConfig, SL_ANDROID_KEY_RECORDING_PRESET, &presetValue, sizeof(presetValue)) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to set SL recording configuration";
		noError = false;
	}

	if (noError && (*slRecorder_)->Realize(slRecorder_, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to realize SL recorder, need to set a permission?";
		noError = false;
	}

	if (noError && slAcousticEchoCancelerInterfaceId != nullptr)
	{
		ocean_assert(slAcousticEchoCancelEffect_ == nullptr);
		if (noError && (*slRecorder_)->GetInterface(slRecorder_, slAcousticEchoCancelerInterfaceId, &slAcousticEchoCancelEffect_) == SL_RESULT_SUCCESS)
		{
			ocean_assert(slAcousticEchoCancelEffect_ != nullptr);

			if (slAcousticEchoCancelEffect_ != nullptr)
			{
				ocean_assert(slAcousticEchoCancelerEffectImplementationId_ != nullptr);

				if ((*slAcousticEchoCancelEffect_)->CreateEffect(slAcousticEchoCancelEffect_, slAcousticEchoCancelerEffectImplementationId_) == SL_RESULT_SUCCESS)
				{
					Log::debug() << "Created Acoustic Echo Canceler effect";
				}
				else
				{
					Log::warning() << "Failed to create Acoustic Echo Canceler effect";
				}
			}
		}
	}

	if (noError && (*slRecorder_)->GetInterface(slRecorder_, SL_IID_RECORD, &slRecordInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL record interface";
		noError = false;
	}

	ocean_assert(slBufferQueueInterface_ == nullptr);
	if (noError && (*slRecorder_)->GetInterface(slRecorder_, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &slBufferQueueInterface_) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to create SL buffer queue interface";
		noError = false;
	}

	if (noError && (*slBufferQueueInterface_)->RegisterCallback(slBufferQueueInterface_, onFillBufferQueueCallback, this) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to register callback";
		noError = false;
	}

	if (noError)
	{
		microphoneTypes_ = resultingMicrophoneTypes;
		microphoneConfigurations_ = resultingMicrophoneConfigurations;
	}
	else
	{
		release();
	}

	return noError;
}

bool AMicrophone::release()
{
	stop();

	slRecordInterface_ = nullptr;
	slBufferQueueInterface_ = nullptr;

	slAcousticEchoCancelEffect_ = nullptr;
	slAcousticEchoCancelerEffectImplementationId_ = nullptr;

	if (slRecorder_)
	{
		(*slRecorder_)->Destroy(slRecorder_);
		slRecorder_ = nullptr;
	}

	return true;
}

void AMicrophone::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue)
{
	ocean_assert(bufferQueue != nullptr);

	Buffer buffer(std::move(bufferQueue_.front()));
	bufferQueue_.pop();

#ifdef OCEAN_INTENSIVE_DEBUG
	SLAndroidSimpleBufferQueueState queueState;
	if ((*bufferQueue)->GetState(bufferQueue, &queueState) == SL_RESULT_SUCCESS)
	{
		Log::debug() << "onFillBufferQueueCallback " << queueState.index << " / " << queueState.count;
	}
#endif

	if ((microphoneConfigurations_ & MC_GAINED) == MC_GAINED)
	{
		// applying a manual gain of 3x

		for (size_t n = 0; n < buffer.size(); ++n)
		{
			buffer[n] = minmax<int32_t>(int32_t(NumericT<int16_t>::minValue()), int32_t(buffer[n]) * 3, int32_t(NumericT<int16_t>::maxValue()));
		}
	}

	sendSamples(ST_INTEGER_16_MONO_48, buffer.data(), buffer.size() * sizeof(int16_t));

	if ((*slBufferQueueInterface_)->Enqueue(slBufferQueueInterface_, buffer.data(), SLuint32(buffer.size() * sizeof(int16_t))) != SL_RESULT_SUCCESS)
	{
		Log::error() << "Failed to enqueue SL buffer";
	}

	bufferQueue_.emplace(std::move(buffer));
}

void AMicrophone::onFillBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
	ocean_assert(bufferQueue != nullptr && context != nullptr);

	AMicrophone* microphone = static_cast<AMicrophone*>(context);
	ocean_assert(microphone != nullptr);

	microphone->onFillBufferQueueCallback(bufferQueue);
}

}

}

}
