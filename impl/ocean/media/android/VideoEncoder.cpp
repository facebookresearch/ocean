/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/VideoEncoder.h"
#include "ocean/media/android/PixelFormats.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

VideoEncoder::VideoEncoder()
{
	nativeMediaLibrarySubscription_ = NativeMediaLibrary::get().initialize();

	if (!nativeMediaLibrarySubscription_)
	{
		Log::error() << "VideoEncoder: Failed to initialize native media library";
	}
}

VideoEncoder::~VideoEncoder()
{
	release();
}

bool VideoEncoder::initialize(const unsigned int width, const unsigned int height, const std::string& mime, const double frameRate, const unsigned int bitrate, const int iFrameInterval, const FrameType::PixelFormat preferredPixelFormat)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(!mime.empty());
	ocean_assert(bitrate > 0u);
	ocean_assert(frameRate > 0.0);

	if (width == 0u || height == 0u || width > maximalWidth_ || height > maximalHeight_)
	{
		ocean_assert(false && "Invalid dimensions");
		return false;
	}

	if (bitrate == 0u || bitrate > maximalBitrate_)
	{
		ocean_assert(false && "Invalid bitrate");
		return false;
	}

	if (frameRate <= 0.0)
	{
		ocean_assert(false && "Invalid frame rate");
		return false;
	}

	if (mime.empty())
	{
		ocean_assert(false && "Invalid MIME type");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (encoder_.isValid())
	{
		ocean_assert(false && "Already initialized");
		return false;
	}

	if (!nativeMediaLibrarySubscription_)
	{
		Log::error() << "VideoEncoder: Native media library is not initialized";
		return false;
	}

	PixelFormats::AndroidMediaFormatColorRange androidColorRange = PixelFormats::COLOR_RANGE_UNKNOWN;
	PixelFormats::AndroidMediaCodecColorFormat androidColorFormat = PixelFormats::pixelFormatToAndroidMediaCodecColorFormat(preferredPixelFormat, androidColorRange);

	if (androidColorFormat == PixelFormats::COLOR_FORMAT_UNKNOWN && preferredPixelFormat != FrameType::FORMAT_Y_UV12_LIMITED_RANGE)
	{
		// the COLOR_FormatYUV420SemiPlanar should always be supported, so we try to use it as a fallback

		Log::warning() << "VideoEncoder: The preferred pixel format " << FrameType::translatePixelFormat(preferredPixelFormat) << " is not supported, falling back to FORMAT_Y_UV12_LIMITED_RANGE";

		androidColorRange = PixelFormats::COLOR_RANGE_UNKNOWN;
		androidColorFormat = PixelFormats::pixelFormatToAndroidMediaCodecColorFormat(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, androidColorRange);
	}

	if (androidColorFormat == PixelFormats::AndroidMediaCodecColorFormat::COLOR_FORMAT_UNKNOWN)
	{
		Log::error() << "VideoEncoder: Failed to determine a valid color format";
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	NativeMediaLibrary::ScopedAMediaCodec encoder(nativeMediaLibrary.AMediaCodec_createEncoderByType(mime.c_str()));

	if (!encoder.isValid())
	{
		Log::error() << "Failed to create encoder for MIME " << mime;
		return false;
	}

	NativeMediaLibrary::ScopedAMediaFormat format(nativeMediaLibrary.AMediaFormat_new());

	if (!format.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// Set required encoder parameters
	nativeMediaLibrary.AMediaFormat_setString(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_MIME, mime.c_str());
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_WIDTH, int(width));
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_HEIGHT, int(height));
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_BIT_RATE, int(bitrate));
	nativeMediaLibrary.AMediaFormat_setFloat(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_FRAME_RATE, float(frameRate));

	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_COLOR_FORMAT, androidColorFormat);
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_COLOR_RANGE, androidColorRange);

	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, iFrameInterval);

	const media_status_t configureStatus = nativeMediaLibrary.AMediaCodec_configure(*encoder, *format, nullptr /*surface*/, nullptr /*crypto*/, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);

	if (configureStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to configure encoder, status: " << configureStatus;
		return false;
	}

	encoder_ = std::move(encoder);

	ocean_assert(isStarted_ == false);

	return true;
}

bool VideoEncoder::start()
{
	const ScopedLock scopedLock(lock_);

	if (!encoder_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (isStarted_)
	{
		return true;
	}

	const media_status_t startStatus = NativeMediaLibrary::get().AMediaCodec_start(*encoder_);

	if (startStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to start encoder, status: " << startStatus;
		return false;
	}

	isStarted_ = true;

	return true;
}

bool VideoEncoder::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!encoder_.isValid() || !isStarted_)
	{
		// no assert, it's fine to stop a not-initialized or non-started encoder
		return true;
	}

	const media_status_t stopStatus = NativeMediaLibrary::get().AMediaCodec_stop(*encoder_);

	if (stopStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to stop encoder, status: " << stopStatus;
		return false;
	}

	isStarted_ = false;

	return true;
}

bool VideoEncoder::pushFrame(const Frame& frame, const uint64_t presentationTime)
{
	ocean_assert(frame.isValid());

	if (!frame.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!encoder_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (!isStarted_)
	{
		ocean_assert(false && "Not started");
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	// Get the input format to determine expected color format and dimensions
	NativeMediaLibrary::ScopedAMediaFormat inputFormat(nativeMediaLibrary.AMediaCodec_getInputFormat(*encoder_));

	if (!inputFormat.isValid())
	{
		Log::error() << "Failed to get input format";
		return false;
	}

	int32_t expectedWidth = NumericT<int32_t>::minValue();
	int32_t expectedHeight = NumericT<int32_t>::minValue();
	int32_t expectedColorFormat = NumericT<int32_t>::minValue();
	int32_t colorRange = NumericT<int32_t>::minValue();

	nativeMediaLibrary.AMediaFormat_getInt32(*inputFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_WIDTH, &expectedWidth);
	nativeMediaLibrary.AMediaFormat_getInt32(*inputFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_HEIGHT, &expectedHeight);
	nativeMediaLibrary.AMediaFormat_getInt32(*inputFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_FORMAT, &expectedColorFormat);
	nativeMediaLibrary.AMediaFormat_getInt32(*inputFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_RANGE, &colorRange);

	if (expectedWidth <= 0 || expectedHeight <= 0)
	{
		Log::error() << "Invalid expected dimensions";
		return false;
	}

	const int64_t timeoutUs = Timestamp::seconds2microseconds(0.5);
	const ssize_t inputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueInputBuffer(*encoder_, timeoutUs);

	if (inputBufferIndex < 0)
	{
		Log::error() << "No available input buffer";
		return false;
	}

	size_t inputBufferSize = 0;
	uint8_t* const inputBuffer = nativeMediaLibrary.AMediaCodec_getInputBuffer(*encoder_, size_t(inputBufferIndex), &inputBufferSize);

	if (inputBuffer == nullptr || inputBufferSize == 0)
	{
		Log::error() << "Invalid input buffer";
		return false;
	}

	Frame encoderFrame = frameFromBuffer(frame.width(), frame.height(), inputBuffer, inputBufferSize, expectedColorFormat, colorRange);

	if (!encoderFrame.isValid())
	{
		return false;
	}

	if (!CV::FrameConverter::Comfort::convertAndCopy(frame, encoderFrame))
	{
		Log::error() << "VideoEncoder:: Failed to copy the provided frame " << frame.width() << "x" << frame.height() << ", " << FrameType::translatePixelFormat(frame.pixelFormat()) << " to the encoder frame " << encoderFrame.width() << "x" << encoderFrame.height() << ", " << FrameType::translatePixelFormat(encoderFrame.pixelFormat());
		return false;
	}

	size_t requiredSize = 0;
	for (unsigned int planeIndex = 0u; planeIndex < encoderFrame.numberPlanes(); ++planeIndex)
	{
		requiredSize += encoderFrame.size(planeIndex);
	}

	const media_status_t queueStatus = nativeMediaLibrary.AMediaCodec_queueInputBuffer(*encoder_, size_t(inputBufferIndex), 0u /*offset*/, requiredSize, presentationTime, 0u /*flags*/);

	if (queueStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to queue input buffer, status: " << queueStatus;
		return false;
	}

	return true;
}

VideoEncoder::Sample VideoEncoder::popSample()
{
	const ScopedLock scopedLock(lock_);

	if (!encoder_.isValid() || !isStarted_)
	{
		ocean_assert(false && "Not initialized or started");
		return Sample();
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	AMediaCodecBufferInfo codecBufferInfo;
	const ssize_t outputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueOutputBuffer(*encoder_, &codecBufferInfo, 0);

	if (outputBufferIndex < 0)
	{
		// no output buffer yet
		return Sample();
	}

	size_t outputBufferSize = 0;
	uint8_t* const outputBuffer = nativeMediaLibrary.AMediaCodec_getOutputBuffer(*encoder_, size_t(outputBufferIndex), &outputBufferSize);

	if (outputBuffer == nullptr || codecBufferInfo.size <= 0)
	{
		Log::warning() << "Invalid output buffer or empty data";
		nativeMediaLibrary.AMediaCodec_releaseOutputBuffer(*encoder_, size_t(outputBufferIndex), false /*render*/);
		return Sample();
	}

	std::vector<uint8_t> encodedData(size_t(codecBufferInfo.size));
	memcpy(encodedData.data(), outputBuffer + codecBufferInfo.offset, size_t(codecBufferInfo.size));

	nativeMediaLibrary.AMediaCodec_releaseOutputBuffer(*encoder_, size_t(outputBufferIndex), false /*render*/);

	return Sample(std::move(encodedData), codecBufferInfo.presentationTimeUs, MediaCodecBufferFlags(codecBufferInfo.flags));
}

void VideoEncoder::release()
{
	const ScopedLock scopedLock(lock_);

	if (encoder_.isValid())
	{
		if (isStarted_)
		{
			stop();
		}

		encoder_.release();
	}
}

Frame VideoEncoder::frameFromBuffer(const unsigned int width, const unsigned int height, uint8_t* const buffer, size_t size, const int32_t androidFormat, const int32_t androidColorRange)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(buffer != nullptr && size > 0);

	const FrameType::PixelFormat pixelFormat = PixelFormats::androidMediaCodecColorFormatToPixelFormat(PixelFormats::AndroidMediaCodecColorFormat(androidFormat), PixelFormats::AndroidMediaFormatColorRange(androidColorRange));

	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);
	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return Frame();
	}

	if (pixelFormat != FrameType::FORMAT_Y_U_V12_LIMITED_RANGE && pixelFormat != FrameType::FORMAT_Y_U_V12_FULL_RANGE && pixelFormat != FrameType::FORMAT_Y_UV12_LIMITED_RANGE && pixelFormat != FrameType::FORMAT_Y_UV12_FULL_RANGE)
	{
		Log::error() << "VideoEncoder: Unsupported pixel format: " << FrameType::translatePixelFormat(pixelFormat);
		return Frame();
	}

	if (width % 2u != 0u || height % 2u != 0u)
	{
		ocean_assert(false && "Width and height must be even for YUV420 formats");
		return Frame();
	}

	const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	if (!frameType.isValid())
	{
		return Frame();
	}

	// Calculate expected buffer size for YUV420 format (assuming no padding initially)
	const size_t expectedMinimumSize = frameType.frameTypeSize();

	if (size < expectedMinimumSize)
	{
		Log::error() << "VideoEncoder: Buffer size " << size << " is too small, expected at least " << expectedMinimumSize;
		return Frame();
	}

	Frame::PlaneInitializers<uint8_t> planeInitializers;

	if (pixelFormat == FrameType::FORMAT_Y_UV12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_UV12_FULL_RANGE)
	{
		// Semi-planar format: Y plane followed by interleaved UV plane
		// Assume tightly packed for simplicity (padding elements = 0)
		const unsigned int paddingElements = 0u;

		const size_t yPlaneSize = size_t(width * height);
		const size_t uvPlaneSize = size_t(width * height / 2u);

		if (size < yPlaneSize + uvPlaneSize)
		{
			Log::error() << "VideoEncoder: Buffer size insufficient for Y_UV12 format";
			return Frame();
		}

		planeInitializers =
		{
			Frame::PlaneInitializer<uint8_t>(buffer, Frame::CM_USE_KEEP_LAYOUT, paddingElements),
			Frame::PlaneInitializer<uint8_t>(buffer + yPlaneSize, Frame::CM_USE_KEEP_LAYOUT, paddingElements)
		};
	}
	else
	{
		ocean_assert(pixelFormat == FrameType::FORMAT_Y_U_V12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_U_V12_FULL_RANGE);

		// Planar format: Y plane followed by U plane followed by V plane
		const unsigned int paddingElements = 0u;

		const size_t yPlaneSize = size_t(width * height);
		const size_t uPlaneSize = size_t(width * height / 4u);
		const size_t vPlaneSize = size_t(width * height / 4u);

		if (size < yPlaneSize + uPlaneSize + vPlaneSize)
		{
			Log::error() << "VideoEncoder: Buffer size insufficient for Y_U_V12 format";
			return Frame();
		}

		planeInitializers =
		{
			Frame::PlaneInitializer<uint8_t>(buffer, Frame::CM_USE_KEEP_LAYOUT, paddingElements),
			Frame::PlaneInitializer<uint8_t>(buffer + yPlaneSize, Frame::CM_USE_KEEP_LAYOUT, paddingElements),
			Frame::PlaneInitializer<uint8_t>(buffer + yPlaneSize + uPlaneSize, Frame::CM_USE_KEEP_LAYOUT, paddingElements)
		};
	}

	Frame frame(frameType, planeInitializers);

	return frame;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24
