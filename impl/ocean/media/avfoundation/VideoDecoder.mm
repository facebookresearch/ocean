/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/VideoDecoder.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/base/StringApple.h"

#include <VideoToolbox/VideoToolbox.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

VideoDecoder::VideoDecoder()
{
	// nothing to do here
}

VideoDecoder::~VideoDecoder()
{
	release();
}

bool VideoDecoder::initialize(const std::string& mime, const unsigned int width, const unsigned int height, const void* codecConfigData, const size_t codecConfigSize)
{
	ocean_assert(!mime.empty());
	ocean_assert(width != 0u && height != 0u);

	if (mime.empty() || width == 0u || height == 0u)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (decompressionSession_.isValid())
	{
		ocean_assert(false && "Already initialized");
		return false;
	}

	const CMVideoCodecType codecType = mimeToCodecType(mime);

	if (codecType == 0)
	{
		Log::error() << "VideoDecoder: Unsupported MIME type: " << mime;
		return false;
	}

	width_ = width;
	height_ = height;

	const OSType pixelFormat = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;

	OSStatus status = noErr;

	// create format description from codec config data if available
	if (codecConfigData != nullptr && codecConfigSize > 0)
	{
		const uint8_t* configBytes = static_cast<const uint8_t*>(codecConfigData);

		if (codecType == kCMVideoCodecType_H264)
		{
			// parse AVCC format to extract SPS and PPS
			// AVCC format: [1 byte version][1 byte profile][1 byte compatibility][1 byte level]
			//              [1 byte NAL unit length size - 1][1 byte num SPS with 0xE0 mask]
			//              [2 bytes SPS length][SPS data]...[1 byte num PPS][2 bytes PPS length][PPS data]...

			std::vector<const uint8_t*> parameterSetPointers;
			std::vector<size_t> parameterSetSizes;

			if (codecConfigSize >= 7)
			{
				size_t offset = 5;

				// number of SPS (lower 5 bits)
				const uint8_t numSPS = configBytes[offset] & 0x1F;
				offset++;

				for (uint8_t i = 0; i < numSPS && offset + 2 <= codecConfigSize; ++i)
				{
					const size_t spsLength = (size_t(configBytes[offset]) << 8) | configBytes[offset + 1];
					offset += 2;

					if (offset + spsLength <= codecConfigSize)
					{
						parameterSetPointers.push_back(configBytes + offset);
						parameterSetSizes.push_back(spsLength);
						offset += spsLength;
					}
				}

				// number of PPS
				if (offset < codecConfigSize)
				{
					const uint8_t numPPS = configBytes[offset];
					offset++;

					for (uint8_t i = 0; i < numPPS && offset + 2 <= codecConfigSize; ++i)
					{
						const size_t ppsLength = (size_t(configBytes[offset]) << 8) | configBytes[offset + 1];
						offset += 2;

						if (offset + ppsLength <= codecConfigSize)
						{
							parameterSetPointers.push_back(configBytes + offset);
							parameterSetSizes.push_back(ppsLength);
							offset += ppsLength;
						}
					}
				}
			}

			if (!parameterSetPointers.empty())
			{
				status = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault, parameterSetPointers.size(), parameterSetPointers.data(), parameterSetSizes.data(), 4 /*NAL unit length size*/, &formatDescription_.resetObject());

				if (status != noErr)
				{
					Log::error() << "VideoDecoder: Failed to create H264 format description from parameter sets, error: " << status;
					return false;
				}
			}
		}
		else if (codecType == kCMVideoCodecType_HEVC)
		{
			// parse HVCC format for HEVC
			// similar to AVCC but with VPS, SPS, and PPS

			std::vector<const uint8_t*> parameterSetPointers;
			std::vector<size_t> parameterSetSizes;

			if (codecConfigSize >= 23)
			{
				size_t offset = 22;
				const uint8_t numArrays = configBytes[offset];
				offset++;

				for (uint8_t arrayIdx = 0; arrayIdx < numArrays && offset + 3 <= codecConfigSize; ++arrayIdx)
				{
					offset++; // NAL unit type
					const uint16_t numNalus = (uint16_t(configBytes[offset]) << 8) | configBytes[offset + 1];
					offset += 2;

					for (uint16_t naluIdx = 0; naluIdx < numNalus && offset + 2 <= codecConfigSize; ++naluIdx)
					{
						const size_t naluLength = (size_t(configBytes[offset]) << 8) | configBytes[offset + 1];
						offset += 2;

						if (offset + naluLength <= codecConfigSize)
						{
							parameterSetPointers.push_back(configBytes + offset);
							parameterSetSizes.push_back(naluLength);
							offset += naluLength;
						}
					}
				}
			}

			if (!parameterSetPointers.empty())
			{
				status = CMVideoFormatDescriptionCreateFromHEVCParameterSets(kCFAllocatorDefault, parameterSetPointers.size(), parameterSetPointers.data(), parameterSetSizes.data(), 4 /*NAL unit length size*/, nullptr /*extensions*/, &formatDescription_.resetObject());

				if (status != noErr)
				{
					Log::error() << "VideoDecoder: Failed to create HEVC format description from parameter sets, error: " << status;
					return false;
				}
			}
		}
	}

	// fallback: create basic format description if no codec config was provided
	if (!formatDescription_.isValid())
	{
		NSDictionary* extensions = @{(__bridge NSString*)kCMFormatDescriptionExtension_FormatName: codecType == kCMVideoCodecType_H264 ? @"avc1" : @"hvc1"};

		status = CMVideoFormatDescriptionCreate(kCFAllocatorDefault, codecType, (int32_t)width, (int32_t)height, (__bridge CFDictionaryRef)extensions, &formatDescription_.resetObject());

		if (status != noErr)
		{
			Log::error() << "VideoDecoder: Failed to create format description, error: " << status;
			return false;
		}
	}

	NSDictionary* destinationImageBufferAttributes = @{(__bridge NSString*)kCVPixelBufferPixelFormatTypeKey: @(pixelFormat), (__bridge NSString*)kCVPixelBufferWidthKey: @(width), (__bridge NSString*)kCVPixelBufferHeightKey: @(height), (__bridge NSString*)kCVPixelBufferIOSurfacePropertiesKey: @{}};

	VTDecompressionOutputCallbackRecord callbackRecord;
	callbackRecord.decompressionOutputCallback = decompressionOutputCallback;
	callbackRecord.decompressionOutputRefCon = this;

	status = VTDecompressionSessionCreate(kCFAllocatorDefault, formatDescription_.object(), nullptr /*videoDecoderSpecification*/, (__bridge CFDictionaryRef)destinationImageBufferAttributes, &callbackRecord, &decompressionSession_.resetObject());

	if (status != noErr)
	{
		Log::error() << "VideoDecoder: Failed to create decompression session, error: " << status;
		formatDescription_.release();
		return false;
	}

	ocean_assert(isStarted_ == false);

	return true;
}

bool VideoDecoder::start()
{
	const ScopedLock scopedLock(lock_);

	if (!decompressionSession_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (isStarted_)
	{
		return true;
	}

	isStarted_ = true;

	return true;
}

bool VideoDecoder::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!decompressionSession_.isValid() || !isStarted_)
	{
		return true;
	}

	VTDecompressionSessionWaitForAsynchronousFrames(*decompressionSession_);

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

	if (!decompressionSession_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (!isStarted_)
	{
		ocean_assert(false && "Not started");
		return false;
	}

	// create a block buffer from the sample data
	ScopedCMBlockBufferRef scopedBlockBuffer;

	OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, nullptr, size, kCFAllocatorDefault, nullptr, 0, size, 0, &scopedBlockBuffer.resetObject());

	if (status != noErr || !scopedBlockBuffer.isValid())
	{
		Log::error() << "VideoDecoder: Failed to create block buffer, error: " << status;
		return false;
	}

	status = CMBlockBufferReplaceDataBytes(data, *scopedBlockBuffer, 0, size);

	if (status != noErr)
	{
		Log::error() << "VideoDecoder: Failed to copy data to block buffer, error: " << status;
		return false;
	}

	// create the sample buffer
	ScopedCMSampleBufferRef scopedSampleBuffer;

	const size_t sampleSizeArray[] = {size};

	CMTime pts = CMTimeMake(int64_t(presentationTime), 1000000);
	CMTime duration = kCMTimeInvalid;

	CMSampleTimingInfo timingInfo;
	timingInfo.presentationTimeStamp = pts;
	timingInfo.decodeTimeStamp = kCMTimeInvalid;
	timingInfo.duration = duration;

	status = CMSampleBufferCreateReady(kCFAllocatorDefault, *scopedBlockBuffer, formatDescription_.object(), 1, 1, &timingInfo, 1, sampleSizeArray, &scopedSampleBuffer.resetObject());

	if (status != noErr || !scopedSampleBuffer.isValid())
	{
		Log::error() << "VideoDecoder: Failed to create sample buffer, error: " << status;
		return false;
	}

	// store the presentation time for the callback
	int64_t* presentationTimePtr = new int64_t(int64_t(presentationTime));

	VTDecodeFrameFlags decodeFlags = kVTDecodeFrame_EnableAsynchronousDecompression;
	VTDecodeInfoFlags infoFlagsOut = 0;

	status = VTDecompressionSessionDecodeFrame(*decompressionSession_, *scopedSampleBuffer, decodeFlags, presentationTimePtr, &infoFlagsOut);

	if (status != noErr)
	{
		delete presentationTimePtr;
		Log::error() << "VideoDecoder: Failed to decode frame, error: " << status;
		return false;
	}

	return true;
}

Frame VideoDecoder::popFrame(int64_t* presentationTime)
{
	const ScopedLock scopedLock(decodedFramesLock_);

	if (decodedFrames_.empty())
	{
		return Frame();
	}

	DecodedFrame decodedFrame = std::move(decodedFrames_.front());
	decodedFrames_.pop_front();

	if (presentationTime != nullptr)
	{
		*presentationTime = decodedFrame.presentationTime_;
	}

	return std::move(decodedFrame.frame_);
}

void VideoDecoder::release()
{
	const ScopedLock scopedLock(lock_);

	if (decompressionSession_.isValid())
	{
		if (isStarted_)
		{
			VTDecompressionSessionWaitForAsynchronousFrames(*decompressionSession_);
			isStarted_ = false;
		}

		decompressionSession_.release();
	}

	formatDescription_.release();

	{
		const ScopedLock decodedLock(decodedFramesLock_);
		decodedFrames_.clear();
	}

	width_ = 0u;
	height_ = 0u;
}

void VideoDecoder::decompressionOutputCallback(void* decompressionOutputReferenceConstant, void* sourceFrameReferenceConstant, OSStatus status, VTDecodeInfoFlags infoFlags, CVImageBufferRef imageBuffer, CMTime presentationTimeStamp, CMTime presentationDuration)
{
	VideoDecoder* decoder = static_cast<VideoDecoder*>(decompressionOutputReferenceConstant);
	ocean_assert(decoder != nullptr);

	int64_t* presentationTimePtr = static_cast<int64_t*>(sourceFrameReferenceConstant);
	const int64_t presentationTime = presentationTimePtr != nullptr ? *presentationTimePtr : 0;
	delete presentationTimePtr;

	if (status != noErr)
	{
		Log::error() << "VideoDecoder: Decompression callback error: " << status;
		return;
	}

	if (imageBuffer == nullptr)
	{
		Log::warning() << "VideoDecoder: Null image buffer in callback";
		return;
	}

	PixelBufferAccessor pixelBufferAccessor(imageBuffer, true);

	if (!pixelBufferAccessor)
	{
		Log::error() << "VideoDecoder: Failed to access pixel buffer";
		return;
	}

	const Frame& frame = pixelBufferAccessor.frame();

	if (!frame.isValid())
	{
		Log::error() << "VideoDecoder: Failed to extract frame from pixel buffer";
		return;
	}

	// make an owning copy of the frame

	DecodedFrame decodedFrame;
	decodedFrame.frame_ = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
    decodedFrame.frame_.setTimestamp(Timestamp(true));
	decodedFrame.frame_.setRelativeTimestamp(Timestamp(Timestamp::microseconds2seconds(presentationTime)));
	decodedFrame.presentationTime_ = presentationTime;

    const ScopedLock scopedLock(decoder->decodedFramesLock_);

	decoder->decodedFrames_.push_back(std::move(decodedFrame));
}

CMVideoCodecType VideoDecoder::mimeToCodecType(const std::string& mime)
{
	if (mime == "video/avc" || mime == "video/h264")
	{
		return kCMVideoCodecType_H264;
	}
	else if (mime == "video/hevc" || mime == "video/h265")
	{
		return kCMVideoCodecType_HEVC;
	}
	else if (mime == "video/mp4v-es")
	{
		return kCMVideoCodecType_MPEG4Video;
	}
	else if (mime == "video/jpeg")
	{
		return kCMVideoCodecType_JPEG;
	}

	return 0;
}

}

}

}
