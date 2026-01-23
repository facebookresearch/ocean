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

#include <VideoToolbox/VideoToolbox.h>

#include <deque>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * Definition of a scoped object holding a CMFormatDescriptionRef object.
 * The wrapped CMFormatDescriptionRef object will be released automatically once the scoped object does not exist anymore.
 */
using ScopedCMFormatDescriptionRef = ScopedObjectCompileTimeT<CMFormatDescriptionRef, CFTypeRef, void, CFRelease>;

/**
 * Release function for VTDecompressionSessionRef that invalidates and releases the session.
 * @param session The session to release
 */
inline void releaseVTDecompressionSession(VTDecompressionSessionRef session)
{
	if (session != nullptr)
	{
		VTDecompressionSessionInvalidate(session);
		CFRelease(session);
	}
}

/**
 * Definition of a scoped object holding a VTDecompressionSessionRef object.
 * The wrapped VTDecompressionSessionRef object will be invalidated and released automatically once the scoped object does not exist anymore.
 */
using ScopedVTDecompressionSessionRef = ScopedObjectCompileTimeT<VTDecompressionSessionRef, VTDecompressionSessionRef, void, releaseVTDecompressionSession>;

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
    protected:

        /**
		 * Definition of a scoped object holding a CMBlockBufferRef object.
		 * The wrapped CMBlockBufferRef object will be released automatically once the scoped object does not exist anymore.
		 */
		using ScopedCMBlockBufferRef = ScopedObjectCompileTimeT<CMBlockBufferRef, CFTypeRef, void, CFRelease>;

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

	public:

		/**
		 * Default constructor creating an un-initialized decoder.
		 */
		VideoDecoder();

		/**
		 * Move constructor.
		 * @param videoDecoder The decoder to be moved
		 */
		inline VideoDecoder(VideoDecoder&& videoDecoder) noexcept;

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
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const std::string& mime, const unsigned int width, const unsigned int height, const void* codecConfigData = nullptr, const size_t codecConfigSize = 0);

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

		/// The queue of decoded frames.
		std::deque<DecodedFrame> decodedFrames_;

		/// The width of the video.
		unsigned int width_ = 0u;

		/// The height of the video.
		unsigned int height_ = 0u;

		/// True, if the decoder is currently started.
		bool isStarted_ = false;

		/// The decoder's lock.
		mutable Lock lock_;

		/// The lock for the decoded frames queue.
		mutable Lock decodedFramesLock_;
};

inline VideoDecoder::VideoDecoder(VideoDecoder&& videoDecoder) noexcept
{
	*this = std::move(videoDecoder);
}

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

inline VideoDecoder& VideoDecoder::operator=(VideoDecoder&& videoDecoder) noexcept
{
	if (this != &videoDecoder)
	{
		release();

		formatDescription_ = std::move(videoDecoder.formatDescription_);
		decompressionSession_ = std::move(videoDecoder.decompressionSession_);

		decodedFrames_ = std::move(videoDecoder.decodedFrames_);

		width_ = videoDecoder.width_;
		videoDecoder.width_ = 0u;

		height_ = videoDecoder.height_;
		videoDecoder.height_ = 0u;

		isStarted_ = videoDecoder.isStarted_;
		videoDecoder.isStarted_ = false;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_MEDIA_AVF_VIDEO_DECODER_H
