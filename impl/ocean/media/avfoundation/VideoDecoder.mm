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

bool VideoDecoder::initialize(const std::string& mime, const unsigned int width, const unsigned int height, const void* codecConfigData, const size_t codecConfigSize, const DecodingMode decodingMode)
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
	decodingMode_ = decodingMode;

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

#ifdef OCEAN_DEBUG
	ocean_assert(debugPreviousSubmittedTimestamp_ <= int64_t(presentationTime));
	debugPreviousSubmittedTimestamp_ = int64_t(presentationTime);
#endif

	if (decodingMode_ == DM_ORDERED)
	{
		const ScopedLock decodedLock(decodedFramesLock_);

		pendingSampleTimestamps_.push_back(int64_t(presentationTime));
	}

	// store the presentation time for the callback
	int64_t* presentationTimePtr = new int64_t(int64_t(presentationTime));

	VTDecodeFrameFlags decodeFlags = kVTDecodeFrame_EnableAsynchronousDecompression;

	if (decodingMode_ == DM_ORDERED)
	{
		decodeFlags |= kVTDecodeFrame_EnableTemporalProcessing;
	}

	VTDecodeInfoFlags infoFlagsOut = 0;
	status = VTDecompressionSessionDecodeFrame(*decompressionSession_, *scopedSampleBuffer, decodeFlags, presentationTimePtr, &infoFlagsOut);

	if (status != noErr)
	{
		delete presentationTimePtr;

		if (decodingMode_ == DM_ORDERED)
		{
			const ScopedLock decodedLock(decodedFramesLock_);

			if (!pendingSampleTimestamps_.empty())
			{
				ocean_assert(pendingSampleTimestamps_.back() == int64_t(presentationTime));
				pendingSampleTimestamps_.pop_back();
			}
			else
			{
				ocean_assert(false && "This should never happen");
			}
		}

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
		pendingSampleTimestamps_.clear();
		deferredFrames_.clear();
	}

	width_ = 0u;
	height_ = 0u;

#ifdef OCEAN_DEBUG
	debugPreviousSubmittedTimestamp_ = NumericT<int64_t>::minValue();
	debugPreviousDecodedTimestamp_ = NumericT<int64_t>::minValue();
#endif
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
		// provide friendly error messages for common VideoToolbox errors
		switch (status)
		{
			case -12909: // kVTVideoDecoderBadDataErr
				Log::debug() << "VideoDecoder: Skipping frame - waiting for keyframe (error -12909)";
				break;

			case -12911: // kVTVideoDecoderMalfunctionErr
				Log::error() << "VideoDecoder: Decoder malfunction (error -12911)";
				break;

			case -12903: // kVTVideoDecoderNotAvailableNowErr
				Log::warning() << "VideoDecoder: Decoder temporarily unavailable (error -12903)";
				break;

			case -8969: // kVTFormatDescriptionChangeNotSupportedErr
				Log::error() << "VideoDecoder: Format change not supported (error -8969)";
				break;

			case -12910: // kVTVideoDecoderReferenceMissingErr
				Log::debug() << "VideoDecoder: Skipping frame - missing reference frame (error -12910)";
				break;

			default:
				Log::error() << "VideoDecoder: Decompression error: " << status;
				break;
		}

		if (decoder->decodingMode_ == DM_ORDERED)
		{
			decoder->removePendingSampleTimestamps(presentationTime);
		}

		return;
	}

	if (imageBuffer == nullptr)
	{
		Log::warning() << "VideoDecoder: Null image buffer in callback";

		if (decoder->decodingMode_ == DM_ORDERED)
		{
			decoder->removePendingSampleTimestamps(presentationTime);
		}

		return;
	}

	PixelBufferAccessor pixelBufferAccessor(imageBuffer, true);

	if (!pixelBufferAccessor)
	{
		Log::error() << "VideoDecoder: Failed to access pixel buffer";

		if (decoder->decodingMode_ == DM_ORDERED)
		{
			decoder->removePendingSampleTimestamps(presentationTime);
		}

		return;
	}

	const Frame& frame = pixelBufferAccessor.frame();

	if (!frame.isValid())
	{
		Log::error() << "VideoDecoder: Failed to extract frame from pixel buffer";

		if (decoder->decodingMode_ == DM_ORDERED)
		{
			decoder->removePendingSampleTimestamps(presentationTime);
		}

		return;
	}

	// make an owning copy of the frame

	DecodedFrame decodedFrame;
	decodedFrame.frame_ = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	decodedFrame.frame_.setTimestamp(Timestamp(true));
	decodedFrame.frame_.setRelativeTimestamp(Timestamp(Timestamp::microseconds2seconds(presentationTime)));
	decodedFrame.presentationTime_ = presentationTime;

	decoder->onNewDecodedFrame(std::move(decodedFrame));
}

void VideoDecoder::removePendingSampleTimestamps(const int64_t presentationTime)
{
	const ScopedLock scopedLock(decodedFramesLock_);

	for (TimestampQueue::iterator iTimestamp = pendingSampleTimestamps_.begin(); iTimestamp != pendingSampleTimestamps_.end(); ++iTimestamp)
	{
		if (*iTimestamp == presentationTime)
		{
			const bool isFrontTimestamp = iTimestamp == pendingSampleTimestamps_.begin();

			pendingSampleTimestamps_.erase(iTimestamp);

			if (isFrontTimestamp)
			{
				// removing the front timestamp may unblock deferred frames that are now next in line
				processDeferredFrames();
			}

			return;
		}
	}

	ocean_assert(false && "Presentation time not found");
}

void VideoDecoder::onNewDecodedFrame(DecodedFrame&& decodedFrame)
{
	const ScopedLock scopedLock(decodedFramesLock_);

	if (decodingMode_ == DM_ORDERED)
	{
		// we need to ensure that a decoded frame is not delivered out of order

		ocean_assert(!pendingSampleTimestamps_.empty());
		if (pendingSampleTimestamps_.empty())
		{
			Log::error() << "VideoDecoder: Received decoded frame with no pending timestamps";
			return;
		}

		if (pendingSampleTimestamps_.front() != decodedFrame.presentationTime_)
		{
			// the frame is newer than the oldest pending frame, so we need to store the frame for later

			deferredFrames_.push_back(std::move(decodedFrame));

			return;
		}

		pendingSampleTimestamps_.pop_front();

		// push the current frame first, then drain any deferred frames that are now next in line
		decodedFrames_.push_back(std::move(decodedFrame));

		processDeferredFrames();

		return;
	}

	decodedFrames_.push_back(std::move(decodedFrame));
}

void VideoDecoder::processDeferredFrames()
{
	// decodedFramesLock_ must be held by the caller

	bool foundMatchingFrame = true;

	while (foundMatchingFrame && !pendingSampleTimestamps_.empty())
	{
		foundMatchingFrame = false;

		for (size_t nDeferred = 0; nDeferred < deferredFrames_.size(); ++nDeferred)
		{
			if (deferredFrames_[nDeferred].presentationTime_ == pendingSampleTimestamps_.front())
			{
				decodedFrames_.push_back(std::move(deferredFrames_[nDeferred]));
				deferredFrames_[nDeferred] = std::move(deferredFrames_.back());
				deferredFrames_.pop_back();

				pendingSampleTimestamps_.pop_front();

				foundMatchingFrame = true;

				break;
			}
		}
	}
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

bool VideoDecoder::convertAnnexBToAvcc(const void* annexBData, const size_t annexBSize, std::vector<uint8_t>& avccData, const bool isCodecConfig, const std::string& mime)
{
	ocean_assert(annexBData != nullptr && annexBSize >= 4);

	if (annexBData == nullptr || annexBSize < 4)
	{
		return false;
	}

	const uint8_t* data = static_cast<const uint8_t*>(annexBData);

	// first pass: find all NAL units by locating start codes (00 00 00 01 or 00 00 01)
	std::vector<std::pair<size_t, size_t>> nalUnitOffsets;

	size_t offset = 0;
	while (offset < annexBSize)
	{
		size_t startCodeSize = 0;

		if (offset + 4 <= annexBSize && data[offset] == 0x00 && data[offset + 1] == 0x00 && data[offset + 2] == 0x00 && data[offset + 3] == 0x01)
		{
			startCodeSize = 4;
		}
		else if (offset + 3 <= annexBSize && data[offset] == 0x00 && data[offset + 1] == 0x00 && data[offset + 2] == 0x01)
		{
			startCodeSize = 3;
		}

		if (startCodeSize > 0)
		{
			const size_t nalStart = offset + startCodeSize;
			size_t nalEnd = annexBSize;

			for (size_t i = nalStart; i + 2 < annexBSize; ++i)
			{
				if (data[i] == 0x00 && data[i + 1] == 0x00 && (data[i + 2] == 0x01 || (i + 3 < annexBSize && data[i + 2] == 0x00 && data[i + 3] == 0x01)))
				{
					nalEnd = i;
					break;
				}
			}

			if (nalEnd > nalStart)
			{
				nalUnitOffsets.push_back(std::make_pair(nalStart, nalEnd - nalStart));
			}

			offset = nalEnd;
		}
		else
		{
			++offset;
		}
	}

	if (nalUnitOffsets.empty())
	{
		return false;
	}

	if (!isCodecConfig)
	{
		avccData.clear();
		avccData.reserve(annexBSize);

		for (const auto& nalOffset : nalUnitOffsets)
		{
			const size_t nalSize = nalOffset.second;

			avccData.push_back(uint8_t((nalSize >> 24) & 0xFF));
			avccData.push_back(uint8_t((nalSize >> 16) & 0xFF));
			avccData.push_back(uint8_t((nalSize >> 8) & 0xFF));
			avccData.push_back(uint8_t(nalSize & 0xFF));

			avccData.insert(avccData.end(), data + nalOffset.first, data + nalOffset.first + nalSize);
		}

		return true;
	}

	const bool isHevc = (mime == "video/hevc" || mime == "video/h265");

	if (isHevc)
	{
		std::vector<std::pair<const uint8_t*, size_t>> vpsNalUnits;
		std::vector<std::pair<const uint8_t*, size_t>> spsNalUnits;
		std::vector<std::pair<const uint8_t*, size_t>> ppsNalUnits;

		for (const auto& nalOffset : nalUnitOffsets)
		{
			const uint8_t* nalData = data + nalOffset.first;
			const size_t nalSize = nalOffset.second;

			if (nalSize < 2)
			{
				continue;
			}

			const uint8_t nalType = (nalData[0] >> 1) & 0x3F;

			if (nalType == 32)
			{
				vpsNalUnits.push_back(std::make_pair(nalData, nalSize));
			}
			else if (nalType == 33)
			{
				spsNalUnits.push_back(std::make_pair(nalData, nalSize));
			}
			else if (nalType == 34)
			{
				ppsNalUnits.push_back(std::make_pair(nalData, nalSize));
			}
		}

		if (spsNalUnits.empty() || ppsNalUnits.empty())
		{
			return false;
		}

		// build HVCC record: [22-byte header][num arrays][[array type][num nalus][nalu length][nalu data]...]
		avccData.clear();

		for (size_t i = 0; i < 22; ++i)
		{
			avccData.push_back(0);
		}

		avccData[0] = 1;
		avccData[21] = 0x03;

		uint8_t numArrays = 0;
		if (!vpsNalUnits.empty())
		{
			++numArrays;
		}

		++numArrays;
		++numArrays;

		avccData.push_back(numArrays);

		if (!vpsNalUnits.empty())
		{
			avccData.push_back(0x80 | 32);
			avccData.push_back(uint8_t((vpsNalUnits.size() >> 8) & 0xFF));
			avccData.push_back(uint8_t(vpsNalUnits.size() & 0xFF));

			for (const auto& vps : vpsNalUnits)
			{
				avccData.push_back(uint8_t((vps.second >> 8) & 0xFF));
				avccData.push_back(uint8_t(vps.second & 0xFF));
				avccData.insert(avccData.end(), vps.first, vps.first + vps.second);
			}
		}

		avccData.push_back(0x80 | 33);
		avccData.push_back(uint8_t((spsNalUnits.size() >> 8) & 0xFF));
		avccData.push_back(uint8_t(spsNalUnits.size() & 0xFF));

		for (const auto& sps : spsNalUnits)
		{
			avccData.push_back(uint8_t((sps.second >> 8) & 0xFF));
			avccData.push_back(uint8_t(sps.second & 0xFF));
			avccData.insert(avccData.end(), sps.first, sps.first + sps.second);
		}

		avccData.push_back(0x80 | 34);
		avccData.push_back(uint8_t((ppsNalUnits.size() >> 8) & 0xFF));
		avccData.push_back(uint8_t(ppsNalUnits.size() & 0xFF));

		for (const auto& pps : ppsNalUnits)
		{
			avccData.push_back(uint8_t((pps.second >> 8) & 0xFF));
			avccData.push_back(uint8_t(pps.second & 0xFF));
			avccData.insert(avccData.end(), pps.first, pps.first + pps.second);
		}

		return true;
	}

	const uint8_t* spsData = nullptr;
	size_t spsSize = 0;
	const uint8_t* ppsData = nullptr;
	size_t ppsSize = 0;

	for (const auto& nalOffset : nalUnitOffsets)
	{
		const uint8_t* nalData = data + nalOffset.first;
		const size_t nalSize = nalOffset.second;

		if (nalSize < 1)
		{
			continue;
		}

		const uint8_t nalType = nalData[0] & 0x1F;

		if (nalType == 7 && spsData == nullptr)
		{
			spsData = nalData;
			spsSize = nalSize;
		}
		else if (nalType == 8 && ppsData == nullptr)
		{
			ppsData = nalData;
			ppsSize = nalSize;
		}
	}

	if (spsData == nullptr || spsSize < 4 || ppsData == nullptr || ppsSize < 1)
	{
		return false;
	}

	// build AVCC record: [version][profile][compat][level][0xFF][0xE1][sps length][sps][num pps][pps length][pps]
	avccData.clear();
	avccData.reserve(11 + spsSize + ppsSize);

	avccData.push_back(1);
	avccData.push_back(spsData[1]);
	avccData.push_back(spsData[2]);
	avccData.push_back(spsData[3]);
	avccData.push_back(0xFF);
	avccData.push_back(0xE1);

	avccData.push_back(uint8_t((spsSize >> 8) & 0xFF));
	avccData.push_back(uint8_t(spsSize & 0xFF));
	avccData.insert(avccData.end(), spsData, spsData + spsSize);

	avccData.push_back(1);

	avccData.push_back(uint8_t((ppsSize >> 8) & 0xFF));
	avccData.push_back(uint8_t(ppsSize & 0xFF));
	avccData.insert(avccData.end(), ppsData, ppsData + ppsSize);

	return true;
}

bool VideoDecoder::isAnnexB(const void* data, const size_t size, const bool isCodecConfig)
{
	ocean_assert(data != nullptr && size >= 4);

	if (data == nullptr || size < 4)
	{
		return false;
	}

	const uint8_t* byteData = static_cast<const uint8_t*>(data);

	if (isCodecConfig)
	{
		// For codec configuration data:
		// - AVCC config starts with version byte 0x01
		// - Annex B config starts with start codes (0x00 0x00 0x00 0x01 or 0x00 0x00 0x01)
		if (byteData[0] == 0x00 && byteData[1] == 0x00 && byteData[2] == 0x00 && byteData[3] == 0x01)
		{
			return true; // Annex B 4-byte start code
		}

		if (byteData[0] == 0x00 && byteData[1] == 0x00 && byteData[2] == 0x01)
		{
			return true; // Annex B 3-byte start code
		}

		// AVCC config starts with 0x01 (configurationVersion)
		return false;
	}

	// For regular NAL unit samples:
	// Need more sophisticated detection because AVCC length prefix for NAL units
	// with sizes 256-511 bytes is 0x00 0x00 0x01 XX, which looks like Annex B 3-byte start code

	if (byteData[0] == 0x00 && byteData[1] == 0x00 && byteData[2] == 0x00 && byteData[3] == 0x01)
	{
		// Definitely Annex B 4-byte start code (AVCC length of 1 byte NAL unit is impossible in practice)
		return true;
	}

	if (byteData[0] == 0x00 && byteData[1] == 0x00 && byteData[2] == 0x01)
	{
		// Could be Annex B 3-byte start code, or AVCC with NAL size 256-511
		// Check if treating as AVCC length makes sense
		const uint32_t possibleLength = (uint32_t(byteData[0]) << 24) | (uint32_t(byteData[1]) << 16) |
		                                (uint32_t(byteData[2]) << 8) | uint32_t(byteData[3]);

		if (possibleLength > 0 && possibleLength <= size - 4)
		{
			// Valid AVCC length - this is AVCC format, not Annex B
			return false;
		}

		// Invalid as AVCC, must be Annex B
		return true;
	}

	// Doesn't start with a recognized Annex B pattern, assume AVCC
	return false;
}

}

}

}
