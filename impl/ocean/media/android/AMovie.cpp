// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/android/AMovie.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/media/android/ALibrary.h"
#include "ocean/media/android/NativeMediaLibrary.h"
#include "ocean/media/android/PixelFormats.h"

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

	ocean_assert(speed_ > 0.0);
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
	const ScopedLock scopedLock(lock_);

	return speed_;
}

bool AMovie::setSpeed(const float speed)
{
	if (speed <= 0.0f)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

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

bool AMovie::respectPlaybackTime() const
{
	return respectPlaybackTime_.load();
}

bool AMovie::setRespectPlaybackTime(const bool state)
{
	respectPlaybackTime_ = true;

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

		ocean_assert(speed_ > 0.0);
		const double invSpeed = 1.0 / speed_;

		if (normalRelativePresentationTime >= 0.0)
		{
			normalPresentationTimeQueue.emplace(normalRelativePresentationTime);
		}

		if (!normalPresentationTimeQueue.empty())
		{
			// now, we process the output buffer

			ocean_assert(videoMediaCodec_ != nullptr);
			Frame frame = extractVideoFrameFromCodecOutputBuffer(videoMediaCodec_);

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

				while (respectPlaybackTime_.load())
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
					const off64_t fileLength = lseek64(fileDescriptor_.object(), 0, SEEK_END);
					lseek64(fileDescriptor_.object(), 0, SEEK_SET);

					status = nativeMediaLibrary.AMediaExtractor_setDataSourceFd(mediaExtractor_, fileDescriptor_.object(), 0, fileLength);
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

			ocean_assert(speed_ > 0.0f);

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

Frame AMovie::extractVideoFrameFromCodecOutputBuffer(AMediaCodec* const mediaCodec)
{
	ocean_assert(mediaCodec != nullptr);

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	AMediaCodecBufferInfo codecBufferInfo;
	const ssize_t outputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueOutputBuffer(mediaCodec, &codecBufferInfo, 0);

	if (outputBufferIndex < 0)
	{
		// no output buffer yet
		return Frame();
	}

	const Timestamp relativeTimestamp(Timestamp::microseconds2seconds(codecBufferInfo.presentationTimeUs));

	Frame frame;

	AMediaFormat* outputMediaFormat = nativeMediaLibrary.AMediaCodec_getOutputFormat(mediaCodec);

	if (outputMediaFormat != nullptr)
	{
#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << "Output buffer format for current sample:";
		Log::debug() << NativeMediaLibrary::get().AMediaFormat_toString(outputMediaFormat);
#endif

		int32_t width = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_WIDTH, &width);

		int32_t height = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_HEIGHT, &height);

		if (width <= 0 || height <= 0)
		{
			ocean_assert(false && "Invalid resolution!");
			return Frame();
		}

		bool applyCrop = false;

		int32_t cropLeft = 0;
		int32_t cropTop = 0;
		int32_t cropRight = 0;
		int32_t cropBottom = 0;
		const bool validDisplayCrop = nativeMediaLibrary.AMediaFormat_getRect(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_DISPLAY_CROP, &cropLeft, &cropTop, &cropRight, &cropBottom);

		if (validDisplayCrop)
		{
			if (cropLeft >= 0 && cropLeft <= cropRight && cropTop >= 0 && cropTop <= cropBottom)
			{
				const int32_t cropWidth = cropRight - cropLeft + 1;
				const int32_t cropHeight = cropBottom - cropTop + 1;

				if (cropWidth > width || cropHeight > height)
				{
					ocean_assert(false && "Invalid crop!");
					return Frame();
				}

				if (cropWidth != width || cropHeight != height)
				{
					applyCrop = true;
				}
			}
			else
			{
				ocean_assert(false && "Invalid crop!");
				return Frame();
			}
		}

		int32_t stride = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_STRIDE, &stride);

		int32_t sliceHeight = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_SLICE_HEIGHT, &sliceHeight);

		if (sliceHeight == 0)
		{
			sliceHeight = height;
		}

		int32_t colorFormat = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_FORMAT, &colorFormat);

		int32_t colorRange = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_RANGE, &colorRange);

		const FrameType::PixelFormat pixelFormat = PixelFormats::androidMediaCodecColorFormatToPixelFormat(PixelFormats::AndroidMediaCodecColorFormat(colorFormat), PixelFormats::AndroidMediaFormatColorRange(colorRange));

		Frame::CopyMode copyMode = applyCrop ? Frame::CM_USE_KEEP_LAYOUT : Frame::CM_COPY_REMOVE_PADDING_LAYOUT;

		if (pixelFormat == FrameType::FORMAT_Y_U_V12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_U_V12_FULL_RANGE || pixelFormat == FrameType::FORMAT_Y_UV12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_UV12_FULL_RANGE)
		{
			if (width >= 0 && width <= 1920 * 8 && height >= 0 && height <= 1920 * 8 && stride >= width && width % 2 == 0 && height % 2 == 0 && sliceHeight >= height)
			{
				const FrameType frameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);
				ocean_assert(frameType.isValid());

				size_t outputBufferSize = 0;
				uint8_t* outputBuffer = nativeMediaLibrary.AMediaCodec_getOutputBuffer(mediaCodec, outputBufferIndex, &outputBufferSize);

				if (outputBuffer != nullptr && frameType.frameTypeSize() <= (unsigned int)(outputBufferSize))
				{
					/**
					 * From Android documentation:
					 * For YUV 420 formats, the stride corresponds to the Y plane; the stride of the U and V planes can be calculated based on the color format,
					 * though it is generally undefined and depends on the device and release.
					 */

					unsigned int paddingElements = 0u;
					if (Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), stride, paddingElements, 0u /*planeIndex*/))
					{
						Frame::PlaneInitializers<uint8_t> planeInitializers;

						const uint8_t* yPlane = outputBuffer;

						if (pixelFormat == FrameType::FORMAT_Y_UV12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_UV12_FULL_RANGE)
						{
							const size_t plane0Size = size_t(stride * sliceHeight);
							const size_t plane1Size = size_t(stride * height / 2);

							const size_t necessarySize = plane0Size + plane1Size;

							if (outputBufferSize >= necessarySize)
							{
								planeInitializers =
								{
									Frame::PlaneInitializer<uint8_t>(yPlane, copyMode, paddingElements), // here we use the number of padding elements for both planes, this is the best guess we can make for this pixel format
									Frame::PlaneInitializer<uint8_t>(yPlane + plane0Size, copyMode, paddingElements)
								};
							}
						}
						else
						{
							ocean_assert(pixelFormat == FrameType::FORMAT_Y_U_V12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_U_V12_FULL_RANGE);

							const unsigned int width_2 = (unsigned int)(width) / 2u;
							const unsigned int height_2 = (unsigned int)(height) / 2u;

							constexpr unsigned int paddingElements12 = 0u; // here we use a fixed number of padding elements, this is the best guess we can make for this pixel format

							const unsigned int plane0StrideElements = width + paddingElements;
							const unsigned int plane12StrideElements = width_2 + paddingElements12;

							const size_t plane0Size = size_t(plane0StrideElements * sliceHeight);
							const size_t plane12Size = size_t(plane12StrideElements * height_2);

							const size_t necessarySize = plane0Size + plane12Size * 2;

							if (outputBufferSize >= necessarySize)
							{
								planeInitializers =
								{
									Frame::PlaneInitializer<uint8_t>(yPlane, copyMode, paddingElements),
									Frame::PlaneInitializer<uint8_t>(yPlane + plane0Size, copyMode, paddingElements12),
									Frame::PlaneInitializer<uint8_t>(yPlane + plane0Size + plane12Size, copyMode, paddingElements12)
								};
							}
						}

						if (!planeInitializers.empty())
						{
							const Timestamp timestamp(true);

							frame = Frame(frameType, planeInitializers, timestamp);
							frame.setRelativeTimestamp(relativeTimestamp);
						}
						else
						{
							ocean_assert(false && "Invalid memory!");
						}
					}
					else
					{
						ocean_assert(false && "Invalid stride!");
					}
				}
				else
				{
					Log::debug() << "buffer size: " << outputBufferSize << ", expected: " << frameType.frameTypeSize();
					ocean_assert(false && "Invalid input data!");
				}
			}
			else
			{
				ocean_assert(false && "Invalid image resolution");
			}
		}
		else
		{
			Log::error() << "The pixel format " << colorFormat << " is not supported";

			ocean_assert(false && "Pixel format is not supported");
		}

		if (frame.isValid() && applyCrop)
		{
			const unsigned int cropWidth = (unsigned int)(cropRight - cropLeft + 1);
			const unsigned int cropHeight = (unsigned int)(cropBottom - cropTop + 1);

			frame = frame.subFrame((unsigned int)(cropLeft), (unsigned int)(cropTop), cropWidth, cropHeight, Frame::CM_COPY_REMOVE_PADDING_LAYOUT);
		}

		nativeMediaLibrary.AMediaFormat_delete(outputMediaFormat);
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	nativeMediaLibrary.AMediaCodec_releaseOutputBuffer(mediaCodec, size_t(outputBufferIndex), false /*render*/);

	return frame;
}

} // namespace Android

} // namespace Media

} // namespace Ocean

#endif // __ANDROID_API__
