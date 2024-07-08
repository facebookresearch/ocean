/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/Device.h"
#include "ocean/system/usb/Utilities.h"

namespace Ocean
{

namespace System
{

namespace USB
{

Device::Device(SharedContext context, libusb_device* usbDevice) :
	context_(context),
	usbDevice_(usbDevice)
{
	if (usbDevice_ != nullptr)
	{
		libusb_ref_device(usbDevice_);

		ocean_assert(isValid());
	}
	else
	{
		ocean_assert(!isValid());
	}
}

Device::Device(SharedContext context, libusb_device_handle* usbDeviceHandle) :
	Device(std::move(context), libusb_get_device(usbDeviceHandle), usbDeviceHandle)
{
	// nothing to do here
}

Device::Device(SharedContext context, libusb_device* usbDevice, libusb_device_handle* usbDeviceHandle)
{
	initialize(std::move(context), usbDevice, usbDeviceHandle);
}

bool Device::initialize(SharedContext context, libusb_device* usbDevice, libusb_device_handle* usbDeviceHandle)
{
	ocean_assert(usbDevice != nullptr && usbDeviceHandle != nullptr);

	const ScopedLock scopedLock(lock_);

	if (usbDevice_ != nullptr || usbDeviceHandle_ != nullptr)
	{
		ocean_assert(false && "Already initialized");
		return false;
	}

	if (usbDevice == nullptr || usbDeviceHandle == nullptr)
	{
		return false;
	}
	context_ = std::move(context);

	usbDevice_ = usbDevice;
	usbDeviceHandle_ = usbDeviceHandle;

	libusb_ref_device(usbDevice_);

	const int descriptorResult = libusb_get_device_descriptor(usbDevice, &usbDeviceDescriptor_);

	if (descriptorResult != LIBUSB_SUCCESS)
	{
		Log::error() << "Failed to determine device description for device, error " << libusb_error_name(descriptorResult);
	}

	return true;
}

Device::Device(SharedContext context, const int64_t systemDeviceHandle)
{
	static_assert(sizeof(systemDeviceHandle) >= sizeof(intptr_t), "Invalid data type");
	static_assert(std::is_signed<intptr_t>::value, "Invalid data type");

	libusb_device_handle* usbDeviceHandle = nullptr;
	const int wrapResult = libusb_wrap_sys_device(context ? context->usbContext() : nullptr, (intptr_t)(systemDeviceHandle), &usbDeviceHandle);

	if (wrapResult == LIBUSB_SUCCESS)
	{
		ocean_assert(usbDeviceHandle != nullptr);

		if (initialize(std::move(context), libusb_get_device(usbDeviceHandle), usbDeviceHandle))
		{
			systemDeviceHandle_ = systemDeviceHandle;
		}
	}
	else
	{
		Log::error() << "Failed to wrap device, error: " << libusb_strerror(wrapResult);
	}
}

Device::~Device()
{
	release();
}

uint16_t Device::vendorId() const
{
	const ScopedLock scopedLock(lock_);

	return usbDeviceDescriptor_.idVendor;
}

uint16_t Device::productId() const
{
	const ScopedLock scopedLock(lock_);

	return usbDeviceDescriptor_.idProduct;
}

std::string Device::name() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());
	if (!isValid())
	{
		return std::string();
	}

	const uint8_t busNumber = libusb_get_bus_number(usbDevice_);
	const uint8_t deviceAddress = libusb_get_device_address(usbDevice_);

	return "/dev/bus/usb/" + String::toAString((unsigned int)(busNumber), 3u) + "/" + String::toAString((unsigned int)(deviceAddress), 3u);
}

std::string Device::productName() const
{
	const ScopedLock scopedLock(lock_);

	std::string result;

	if (usbDeviceHandle_ != nullptr)
	{
		result = stringDescriptor(usbDeviceHandle_, usbDeviceDescriptor_.iProduct);
	}

	if (result.empty())
	{
		result = Utilities::productName(usbDeviceDescriptor_.idVendor, usbDeviceDescriptor_.idProduct);
	}

	return result;
}

std::string Device::manufacturerName() const
{
	const ScopedLock scopedLock(lock_);

	std::string result;

	if (usbDeviceHandle_ != nullptr)
	{
		result = stringDescriptor(usbDeviceHandle_, usbDeviceDescriptor_.iManufacturer);
	}

	if (result.empty())
	{
		result = Utilities::vendorName(usbDeviceDescriptor_.idVendor);
	}

	return result;
}

std::string Device::serialNumber() const
{
	const ScopedLock scopedLock(lock_);

	if (usbDeviceHandle_ != nullptr)
	{
		return stringDescriptor(usbDeviceHandle_, usbDeviceDescriptor_.iSerialNumber);
	}

	return std::string();
}

bool Device::open()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());
	if (!isValid())
	{
		return false;
	}

	if (isOpen())
	{
		ocean_assert(false && "The device is already open!");
		return true;
	}

	ocean_assert(usbDeviceHandle_ == nullptr);

	const int openResult = libusb_open(usbDevice_, &usbDeviceHandle_);

	if (openResult != LIBUSB_SUCCESS)
	{
		Log::error() << "Failed to open device, error " << libusb_error_name(openResult);
		return false;
	}

	ocean_assert(usbDeviceHandle_ != nullptr);
	return true;
}

bool Device::close()
{
	const ScopedLock scopedLock(lock_);

	if (!isValid() || !isOpen())
	{
		return true;
	}

	ocean_assert(detachedInterfaceUsageMap_.empty());
	ocean_assert(claimedInterfaceUsageMap_.empty());

	ocean_assert(usbDeviceHandle_ != nullptr);
	libusb_close(usbDeviceHandle_);
	usbDeviceHandle_ = nullptr;

	return true;
}

Device::ScopedSubscription Device::detachKernelDriver(const int interfaceIndex, bool* driverWasNotActive)
{
	ocean_assert(interfaceIndex >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());
	if (!isValid())
	{
		return ScopedSubscription();
	}

	UsageMap::iterator iUsage = detachedInterfaceUsageMap_.find(interfaceIndex);

	if (iUsage == detachedInterfaceUsageMap_.end())
	{
		const int detachResult = libusb_detach_kernel_driver(usbDeviceHandle_, interfaceIndex);

		if (detachResult != 0)
		{
			bool showErrorMessage = true;

			if (driverWasNotActive != nullptr)
			{
				*driverWasNotActive = detachResult == LIBUSB_ERROR_NOT_FOUND;

				if (detachResult == LIBUSB_ERROR_NOT_FOUND)
				{
					showErrorMessage = false;
				}
			}

			if (showErrorMessage)
			{
				Log::error() << "Failed to detach kernel driver for interface " << interfaceIndex << ", error: " << libusb_error_name(detachResult);
			}

			return ScopedSubscription();
		}

		iUsage = detachedInterfaceUsageMap_.emplace(interfaceIndex, 0u).first;
	}

	ocean_assert(iUsage != detachedInterfaceUsageMap_.end());

	++iUsage->second;

	return ScopedSubscription(interfaceIndex, std::bind(&Device::reattachKernelDriver, this, std::placeholders::_1));
}

Device::ScopedSubscription Device::claimInterface(const int interfaceIndex)
{
	ocean_assert(interfaceIndex >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());
	if (!isValid())
	{
		return ScopedSubscription();
	}

	UsageMap::iterator iUsage = claimedInterfaceUsageMap_.find(interfaceIndex);

	if (iUsage == claimedInterfaceUsageMap_.end())
	{
		const int claimResult = libusb_claim_interface(usbDeviceHandle_, interfaceIndex);

		if (claimResult != 0)
		{
			Log::error() << "Failed to claim interface " << interfaceIndex << ", error: " << libusb_error_name(claimResult);

			if (claimResult == LIBUSB_ERROR_BUSY)
			{
				Log::debug() << "Need to detach kernel driver for interface before claiming it?";
			}

			return ScopedSubscription();
		}

		iUsage = claimedInterfaceUsageMap_.emplace(interfaceIndex, 0u).first;
	}

	ocean_assert(iUsage != claimedInterfaceUsageMap_.end());

	++iUsage->second;

	return ScopedSubscription(interfaceIndex, std::bind(&Device::releaseInterface, this, std::placeholders::_1));
}

void Device::release()
{
	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_DEBUG
	if (!claimedInterfaceUsageMap_.empty())
	{
		Log::debug() << "The device has " << claimedInterfaceUsageMap_.size() << " remaining claimed interfaces";
	}

	if (!detachedInterfaceUsageMap_.empty())
	{
		Log::debug() << "The device has " << detachedInterfaceUsageMap_.size() << " remaining detached kernel drivers";
	}
#endif // OCEAN_DBUG

	ocean_assert(detachedInterfaceUsageMap_.empty());
	ocean_assert(claimedInterfaceUsageMap_.empty());

	close();

	ocean_assert(usbDeviceHandle_ == nullptr);

	if (usbDevice_ != nullptr)
	{
		libusb_unref_device(usbDevice_);
	}

	systemDeviceHandle_ = 0;
	usbDevice_ = nullptr;
}

Device& Device::operator=(Device&& device)
{
	if (this != &device)
	{
		release();

		context_ = std::move(device.context_);

		systemDeviceHandle_ = device.systemDeviceHandle_;
		usbDeviceHandle_ = device.usbDeviceHandle_;
		usbDevice_ = device.usbDevice_;

		detachedInterfaceUsageMap_.swap(device.detachedInterfaceUsageMap_);
		claimedInterfaceUsageMap_.swap(device.claimedInterfaceUsageMap_);

		device.systemDeviceHandle_ = 0;
		device.usbDeviceHandle_ = nullptr;
		device.usbDevice_ = nullptr;
	}

	return *this;
}

std::string Device::stringDescriptor(libusb_device_handle* usbDeviceHandle, const uint8_t index)
{
	constexpr int maxBufferSize = 256;
	unsigned char buffer[maxBufferSize + 1] = {};

	const int length = libusb_get_string_descriptor_ascii(usbDeviceHandle, index, buffer, maxBufferSize);

	if (length < 0)
	{
		Log::error() << "Failed to determine device description for device, error: " << libusb_strerror(length);
		return std::string();
	}

	if (length == 0)
	{
		std::string();
	}

	return std::string((const char*)(buffer), size_t(length));
}

void Device::reattachKernelDriver(const int interfaceIndex)
{
	ocean_assert(interfaceIndex >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	UsageMap::iterator iUsage = detachedInterfaceUsageMap_.find(interfaceIndex);
	ocean_assert(iUsage != detachedInterfaceUsageMap_.end());

	if (iUsage != detachedInterfaceUsageMap_.end())
	{
		ocean_assert(iUsage->second >= 1u);

		if (--iUsage->second == 0u)
		{
			detachedInterfaceUsageMap_.erase(iUsage);

			const int attachResult = libusb_attach_kernel_driver(usbDeviceHandle_, interfaceIndex);

			if (attachResult == 0)
			{
				Log::debug() << "Re-attached kernel driver for interface " << interfaceIndex;
			}
			else
			{
				Log::error() << "Failed to re-attach kernel driver for interface " << interfaceIndex << ", error: " << libusb_error_name(attachResult);
			}
		}
	}
}

void Device::releaseInterface(const int interfaceIndex)
{
	ocean_assert(interfaceIndex >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	Log::debug() << "releaseInterface(const int interfaceIndex): " << int(interfaceIndex);

	UsageMap::iterator iUsage = claimedInterfaceUsageMap_.find(interfaceIndex);
	ocean_assert(iUsage != claimedInterfaceUsageMap_.end());

	if (iUsage != claimedInterfaceUsageMap_.end())
	{
		ocean_assert(iUsage->second >= 1u);

		if (--iUsage->second == 0u)
		{
			claimedInterfaceUsageMap_.erase(iUsage);

			const int releaseResult = libusb_release_interface(usbDeviceHandle_, interfaceIndex);

			if (releaseResult == 0)
			{
				Log::debug() << "Released claimed interface " << interfaceIndex;
			}
			else
			{
				Log::error() << "Failed to re-attach kernel driver for interface " << interfaceIndex << ", error " << libusb_error_name(releaseResult);
			}
		}
	}
}

bool Device::extractPayload(struct libusb_transfer& usbTransfer, BufferPointers& bufferPointers)
{
	ocean_assert(usbTransfer.status == LIBUSB_TRANSFER_COMPLETED);

	ocean_assert(bufferPointers.empty());

	bool result = true;

	if (usbTransfer.num_iso_packets == 0)
	{
		// bulk mode (for large data, with error correction, but not real-time)

		ocean_assert(usbTransfer.buffer != nullptr && usbTransfer.actual_length > 0u);

		if (usbTransfer.buffer != nullptr && usbTransfer.actual_length > 0u)
		{
			bufferPointers.emplace_back((const void*)(usbTransfer.buffer), size_t(usbTransfer.actual_length));
		}
		else
		{
			result = false;
		}
	}
	else
	{
		// isochronous mode (for real-time data, no error correction)

		for (int packetIndex = 0; packetIndex < usbTransfer.num_iso_packets; ++packetIndex)
		{
			const libusb_iso_packet_descriptor& isoPacketDescriptor = usbTransfer.iso_packet_desc[packetIndex];

			if (isoPacketDescriptor.status == LIBUSB_TRANSFER_COMPLETED)
			{
				// we can ignore empty packets (they are intended)

				if (isoPacketDescriptor.actual_length >= 1u)
				{
					const unsigned char* packetBuffer = libusb_get_iso_packet_buffer_simple(&usbTransfer, (unsigned int)(packetIndex));

					ocean_assert(packetBuffer != nullptr);

					if (packetBuffer != nullptr)
					{
						bufferPointers.emplace_back((const void*)(packetBuffer), size_t(isoPacketDescriptor.actual_length));
					}
				}
			}
			else
			{
				Log::debug() << "Iso packet " << packetIndex << " is not completed (skipping), status: " << libusb_error_name(isoPacketDescriptor.status);
				result = false;
			}
		}
	}

	return result;
}

int Device::determineIsochronousTransferLayout(libusb_context* usbContext, const libusb_interface& interface, const uint8_t endpointAddress, const uint32_t maxVideoFrameSize, const uint32_t maxPayloadTransferSize, size_t& transferSize, size_t& packetsPerTransfer, size_t& bytesPerPacket)
{
	ocean_assert(maxVideoFrameSize != 0u);
	ocean_assert(maxPayloadTransferSize != 0u);

	transferSize = 0;

	for (int altsettingIndex = 0; altsettingIndex < interface.num_altsetting; ++altsettingIndex)
	{
		const libusb_interface_descriptor& altSetting = interface.altsetting[altsettingIndex];

		uint32_t endpointPacketSize = 0u;

		for (uint8_t endpointIndex = 0u; endpointIndex < altSetting.bNumEndpoints; ++endpointIndex)
		{
			const libusb_endpoint_descriptor& endpointDescriptor = altSetting.endpoint[endpointIndex];

			libusb_ss_endpoint_companion_descriptor* endpointCompanionDescriptor = nullptr;
			if (libusb_get_ss_endpoint_companion_descriptor(usbContext, &endpointDescriptor, &endpointCompanionDescriptor) == LIBUSB_SUCCESS)
			{
				ocean_assert(endpointCompanionDescriptor != nullptr);

				endpointPacketSize = uint32_t(endpointCompanionDescriptor->wBytesPerInterval);
				libusb_free_ss_endpoint_companion_descriptor(endpointCompanionDescriptor);
			}
			else
			{
				if (endpointDescriptor.bEndpointAddress == endpointAddress)
				{
					const uint16_t wMaxPacketSize = endpointDescriptor.wMaxPacketSize;
					ocean_assert((wMaxPacketSize & 0xE000u) == 0u); // the upper 3 bits should always be zero, otherwise we may have USB 3.0

					const uint16_t sizePerTransaction = wMaxPacketSize & 0x7FFu; // the lower 11 bits provide the actual size of the transaction

					const uint16_t additionalTransactions = (wMaxPacketSize >> 11u) & 0b11u; // the next two bytes give the number of additional transactions
					ocean_assert(additionalTransactions <= 2u);

					const uint16_t overallTransactions = additionalTransactions + 1u;

					endpointPacketSize = uint32_t(sizePerTransaction) * uint32_t(overallTransactions);
					break;
				}
			}
		}

		if (endpointPacketSize >= maxPayloadTransferSize)
		{
			// the endpoint's packet size is large enough

			constexpr size_t maximalPacketsPerTransfer = 32; // ensure that we don't use too may packets to avoid dropped packets

			ocean_assert(endpointPacketSize >= 1u);
			const size_t necessaryPacketsPerTransfer = (maxVideoFrameSize + (endpointPacketSize - 1u)) / endpointPacketSize;
			packetsPerTransfer = std::min(necessaryPacketsPerTransfer, maximalPacketsPerTransfer);

			transferSize = size_t(endpointPacketSize) * packetsPerTransfer;

			bytesPerPacket = size_t(endpointPacketSize);

			return altsettingIndex;
		}
	}

	// no endpoint has a packet size large enough for the transfer

	return -1;
}

}

}

}
