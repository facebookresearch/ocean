/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFMicrophone.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#import <Foundation/Foundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFMicrophone::AVFMicrophone(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	Microphone(url)
{
#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	AudioSessionManager::get().initialize(AVAudioSessionCategoryPlayAndRecord, AVAudioSessionModeVideoChat);
#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS

	avAudioEngine_ = [[AVAudioEngine alloc] init];

	if (avAudioEngine_.inputNode != nullptr)
	{
		[avAudioEngine_.inputNode installTapOnBus:0 bufferSize:1 format:nullptr block:^(AVAudioPCMBuffer* avAudioPCMBuffer, AVAudioTime* avAudioTime) {
			if (!sendSamplesMono(avAudioPCMBuffer))
			{
				Log::error() << "AVFMicrophone received wrong sample format";
				stop();
			}
		}];

		isValid_ = true;
	}
	else
	{
		Log::error() << "No input node in AVAudioEngine";

		isValid_ = false;
	}
}

AVFMicrophone::~AVFMicrophone()
{
	stop();

	avAudioFormatInternalInt16_ = nullptr;
	avAudioPCMBufferInternalInt16_ = nullptr;
	avAudioConverter_ = nullptr;

	avAudioEngine_ = nullptr;
}

bool AVFMicrophone::internalStart()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);

	audioSessionStarted_ = AudioSessionManager::get().start();

	ocean_assert(avAudioEngine_ != nullptr);

	if ([avAudioEngine_ startAndReturnError:nullptr] != YES)
	{
		Log::error() << "Failed to start AVAudioEngine";
		return false;
	}

	return true;
}

bool AVFMicrophone::internalPause()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);

	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (!startTimestamp_.isValid())
	{
		return false;
	}

	ocean_assert(avAudioEngine_ != nullptr);

	[avAudioEngine_ pause];

	return true;
}

bool AVFMicrophone::internalStop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	ocean_assert(avAudioEngine_ != nullptr);

	[avAudioEngine_ stop];

	if (audioSessionStarted_)
	{
		AudioSessionManager::get().stop();
		audioSessionStarted_ = false;
	}

	return true;
}

bool AVFMicrophone::sendSamplesMono(AVAudioPCMBuffer* avAudioPCMBuffer)
{
	ocean_assert(avAudioPCMBuffer != nullptr);

	constexpr SamplesType sampleType = ST_INTEGER_16_MONO_48;
	constexpr unsigned int expectedSampleRate = 48000u;

	if (avAudioPCMBuffer.format.sampleRate != expectedSampleRate)
	{
		Log::debug() << "AVFMicrophone: The PCM buffer has wrong sample rate got " << avAudioPCMBuffer.format.sampleRate << " Hz";
		return false;
	}

	if (avAudioPCMBuffer.format.channelCount != 1u)
	{
		Log::debug() << "AVFMicrophone: The PCM buffer is not a mono channel, got " << avAudioPCMBuffer.format.channelCount << " channels";
		return false;
	}

	if (avAudioPCMBuffer.stride != 1u)
	{
		Log::debug() << "AVFMicrophone: The PCM buffer does not have stride 1, got stride " << avAudioPCMBuffer.stride;
		return false;
	}

	if (avAudioPCMBuffer.frameLength == 0u)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (avAudioPCMBuffer.format.commonFormat == AVAudioPCMFormatFloat32)
	{
		if (avAudioConverter_ == nullptr)
		{
			ocean_assert(avAudioFormatInternalInt16_ == nullptr);

			constexpr double sampleRate = 48000.0;
			constexpr AVAudioChannelCount channelCount = 1u;
			constexpr bool interleaved = false;

			avAudioFormatInternalInt16_ = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatInt16 sampleRate:sampleRate channels:channelCount interleaved:interleaved];

			avAudioConverter_ = [[AVAudioConverter alloc] initFromFormat:avAudioPCMBuffer.format toFormat:avAudioFormatInternalInt16_];
		}

		if (avAudioPCMBufferInternalInt16_ == nullptr || avAudioPCMBuffer.frameCapacity > avAudioPCMBufferInternalInt16_.frameCapacity)
		{
			avAudioPCMBufferInternalInt16_ = [[AVAudioPCMBuffer alloc] initWithPCMFormat:avAudioFormatInternalInt16_ frameCapacity:avAudioPCMBuffer.frameCapacity];
		}

		if ([avAudioConverter_ convertToBuffer:avAudioPCMBufferInternalInt16_ fromBuffer:avAudioPCMBuffer error:nullptr] != YES)
		{
			Log::error() << "AVFMicrophone: Failed to convert buffer";
			return false;
		}

		const int16_t* const * data = avAudioPCMBufferInternalInt16_.int16ChannelData;
		ocean_assert(data != nullptr);
		const int16_t* monoData = data[0];

		sendSamples(sampleType, monoData, sizeof(int16_t) * avAudioPCMBufferInternalInt16_.frameLength);
	}
	else if (avAudioPCMBuffer.format.commonFormat == AVAudioPCMFormatInt16)
	{
		const int16_t* const * data = avAudioPCMBuffer.int16ChannelData;
		ocean_assert(data != nullptr);
		const int16_t* monoData = data[0];

		sendSamples(sampleType, monoData, sizeof(int16_t) * avAudioPCMBuffer.frameLength);
	}
	else
	{
		Log::debug() << "AVFMicrophone: The PCM buffer has wrong common format: " << int(avAudioPCMBuffer.format.commonFormat);
		return false;
	}

	return true;
}

}

}

}
