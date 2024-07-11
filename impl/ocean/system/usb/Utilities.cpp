/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/Utilities.h"

namespace Ocean
{

namespace System
{

namespace USB
{


std::string Utilities::vendorName(const uint16_t vendorId)
{
	const VendorMap& vendorMap_ = vendorMap();

	VendorMap::const_iterator iVendor = vendorMap_.find(vendorId);

	if (iVendor == vendorMap_.cend())
	{
		return std::string();
	}

	return iVendor->second.vendorName_;
}

std::string Utilities::productName(const uint16_t vendorId, const uint16_t productId)
{
	const VendorMap& vendorMap_ = vendorMap();

	VendorMap::const_iterator iVendor = vendorMap_.find(vendorId);

	if (iVendor == vendorMap_.cend())
	{
		return std::string();
	}

	const ProductMap& productMap = iVendor->second.productMap_;

	ProductMap::const_iterator iProduct = productMap.find(productId);

	if (iProduct == productMap.cend())
	{
		return std::string();
	}

	return iProduct->second.deviceName_;
}

Utilities::DeviceCategory Utilities::deviceCategory(const uint16_t vendorId, const uint16_t productId)
{
	const VendorMap& vendorMap_ = vendorMap();

	VendorMap::const_iterator iVendor = vendorMap_.find(vendorId);

	if (iVendor == vendorMap_.cend())
	{
		return DC_UNKNOWN;
	}

	const ProductMap& productMap = iVendor->second.productMap_;

	ProductMap::const_iterator iProduct = productMap.find(productId);

	if (iProduct == productMap.cend())
	{
		return DC_UNKNOWN;
	}

	return iProduct->second.deviceCategory_;
}

const Utilities::VendorMap& Utilities::vendorMap()
{
	static const VendorMap vendorMap =
	{
		{
			0x0291A,
			{
				"Anker Innovations Limited",
				{
					{0x3369, {"Anker PowerConf C200", DC_CAMERA}},
				}
			}
		},

		{
			0x0328F,
			{
				"DSA",
				{
					{0x0072, {"Groov-e USB Camera", DC_CAMERA}}
				}
			}
		},

		{
			0x32E4,
			{
				"Ailipu Technology Co., Ltd.",
				{
					{0x9230, {"HD USB Camera", DC_CAMERA}},
					{0x9750, {"3D USB Camera", DC_CAMERA}} // ELP-USB960P2CAM-V90
				}
			}
		},

		{
			0x046D,
			{
				"Logitech, Inc.",
				{
					{0x0825, {"C270 HD WEBCAM", DC_CAMERA}},
					{0x082D, {"HD Pro Webcam C920", DC_CAMERA}},
					{0x0892, {"C920 HD Pro Webcam", DC_CAMERA}},
					{0x08E7, {"Logi Webcam C920e", DC_CAMERA}},
					{0x0945, {"BRIO 301", DC_CAMERA}}
				}
			}
		},

		{
			0x0C45,
			{
				"Sonix Technology Co., Ltd.",
				{
					{0x636B, {"USB 2.0 Camera", DC_CAMERA}}
				}
			}
		}
	};

	return vendorMap;
}

bool Utilities::doesDeviceContainCategory(const uint16_t vendorId, const uint16_t productId, const uint8_t deviceClass, const DeviceCategory deviceCategory)
{
	ocean_assert(deviceCategory != DC_UNKNOWN);

	if (deviceCategory == DC_UNKNOWN)
	{
		return false;
	}

	const DeviceCategory knownDeviceType = Utilities::deviceCategory(vendorId, productId);

	if ((knownDeviceType & deviceCategory) == deviceCategory)
	{
		return true;
	}

	switch (deviceCategory)
	{
		case DC_UNKNOWN:
			ocean_assert(false && "This should never happen!");
			break;

		case DC_CAMERA:
		{
			// Still Imaging, or Video
			if (deviceClass == LIBUSB_CLASS_IMAGE || deviceClass == LIBUSB_CLASS_VIDEO)
			{
				return true;
			}

			break;
		}
	}

	return false;
}

}

}

}
