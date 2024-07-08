/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_UTILITIES_H
#define META_OCEAN_SYSTEM_USB_UTILITIES_H

#include "ocean/system/usb/USB.h"

namespace Ocean
{

namespace System
{

namespace USB
{

/**
 * This class implements utility functions for USB devices.
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_EXPORT Utilities
{
	public:

		/**
		 * Definition of individual device categories.
		 */
		enum DeviceCategory : uint32_t
		{
			/// An unknown device category.
			DC_UNKNOWN = 0u,
			/// A device which contains a camera in some way.
			DC_CAMERA = 1u << 0u
		};

	protected:

		/**
		 * This class holds the relevant information of a USB product.
		 */
		class Product
		{
			public:

				/**
				 * Default constructor.
				 */
				Product() = default;

				/**
				 * Creates a new product object.
				 * @param deviceName The name of the product, must be valid
				 * @param deviceCategory The category of the product, must be valid
				 */
				inline Product(std::string&& deviceName, const DeviceCategory deviceCategory);

			public:

				/// The name of the product.
				std::string deviceName_;

				/// The category of the product.
				DeviceCategory deviceCategory_ = DC_UNKNOWN;
		};

		/**
		 * Definition of an unordered map mapping product ids to products.
		 */
		using ProductMap = std::unordered_map<uint16_t, Product>;

		/**
		 * This class holds several products of a vendor.
		 */
		class Vendor
		{
			public:

				/**
				 * Default constructor.
				 */
				Vendor() = default;

				/**
				 * Creates a new vendor object.
				 * @param vendorName The name of the vendor, must be valid
				 * @param productMap The map mapping the ids of products (the vendor's product ids) to product objects, must be valid
				 */
				inline Vendor(std::string&& vendorName, ProductMap&& productMap);

			public:

				/// The name of the vendor.
				std::string vendorName_;

				/// The map with all known products of the vendor.
				ProductMap productMap_;
		};

		/**
		 * Definition of an unordered map mapping vendor ids to vendor objects (a lookup table for USB devices).
		 */
		using VendorMap = std::unordered_map<uint16_t, Vendor>;

	public:

		/**
		 * Lookups the name of a vendor id (if known).
		 * @return vendorId The id of the vendor, must be valid
		 * @return The name of the vendor, empty if the id is not stored in the lookup table
		 */
		static std::string vendorName(const uint16_t vendorId);

		/**
		 * Lookups the name of a product id (if known).
		 * @param vendorId The id of the vendor, must be valid
		 * @param productId The id of the product, must be valid
		 * @return The name of the product, empty if not stored in the lookup table
		 */
		static std::string productName(const uint16_t vendorId, const uint16_t productId);

		/**
		 * Lookups the device category of a device defined by it's vendor and product id.
		 * @param vendorId The id of the vendor, must be valid
		 * @param productId The id of the product, must be valid
		 * @return The device category of the product, DC_UNKNOWN if not stored in the lookup table
		 */
		static DeviceCategory deviceCategory(const uint16_t vendorId, const uint16_t productId);

		/**
		 * Checks whether a device contains a specific category.
		 * In case the vendor id or product id is not stored in the lookup table, the device class is used as a backup.
		 * @param vendorId The id of the vendor, must be valid
		 * @param productId The id of the product, must be valid
		 * @param deviceClass The class of the device, must be valid
		 * @param deviceCategory The category to be checked, may be a combination of several categories
		 * @return True, if so
		 */
		static bool doesDeviceContainCategory(const uint16_t vendorId, const uint16_t productId, const uint8_t deviceClass, const DeviceCategory deviceCategory);

		/**
		 * Checks whether a device contains a specific category.
		 * In case the vendor id or product id is not stored in the lookup table, the device class is used as a backup.
		 * @param deviceDescriptor The libusb device descriptor of the device, must be valid
		 * @param deviceCategory The category to be checked, may be a combination of several categories
		 * @return True, if so
		 */
		static inline bool doesDeviceContainCategory(const libusb_device_descriptor& deviceDescriptor, const DeviceCategory deviceCategory);

	protected:

		/**
		 * Returns the lookup table with all known vendors.
		 * @return The vendor lookup table
		 */
		static const VendorMap& vendorMap();
};

inline Utilities::Product::Product(std::string&& deviceName, const DeviceCategory deviceCategory) :
	deviceName_(std::move(deviceName)),
	deviceCategory_(deviceCategory)
{
	// nothing to do here
}

inline Utilities::Vendor::Vendor(std::string&& vendorName, ProductMap&& productMap) :
	vendorName_(std::move(vendorName)),
	productMap_(std::move(productMap))
{
	// nothing to do here
}

inline bool Utilities::doesDeviceContainCategory(const libusb_device_descriptor& deviceDescriptor, const DeviceCategory deviceCategory)
{
	return doesDeviceContainCategory(deviceDescriptor.idVendor, deviceDescriptor.idProduct, deviceDescriptor.bDeviceClass, deviceCategory);
}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_UTILITIES_H
