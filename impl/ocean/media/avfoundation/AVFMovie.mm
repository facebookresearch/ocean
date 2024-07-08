/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFMovie.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/StringApple.h"

#include "ocean/system/Performance.h"

#include <Foundation/Foundation.h>

/**
 * Definition of an AVPlayer observer.
 */
@interface AVFMovieObserver: NSObject
{
	/// The callback function for finished playing
	@private Ocean::Media::AVFoundation::AVFMovie::FinishedPlayingCallback observerFinishedPlayingCallback;
}

/**
 * Initialize the observer by a given callback function for finished playing.
 * @param finishedPlayingCallback Callback function for finished playing
 * @return The instance of the observer
 */
- (id) initWithCallback:(Ocean::Media::AVFoundation::AVFMovie::FinishedPlayingCallback)finishedPlayingCallback;

/**
 * Event function for finish playing.
 * Called, if the AVPlayer has finished playing.
 * @param notification The item that finished playing.
 */
- (void) itemDidFinishPlaying:(NSNotification *) notification;

@end

@implementation AVFMovieObserver

- (id)initWithCallback:(Ocean::Media::AVFoundation::AVFMovie::FinishedPlayingCallback)finishedPlayingCallback
{
	if (self = [super init])
	{
		observerFinishedPlayingCallback = finishedPlayingCallback;
	}

	return self;
}

-(void)itemDidFinishPlaying:(NSNotification *) notification
{
	observerFinishedPlayingCallback();
}

@end

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFMovie::AVFMovie(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	FrameMedium(url),
	AVFFrameMedium(url),
	FiniteMedium(url),
	SoundMedium(url),
	Movie(url)
{
	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(url_)];

	NSDictionary* assetOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
	asset_ = [AVURLAsset URLAssetWithURL:nsUrl options:assetOptions];

	if (asset_ && createNewAssetReader(0.0) && createNewPlayer())
	{
		isValid_ = true;
		startThread();
	}
}

AVFMovie::~AVFMovie()
{
	stopThreadExplicitly();

	frameCollection_.clear();

	[[NSNotificationCenter defaultCenter] removeObserver:observer_ name:AVPlayerItemDidPlayToEndTimeNotification object:playerItem_];

	asset_ = nullptr;
	assetReader_ = nullptr;
	assetReaderTrackOutput_ = nullptr;
	player_ = nullptr;
	playerItem_ = nullptr;
	playerItemVideoOutput_ = nullptr;
	observer_ = nullptr;
}

bool AVFMovie::createNewAssetReader(const double startTime)
{
	ocean_assert(asset_ != nullptr);
	ocean_assert(assetReader_ == nullptr && assetReaderTrackOutput_ == nullptr);

	if (!asset_.readable || startTime >= normalDuration())
	{
		return false;
	}

	NSArray* tracks = [asset_ tracksWithMediaType:AVMediaTypeVideo];

	if ([tracks count] != 1u)
	{
		return false;
	}

	AVAssetTrack* videoTrack = [tracks objectAtIndex:0];

	NSError* errorValue;
	assetReader_ = [[AVAssetReader alloc] initWithAsset:asset_ error:&errorValue];

	if (errorValue)
	{
		assetReader_ = nullptr;
		return false;
	}

	CMTime seekTime = CMTimeMakeWithSeconds(startTime, 1000000);
	CMTimeRange timeRange = CMTimeRangeMake(seekTime, kCMTimeIndefinite);

	assetReader_.timeRange = timeRange;

	// **TODO** selecting optimal pixel format for individual platforms
	// https://developer.apple.com/documentation/avfoundation/avassetreadertrackoutput?language=objc

	NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
	NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key];

	assetReaderTrackOutput_ = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:videoSettings];

	if (![assetReader_ canAddOutput:assetReaderTrackOutput_])
	{
		assetReader_ = nullptr;
		assetReaderTrackOutput_ = nullptr;

		return false;
	}

	[assetReader_ addOutput:assetReaderTrackOutput_];

	return true;
}

bool AVFMovie::createNewPlayer()
{
	ocean_assert(asset_);

	NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
	NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key];

	playerItemVideoOutput_ = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:videoSettings];

	playerItem_ = [AVPlayerItem playerItemWithAsset: asset_];

	FinishedPlayingCallback callback(*this, &AVFMovie::onFinishedPlaying);

	observer_ = [[AVFMovieObserver alloc] initWithCallback:callback];

	[[NSNotificationCenter defaultCenter] addObserver:observer_ selector:@selector(itemDidFinishPlaying:) name:AVPlayerItemDidPlayToEndTimeNotification object:playerItem_];

	player_ = [AVPlayer playerWithPlayerItem: playerItem_];

	[player_.currentItem addOutput:playerItemVideoOutput_];

	return true;
}

MediumRef AVFMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return AVFLibrary::newMovie(url_, true);
	}

	return MediumRef();
}

bool AVFMovie::internalStart()
{
	const ScopedLock scopedLock(lock_);

	if (speed_ > 0.0f)
	{
		playerShouldStart_ = true;
		return true;
	}

	// the movie is supposed to deliver the media content as fast as possible

	if ([assetReader_ status] == AVAssetReaderStatusUnknown || [assetReader_ status] == AVAssetReaderStatusCancelled)
	{
		[assetReader_ startReading];
	}

	return [assetReader_ status] == AVAssetReaderStatusReading;
}

bool AVFMovie::internalPause()
{
	if (pauseTimestamp_.isValid())
	{
		return true;
	}

	if (!startTimestamp_.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (speed_ > 0.0f)
	{
		[player_ setRate:0.0f];
	}

	return true;
}

bool AVFMovie::internalStop()
{
	if (stopTimestamp_.isValid())
	{
		return true;
	}

	const ScopedLock scopedLock(lock_);

	if (speed_ > 0.0f)
	{
		[player_ setRate:0.0f];
	}
	else
	{
		sleep(200); // known issue: apple race condition

		[assetReader_ cancelReading];
	}

	setPosition(0.0);

	return true;
}

void AVFMovie::threadRun()
{
	RandomI::initialize();

	bool maySleep;

	while (shouldThreadStop() == false)
	{
		maySleep = true;

		if (speed_ > 0.0f)
		{
			// the movie is supposed to respect the playback time

			TemporaryScopedLock temporaryScopedLock(lock_);

			if (playerShouldStart_ && [playerItem_ status] == AVPlayerItemStatusReadyToPlay && [player_ status] == AVPlayerStatusReadyToPlay)
			{
				[player_ setRate:speed_];

				playerShouldStart_ = false;
			}

			if ([player_ rate] > 0.0f)
			{
				CMTime currentTime = [playerItem_ currentTime];

				// 'currentTime' provides a relative timestamp in relation to the duration/length of the movie in seconds (with playback speed 1.0x)
				// Thus, 'currentTime' will be within the range [0, movieDuration_for_speed_1x]

				if ([playerItemVideoOutput_ hasNewPixelBufferForItemTime: currentTime])
				{
					const ScopedCVPixelBufferRef pixelBuffer([playerItemVideoOutput_ copyPixelBufferForItemTime: currentTime itemTimeForDisplay: nil]);

					if (pixelBuffer.object() != nullptr)
					{
						ocean_assert(currentTime.timescale != 0);
						const double unixtime = double([[NSDate date] timeIntervalSince1970]);

						temporaryScopedLock.release();

						onNewSample(pixelBuffer.object(), SharedAnyCamera(), unixtime, double(currentTime.value) / double(currentTime.timescale));
					}
				}
			}
		}
		else
		{
			// the movie is supposed to deliver the media content as fast as possible

			ocean_assert(speed_ == 0.0f);

			TemporaryScopedLock temporaryScopedLock(lock_);

			if (startTimestamp_.isValid())
			{
				const AVAssetReaderStatus status = [assetReader_ status];

				if (status == AVAssetReaderStatusFailed)
				{
					NSError* error = [assetReader_ error];

					const std::string errorDescription = StringApple::toUTF8(error.localizedDescription);
					const std::string errorReason = StringApple::toUTF8(error.localizedFailureReason);

					Log::error() << "AVFMovie: Failed to decode frame: " << error.code << ", " << errorDescription << ", " << errorReason;

					// **TODO** add some kind of error state to `Media::Medium` objects to allow catching an error inside

					onFinishedPlaying();
				}
				else if (status == AVAssetReaderStatusReading)
				{
					const ScopedCMSampleBufferRef sampleBuffer([assetReaderTrackOutput_ copyNextSampleBuffer]);

					if (sampleBuffer.object() != nullptr)
					{
						const CMTime presentationTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer.object());

						const CVPixelBufferRef frameBuffer = CMSampleBufferGetImageBuffer(sampleBuffer.object());
						const ScopedCVPixelBufferRef pixelBuffer(CVBufferRetain(frameBuffer));

						if (pixelBuffer.object())
						{
							// 'presentationTime' provides a relative timestamp in relation to the duration/length of the movie in seconds (with playback speed 1.0x)
							// Thus, 'presentationTime' will be within the range [0, movieDuration_for_speed_1x]

							ocean_assert(presentationTime.timescale != 0);
							const double unixtime = double([[NSDate date] timeIntervalSince1970]);

							temporaryScopedLock.release();

							onNewSample(pixelBuffer.object(), SharedAnyCamera(), unixtime, double(presentationTime.value) / double(presentationTime.timescale));
						}

						CMSampleBufferInvalidate(sampleBuffer.object());
					}

					const ScopedLock scopedLock(lock_);

					if ([assetReader_ status] == AVAssetReaderStatusCompleted)
					{
						onFinishedPlaying();
					}
					else
					{
						maySleep = false;
					}
				}
			}
		}

		if (maySleep)
		{
			sleep(1u);
		}
	}
}

void AVFMovie::onFinishedPlaying()
{
	if (loop_)
	{
		assetReader_ = nullptr;
		assetReaderTrackOutput_ = nullptr;

		if (createNewAssetReader(0.0))
		{
			internalStart();
		}
	}
	else
	{
		stop();
	}
}

double AVFMovie::duration() const
{
	const ScopedLock scopedLock(lock_);

	const float currentSpeed = speed();

	if (currentSpeed == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(currentSpeed);
}

double AVFMovie::normalDuration() const
{
	const CMTime duration = [asset_ duration];

	return double(duration.value) / double(duration.timescale);
}

double AVFMovie::position() const
{
	const ScopedLock scopedLock(lock_);

	const CMTime currentTime = [player_ currentTime];

	return double(currentTime.value) / double(currentTime.timescale);
}

float AVFMovie::speed() const
{
	return speed_;
}

float AVFMovie::soundVolume() const
{
	return 0.0f;
}

bool AVFMovie::soundMute() const
{
	return true;
}

bool AVFMovie::setSoundVolume(const float volume)
{
	return false;
}

bool AVFMovie::setSoundMute(const bool mute)
{
	return false;
}

bool AVFMovie::setUseSound(const bool state)
{
	return !state;
}

bool AVFMovie::setPosition(const double position)
{
	if (position > duration())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	assetReader_ = nullptr;
	assetReaderTrackOutput_ = nullptr;

	createNewAssetReader(position);

	const CMTime seekTime = CMTimeMakeWithSeconds(position, 1000000);

	[player_ seekToTime:seekTime toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];

	return true;
}

bool AVFMovie::setSpeed(const float speed)
{
	if (speed < 0.0f)
	{
		ocean_assert(false && "Invalid speed");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (speed_ == speed)
	{
		return true;
	}

	if (speed == 0.0f || (speed_ == 0.0f && speed > 0.0f))
	{
		// the caller wants to change respect-playback-time behavior, either active it or deactivate it

		if (startTimestamp_.isValid())
		{
			return false;
		}

		if (speed == 0.0f)
		{
			speed_ = 0.0f;

			return true;
		}

		// we need to respect the playback time, and we have to set the correct speed
	}

	ocean_assert(speed_ > 0.0f && speed > 0.0f);

	if (startTimestamp_.isValid())
	{
		[player_ setRate:speed];
	}

	speed_ = speed;

	return true;
}

}

}

}
