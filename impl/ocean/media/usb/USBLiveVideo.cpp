/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/usb/USBLiveVideo.h"

#include "ocean/base/Frame.h"

#include "ocean/io/image/Image.h"

#include "ocean/system/usb/Manager.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/NativeInterfaceManager.h"

	#include "ocean/media/android/Image.h"
#endif

namespace Ocean
{

namespace Media
{

namespace USB
{

USBLiveVideo::VideoDecoder::VideoDecoder(const std::string& mime, const unsigned int width, const unsigned int height)
{
#ifdef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

	if (videoDecoder_.initialize(mime, width, height) && videoDecoder_.start())
	{
		ocean_assert(isValid());
	}
	else
	{
		ocean_assert(!isValid());
	}

#else

	Log::warning() << "The platform does not support a video decoder";

	OCEAN_SUPPRESS_UNUSED_WARNING(mime);
	OCEAN_SUPPRESS_UNUSED_WARNING(width);
	OCEAN_SUPPRESS_UNUSED_WARNING(height);

#endif // OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

	timestampMap_.reserve(64);
}

bool USBLiveVideo::VideoDecoder::pushSample(const void* data, const size_t size, const Timestamp& timestamp)
{
#ifdef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

	constexpr double framesPerSecond = 30.0; // just a rough estimate, does not need to fit the actual frame rate

	const int64_t presentationTime = int64_t(double(sampleCounter_) * Timestamp::seconds2microseconds(1.0 / framesPerSecond) + 0.5);
	++sampleCounter_;

	if (videoDecoder_.pushSample(data, size, uint64_t(presentationTime)))
	{
		if (timestampMap_.size() >= 4096)
		{
			// let's ensure that the map does not blow up
			timestampMap_.clear();

			Log::warning() << "Video decoder timestamp it too large, therefore clearing it";
		}

		ocean_assert(timestampMap_.find(presentationTime) == timestampMap_.cend());
		timestampMap_.emplace(presentationTime, timestamp);

		return true;
	}

	return false;

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(data);
	OCEAN_SUPPRESS_UNUSED_WARNING(size);
	OCEAN_SUPPRESS_UNUSED_WARNING(timestamp);

	return false;

#endif // OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE
}

Frame USBLiveVideo::VideoDecoder::popFrame()
{
#ifdef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE

	int64_t presentationTime = 0ll;
	Frame frame = videoDecoder_.popFrame(&presentationTime);

	if (frame.isValid())
	{
		// now we have to set the correct frame timestamp - this will be the timestamp associated with the media sample (as provided in pushSample())

		Timestamp timestamp(true); // backup timestamp

		TimestampMap::const_iterator iTimestamp = timestampMap_.find(presentationTime);
		ocean_assert(iTimestamp != timestampMap_.cend());

		if (iTimestamp != timestampMap_.cend())
		{
			timestamp = iTimestamp->second;
			timestampMap_.erase(iTimestamp);
		}

		frame.setTimestamp(timestamp);
	}

	return frame;

#else
	return Frame();
#endif // OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE
}

bool USBLiveVideo::VideoDecoder::isValid() const
{
#ifdef OCEAN_MEDIA_ANDROID_VIDEODECODER_AVAILABLE
	return videoDecoder_.isStarted();
#else
	return false;
#endif
}

USBLiveVideo::USBLiveVideo(const std::string& url, const std::string& deviceName) :
	Medium(url),
	USBMedium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url)
{
	libraryName_ = nameUSBLibrary();

	isValid_ = false;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	// we cannot directly open the device, instead we have to request permission first
	// once, the permission is granted, we can indirectly open the device through Java

	JNIEnv* jniEnv = Platform::Android::NativeInterfaceManager::get().environment();

	if (jniEnv != nullptr)
	{
		permissionSubscription_ = System::USB::Android::OceanUSBManager::get().requestPermission(jniEnv, deviceName, std::bind(&USBLiveVideo::onPermission, this, std::placeholders::_1, std::placeholders::_2));

		if (permissionSubscription_)
		{
			deviceName_ = deviceName;

			// the device exists, however we still need to wait for permission (e.g., on Android)
		}
	}
#else

	if (!openDevice(deviceName))
	{
		ocean_assert(!isValid_);
		return;
	}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	frameCollection_ = FrameCollection(10);

	isValid_ = true;

	startThread();
}

USBLiveVideo::~USBLiveVideo()
{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	permissionSubscription_.release();
#endif
	closeDevice();

	ocean_assert(videoDevice_ == nullptr);
}

USBLiveVideo::StreamTypes USBLiveVideo::supportedStreamTypes() const
{
	StreamTypes streamTypes;
	streamTypes.reserve(4);

	const ScopedLock scopedLock(lock_);

	if (videoDevice_)
	{
		if (!videoDevice_->videoStreamingInterface().uncompressedVideoStreams().empty())
		{
			streamTypes.emplace_back(ST_FRAME);
		}

		if (!videoDevice_->videoStreamingInterface().mjpegVideoStreams().empty())
		{
			streamTypes.emplace_back(ST_MJPEG);
		}

		if (!videoDevice_->videoStreamingInterface().frameBasedVideoStreams().empty())
		{
			streamTypes.emplace_back(ST_CODEC);
		}
	}

	return streamTypes;
}

USBLiveVideo::StreamConfigurations USBLiveVideo::supportedStreamConfigurations(const StreamType streamType) const
{
	StreamConfigurations streamConfigurations;
	streamConfigurations.reserve(64);

	const ScopedLock scopedLock(lock_);

	if (videoDevice_)
	{
		if (streamType == ST_INVALID || streamType == ST_FRAME)
		{
			if (!videoDevice_->videoStreamingInterface().uncompressedVideoStreams().empty())
			{
				const System::USB::Video::VideoDevice::VideoStreamingInterface::UncompressedVideoStream& uncompressedVideoStream = videoDevice_->videoStreamingInterface().uncompressedVideoStreams().front();
				const System::USB::Video::VSUncompressedVideoFormatDescriptor& formatDescriptor = uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_;

				const FrameType::PixelFormat pixelFormat = System::USB::Video::VSUncompressedVideoFormatDescriptor::translatePixelFormat(formatDescriptor.guidFormat_);

				if (pixelFormat != FrameType::FORMAT_UNDEFINED)
				{
					for (const System::USB::Video::VSUncompressedVideoFrameDescriptor& frameDescriptor : uncompressedVideoStream.vsUncompressedVideoFrameDescriptors_)
					{
						const unsigned int width = frameDescriptor.wWidth_;
						const unsigned int height = frameDescriptor.wHeight_;

						std::vector<double> frameRates;
						const Indices32 frameIntervals = frameDescriptor.frameIntervals(&frameRates);

						streamConfigurations.emplace_back(ST_FRAME, width, height, std::move(frameRates), pixelFormat, CT_INVALID);
					}
				}
				else
				{
					Log::warning() << "Unknown pixel format in frame descriptor: " << System::USB::Descriptor::guid2string(formatDescriptor.guidFormat_);
				}
			}
		}

		if (streamType == ST_INVALID || streamType == ST_MJPEG)
		{
			if (!videoDevice_->videoStreamingInterface().mjpegVideoStreams().empty())
			{
				const System::USB::Video::VideoDevice::VideoStreamingInterface::MJPEGVideoStream& mjpegVideoStream = videoDevice_->videoStreamingInterface().mjpegVideoStreams().front();

				for (const System::USB::Video::VSMJPEGVideoFrameDescriptor& frameDescriptor : mjpegVideoStream.vsMJPEGVideoFrameDescriptors_)
				{
					const unsigned int width = frameDescriptor.wWidth_;
					const unsigned int height = frameDescriptor.wHeight_;

					std::vector<double> frameRates;
					const Indices32 frameIntervals = frameDescriptor.frameIntervals(&frameRates);

					streamConfigurations.emplace_back(ST_MJPEG, width, height, std::move(frameRates), FrameType::FORMAT_UNDEFINED, CT_INVALID);
				}
			}
		}

		if (streamType == ST_INVALID || streamType == ST_CODEC)
		{
			if (!videoDevice_->videoStreamingInterface().frameBasedVideoStreams().empty())
			{
				const System::USB::Video::VideoDevice::VideoStreamingInterface::FrameBasedVideoStream& frameBasedVideoStream = videoDevice_->videoStreamingInterface().frameBasedVideoStreams().front();
				const System::USB::Video::VSFrameBasedVideoFormatDescriptor& formatDescriptor = frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_;

				const CodecType codecType = translateEncodingFormat(formatDescriptor.encodingFormat());

				if (codecType != CT_INVALID)
				{
					for (const System::USB::Video::VSFrameBasedFrameDescriptor& frameDescriptor : frameBasedVideoStream.vsFrameBasedFrameDescriptors_)
					{
						const unsigned int width = frameDescriptor.wWidth_;
						const unsigned int height = frameDescriptor.wHeight_;

						std::vector<double> frameRates;
						const Indices32 frameIntervals = frameDescriptor.frameIntervals(&frameRates);

						streamConfigurations.emplace_back(ST_CODEC, width, height, std::move(frameRates), FrameType::FORMAT_UNDEFINED, codecType);
					}
				}
				else
				{
					Log::warning() << "Unknown codec type in frame descriptor: " << System::USB::Descriptor::guid2string(formatDescriptor.guidFormat_);
				}
			}
		}
	}

	return streamConfigurations;
}

bool USBLiveVideo::setPreferredStreamType(const StreamType streamType)
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted())
	{
		preferredStreamType_ = streamType;

		return true;
	}

	return false;
}

bool USBLiveVideo::setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration)
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted())
	{
		preferredStreamType_ = streamConfiguration.streamType_;
		preferredCodecType_ = streamConfiguration.codecType_;

		setPreferredFrameDimension(streamConfiguration.width_, streamConfiguration.height_);

		if (!streamConfiguration.frameRates_.empty())
		{
			setPreferredFrameFrequency(FrameFrequency(streamConfiguration.frameRates_.front()));
		}
		else
		{
			setPreferredFrameFrequency(FrameFrequency(0));
		}

		return true;
	}

	return false;
}

bool USBLiveVideo::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp USBLiveVideo::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp USBLiveVideo::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp USBLiveVideo::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

bool USBLiveVideo::start()
{
	const ScopedLock scopedLock(lock_);

	frameCollection_.clear();

	if (!videoDevice_)
	{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		delayedStart_ = true;

		return true;
#endif

		return false;
	}

	return startInternal();
}

bool USBLiveVideo::pause()
{
	// not yet implemented

	return false;
}

bool USBLiveVideo::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!videoDevice_)
	{
		return true;
	}

	if (!videoDevice_->isStarted())
	{
		return true;
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	delayedStop_ = true; // on Android platforms, we perform a delayed stop to avoid blocking the calling thread for too long
#else
	stopInternal();
#endif

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

bool USBLiveVideo::startInternal()
{
	const ScopedLock scopedLock(lock_);

	if (videoDevice_->isStarted())
	{
		return true;
	}

	const unsigned int preferredWidth = preferredFrameWidth();
	const unsigned int preferredHeight = preferredFrameHeight();
	const double preferredFrameRate = double(preferredFrameFrequency());

	System::USB::Video::VideoDevice::DeviceStreamType preferredDeviceStreamType = translateStreamType(preferredStreamType_);
	const FrameType::PixelFormat preferredPixelFormat = preferredFrameType_.pixelFormat();
	const System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat preferredEncodingFormat = translateCodecType(preferredCodecType_);

	if (!videoDevice_->start(preferredWidth, preferredHeight, preferredFrameRate, preferredDeviceStreamType, preferredPixelFormat, preferredEncodingFormat))
	{
		Log::error() << "Failed to start the video device";
		return false;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	return true;
}

bool USBLiveVideo::stopInternal()
{
	return videoDevice_->stop();
}

bool USBLiveVideo::openDevice(const std::string& deviceName)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!videoDevice_);

	System::USB::SharedContext context = System::USB::Manager::get().context();

	if (!context)
	{
		Log::error() << "USBLiveVideo: Failed to get USB context.";
		return false;
	}

	System::USB::SharedDevice device;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	ocean_assert(hasPermission_ >= 1);

	JNIEnv* jniEnv = Platform::Android::NativeInterfaceManager::get().environment();

	int64_t fileDescriptor = -1;
	if (System::USB::Android::OceanUSBManager::get().openDevice(jniEnv, deviceName, fileDescriptor))
	{
		device = std::make_shared<System::USB::Device>(context, fileDescriptor);

		if (!device->isValid())
		{
			Log::error() << "USBLiveVideo: Failed to create device based on file descriptor for " << deviceName;
			return false;
		}
	}

#else

	device = System::USB::Manager::get().findDevice(deviceName);

	if (!device)
	{
		Log::error() << "USBLiveVideo: Failed to create device for " << deviceName;
		return false;
	}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	ocean_assert(device && device->isValid());

	System::USB::Video::SharedVideoDevice videoDevice = std::make_shared<System::USB::Video::VideoDevice>(std::move(*device));

	if (!videoDevice->isValid())
	{
		Log::error() << "USBLiveVideo: Failed to create video device for " << deviceName;
		return false;
	}

	videoDevice_ = std::move(videoDevice);
	ocean_assert(videoDevice_->isValid());

	return true;
}

bool USBLiveVideo::closeDevice()
{
	const ScopedLock scopedLock(lock_);

	if (videoDevice_)
	{
		videoDevice_->stop();

		stopThreadExplicitly();

		videoDevice_ = nullptr;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		JNIEnv* jniEnv = Platform::Android::NativeInterfaceManager::get().environment();

		ocean_assert(!deviceName_.empty());
		const bool closeResult = System::USB::Android::OceanUSBManager::get().closeDevice(jniEnv, deviceName_);
		ocean_assert_and_suppress_unused(closeResult, closeResult);
#endif
	}

	return true;
}

void USBLiveVideo::onPermission(const std::string& deviceName, const bool permissionGranted)
{
	ocean_assert(deviceName == deviceName_);

	if (permissionGranted)
	{
		Log::debug() << "USBLiveVideo: Permission granted for device <" << url_ << "(" << deviceName << ").";
	}
	else
	{
		Log::warning() << "USBLiveVideo: Permission denied for device <" << url_ << "(" << deviceName << ").";
		return;
	}

	hasPermission_ = permissionGranted ? 1 : -1;
}

void USBLiveVideo::threadRun()
{

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	// on Android platforms, we first need to open the device (once we have a granted permission)

	const Timestamp waitForPermissionStartTimestamp(true);
	double nextPermissionWarningDelay = 1.0;

	while (!shouldThreadStop())
	{
		const int hasPermssion = hasPermission_;

		if (hasPermssion >= 1)
		{
			if (openDevice(deviceName_))
			{
				break;
			}
			else
			{
				// there is nothing we can do here anymore
				return;
			}
		}
		else if (hasPermssion <= -1)
		{
			// permission was denied, there is nothing we can do here anymore
			return;
		}

		Thread::sleep(1u);

		if (waitForPermissionStartTimestamp.hasTimePassed(4.0 + nextPermissionWarningDelay))
		{
			// the user has not yet decided whether to grant the permission or not, or the permission request never reached the user

			Log::warning() << "USBLiveVideo: Waiting for permission for device '" << url_ << "' (" << deviceName_ << ")";

			nextPermissionWarningDelay *= 2.0;
		}
	}
#endif

	using VideoDevice = System::USB::Video::VideoDevice;
	using VSFrameBasedVideoFormatDescriptor = System::USB::Video::VSFrameBasedVideoFormatDescriptor;

	VideoDevice::DeviceStreamType deviceStreamType = VideoDevice::DST_INVALID;

	uint8_t previousDescriptorFormatIndex = 0u;
	uint8_t previousDescriptorFrameIndex = 0u;

	unsigned int width = 0u;
	unsigned int height = 0u;

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;
	VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID;

	bool shouldSleep = true;

	VideoDecoder videoDecoder;

	while (!shouldThreadStop())
	{
		shouldSleep = true;

		if (videoDevice_ && videoDevice_->isStarted())
		{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
			if (delayedStop_)
			{
				stopInternal();
				delayedStop_ = false;

				continue;
			}
#endif // OCEAN_PLATFORM_BUILD_ANDROID

			VideoDevice::SharedSample sample = videoDevice_->nextSample();

			if (sample)
			{
				ocean_assert(sample->data() != nullptr && sample->size() != 0);
				ocean_assert(sample->descriptorFormatIndex() != 0u && sample->descriptorFrameIndex() != 0u);

				if (sample->descriptorFormatIndex() != previousDescriptorFormatIndex || sample->descriptorFrameIndex() != previousDescriptorFrameIndex)
				{
					deviceStreamType = videoDevice_->extractStreamProperties(sample->descriptorFormatIndex(), sample->descriptorFrameIndex(), width, height, pixelFormat, encodingFormat);

					if (deviceStreamType == VideoDevice::DST_FRAME_BASED)
					{
						const std::string mime = mimeFromEncodingFormat(encodingFormat);

						videoDecoder = VideoDecoder(mime, width, height);

						if (!videoDecoder.isValid())
						{
							Log::error() << "Failed to initialize and started the video decoder for '" << mime << "' with " << width << "x" << height;

							// either the platform does not support video decoding or the format is not supported
							// let's avoid to use the encoder

							deviceStreamType = VideoDevice::DST_INVALID;
						}
					}

					previousDescriptorFormatIndex = sample->descriptorFormatIndex();
					previousDescriptorFrameIndex = sample->descriptorFrameIndex();
				}

				Timestamp timestamp = sample->determineCaptureTimestamp();

				if (!timestamp.isValid())
				{
					Log::warning() << "Invalid sample timestamp, using default timestamp instead";
					timestamp.toNow();
				}

				Frame frame;

				switch (deviceStreamType)
				{
					case VideoDevice::DST_INVALID:
						// e.g., the encoder is invalid
						break;

					case VideoDevice::DST_UNCOMPRESSED:
					{
						frame = processUncompressedSample(width, height, pixelFormat, sample->data(), sample->size());

						if (frame.isValid())
						{
							frame.setTimestamp(timestamp);

							FrameMedium::deliverNewFrame(std::move(frame)); // we don't need to worry whether the frame is owning the memory or not, deliverNewFrame() will make a copy of the memory if necessary
						}

						break;
					}

					case VideoDevice::DST_MJPEG:
					{
						frame = processMjpegSample(width, height, sample->data(), sample->size());

						if (frame.isValid())
						{
							frame.setTimestamp(timestamp);

							FrameMedium::deliverNewFrame(std::move(frame));
						}

						break;
					}

					case VideoDevice::DST_FRAME_BASED:
					{
						ocean_assert(videoDecoder.isValid());

						videoDecoder.pushSample(sample->data(), sample->size(), timestamp);

						break;
					}
				}

				videoDevice_->giveSampleBack(std::move(sample));

				shouldSleep = false;
			}

			if (deviceStreamType == VideoDevice::DST_FRAME_BASED)
			{
				ocean_assert(videoDecoder.isValid());

				Frame frame = videoDecoder.popFrame();

				if (frame.isValid())
				{
					FrameMedium::deliverNewFrame(std::move(frame));

					shouldSleep = false;
				}
			}
		}
		else
		{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
			if (delayedStart_ && videoDevice_)
			{
				delayedStart_ = false;

				if (!startInternal())
				{
					Log::error() << "Failed to invoke delayed start of USBLiveVideo device " << deviceName_;
				}
			}
#endif // OCEAN_PLATFORM_BUILD_ANDROID
		}

		if (shouldSleep)
		{
			Thread::sleep(1u);
		}
	}
}

Frame USBLiveVideo::processUncompressedSample(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const void* data, const size_t size)
{
	if (width == 0u || height == 0u || pixelFormat == FrameType::FORMAT_UNDEFINED || data == nullptr || size == 0)
	{
		return Frame();
	}

	constexpr unsigned int maxDimension = 16384u;

	if (width > maxDimension || height > maxDimension)
	{
		return Frame();
	}

	const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	if (!frameType.isValid())
	{
		Log::error() << "Invalid frame type in uncompressed sample: " << width << "x" << height << ", " << FrameType::translatePixelFormat(pixelFormat);
		return Frame();
	}

	if (size != frameType.frameTypeSize())
	{
		Log::error() << "Wrong sample size: " << size / 1024 << "KB vs. " << frameType.frameTypeSize() / 1024u << "KB expected";
		return Frame();
	}

	constexpr unsigned int paddingElements = 0u;

	return Frame(frameType, data, Frame::CM_USE_KEEP_LAYOUT, paddingElements);
}

Frame USBLiveVideo::processMjpegSample(const unsigned int width, const unsigned int height, const void* data, const size_t size)
{
	// each individual jpeg buffer starts with 0xFFD8 and should end with 0xFFD9

	if (size <= 4)
	{
		return Frame();
	}

	const uint8_t* const byteData = (const uint8_t*)(data);

	// checking magic number at beginning of the buffer, we don't verify whether the end contains 0xFFD9 as some cameras may send a buffer slightly larger than expected
	if (byteData[0] == 0xFFu && byteData[1] == 0xD8u)
	{
#ifdef OCEAN_MEDIA_ANDROID_IMAGE_AVAILABLE
		// we have access to Android built-in JPEG decoder
		Frame frame = Media::Android::Image::decodeImage(data, size, "jpg");
#else
		// we need to use the backup (software) decoder
		Frame frame = IO::Image::decodeImage(data, size, "jpg");
#endif

		if (frame.isValid())
		{
			if (frame.width() != width || frame.height() != height)
			{
				Log::warning() << "Unexpected image resolution " << frame.width() << "x" << frame.height() << " vs. " << width << "x" << height << " expected";
			}
		}

		return frame;
	}

	return Frame();
}

System::USB::Video::VideoDevice::DeviceStreamType USBLiveVideo::translateStreamType(const StreamType streamType)
{
	switch (streamType)
	{
		case ST_INVALID:
			return System::USB::Video::VideoDevice::DST_INVALID;

		case ST_FRAME:
			return System::USB::Video::VideoDevice::DST_UNCOMPRESSED;

		case ST_MJPEG:
			return System::USB::Video::VideoDevice::DST_MJPEG;

		case ST_CODEC:
			return System::USB::Video::VideoDevice::DST_FRAME_BASED;
	}

	ocean_assert(false && "Invalid stream type!");
	return System::USB::Video::VideoDevice::DST_INVALID;
}

System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat USBLiveVideo::translateCodecType(const CodecType codecType)
{
	switch (codecType)
	{
		case CT_INVALID:
			return System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_INVALID;

		case CT_H264:
			return System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H264;

		case CT_H265:
			return System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H265;
	}

	ocean_assert(false && "Invalid encoding format!");
	return System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_INVALID;
}

USBLiveVideo::CodecType USBLiveVideo::translateEncodingFormat(const System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat)
{
	switch (encodingFormat)
	{
		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_INVALID:
			return CT_INVALID;

		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H264:
			return CT_H264;

		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H265:
			return CT_H265;
	}

	ocean_assert(false && "Invalid encoding format!");
	return CT_INVALID;
}

std::string USBLiveVideo::mimeFromEncodingFormat(const System::USB::Video::VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat)
{
	switch (encodingFormat)
	{
		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_INVALID:
			return std::string();

		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H264:
			return std::string("video/avc");

		case System::USB::Video::VSFrameBasedVideoFormatDescriptor::EF_H265:
			return std::string("video/hevc");
	}

	ocean_assert(false && "Invalid encoding format!");
	return std::string();
}

}

}

}
