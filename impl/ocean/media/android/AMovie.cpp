/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/AMovie.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/media/android/ALibrary.h"
#include "ocean/media/android/NativeMediaLibrary.h"
#include "ocean/media/android/PixelFormats.h"
#include "ocean/media/android/VideoDecoder.h"

#include "ocean/io/File.h"

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaError.h>

#include <fcntl.h>
#include <sys/stat.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

AMovie::AMovie(const std::string& url) :
	Medium(url),
	AMedium(url),
	FiniteMedium(url),
	FrameMedium(url),
	SoundMedium(url),
	Movie(url)
{
	isValid_ = initializeMediaExtractor() && initializeMediaCodecs();
}

AMovie::~AMovie()
{
	stop();

	stopThreadExplicitly();

	release();
}

MediumRef AMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return ALibrary::newMovie(url_, true);
	}

	return MediumRef();
}

bool AMovie::start()
{
	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid())
	{
		startTimestamp_.toNow();
		pauseTimestamp_.toInvalid();

		return true;
	}

	if (startTimestamp_.isValid())
	{
		return true;
	}

	if (videoMediaCodec_ != nullptr)
	{
		NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

		media_status_t status = nativeMediaLibrary.AMediaCodec_start(videoMediaCodec_);

		if (status != AMEDIA_OK)
		{
			Log::error() << "Failed to start video decoder, reason: " << int(status);
			release();

			return false;
		}

		if (audioMediaCodec_ != nullptr)
		{
			status = nativeMediaLibrary.AMediaCodec_start(audioMediaCodec_);

			if (status != AMEDIA_OK)
			{
				Log::warning() << "Failed to start audio decoder, reason: " << int(status);
			}
		}

		startThread();

		startTimestamp_.toNow();

		return true;
	}

	return false;
}

bool AMovie::pause()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isInvalid())
	{
		return false;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();

	return true;
}

bool AMovie::stop()
{
	const ScopedLock scopedLock(lock_);

	if (pauseTimestamp_.isValid() || (startTimestamp_.isValid() && pause()))
	{
		ocean_assert(startTimestamp_.isInvalid());
		ocean_assert(stopTimestamp_.isInvalid());

		pauseTimestamp_.toInvalid();
		stopTimestamp_.toNow();

		ocean_assert(mediaExtractor_ != nullptr);

		const media_status_t status = NativeMediaLibrary::get().AMediaExtractor_seekTo(mediaExtractor_, 0, AMEDIAEXTRACTOR_SEEK_CLOSEST_SYNC);
		ocean_assert_and_suppress_unused(status == AMEDIA_OK, status);

		return true;
	}

	return startTimestamp_.isInvalid() && stopTimestamp_.isInvalid();
}

bool AMovie::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

double AMovie::duration() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(speed_ >= 0.0f);

	if (speed_ == 0.0f)
	{
		return 0.0;
	}

	return normalDuration() / double(speed_);
}

double AMovie::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	return normalDuration_;
}

double AMovie::position() const
{
	ocean_assert(false && "Missing implementation!");
	return -1.0f;
}

bool AMovie::setPosition(const double position)
{
	if (position < 0.0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	const int64_t seekPosUs = int64_t(position * 1000000.0 + 0.5);

	if (NativeMediaLibrary::get().AMediaExtractor_seekTo(mediaExtractor_, seekPosUs, AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC) == AMEDIA_OK)
	{
		return true;
	}

	return false;
}

float AMovie::speed() const
{
	return speed_;
}

bool AMovie::setSpeed(const float speed)
{
	if (speed < 0.0f)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return false;
	}

	speed_ = speed;

	return true;
}

float AMovie::soundVolume() const
{
	ocean_assert(false && "Missing implementation!");
	return 0.0f;
}

bool AMovie::soundMute() const
{
	ocean_assert(false && "Missing implementation!");
	return false;
}

bool AMovie::setSoundVolume(const float volume)
{
	ocean_assert(false && "Missing implementation!");
	return false;
}

bool AMovie::setSoundMute(const bool mute)
{
	ocean_assert(false && "Missing implementation!");
	return false;
}

bool AMovie::useSound() const
{
	const ScopedLock scopedLock(lock_);

	return useSound_;
}

bool AMovie::setUseSound(const bool state)
{
	const ScopedLock scopedLock(lock_);

	if (useSound_ == state)
	{
		return true;
	}

	if (state)
	{
		return false;
	}

	useSound_ = state;

	return true;
}

Timestamp AMovie::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_;
}

Timestamp AMovie::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return pauseTimestamp_;
}

Timestamp AMovie::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return stopTimestamp_;
}

void AMovie::release()
{
	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	if (videoMediaCodec_ != nullptr)
	{
		const media_status_t status = nativeMediaLibrary.AMediaCodec_delete(videoMediaCodec_);
		ocean_assert_and_suppress_unused(status == AMEDIA_OK, status);

		videoMediaCodec_ = nullptr;
	}

	if (audioMediaCodec_ != nullptr)
	{
		const media_status_t status = nativeMediaLibrary.AMediaCodec_delete(audioMediaCodec_);
		ocean_assert_and_suppress_unused(status == AMEDIA_OK, status);

		audioMediaCodec_ = nullptr;
	}

	if (mediaExtractor_ != nullptr)
	{
		const media_status_t status = nativeMediaLibrary.AMediaExtractor_delete(mediaExtractor_);
		ocean_assert_and_suppress_unused(status == AMEDIA_OK, status);

		mediaExtractor_ = nullptr;
	}

	fileDescriptor_.release();
}

void AMovie::threadRun()
{
	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	Timestamp presentationStartTimestamp(true);

	std::queue<double> normalPresentationTimeQueue;

	bool stopOnceQueueIsEmpty = false;

	while (!shouldThreadStop())
	{
		ocean_assert(videoMediaCodec_ != nullptr);

		{
			TemporaryScopedLock scopedLock(lock_);

			while (pauseTimestamp_.isValid() && shouldThreadStop() == false)
			{
				scopedLock.release();

				sleep(1u);

				scopedLock.relock(lock_);
			}
		}

		// first, we process a new input buffer

		TemporaryScopedLock scopedLock(lock_);

		double normalRelativePresentationTime = -1.0;
		if (!stopOnceQueueIsEmpty && !processInputBuffer(videoMediaCodec_, normalRelativePresentationTime))
		{
			// we reached the end of the stream
			// however, we still may have some pending frames to process

			stopOnceQueueIsEmpty = true;
		}

		/*if (audioMediaCodec_ != nullptr) **TODO** not yet activated
		{
			double presentationTime = -1.0;
			processInputBuffer(audioMediaCodec_, presentationTime);
		}*/

		ocean_assert(speed_ >= 0.0f);
		const double invSpeed = NumericD::ratio(1.0, double(speed_), 0.0); // 0.0 in case the media content is supposed to be delivered as fast as possible

		if (normalRelativePresentationTime >= 0.0)
		{
			normalPresentationTimeQueue.emplace(normalRelativePresentationTime);
		}

		if (!normalPresentationTimeQueue.empty())
		{
			// now, we process the output buffer

			ocean_assert(videoMediaCodec_ != nullptr);
			Frame frame = VideoDecoder::extractVideoFrameFromCodecOutputBuffer(videoMediaCodec_);

			/*if (audioMediaCodec_ != nullptr) **TODO** not yet activated
			{
				extractAudioSamplesFromCodecOutputBuffer(audioMediaCodec_);
			}*/

			if (frame.isValid())
			{
				scopedLock.release();

				normalRelativePresentationTime = normalPresentationTimeQueue.front();
				normalPresentationTimeQueue.pop();

				// we need to wait until we reach the presentation time

				while (invSpeed != 0.0)
				{
					const double relativePresentationTime = normalRelativePresentationTime * invSpeed;

					const double waitTime = double(presentationStartTimestamp) + relativePresentationTime - double(Timestamp(true));

					if (waitTime < 0.001 || shouldThreadStop())
					{
						break;
					}

					sleep(1u);
				}

				frame.setRelativeTimestamp(Timestamp(normalRelativePresentationTime));

				deliverNewFrame(std::move(frame));
			}
		}

		if (stopOnceQueueIsEmpty && normalPresentationTimeQueue.empty())
		{
			scopedLock.relock(lock_);

			// we have reached the end of the stream, and we have processed all frames

			if (nativeMediaLibrary.AMediaCodec_stop(videoMediaCodec_) != AMEDIA_OK)
			{
				Log::error() << "Failed to stop video decoder";
			}

			if (audioMediaCodec_ != nullptr)
			{
				if (nativeMediaLibrary.AMediaCodec_stop(audioMediaCodec_) != AMEDIA_OK)
				{
					Log::error() << "Failed to stop audio decoder";
				}
			}

			if (loop_)
			{
				// we need to re-setup the entire media pipeline

				release();

				if (initializeMediaExtractor() && initializeMediaCodecs())
				{
					media_status_t status = nativeMediaLibrary.AMediaCodec_start(videoMediaCodec_);

					if (status != AMEDIA_OK)
					{
						Log::error() << "Failed to re-start video decoder, reason: " << int(status);
						release();

						return;
					}

					if (audioMediaCodec_ != nullptr)
					{
						status = nativeMediaLibrary.AMediaCodec_start(audioMediaCodec_);

						if (status != AMEDIA_OK)
						{
							Log::warning() << "Failed to re-start audio decoder, reason: " << int(status);
						}
					}

					presentationStartTimestamp.toNow();

					stopOnceQueueIsEmpty = false;

					continue;
				}
			}

			startTimestamp_.toInvalid();
			stopTimestamp_.toNow();

			break;
		}
	}
}

bool AMovie::initializeMediaExtractor()
{
	const ScopedLock scopedLock(lock_);

	if (mediaExtractor_ != nullptr)
	{
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	if (nativeMediaLibrary.isInitialized())
	{
		mediaExtractor_ = nativeMediaLibrary.AMediaExtractor_new();

		if (mediaExtractor_ != nullptr && !url_.empty())
		{
			media_status_t status = AMEDIA_ERROR_BASE;

			const IO::File file(url_);

			if (file.exists())
			{
				// due to a bug in Android 10, we cannot use 'AMediaExtractor_setDataSource' for local files, therefore, using the ...fd() function instead

				fileDescriptor_ = ScopedFileDescriptor(open(url_.c_str(), O_RDONLY));

				if (fileDescriptor_.isValid())
				{
					const off64_t fileLength = lseek64(*fileDescriptor_, 0, SEEK_END);
					lseek64(*fileDescriptor_, 0, SEEK_SET);

					status = nativeMediaLibrary.AMediaExtractor_setDataSourceFd(mediaExtractor_, *fileDescriptor_, 0, fileLength);
				}
				else
				{
					Log::error() << "AMovie: Failed to open file descriptor";
				}
			}
			else
			{
				Log::debug() << "AMovie: Movie URL does not exist, trying to load via HTTP";

				status = nativeMediaLibrary.AMediaExtractor_setDataSource(mediaExtractor_, url_.c_str());
			}

			if (status != AMEDIA_OK || nativeMediaLibrary.AMediaExtractor_getTrackCount(mediaExtractor_) == 0)
			{
				Log::error() << "AMovie: Failed to load movie '" << url_ << "', error: " << int(status);

				release();
			}
		}
		else
		{
			ocean_assert(false && "Invalid media extractor!");
		}
	}

	return mediaExtractor_ != nullptr;
}

bool AMovie::initializeMediaCodecs()
{
	const ScopedLock scopedLock(lock_);

	if (mediaExtractor_ == nullptr || videoMediaCodec_ != nullptr)
	{
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	const size_t numberTracks = nativeMediaLibrary.AMediaExtractor_getTrackCount(mediaExtractor_);

	for (size_t nTrack = 0; nTrack < numberTracks; ++nTrack)
	{
		AMediaFormat* trackFormat = nativeMediaLibrary.AMediaExtractor_getTrackFormat(mediaExtractor_, nTrack);
		ocean_assert(trackFormat != nullptr);

		const char* mime = nullptr;
		nativeMediaLibrary.AMediaFormat_getString(trackFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_MIME, &mime);

		if (mime != nullptr)
		{
			if (videoMediaCodec_ == nullptr && std::string(mime).find("video/") == 0)
			{
				videoMediaCodec_ = createCodecForTrack(trackFormat, mime, nTrack);

				if (videoMediaCodec_ != nullptr)
				{
					int64_t durationUs = 0;
					nativeMediaLibrary.AMediaFormat_getInt64(trackFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_DURATION, &durationUs);

					normalDuration_ = Timestamp::microseconds2seconds(durationUs);

					Log::debug() << "AMovie: Video track format:";
					Log::debug() << NativeMediaLibrary::get().AMediaFormat_toString(trackFormat);
				}
			}

			if (useSound_ && audioMediaCodec_ == nullptr && std::string(mime).find("audio/") == 0)
			{
				audioMediaCodec_ = createCodecForTrack(trackFormat, mime, nTrack);
			}
		}

		nativeMediaLibrary.AMediaFormat_delete(trackFormat);

		if (videoMediaCodec_ != nullptr && (!useSound_ || audioMediaCodec_ != nullptr))
		{
			break;
		}
	}

	return videoMediaCodec_ != nullptr;
}

AMediaCodec* AMovie::createCodecForTrack(AMediaFormat* trackFormat, const char* mime, const size_t trackIndex)
{
	ocean_assert(trackFormat != nullptr);
	ocean_assert(mime != nullptr);

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	if (nativeMediaLibrary.AMediaExtractor_selectTrack(mediaExtractor_, trackIndex) == AMEDIA_OK)
	{
		AMediaCodec* mediaCodec = nativeMediaLibrary.AMediaCodec_createDecoderByType(mime);

		if (mediaCodec != nullptr)
		{
			media_status_t status = nativeMediaLibrary.AMediaCodec_configure(mediaCodec, trackFormat, nullptr, nullptr, 0);

			if (status == AMEDIA_OK)
			{

#ifdef OCEAN_DEBUG
				{
					AMediaFormat* mediaFormat = NativeMediaLibrary::get().AMediaCodec_getInputFormat(mediaCodec);

					if (mediaFormat != nullptr)
					{
						const char* readableFormat = NativeMediaLibrary::get().AMediaFormat_toString(mediaFormat);

						if (readableFormat != nullptr)
						{
							Log::debug() << "AMovie: Accepted input media format: " << readableFormat;
						}

						NativeMediaLibrary::get().AMediaFormat_delete(mediaFormat);
						mediaFormat = nullptr;
					}

					mediaFormat = NativeMediaLibrary::get().AMediaCodec_getOutputFormat(mediaCodec);

					if (mediaFormat != nullptr)
					{
						const char* readableFormat = NativeMediaLibrary::get().AMediaFormat_toString(mediaFormat);

						if (readableFormat != nullptr)
						{
							Log::debug() << "AMovie: Accepted output media format: " << readableFormat;
						}

						NativeMediaLibrary::get().AMediaFormat_delete(mediaFormat);
						mediaFormat = nullptr;
					}
				}
#endif

				return mediaCodec;
			}
			else
			{
				Log::error() << "Failed to configure decoder for '" << mime << "', reason: " << int(status);
			}

			nativeMediaLibrary.AMediaCodec_delete(mediaCodec);
		}
		else
		{
			Log::error() << "Failed to create decoder '" << mime;
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	return nullptr;
}

bool AMovie::processInputBuffer(AMediaCodec* const mediaCodec, double& normalRelativePresentationTime)
{
	ocean_assert(mediaCodec != nullptr);

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	constexpr int64_t timeoutUs = 2000; // 2ms
	const ssize_t inputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueInputBuffer(mediaCodec, timeoutUs);

	bool endOfInputStream = false;

	if (inputBufferIndex >= 0)
	{
		size_t bufferSize;
		uint8_t* buffer = nativeMediaLibrary.AMediaCodec_getInputBuffer(mediaCodec, inputBufferIndex, &bufferSize);
		ssize_t sampleSize = nativeMediaLibrary.AMediaExtractor_readSampleData(mediaExtractor_, buffer, bufferSize);

		const int64_t presentationTimeUs = nativeMediaLibrary.AMediaExtractor_getSampleTime(mediaExtractor_);

		if (sampleSize <= 0)
		{
			endOfInputStream = true;
			sampleSize = 0;
		}
		else
		{
			ocean_assert(presentationTimeUs >= 0);

			normalRelativePresentationTime = Timestamp::microseconds2seconds(presentationTimeUs);
		}

		const uint32_t flags = endOfInputStream ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM : 0;

		const media_status_t status = nativeMediaLibrary.AMediaCodec_queueInputBuffer(mediaCodec, inputBufferIndex, 0, sampleSize, presentationTimeUs, flags);
		ocean_assert_and_suppress_unused(status == AMEDIA_OK, status);

		nativeMediaLibrary.AMediaExtractor_advance(mediaExtractor_);
	}

	return !endOfInputStream;
}

bool AMovie::extractAudioSamplesFromCodecOutputBuffer(AMediaCodec* const mediaCodec)
{
	ocean_assert(mediaCodec != nullptr);

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	AMediaCodecBufferInfo codecBufferInfo;
	const ssize_t outputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueOutputBuffer(mediaCodec, &codecBufferInfo, 0);

	if (outputBufferIndex < 0)
	{
		// no output buffer yet
		return false;
	}

	bool result = false;

	AMediaFormat* outputMediaFormat = nativeMediaLibrary.AMediaCodec_getOutputFormat(mediaCodec);

	if (outputMediaFormat != nullptr)
	{
		int32_t channelCount = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_CHANNEL_COUNT, &channelCount);

		int32_t sampleRate = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_SAMPLE_RATE, &sampleRate);

		int32_t bitsPerSample = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_BITS_PER_SAMPLE, &bitsPerSample);

		int32_t pcmEncoding = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_PCM_ENCODING, &pcmEncoding);

		Log::info() << "Channel count: " << channelCount; // needs further investigations
		Log::info() << "Sample rate: " << sampleRate;
		Log::info() << "Bits per sample: " << bitsPerSample;
		Log::info() << "PCM encoding: " << pcmEncoding;

		size_t outputBufferSize = 0;
		uint8_t* outputBuffer = nativeMediaLibrary.AMediaCodec_getOutputBuffer(mediaCodec, outputBufferIndex, &outputBufferSize);

		if (outputBuffer != nullptr)
		{
			Log::info() << "Output buffer size " << outputBufferSize;
		}

		nativeMediaLibrary.AMediaFormat_delete(outputMediaFormat);
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	nativeMediaLibrary.AMediaCodec_releaseOutputBuffer(mediaCodec, size_t(outputBufferIndex), false /*render*/);

	return result;
}

} // namespace Android

} // namespace Media

} // namespace Ocean

#endif // __ANDROID_API__
