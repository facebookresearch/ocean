/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/VideoDecoder.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include "ocean/media/android/PixelFormats.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

VideoDecoder::VideoDecoder()
{
	nativeMediaLibrarySubscription_ = NativeMediaLibrary::get().initialize();

	if (!nativeMediaLibrarySubscription_)
	{
		Log::error() << "VideoDecoder:: Failed to initialize native media library";
	}
}

VideoDecoder::~VideoDecoder()
{
	release();
}

bool VideoDecoder::initialize(const std::string& mime, const unsigned int width, const unsigned int height)
{
	ocean_assert(!mime.empty());
	ocean_assert(width != 0u && height != 0u);

	if (mime.empty() || width == 0u || height == 0u)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (decoder_.isValid())
	{
		ocean_assert(false && "Already initialized");
		return false;
	}

	if (!nativeMediaLibrarySubscription_)
	{
		Log::error() << "VideoRecorder: Native media library is not initialized";
		return false;
	}

	NativeMediaLibrary& nativeMediaLibrary = NativeMediaLibrary::get();

	NativeMediaLibrary::ScopedAMediaCodec decoder(nativeMediaLibrary.AMediaCodec_createDecoderByType(mime.c_str()));

	if (!decoder.isValid())
	{
		Log::error() << "Failed to create decoder for MIME " << mime;
		return false;
	}

	NativeMediaLibrary::ScopedAMediaFormat format(nativeMediaLibrary.AMediaFormat_new());

	if (!format.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	nativeMediaLibrary.AMediaFormat_setString(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_MIME, mime.c_str());
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_WIDTH, int(width));
	nativeMediaLibrary.AMediaFormat_setInt32(*format, nativeMediaLibrary.AMEDIAFORMAT_KEY_HEIGHT, int(height));

	const media_status_t configureStatus = nativeMediaLibrary.AMediaCodec_configure(*decoder, *format, nullptr /*surface*/, nullptr /*crypto*/, 0 /*flags*/);

	if (configureStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to configure codec, status: " << configureStatus;
		return false;
	}

	decoder_ = std::move(decoder);

	ocean_assert(isStarted_ == false);

	return true;
}

bool VideoDecoder::start()
{
	const ScopedLock scopedLock(lock_);

	if (!decoder_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (isStarted_)
	{
		return true;
	}

	const media_status_t startStatus = NativeMediaLibrary::get().AMediaCodec_start(*decoder_);

	if (startStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to start codec, status: " << startStatus;
		return false;
	}

	isStarted_ = true;

	return true;
}

bool VideoDecoder::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!decoder_.isValid() || !isStarted_)
	{
		// no assert, it's fine to stop a not-initialized or non-started decoder
		return true;
	}

	const media_status_t startStatus = NativeMediaLibrary::get().AMediaCodec_stop(*decoder_);

	if (startStatus != AMEDIA_OK)
	{
		Log::error() << "Failed to stop codec, status: " << startStatus;
		return false;
	}

	isStarted_ = false;

	return true;
}

bool VideoDecoder::pushSample(const void* data, const size_t size, const uint64_t presentationTime)
{
	ocean_assert(data != nullptr && size != 0);

	if (data == nullptr || size == 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!decoder_.isValid())
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

	const uint8_t* remainingData = (const uint8_t*)(data);
	size_t remainingSize = size;

	while (remainingSize > 0)
	{
		const int64_t timeoutUs = Timestamp::seconds2microseconds(0.5);

		const ssize_t inputBufferIndex = nativeMediaLibrary.AMediaCodec_dequeueInputBuffer(*decoder_, timeoutUs);

		if (inputBufferIndex < 0)
		{
			Log::error() << "No remaining input buffer";
			return false;
		}

		size_t inputBufferSize = 0;
		uint8_t* const inputBuffer = nativeMediaLibrary.AMediaCodec_getInputBuffer(*decoder_, size_t(inputBufferIndex), &inputBufferSize);

		if (inputBuffer == nullptr || inputBufferSize == 0)
		{
			Log::error() << "Invalid input buffer";
			return false;
		}

		const size_t bufferSize = std::min(inputBufferSize, remainingSize);

		memcpy(inputBuffer, remainingData, bufferSize);

		remainingData += bufferSize;
		remainingSize -= bufferSize;
		ocean_assert(remainingSize < size);

		const media_status_t queueStatus = nativeMediaLibrary.AMediaCodec_queueInputBuffer(*decoder_, size_t(inputBufferIndex), 0u /*offset*/, bufferSize, presentationTime, 0u);

		if (queueStatus != AMEDIA_OK)
		{
			Log::error() << "Failed to queue the input buffer, status: " << queueStatus;
			return false;
		}
	}

	return true;
}

Frame VideoDecoder::popFrame(int64_t* presentationTime)
{
	const ScopedLock scopedLock(lock_);

	if (!decoder_.isValid() || !isStarted_)
	{
		ocean_assert(false && "Not initialized or started");
		return Frame();
	}

	return extractVideoFrameFromCodecOutputBuffer(*decoder_, presentationTime);
}

void VideoDecoder::release()
{
	const ScopedLock scopedLock(lock_);

	if (decoder_.isValid())
	{
		if (isStarted_)
		{
			stop();
		}

		decoder_.release();
	}
}

Frame VideoDecoder::extractVideoFrameFromCodecOutputBuffer(AMediaCodec* const mediaCodec, int64_t* presentationTime)
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

	NativeMediaLibrary::ScopedAMediaFormat outputMediaFormat(nativeMediaLibrary.AMediaCodec_getOutputFormat(mediaCodec));

	if (outputMediaFormat.isValid())
	{
#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << "Output buffer format for current sample:";
		Log::debug() << NativeMediaLibrary::get().AMediaFormat_toString(*outputMediaFormat);
#endif

		int32_t width = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_WIDTH, &width);

		int32_t height = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_HEIGHT, &height);

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
		const bool validDisplayCrop = nativeMediaLibrary.AMediaFormat_getRect(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_DISPLAY_CROP, &cropLeft, &cropTop, &cropRight, &cropBottom);

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
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_STRIDE, &stride);

		int32_t sliceHeight = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_SLICE_HEIGHT, &sliceHeight);

		if (sliceHeight == 0)
		{
			sliceHeight = height;
		}

		int32_t colorFormat = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_FORMAT, &colorFormat);

		int32_t colorRange = 0;
		nativeMediaLibrary.AMediaFormat_getInt32(*outputMediaFormat, NativeMediaLibrary::AMEDIAFORMAT_KEY_COLOR_RANGE, &colorRange);

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
							//ocean_assert(false && "Invalid memory!");
							Log::warning() << "Invalid memory";
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
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	nativeMediaLibrary.AMediaCodec_releaseOutputBuffer(mediaCodec, size_t(outputBufferIndex), false /*render*/);

	if (presentationTime != nullptr)
	{
		*presentationTime = codecBufferInfo.presentationTimeUs;
	}

	return frame;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24
