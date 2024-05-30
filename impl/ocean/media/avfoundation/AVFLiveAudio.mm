/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFLiveAudio.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#import <Foundation/Foundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFLiveAudio::AVFLiveAudio(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	SoundMedium(url),
	LiveAudio(url)
{
#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	AudioSessionManager::get().initialize(AVAudioSessionCategoryPlayAndRecord, AVAudioSessionModeVideoChat);
#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS

	avAudioEngine_ = [[AVAudioEngine alloc] init];

	avAudioPlayerNode_ = [[AVAudioPlayerNode alloc] init];
	[avAudioEngine_ attachNode:avAudioPlayerNode_];

	constexpr double sampleRate = 48000.0;
	constexpr AVAudioChannelCount channelCount = 1u;
	constexpr bool interleaved = false;

	avAudioFormat_ = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:sampleRate channels:channelCount interleaved:interleaved];

	avAudioMixerNode_ = [avAudioEngine_ mainMixerNode]; // creates the mixer node and connecting the node with the output node

	[avAudioEngine_ connect:avAudioPlayerNode_ to:avAudioMixerNode_ format:avAudioFormat_];

	if (avAudioEngine_.outputNode != nullptr)
	{
		[avAudioEngine_ prepare];

		isValid_ = true;
	}
	else
	{
		isValid_ = false;
	}
}

AVFLiveAudio::~AVFLiveAudio()
{
	stop();

	avAudioFormatInternalInt16_ = nullptr;
	avAudioPCMBufferInternalInt16_ = nullptr;
	avAudioConverter_ = nullptr;

	avAudioPlayerNode_ = nullptr;
	avAudioEngine_ = nullptr;
}

bool AVFLiveAudio::addSamples(const SampleType sampleType, const void* data, const size_t size)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayerNode_ != nullptr);

	if ([avAudioPlayerNode_ isPlaying] != YES)
	{
		return true;
	}

	ocean_assert(data != nullptr && size > 0);

	if (sampleType != ST_INTEGER_16_MONO_48)
	{
		return false;
	}

	if (size < sizeof(int16_t) || size % sizeof(int16_t) != 0)
	{
		return false;
	}

	const unsigned int numberSamples = (unsigned int)(size / sizeof(int16_t));

	if (avAudioConverter_ == nullptr)
	{
		ocean_assert(avAudioFormatInternalInt16_ == nullptr);

		constexpr double sampleRate = 48000.0;
		constexpr AVAudioChannelCount channelCount = 1u;
		constexpr bool interleaved = false;

		avAudioFormatInternalInt16_ = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatInt16 sampleRate:sampleRate channels:channelCount interleaved:interleaved];

		avAudioConverter_ = [[AVAudioConverter alloc] initFromFormat:avAudioFormatInternalInt16_ toFormat:avAudioFormat_];
	}

	if (avAudioPCMBufferInternalInt16_ == nullptr || numberSamples > avAudioPCMBufferInternalInt16_.frameCapacity)
	{
		avAudioPCMBufferInternalInt16_ = [[AVAudioPCMBuffer alloc] initWithPCMFormat:avAudioFormatInternalInt16_ frameCapacity:numberSamples];
	}

	avAudioPCMBufferInternalInt16_.frameLength = numberSamples;

	int16_t* const * bufferData = avAudioPCMBufferInternalInt16_.int16ChannelData;
	ocean_assert(bufferData != nullptr);
	int16_t* const monoBufferData = bufferData[0];

	memcpy(monoBufferData, data, size);

	AVAudioPCMBuffer* avAudioPCMBuffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:avAudioFormat_ frameCapacity:numberSamples];

	if ([avAudioConverter_ convertToBuffer:avAudioPCMBuffer fromBuffer:avAudioPCMBufferInternalInt16_ error:nullptr] != YES)
	{
		Log::error() << "AVFLiveAudio: Failed to convert buffer";
		return false;
	}

	needNewSamples_ = false;

	[avAudioPlayerNode_ scheduleBuffer:avAudioPCMBuffer completionCallbackType:AVAudioPlayerNodeCompletionDataConsumed completionHandler:^(AVAudioPlayerNodeCompletionCallbackType callbackType)
	{
		if (callbackType == AVAudioPlayerNodeCompletionDataConsumed)
		{
			needNewSamples_ = true;
		}
	}];

	return true;
}

bool AVFLiveAudio::needNewSamples() const
{
	return needNewSamples_;
}

float AVFLiveAudio::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	if (avAudioMixerNode_ == nullptr)
	{
		return -1.0f;
	}

	return [avAudioMixerNode_ outputVolume];
}

bool AVFLiveAudio::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	return previousVolume_ != -1.0f;
}

bool AVFLiveAudio::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	if (avAudioMixerNode_ == nullptr)
	{
		return false;
	}

	[avAudioMixerNode_ setOutputVolume:volume];

	return true;
}

bool AVFLiveAudio::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	const bool isMuted = previousVolume_ != -1.0f;

	if (isMuted == mute)
	{
		return true;
	}

	if (avAudioMixerNode_ == nullptr)
	{
		return false;
	}

	if (mute)
	{
		previousVolume_ = [avAudioMixerNode_ outputVolume];
		ocean_assert(previousVolume_ >= 0.0f && previousVolume_ <= 1.0f);

		[avAudioMixerNode_ setOutputVolume:0.0f];
	}
	else
	{
		ocean_assert(previousVolume_ >= 0.0f && previousVolume_ <= 1.0f);
		[avAudioMixerNode_ setOutputVolume:previousVolume_];

		previousVolume_ = -1.0f;
	}

	return true;
}

bool AVFLiveAudio::internalStart()
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

	@try
	{
		[avAudioPlayerNode_ play];
	}
	@catch (id anException)
	{
		Log::error() << "Failed to start AVFLiveAudio";
		return false;
	}

	return true;
}

bool AVFLiveAudio::internalPause()
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

	ocean_assert(avAudioPlayerNode_ != nullptr);
	[avAudioPlayerNode_ pause];

	ocean_assert(avAudioEngine_ != nullptr);
	[avAudioEngine_ pause];

	return true;
}

bool AVFLiveAudio::internalStop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	ocean_assert(avAudioPlayerNode_ != nullptr);
	[avAudioPlayerNode_ stop];

	ocean_assert(avAudioEngine_ != nullptr);
	[avAudioEngine_ stop];

	if (audioSessionStarted_)
	{
		AudioSessionManager::get().stop();
		audioSessionStarted_ = false;
	}

	return true;
}

}

}

}
