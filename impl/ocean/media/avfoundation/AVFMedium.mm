/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFMedium.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#import <Foundation/Foundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS

void AVFMedium::AudioSessionManager::initialize(const AVAudioSessionCategory avAudioSessionCategory, const AVAudioSessionMode avAudioSessionMode)
{
	const ScopedLock scopedLock(lock_);

	if (avAudioSession_ == nullptr)
	{
		avAudioSession_ = [AVAudioSession sharedInstance];

		NSError* nsError = nullptr;
		if ([avAudioSession_ setCategory:avAudioSessionCategory error:&nsError] == YES)
		{
			avAudioSessionCategory_ = avAudioSessionCategory;
		}
		else
		{
			Log::warning() << "Failed to set category in AVAudioSession: " << nsError.code;
		}

		nsError = nullptr;
		if ([avAudioSession_ setMode:avAudioSessionMode error:&nsError] == YES)
		{
			avAudioSessionMode_ = avAudioSessionMode;
		}
		else
		{
			Log::warning() << "Failed to set mode in AVAudioSession: " << nsError.code;
		}
	}
	else
	{
		if (avAudioSessionCategory_ != avAudioSessionCategory)
		{
			Log::warning() << "Audio session is already using category " << size_t(avAudioSessionCategory);
		}

		if (avAudioSessionMode_ != avAudioSessionMode)
		{
			Log::warning() << "Audio session is already using mode " << size_t(avAudioSessionMode_);
		}
	}
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS

bool AVFMedium::AudioSessionManager::start()
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS

	return true;

#else

	const ScopedLock scopedLock(lock_);

	if (avAudioSession_ == nullptr)
	{
		initialize(avAudioSessionCategory_, avAudioSessionMode_);
	}

	ocean_assert(avAudioSession_ != nullptr);

	NSError* nsError = nullptr;

	if ([avAudioSession_ setActive:true error:&nsError] == YES)
	{
		++usageCounter_;
		return true;
	}

	Log::error() << "Failed to start AVAudioSession: " << nsError.code;

	return false;

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS
}

void AVFMedium::AudioSessionManager::stop()
{
#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS

	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioSession_ != nullptr);

	if (avAudioSession_ != nullptr)
	{
		ocean_assert(usageCounter_ != 0u);

		if (--usageCounter_ == 0u)
		{
			NSError* nsError = nullptr;

			if ([avAudioSession_ setActive:false error:&nsError] != YES)
			{
				Log::warning() << "Failed to disable AVAudioSession: " << nsError.code;
			}

			avAudioSession_ = nullptr;
		}
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS
}

void AVFMedium::AudioSessionManager::requestRecordPermission()
{
#ifndef OCEAN_PLATFORM_BUILD_APPLE_MACOS

	const ScopedLock scopedLock(lock_);

	if (avAudioSession_ != nullptr)
	{
		[avAudioSession_ requestRecordPermission:^(BOOL granted) {
			if (granted)
			{
				Log::debug() << "User granted access to audio recording";
			}
			else
			{
				Log::warning() << "User did not grant access to audio recording";
			}
		}];
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS
}

AVFMedium::AVFMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameAVFLibrary();
}

AVFMedium::~AVFMedium()
{
	// nothing to do here
}

bool AVFMedium::isStarted() const
{
	return startTimestamp_.isValid();
}

Timestamp AVFMedium::startTimestamp() const
{
	return startTimestamp_;
}

Timestamp AVFMedium::pauseTimestamp() const
{
	return pauseTimestamp_;
}

Timestamp AVFMedium::stopTimestamp() const
{
	return stopTimestamp_;
}

bool AVFMedium::start()
{
	if (startTimestamp_.isInvalid())
	{
		if (!internalStart())
		{
			return false;
		}
	}

	const ScopedLock scopedLock(lock_);

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	return true;
}

bool AVFMedium::pause()
{
	if (!internalPause())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	return true;
}

bool AVFMedium::stop()
{
	if (!internalStop())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

}

}

}
