/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_USB_USB_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_USB_USB_LIVE_VIDEO_H

#include "ocean/media/usb/USB.h"
#include "ocean/media/usb/USBMedium.h"

#include "ocean/base/Thread.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/system/usb/video/VideoDevice.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/media/android/VideoDecoder.h"

	#include "ocean/system/usb/android/OceanUSBManager.h"
#endif

namespace Ocean
{

namespace Media
{

namespace USB
{

/**
 * This class implements an live video class for USB devices.
 * @ingroup mediausb
 */
class OCEAN_MEDIA_USB_EXPORT USBLiveVideo final :
	virtual public USBMedium,
	virtual public LiveVideo,
	protected Thread
{
	friend class USBLibrary;

		/**
		 * This class implements a simple wrapper around Media::Android::VideoDecoder.
		 */
		class OCEAN_MEDIA_USB_EXPORT VideoDecoder
		{
			protected:

				/**
				 * Definition of an unordered map mapping presentation times to timestamps.
				 */
				using TimestampMap = std::unordered_map<int64_t, Timestamp>;

			public:

				/**
				 * Default constructor creating an invalid decoder.
				 */
				VideoDecoder() = default;

				/**
				 * Default move constructor.
				 * @param videoDecoder The video decoder to be moved
				 */
				VideoDecoder(VideoDecoder&& videoDecoder) = default;

				/**
				 * Creates a new decoder object and initializes and starts the decoder.
				 * @param mime The MIME type (Multipurpose Internet Mail Extensions) of the video to be decoded, e.g., "video/avc", "video/hevc", ...
				 * @param width The width of the video to be decoded, in pixel, with range [1, infinity)
				 * @param height The height of the video to be decoded, in pixel, with range [1, infinity)
				 * @see isValid()
				 */
				VideoDecoder(const std::string& mime, const unsigned int width, const unsigned int height);

				/**
				 * Inserts a new sample to the video decoder.
				 * @param data The data of the sample, must be valid
				 * @param size The size of the sample, in bytes, with range [1, infinity)
				 * @param timestamp The capture timestamp of the sample, must be valid
				 * @return True, if the sample could be inserted
				 * @see popFrame().
				 */
				bool pushSample(const void* data, const size_t size, const Timestamp& timestamp);

				/**
				 * Pops the next encoded frame from the video decoder.
				 * @return The encoded frame; an invalid frame if the decoder is not valid or if currently no decoded frame is waiting in the queue
				 * @see pushSample().
				 */
				Frame popFrame();

				/**
				 * Returns whether the video decoder is valid and can be used to decode video samples.
				 * @return True, if so; False, if the video decoder is not available on this platform or if the video decoder could not be initialized
				 */
				bool isValid() const;

				/**
				 * Default move operator.
				 * @param videoDecoder The video decoder to be moved
				 * @return Reference to this object
				 */
				VideoDecoder& operator=(VideoDecoder&& videoDecoder) = default;

			protected:

				/**
				 * Disabled copy constructor.
				 */
				VideoDecoder(const VideoDecoder&) = delete;

			protected:

#ifdef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

				/// The actual video decoder.
				Media::Android::VideoDecoder videoDecoder_;

#endif // OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

				/// The counter for inserted media samples.
				unsigned int sampleCounter_ = 0u;

				/// The map mapping presentation times to timestamps.
				TimestampMap timestampMap_;
		};

	public:

		/**
		 * Returns the supported stream types.
		 * @see LiveVideo::streamTypes().
		 */
		StreamTypes supportedStreamTypes() const override;

		/**
		 * Returns the supported stream configurations for a given stream type.
		 * @see LiveVideo::streamConfigurations().
		 */
		StreamConfigurations supportedStreamConfigurations(const StreamType streamType) const override;

		/**
		 * Sets the preferred stream type.
		 * @see LiveVideo::setPreferredStreamType().
		 */
		bool setPreferredStreamType(const StreamType streamType) override;

		/**
		 * Sets the preferred stream configuration.
		 * @see LiveVideo::setPreferredStreamConfiguration().
		 */
		bool setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration) override;

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see FiniteMedium::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see FiniteMedium::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see FiniteMedium::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 * @param deviceName The name of the USB device
		 */
		explicit USBLiveVideo(const std::string& url, const std::string& deviceName);

		/**
		 * Destructs the live video object.
		 */
		~USBLiveVideo() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * Internal start function actually starting the USB video stream.
		 * @return True, if succeeded
		 */
		bool startInternal();

		/**
		 * Internal stop function actually stopping the USB video stream.
		 * @return True, if succeeded
		 */
		bool stopInternal();

		/**
		 * Opens the USB device.
		 * For Android platforms: Ensure that permission has been grated before opening the device.
		 * @param deviceName The name of the USB device to open
		 * @return True, if succeeded
		 */
		bool openDevice(const std::string& deviceName);

		/**
		 * Closes the USB device.
		 * @return True, if succeeded
		 */
		bool closeDevice();

		/**
		 * Event function for device permission events.
		 * @param deviceName The name of the USB device for which the event has been triggered, will be valid
		 * @param permissionGranted True, if the permission was granted; False, if the permission has been denied
		 */
		void onPermission(const std::string& deviceName, const bool permissionGranted);

		/**
		 * The thread function of this medium in which frame processing is handled.
		 */
		void threadRun() override;

		/**
		 * Processes a sample from an uncompressed video stream.
		 * @param width The width of the uncompressed frame in pixel, with range [1, infinity)
		 * @param height The height of the uncompressed frame in pixel, with range [1, infinity)
		 * @param pixelFormat The pixel format of the uncompressed frame, must be valid
		 * @param data The actual frame data, must be valid
		 * @param size The size of the frame data, in bytes, with range [1, infinity)
		 * @return The resulting frame, invalid in case of an error
		 */
		static Frame processUncompressedSample(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const void* data, const size_t size);

		/**
		 * Processes a sample from an MJPEG stream.
		 * @param width The width of the decoded frame in pixel, with range [1, infinity)
		 * @param height The height of the decoded frame in pixel, with range [1, infinity)
		 * @param data The data holding the MJPEG data, must be valid
		 * @param size The size of the MJPEG data, in bytes, with range [1, infinity)
		 * @return The resulting frame, invalid in case of an error
		 */
		static Frame processMjpegSample(const unsigned int width, const unsigned int height, const void* data, const size_t size);

		/**
		 * Translates a LiveVideo stream type to the corresponding USB device stream type.
		 * @param streamType The live video stream type to be translated
		 * @return The translated USB device stream type, DST_INVALID if the LiveVideo stream type is invalid or unknown
		 */
		static System::USB::Video::VideoDevice::DeviceStreamType translateStreamType(const StreamType streamType);

		/**
		 * Translates a LiveVideo codec type to the corresponding USB video encoding format.
		 * @param codecType The live video codec type to be translated
		 * @return The translated USB video encoding format, EF_INVALID if the LiveVideo codec type is invalid or unknown
		 */
		static System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat translateCodecType(const CodecType codecType);

		/**
		 * Translates a USB video encoding format to the corresponding LiveVideo codec type.
		 * @param encodingFormat The USB video encoding format to be translated
		 * @return The translated LiveVideo codec type, CT_INVALID if the USB video encoding format is invalid or unknown
		 */
		static CodecType translateEncodingFormat(const System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat);

		/**
		 * Translates a USB video encoding format to the corresponding MIME type.
		 * @param encodingFormat The USB video encoding format to be translated
		 * @return The translated MIME type, empty if the USB video encoding format is invalid or unknown
		 */
		static std::string mimeFromEncodingFormat(const System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat);

	protected:

		/// The name of the USB device.
		std::string deviceName_;

		/// The preferred stream type.
		StreamType preferredStreamType_ = ST_INVALID;

		/// The preferred codec type.
		CodecType preferredCodecType_ = CT_INVALID;

		/// The actual USB video device which will be used.
		System::USB::Video::SharedVideoDevice videoDevice_;

		/// 1 if the medium has permission to access the USB device, -1 if permission was denied, 0 if the permission is not yet decided.
		std::atomic_int hasPermission_ = 0;

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Pause timestamp.
		Timestamp pauseTimestamp_;

		/// Stop timestamp.
		Timestamp stopTimestamp_;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/// The permission request object for the USB device.
		System::USB::Android::OceanUSBManager::ScopedPermissionSubscription permissionSubscription_;

		/// True, if the device needs to be started (out of the thread function).
		std::atomic_bool delayedStart_ = false;

		/// True, if the device needs to be stopped (out of the thread function).
		std::atomic_bool delayedStop_ = false;

#endif // OCEAN_PLATFORM_BUILD_ANDROID
};

}

}

}

#endif // META_OCEAN_MEDIA_USB_USB_LIVE_VIDEO_H
