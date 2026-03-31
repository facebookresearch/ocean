/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_VIDEO_DECODER_H
#define META_OCEAN_MEDIA_AVF_VIDEO_DECODER_H

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

#include "ocean/math/Numeric.h"

#include <VideoToolbox/VideoToolbox.h>

#include <deque>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements a simple video decoder for iOS/macOS using encoded media samples from memory as input.
 * The decoder uses Apple's VideoToolbox framework (VTDecompressionSession) for hardware-accelerated decoding.
 *
 * Usage:
 * @code
 * // a function which is e.g., running in a separate thread
 * void threadRun()
 * {
 *     VideoDecoder videoDecoder;
 *
 *     // initializing the decoder with the input format of the media samples
 *     if (!videoDecoder.initialize("video/avc", 1920u, 1080u))
 *     {
 *         // handle error
 *     }
 *
 *     if (!videoDecoder.start())
 *     {
 *         // handle error
 *     }
 *
 *     unsigned int frameIndex = 0u;
 *     double frameRate = 30.0;
 *
 *     while (true)
 *     {
 *         void* sampleData = nullptr;
 *         size_t sampleSize = 0;
 *
 *         // external function: function needs to provide the new media samples from an external source - e.g., from an external webcam, a video stream, etc.
 *         if (doesNewInputSampleExist(sampleData, &sampleSize))
 *         {
 *             // presentation time in microseconds
 *             uint64_t presentationTime = uint64_t(1.0e6 * double(frameIndex) / frameRate);
 *
 *             // we forward the media sample to the decoder, eventually it will be decoded and will be available through decodedFrame()
 *             if (!videoDecoder.pushSample(sampleData, sampleSize, presentationTime))
 *             {
 *                 // handle error
 *             }
 *
 *             ++frameIndex;
 *         }
 *
 *         // we simply check whether another frame has been decoded (there may be a delay between
 *         Frame newFrame = videoDecoder.popFrame();
 *
 *         if (newFrame.isValid())
 *         {
 *             // external function: receiving new frames and processes the frames
 *             sendFrameToReceiver(std::move(newFrame));
 *         }
 *     }
 * }
 * @endcode
 * @ingroup mediaavf
 */
class VideoDecoder
{
	public:

		/**
		 * Definition of the decoding mode controlling frame delivery order and latency.
		 */
		enum DecodingMode
		{
			/// Frames are decoded and delivered with minimal latency, frame ordering is not guaranteed, decoded frames may arrive out of presentation-timestamp order but as fast as possible.
			/// Use this when low latency is more important than frame order.
			DM_PERFORMANCE,

			/// Frames are decoded and delivered in presentation-timestamp order, but frame delivery may be delayed.
			DM_ORDERED
		};

    protected:

        /**
		 * Definition of a scoped object holding a CMBlockBufferRef object.
		 * The wrapped CMBlockBufferRef object will be released automatically once the scoped object does not exist anymore.
		 */
		using ScopedCMBlockBufferRef = ScopedObjectCompileTimeT<CMBlockBufferRef, CFTypeRef, void, CFRelease>;

		/**
		 * Definition of a scoped object holding a CMFormatDescriptionRef object.
		 * The wrapped CMFormatDescriptionRef object will be released automatically once the scoped object does not exist anymore.
		 */
		using ScopedCMFormatDescriptionRef = ScopedObjectCompileTimeT<CMFormatDescriptionRef, CFTypeRef, void, CFRelease>;

		/**
		 * Release function for VTDecompressionSessionRef that invalidates and releases the session.
		 * @param session The session to release
		 */
		static inline void releaseVTDecompressionSession(VTDecompressionSessionRef session);

		/**
		* Definition of a scoped object holding a VTDecompressionSessionRef object.
		* The wrapped VTDecompressionSessionRef object will be invalidated and released automatically once the scoped object does not exist anymore.
		*/
		using ScopedVTDecompressionSessionRef = ScopedObjectCompileTimeT<VTDecompressionSessionRef, VTDecompressionSessionRef, void, releaseVTDecompressionSession>;

        /**
		 * Definition of a decoded frame entry.
		 */
		struct DecodedFrame
		{
			/// The decoded frame.
			Frame frame_;

			/// The presentation time in microseconds.
			int64_t presentationTime_ = 0;
		};

		/// Definition of a vector holding decoded frames.
		using DecodedFrames = std::vector<DecodedFrame>;

		/// Definition of a queue holding decoded frames.
		using DecodedFrameQueue = std::deque<DecodedFrame>;

		/// Definition of a queue holding presentation timestamps.
		using TimestampQueue = std::deque<int64_t>;

	public:

		/**
		 * Default constructor creating an un-initialized decoder.
		 */
		VideoDecoder();

		/**
		 * Destructs the video decoder and releases all associated resources.
		 */
		~VideoDecoder();

		/**
		 * Initializes the video decoder with codec configuration data (SPS/PPS for H.264, VPS/SPS/PPS for HEVC).
		 * @param mime The MIME type (Multipurpose Internet Mail Extensions) of the video to be decoded, e.g., "video/avc", "video/hevc", ...
		 * @param width The width of the video to be decoded, in pixel, with range [1, infinity)
		 * @param height The height of the video to be decoded, in pixel, with range [1, infinity)
		 * @param codecConfigData The codec configuration data containing parameter sets (SPS/PPS for H.264), can be nullptr if not available yet
		 * @param codecConfigSize The size of the codec configuration data in bytes, 0 if not available yet
		 * @param decodingMode The decoding mode controlling frame delivery order, default is DM_PERFORMANCE
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const std::string& mime, const unsigned int width, const unsigned int height, const void* codecConfigData = nullptr, const size_t codecConfigSize = 0, const DecodingMode decodingMode = DM_PERFORMANCE);

		/**
		 * Starts the video decoder.
		 * @return True, if succeeded
		 * @see isStarted().
		 */
		bool start();

		/**
		 * Stops the video decoder.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Adds a new media sample which needs to be decoded to the video decoder.
		 * The decoder needs to be initialized and started.
		 * The presentation time is mainly intended to allow associating the provided encoded media sample with the resulting decoded frame when calling popFrame().
		 * However, it's recommended to define a reasonable presentation time for each sample (e.g., let the first sample start at 0 and increment the time by 1^6/fps for each following sample.
		 * @param data The data of the encoded media sample, must be valid
		 * @param size The size of the encoded media sample, in bytes, with range [1, infinity)
		 * @param presentationTime The presentation time of the sample, in microseconds, with range [0, infinity)
		 * @return True, if succeeded
		 * @see start(), isInitialized(), isStarted().
		 */
		bool pushSample(const void* data, const size_t size, const uint64_t presentationTime);

		/**
		 * Pops the next decoded frame from the decoder.
		 * Optional the frame's presentation time will be returned, this is the presentation time which was used when the corresponding sample was provided in pushSample().
		 * @param presentationTime Optional resulting presentation time in micro seconds, with range (-infinity, infinity)
		 * @return The resulting frame, invalid if currently no decoded frame is available
		 * @see pushSample().
		 */
		Frame popFrame(int64_t* presentationTime = nullptr);

		/**
		 * Returns whether this decoder is initialized.
		 * @return True, if so
		 * @see initialize().
		 */
		inline bool isInitialized() const;

		/**
		 * Returns whether this decoder is currently running.
		 * @return True, if so
		 * @see start().
		 */
		inline bool isStarted() const;

		/**
		 * Explicitly releases this video decoder.
		 * If the decoder is still running, the decoder will be stopped as well.
		 */
		void release();

		/**
		 * Converts Annex B formatted H.264/H.265 data to AVCC/HVCC format.
		 * For encoded samples (isCodecConfig = false): Replaces start code prefixes (00 00 00 01 or 00 00 01) with 4-byte big-endian length prefixes.
		 * For codec config (isCodecConfig = true): Extracts SPS/PPS (and VPS for HEVC) NAL units and builds an AVCC/HVCC configuration record.
		 * @param annexBData The Annex B formatted data containing NAL units with start code prefixes, must be valid
		 * @param annexBSize The size of the Annex B data in bytes, with range [4, infinity)
		 * @param avccData The resulting AVCC/HVCC formatted data
		 * @param isCodecConfig True to build an AVCC/HVCC codec configuration record from the NAL units; False to simply replace start codes with length prefixes
		 * @param mime The MIME type, used only when isCodecConfig is true to determine H.264 vs HEVC format, either "video/avc" or "video/hevc"
		 * @return True if conversion succeeded; False if the input data is invalid or conversion failed
		 */
		static bool convertAnnexBToAvcc(const void* annexBData, const size_t annexBSize, std::vector<uint8_t>& avccData, const bool isCodecConfig = false, const std::string& mime = "video/avc");

		/**
		 * Determines whether the given data is in Annex B format (start code prefixed) or AVCC format (length prefixed).
		 * Annex B format uses start codes (0x00 0x00 0x00 0x01 or 0x00 0x00 0x01) to delimit NAL units.
		 * AVCC format uses 4-byte big-endian length prefixes before each NAL unit.
		 *
		 * Note: For codec configuration data, use isCodecConfig=true as AVCC config starts with version byte 0x01.
		 * For regular NAL unit samples, use isCodecConfig=false which applies more sophisticated detection
		 * to distinguish AVCC length prefixes from Annex B start codes (especially for NAL sizes 256-511 bytes
		 * where the length prefix 0x00 0x00 0x01 XX looks like an Annex B 3-byte start code).
		 *
		 * @param data The data to check, must be valid
		 * @param size The size of the data in bytes, with range [4, infinity)
		 * @param isCodecConfig True if the data is codec configuration (SPS/PPS), false for regular NAL samples
		 * @return True if the data is in Annex B format; false if it's in AVCC format
		 */
		static bool isAnnexB(const void* data, const size_t size, const bool isCodecConfig = false);

		/**
		 * Move operator.
		 * @param videoDecoder The video decoder to be moved
		 * @return Reference to this object
		 */
		inline VideoDecoder& operator=(VideoDecoder&& videoDecoder) noexcept;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		VideoDecoder(const VideoDecoder&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		VideoDecoder& operator=(const VideoDecoder&) = delete;

		/**
		 * Handles a newly decoded frame by either delivering it directly or deferring it for later delivery in DM_ORDERED mode.
		 * @param decodedFrame The decoded frame to be processed
		 */
		void onNewDecodedFrame(DecodedFrame&& decodedFrame);

		/**
		 * Removes a presentation timestamp from the pending sample timestamps queue.
		 * This is called when a frame fails to decode, so that the failed frame does not block delivery of subsequent frames.
		 * @param presentationTime The presentation time to remove, in microseconds
		 */
		void removePendingSampleTimestamps(const int64_t presentationTime);

		/**
		 * Processes deferred frames whose presentation timestamps now match the front of the pending timestamps queue.
		 */
		void processDeferredFrames();

		/**
		 * Callback function for decoded frames from VideoToolbox.
		 * @param decompressionOutputRefCon Reference to this decoder
		 * @param sourceFrameRefCon Reference containing the presentation time
		 * @param status The status of the decompression operation
		 * @param infoFlags Information flags
		 * @param imageBuffer The decoded image buffer, may be nullptr on error
		 * @param presentationTimeStamp The presentation time stamp
		 * @param presentationDuration The presentation duration
		 */
		static void decompressionOutputCallback(void* decompressionOutputRefCon, void* sourceFrameRefCon, OSStatus status, VTDecodeInfoFlags infoFlags, CVImageBufferRef imageBuffer, CMTime presentationTimeStamp, CMTime presentationDuration);

		/**
		 * Translates a MIME type to a CMVideoCodecType.
		 * @param mime The MIME type
		 * @return The corresponding codec type, 0 if not supported
		 */
		static CMVideoCodecType mimeToCodecType(const std::string& mime);

	protected:

		/// The video format description.
		ScopedCMFormatDescriptionRef formatDescription_;

		/// The decompression session.
		ScopedVTDecompressionSessionRef decompressionSession_;

		/// The queue timestamps of submitted but not yet processed samples.
		TimestampQueue pendingSampleTimestamps_;

		/// The queue of decoded frames ready for consumption.
		DecodedFrameQueue decodedFrames_;

		/// Decoded frames waiting to be delivered in presentation-timestamp order (DM_ORDERED mode only).
		DecodedFrames deferredFrames_;

		/// The width of the video.
		unsigned int width_ = 0u;

		/// The height of the video.
		unsigned int height_ = 0u;

		/// The decoding mode.
		DecodingMode decodingMode_ = DM_PERFORMANCE;

		/// True, if the decoder is currently started.
		bool isStarted_ = false;

		/// The decoder's lock.
		mutable Lock lock_;

		/// The lock for the decoded frames queue.
		mutable Lock decodedFramesLock_;

#ifdef OCEAN_DEBUG
		/// The previous presentation timestamp submitted via pushSample(), in microseconds, NumericT<int64_t>::minValue() if no sample has been submitted yet.
		int64_t debugPreviousSubmittedTimestamp_ = NumericT<int64_t>::minValue();

		/// The previous presentation timestamp of a decoded frame released to decodedFrames_, in microseconds, NumericT<int64_t>::minValue() if no frame has been released yet.
		int64_t debugPreviousDecodedTimestamp_ = NumericT<int64_t>::minValue();
#endif
};

inline bool VideoDecoder::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return decompressionSession_.isValid();
}

inline bool VideoDecoder::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!isStarted_ || isInitialized());

	return isStarted_;
}

inline void VideoDecoder::releaseVTDecompressionSession(VTDecompressionSessionRef session)
{
	if (session != nullptr)
	{
		VTDecompressionSessionInvalidate(session);
		CFRelease(session);
	}
}

}

}

}

#endif // META_OCEAN_MEDIA_AVF_VIDEO_DECODER_H
