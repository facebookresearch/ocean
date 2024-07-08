/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/video/VCDescriptor.h"

#include "ocean/system/usb/Device.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

VCHeaderDescriptor::VCHeaderDescriptor(const void* buffer, const size_t size)
{
	if (size < 12)
	{
		ocean_assert(!isValid());
		return;
	}

	value(0, bLength_, buffer);
	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bcdUVC_, buffer);
	value(5, wTotalLength_, buffer);
	value(7, dwClockFrequency_, buffer);
	value(11, bInCollection_, buffer);

	if (size != 12 + size_t(bInCollection_))
	{
		invalidate(*this);
		return;
	}

	baInterfaceNr_.reserve(bInCollection_);

	for (uint8_t n = 0u; n < bInCollection_; ++n)
	{
		uint8_t baInterfaceNr;
		value(12 + n, baInterfaceNr, buffer);

		baInterfaceNr_.push_back(baInterfaceNr);
	}
}

std::string VCHeaderDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbcdUVC: 0x" + String::toAStringHex(bcdUVC_);
	result += "\nwTotalLength: " + String::toAString(wTotalLength_);
	result += "\ndwClockFrequency: " + String::toAString(dwClockFrequency_) + "Hz";
	result += "\nbInCollection: " + String::toAString(int(bInCollection_)) + ":";

	for (size_t n = 0; n < baInterfaceNr_.size(); ++n)
	{
		result += "\baInterfaceNr " + String::toAString(n) + ": " + String::toAString(int(baInterfaceNr_[n]));
	}

	return result;
}

bool VCHeaderDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VC_HEADER)
	{
		return false;
	}

	return bLength_ == 12 + size_t(bInCollection_);
}

VCInputTerminalDescriptor::VCInputTerminalDescriptor(const void* buffer, const size_t size)
{
	if (size < 8)
	{
		ocean_assert(!isValid());
		return;
	}

	value(0, bLength_, buffer);
	value(1, bDescriptorType_, buffer);
	value(2, bDescriptorSubtype_, buffer);

	value(3, bTerminalID_, buffer);
	value(4, wTerminalType_, buffer);
	value(6, bAssocTerminal_, buffer);
	value(7, iTerminal_, buffer);

	if (wTerminalType_ == ITT_CAMERA && size >= 15)
	{
		value(8, wObjectiveFocalLengthMin_, buffer);
		value(10, wObjectiveFocalLengthMax_, buffer);
		value(12, wOcularFocalLength_, buffer);
		value(14, bControlSize_, buffer);

		if (size < 15 + size_t(bControlSize_))
		{
			ocean_assert(false && "Invalid buffer size!");

			invalidate(*this);
			return;
		}

		bmControls_.reserve(bControlSize_);

		for (uint8_t n = 0u; n < bControlSize_; ++n)
		{
			uint8_t control;
			value(15 + n, control, buffer);
			bmControls_.push_back(control);
		}
	}
}

std::string VCInputTerminalDescriptor::toString(libusb_device_handle* usbDeviceHandle) const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbTerminalID: " + String::toAString(int(bTerminalID_));
	result += "\nwTerminalType: 0x" + String::toAStringHex(int(wTerminalType_));
	result += "\nbAssocTerminal: " + String::toAString(int(bAssocTerminal_));
	result += "\niTerminal: " + String::toAString(int(iTerminal_));

	if (iTerminal_ != 0u && usbDeviceHandle != nullptr)
	{
		result += " (" + Device::stringDescriptor(usbDeviceHandle, iTerminal_) + ")";
	}

	result += "\nwObjectiveFocalLengthMin: " + String::toAString(wObjectiveFocalLengthMin_);
	result += "\nwObjectiveFocalLengthMax: " + String::toAString(wObjectiveFocalLengthMax_);
	result += "\nwOcularFocalLength: " + String::toAString(wOcularFocalLength_);
	result += "\nbControlSize: " + String::toAString(bControlSize_);

	bool hasControls = false;

	for (size_t n = 0; !hasControls && n < bmControls_.size(); ++n)
	{
		hasControls = bmControls_[n] != 0u;
	}

	if (hasControls)
	{
		result += " (";

		if (bmControls_[0] & (1u << 0u))
		{
			result += "Scanning Mode, ";
		}

		if (bmControls_[0] & (1u << 1u))
		{
			result += "Auto-Exposure Mode, ";
		}

		if (bmControls_[0] & (1u << 2u))
		{
			result += "Auto-Exposure Priority, ";
		}

		if (bmControls_[0] & (1u << 3u))
		{
			result += "Exposure Time (Absolute), ";
		}

		if (bmControls_[0] & (1u << 4u))
		{
			result += "Exposure Time (Relative), ";
		}

		if (bmControls_[0] & (1u << 5u))
		{
			result += "Focus (Absolute), ";
		}

		if (bmControls_[0] & (1u << 6u))
		{
			result += "Focus (Relative), ";
		}

		if (bmControls_[0] & (1u << 7u))
		{
			result += "Iris (Absolute), ";
		}

		if (bmControls_.size() >= 2)
		{
			if (bmControls_[1] & (1u << 0u))
			{
				result += "Iris (Relative), ";
			}

			if (bmControls_[1] & (1u << 1u))
			{
				result += "Zoom (Absolute), ";
			}

			if (bmControls_[1] & (1u << 2u))
			{
				result += "Zoom (Relative), ";
			}

			if (bmControls_[1] & (1u << 3u))
			{
				result += "PanTilt (Absolute), ";
			}

			if (bmControls_[1] & (1u << 4u))
			{
				result += "PanTilt (Relative), ";
			}

			if (bmControls_[1] & (1u << 5u))
			{
				result += "Roll (Absolute), ";
			}

			if (bmControls_[1] & (1u << 6u))
			{
				result += "Roll (Relative), ";
			}
		}

		if (bmControls_.size() >= 3)
		{
			if (bmControls_[2] & (1u << 1u))
			{
				result += "Auto Focus, ";
			}
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	return result;
}

bool VCInputTerminalDescriptor::isValid() const
{
	return bLength_ >= 8u && bDescriptorType_ == CS_INTERFACE && bDescriptorSubtype_ == VC_INPUT_TERMINAL;
}

VCProcessingUnitDescriptor::VCProcessingUnitDescriptor(const void* buffer, const size_t size)
{
	if (size < 10)
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

	value(3, bUnitID_, buffer);
	value(4, bSourceID_, buffer);

	value(5, wMaxMultiplier_, buffer);
	value(7, bControlSize_, buffer);

	// instead of checking for size == 10 + bControlSize_, we only check whether the controls fit into the buffer
	// it seems that the last fields 'iProcessing_' or 'bmVideoStandards_' may not be provided

	if (size < 8 + size_t(bControlSize_))
	{
		ocean_assert(false && "Invalid buffer size!");

		invalidate(*this);
		return;
	}

	bmControls_.reserve(bControlSize_);

	for (uint8_t n = 0u; n < bControlSize_; ++n)
	{
		uint8_t control;
		value(8 + n, control, buffer);
		bmControls_.push_back(control);
	}

	if (size < 9 + size_t(bControlSize_))
	{
		return;
	}

	value(8 + bControlSize_, iProcessing_, buffer);

	if (size < 10 + size_t(bControlSize_))
	{
		return;
	}

	value(9 + bControlSize_, bmVideoStandards_, buffer);
}

std::string VCProcessingUnitDescriptor::toString(libusb_device_handle* usbDeviceHandle) const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbUnitID: " + String::toAString(int(bUnitID_));
	result += "\nbSourceID: " + String::toAString(int(bSourceID_));
	result += "\nwMaxMultiplier: " + String::toAString(wMaxMultiplier_);
	result += "\nbControlSize: " + String::toAString(int(bControlSize_));

	result += "\nbmControls: 0x" + String::toAStringHex(bmControls_.data(), bmControls_.size());

	bool hasControls = false;

	for (size_t n = 0; !hasControls && n < bmControls_.size(); ++n)
	{
		hasControls = bmControls_[n] != 0u;
	}

	if (hasControls)
	{
		result += " (";

		if (bmControls_[0] & (1u << 0u))
		{
			result += "Brightness, ";
		}

		if (bmControls_[0] & (1u << 1u))
		{
			result += "Contrast, ";
		}

		if (bmControls_[0] & (1u << 2u))
		{
			result += "Hue, ";
		}

		if (bmControls_[0] & (1u << 3u))
		{
			result += "Saturation, ";
		}

		if (bmControls_[0] & (1u << 4u))
		{
			result += "Sharpness, ";
		}

		if (bmControls_[0] & (1u << 5u))
		{
			result += "Gamma, ";
		}

		if (bmControls_[0] & (1u << 6u))
		{
			result += "White Balance Temperature, ";
		}

		if (bmControls_[0] & (1u << 7u))
		{
			result += "White Balance Component, ";
		}

		if (bmControls_.size() >= 2)
		{
			if (bmControls_[1] & (1u << 0u))
			{
				result += "Backlight Compensation, ";
			}

			if (bmControls_[1] & (1u << 1u))
			{
				result += "Gain, ";
			}

			if (bmControls_[1] & (1u << 2u))
			{
				result += "Power Line Frequency, ";
			}

			if (bmControls_[1] & (1u << 3u))
			{
				result += "Auto Hue, ";
			}

			if (bmControls_[1] & (1u << 4u))
			{
				result += "Auto White Balance Temperature, ";
			}

			if (bmControls_[1] & (1u << 5u))
			{
				result += "Auto White Balance Component, ";
			}

			if (bmControls_[1] & (1u << 6u))
			{
				result += "Digital Multiplier, ";
			}

			if (bmControls_[1] & (1u << 7u))
			{
				result += "Digital Multiplier Limit, ";
			}
		}

		if (bmControls_.size() >= 3)
		{
			if (bmControls_[2] & (1u << 0u))
			{
				result += "Analog Video Standard, ";
			}

			if (bmControls_[2] & (1u << 1u))
			{
				result += "Analog Video Lock Status, ";
			}
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	result += "\niProcessing: " + String::toAString(int(iProcessing_));

	if (iProcessing_ != 0u && usbDeviceHandle != nullptr)
	{
		result += " (" + Device::stringDescriptor(usbDeviceHandle, iProcessing_) + ")";
	}

	result += "\nbmVideoStandards: " + String::toAString(int(bmVideoStandards_));

	if (bmVideoStandards_ != 0u)
	{
		result += " (";

		if (bmControls_[0] & (1u << 0u))
		{
			result += "None, ";
		}

		if (bmControls_[0] & (1u << 1u))
		{
			result += "NTSC – 525/60, ";
		}

		if (bmControls_[0] & (1u << 2u))
		{
			result += "PAL – 625/50, ";
		}

		if (bmControls_[0] & (1u << 3u))
		{
			result += "SECAM – 625/50, ";
		}

		if (bmControls_[0] & (1u << 4u))
		{
			result += "NTSC – 625/50, ";
		}

		if (bmControls_[0] & (1u << 5u))
		{
			result += "PAL – 525/60, ";
		}

		result.pop_back();
		result.pop_back();

		result += ")";
	}

	return result;
}

bool VCProcessingUnitDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VC_PROCESSING_UNIT)
	{
		return false;
	}

	// checking for >= 8 + bmControls_.size(), instead of 10 + bmControls_size()
	// because iProcessing_ or bmVideoStandards_ may not be part of this descriptor
	// e.g., https://community.infineon.com/gfawx74859/attachments/gfawx74859/jpusbsuperspeed/166/1/USB_Video_Example%201.1.pdf

	return size_t(bLength_) >= 8 + bmControls_.size();
}

VCSelectorUnitDescriptor::VCSelectorUnitDescriptor(const void* buffer, const size_t size)
{
	if (size < 6)
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

	value(3, bUnitID_, buffer);
	value(4, bNrInPins_, buffer);

	if (size != 6 + size_t(bNrInPins_))
	{
		ocean_assert(false && "Invalid buffer size!");

		invalidate(*this);
		return;
	}

	baSourceID_.reserve(bNrInPins_);

	for (uint8_t n = 0u; n < bNrInPins_; ++n)
	{
		uint8_t control;
		value(5 + n, control, buffer);
		baSourceID_.push_back(control);
	}

	value(5 + bNrInPins_, iSelector_, buffer);
}

std::string VCSelectorUnitDescriptor::toString(libusb_device_handle* usbDeviceHandle) const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbUnitID: " + String::toAString(int(bUnitID_));
	result += "\nbNrInPins: " + String::toAString(int(bNrInPins_));

	for (size_t n = 0; n < baSourceID_.size(); ++n)
	{
		result += "\nbaSourceID " + String::toAString(n) + ": " + String::toAString(int(baSourceID_[n]));
	}

	result += "\niSelector: " + String::toAString(int(iSelector_));

	if (iSelector_ != 0u && usbDeviceHandle != nullptr)
	{
		result += " (" + Device::stringDescriptor(usbDeviceHandle, iSelector_) + ")";
	}

	return result;
}

bool VCSelectorUnitDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VC_SELECTOR_UNIT)
	{
		return false;
	}

	return size_t(bLength_) == 6 + baSourceID_.size();
}

VCExtensionUnitDescriptor::VCExtensionUnitDescriptor(const void* buffer, const size_t size)
{
	if (size < 24)
	{
		ocean_assert(!isValid());
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

	value(3, bUnitID_, buffer);
	memcpy(guidExtensionCode_, (const uint8_t*)(buffer) + 4, sizeof(uint8_t) * 16);
	value(20, bNumControls_, buffer);
	value(21, bNrInPins_, buffer);

	if (size < 24 + size_t(bNrInPins_))
	{
		ocean_assert(false && "Invalid buffer size!");

		invalidate(*this);
		return;
	}

	baSourceID_.reserve(bNrInPins_);

	for (uint8_t n = 0u; n < bNrInPins_; ++n)
	{
		uint8_t baSourceId;
		value(22 + n, baSourceId, buffer);
		baSourceID_.push_back(baSourceId);
	}

	value(22 + bNrInPins_, bControlSize_, buffer);

	if (size < 24 + size_t(bNrInPins_) + size_t(bControlSize_))
	{
		ocean_assert(false && "Invalid buffer size!");

		invalidate(*this);
		return;
	}

	bmControls_.reserve(bControlSize_);

	for (uint8_t n = 0u; n < bControlSize_; ++n)
	{
		uint8_t bmControls;
		value(23 + bNrInPins_ + n, bmControls, buffer);
		bmControls_.push_back(bmControls);
	}

	value(23 + bNrInPins_ + bControlSize_, iExtension_, buffer);
}

std::string VCExtensionUnitDescriptor::toString(libusb_device_handle* usbDeviceHandle) const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "bLength: " + String::toAString(int(bLength_));
	result += "\nbDescriptorType: " + String::toAString(int(bDescriptorType_));
	result += "\nbDescriptorSubtype: " + String::toAString(int(bDescriptorSubtype_));

	result += "\nbUnitID: " + String::toAString(int(bUnitID_));
	result += "\nguidExtensionCode: " + guid2string(guidExtensionCode_);
	result += "\nbNumControls: " + String::toAString(int(bNumControls_));
	result += "\nbNrInPins: " + String::toAString(int(bNrInPins_));

	for (size_t n = 0; n < baSourceID_.size(); ++n)
	{
		result += "\nbaSourceID " + String::toAString(n) + ": " + String::toAString(int(baSourceID_[n]));
	}

	result += "\nbControlSize: " + String::toAString(int(bControlSize_));

	for (size_t n = 0; n < bmControls_.size(); ++n)
	{
		result += "\nbmControls " + String::toAString(n) + ": 0x" + String::toAStringHex(bmControls_[n]);
	}

	result += "\niExtension: " + String::toAString(int(iExtension_));

	if (iExtension_ != 0u && usbDeviceHandle != nullptr)
	{
		result += " (" + Device::stringDescriptor(usbDeviceHandle, iExtension_) + ")";
	}

	return result;
}

bool VCExtensionUnitDescriptor::isValid() const
{
	if (bDescriptorType_ != CS_INTERFACE || bDescriptorSubtype_ != VC_EXTENSION_UNIT)
	{
		return false;
	}

	return bLength_ == 24 + size_t(bNrInPins_) + size_t(bControlSize_);
}

}

}

}

}
