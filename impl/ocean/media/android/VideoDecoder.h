/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_VIDEO_DECODER_H
#define META_OCEAN_MEDIA_ANDROID_VIDEO_DECODER_H

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#ifndef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE
	#define OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE
#endif

#include "ocean/media/android/Android.h"
#include "ocean/media/android/NativeMediaLibrary.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements a simple video decoder for Android using encoded media samples from memory as input.
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
  *            // presentation time in microseconds
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
 * @ingroup mediaandroid
 */
class VideoDecoder
{
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
		 * @param mime The MIME type (Multipurpose Internet Mail Extensions) of the video to be decoded, e.g., "video/avc", "video/hevc", ...
		 * @param width The width of the video to be decoded, in pixel, with range [1, infinity)
		 * @param height The height of the video to be decoded, in pixel, with range [1, infinity)
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const std::string& mime, const unsigned int width, const unsigned int height);

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
		 * The decoder needs to be initialized and started.<br>
		 * The presentation time is mainly intended to allow associating the provided encoded media sample with the resulting decoded frame when calling popFrame().<br>
		 * However, it's recommended to define a reasonable presentation time for each sample (e.g., let the first sample start at 0 and increment the time by 1^6/fps for each following sample.
		 * @param data The data of the encoded media sample, must be valid
		 * @param size The size of the encoded media sample, in bytes, with range [1, infinity)
		 * @param presentationTime The presentation time of the sample, in microseconds, with range [0, infinity)
		 * @return True, if succeeded
		 * @see start(), isInitialized(), isStarted().
		 */
		bool pushSample(const void* data, const size_t size, const uint64_t presentationTime);

		/**
		 * Optional the frame's presentation time will be returned, this is the presentation time which was used when the corresponding sample was provided in decodedSample().
		 * @param presentationTime Optional resulting presentation time in micro seconds, with range (-infinity, infinity)
		 * @return The resulting frame, invalid if currently no decoded frame is available
		 * @see pushSample().
		 */
		Frame popFrame(int64_t* presentationTime = nullptr);

		/**
		 * Returns whether this decode is initialized.
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
		 * Explicitly releases this video encoder.
		 * If the encoder is still running, the encoder will be stopped as well.
		 */
		void release();

		/**
		 * Move operator.
		 * @param videoDecoder The video decoder to be moved
		 * @return Reference to this object
		 */
		inline VideoDecoder& operator=(VideoDecoder&& videoDecoder) noexcept;

		/**
		 * Extracts the video frame from an output buffer of a video codec.
		 * @param mediaCodec The media codec to which the output buffer belongs, must be valid
		 * @param presentationTime Optional resulting presentation time in micro seconds, with range (-infinity, infinity)
		 * @return The resulting extracted frame, invalid if the frame could not be extracted
		 */
		static Frame extractVideoFrameFromCodecOutputBuffer(AMediaCodec* const mediaCodec, int64_t* presentationTime = nullptr);

		/**
		 * Converts AVCC/HVCC formatted H.264/H.265 data to Annex B format.
		 * For encoded samples (isCodecConfig = false): Replaces 4-byte big-endian length prefixes with start code prefixes (00 00 00 01).
		 * For codec config (isCodecConfig = true): Parses the AVCC/HVCC configuration record and extracts SPS/PPS (and VPS for HEVC) NAL units with start codes.
		 * @param avccData The AVCC/HVCC formatted data, must be valid
		 * @param avccSize The size of the AVCC/HVCC data in bytes, with range [4, infinity)
		 * @param annexBData The resulting Annex B formatted data with start code prefixes
		 * @param isCodecConfig True if the input is an AVCC/HVCC codec configuration record; False if it contains length-prefixed NAL units
		 * @param mime The MIME type, used only when isCodecConfig is true to determine H.264 vs HEVC format, either "video/avc" or "video/hevc"
		 * @return True if conversion succeeded; False if the input data is invalid or conversion failed
		 */
		static bool convertAvccToAnnexB(const void* avccData, const size_t avccSize, std::vector<uint8_t>& annexBData, const bool isCodecConfig = false, const std::string& mime = "video/avc");

		/**
		 * Determines whether the given data is in AVCC format (length prefixed) or Annex B format (start code prefixed).
		 * AVCC format uses 4-byte big-endian length prefixes before each NAL unit.
		 * Annex B format uses start codes (0x00 0x00 0x00 0x01 or 0x00 0x00 0x01) to delimit NAL units.
		 *
		 * Note: For codec configuration data, use isCodecConfig=true as AVCC config starts with version byte 0x01.
		 * For regular NAL unit samples, use isCodecConfig=false which applies more sophisticated detection
		 * to distinguish AVCC length prefixes from Annex B start codes (especially for NAL sizes 256-511 bytes
		 * where the length prefix 0x00 0x00 0x01 XX looks like an Annex B 3-byte start code).
		 *
		 * @param data The data to check, must be valid
		 * @param size The size of the data in bytes, with range [4, infinity)
		 * @param isCodecConfig True if the data is codec configuration (SPS/PPS), false for regular NAL samples
		 * @return True if the data is in AVCC format; false if it's in Annex B format
		 */
		static bool isAvcc(const void* data, const size_t size, const bool isCodecConfig = false);

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

	protected:

		/// The subscription for the native media library.
		NativeMediaLibrary::ScopedSubscription nativeMediaLibrarySubscription_;

		/// The Android media decoder used to decode the video.
		NativeMediaLibrary::ScopedAMediaCodec decoder_;

		/// True, if the decoder is currently started.
		bool isStarted_ = false;

		/// The decoder's lock.
		mutable Lock lock_;
};

inline VideoDecoder::VideoDecoder(VideoDecoder&& videoDecoder) noexcept
{
	*this = std::move(videoDecoder);
}

inline bool VideoDecoder::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return decoder_.isValid();
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

		decoder_ = std::move(videoDecoder.decoder_);

		isStarted_ = videoDecoder.isStarted_;
		videoDecoder.isStarted_ = false;
	}

	return *this;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#endif // META_OCEAN_MEDIA_ANDROID_VIDEO_DECODER_H
