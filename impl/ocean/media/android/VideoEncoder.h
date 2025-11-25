/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_VIDEO_ENCODER_H
#define META_OCEAN_MEDIA_ANDROID_VIDEO_ENCODER_H

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#ifndef OCEAN_MEDIA_ANDROID_VIDEOENCODER_AVAILABLE
	#define OCEAN_MEDIA_ANDROID_VIDEOENCODER_AVAILABLE
#endif

#include "ocean/media/android/Android.h"
#include "ocean/media/android/NativeMediaLibrary.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements a simple video encoder for Android using Ocean::Frame objects as input.
 *
 * Usage:
 * @code
 * // a function which is e.g., running in a separate thread
 * void threadRun()
 * {
 *     VideoEncoder videoEncoder;
 * *
 *     if (!videoEncoder.initialize(1920u, 1080u))
 *     {
 *         // handle error
 *     }
 *
 *     if (!videoEncoder.start())
 *     {
 *         // handle error
 *     }
 *
 *     unsigned int frameIndex = 0u;
 *     double frameRate = 30.0;
 *
 *     while (true)
 *     {
 *         Frame frame;
 *
 *         // external function: function needs to provide frames from an external source - e.g., from a camera, a video stream, etc.
 *         if (doesNewFrameExist(frame))
 *         {
 *             // presentation time in microseconds
 *             uint64_t presentationTime = uint64_t(1.0e6 * double(frameIndex) / frameRate);
 *
 *             // we forward the frame to the encoder, eventually it will be encoded and will be available through popSample()
 *             if (!videoEncoder.pushFrame(frame, presentationTime))
 *             {
 *                 // handle error
 *             }
 *
 *             ++frameIndex;
 *         }
 *
 *         // we simply check whether another sample has been encoded
 *         VideoEncoder::Sample encodedSample = videoEncoder.popSample();
 *
 *         if (encodedSample.isValid())
 *         {
 *             // external function: receiving encoded samples and processes them
 *             sendSampleToReceiver(std::move(encodedSample));
 *         }
 *     }
 * }
 * @endcode
 * @ingroup mediaandroid
 */
class VideoEncoder
{
	public:

		/// Definition of a 1 Mbps bit rate
		static constexpr int bitrateMbps1_ = 1000 * 1000;

		/// Definition of a 2 Mbps bit rate
		static constexpr int bitrateMbps2_ = bitrateMbps1_ * 2;

		/// Definition of a 5 Mbps bit rate
		static constexpr int bitrateMbps5_ = bitrateMbps1_ * 5;

		/// Definition of a 10 Mbps bit rate
		static constexpr int bitrateMbps10_ = bitrateMbps1_ * 10;

		/**
		 * Definition of individual MediaCodec buffer flag constants.
		 * From Android MediaCodec.BufferInfo: https://developer.android.com/reference/android/media/MediaCodec.BufferInfo
		 */
		enum MediaCodecBufferFlags : uint32_t
		{
			/// The buffer has no special property.
			BUFFER_FLAG_NONE = 0u,
			/// Indicates that the (encoded) buffer marked as such contains the data for a key frame.
			BUFFER_FLAG_KEY_FRAME = 1u,
			/// Indicates that the buffer marked as such contains codec initialization / codec specific data instead of media data.
			BUFFER_FLAG_CODEC_CONFIG = 2u,
			/// Indicates that the buffer is the last buffer in the stream.
			BUFFER_FLAG_END_OF_STREAM = 4u,
			/// Indicates that the buffer only contains part of a frame.
			BUFFER_FLAG_PARTIAL_FRAME = 8u
		};

		/**
		 * Definition of an encoded sample.
		 */
		class Sample
		{
			friend class VideoEncoder;

			public:

				/**
				 * Creates an invalid sample.
				 */
				Sample() = default;

				/**
				 * Move constructor.
				 * @param sample The sample to be moved
				 */
				inline Sample(Sample&& sample) noexcept;

				/**
				 * Returns whether this sample is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns the encoded data.
				 * @return The encoded data
				 */
				inline const std::vector<uint8_t>& data() const;

				/**
				 * Returns the presentation time in microseconds.
				 * @return The presentation time
				 */
				inline int64_t presentationTime() const;

				/**
				 * Returns whether this sample is a key frame.
				 * @return True, if so
				 */
				inline bool isKeyFrame() const;

				/**
				 * Returns whether this sample contains codec configuration data instead of media data.
				 * @return True, if so
				 */
				inline bool isConfiguration() const;

				/**
				 * Returns whether this sample marks the end of the stream.
				 * @return True, if so
				 */
				inline bool isEndOfStream() const;

				/**
				 * Returns whether this sample contains only part of a frame.
				 * @return True, if so
				 */
				inline bool isPartialFrame() const;

				/**
				 * Move operator.
				 * @param sample The sample to be moved
				 * @return Reference to this object
				 */
				inline Sample& operator=(Sample&& sample) noexcept;

				/**
				 * Returns whether this sample is valid.
				 * @return True, if so
				 */
				inline explicit operator bool() const;

			protected:

				/**
				 * Creates a sample with specified data.
				 * @param data The encoded data, will be moved
				 * @param presentationTime The presentation time in microseconds, with range [0, infinity)
				 * @param bufferFlags The buffer flags of the sample
				 */
				inline Sample(std::vector<uint8_t>&& data, const int64_t presentationTime, const MediaCodecBufferFlags bufferFlags);

				/**
				 * Disabled copy constructor.
				 */
				Sample(const Sample&) = delete;

				/**
				 * Disabled copy operator.
				 * @return Reference to this object
				 */
				Sample& operator=(const Sample&) = delete;

			protected:

				/// The encoded data.
				std::vector<uint8_t> data_;

				/// The presentation time in microseconds.
				int64_t presentationTime_ = NumericT<int64_t>::minValue();

				MediaCodecBufferFlags bufferFlags_ = BUFFER_FLAG_NONE;
		};

		/**
		 * Definition of a vector holding sample objects.
		 */
		using Samples = std::vector<Media::Android::VideoEncoder::Sample>;

	protected:

		/// Definition of the maximal image width.
		static constexpr unsigned int maximalWidth_ = 1920u * 8u;

		/// Definition of the maximal image height.
		static constexpr unsigned int maximalHeight_ = 1080u * 8u;

		/// Definition of the maximal bit rate.
		static constexpr int maximalBitrate_ = bitrateMbps10_ * 10;

	public:

		/**
		 * Default constructor creating an un-initialized encoder.
		 */
		VideoEncoder();

		/**
		 * Move constructor.
		 * @param videoEncoder The encoder to be moved
		 */
		inline VideoEncoder(VideoEncoder&& videoEncoder) noexcept;

		/**
		 * Destructs the video encoder and releases all associated resources.
		 */
		~VideoEncoder();

		/**
		 * Initializes the video encoder with the specified configuration.
		 * @param width The width of the video to be encoded, in pixel, with range [1, infinity)
		 * @param height The height of the video to be encoded, in pixel, with range [1, infinity)
		 * @param mime The MIME type (Multipurpose Internet Mail Extensions) of the video to be encoded, e.g., "video/avc", "video/hevc", ...
		 * @param frameRate The target frame rate in frames per second, with range (0, infinity), e.g., 30.0
		 * @param bitrate The target bitrate in bits per second, with range [1, infinity), e.g., 5000000 for 5 Mbps
		 * @param iFrameInterval The interval between I-frames (key frames) in seconds: negative value = no key frames after first frame, 0 = all frames are key frames, positive value = key frames every N seconds
		 * @param preferredPixelFormat The preferred pixel format the encoded uses internally
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const unsigned int width, const unsigned int height, const std::string& mime = "video/avc", const double frameRate = 30.0, const unsigned int bitrate = bitrateMbps2_, const int iFrameInterval = 1, const FrameType::PixelFormat preferredPixelFormat = FrameType::FORMAT_Y_UV12_LIMITED_RANGE);

		/**
		 * Starts the video encoder.
		 * @return True, if succeeded
		 * @see isStarted().
		 */
		bool start();

		/**
		 * Stops the video encoder.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Adds a new frame which needs to be encoded to the video encoder.
		 * The encoder needs to be initialized and started.<br>
		 * The presentation time is mainly intended to allow associating the provided frame with the resulting encoded sample when calling popSample().<br>
		 * However, it's recommended to define a reasonable presentation time for each frame (e.g., let the first frame start at 0 and increment the time by 1^6/fps for each following frame).
		 * @param frame The frame to be encoded, must be valid
		 * @param presentationTime The presentation time of the frame, in microseconds, with range [0, infinity)
		 * @return True, if succeeded
		 * @see start(), isInitialized(), isStarted().
		 */
		bool pushFrame(const Frame& frame, const uint64_t presentationTime);

		/**
		 * Returns the next encoded sample if available.
		 * @return The resulting encoded sample, invalid if currently no encoded sample is available
		 * @see pushFrame().
		 */
		Sample popSample();

		/**
		 * Returns whether this encoder is initialized.
		 * @return True, if so
		 * @see initialize().
		 */
		inline bool isInitialized() const;

		/**
		 * Returns whether this encoder is currently running.
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
		 * @param videoEncoder The video encoder to be moved
		 * @return Reference to this object
		 */
		inline VideoEncoder& operator=(VideoEncoder&& videoEncoder) noexcept;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		VideoEncoder(const VideoEncoder&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		VideoEncoder& operator=(const VideoEncoder&) = delete;

		/**
		 * Creates a Frame object from a raw buffer provided by the Android MediaCodec.
		 * This function wraps the raw buffer memory into a Frame object with the appropriate pixel format and plane layout.
		 * The function supports YUV420 formats in both semi-planar (Y_UV12) and planar (Y_U_V12) layouts.
		 * @param width The width of the frame in pixels, must be even, with range [2, infinity)
		 * @param height The height of the frame in pixels, must be even, with range [2, infinity)
		 * @param buffer The raw buffer containing the frame data, must be valid
		 * @param size The size of the buffer in bytes, with range [1, infinity)
		 * @param androidFormat The Android MediaCodec color format constant
		 * @param androidColorRange The Android MediaFormat color range constant
		 * @return The resulting Frame object wrapping the buffer, invalid if the parameters are invalid or the format is not supported
		 */
		static Frame frameFromBuffer(const unsigned int width, const unsigned int height, uint8_t* const buffer, size_t size, const int32_t androidFormat, const int32_t androidColorRange);

	protected:

		/// The subscription for the native media library.
		NativeMediaLibrary::ScopedSubscription nativeMediaLibrarySubscription_;

		/// The Android media encoder used to encode the video.
		NativeMediaLibrary::ScopedAMediaCodec encoder_;

		/// True, if the encoder is currently started.
		bool isStarted_ = false;

		/// The encoder's lock.
		mutable Lock lock_;
};

inline VideoEncoder::Sample::Sample(std::vector<uint8_t>&& data, const int64_t presentationTime, const MediaCodecBufferFlags bufferFlags) :
	data_(std::move(data)),
	presentationTime_(presentationTime),
	bufferFlags_(bufferFlags)
{
	// nothing to do here
}

inline VideoEncoder::Sample::Sample(Sample&& sample) noexcept
{
	*this = std::move(sample);
}

inline bool VideoEncoder::Sample::isValid() const
{
	return !data_.empty();
}

inline const std::vector<uint8_t>& VideoEncoder::Sample::data() const
{
	return data_;
}

inline int64_t VideoEncoder::Sample::presentationTime() const
{
	return presentationTime_;
}

inline bool VideoEncoder::Sample::isKeyFrame() const
{
	return bufferFlags_ & BUFFER_FLAG_KEY_FRAME;
}

inline bool VideoEncoder::Sample::isConfiguration() const
{
	return bufferFlags_ & BUFFER_FLAG_CODEC_CONFIG;
}

inline bool VideoEncoder::Sample::isEndOfStream() const
{
	return bufferFlags_ & BUFFER_FLAG_END_OF_STREAM;
}

inline bool VideoEncoder::Sample::isPartialFrame() const
{
	return bufferFlags_ & BUFFER_FLAG_PARTIAL_FRAME;
}

inline VideoEncoder::Sample& VideoEncoder::Sample::operator=(Sample&& sample) noexcept
{
	if (this != &sample)
	{
		data_ = std::move(sample.data_);
		presentationTime_ = sample.presentationTime_;
		bufferFlags_ = sample.bufferFlags_;

		sample.presentationTime_ = -1;
		sample.bufferFlags_ = BUFFER_FLAG_NONE;
	}

	return *this;
}

inline VideoEncoder::Sample::operator bool() const
{
	return isValid();
}

inline VideoEncoder::VideoEncoder(VideoEncoder&& videoEncoder) noexcept
{
	*this = std::move(videoEncoder);
}

inline bool VideoEncoder::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return encoder_.isValid();
}

inline bool VideoEncoder::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!isStarted_ || isInitialized());

	return isStarted_;
}

inline VideoEncoder& VideoEncoder::operator=(VideoEncoder&& videoEncoder) noexcept
{
	if (this != &videoEncoder)
	{
		release();

		encoder_ = std::move(videoEncoder.encoder_);

		isStarted_ = videoEncoder.isStarted_;
		videoEncoder.isStarted_ = false;
	}

	return *this;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#endif // META_OCEAN_MEDIA_ANDROID_VIDEO_ENCODER_H
