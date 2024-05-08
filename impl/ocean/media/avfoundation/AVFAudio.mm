/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFAudio.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#include "ocean/base/StringApple.h"

#import <Foundation/Foundation.h>

using namespace Ocean;

/**
 * Definition of a sample buffer delegate object.
 */
@interface OceanMediaAVFAudioAVAudioPlayerDelegate()
{
	/// The callback function for finished playing events.
	@private Media::AVFoundation::AVFAudio::FinishedPlayingCallback finishedPlayingCallback_;
}

@end

@implementation OceanMediaAVFAudioAVAudioPlayerDelegate

- (id)initWithCallback:(Media::AVFoundation::AVFAudio::FinishedPlayingCallback&&)callback
{
	if (self = [super init])
	{
		finishedPlayingCallback_ = std::move(callback);
		ocean_assert(finishedPlayingCallback_);
  	}

	return self;
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
	ocean_assert(finishedPlayingCallback_);
	if (finishedPlayingCallback_)
	{
		finishedPlayingCallback_();
	}
}

@end

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFAudio::AVFAudio(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	FiniteMedium(url),
	SoundMedium(url),
	Audio(url)
{
	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(url_)];

	NSError* nsError = nullptr;
	avAudioPlayer_ = [[AVAudioPlayer alloc] initWithContentsOfURL:nsUrl error:&nsError];

	isValid_ = false;

	if (avAudioPlayer_ != nullptr)
	{
		oceanMediaAVFAudioAVAudioPlayerDelegate_ = [[OceanMediaAVFAudioAVAudioPlayerDelegate alloc] initWithCallback:FinishedPlayingCallback::create(*this, &AVFAudio::onFinishedPlaying)];
		avAudioPlayer_.delegate = oceanMediaAVFAudioAVAudioPlayerDelegate_;

		if ([avAudioPlayer_ prepareToPlay] == TRUE)
		{
			isValid_ = true;
		}
	}
}

AVFAudio::~AVFAudio()
{
	oceanMediaAVFAudioAVAudioPlayerDelegate_ = nullptr;

	if (avAudioPlayer_ != nullptr)
	{
		[avAudioPlayer_ stop];

		avAudioPlayer_ = nullptr;
	}
}

MediumRef AVFAudio::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return AVFLibrary::newAudio(url_, true);
	}

	return MediumRef();
}

double AVFAudio::duration() const
{
	const ScopedLock scopedLock(lock_);

	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double AVFAudio::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	return [avAudioPlayer_ duration];
}

double AVFAudio::position() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	return avAudioPlayer_.currentTime;
}

float AVFAudio::speed() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	return avAudioPlayer_.rate;
}

float AVFAudio::soundVolume() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	return avAudioPlayer_.volume;
}

bool AVFAudio::soundMute() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	return avAudioPlayer_.volume == 0.0f;
}

bool AVFAudio::setSoundVolume(const float volume)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	avAudioPlayer_.volume = volume;
	volumeBeforeMute_ = -1.0f;

	return true;
}

bool AVFAudio::setSoundMute(const bool mute)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(avAudioPlayer_ != nullptr);

	if (mute)
	{
		volumeBeforeMute_ = avAudioPlayer_.volume;
		avAudioPlayer_.volume = 0.0f;
	}
	else
	{
		if (volumeBeforeMute_ >= 0.0f)
		{
			avAudioPlayer_.volume = volumeBeforeMute_;
			volumeBeforeMute_ = -1.0f;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool AVFAudio::setPosition(const double position)
{
	if (position < 0.0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (position > duration())
	{
		return false;
	}

	ocean_assert(avAudioPlayer_ != nullptr);

	avAudioPlayer_.currentTime = position;

	return true;
}

bool AVFAudio::setSpeed(const float speed)
{
	if (speed <= 0.0f)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (NumericF::isEqual(avAudioPlayer_.rate, speed))
	{
		return true;
	}

	ocean_assert(avAudioPlayer_ != nullptr);

	if ([avAudioPlayer_ enableRate] == FALSE)
	{
		return false;
	}

	avAudioPlayer_.rate = speed;

	return true;
}

bool AVFAudio::setLoop(bool value)
{
	if (FiniteMedium::setLoop(value))
	{
		if (!value)
		{
			avAudioPlayer_.numberOfLoops = 0;
		}

		return true;
	}

	return false;
}

bool AVFAudio::internalStart()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);

	ocean_assert(avAudioPlayer_ != nullptr);

	if (loop_)
	{
		avAudioPlayer_.numberOfLoops = -1;
	}
	else
	{
		avAudioPlayer_.numberOfLoops = 0;
	}

	return [avAudioPlayer_ play] == TRUE;
}

bool AVFAudio::internalPause()
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

	ocean_assert(avAudioPlayer_ != nullptr);

	[avAudioPlayer_ pause];

	return true;
}

bool AVFAudio::internalStop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	ocean_assert(avAudioPlayer_ != nullptr);

	[avAudioPlayer_ stop];

	return true;
}

void AVFAudio::onFinishedPlaying()
{
	stop();

	if (loop_)
	{
		start();
	}
}

}

}

}
