/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/VideoEncoder.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/base/StringApple.h"

#include "ocean/cv/FrameConverter.h"

#include <VideoToolbox/VideoToolbox.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

VideoEncoder::VideoEncoder()
{
	// nothing to do here
}

VideoEncoder::~VideoEncoder()
{
	release();
}

bool VideoEncoder::initialize(const unsigned int width, const unsigned int height, const std::string& mime, const double frameRate, const unsigned int bitrate, const int iFrameInterval)
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

	if (bitrate == 0u || bitrate > (unsigned int)(maximalBitrate_))
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

	if (compressionSession_.isValid())
	{
		ocean_assert(false && "Already initialized");
		return false;
	}

	const CMVideoCodecType codecType = mimeToCodecType(mime);

	if (codecType == 0)
	{
		Log::error() << "VideoEncoder: Unsupported MIME type: " << mime;
		return false;
	}

	width_ = width;
	height_ = height;

	NSDictionary* encoderSpecification = nil;

#if TARGET_OS_IPHONE
	encoderSpecification = @{(__bridge NSString*)kVTVideoEncoderSpecification_EnableHardwareAcceleratedVideoEncoder: @YES};
#else
	encoderSpecification = @{(__bridge NSString*)kVTVideoEncoderSpecification_EnableHardwareAcceleratedVideoEncoder: @YES, (__bridge NSString*)kVTVideoEncoderSpecification_RequireHardwareAcceleratedVideoEncoder: @NO};
#endif

	NSDictionary* sourceImageBufferAttributes = @{(__bridge NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange), (__bridge NSString*)kCVPixelBufferWidthKey: @(width), (__bridge NSString*)kCVPixelBufferHeightKey: @(height), (__bridge NSString*)kCVPixelBufferIOSurfacePropertiesKey: @{}};

	OSStatus status = VTCompressionSessionCreate(kCFAllocatorDefault, (int32_t)width, (int32_t)height, codecType, (__bridge CFDictionaryRef)encoderSpecification, (__bridge CFDictionaryRef)sourceImageBufferAttributes, kCFAllocatorDefault, compressionOutputCallback, this, &compressionSession_.resetObject());

	if (status != noErr || !compressionSession_.isValid())
	{
		Log::error() << "VideoEncoder: Failed to create compression session, error: " << status;
		return false;
	}

	// configure encoder properties
	status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);

	if (status != noErr)
	{
		Log::warning() << "VideoEncoder: Failed to set real-time property, error: " << status;
	}

	// set average bit rate
	ScopedCFNumberRef scopedBitrateRef(CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &bitrate));

	if (scopedBitrateRef.isValid())
	{
		status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_AverageBitRate, scopedBitrateRef.object());

		if (status != noErr)
		{
			Log::warning() << "VideoEncoder: Failed to set bitrate, error: " << status;
		}
	}

	// set expected frame rate
	float frameRateFloat = float(frameRate);
	ScopedCFNumberRef scopedFrameRateRef(CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &frameRateFloat));

	if (scopedFrameRateRef.isValid())
	{
		status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_ExpectedFrameRate, scopedFrameRateRef.object());

		if (status != noErr)
		{
			Log::warning() << "VideoEncoder: Failed to set frame rate, error: " << status;
		}
	}

	// set key frame interval
	if (iFrameInterval >= 0)
	{
		int maxKeyFrameInterval = iFrameInterval == 0 ? 1 : int(frameRate * double(iFrameInterval));
		ScopedCFNumberRef scopedKeyFrameRef(CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &maxKeyFrameInterval));

		if (scopedKeyFrameRef.isValid())
		{
			status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_MaxKeyFrameInterval, scopedKeyFrameRef.object());

			if (status != noErr)
			{
				Log::warning() << "VideoEncoder: Failed to set key frame interval, error: " << status;
			}
		}

		// also set MaxKeyFrameIntervalDuration for time-based keyframes
		if (iFrameInterval > 0)
		{
			float intervalDuration = float(iFrameInterval);
			ScopedCFNumberRef scopedIntervalRef(CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &intervalDuration));

			if (scopedIntervalRef.isValid())
			{
				status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_MaxKeyFrameIntervalDuration, scopedIntervalRef.object());
			}
		}
	}

	// disable frame reordering
	status = VTSessionSetProperty(*compressionSession_, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);

	if (status != noErr)
	{
		Log::debug() << "VideoEncoder: Failed to disable frame reordering, error: " << status;
	}

	status = VTCompressionSessionPrepareToEncodeFrames(*compressionSession_);

	if (status != noErr)
	{
		Log::error() << "VideoEncoder: Failed to prepare compression session, error: " << status;
		compressionSession_.release();
		return false;
	}

	ocean_assert(isStarted_ == false);

	return true;
}

bool VideoEncoder::start()
{
	const ScopedLock scopedLock(lock_);

	if (!compressionSession_.isValid())
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

bool VideoEncoder::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!compressionSession_.isValid() || !isStarted_)
	{
		return true;
	}

	VTCompressionSessionCompleteFrames(*compressionSession_, kCMTimeInvalid);

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

	if (!compressionSession_.isValid())
	{
		ocean_assert(false && "Not initialized");
		return false;
	}

	if (!isStarted_)
	{
		ocean_assert(false && "Not started");
		return false;
	}

	if (frame.width() != width_ || frame.height() != height_)
	{
		Log::error() << "VideoEncoder: Frame dimensions " << frame.width() << "x" << frame.height() << " don't match encoder dimensions " << width_ << "x" << height_;
		return false;
	}

	ScopedCVPixelBufferRef scopedPixelBuffer;

	NSDictionary* pixelBufferAttributes = @{(__bridge NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange), (__bridge NSString*)kCVPixelBufferWidthKey: @(width_), (__bridge NSString*)kCVPixelBufferHeightKey: @(height_), (__bridge NSString*)kCVPixelBufferIOSurfacePropertiesKey: @{}};

	CVReturn cvStatus = CVPixelBufferCreate(kCFAllocatorDefault, width_, height_, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, (__bridge CFDictionaryRef)pixelBufferAttributes, &scopedPixelBuffer.resetObject());

	if (cvStatus != kCVReturnSuccess || !scopedPixelBuffer.isValid())
	{
		Log::error() << "VideoEncoder: Failed to create pixel buffer, error: " << cvStatus;
		return false;
	}

	// lock the pixel buffer and copy frame data
	cvStatus = CVPixelBufferLockBaseAddress(*scopedPixelBuffer, 0);

	if (cvStatus != kCVReturnSuccess)
	{
		Log::error() << "VideoEncoder: Failed to lock pixel buffer, error: " << cvStatus;
		return false;
	}

	uint8_t* yPlane = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(*scopedPixelBuffer, 0);
	uint8_t* uvPlane = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(*scopedPixelBuffer, 1);

	const size_t yBytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(*scopedPixelBuffer, 0);
	const size_t uvBytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(*scopedPixelBuffer, 1);

	// create a target frame wrapping the pixel buffer memory
	Frame::PlaneInitializers<uint8_t> planeInitializers;

	unsigned int yPaddingElements = 0u;

	if (!Frame::strideBytes2paddingElements(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, width_, (unsigned int)(yBytesPerRow), yPaddingElements, 0u))
	{
		Log::error() << "VideoEncoder: Failed to calculate Y plane padding";
		CVPixelBufferUnlockBaseAddress(*scopedPixelBuffer, 0);
		return false;
	}

	unsigned int uvPaddingElements = 0u;

	if (!Frame::strideBytes2paddingElements(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, width_, (unsigned int)(uvBytesPerRow), uvPaddingElements, 1u))
	{
		Log::error() << "VideoEncoder: Failed to calculate UV plane padding";
		CVPixelBufferUnlockBaseAddress(*scopedPixelBuffer, 0);
		return false;
	}

	planeInitializers = {Frame::PlaneInitializer<uint8_t>(yPlane, Frame::CM_USE_KEEP_LAYOUT, yPaddingElements), Frame::PlaneInitializer<uint8_t>(uvPlane, Frame::CM_USE_KEEP_LAYOUT, uvPaddingElements)};

	FrameType targetFrameType(width_, height_, FrameType::FORMAT_Y_UV12_LIMITED_RANGE, FrameType::ORIGIN_UPPER_LEFT);
	Frame targetFrame(targetFrameType, planeInitializers);

	// convert and copy the input frame to the target frame
	if (!CV::FrameConverter::Comfort::convertAndCopy(frame, targetFrame))
	{
		Log::error() << "VideoEncoder: Failed to convert frame from " << FrameType::translatePixelFormat(frame.pixelFormat()) << " to " << FrameType::translatePixelFormat(targetFrameType.pixelFormat());
		CVPixelBufferUnlockBaseAddress(*scopedPixelBuffer, 0);
		return false;
	}

	CVPixelBufferUnlockBaseAddress(*scopedPixelBuffer, 0);

	CMTime pts = CMTimeMake(int64_t(presentationTime), 1000000);
	CMTime duration = kCMTimeInvalid;

	int64_t* presentationTimePtr = new int64_t(int64_t(presentationTime));

	// encode the frame
	VTEncodeInfoFlags infoFlagsOut = 0;

	OSStatus status = VTCompressionSessionEncodeFrame(*compressionSession_, *scopedPixelBuffer, pts, duration, nullptr, presentationTimePtr, &infoFlagsOut);

	if (status != noErr)
	{
		delete presentationTimePtr;
		Log::error() << "VideoEncoder: Failed to encode frame, error: " << status;
		return false;
	}

	return true;
}

VideoEncoder::Sample VideoEncoder::popSample()
{
	const ScopedLock scopedLock(encodedSamplesLock_);

	if (encodedSamples_.empty())
	{
		return Sample();
	}

	Sample sample = std::move(encodedSamples_.front());
	encodedSamples_.pop_front();

	return sample;
}

void VideoEncoder::release()
{
	const ScopedLock scopedLock(lock_);

	if (compressionSession_.isValid())
	{
		if (isStarted_)
		{
			VTCompressionSessionCompleteFrames(*compressionSession_, kCMTimeInvalid);
			isStarted_ = false;
		}

		compressionSession_.release();
	}

	{
		const ScopedLock encodedLock(encodedSamplesLock_);
		encodedSamples_.clear();
	}

	width_ = 0u;
	height_ = 0u;
}

void VideoEncoder::compressionOutputCallback(void* outputCallbackReferenceConstant, void* sourceFrameReferenceConstant, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer)
{
	VideoEncoder* encoder = static_cast<VideoEncoder*>(outputCallbackReferenceConstant);
	ocean_assert(encoder != nullptr);

	int64_t* presentationTimePtr = static_cast<int64_t*>(sourceFrameReferenceConstant);
	const int64_t presentationTime = presentationTimePtr != nullptr ? *presentationTimePtr : 0;
	delete presentationTimePtr;

	if (status != noErr)
	{
		Log::error() << "VideoEncoder: Compression callback error: " << status;
		return;
	}

	if (sampleBuffer == nullptr)
	{
		Log::warning() << "VideoEncoder: Null sample buffer in callback";
		return;
	}

	// Check if this is a key frame
	CFArrayRef attachments = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, false);
	bool isKeyFrame = false;

	if (attachments != nullptr && CFArrayGetCount(attachments) > 0)
	{
		CFDictionaryRef attachment = (CFDictionaryRef)CFArrayGetValueAtIndex(attachments, 0);
		CFBooleanRef notSync = (CFBooleanRef)CFDictionaryGetValue(attachment, kCMSampleAttachmentKey_NotSync);
		isKeyFrame = (notSync == nullptr || !CFBooleanGetValue(notSync));
	}

	CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);

	if (blockBuffer == nullptr)
	{
		Log::error() << "VideoEncoder: Failed to get block buffer";
		return;
	}

	size_t totalLength = 0;
	size_t lengthAtOffset = 0;
	char* dataPointer = nullptr;

	OSStatus dataStatus = CMBlockBufferGetDataPointer(blockBuffer, 0, &lengthAtOffset, &totalLength, &dataPointer);

	if (dataStatus != kCMBlockBufferNoErr || dataPointer == nullptr)
	{
		Log::error() << "VideoEncoder: Failed to get data pointer from block buffer";
		return;
	}

	// Copy the encoded data
	std::vector<uint8_t> encodedData(totalLength);
	memcpy(encodedData.data(), dataPointer, totalLength);

	// Determine buffer flags
	BufferFlags bufferFlags = BUFFER_FLAG_NONE;

	if (isKeyFrame)
	{
		bufferFlags = BufferFlags(bufferFlags | BUFFER_FLAG_KEY_FRAME);
	}

	// Extract codec configuration data for key frames
	CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);

	if (formatDesc != nullptr && isKeyFrame)
	{
		std::vector<uint8_t> codecConfigData;

		const CMVideoCodecType codecType = CMFormatDescriptionGetMediaSubType(formatDesc);

		if (codecType == kCMVideoCodecType_H264)
		{
			// build AVCC format for H.264
			// AVCC format: [1 byte version][1 byte profile][1 byte compatibility][1 byte level]
			//              [1 byte NAL unit length size - 1][1 byte num SPS with 0xE0 mask]
			//              [2 bytes SPS length][SPS data]...[1 byte num PPS][2 bytes PPS length][PPS data]...

			size_t spsCount = 0;
			OSStatus paramStatus = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(formatDesc, 0, nullptr, nullptr, &spsCount, nullptr);

			if (paramStatus == noErr && spsCount > 0)
			{
				const uint8_t* spsData = nullptr;
				size_t spsSize = 0;
				CMVideoFormatDescriptionGetH264ParameterSetAtIndex(formatDesc, 0, &spsData, &spsSize, nullptr, nullptr);

				const uint8_t* ppsData = nullptr;
				size_t ppsSize = 0;
				size_t ppsCount = 0;
				CMVideoFormatDescriptionGetH264ParameterSetAtIndex(formatDesc, 1, &ppsData, &ppsSize, &ppsCount, nullptr);

				if (spsData != nullptr && spsSize > 3 && ppsData != nullptr && ppsSize > 0)
				{
					codecConfigData.reserve(7 + spsSize + 3 + ppsSize);

					codecConfigData.push_back(1); // configurationVersion
					codecConfigData.push_back(spsData[1]); // AVCProfileIndication
					codecConfigData.push_back(spsData[2]); // profile_compatibility
					codecConfigData.push_back(spsData[3]); // AVCLevelIndication
					codecConfigData.push_back(0xFF); // lengthSizeMinusOne = 3
					codecConfigData.push_back(0xE1); // numOfSequenceParameterSets = 1

					codecConfigData.push_back((spsSize >> 8) & 0xFF);
					codecConfigData.push_back(spsSize & 0xFF);
					codecConfigData.insert(codecConfigData.end(), spsData, spsData + spsSize);

					codecConfigData.push_back(1); // numOfPictureParameterSets
					codecConfigData.push_back((ppsSize >> 8) & 0xFF);
					codecConfigData.push_back(ppsSize & 0xFF);
					codecConfigData.insert(codecConfigData.end(), ppsData, ppsData + ppsSize);
				}
			}
		}
		else if (codecType == kCMVideoCodecType_HEVC)
		{
			// Build HVCC format for HEVC
			size_t paramCount = 0;
			OSStatus paramStatus = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(formatDesc, 0, nullptr, nullptr, &paramCount, nullptr);

			if (paramStatus == noErr && paramCount > 0)
			{
				std::vector<std::pair<const uint8_t*, size_t>> paramSets;
				std::vector<uint8_t> nalTypes;

				for (size_t i = 0; i < paramCount; ++i)
				{
					const uint8_t* paramData = nullptr;
					size_t paramSize = 0;
					int nalType = 0;

					OSStatus paramStatus2 = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(formatDesc, i, &paramData, &paramSize, nullptr, &nalType);

					if (paramStatus2 == noErr && paramData != nullptr && paramSize > 0)
					{
						paramSets.push_back(std::make_pair(paramData, paramSize));
						nalTypes.push_back(uint8_t(nalType));
					}
				}

				if (!paramSets.empty())
				{
                    // build a simplified HVCC header
					// full HVCC is complex; we'll use a minimal version that works with our decoder

					// calculate total size

					size_t totalSize = 23;

					for (size_t i = 0; i < paramSets.size(); ++i)
					{
						totalSize += 3 + 2 + paramSets[i].second;
					}

					codecConfigData.reserve(totalSize);

					for (int i = 0; i < 22; ++i)
					{
						codecConfigData.push_back(0);
					}

					codecConfigData[0] = 1; // configurationVersion
					codecConfigData.push_back(uint8_t(paramSets.size())); // numOfArrays

					for (size_t i = 0; i < paramSets.size(); ++i)
					{
                        // array_completeness and NAL unit type
						codecConfigData.push_back(0x80 | nalTypes[i]);

                        // numNalus
						codecConfigData.push_back(0);
						codecConfigData.push_back(1);

                        // nalUnitLength (big endian)
						codecConfigData.push_back((paramSets[i].second >> 8) & 0xFF);
						codecConfigData.push_back(paramSets[i].second & 0xFF);

                        // NAL unit data
						codecConfigData.insert(codecConfigData.end(), paramSets[i].first, paramSets[i].first + paramSets[i].second);
					}
				}
			}
		}

		// add codec config sample before the key frame
		if (!codecConfigData.empty())
		{
			Sample configSample(std::move(codecConfigData), presentationTime, BUFFER_FLAG_CODEC_CONFIG);

			const ScopedLock scopedLock(encoder->encodedSamplesLock_);
			encoder->encodedSamples_.push_back(std::move(configSample));
		}
	}

	// create the sample
	Sample sample(std::move(encodedData), presentationTime, bufferFlags);

	// add to the encoded samples queue
	{
		const ScopedLock scopedLock(encoder->encodedSamplesLock_);
		encoder->encodedSamples_.push_back(std::move(sample));
	}
}

CMVideoCodecType VideoEncoder::mimeToCodecType(const std::string& mime)
{
	if (mime == "video/avc" || mime == "video/h264")
	{
		return kCMVideoCodecType_H264;
	}
	else if (mime == "video/hevc" || mime == "video/h265")
	{
		return kCMVideoCodecType_HEVC;
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
