/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/AMovieRecorder.h"

#include "ocean/base/Thread.h"

#include "ocean/io/File.h"

#include "ocean/media/android/NativeMediaLibrary.h"
#include "ocean/media/android/PixelFormats.h"

#include <stdio.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21

AMovieRecorder::AMovieRecorder()
{
	recorderFrameEncoder = "h264";
	recorderFrameFrequency = 30.0;
	recorderFilenameSuffixed = false;
}

AMovieRecorder::~AMovieRecorder()
{
	stop();

	while (isStopped_ == false)
	{
		Thread::sleep(1u);
	}
}

bool AMovieRecorder::setFilename(const std::string& filename)
{
	const ScopedLock scopedLock(recorderLock);

	if (mediaCodec_ != nullptr)
	{
		Log::error() << "The filename cannot be changed after recording has started.";
		return false;
	}

	return MovieRecorder::setFilename(filename);
}

bool AMovieRecorder::setPreferredFrameType(const FrameType& type)
{
	if (!FrameRecorder::setPreferredFrameType(type))
	{
		return false;
	}

	PixelFormats::AndroidMediaFormatColorRange colorRange = PixelFormats::AndroidMediaFormatColorRange::UNKNOWN;
	const PixelFormats::AndroidMediaCodecColorFormat colorFormat = PixelFormats::pixelFormatToAndroidMediaCodecColorFormat(recorderFrameType.pixelFormat(), colorRange);

	if (colorFormat == PixelFormats::AndroidMediaCodecColorFormat::UNKNOWN)
	{
		Log::info() << "The preferred pixel format '" << FrameType::translatePixelFormat(recorderFrameType.pixelFormat()) << "' is not supported, using 'FORMAT_Y_UV12_LIMITED_RANGE' instead";

		// FORMAT_Y_UV12_LIMITED_RANGE corresponds to COLOR_FormatYUV420SemiPlanar which is deprecated
		// better would be to use FORMAT_Y_U_V12_LIMITED_RANGE which corresponds to COLOR_FormatYUV420Flexible
		// unfortuately, COLOR_FormatYUV420Flexible does not seem to be well supported in native code (it's not possible to determine which underlying format is actually used)
		// therefore, using the deprecated format instead

		recorderFrameType.setPixelFormat(FrameType::FORMAT_Y_UV12_LIMITED_RANGE);
	}

	return true;
}

bool AMovieRecorder::start()
{
	if (!NativeMediaLibrary::get().isInitialized())
	{
		Log::error() << "Android NativeMediaLibrary is not initialized.";
		return false;
	}

	const ScopedLock scopedLock(recorderLock);

	if (mediaCodec_ != nullptr || isRecording_)
	{
		return false;
	}

	if (createNewMediaCodec())
	{
		nextFrameTimestamp_ = 0.0;
		isRecording_ = true;
		isStopped_ = false;
		return true;
	}

	release();

	return false;
}

bool AMovieRecorder::stop()
{
	const ScopedLock scopedLock(recorderLock);

	if (mediaCodec_ == nullptr || isRecording_ == false)
	{
		return false;
	}

	release();

	return true;
}

bool AMovieRecorder::isRecording() const
{
	const ScopedLock scopedLock(recorderLock);

	return isRecording_;
}

AMovieRecorder::Encoders AMovieRecorder::frameEncoders() const
{
	Encoders encoders;

	encoders.emplace_back("H264");
	encoders.emplace_back("H265");
	encoders.emplace_back("HEVC"); // equivalent to H265

	return encoders;
}

bool AMovieRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	ocean_assert(respectFrameFrequency == false && "currently not supported!");

	const ScopedLock scopedLock(recorderLock);

	if (mediaCodec_ == nullptr)
	{
		return false;
	}

	if (bufferIndex_ != -1)
	{
		ocean_assert(false && "Previous buffer has not been unlocked");
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	ocean_assert(bufferSize_ == 0);

	constexpr int64_t kInputTimeoutUs = int64_t(1000000); // 1 second

	bufferIndex_ = nativeMediaLibrary.AMediaCodec_dequeueInputBuffer(mediaCodec_, kInputTimeoutUs);

	if (bufferIndex_ < ssize_t(0))
	{
		Log::error() << "Failed to dequeue codec input buffer (" << bufferIndex_ << ").";
		return false;
	}

	uint8_t* const buffer = nativeMediaLibrary.AMediaCodec_getInputBuffer(mediaCodec_, size_t(bufferIndex_), &bufferSize_);

	if (bufferSize_ < recorderFrameType.frameTypeSize())
	{
		ocean_assert(false && "Invalid buffer!");
		return false;
	}

	AMediaFormat* inputMediaFormat = nativeMediaLibrary.AMediaCodec_getInputFormat(mediaCodec_);

	if (inputMediaFormat == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << "Input buffer format for current sample:";
		Log::debug() << nativeMediaLibrary.AMediaFormat_toString(inputMediaFormat);
#endif

	int32_t stride = 0;
	nativeMediaLibrary.AMediaFormat_getInt32(inputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_STRIDE, &stride);

	if (stride < int32_t(recorderFrameType.width()))
	{
		ocean_assert(false && "Invalid stride");
		return false;
	}

	int32_t sliceHeight = 0;
	nativeMediaLibrary.AMediaFormat_getInt32(inputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_SLICE_HEIGHT, &sliceHeight);

	if (sliceHeight == 0)
	{
		sliceHeight = int32_t(recorderFrameType.height());
	}

	Frame::PlaneInitializers<uint8_t> planeInitializer;
	planeInitializer.reserve(recorderFrameType.numberPlanes());

	const unsigned int bytesPerElement = recorderFrameType.bytesPerDataType();

	unsigned int bufferOffset = 0u;

	for (unsigned int planeIndex = 0u; planeIndex < recorderFrameType.numberPlanes(); ++planeIndex)
	{
		unsigned int planePaddingElements = 0u;

		// in case we have a 3-plane image, we use a hard-coded 0 as padding elements, this is the best guess we can make for this pixel format
		if (planeIndex == 0u || recorderFrameType.numberPlanes() <= 2u)
		{
			if (!Frame::strideBytes2paddingElements(recorderFrameType.pixelFormat(), recorderFrameType.width(), stride, planePaddingElements, planeIndex))
			{
				Log::debug() << "AMovieRecorder: Invalid padding elements!";

				ocean_assert(false && "Invalid padding elements!");
				return false;
			}
		}

		unsigned int planeWidth = 0u;
		unsigned int planeHeight = 0u;
		unsigned int planeChannels = 0u;
		if (!FrameType::planeLayout(recorderFrameType, planeIndex, planeWidth, planeHeight, planeChannels))
		{
			return false;
		}

		const unsigned int planeSliceHeight = planeIndex == 0u ? (unsigned int)(sliceHeight) : planeHeight;

		const unsigned int planeSize = (planeWidth * planeChannels + planePaddingElements) * bytesPerElement * planeSliceHeight;

		if (bufferOffset + planeSize > bufferSize_)
		{
			// the provided buffer is not large enough, this indicates a bug in the underlying media codec

			Log::debug() << "AMovieRecorder: Invalid memory!";

			ocean_assert(false && "Invalid memory!");
			return false;
		}

		planeInitializer.emplace_back(buffer + bufferOffset, Frame::CM_USE_KEEP_LAYOUT, planePaddingElements);

		bufferOffset += planeSize;
	}

	recorderFrame = Frame(recorderFrameType, planeInitializer);
	ocean_assert(recorderFrame.isValid());

	return recorderFrame.isValid();
}

void AMovieRecorder::unlockBufferToFill()
{
	ocean_assert(mediaMuxer_ != nullptr);
	ocean_assert(bufferIndex_ >= ssize_t(0));
	ocean_assert(bufferSize_ > size_t(0));

	const long nextFrameTimestampMicroseconds = static_cast<long>(nextFrameTimestamp_ * 1000000.0);
	NativeMediaLibrary::get().AMediaCodec_queueInputBuffer(mediaCodec_, bufferIndex_, /* offset */ 0, bufferSize_, nextFrameTimestampMicroseconds, /* flags */ 0);

	const bool writeWasSuccessful = readCodecOutputBufferAndWriteToMuxer();
	ocean_assert_and_suppress_unused(writeWasSuccessful, writeWasSuccessful);

	ocean_assert(recorderFrameFrequency > 0.0);
	nextFrameTimestamp_ += 1.0 / recorderFrameFrequency;

	bufferIndex_ = -1;
	bufferSize_ = 0;
}

bool AMovieRecorder::createNewMediaCodec()
{
	if (!recorderFrameType.isValid())
	{
		Log::error() << "The frame type of the recorder is not configured yet.";
		return false;
	}

	ocean_assert(mediaCodec_ == nullptr);

	const std::string filename = addOptionalSuffixToFilename(recorderFilename, recorderFilenameSuffixed);

	if (IO::File(filename).exists())
	{
		Log::error() << "The movie file \"" << filename << "\" exists already.";
		return false;
	}

	ocean_assert(!file_.isValid());
	file_ = ScopedFILE(fopen(filename.c_str(), "w"));

	if (!file_.isValid())
	{
		Log::error() << "Could not open movie file \"" << filename << "\" for writing.";
		return false;
	}

	mediaFormat_ = NativeMediaLibrary::get().AMediaFormat_new();
	ocean_assert(mediaFormat_);

	if (recorderFrameEncoder.empty())
	{
		recorderFrameEncoder = frameEncoders().front();
	}

	std::string mimeType;
	const std::string frameEncoder = String::toLower(recorderFrameEncoder);

	if (frameEncoder == "h264")
	{
		mimeType = "video/avc";
	}
	else if (frameEncoder == "h265" || frameEncoder == "hevc")
	{
		mimeType = "video/hevc";
	}

	if (!mimeType.empty())
	{
		mediaCodec_ = NativeMediaLibrary::get().AMediaCodec_createEncoderByType(mimeType.c_str());
		NativeMediaLibrary::get().AMediaFormat_setString(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_MIME, mimeType.c_str());
	}

	if (!mediaCodec_)
	{
		Log::error() << "Could not create a valid media codec with type \"" << recorderFrameEncoder << "\"";
		release();
		return false;
	}

	PixelFormats::AndroidMediaFormatColorRange colorRange = PixelFormats::AndroidMediaFormatColorRange::UNKNOWN;
	const PixelFormats::AndroidMediaCodecColorFormat colorFormat = PixelFormats::pixelFormatToAndroidMediaCodecColorFormat(recorderFrameType.pixelFormat(), colorRange);

	if (colorFormat == PixelFormats::AndroidMediaCodecColorFormat::UNKNOWN)
	{
		Log::error() << "Color format '" << FrameType::translatePixelFormat(recorderFrameType.pixelFormat()) << "' not supported for video output!";
		release();
		return false;
	}

	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_WIDTH, recorderFrameType.width());
	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_HEIGHT, recorderFrameType.height());
	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_SLICE_HEIGHT, recorderFrameType.height());
	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_BIT_RATE, DEFAULT_BITRATE);
	NativeMediaLibrary::get().AMediaFormat_setFloat(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_CAPTURE_RATE, recorderFrameFrequency);
	NativeMediaLibrary::get().AMediaFormat_setFloat(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_FRAME_RATE, recorderFrameFrequency);
	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, DEFAULT_IFRAME_INTERVAL_SECONDS);
	NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_FORMAT, int32_t(colorFormat));

	if (colorRange != PixelFormats::AndroidMediaFormatColorRange::UNKNOWN)
	{
		NativeMediaLibrary::get().AMediaFormat_setInt32(mediaFormat_, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_RANGE, int32_t(colorRange));
	}

	media_status_t status = NativeMediaLibrary::get().AMediaCodec_configure(mediaCodec_, mediaFormat_, nullptr,  nullptr, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);

	if (status != AMEDIA_OK)
	{
		Log::error() << "Failed to configure the media codec: " << status;
		release();
		return false;
	}

#ifdef OCEAN_DEBUG
	{
		AMediaFormat* mediaFormat = NativeMediaLibrary::get().AMediaCodec_getInputFormat(mediaCodec_);

		if (mediaFormat != nullptr)
		{
			const char* readableFormat = NativeMediaLibrary::get().AMediaFormat_toString(mediaFormat);

			if (readableFormat != nullptr)
			{
				Log::debug() << "AMovieRecorder: Accepted input media format: " << readableFormat;
			}

			NativeMediaLibrary::get().AMediaFormat_delete(mediaFormat);
			mediaFormat = nullptr;
		}

		mediaFormat = NativeMediaLibrary::get().AMediaCodec_getOutputFormat(mediaCodec_);

		if (mediaFormat != nullptr)
		{
			const char* readableFormat = NativeMediaLibrary::get().AMediaFormat_toString(mediaFormat);

			if (readableFormat != nullptr)
			{
				Log::debug() << "AMovieRecorder: Accepted output media format: " << readableFormat;
			}

			NativeMediaLibrary::get().AMediaFormat_delete(mediaFormat);
			mediaFormat = nullptr;
		}
	}
#endif

	status = NativeMediaLibrary::get().AMediaCodec_start(mediaCodec_);

	if (status != AMEDIA_OK)
	{
		Log::error() << "Failed to start the media codec: " << status;
		release();
		return false;
	}

	mediaMuxer_ = NativeMediaLibrary::get().AMediaMuxer_new(fileno(*file_), AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
	if (mediaMuxer_ == nullptr)
	{
		Log::error() << "Failed to create media muxer.";
		release();
		return false;
	}

	return true;
}

void AMovieRecorder::release()
{
	const bool wasRecording = isRecording_;
	isRecording_ = false;

	if (mediaCodec_ != nullptr)
	{
		// Signal an end-of-stream to the codec and gather the remaining frames, if any.
		if (wasRecording)
		{
			const ScopedLock scopedLock(recorderLock);

			ssize_t bufferIndex = ssize_t(-1);

			constexpr int64_t kInputTimeoutUs = int64_t(1000000); // 1 second
			bufferIndex = NativeMediaLibrary::get().AMediaCodec_dequeueInputBuffer(mediaCodec_, kInputTimeoutUs);

			if (bufferIndex < ssize_t(0))
			{
				Log::error() << "Failed to dequeue codec input buffer (" << bufferIndex << ").";
			}
			else
			{
				// Once EOS is enqueued, no further queueing will occur.
				const long nextFrameTimestampMicroseconds = static_cast<long>(nextFrameTimestamp_ * 1000000.0);
				NativeMediaLibrary::get().AMediaCodec_queueInputBuffer(mediaCodec_, bufferIndex, /* offset */ 0, /* size */ size_t(0), nextFrameTimestampMicroseconds, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);

				const bool writeWasSuccessful = readCodecOutputBufferAndWriteToMuxer(/* loopUntilEndOfStream */ true);
				ocean_assert_and_suppress_unused(writeWasSuccessful, writeWasSuccessful);
			}

			NativeMediaLibrary::get().AMediaCodec_stop(mediaCodec_);
		}

		NativeMediaLibrary::get().AMediaCodec_delete(mediaCodec_);
		mediaCodec_ = nullptr;
	}

	if (mediaMuxer_ != nullptr)
	{
		if (wasRecording)
		{
			NativeMediaLibrary::get().AMediaMuxer_stop(mediaMuxer_);
		}

		NativeMediaLibrary::get().AMediaMuxer_delete(mediaMuxer_);
		mediaMuxer_ = nullptr;
	}

	if (mediaFormat_ != nullptr)
	{
		NativeMediaLibrary::get().AMediaFormat_delete(mediaFormat_);
		mediaFormat_ = nullptr;
	}

	file_.release();

	nextFrameTimestamp_ = 0.0;

	isStopped_ = true; // only do this at the very end
}

bool AMovieRecorder::readCodecOutputBufferAndWriteToMuxer(const bool loopUntilEndOfStream)
{
	ocean_assert(mediaCodec_ != nullptr);
	ocean_assert(mediaMuxer_ != nullptr);

	bool success = true;

	// Loop while the codec times out, or when we receive a signal that the output format changed, at which point we'll start the muxer and then repeat to grab the first frame's data.
	// (Note that we only start the muxer after the first frame has been submitted to the codec.)
	while (true)
	{
		constexpr int64_t kOutputBufferTimeoutMicroseconds = int64_t(1);

		AMediaCodecBufferInfo bufferInfo;
		const ssize_t outputBufferIndex = NativeMediaLibrary::get().AMediaCodec_dequeueOutputBuffer(mediaCodec_, &bufferInfo, kOutputBufferTimeoutMicroseconds);

		if (outputBufferIndex == AMEDIACODEC_INFO_TRY_AGAIN_LATER)
		{
			continue; // timeout
		}

		if (outputBufferIndex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED)
		{
			AMediaFormat* outputFormat = NativeMediaLibrary::get().AMediaCodec_getOutputFormat(mediaCodec_);

			ocean_assert(trackIndex_ == -1);
			trackIndex_ = NativeMediaLibrary::get().AMediaMuxer_addTrack(mediaMuxer_, outputFormat);
			NativeMediaLibrary::get().AMediaFormat_delete(outputFormat);

			if (trackIndex_ < ssize_t(0))
			{
				Log::info() << "Error adding a track to the muxer: " << trackIndex_;
				success = false;
				break; // an error occurred
			}

			const media_status_t muxerResult = NativeMediaLibrary::get().AMediaMuxer_start(mediaMuxer_);

			if (muxerResult != AMEDIA_OK)
			{
				Log::info() << "Error starting the muxer: " << muxerResult;
				success = false;
				break; // an error occurred
			}

			continue; // we're now ready to grab the first frame's data
		}

		if (outputBufferIndex < ssize_t(0))
		{
			Log::info() << "Error getting an output buffer from the codec: " << outputBufferIndex;
			success = false;
			break; // an error occurred
		}

		if (bufferInfo.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
		{
			break; // EOS
		}

		ocean_assert(trackIndex_ >= ssize_t(0));

		// When we first initialize the video, the encoder may output codec-specific data.
		// However, this is already written when we create the muxer, so we'll skip it.
		const bool bufferContainsCodecSpecificData = (bool)(bufferInfo.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG);

		size_t bufferSize;
		uint8_t* buffer = NativeMediaLibrary::get().AMediaCodec_getOutputBuffer(mediaCodec_, outputBufferIndex, &bufferSize);

		if (!bufferContainsCodecSpecificData)
		{
			const media_status_t muxerResult = NativeMediaLibrary::get().AMediaMuxer_writeSampleData(mediaMuxer_, trackIndex_, buffer, &bufferInfo);

			if (muxerResult != AMEDIA_OK)
			{
				Log::error() << "Failed to write sample data to the muxer: " << muxerResult;
				success = false;
				break; // an error occurred
			}
		}

		const media_status_t codecBufferReleaseResult = NativeMediaLibrary::get().AMediaCodec_releaseOutputBuffer(mediaCodec_, outputBufferIndex, /* renderToSurface */ false);

		if (codecBufferReleaseResult != AMEDIA_OK)
		{
			Log::error() << "Failed to release codec output buffer: " << codecBufferReleaseResult;
			success = false;
			break; // an error occurred
		}

		// Codec-specific data, if present, will always precede actual frame data, so it's safe to loop.
		if (!bufferContainsCodecSpecificData && !loopUntilEndOfStream)
		{
			break;
		}
	}

	return success;
}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 21

} // namespace Android

} // namespace Media

} // namespace Ocean
