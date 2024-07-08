/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/video/VideoDevice.h"

#include "ocean/math/Numeric.h"

#include "ocean/system/usb/Manager.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

std::string VideoDevice::VideoControl::toString() const
{
	std::string result;

	result += "bmHint: " + String::toAString(int(bmHint_));

	if (bmHint_ != 0u)
	{
		result += " (";

		if (bmHint_ & (1u << 0u))
		{
			result += "dwFrameInterval, ";
		}

		if (bmHint_ & (1u << 1u))
		{
			result += "wKeyFrameRate, ";
		}

		if (bmHint_ & (1u << 2u))
		{
			result += "wPFrameRate, ";
		}

		if (bmHint_ & (1u << 3u))
		{
			result += "wCompQuality, ";
		}

		if (bmHint_ & (1u << 4u))
		{
			result += "wCompWindowSize, ";
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nbFormatIndex: " + String::toAString(int(bFormatIndex_));
	result += "\nbFrameIndex: " + String::toAString(int(bFrameIndex_));
	result += "\ndwFrameInterval: " + String::toAString(dwFrameInterval_);
	result += "\nwKeyFrameRate: " + String::toAString(wKeyFrameRate_);
	result += "\nwPFrameRate: " + String::toAString(wPFrameRate_);
	result += "\nwCompQuality: " + String::toAString(wCompQuality_);
	result += "\nwCompWindowSize: " + String::toAString(wCompWindowSize_);
	result += "\nwDelay: " + String::toAString(wDelay_);
	result += "\ndwMaxVideoFrameSize: " + String::toAString(dwMaxVideoFrameSize_);
	result += "\ndwMaxPayloadTransferSize: " + String::toAString(dwMaxPayloadTransferSize_);
	result += "\ndwClockFrequency: " + String::toAString(dwClockFrequency_);

	result += "\nbmFramingInfo: " + String::toAString(int(bmFramingInfo_));

	if (bmFramingInfo_ != 0u)
	{
		result += " (";

		if (bmFramingInfo_ & (1u << 0u))
		{
			result += "FID, ";
		}

		if (bmFramingInfo_ & (1u << 1u))
		{
			result += "EOF, ";
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nbPreferedVersion: " + String::toAString(int(bPreferedVersion_));
	result += "\nbMinVersion: " + String::toAString(int(bMinVersion_));
	result += "\nbMaxVersion: " + String::toAString(int(bMaxVersion_));

	return result;
}

bool VideoDevice::VideoControl::executeVideoControlCommit(libusb_device_handle* usbDeviceHandle, const uint8_t interfaceIndex, const VideoControl& videoControl, const size_t videoControlSize, const uint8_t bRequest)
{
	VideoControl copyVideoControl(videoControl);

	// bmRequestType:          wIndex
	// 0b00100001              Entity ID and Interface.
	// 0b00100010              Endpoint

	constexpr uint8_t bmRequestType = 0b00100001u;

	constexpr uint16_t wValue = uint16_t(VS_COMMIT_CONTROL) << 8u;
	const uint16_t wIndex = uint16_t(interfaceIndex);

	return executeVideoControl(usbDeviceHandle, bmRequestType, bRequest, wValue, wIndex, (uint8_t*)(&copyVideoControl), videoControlSize);
}

bool VideoDevice::VideoControl::executeVideoControlProbe(libusb_device_handle* usbDeviceHandle, const uint8_t interfaceIndex, VideoControl& videoControl, const size_t videoControlSizex, const uint8_t bRequest)
{
	// bmRequestType:          wIndex
	// 0b10100001              Entity ID and Interface.
	// 0b10100010              Endpoint

	constexpr uint8_t bmRequestType = 0b10100001u;

	constexpr uint16_t wValue = uint16_t(VS_PROBE_CONTROL) << 8u;
	const uint16_t wIndex = uint16_t(interfaceIndex);

	return executeVideoControl(usbDeviceHandle, bmRequestType, bRequest, wValue, wIndex, (uint8_t*)(&videoControl), videoControlSizex);
}

bool VideoDevice::VideoControl::executeVideoControl(libusb_device_handle* usbDeviceHandle, const uint8_t bmRequestType, const uint8_t bRequest, const uint16_t wValue, const uint16_t wIndex, uint8_t* buffer, const size_t size)
{
	ocean_assert(usbDeviceHandle != nullptr);
	ocean_assert(size == 26 || size == sizeof(VideoControl));

	const int result = libusb_control_transfer(usbDeviceHandle, bmRequestType, bRequest, wValue, wIndex, buffer, uint16_t(size), 0u);

	if (result != int(size))
	{
		Log::info() << "Control transfer failed: " << result << ", " << libusb_error_name(result);
		return false;
	}

	return true;
}

VideoDevice::VideoControlInterface::VideoControlInterface(const libusb_interface_descriptor& interfaceDescriptor, libusb_device_handle* usbDeviceHandle)
{
	ocean_assert(interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_VIDEO);
	ocean_assert(interfaceDescriptor.bInterfaceSubClass == SC_VIDEOCONTROL);

	const unsigned char* extraBuffer = interfaceDescriptor.extra;
	const int extraBufferLength = interfaceDescriptor.extra_length;

	ocean_assert(!isValid());

	if (extraBuffer == nullptr || extraBufferLength <= 0)
	{
		return;
	}

	const uint8_t* remainingBuffer = (const uint8_t*)(extraBuffer);
	size_t remainingSize = size_t(extraBufferLength);

	while (remainingSize >= 3)
	{
		/**
		 * byte 0: bLength
		 * byte 1: bDescriptorType
		 * byte 2: bDescriptorSubtype
		 */

		const uint8_t bLength = remainingBuffer[0];
		const uint8_t bDescriptorType = remainingBuffer[1];
		const uint8_t bDescriptorSubtype = remainingBuffer[2];

		const size_t descriptorSize = size_t(bLength);

		if (bLength > remainingSize)
		{
			return;
		}

		if (bDescriptorType != CS_INTERFACE)
		{
			return;
		}

		const VCDescriptor::VideoControlInterfaceTypes type = VCDescriptor::VideoControlInterfaceTypes(bDescriptorSubtype);

		switch (type)
		{
			case VCDescriptor::VC_HEADER:
			{
				if (vcHeaderDescriptor_.isValid())
				{
					Log::error() << "VC_HEADER already parsed";
					return;
				}

				vcHeaderDescriptor_ = VCHeaderDescriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VC_HEADER, with descriptor size " << descriptorSize;
				Log::debug() << vcHeaderDescriptor_.toString();
#endif

				if (!vcHeaderDescriptor_.isValid())
				{
					Log::error() << "VC_HEADER is invalid";
					return;
				}

				break;
			}

			case VCDescriptor::VC_INPUT_TERMINAL:
			{
				const VCInputTerminalDescriptor& inputTerminalDescriptor = vcInputTerminalDescriptors_.emplace_back(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VC_INPUT_TERMINAL, with descriptor size " << descriptorSize;
				Log::debug() << inputTerminalDescriptor.toString(usbDeviceHandle);
#else
				OCEAN_SUPPRESS_UNUSED_WARNING(usbDeviceHandle);
#endif

				if (!inputTerminalDescriptor.isValid())
				{
					Log::error() << "VC_INPUT_TERMINAL is invalid";

					vcInputTerminalDescriptors_.pop_back();
					return;
				}

				break;
			}

			case VCDescriptor::VC_OUTPUT_TERMINAL:
#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << "Skipping VC_OUTPUT_TERMINAL";
#endif
				break;

			case VCDescriptor::VC_SELECTOR_UNIT:
			{
				const VCSelectorUnitDescriptor& selectorUnitDescriptor = vcSelectorUnitDescriptors_.emplace_back(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VC_SELECTOR_UNIT, with descriptor size " << descriptorSize;
				Log::debug() << selectorUnitDescriptor.toString(usbDeviceHandle);
#endif

				if (!selectorUnitDescriptor.isValid())
				{
					Log::error() << "VC_SELECTOR_UNIT is invalid";

					vcSelectorUnitDescriptors_.pop_back();
					return;
				}

				break;
			}

			case VCDescriptor::VC_PROCESSING_UNIT:
			{
				const VCProcessingUnitDescriptor& processingUnitDescriptor = vcProcessingUnitDescriptors_.emplace_back(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VC_PROCESSING_UNIT, with descriptor size " << descriptorSize;
				Log::debug() << processingUnitDescriptor.toString(usbDeviceHandle);
#endif

				if (!processingUnitDescriptor.isValid())
				{
					Log::error() << "VC_PROCESSING_UNIT is invalid";

					vcProcessingUnitDescriptors_.pop_back();
					return;
				}

				break;
			}

			case VCDescriptor::VC_EXTENSION_UNIT:
			{
				const VCExtensionUnitDescriptor& extensionUnitDescriptor = vcExtensionUnitDescriptors_.emplace_back(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VC_EXTENSION_UNIT, with descriptor size " << descriptorSize;
				Log::debug() << extensionUnitDescriptor.toString(usbDeviceHandle);
#endif

				if (!extensionUnitDescriptor.isValid())
				{
					Log::error() << "VC_EXTENSION_UNIT is invalid";

					vcExtensionUnitDescriptors_.pop_back();
					return;
				}

				break;
			}

			default:
			{
				Log::info() << "Unknown descriptor subtype: " << bDescriptorSubtype;
				break;
			}
		}

		remainingBuffer += descriptorSize;
		remainingSize -= size_t(descriptorSize);
	}

	ocean_assert(remainingSize == 0);

	bInterfaceIndex_ = interfaceDescriptor.bInterfaceNumber;

	if (interfaceDescriptor.bNumEndpoints != 0u)
	{
		bEndpointAddress_ = interfaceDescriptor.endpoint[0].bEndpointAddress;
	}

	isValid_ = true;
}

bool VideoDevice::VideoStreamingInterface::parseAdditionalInterface(const libusb_interface_descriptor& interfaceDescriptor, libusb_device_handle* /*usbDeviceHandle*/)
{
	ocean_assert(interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_VIDEO);
	ocean_assert(interfaceDescriptor.bInterfaceSubClass == SC_VIDEOSTREAMING);

	const unsigned char* extraBuffer = interfaceDescriptor.extra;
	const int extraBufferLength = interfaceDescriptor.extra_length;

	if (extraBuffer == nullptr && extraBufferLength == 0)
	{
		// no extra data to process
		return true;
	}

	const uint8_t* remainingBuffer = (const uint8_t*)(extraBuffer);
	size_t remainingSize = size_t(extraBufferLength);

	VSDescriptor::VideoStreamingInterfaceTypes currentFrameDescriptorType = VSDescriptor::VS_UNDEFINED;

	while (remainingSize >= 3)
	{
		/**
		 * byte 0: bLength
		 * byte 1: bDescriptorType
		 * byte 2: bDescriptorSubtype
		 */

		const uint8_t bLength = remainingBuffer[0];
		const uint8_t bDescriptorType = remainingBuffer[1];
		const uint8_t bDescriptorSubtype = remainingBuffer[2];

		const size_t descriptorSize = size_t(bLength);

		if (bLength > remainingSize)
		{
			Log::info() << "B";
			return false;
		}

		if (bDescriptorType != CS_INTERFACE)
		{
			Log::info() << "C";
			return false;
		}

		const VSDescriptor::VideoStreamingInterfaceTypes type = VSDescriptor::VideoStreamingInterfaceTypes(bDescriptorSubtype);

		switch (type)
		{
			case VSDescriptor::VS_INPUT_HEADER:
			{
#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_INPUT_HEADER, with descriptor size " << descriptorSize;
#endif

				if (vsInputHeaderDescriptor_.isValid())
				{
					Log::error() << "VS_INPUT_HEADER exists already";
					return false;
				}

				vsInputHeaderDescriptor_ = VSInputHeaderDescriptor(remainingBuffer, descriptorSize);

				if (!vsInputHeaderDescriptor_.isValid())
				{
					Log::error() << "VS_INPUT_HEADER is invalid";
					return false;
				}

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << vsInputHeaderDescriptor_.toString();
#endif
				break;
			}

			case VSDescriptor::VS_OUTPUT_HEADER:
			{
#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Skipping VS_OUTPUT_HEADER";
#endif

				break;
			}

			case VSDescriptor::VS_STILL_FRAME:
			{
#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Skipping VS_STILL_FRAME";
#endif

				break;
			}

			case VSDescriptor::VS_FORMAT_UNCOMPRESSED:
			{
				VSUncompressedVideoFormatDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FORMAT_UNCOMPRESSED, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (descriptor.isValid())
				{
					UncompressedVideoStream& newStream = uncompressedVideoStreams_.emplace_back();

					newStream.vsUncompressedVideoFormatDescriptor_ = std::move(descriptor);

					currentFrameDescriptorType = VSDescriptor::VS_FORMAT_UNCOMPRESSED;
				}
				else
				{
					Log::error() << "Invalid VS_FORMAT_UNCOMPRESSED descriptor";
				}

				break;
			}

			case VSDescriptor::VS_FRAME_UNCOMPRESSED:
			{
				VSUncompressedVideoFrameDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FRAME_UNCOMPRESSED, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (uncompressedVideoStreams_.empty())
				{
					Log::error() << "VS_FRAME_UNCOMPRESSED without previous format descriptor.";
					break;
				}

				if (descriptor.isValid())
				{
					uncompressedVideoStreams_.back().vsUncompressedVideoFrameDescriptors_.emplace_back(std::move(descriptor));
				}
				else
				{
					Log::error() << "Invalid VS_FRAME_UNCOMPRESSED descriptor";
				}

				break;
			}

			case VSDescriptor::VS_FORMAT_MJPEG:
			{
				VSMJPEGVideoFormatDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FORMAT_MJPEG, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (descriptor.isValid())
				{
					MJPEGVideoStream& newStream = mjpegVideoStreams_.emplace_back();

					newStream.vsMJPEGVideoFormatDescriptor_ = std::move(descriptor);

					currentFrameDescriptorType = VSDescriptor::VS_FORMAT_MJPEG;
				}
				else
				{
					Log::error() << "Invalid VS_FORMAT_MJPEG descriptor";
				}

				break;
			}

			case VSDescriptor::VS_FRAME_MJPEG:
			{
				VSMJPEGVideoFrameDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FRAME_MJPEG, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (mjpegVideoStreams_.empty())
				{
					Log::error() << "VS_FRAME_MJPEG without previous format descriptor.";
					break;
				}

				if (descriptor.isValid())
				{
					mjpegVideoStreams_.back().vsMJPEGVideoFrameDescriptors_.emplace_back(std::move(descriptor));
				}
				else
				{
					Log::error() << "Invalid VS_FRAME_MJPEG descriptor";
				}

				break;
			}

			case VSDescriptor::VS_COLORFORMAT:
			{
				VSColorMatchingDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_COLORFORMAT, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (descriptor.isValid())
				{
					switch (currentFrameDescriptorType)
					{
						case VSDescriptor::VS_UNDEFINED:
							Log::error() << "VS_COLORFORMAT without previous frame descriptor";
							break;

						case VSDescriptor::VS_FORMAT_UNCOMPRESSED:
						{
							ocean_assert(!uncompressedVideoStreams_.empty());

							UncompressedVideoStream& currentUncompressedVideoStream = uncompressedVideoStreams_.back();

							if (currentUncompressedVideoStream.vsColorMatchingDescriptor_.isValid())
							{
								Log::warning() << "The uncompressed video stream has a VS_COLORFORMAT descriptor already";
							}

							currentUncompressedVideoStream.vsColorMatchingDescriptor_ = std::move(descriptor);

							break;
						}

						case VSDescriptor::VS_FORMAT_MJPEG:
						{
							ocean_assert(!mjpegVideoStreams_.empty());

							MJPEGVideoStream& currentMJPEGVideoStream = mjpegVideoStreams_.back();

							if (currentMJPEGVideoStream.vsColorMatchingDescriptor_.isValid())
							{
								Log::warning() << "The MJPEG video stream has a VS_COLORFORMAT descriptor already";
							}

							currentMJPEGVideoStream.vsColorMatchingDescriptor_ = std::move(descriptor);

							break;
						}

						case VSDescriptor::VS_FORMAT_FRAME_BASED:
						{
							ocean_assert(!frameBasedVideoStreams_.empty());

							FrameBasedVideoStream& currentFrameBasedVideoStream = frameBasedVideoStreams_.back();

							if (currentFrameBasedVideoStream.vsColorMatchingDescriptor_.isValid())
							{
								Log::warning() << "The Frame Based video stream has a VS_COLORFORMAT descriptor already";
							}

							currentFrameBasedVideoStream.vsColorMatchingDescriptor_ = std::move(descriptor);

							break;
						}

						default:
							ocean_assert(false && "This should never happen!");
							break;
					}
				}
				else
				{
					Log::error() << "Invalid VS_COLORFORMAT descriptor";
				}

				break;
			}

			case VSDescriptor::VS_FORMAT_FRAME_BASED:
			{
				VSFrameBasedVideoFormatDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FORMAT_FRAME_BASED, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (descriptor.isValid())
				{
					FrameBasedVideoStream& newStream = frameBasedVideoStreams_.emplace_back();

					newStream.vsFrameBasedVideoFormatDescriptor_ = std::move(descriptor);

					currentFrameDescriptorType = VSDescriptor::VS_FORMAT_FRAME_BASED;
				}
				else
				{
					Log::error() << "Invalid VS_FORMAT_FRAME_BASED descriptor";
				}

				break;
			}

			case VSDescriptor::VS_FRAME_FRAME_BASED:
			{
				VSFrameBasedFrameDescriptor descriptor(remainingBuffer, descriptorSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Parsing VS_FRAME_FRAME_BASED, with descriptor size " << descriptorSize;
				Log::debug() << descriptor.toString();
#endif

				if (frameBasedVideoStreams_.empty())
				{
					Log::error() << "VS_FRAME_FRAME_BASED without previous format descriptor.";
					break;
				}

				if (descriptor.isValid())
				{
					frameBasedVideoStreams_.back().vsFrameBasedFrameDescriptors_.emplace_back(std::move(descriptor));
				}
				else
				{
					Log::error() << "Invalid VS_FRAME_FRAME_BASED descriptor";
				}

				break;
			}

			default:
			{
#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << "Unknown descriptor subtype: " << bDescriptorSubtype;
#endif

				break;
			}
		}

		remainingBuffer += descriptorSize;
		remainingSize -= size_t(descriptorSize);
	}

	ocean_assert(remainingSize == 0);

	bInterfaceIndex_ = interfaceDescriptor.bInterfaceNumber;

	bEndpointAddress_ = vsInputHeaderDescriptor_.endpointAddress();

	return true;
}

VideoDevice::VideoStreamingInterface::PriorityMap VideoDevice::VideoStreamingInterface::findBestMatchingStream(const unsigned int preferredWidth, const unsigned int preferredHeight, const double preferredFrameRate, const DeviceStreamType deviceStreamType, const FrameType::PixelFormat pixelFormat, VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat)
{
	PriorityMap priorityMap;

	constexpr unsigned int defaultPixels = 1280u * 720u;
	constexpr double defaultFrameRate = 30.0;

	float priorityOffset = 0.0f;
	constexpr float eps = 0.00001f;

	if (deviceStreamType == DST_INVALID || deviceStreamType == DST_UNCOMPRESSED)
	{
		const float basePriority = 9.5f;

		for (const UncompressedVideoStream& uncompressedVideoStream : uncompressedVideoStreams_)
		{
			if (pixelFormat != FrameType::FORMAT_UNDEFINED && uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_.pixelFormat() != pixelFormat)
			{
				// a pixel format is specified, but it does not match with the stream
				continue;
			}

			if (uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_.pixelFormat() == FrameType::FORMAT_UNDEFINED)
			{
				// we don't support the pixel format of the stream
				continue;
			}

			for (const VSUncompressedVideoFrameDescriptor& frameDescriptor : uncompressedVideoStream.vsUncompressedVideoFrameDescriptors_)
			{
				uint32_t frameInterval = 0u;

				const float resolutionFactor = determineResolutionFactor(frameDescriptor, preferredWidth, preferredHeight, defaultPixels);
				const float frameRateFactor = determineFrameRateFactor(frameDescriptor, preferredFrameRate, frameInterval, defaultFrameRate);

				if (resolutionFactor == 0.0f || frameRateFactor == 0.0f)
				{
					continue;
				}

				const float factor = (resolutionFactor + 0.1f) * frameRateFactor;

				const float priority = basePriority * factor + priorityOffset;
				priorityOffset += eps;

				priorityMap.emplace(priority, PriorityTriple(uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_.bFormatIndex_, frameDescriptor.bFrameIndex_, frameInterval));
			}
		}
	}

	if (deviceStreamType == DST_INVALID || deviceStreamType == DST_MJPEG)
	{
		const float basePriority = 9.75f;

		for (const MJPEGVideoStream& mjpegVideoStream : mjpegVideoStreams_)
		{
			for (const VSMJPEGVideoFrameDescriptor& frameDescriptor : mjpegVideoStream.vsMJPEGVideoFrameDescriptors_)
			{
				uint32_t frameInterval = 0u;

				const float resolutionFactor = determineResolutionFactor(frameDescriptor, preferredWidth, preferredHeight, defaultPixels);
				const float frameRateFactor = determineFrameRateFactor(frameDescriptor, preferredFrameRate, frameInterval, defaultFrameRate);

				if (resolutionFactor == 0.0f || frameRateFactor == 0.0f)
				{
					continue;
				}

				const float factor = (resolutionFactor + 0.1f) * frameRateFactor;

				const float priority = basePriority * factor + priorityOffset;
				priorityOffset += eps;

				priorityMap.emplace(priority, PriorityTriple(mjpegVideoStream.vsMJPEGVideoFormatDescriptor_.bFormatIndex_, frameDescriptor.bFrameIndex_, frameInterval));
			}
		}
	}

	if (deviceStreamType == DST_INVALID || deviceStreamType == DST_FRAME_BASED)
	{
		const float basePriority = 10.0f;

		for (const FrameBasedVideoStream& frameBasedVideoStream : frameBasedVideoStreams_)
		{
			if (encodingFormat != VSFrameBasedVideoFormatDescriptor::EF_INVALID && encodingFormat != frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_.encodingFormat())
			{
				continue;
			}

			if (frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_.encodingFormat() == VSFrameBasedVideoFormatDescriptor::EF_INVALID)
			{
				// we don't support the encoding format of the stream
				continue;
			}

			for (const VSFrameBasedFrameDescriptor& frameDescriptor : frameBasedVideoStream.vsFrameBasedFrameDescriptors_)
			{
				uint32_t frameInterval = 0u;

				const float resolutionFactor = determineResolutionFactor(frameDescriptor, preferredWidth, preferredHeight, defaultPixels);
				const float frameRateFactor = determineFrameRateFactor(frameDescriptor, preferredFrameRate, frameInterval, defaultFrameRate);

				if (resolutionFactor == 0.0f || frameRateFactor == 0.0f)
				{
					continue;
				}

				const float factor = (resolutionFactor + 0.1f) * frameRateFactor;

				const float priority = basePriority * factor + priorityOffset;
				priorityOffset += eps;

				priorityMap.emplace(priority, PriorityTriple(frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_.bFormatIndex_, frameDescriptor.bFrameIndex_, frameInterval));
			}
		}
	}

	return priorityMap;
}

template <typename T>
float VideoDevice::VideoStreamingInterface::determineResolutionFactor(const T& frameDescriptor, const unsigned int preferredWidth, const unsigned int preferredHeight, const unsigned int defaultPixels)
{
	ocean_assert(defaultPixels >= 1u);

	float resolutionFactor = 0.0f;

	if (preferredWidth == 0u && preferredHeight == 0u)
	{
		if (defaultPixels == 0u)
		{
			return 0.0f;
		}

		const float targetPixels = float(defaultPixels);

		const unsigned int descriptorPixels = frameDescriptor.wWidth_ * frameDescriptor.wHeight_;

		resolutionFactor = NumericF::abs(float(descriptorPixels) - targetPixels) / targetPixels;
	}
	else if (preferredWidth != 0u && preferredHeight != 0u)
	{
		const float targetPixels = float(preferredWidth * preferredHeight);

		const unsigned int descriptorPixels = frameDescriptor.wWidth_ * frameDescriptor.wHeight_;

		resolutionFactor = NumericF::abs(float(descriptorPixels) - targetPixels) / targetPixels;
	}
	else if (preferredWidth != 0u)
	{
		ocean_assert(preferredHeight == 0u);

		const float targetWidth = float(preferredWidth);

		resolutionFactor = NumericF::abs(float(frameDescriptor.wWidth_) - targetWidth) / targetWidth;
	}
	else
	{
		ocean_assert(preferredWidth == 0u);

		const float targetHeight = float(preferredHeight);

		resolutionFactor = NumericF::abs(float(frameDescriptor.wHeight_) - targetHeight) / targetHeight;
	}

	ocean_assert(resolutionFactor >= 0.0f);

	// Returns 1 for an input of 0 and values progressively closer to 0 as the input increases, following the exponential decay formula f(x) = e^(-x).
	return NumericF::exp(-resolutionFactor);
}

template <typename T>
float VideoDevice::VideoStreamingInterface::determineFrameRateFactor(const T& frameDescriptor, const double preferredFrameRate, uint32_t& frameInterval, const double defaultFrameRate)
{
	std::vector<double> frameRates;
	const Indices32 frameIntervals = frameDescriptor.frameIntervals(&frameRates);

	ocean_assert(frameIntervals.size() == frameRates.size());

	frameInterval = 0u;

	if (frameIntervals.empty())
	{
		ocean_assert(false && "This should never happen!");
		return 0.0f;
	}

	double targetFrameRate = defaultFrameRate;

	if (preferredFrameRate > 0.0)
	{
		targetFrameRate = preferredFrameRate;
	}

	if (targetFrameRate < 0.0)
	{
		return 0.0f;
	}

	double bestRatio = NumericD::maxValue();

	for (size_t n = 0; n < frameIntervals.size(); ++n)
	{
		const double ratio = NumericD::abs(frameRates[n] - targetFrameRate) / targetFrameRate;

		if (ratio < bestRatio)
		{
			bestRatio = ratio;

			frameInterval = frameIntervals[n];
		}
	}

	ocean_assert(bestRatio >= 0.0);

	// Returns 1 for an input of 0 and values progressively closer to 0 as the input increases, following the exponential decay formula f(x) = e^(-x).
	return float(NumericD::exp(-bestRatio));
}

VideoDevice::PayloadHeader::PayloadHeader(const uint8_t* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size >= 1);

	if (size < 2)
	{
		// setting the error bit
		bmHeaderInfo_ = 0b00100000u;
		return;
	}

	bHeaderLength_ = buffer[0];
	bmHeaderInfo_ = buffer[1];

	size_t variableOffset = 2;

	if (hasPresentationTime())
	{
		memcpy(&dwPresentationTime_, buffer + variableOffset, sizeof(dwPresentationTime_));
		variableOffset += sizeof(dwPresentationTime_);
	}

	if (hasSourceClockReference())
	{
		memcpy(&scrSourceClock_, buffer + variableOffset, sizeof(scrSourceClock_));
		variableOffset += sizeof(scrSourceClock_);
	}
}

VideoDevice::Sample::Sample(const size_t capacity, const uint8_t descriptorFormatIndex, const uint8_t descriptorFrameIndex, const uint32_t dwClockFrequency) :
	descriptorFormatIndex_(descriptorFormatIndex),
	descriptorFrameIndex_(descriptorFrameIndex),
	dwClockFrequency_(dwClockFrequency)
{
	buffer_.resize(capacity);

	ocean_assert(dwClockFrequency_ != 0u);
}

Timestamp VideoDevice::Sample::determineCaptureTimestamp() const
{
	ocean_assert(dwClockFrequency_ != 0u);

	if (nextDeviceTimeIndex_ == 0 || captureDeviceTime_ == uint64_t(-1))
	{
		// the sample did not deliver any time/clock information
		return Timestamp(false);
	}

	uint64_t captureDeviceTime = captureDeviceTime_;

	if (nextDeviceTimeIndex_ == 1)
	{
		ocean_assert(payloadDeviceTimes_[0] != uint64_t(-1));
		ocean_assert(payloadHostTimestamps_[0].isValid());

		// both times 'captureDeviceTime_' and 'payloadDeviceTimes_' have the same unit and domain - however, due to the 32bit resolution (and the high clock rate), they may be wrapped around 2^32 (we only see the modulo times)

		uint64_t payloadDeviceTime = payloadDeviceTimes_[0];

		unwrapTimestamps(captureDeviceTime, payloadDeviceTime);

		ocean_assert(captureDeviceTime <= uint32_t(uint32_t(-1)) * 2ull);
		ocean_assert(payloadDeviceTime <= uint32_t(uint32_t(-1)) * 2ull);

		const int64_t payloadDelay = int64_t(payloadDeviceTime) - int64_t(captureDeviceTime); // we expect a positive delay as the sample should have been captured before the payload arrived

		const double sPayloadDelay = double(payloadDelay) / double(dwClockFrequency_);

		return payloadHostTimestamps_[0] - sPayloadDelay;
	}
	else
	{
		ocean_assert(nextDeviceTimeIndex_ == 2);

		ocean_assert(payloadDeviceTimes_[0] != uint64_t(-1) && payloadDeviceTimes_[1] != uint64_t(-1));
		ocean_assert(payloadHostTimestamps_[0].isValid() && payloadHostTimestamps_[1].isValid());
		ocean_assert(payloadHostTimestamps_[0] <= payloadHostTimestamps_[1]);

		uint64_t payloadDeviceTimeFirst = payloadDeviceTimes_[0];
		uint64_t payloadDeviceTimeLast = payloadDeviceTimes_[1];

		unwrapTimestamps(payloadDeviceTimeFirst, payloadDeviceTimeLast);
		ocean_assert(payloadDeviceTimeFirst <= payloadDeviceTimeLast || payloadHostTimestamps_[1] >= payloadHostTimestamps_[0]);

		if (unwrapTimestamps(captureDeviceTime, payloadDeviceTimeFirst) == 1)
		{
			// the payload time needed to be wrapped, which means we also need to wrap the last payload time
			payloadDeviceTimeLast += uint64_t(uint32_t(-1));
		}

		ocean_assert(payloadDeviceTimeFirst <= payloadDeviceTimeLast);

		const int64_t payloadDelayFirst = int64_t(payloadDeviceTimeFirst) - int64_t(captureDeviceTime); // we expect a positive delay as the sample should have been captured before the payload arrived
		const int64_t payloadDelayLast = int64_t(payloadDeviceTimeLast) - int64_t(captureDeviceTime);

		const double sPayloadDelayFirst = double(payloadDelayFirst) / double(dwClockFrequency_);
		const double sPayloadDelayLast = double(payloadDelayLast) / double(dwClockFrequency_);

		const double hostTimestampFirst = double(payloadHostTimestamps_[0] - sPayloadDelayFirst);
		const double hostTimestampLast = double(payloadHostTimestamps_[1] - sPayloadDelayLast);
		ocean_assert(hostTimestampLast - hostTimestampFirst <= 1.0);

		return Timestamp((hostTimestampFirst + hostTimestampLast) * 0.5);
	}
}

std::string VideoDevice::Sample::toString() const
{
	std::string result;

	result += "Capacity: " + String::toAString(buffer_.size());
	result += "\nSize: " + String::toAString(position_);
	result += "\nCapture device time: " + String::toAString(captureDeviceTime_);

	result += "\nPayload timestamps: " + String::toAString(nextDeviceTimeIndex_);
	result += "\nPayload device time0: " + String::toAString(payloadDeviceTimes_[0]);
	result += "\nPayload device time1: " + String::toAString(payloadDeviceTimes_[1]);

	result += "\nHost payload timestamp0: " + String::toAString(double(payloadHostTimestamps_[0]), 5u);
	result += "\nHost payload timestamp1: " + String::toAString(double(payloadHostTimestamps_[1]), 5u);

	return result;
}

bool VideoDevice::Sample::append(const PayloadHeader& payloadHeader, const void *data, const size_t size)
{
	ocean_assert(data != nullptr && size > 0);
	ocean_assert(!buffer_.empty());

	if (payloadHeader.hasError())
	{
		mayContainError_ = true;
	}

	if (payloadHeader.hasSourceClockReference())
	{
		if (nextDeviceTimeIndex_ == 0 || payloadHeader.isEndOfFrame())
		{
			ocean_assert(nextDeviceTimeIndex_ <= 1);

			ocean_assert(payloadDeviceTimes_[nextDeviceTimeIndex_] == uint64_t(-1));
			ocean_assert(payloadHostTimestamps_[nextDeviceTimeIndex_].isInvalid());

			payloadDeviceTimes_[nextDeviceTimeIndex_] = uint64_t(payloadHeader.sourceTimeClock());
			payloadHostTimestamps_[nextDeviceTimeIndex_].toNow();

			++nextDeviceTimeIndex_;
		}
	}

	if (payloadHeader.hasPresentationTime())
	{
		if (captureDeviceTime_ == uint64_t(-1))
		{
			captureDeviceTime_ = uint64_t(payloadHeader.dwPresentationTime_);
		}
		else
		{
			// the presentation time (the capture time) of a sample should never change within a frame/sample

			if (captureDeviceTime_ != uint64_t(payloadHeader.dwPresentationTime_))
			{
				// however, some device seem to set the presentation time bit of 'bmHeaderInfo_' but provide a time == 0 (e.g., C270 HD WEBCAM)
				// so, we try to catch/ignore this case

				if (captureDeviceTime_ == 0ull)
				{
					// now the presentation time is actually valid for the first time
					captureDeviceTime_ = uint64_t(payloadHeader.dwPresentationTime_);
				}
				else if (payloadHeader.dwPresentationTime_ == 0u)
				{
					// we rate the presentation time to be invalid
				}
				else
				{
					// we still have two individual presentation times within the same sample - let's use the earlier time of both

					captureDeviceTime_ = earlierTimestamp(captureDeviceTime_, uint64_t(payloadHeader.dwPresentationTime_));
				}
			}
		}
	}

	ocean_assert(position_ <= buffer_.size());
	if (position_ + size > buffer_.size())
	{
		if (allowToResize_)
		{
			const size_t newBufferSize = buffer_.size() + std::max(size, size_t(1024 * 10));

			if (newBufferSize > sampleMaxPayloadSize_)
			{
				buffer_.resize(newBufferSize);

				Log::debug() << "Increased buffer to " << buffer_.size();
			}
			else
			{
				Log::error() << "Buffer is getting to large";
				return false;
			}
		}
		else
		{
			Log::warning() << "Refusing to extend payload buffer from " << buffer_.size() << " to " << position_ + size;
			return false;
		}
	}

	memcpy(buffer_.data() + position_, data, size);
	position_ += size;

	return true;
}

void VideoDevice::Sample::reset()
{
	position_ = 0;

	mayContainError_ = false;

	captureDeviceTime_ = uint64_t(-1);

	payloadDeviceTimes_[0] = uint64_t(-1);
	payloadDeviceTimes_[1] = uint64_t(-1);

	payloadHostTimestamps_[0].toInvalid();
	payloadHostTimestamps_[1].toInvalid();

	nextDeviceTimeIndex_ = 0;
}

int VideoDevice::Sample::unwrapTimestamps(uint64_t& timestampA, uint64_t& timestampB)
{
	// timestamp A and timestamp B need to be reasonable close together (<<<< ~2^31)

	constexpr uint64_t maxTimeDifference = 1ull << 31ull;

	int indexWrapped = -1;

	if (timestampA < timestampB)
	{
		//  0                                   2^32-1
		// [----------------A----B--------------------]

		if (timestampB - timestampA > maxTimeDifference)
		{
			//  0                                   2^32-1
			// [---A---------------------------------B----]

			// timestamp A was wrapped around 2^32 (it's actually behind the timestamp B, so unwrapping it
			timestampA += uint64_t(uint32_t(-1));

			//  0                                   2^32-1
			// [-------------------------------------B----|---A--------------------------------------]

			ocean_assert(timestampA - timestampB < maxTimeDifference);

			indexWrapped = 0;
		}
	}
	else
	{
		if (timestampA - timestampB> maxTimeDifference)
		{
			timestampB += uint64_t(uint32_t(-1));

			ocean_assert(timestampB - timestampA < maxTimeDifference);

			indexWrapped = 1;
		}
	}

#ifdef OCEAN_DEBUG
	if (timestampA < timestampB)
	{
		const int64_t difference = timestampB - timestampA;
		ocean_assert(difference < maxTimeDifference / 10ull);
	}
	else
	{
		const int64_t difference = timestampA - timestampB;
		ocean_assert(difference < maxTimeDifference / 10ull);
	}
#endif

	return indexWrapped;
}

uint64_t VideoDevice::Sample::earlierTimestamp(const uint64_t timestampA, const uint64_t timestampB)
{
	// timestamp A and timestamp B need to be reasonable close together (<<<< ~2^31)

	// we cannot simply return the timestamp with is lesser than the other because timestamps are wrapped around 2^32

	constexpr uint64_t maxTimeDifference = 1ull << 31ull;

	if (timestampA < timestampB)
	{
		//  0                                   2^32-1
		// [----------------A----B--------------------]

		if (timestampB - timestampA > maxTimeDifference)
		{
			//  0                                   2^32-1
			// [---A---------------------------------B----]

			// timestamp A is actually later than timestamp B, thus timestamp B is the earlier timestamp

			return timestampB;
		}

		return timestampA;
	}
	else
	{
		if (timestampA - timestampB> maxTimeDifference)
		{
			return timestampA;
		}

		return timestampB;
	}
}

VideoDevice::VideoDevice(Device&& device) :
	Device(std::move(device))
{
	if (parseInterfaces())
	{
		if (initializeControlInterface())
		{
			ocean_assert(isValid());
		}
		else
		{
			Log::error() << "VideoDevice: Failed to initialize the control interface";

			ocean_assert(!isValid());
		}
	}
	else
	{
		Log::error() << "VideoDevice: Failed to parse interfaces";

		ocean_assert(!isValid());
	}
}

VideoDevice::~VideoDevice()
{
	{
		const ScopedLock scopedLock(lock_);

		stop();

		if (interruptTransfer_.isValid())
		{
			libusb_cancel_transfer(*interruptTransfer_);
		}
	}

	const Timestamp startTimestamp(true);

	while (!startTimestamp.hasTimePassed(5.0))
	{
		Thread::sleep(1u);

		const ScopedLock scopedLock(lock_);

		if (!interruptTransfer_.isValid())
		{
			break;
		}
	}

	const ScopedLock scopedLock(lock_);

	claimedVideoControlInterfaceSubscription_.release();
}

bool VideoDevice::initializeControlInterface()
{
	ocean_assert(Device::isValid() && videoControlInterface_.isValid() && videoStreamingInterface_.isValid());
	if (!Device::isValid() || !videoControlInterface_.isValid() || !videoStreamingInterface_.isValid())
	{
		return false;
	}

	ocean_assert(!initializeControlInterfaceInitialized_);

#ifdef OCEAN_DEBUG
	{
		const int isActiveResult = libusb_kernel_driver_active(this->usbDeviceHandle_, videoControlInterface_.bInterfaceIndex_);

		if (isActiveResult == LIBUSB_ERROR_NOT_SUPPORTED)
		{
			// this platform does not support interacting with the kernel driver
			// so we skip detaching the driver, however this may results in issues later in the pipeline
		}
		else
		{
			if (isActiveResult == 0)
			{
				Log::debug() << "Kernel driver is not active";
			}
			else if (isActiveResult == 1)
			{
				Log::debug() << "Kernel driver is active";
			}
			else
			{
				Log::error() << "Failed to determine if kernel driver is active, error: " << libusb_error_name(isActiveResult);
			}
		}
	}
#endif

	bool driverWasNotActive = false;
	detachedKernelDriverSubscription_ = detachKernelDriver(videoControlInterface_.bInterfaceIndex_, &driverWasNotActive);

	if (!detachedKernelDriverSubscription_)
	{
		if (driverWasNotActive)
		{
			Log::warning() << "VideoDevice: Failed to detach kernel driver from interface: " << int(videoControlInterface_.bInterfaceIndex_) << ", however it seems that no driver was attached, so there may be no need to detach the driver";
		}
		else
		{
			Log::error() << "VideoDevice: Failed to detach kernel driver from interface: " << int(videoControlInterface_.bInterfaceIndex_);
		}
	}

	claimedVideoControlInterfaceSubscription_ = claimInterface(videoControlInterface_.bInterfaceIndex_);

	if (claimedVideoControlInterfaceSubscription_)
	{
		Log::debug() << "VideoDevice: Succeeded to claim video control interface " << int(videoControlInterface_.bInterfaceIndex_);
	}
	else
	{
		Log::error() << "VideoDevice: Failed to claim video control interface " << int(videoControlInterface_.bInterfaceIndex_);
	}

	interruptTransfer_ = ScopedTransfer(libusb_alloc_transfer(0));

	static uint8_t buffer[64];
	libusb_fill_interrupt_transfer(*interruptTransfer_, usbDeviceHandle_, videoControlInterface_.bEndpointAddress_, buffer, sizeof(buffer), libStatusCallback, this, 0u);

	const int submitResult = libusb_submit_transfer(*interruptTransfer_);

	if (submitResult == LIBUSB_SUCCESS)
	{
		Log::debug() << "VideoDevice: Succeeded to submit interrupt transfer for the video control interface";
	}
	else
	{
		Log::error() << "VideoDevice: Failed to submit interrupt transfer for the video control interface: " << libusb_error_name(submitResult);
		return false;
	}

	initializeControlInterfaceInitialized_ = true;

	return true;
}

bool VideoDevice::parseInterfaces()
{
	ocean_assert(Device::isValid());
	if (!Device::isValid())
	{
		return false;
	}

	libusb_device_descriptor usbDescriptor;
	const int result = libusb_get_device_descriptor(usbDevice_, &usbDescriptor);

	if (result < 0)
	{
		Log::error() << "Failed to determine device description for device";
		return false;
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	Log::debug() << "Parsing interfaces of video device:";
	Log::debug() << usbDescriptor.idVendor << ", " << usbDescriptor.idProduct << ", " << usbDescriptor.bDeviceClass << ", " << usbDescriptor.bDeviceSubClass << ", " << libusb_get_port_number(usbDevice_) << ", " << libusb_get_device_address(usbDevice_);
	Log::debug() << "Product name: " << stringDescriptor(usbDeviceHandle_, usbDescriptor.iProduct);

	Log::debug() << "The device has " << usbDescriptor.bNumConfigurations << " configuration(s)";
#endif

	for (uint8_t configurationIndex = 0u; configurationIndex < usbDescriptor.bNumConfigurations; ++configurationIndex)
	{
		if (configurationIndex != 0u)
		{
			Log::warning() << "The device has more than one configuration, skipping after first configuration";
			break;
		}

		ocean_assert(usbConfigDescriptor_ == nullptr);
		const int conifgDescriptorResult = libusb_get_config_descriptor(usbDevice_, configurationIndex, &usbConfigDescriptor_);

		if (conifgDescriptorResult == LIBUSB_SUCCESS)
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << "The configuration " << configurationIndex << " has " <<  usbConfigDescriptor_->bNumInterfaces << " interface(s)";
#endif

			for (uint8_t interfaceIndex = 0u; interfaceIndex < usbConfigDescriptor_->bNumInterfaces; ++interfaceIndex)
			{
				const libusb_interface& interface = usbConfigDescriptor_->interface[interfaceIndex];

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Interface: " << interfaceIndex;
				Log::debug() << "Number of altsetting settings: " << interface.num_altsetting;
#endif // OCEAN_INTENSIVE_DEBUG

				for (int altsettingIndex = 0; altsettingIndex < interface.num_altsetting; ++altsettingIndex)
				{
					const libusb_interface_descriptor& interfaceDescriptor = interface.altsetting[altsettingIndex];

#ifdef OCEAN_INTENSIVE_DEBUG
					Log::debug() << " ";
					Log::debug() << "Parsing altsetting index " << altsettingIndex << ", bInterfaceNumber " << int(interfaceDescriptor.bInterfaceNumber) << ", class: " << interfaceDescriptor.bInterfaceClass << ", " << interfaceDescriptor.bInterfaceSubClass;
#endif

					if (interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_AUDIO)
					{
#ifdef OCEAN_INTENSIVE_DEBUG
						Log::debug() << "Skipping audio interface";
#endif
					}
					else if (interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_VIDEO)
					{
						if (!parseVideoInterface(interfaceDescriptor))
						{
							Log::error() << "Failed to parse video class interface";
						}
					}
					else if (interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_HID)
					{
#ifdef OCEAN_INTENSIVE_DEBUG
						Log::debug() << "Skipping Human Interface Device (HID) interface";
#endif
					}
					else
					{
#ifdef OCEAN_INTENSIVE_DEBUG
						Log::debug() << "Skipping unknown interface class";
#endif
					}
				}
			}
		}
		else
		{
			Log::error() << "Failed to determine configuration descriptor, error " << libusb_error_name(conifgDescriptorResult);
		}
	}

	return true;
}

bool VideoDevice::parseVideoInterface(const libusb_interface_descriptor& interfaceDescriptor)
{
	ocean_assert(interfaceDescriptor.bInterfaceClass == LIBUSB_CLASS_VIDEO);
	if (interfaceDescriptor.bInterfaceClass != LIBUSB_CLASS_VIDEO)
	{
		return false;
	}

	switch (interfaceDescriptor.bInterfaceSubClass)
	{
		case SC_VIDEOCONTROL:
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << " ";
			Log::debug() << "Parsing sub-class SC_VIDEOCONTROL";
#endif

			if (videoControlInterface_.isValid())
			{
				Log::error() << "Video control interface already exists";
				return false;
			}

			videoControlInterface_ = VideoControlInterface(interfaceDescriptor, usbDeviceHandle_);

			if (!videoControlInterface_.isValid())
			{
				Log::error() << "Failed to parse video control interface";
				return false;
			}

			return true;
		}

		case SC_VIDEOSTREAMING:
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << " ";
			Log::debug() << "Parsing sub-class SC_VIDEOSTREAMING";
#endif

			if (!videoStreamingInterface_.parseAdditionalInterface(interfaceDescriptor, usbDeviceHandle_))
			{
				Log::error() << "Failed to parse an additional video streaming interface: ";
				return false;
			}

			return true;
		}

		case SC_VIDEO_INTERFACE_COLLECTION:
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << " ";
			Log::debug() << "Skipping sub-class SC_VIDEO_INTERFACE_COLLECTION";
#endif

			return true;
		}

		default:
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << " ";
			Log::debug() << "Unknown interface sub-class: "  << interfaceDescriptor.bInterfaceSubClass;
#endif

			break;
		}
	}

	return false;
}

bool VideoDevice::start(const unsigned int preferredWidth, const unsigned int preferredHeight, const double preferredFrameRate, const DeviceStreamType preferredDeviceStreamType, const FrameType::PixelFormat preferredPixelFormat, const VSFrameBasedVideoFormatDescriptor::EncodingFormat preferredEncodingFormat)
{
#ifdef OCEAN_INTENSIVE_DEBUG
	Log::debug() << "VideoDevice::start(): " << preferredWidth << "x" << preferredHeight << ", " << String::toAString(preferredFrameRate, 1u) << "fps, " << translateDeviceStreamType(preferredDeviceStreamType) << ", " << FrameType::translatePixelFormat(preferredPixelFormat) << ", " + VSFrameBasedVideoFormatDescriptor::translateEncodingFormat(preferredEncodingFormat);
#endif

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());
	if (!isValid())
	{
		return false;
	}

	if (isStarted_)
	{
		return true;
	}

	ocean_assert(videoStreamingInterface_.isValid());

	const uint8_t streamingInterfaceIndex = videoStreamingInterface_.bInterfaceIndex_;

	if (streamingInterfaceIndex >= usbConfigDescriptor_->bNumInterfaces)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(!claimedVideoStreamInterfaceSubscription_);
	claimedVideoStreamInterfaceSubscription_ = claimInterface(streamingInterfaceIndex);

	if (!claimedVideoStreamInterfaceSubscription_)
	{
		Log::error() << "Failed to claim streaming interface " << int(streamingInterfaceIndex);
		return false;
	}

	activeDescriptorFormatIndex_ = 0u;
	activeDescriptorFrameIndex_ = 0u;
	activeClockFrequency_ = 0u;
	maximalSampleSize_ = 0u;

	uint32_t dwMaxVideoFrameSize = 0u;
	uint32_t dwMaxPayloadTransferSize = 0u;

	DeviceStreamType deviceStreamType = preferredDeviceStreamType;
	FrameType::PixelFormat pixelFormat = preferredPixelFormat;
	VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat = preferredEncodingFormat;

	while (true)
	{
		const VideoStreamingInterface::PriorityMap priorityMap = videoStreamingInterface_.findBestMatchingStream(preferredWidth, preferredHeight, preferredFrameRate, deviceStreamType, pixelFormat, encodingFormat);

		if (!priorityMap.empty())
		{
#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << "Found " << priorityMap.size() << " matching streams:";

			for (VideoStreamingInterface::PriorityMap::const_reverse_iterator iPriority = priorityMap.crbegin(); iPriority != priorityMap.crend(); ++iPriority)
			{
				const VideoStreamingInterface::PriorityTriple& priorityTriple = iPriority->second;

				unsigned int width;
				unsigned int height;
				FrameType::PixelFormat pixelFormat;
				VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat;

				const DeviceStreamType deviceStreamType = extractStreamProperties(priorityTriple.first(), priorityTriple.second(), width, height, pixelFormat, encodingFormat);

				Log::debug() << iPriority->first << ": " << priorityTriple.first() << " " << priorityTriple.second() << " " << priorityTriple.third() << ": " << translateDeviceStreamType(deviceStreamType) + ": " << width << "x" << height << ", " << FrameType::translatePixelFormat(pixelFormat) << ", " << VSFrameBasedVideoFormatDescriptor::translateEncodingFormat(encodingFormat);
			}
#endif // OCEAN_INTENSIVE_DEBUG

			const VideoStreamingInterface::PriorityTriple priorityTriple = priorityMap.rbegin()->second;
			ocean_assert(priorityTriple.first() != 0u && priorityTriple.second() != 0u && priorityTriple.third() != 0u);

			size_t controlBufferSize = 26;
			if (videoControlInterface_.vcHeaderDescriptor_.bcdUVC_ >= 0x0110u)
			{
				controlBufferSize = sizeof(VideoControl);
			}

#ifdef OCEAN_INTENSIVE_DEBUG
			VideoControl getMaxVideoControl;
			if (VideoControl::executeVideoControlProbe(usbDeviceHandle_, streamingInterfaceIndex, getMaxVideoControl, controlBufferSize, VideoControl::RC_GET_MAX))
			{
				Log::debug() << " ";
				Log::debug() << "Get max video control:\n" << getMaxVideoControl.toString();
				Log::debug() << " ";
			}
#endif

			VideoControl commitVideoControl;

			commitVideoControl.bmHint_ = 1u << 0u; // try to prioritize dwFrameInterval
			commitVideoControl.bFormatIndex_ = priorityTriple.first();
			commitVideoControl.bFrameIndex_ = priorityTriple.second();
			commitVideoControl.dwFrameInterval_ = priorityTriple.third();

			if (VideoControl::executeVideoControlCommit(usbDeviceHandle_, streamingInterfaceIndex, commitVideoControl, controlBufferSize))
			{

#ifdef OCEAN_INTENSIVE_DEBUG
				Log::debug() << " ";
				Log::debug() << "Commit video control:\n" << commitVideoControl.toString();
				Log::debug() << " ";
#endif

				VideoControl probeVideoControl;
				if (VideoControl::executeVideoControlProbe(usbDeviceHandle_, streamingInterfaceIndex, probeVideoControl, controlBufferSize))
				{
#ifdef OCEAN_INTENSIVE_DEBUG
					Log::debug() << " ";
					Log::debug() << "Probe video control:\n" << probeVideoControl.toString();
					Log::debug() << " ";
#endif

					if (probeVideoControl.dwClockFrequency_ != 0u)
					{
						activeClockFrequency_ = probeVideoControl.dwClockFrequency_;
					}

					dwMaxPayloadTransferSize = probeVideoControl.dwMaxPayloadTransferSize_;
					dwMaxVideoFrameSize = probeVideoControl.dwMaxVideoFrameSize_;

					// some webcams return an invalid format/frame in the probe, so we use the commit values instead
					// further some cameras return an invalid probe 'dwMaxVideoFrameSize' value when using uncompressed video frames (e.g., BRIO 301), so that we always re-calculate the frame size manually for uncompressed video streams

					unsigned int priorityWidth = 0u;
					unsigned int priorityHeight = 0u;
					FrameType::PixelFormat priorityPixelFormat = FrameType::FORMAT_UNDEFINED;
					VSFrameBasedVideoFormatDescriptor::EncodingFormat priorityEncodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID;
					const DeviceStreamType priorityDeviceStreamType = extractStreamProperties(priorityTriple.first(), priorityTriple.second(), priorityWidth, priorityHeight, priorityPixelFormat, priorityEncodingFormat);

					if (priorityDeviceStreamType == DST_UNCOMPRESSED)
					{
						const FrameType frameType(priorityWidth, priorityHeight, priorityPixelFormat, FrameType::ORIGIN_UPPER_LEFT);

						ocean_assert(frameType.isValid());
						if (frameType.isValid())
						{
							const unsigned int expectedFrameSize = frameType.frameTypeSize();

							if (expectedFrameSize != dwMaxVideoFrameSize)
							{
								Log::warning() << "VideoDevice: Detected invalid frame size for uncompressed video stream, expected: " << expectedFrameSize << ", claimed: " << dwMaxVideoFrameSize << ", using the expected frame size instead";

								dwMaxVideoFrameSize = expectedFrameSize;
							}
						}
						else
						{
							Log::error() << "VideoDevice: Failed to determine frame type for uncompressed video stream";
						}
					}

					activeDescriptorFormatIndex_ = commitVideoControl.bFormatIndex_;
					activeDescriptorFrameIndex_ = commitVideoControl.bFrameIndex_;

					break;
				}
				else
				{
					Log::error() << "Failed to receive probe video control";
				}
			}
			else
			{
				Log::error() << "Failed to commit video control";

				Log::debug() << "The commit was: " << commitVideoControl.toString();
			}
		}
		else
		{
			Log::warning() << "No matching stream type";
		}

		if (deviceStreamType == DST_INVALID && pixelFormat == FrameType::FORMAT_UNDEFINED && encodingFormat == VSFrameBasedVideoFormatDescriptor::EF_INVALID)
		{
			break;
		}

		// we set the stream type, pixel format, and encoding format to invalid to increase the chance to find a matching stream

		deviceStreamType = DST_INVALID;
		pixelFormat = FrameType::FORMAT_UNDEFINED;
		encodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID;
	}

	if (activeDescriptorFormatIndex_ == 0u || activeDescriptorFrameIndex_ == 0u || dwMaxVideoFrameSize == 0u)
	{
		Log::error() << "No valid stream found";

		claimedVideoStreamInterfaceSubscription_.release();
		return false;
	}

	if (dwMaxPayloadTransferSize == 0u)
	{
		Log::error() << "Unknown maximal payload size";
	}

	if (activeClockFrequency_ == 0u)
	{
		activeClockFrequency_ = videoControlInterface_.vcHeaderDescriptor_.dwClockFrequency_;
	}

	if (activeClockFrequency_ == 0u)
	{
		Log::error() << "The device clock is unknown";

		claimedVideoStreamInterfaceSubscription_.release();
		return false;
	}

	maximalSampleSize_ = size_t(dwMaxVideoFrameSize);

	ocean_assert(activeSample_ == nullptr);
	activeSample_ = std::make_shared<Sample>(maximalSampleSize_, activeDescriptorFormatIndex_, activeDescriptorFrameIndex_, activeClockFrequency_);

	// let's a second sample for double buffering (addtional samples will be added on demand)
	reusableSamples_.emplace_back(std::make_shared<Sample>(maximalSampleSize_, activeDescriptorFormatIndex_, activeDescriptorFrameIndex_, activeClockFrequency_));

	ocean_assert(streamingTransfers_.empty());
	ocean_assert(streamingTransferMemories_.empty());
	streamingTransfers_.clear();
	streamingTransferMemories_.clear();

	ocean_assert(streamingInterfaceIndex < usbConfigDescriptor_->bNumInterfaces);
	const libusb_interface& interface = usbConfigDescriptor_->interface[streamingInterfaceIndex];

	constexpr unsigned int numberTransferBuffers = 100u;

	const uint8_t endpointAddress = videoStreamingInterface_.bEndpointAddress_;

	const bool isochronousStreaming = interface.num_altsetting > 1u;

	if (isochronousStreaming)
	{
		size_t transferSize = 0;
		size_t packetsPerTransfer = 0;
		size_t bytesPerPacket = 0;

		const int altsettingIndex = determineIsochronousTransferLayout(context_ ? context_->usbContext() : nullptr, interface, endpointAddress, dwMaxVideoFrameSize, dwMaxPayloadTransferSize, transferSize, packetsPerTransfer, bytesPerPacket);

		ocean_assert(altsettingIndex < interface.num_altsetting);
		if (altsettingIndex < 0 || altsettingIndex >= interface.num_altsetting)
		{
			Log::error() << "VideoDevice: Failed to determine isochronous transfer layout";
			return false;
		}

		Log::debug() << "IsochronousTransferLayout:";
		Log::debug() << "TransferSize: " << transferSize;
		Log::debug() << "PacketsPerTransfer: " << packetsPerTransfer;
		Log::debug() << "BytesPerPacket: " << bytesPerPacket;

		const libusb_interface_descriptor& altsetting = interface.altsetting[altsettingIndex];

		const int altsettingResult = libusb_set_interface_alt_setting(usbDeviceHandle_, altsetting.bInterfaceNumber, altsetting.bAlternateSetting);

		if (altsettingResult != LIBUSB_SUCCESS)
		{
			Log::error() << "Failed to set altsetting";
			return false;
		}

		if (!NumericT<int>::isInsideValueRange(transferSize) || !NumericT<int>::isInsideValueRange(packetsPerTransfer) || !NumericT<int>::isInsideValueRange(bytesPerPacket))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		for (unsigned int transferIndex = 0u; transferIndex < numberTransferBuffers; ++transferIndex)
		{
			libusb_transfer* transfer = libusb_alloc_transfer(int(packetsPerTransfer));

			Memory memory(transferSize);

			libusb_fill_iso_transfer(transfer, usbDeviceHandle_, endpointAddress, (unsigned char*)(memory.data()), int(transferSize), int(packetsPerTransfer), libusbStreamCallback, this, 5000);

			libusb_set_iso_packet_lengths(transfer, (unsigned int)(bytesPerPacket));

			streamingTransfers_.emplace_back(transfer);
			streamingTransferMemories_.emplace_back(std::move(memory));
		}
	}
	else
	{
		ocean_assert(!isochronousStreaming);

		for (unsigned int transferIndex = 0u; transferIndex < numberTransferBuffers; ++transferIndex)
		{
			libusb_transfer* transfer = libusb_alloc_transfer(0);

			Memory memory(dwMaxPayloadTransferSize);

			libusb_fill_bulk_transfer(transfer, this->usbDeviceHandle_, endpointAddress, (unsigned char*)(memory.data()), dwMaxPayloadTransferSize, libusbStreamCallback, this, 5000);

			streamingTransfers_.emplace_back(transfer);
			streamingTransferMemories_.emplace_back(std::move(memory));
		}
	}

	ocean_assert(transferIndexMap_.empty());
	transferIndexMap_.clear();
	transferIndexMap_.reserve(streamingTransfers_.size());

	for (size_t n = 0; n < streamingTransfers_.size(); ++n)
	{
		ocean_assert(transferIndexMap_.find(*streamingTransfers_[n]) == transferIndexMap_.cend());
		transferIndexMap_.emplace(*streamingTransfers_[n], n);
	}

	Log::debug() << "VideoDevice: Starting " << numberTransferBuffers << " streaming transfers";

	for (unsigned int transferIndex = 0u; transferIndex < numberTransferBuffers; ++transferIndex)
	{
		const int submitResult = libusb_submit_transfer(*streamingTransfers_[transferIndex]);

		if (submitResult != 0)
		{
			Log::debug() << "Failed to submit transfer " << transferIndex << ": " << submitResult << ", " << libusb_error_name(submitResult);
		}
	}

	isStarted_ = true;

	return true;
}

bool VideoDevice::stop()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	Log::debug() << "VideoDevice::stop()";
#endif

	{
		// adjusting the device's states

		const ScopedLock scopedLock(lock_);

		if (!isStarted_)
		{
			return true;
		}

		ocean_assert(isValid());

		for (ScopedTransfer& transfer : streamingTransfers_)
		{
			const int cancelResult = libusb_cancel_transfer(*transfer);

			if (cancelResult != 0)
			{
				Log::info() << "Failed to cancel transfer: " << libusb_error_name(cancelResult);
			}
		}

		isStarted_ = false;

		activeDescriptorFormatIndex_ = 0u;
		activeDescriptorFrameIndex_ = 0u;
		activeClockFrequency_ = 0u;
		maximalSampleSize_ = 0u;

		activeSample_ = nullptr;

		claimedVideoStreamInterfaceSubscription_.release();
	}

	{
		// releasing remaining samples

		const ScopedLock scopedLock(samplesLock_);

		sampleQueue_ = SampleQueue();
		reusableSamples_.clear();
	}

	// now, we need to wait until all transfers are finished

	const Timestamp startTimestamp(true);

	while (true)
	{
		TemporaryScopedLock scopedLock(lock_);

			if (transferIndexMap_.empty())
			{
				streamingTransfers_.clear();
				streamingTransferMemories_.clear();
				break;
			}

		scopedLock.release();

		if (startTimestamp.hasTimePassed(5.0))
		{
			Log::warning() << "Failed to waite for transfers to finish";
			break;
		}

		Thread::sleep(1u);
	}

	return true;
}

VideoDevice::DeviceStreamType VideoDevice::extractStreamProperties(const uint8_t descriptorFormatIndex, const uint8_t descriptorFrameIndex, unsigned int& width, unsigned int& height, FrameType::PixelFormat& pixelFormat, VSFrameBasedVideoFormatDescriptor::EncodingFormat& encodingFormat) const
{
	if (descriptorFormatIndex == 0u || descriptorFrameIndex == 0u)
	{
		return DST_INVALID;
	}

	const ScopedLock scopedLock(lock_);

	if (!isValid())
	{
		return DST_INVALID;
	}

	for (const VideoStreamingInterface::UncompressedVideoStream& uncompressedVideoStream : videoStreamingInterface_.uncompressedVideoStreams_)
	{
		if (uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_.bFormatIndex_ == descriptorFormatIndex)
		{
			for (const VSUncompressedVideoFrameDescriptor& uncompressedVideoFrameDescriptor : uncompressedVideoStream.vsUncompressedVideoFrameDescriptors_)
			{
				if (uncompressedVideoFrameDescriptor.bFrameIndex_ == descriptorFrameIndex)
				{
					width = uncompressedVideoFrameDescriptor.wWidth_;
					height = uncompressedVideoFrameDescriptor.wHeight_;

					pixelFormat = uncompressedVideoStream.vsUncompressedVideoFormatDescriptor_.pixelFormat();

					encodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID;

					return DST_UNCOMPRESSED;
				}
			}
		}
	}

	for (const VideoStreamingInterface::MJPEGVideoStream& mjpegVideoStream : videoStreamingInterface_.mjpegVideoStreams_)
	{
		if (mjpegVideoStream.vsMJPEGVideoFormatDescriptor_.bFormatIndex_ == descriptorFormatIndex)
		{
			for (const VSMJPEGVideoFrameDescriptor& mjpegVideoFrameDescriptor : mjpegVideoStream.vsMJPEGVideoFrameDescriptors_)
			{
				if (mjpegVideoFrameDescriptor.bFrameIndex_ == descriptorFrameIndex)
				{
					width = mjpegVideoFrameDescriptor.wWidth_;
					height = mjpegVideoFrameDescriptor.wHeight_;

					pixelFormat = FrameType::FORMAT_UNDEFINED;

					encodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID;

					return DST_MJPEG;
				}
			}
		}
	}

	for (const VideoStreamingInterface::FrameBasedVideoStream& frameBasedVideoStream : videoStreamingInterface_.frameBasedVideoStreams_)
	{
		if (frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_.bFormatIndex_ == descriptorFormatIndex)
		{
			for (const VSFrameBasedFrameDescriptor& frameBasedVideoFrameDescriptor : frameBasedVideoStream.vsFrameBasedFrameDescriptors_)
			{
				if (frameBasedVideoFrameDescriptor.bFrameIndex_ == descriptorFrameIndex)
				{
					width = frameBasedVideoFrameDescriptor.wWidth_;
					height = frameBasedVideoFrameDescriptor.wHeight_;

					pixelFormat = FrameType::FORMAT_UNDEFINED;

					encodingFormat = frameBasedVideoStream.vsFrameBasedVideoFormatDescriptor_.encodingFormat();

					return DST_FRAME_BASED;
				}
			}
		}
	}

	ocean_assert(false && "Unknown combination of descriptor index and frame index");
	return DST_INVALID;
}

VideoDevice::SharedSample VideoDevice::nextSample(size_t* pendingSamples)
{
	if (pendingSamples != nullptr)
	{
		*pendingSamples = 0;
	}

	const ScopedLock scopedLock(samplesLock_);

	if (sampleQueue_.empty())
	{
		return nullptr;
	}

	SharedSample sample = std::move(sampleQueue_.front());
	sampleQueue_.pop();

	if (pendingSamples != nullptr)
	{
		*pendingSamples = sampleQueue_.size();
	}

	return sample;
}

void VideoDevice::giveSampleBack(SharedSample&& sample)
{
	ocean_assert(sample);

	if (sample)
	{
		sample->reset();

		const ScopedLock scopedLock(lock_);

		reusableSamples_.emplace_back(std::move(sample));
	}
}

void VideoDevice::processPayload(const BufferPointers& bufferPointers)
{
	for (const BufferPointer& bufferPointer : bufferPointers)
	{
		const uint8_t* data = (const uint8_t*)(bufferPointer.first);
		const size_t size = bufferPointer.second;

		ocean_assert(data != nullptr && size >= 1);

		const PayloadHeader payloadHeader(data, size);

		if (payloadHeader.hasError())
		{
			Log::error() << "Invalid payload header";
		}

		const size_t payloadSize = size - payloadHeader.bHeaderLength_;

		if (payloadSize > 0) // we skip buffers with zero payload (hopefully, we don't miss an important information from the header (e.g., timestamp)
		{
			ocean_assert(activeSample_);
			if (!activeSample_->append(payloadHeader, data + payloadHeader.bHeaderLength_, payloadSize))
			{
				ocean_assert(false && "Failed to append payload");
			}
		}

		// frame-based samples may be empty
		// however, buffers with empty payload may still indicate the end of the sample
		if (payloadHeader.isEndOfFrame() && activeSample_->size() != 0)
		{
			const ScopedLock scopedLock(samplesLock_);

			sampleQueue_.push(std::move(activeSample_));

			while (sampleQueue_.size() > maximalSampleQueueSize_)
			{
				Log::debug() << "USB VideoDevice: Dropping queued sample";

				SharedSample sample = std::move(sampleQueue_.front());
				sampleQueue_.pop();

				sample->reset();
				reusableSamples_.push_back(std::move(sample));
			}

			if (!reusableSamples_.empty())
			{
				activeSample_ = std::move(reusableSamples_.back());
				ocean_assert(activeSample_);

				reusableSamples_.pop_back();

				continue;
			}

			Log::warning() << "USB VideoDevice: Not enough reusable samples, need to add a new sample";
			activeSample_ = std::make_shared<Sample>(maximalSampleSize_, activeDescriptorFormatIndex_, activeDescriptorFrameIndex_, activeClockFrequency_);
		}
	}
}

bool VideoDevice::libStatusCallback(libusb_transfer& usbTransfer)
{
	const ScopedLock scopedLock(lock_);

	bool resubmit = true;

	switch (usbTransfer.status)
	{
		case LIBUSB_TRANSFER_COMPLETED:
			Log::debug() << "libusb transfer completed";
			break;

		case LIBUSB_TRANSFER_ERROR:
		{
			Log::error() << "libusb transfer error, type " << int(usbTransfer.type);
			resubmit = false;
			break;
		}

		case LIBUSB_TRANSFER_TIMED_OUT:
			Log::debug() << "libusb transfer timed out";
			break;

		case LIBUSB_TRANSFER_CANCELLED:
			Log::debug() << "libusb transfer canceled";
			resubmit = false;
			break;

		case LIBUSB_TRANSFER_STALL:
			Log::debug() << "libusb transfer stall";
			break;

		case LIBUSB_TRANSFER_NO_DEVICE:
			Log::debug() << "libusb transfer no device";
			resubmit = false;
			break;

		case LIBUSB_TRANSFER_OVERFLOW:
			Log::debug() << "libusb transfer overflow";
			break;
	}

	if (!resubmit)
	{
		if (*interruptTransfer_ == &usbTransfer)
		{
			interruptTransfer_.release();
		}
	}

	return resubmit;
}

bool VideoDevice::libusbStreamCallback(libusb_transfer& usbTransfer)
{
	const ScopedLock scopedLock(lock_);

	bool resubmit = false;

	if (isStarted_)
	{
		resubmit = true;

		ocean_assert(reusableBufferPointers_.empty());

		switch (usbTransfer.status)
		{
			case LIBUSB_TRANSFER_COMPLETED:
			{
				extractPayload(usbTransfer, reusableBufferPointers_);

				if (!reusableBufferPointers_.empty())
				{
					processPayload(reusableBufferPointers_);
				}

				reusableBufferPointers_.clear();

				break;
			}

			case LIBUSB_TRANSFER_ERROR:
				resubmit = false;
				Log::info() << "libusb streaming transfer error";
				break;

			case LIBUSB_TRANSFER_TIMED_OUT:
				Log::info() << "libusb streaming transfer timed out";
				break;

			case LIBUSB_TRANSFER_CANCELLED:
				Log::info() << "libusb streaming transfer canceled";
				resubmit = false;
				break;

			case LIBUSB_TRANSFER_STALL:
				Log::info() << "libusb streaming transfer stall";
				break;

			case LIBUSB_TRANSFER_NO_DEVICE:
				resubmit = false;
				Log::info() << "libusb streaming transfer no device";
				break;

			case LIBUSB_TRANSFER_OVERFLOW:
				Log::info() << "libusb streaming transfer overflow";
				break;
		}
	}

	if (!resubmit)
	{
		// we release transfer objects which will not be re-submitted so that we know when all transfers have finished (e.g., when the device stops)

		const TransferIndexMap::const_iterator iTransfer = transferIndexMap_.find(&usbTransfer);

		ocean_assert(iTransfer != transferIndexMap_.end());
		if (iTransfer != transferIndexMap_.end())
		{
			const size_t transferIndex = iTransfer->second;

			ocean_assert(transferIndex < streamingTransfers_.size());
			if (transferIndex < streamingTransfers_.size())
			{
				streamingTransfers_[transferIndex].release();
			}

			transferIndexMap_.erase(iTransfer);
		}
	}

	return resubmit;
}

std::string VideoDevice::translateDeviceStreamType(const DeviceStreamType deviceStreamType)
{
	switch (deviceStreamType)
	{
		case DST_INVALID:
			return std::string("Invalid");

		case DST_UNCOMPRESSED:
			return std::string("Uncompressed");

		case DST_MJPEG:
			return std::string("MJpeg");

		case DST_FRAME_BASED:
			return std::string("FrameBased");
	}

	ocean_assert(false && "Invalid stream type!");
	return std::string("Invalid");
}

void VideoDevice::libStatusCallback(libusb_transfer* usbTransfer)
{
	ocean_assert(usbTransfer != nullptr && usbTransfer->user_data);

	if (usbTransfer != nullptr && usbTransfer->user_data != nullptr)
	{
		VideoDevice* videoDevice = reinterpret_cast<VideoDevice*>(usbTransfer->user_data);
		ocean_assert(videoDevice != nullptr);

		if (videoDevice->libStatusCallback(*usbTransfer))
		{
			libusb_submit_transfer(usbTransfer);
		}
	}
}

void VideoDevice::libusbStreamCallback(libusb_transfer* usbTransfer)
{
	ocean_assert(usbTransfer != nullptr && usbTransfer->user_data);

	if (usbTransfer != nullptr && usbTransfer->user_data != nullptr)
	{
		VideoDevice* videoDevice = reinterpret_cast<VideoDevice*>(usbTransfer->user_data);
		ocean_assert(videoDevice != nullptr);

		if (videoDevice->libusbStreamCallback(*usbTransfer))
		{
			libusb_submit_transfer(usbTransfer);
		}
	}
}

}

}

}

}
