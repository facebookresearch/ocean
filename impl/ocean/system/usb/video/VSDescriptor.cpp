/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/video/VSDescriptor.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

VSInputHeaderDescriptor::VSInputHeaderDescriptor(const void* buffer, const size_t size)
{
	if (size < 13)
	{
		return;
	}

	value(0, bLength_, buffer);

	if (size_t(bLength_) != size)
	{
		invalidate(*this);
		return;
	}

	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bNumFormats_, buffer);
	value(4, wTotalLength_, buffer);
	value(6, bEndpointAddress_, buffer);
	value(7, bmInfo_, buffer);
	value(8, bTerminalLink_, buffer);
	value(9, bStillCaptureMethod_, buffer);
	value(10, bTriggerSupport_, buffer);
	value(11, bTriggerUsage_, buffer);
	value(12, bControlSize_, buffer);

	if (size != 13 + size_t(bNumFormats_ * bControlSize_))
	{
		ocean_assert(false && "Invalid buffer size!");

		invalidate(*this);
		return;
	}

	bmaControls_.resize(bNumFormats_, std::vector<uint8_t>(bControlSize_));

	for (uint8_t p = 0u; p < bNumFormats_; ++p)
	{
		memcpy(bmaControls_[p].data(), (const uint8_t*)(buffer) + 13 + p * bControlSize_, bControlSize_);
	}
}

std::string VSInputHeaderDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbNumFormats: " + String::toAString(int(bNumFormats_));
	result += "\nwTotalLength: " + String::toAString(int(wTotalLength_));
	result += "\nbEndpointAddress: 0x" + String::toAStringHex(bEndpointAddress_) + ", endpoint number: " + String::toAString(int(bEndpointAddress_ & 0b1111u));

	if (bEndpointAddress_ & (1u << 7u))
	{
		result += " (IN)";
	}

	result += "\nbmInfo: 0x" + String::toAStringHex(bmInfo_);

	if (bmInfo_ & (1u << 0u))
	{
		result += " (Dynamic Format Change supported)";
	}

	result += "\nbTerminalLink: " + String::toAString(int(bTerminalLink_));
	result += "\nbStillCaptureMethod: " + String::toAString(int(bStillCaptureMethod_));
	result += "\nbTriggerSupport: " + String::toAString(int(bTriggerSupport_));
	result += "\nbTriggerUsage: " + String::toAString(int(bTriggerUsage_));
	result += "\nbControlSize: " + String::toAString(int(bControlSize_));

	for (size_t n = 0; n < bmaControls_.size(); ++n)
	{
		const std::vector<uint8_t>& bmaControl = bmaControls_[n];

		result += "\nControl : " + String::toAString(n) + ": " + String::toAStringHex(bmaControl.data(), bmaControl.size());
	}

	return result;
}

bool VSInputHeaderDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VS_INPUT_HEADER)
	{
		return false;
	}

	return bLength_ == 13 + size_t(bNumFormats_) * size_t(bControlSize_);
}

VSUncompressedVideoFormatDescriptor::VSUncompressedVideoFormatDescriptor(const void* buffer, const size_t size)
{
	static_assert(sizeof(VSUncompressedVideoFormatDescriptor) == 27, "Invalid struct size");

	if (size != 27)
	{
		return;
	}

	memcpy(this, buffer, sizeof(VSUncompressedVideoFormatDescriptor));
}

std::string VSUncompressedVideoFormatDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFormatIndex: " + String::toAString(int(bFormatIndex_));
	result += "\nbNumFrameDescriptors: " + String::toAString(int(bNumFrameDescriptors_));
	result += "\nguidFormat: " + guid2string(guidFormat_) + " (" + FrameType::translatePixelFormat(translatePixelFormat(guidFormat_)) + ")";
	result += "\nbBitsPerPixel: " + String::toAString(int(bBitsPerPixel_));
	result += "\nbDefaultFrameIndex: " + String::toAString(int(bDefaultFrameIndex_));
	result += "\nbAspectRatioX: " + String::toAString(int(bAspectRatioX_));
	result += "\nbAspectRatioY: " + String::toAString(int(bAspectRatioY_));
	result += "\nbmInterlaceFlags: " + String::toAString(int(bmInterlaceFlags_));

	if (bmInterlaceFlags_ != 0u)
	{
		result += " (";

		if (bmInterlaceFlags_ & (1u << 0u))
		{
			result += "Interlaced stream or variable, ";
		}

		if (bmInterlaceFlags_ & (1u << 1u))
		{
			result += "1 field per frame, ";
		}
		else
		{
			result += "2 fields per frame, ";
		}

		if (bmInterlaceFlags_ & (1u << 2u))
		{
			result += "Field 1 first, ";
		}

		switch ((bmInterlaceFlags_ >> 4) & 0b11u)
		{
			case 0b00u:
				result += "Field 1 only, ";
				break;

			case 0b01u:
				result += "Field 2 only, ";
				break;

			case 0b10u:
				result += "Regular pattern of fields 1 and 2, ";
				break;

			case 0b11u:
				result += "Random pattern of fields 1 and 2, ";
				break;
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nbCopyProtect: " + String::toAString(int(bCopyProtect_));

	return result;
}

bool VSUncompressedVideoFormatDescriptor::isValid() const
{
	return bLength_ == 27u && bDescriptorType_ == CS_INTERFACE && bDescriptorSubtype_ == VS_FORMAT_UNCOMPRESSED;
}

FrameType::PixelFormat VSUncompressedVideoFormatDescriptor::translatePixelFormat(const uint8_t guidFormat[16])
{
	{
		/*
		*  Y U V 2
		* 59555932-0000-1000-8000-00AA00389B71
		*/
		constexpr std::array<uint8_t, 16> guid_yuv2 = createGUID<0x59555932u, 0x0000u, 0x1000, 0x8000, 0x00AA00389B71u>();

		if (memcmp(guidFormat, guid_yuv2.data(), guid_yuv2.size()) == 0)
		{
			return FrameType::FORMAT_YUYV16;
		}
	}

	{
		/*
		*  N V 1 2
		* 4E563132-0000-1000-8000-00AA00389B71
		*/
		constexpr std::array<uint8_t, 16> guid_nv12 = createGUID<0x4E563132u, 0x0000u, 0x1000, 0x8000, 0x00AA00389B71u>();

		if (memcmp(guidFormat, guid_nv12.data(), guid_nv12.size()) == 0)
		{
			return FrameType::FORMAT_Y_UV12;
		}
	}

	return FrameType::FORMAT_UNDEFINED;
}

VSUncompressedVideoFrameDescriptor::VSUncompressedVideoFrameDescriptor(const uint8_t* buffer, const size_t size)
{
	if (size <= 26)
	{
		return;
	}

	value(0, bLength_, buffer);

	if (size_t(bLength_) != size)
	{
		// the descriptor is invalid

		ocean_assert(!isValid());
		return;
	}

	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bFrameIndex_, buffer);
	value(4, bmCapabilities_, buffer);

	value(5, wWidth_, buffer);
	value(7, wHeight_, buffer);
	value(9, dwMinBitRate_, buffer);
	value(13, dwMaxBitRate_, buffer);
	value(17, dwMaxVideoFrameBufferSize_, buffer);
	value(21, dwDefaultFrameInterval_, buffer);
	value(25, bFrameIntervalType_, buffer);

	if (bFrameIntervalType_ == 0u)
	{
		// continuous Frame Intervals

		if (size != 38)
		{
			invalidate(*this);
			return;
		}

		value(26, dwMinFrameInterval_, buffer);
		value(30, dwMaxFrameInterval_, buffer);
		value(34, dwFrameIntervalStep_, buffer);
	}
	else
	{
		// discrete Frame Intervals

		size_t remainingSize = size - 26;

		if (remainingSize % 4 != 0)
		{
			invalidate(*this);
			return;
		}

		dwFrameInterval_.reserve(remainingSize / 4);

		for (size_t n = 0; n < remainingSize; n += 4)
		{
			uint32_t dwFrameInterval;
			value(26 + n, dwFrameInterval, buffer);

			dwFrameInterval_.push_back(dwFrameInterval);
		}
	}
}

Indices32 VSUncompressedVideoFrameDescriptor::frameIntervals(std::vector<double>* frameRates) const
{
	return VSDescriptor::frameIntervals(*this, frameRates);
}

std::string VSUncompressedVideoFrameDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFrameIndex: " + String::toAString(int(bFrameIndex_));
	result += "\nCapabilities: " + String::toAString(int(bmCapabilities_));

	if (bmCapabilities_ != 0u)
	{
		result += " (";

		if (bmCapabilities_ & (1u << 0u))
		{
			result += "Still image, ";
		}

		if (bmCapabilities_ & (1u << 1u))
		{
			result += "Fixed frame-rate, ";
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nwWidth: " + String::toAString(wWidth_) + "px";
	result += "\nwHeight: " + String::toAString(wHeight_) + "px";
	result += "\ndwMinBitRate: " + String::toAString(dwMinBitRate_ / 1024u) + "Kbps";
	result += "\ndwMaxBitRate: " + String::toAString(dwMaxBitRate_ / 1024u) + "Kbps";
	result += "\ndwMaxVideoFrameBufferSize: " + String::toAString(dwMaxVideoFrameBufferSize_ / 1024) + "KB (deprecated)";
	result += "\ndwDefaultFrameInterval: " + String::toAString(dwDefaultFrameInterval_) + " (" + String::toAString(interval2frequency(dwDefaultFrameInterval_), 1u) + "Hz)";
	result += "\nbFrameIntervalType: " + String::toAString(int(bFrameIntervalType_));

	if (bFrameIntervalType_ == 0u)
	{
		result += "\nContinuous frame intervals mode:";
		result += "\ndwMinFrameInterval: " + String::toAString(dwMinFrameInterval_) + " (" + String::toAString(interval2frequency(dwMinFrameInterval_), 1u) + "Hz)";
		result += "\ndwFrameIntervalStep: " + String::toAString(dwFrameIntervalStep_) + " (" + String::toAString(interval2frequency(dwFrameIntervalStep_), 1u) + "Hz)";
		result += "\ndwMaxFrameInterval: " + String::toAString(dwMaxFrameInterval_) + " (" + String::toAString(interval2frequency(dwMaxFrameInterval_), 1u) + "Hz)";
	}
	else
	{
		result += "\nDiscrete frame intervals mode, with " + String::toAString(dwFrameInterval_.size()) + " intervals:";

		for (size_t n = 0; n < dwFrameInterval_.size(); ++n)
		{
			result += "\ndwFrameInterval " + String::toAString(n) + ": " + String::toAString(interval2frequency(dwFrameInterval_[n]), 1u) + "Hz";
		}
	}

	return result;
}

bool VSUncompressedVideoFrameDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VS_FRAME_UNCOMPRESSED)
	{
		return false;
	}

	if (bFrameIntervalType_ == 0u)
	{
		return bLength_ == 38u;
	}

	return size_t(bLength_) == 26 + dwFrameInterval_.size() * 4;
}

VSFrameBasedVideoFormatDescriptor::VSFrameBasedVideoFormatDescriptor(const void* buffer, const size_t size)
{
	static_assert(sizeof(VSFrameBasedVideoFormatDescriptor) == 28, "Invalid struct size");

	if (size != 28)
	{
		return;
	}

	memcpy(this, buffer, sizeof(VSFrameBasedVideoFormatDescriptor));
}

std::string VSFrameBasedVideoFormatDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFormatIndex: " + String::toAString(int(bFormatIndex_));
	result += "\nbNumFrameDescriptors: " + String::toAString(int(bNumFrameDescriptors_));
	result += "\nguidFormat: " + guid2string(guidFormat_) + " (" + translateEncodingFormat(translateEncodingFormat(guidFormat_)) + ")";
	result += "\nbBitsPerPixel: " + String::toAString(int(bBitsPerPixel_));
	result += "\nbDefaultFrameIndex: " + String::toAString(int(bDefaultFrameIndex_));
	result += "\nbAspectRatioX: " + String::toAString(int(bAspectRatioX_));
	result += "\nbAspectRatioY: " + String::toAString(int(bAspectRatioY_));
	result += "\nbmInterlaceFlags: " + String::toAString(int(bmInterlaceFlags_));

	if (bmInterlaceFlags_ != 0u)
	{
		result += " (";

		if (bmInterlaceFlags_ & (1u << 0u))
		{
			result += "Interlaced stream or variable, ";
		}

		if (bmInterlaceFlags_ & (1u << 1u))
		{
			result += "1 field per frame, ";
		}
		else
		{
			result += "2 fields per frame, ";
		}

		if (bmInterlaceFlags_ & (1u << 2u))
		{
			result += "Field 1 first, ";
		}

		switch ((bmInterlaceFlags_ >> 4) & 0b11u)
		{
			case 0b00u:
				result += "Field 1 only, ";
				break;

			case 0b01u:
				result += "Field 2 only, ";
				break;

			case 0b10u:
				result += "Regular pattern of fields 1 and 2, ";
				break;

			case 0b11u:
				result += "Random pattern of fields 1 and 2, ";
				break;
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nbCopyProtect: " + String::toAString(int(bCopyProtect_));
	result += "\nbVariableSize: " + String::toAString(int(bVariableSize_));

	return result;
}

bool VSFrameBasedVideoFormatDescriptor::isValid() const
{
	return bLength_ == 28u && bDescriptorType_ == CS_INTERFACE && bDescriptorSubtype_ == VS_FORMAT_FRAME_BASED;
}

std::string VSFrameBasedVideoFormatDescriptor::translateEncodingFormat(const EncodingFormat encodingFormat)
{
	switch (encodingFormat)
	{
		case EF_INVALID:
			return "Invalid";

		case EF_H264:
			return "H264";

		case EF_H265:
			return "H265";
	}

	ocean_assert(false && "Invalid encoding format!");
	return "Invalid";
}

VSFrameBasedVideoFormatDescriptor::EncodingFormat VSFrameBasedVideoFormatDescriptor::translateEncodingFormat(const uint8_t guidFormat[16])
{
	/*
	 *  H 2 6 4
	 * 48323634-0000-1000-8000-00AA00389B71
	 */
	constexpr std::array<uint8_t, 16> guid_h264 = createGUID<0x48323634u, 0x0000u, 0x1000, 0x8000, 0x00AA00389B71u>();

	if (memcmp(guidFormat, guid_h264.data(), guid_h264.size()) == 0)
	{
		return EF_H264;
	}

	return EF_INVALID;
}

VSFrameBasedFrameDescriptor::VSFrameBasedFrameDescriptor(const void* buffer, const size_t size)
{
	if (size <= 26)
	{
		return;
	}

	value(0, bLength_, buffer);

	if (size_t(bLength_) != size)
	{
		// the descriptor is invalid

		ocean_assert(!isValid());
		return;
	}

	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bFrameIndex_, buffer);
	value(4, bmCapabilities_, buffer);

	value(5, wWidth_, buffer);
	value(7, wHeight_, buffer);
	value(9, dwMinBitRate_, buffer);
	value(13, dwMaxBitRate_, buffer);
	value(17, dwDefaultFrameInterval_, buffer);
	value(21, bFrameIntervalType_, buffer);
	value(22, dwBytesPerLine_, buffer);

	if (bFrameIntervalType_ == 0u)
	{
		// continuous Frame Intervals

		if (size != 38)
		{
			invalidate(*this);
			return;
		}

		value(26, dwMinFrameInterval_, buffer);
		value(30, dwMaxFrameInterval_, buffer);
		value(34, dwFrameIntervalStep_, buffer);
	}
	else
	{
		// discrete Frame Intervals

		size_t remainingSize = size - 26;

		if (remainingSize % 4 != 0)
		{
			invalidate(*this);
			return;
		}

		dwFrameInterval_.reserve(remainingSize / 4);

		for (size_t n = 0; n < remainingSize; n += 4)
		{
			uint32_t dwFrameInterval;
			value(26 + n, dwFrameInterval, buffer);

			dwFrameInterval_.push_back(dwFrameInterval);
		}
	}
}

Indices32 VSFrameBasedFrameDescriptor::frameIntervals(std::vector<double>* frameRates) const
{
	return VSDescriptor::frameIntervals(*this, frameRates);
}

std::string VSFrameBasedFrameDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFrameIndex: " + String::toAString(int(bFrameIndex_));
	result += "\nCapabilities: " + String::toAString(int(bmCapabilities_));

	if (bmCapabilities_ & (1u << 0u))
	{
		result += " (Still image supported)";
	}

	result += "\nwWidth: " + String::toAString(wWidth_) + "px";
	result += "\nwHeight: " + String::toAString(wHeight_) + "px";
	result += "\ndwMinBitRate: " + String::toAString(dwMinBitRate_ / 1024u) + "Kbps";
	result += "\ndwMaxBitRate: " + String::toAString(dwMaxBitRate_ / 1024u) + "Kbps";
	result += "\ndwDefaultFrameInterval: " + String::toAString(dwDefaultFrameInterval_) + " (" + String::toAString(interval2frequency(dwDefaultFrameInterval_), 1u) + "Hz)";
	result += "\nbFrameIntervalType: " + String::toAString(int(bFrameIntervalType_));
	result += "\ndwBytesPerLine: " + String::toAString(dwBytesPerLine_);

	if (bFrameIntervalType_ == 0u)
	{
		result += "\nContinuous frame intervals mode:";
		result += "\ndwMinFrameInterval: " + String::toAString(interval2frequency(dwMinFrameInterval_), 1u) + "Hz";
		result += "\ndwFrameIntervalStep: " + String::toAString(interval2frequency(dwFrameIntervalStep_), 1u) + "Hz";
		result += "\ndwMaxFrameInterval: " + String::toAString(interval2frequency(dwMaxFrameInterval_), 1u) + "Hz";
	}
	else
	{
		result += "\nDiscrete frame intervals mode, with " + String::toAString(dwFrameInterval_.size()) + " intervals:";

		for (size_t n = 0; n < dwFrameInterval_.size(); ++n)
		{
			result += "\ndwFrameInterval " + String::toAString(n) + ": " + String::toAString(interval2frequency(dwFrameInterval_[n]), 1u) + "Hz";
		}
	}

	return result;
}

bool VSFrameBasedFrameDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VS_FRAME_FRAME_BASED)
	{
		return false;
	}

	if (bFrameIntervalType_ == 0u)
	{
		return bLength_ == 38u;
	}

	return size_t(bLength_) == 26 + dwFrameInterval_.size() * 4;
}

VSColorMatchingDescriptor::VSColorMatchingDescriptor(const void* buffer, const size_t size)
{
	static_assert(sizeof(VSColorMatchingDescriptor) == 6, "Invalid struct size");

	if (size != 6)
	{
		return;
	}

	memcpy(this, buffer, sizeof(VSColorMatchingDescriptor));
}

std::string VSColorMatchingDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	static const std::array<std::string, 6> colorPrimaryStrings =
	{
		"Unspecified",
		"BT.709, sRGB",
		"BT.470-2 (M)",
		"BT.470-2 (B, G)",
		"SMPTE 170M",
		"SMPTE 240M"
	};

	static const std::array<std::string, 8> transferCharacteristicStrings =
	{
		"Unspecified",
		"BT.709",
		"BT.470-2 M",
		"BT.470-2 B, G",
		"SMPTE 170M",
		"SMPTE 240M",
		"Linear (V = Lc)",
		"sRGB"
	};

	static const std::array<std::string, 6> matrixCoefficientStrings =
	{
		"Unspecified",
		"BT. 709",
		"FCC",
		"BT.470-2 B, G",
		"SMPTE 170M (BT.601)",
		"SMPTE 240M"
	};

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "bColorPrimaries: " + String::toAString(int(bColorPrimaries_));

	if (bColorPrimaries_ < colorPrimaryStrings.size())
	{
		result += " (" + colorPrimaryStrings[bColorPrimaries_] + ")";
	}

	result += "\nbTransferCharacteristics: " + String::toAString(int(bTransferCharacteristics_));

	if (bTransferCharacteristics_ < transferCharacteristicStrings.size())
	{
		result += " (" + transferCharacteristicStrings[bTransferCharacteristics_] + ")";
	}

	result += "\nbMatrixCoefficients: " + String::toAString(int(bMatrixCoefficients_));

	if (bMatrixCoefficients_ < matrixCoefficientStrings.size())
	{
		result += " (" + matrixCoefficientStrings[bMatrixCoefficients_] + ")";
	}

	return result;
}

bool VSColorMatchingDescriptor::isValid() const
{
	return bLength_ == 6u && bDescriptorType_ == CS_INTERFACE && bDescriptorSubtype_ == VS_COLORFORMAT;
}

VSMJPEGVideoFormatDescriptor::VSMJPEGVideoFormatDescriptor(const void* buffer, const size_t size)
{
	static_assert(sizeof(VSMJPEGVideoFormatDescriptor) == 11, "Invalid struct size");

	if (size != 11)
	{
		return;
	}

	memcpy(this, buffer, sizeof(VSMJPEGVideoFormatDescriptor));
}

std::string VSMJPEGVideoFormatDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFormatIndex: " + String::toAString(int(bFormatIndex_));
	result += "\nbNumFrameDescriptors: " + String::toAString(int(bNumFrameDescriptors_));
	result += "\nbmFlags: " + String::toAString(int(bmFlags_));

	if (bmFlags_ & (1u << 0u))
	{
		result += " (FixedSizeSamples)";
	}

	result += "\nbDefaultFrameIndex: " + String::toAString(int(bDefaultFrameIndex_));
	result += "\nbAspectRatioX: " + String::toAString(int(bAspectRatioX_));
	result += "\nbAspectRatioY: " + String::toAString(int(bAspectRatioY_));
	result += "\nIbmInterlaceFlags: " + String::toAString(int(bmInterlaceFlags_));

	if (bmInterlaceFlags_ != 0u)
	{
		result += " (";

		if (bmInterlaceFlags_ & (1u << 0u))
		{
			result += "Interlaced stream or variable, ";
		}

		if (bmInterlaceFlags_ & (1u << 1u))
		{
			result += "1 field per frame, ";
		}
		else
		{
			result += "2 fields per frame, ";
		}

		if (bmInterlaceFlags_ & (1u << 2u))
		{
			result += "Field 1 first, ";
		}

		switch ((bmInterlaceFlags_ >> 4) & 0b11u)
		{
			case 0b00u:
				result += "Field 1 only, ";
				break;

			case 0b01u:
				result += "Field 2 only, ";
				break;

			case 0b10u:
				result += "Regular pattern of fields 1 and 2, ";
				break;

			case 0b11u:
				result += "Random pattern of fields 1 and 2, ";
				break;
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nbCopyProtect: " + String::toAString(int(bCopyProtect_));

	return result;
}

bool VSMJPEGVideoFormatDescriptor::isValid() const
{
	return bLength_ == 11u && bDescriptorType_ == CS_INTERFACE && bDescriptorSubtype_ == VS_FORMAT_MJPEG;
}

VSMJPEGVideoFrameDescriptor::VSMJPEGVideoFrameDescriptor(const void* buffer, const size_t size)
{
	if (size <= 26)
	{
		return;
	}

	value(0, bLength_, buffer);

	if (size_t(bLength_) != size)
	{
		// the descriptor is invalid

		ocean_assert(!isValid());
		return;
	}

	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bFrameIndex_, buffer);
	value(4, bmCapabilities_, buffer);

	value(5, wWidth_, buffer);
	value(7, wHeight_, buffer);
	value(9, dwMinBitRate_, buffer);
	value(13, dwMaxBitRate_, buffer);
	value(17, dwMaxVideoFrameBufferSize_, buffer);
	value(21, dwDefaultFrameInterval_, buffer);
	value(22, bFrameIntervalType_, buffer);

	if (bFrameIntervalType_ == 0u)
	{
		// continuous Frame Intervals

		if (size != 38)
		{
			invalidate(*this);
			return;
		}

		value(26, dwMinFrameInterval_, buffer);
		value(30, dwMaxFrameInterval_, buffer);
		value(34, dwFrameIntervalStep_, buffer);
	}
	else
	{
		// discrete Frame Intervals

		size_t remainingSize = size - 26;

		if (remainingSize % 4 != 0)
		{
			invalidate(*this);
			return;
		}

		dwFrameInterval_.reserve(remainingSize / 4);

		for (size_t n = 0; n < remainingSize; n += 4)
		{
			uint32_t dwFrameInterval;
			value(26 + n, dwFrameInterval, buffer);

			dwFrameInterval_.push_back(dwFrameInterval);
		}
	}
}

Indices32 VSMJPEGVideoFrameDescriptor::frameIntervals(std::vector<double>* frameRates) const
{
	return VSDescriptor::frameIntervals(*this, frameRates);
}

std::string VSMJPEGVideoFrameDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbFrameIndex: " + String::toAString(int(bFrameIndex_));
	result += "\nbmCapabilities: " + String::toAString(int(bmCapabilities_));

	if (bmCapabilities_ != 0u)
	{
		result += " (";

		if (bmCapabilities_ & (1u << 0u))
		{
			result += "Still image, ";
		}

		if (bmCapabilities_ & (1u << 1u))
		{
			result += "Fixed frame-rate, ";
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\nwWidth: " + String::toAString(wWidth_) + "px";
	result += "\nwHeight: " + String::toAString(wHeight_) + "px";
	result += "\ndwMinBitRate: " + String::toAString(dwMinBitRate_ / 1024u) + "Kbps";
	result += "\ndwMaxBitRate: " + String::toAString(dwMaxBitRate_ / 1024u) + "Kbps";
	result += "\ndwMaxVideoFrameBufferSize: " + String::toAString(dwMaxVideoFrameBufferSize_ / 1024) + "KB (deprecated)";
	result += "\ndwDefaultFrameInterval: " + String::toAString(interval2frequency(dwDefaultFrameInterval_), 1u) + "Hz";
	result += "\nbFrameIntervalType: " + String::toAString(int(bFrameIntervalType_));

	if (bFrameIntervalType_ == 0u)
	{
		result += "\nContinuous frame intervals mode:";
		result += "\ndwMinFrameInterval: " + String::toAString(interval2frequency(dwMinFrameInterval_), 1u) + "Hz";
		result += "\ndwFrameIntervalStep: " + String::toAString(interval2frequency(dwFrameIntervalStep_), 1u) + "Hz";
		result += "\ndwMaxFrameInterval: " + String::toAString(interval2frequency(dwMaxFrameInterval_), 1u) + "Hz";
	}
	else
	{
		result += "\nDiscrete frame intervals mode, with " + String::toAString(dwFrameInterval_.size()) + " intervals:";

		for (size_t n = 0; n < dwFrameInterval_.size(); ++n)
		{
			result += "\ndwFrameInterval " + String::toAString(n) + ": " + String::toAString(interval2frequency(dwFrameInterval_[n]), 1u) + "Hz";
		}
	}

	return result;
}

bool VSMJPEGVideoFrameDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VS_FRAME_MJPEG)
	{
		return false;
	}

	if (bFrameIntervalType_ == 0u)
	{
		return bLength_ == 38u;
	}

	return size_t(bLength_) == 26 + dwFrameInterval_.size() * 4;
}

}

}

}

}
