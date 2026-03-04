/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_VIDEO_ENCODER_H
#define META_OCEAN_MEDIA_MF_VIDEO_ENCODER_H

#include "ocean/media/mediafoundation/MediaFoundation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

#include "ocean/math/Numeric.h"

#include <strmif.h>

#include <deque>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements a simple video encoder for Windows using Ocean::Frame objects as input.
 * The encoder uses Microsoft's Media Foundation Transform (MFT) framework for hardware-accelerated encoding.
 *
 * Usage:
 * @code
 * // a function which is e.g., running in a separate thread
 * void threadRun()
 * {
 *     VideoEncoder videoEncoder;
 *
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
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT VideoEncoder
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
		 * Definition of individual buffer flag constants.
		 * Modeled after Android's MediaCodec.BufferInfo for API compatibility.
		 */
		enum BufferFlags : uint32_t
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
				inline Sample(std::vector<uint8_t>&& data, const int64_t presentationTime, const BufferFlags bufferFlags);

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

				/// The buffer flags.
				BufferFlags bufferFlags_ = BUFFER_FLAG_NONE;
		};

		/**
		 * Definition of a vector holding sample objects.
		 */
		using Samples = std::vector<Sample>;

	protected:

		/// Definition of the maximal image width.
		static constexpr unsigned int maximalWidth_ = 1920u * 8u;

		/// Definition of the maximal image height.
		static constexpr unsigned int maximalHeight_ = 1080u * 8u;

		/// Definition of the maximal bit rate.
		static constexpr int maximalBitrate_ = bitrateMbps10_ * 10;

		/// Scoped object for ICodecAPI, used for configuring encoder parameters.
		using ScopedICodecAPI = ScopedMediaFoundationObject<ICodecAPI>;

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
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const unsigned int width, const unsigned int height, const std::string& mime = "video/avc", const double frameRate = 30.0, const unsigned int bitrate = bitrateMbps2_, const int iFrameInterval = 1);

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
		 * The encoder needs to be initialized and started.
		 * The presentation time is mainly intended to allow associating the provided frame with the resulting encoded sample when calling popSample().
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
		 * Tries to drain encoded output samples from the MFT into the internal queue.
		 * @return The number of samples drained
		 */
		size_t drainOutputSamples();

		/**
		 * Translates a MIME type to a Media Foundation video format GUID.
		 * @param mime The MIME type
		 * @return The corresponding video format GUID, GUID_NULL if not supported
		 */
		static GUID mimeToVideoFormat(const std::string& mime);

	protected:

		/// The MFT encoder used to encode the video.
		ScopedIMFTransform encoder_;

		/// The width of the video.
		unsigned int width_ = 0u;

		/// The height of the video.
		unsigned int height_ = 0u;

		/// True, if the encoder is currently started.
		bool isStarted_ = false;

		/// True if MFStartup has been called by this instance.
		bool mfStarted_ = false;

		/// True if the MFT provides its own output samples.
		bool mftProvidesOutputSamples_ = false;

		/// The size of the output buffer in bytes, used when the MFT does not provide its own output samples.
		DWORD outputBufferSize_ = 0u;

		/// True if codec config data has been emitted at least once.
		bool codecConfigEmitted_ = false;

		/// The queue of encoded samples.
		std::deque<Sample> encodedSamples_;

		/// The encoder's lock.
		mutable Lock lock_;
};

inline VideoEncoder::Sample::Sample(std::vector<uint8_t>&& data, const int64_t presentationTime, const BufferFlags bufferFlags) :
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

		sample.presentationTime_ = NumericT<int64_t>::minValue();
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

		encodedSamples_ = std::move(videoEncoder.encodedSamples_);

		width_ = videoEncoder.width_;
		videoEncoder.width_ = 0u;

		height_ = videoEncoder.height_;
		videoEncoder.height_ = 0u;

		isStarted_ = videoEncoder.isStarted_;
		videoEncoder.isStarted_ = false;

		mfStarted_ = videoEncoder.mfStarted_;
		videoEncoder.mfStarted_ = false;

		mftProvidesOutputSamples_ = videoEncoder.mftProvidesOutputSamples_;
		videoEncoder.mftProvidesOutputSamples_ = false;

		outputBufferSize_ = videoEncoder.outputBufferSize_;
		videoEncoder.outputBufferSize_ = 0u;

		codecConfigEmitted_ = videoEncoder.codecConfigEmitted_;
		videoEncoder.codecConfigEmitted_ = false;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_MEDIA_MF_VIDEO_ENCODER_H
