/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_DEVICE_H
#define META_OCEAN_SYSTEM_USB_DEVICE_H

#include "ocean/system/usb/USB.h"
#include "ocean/system/usb/Context.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ScopedSubscription.h"

namespace Ocean
{

namespace System
{

namespace USB
{

// Forward declaration.
class Device;

/**
 * Definition of a shared pointer holding a device.
 * @see Device.
 * @ingroup systemusb
 */
using SharedDevice = std::shared_ptr<Device>;

/**
 * Definition of a vector holding SharedDevice objects.
 * @see SharedDevice.
 * @ingroup systemusb
 */
using SharedDevices = std::vector<SharedDevice>;

/**
 * This class wraps a libusb device.
 * The class holds a reference to the libusb device as long as the object exists.
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_EXPORT Device
{
	public:

		/**
		 * Definition of a scoped subscription object.
		 */
		using ScopedSubscription = ScopedSubscriptionT<int, Device>;

	protected:

		/**
		 * Definition of an unordered map mapping interface indices to usages.
		 */
		using UsageMap = std::unordered_map<int, unsigned int>;

		/**
		 * Definition of a pair combining a pointer to a buffer and the size of this buffer.
		 */
		using BufferPointer = std::pair<const void*, size_t>;

		/**
		 * Definition of a vector holding buffer pointers.
		 */
		using BufferPointers = std::vector<BufferPointer>;

	public:

		/**
		 * Default constructor creating an invalid device.
		 */
		Device() = default;

		/**
		 * Move constructor.
		 * @param device The device to be moved
		 */
		inline Device(Device&& device);

		/**
		 * Creates a new device object based on a given (not yet opened) libusb device.
		 * @param context The USB context to be used, invalid to use the default context
		 * @param usbDevice The libusb device to be wrapped, the device is not yet open (otherwise a handle would exist), must be valid
		 */
		Device(SharedContext context, libusb_device* usbDevice);

		/**
		 * Creates a new device object based on a given libusb device handle (which means that the devices is already opened).
		 * @param context The USB context to be used, invalid to use the default context
		 * @param usbDeviceHandle The handle of the device, must be valid
		 */
		Device(SharedContext context, libusb_device_handle* usbDeviceHandle);

		/**
		 * Creates a new device object based on a given (already opened) libusb device and its device handle.
		 * @param context The USB context to be used, invalid to use the default context
		 * @param usbDevice The libusb device to be wrapped, the device has been opened already (because a device handle exists), must be valid
		 * @param usbDeviceHandle The handle of the device, must be valid
		 */
		Device(SharedContext context, libusb_device* usbDevice, libusb_device_handle* usbDeviceHandle);

		/**
		 * Creates a new device object (for an already opened device) based on a given system handle.
		 * On some platforms like e.g., Android, the device must be opened on the Java side (resulting in a file descriptor), this file descriptor can then be used as a system handle.
		 * @param context The USB context to be used, invalid to use the default context
		 * @param systemDeviceHandle The system specific handle to the device (e.g., a file descriptor received from Java on Android platforms), must be valid
		 */
		Device(SharedContext context, const int64_t systemDeviceHandle);

		/**
		 * Destructs the device and releases all resources.
		 */
		virtual ~Device();

		/**
		 * Returns the vendor id of the device.
		 * @return The device's vendor id
		 */
		uint16_t vendorId() const;

		/**
		 * Returns the product id of the device.
		 * @return The device's product id
		 */
		uint16_t productId() const;

		/**
		 * Returns the name of the device (not the product name).
		 * @return The device's name
		 */
		std::string name() const;

		/**
		 * Returns the product name of the device.
		 * The device needs to be open before the product name can be queried.<br>
		 * In case the name of the product is not available, a lookup table is used based on the vendor id and product id.
		 * @return The device's product name, empty if not available
		 * @see isOpen().
		 */
		std::string productName() const;

		/**
		 * Returns the manufacturer name of this device.
		 * The device needs to be open before the manufacturer name can be queried.<br>
		 * In case the name of the manufacturer is not available, a lookup table is used based on the vendor id.
		 * @return The device's manufacturer name, empty if not available
		 * @see isOpen().
		 */
		std::string manufacturerName() const;

		/**
		 * Returns the serial number of this device.
		 * The device needs to be open before the serial number can be queried.
		 * @return The device's serial number, empty if not available
		 * @see isOpen().
		 */
		std::string serialNumber() const;

		/**
		 * Opens the device.
		 * @return True, if succeeded
		 * @see isOpen().
		 */
		bool open();

		/**
		 * Closes an opened device.
		 * In case the device was opened via a system device handle (e.g., a file descriptor on Android platforms, calling this function does have no effect).
		 * @return True, if succeeded
		 * @see isOpen().
		 */
		bool close();

		/**
		 * Detaches the kernel driver for the device.
		 * @param interfaceIndex The index of the interface for which the kernel driver will be detached
		 * @param driverWasNotActive Optional resulting information why the resulting subscription object may be invalid; True, in case no kernel driver was active (may indicate that there was no need to detach the driver)
		 * @return The scoped subscription object which will keep the driver detached as long as the subscription object exists, an invalid subscription in case of an error
		 */
		[[nodiscard]] ScopedSubscription detachKernelDriver(const int interfaceIndex, bool* driverWasNotActive = nullptr);

		/**
		 * Claims an interface of the device.
		 * @param interfaceIndex The index of the interface to claim
		 * @return The scoped subscription object which will keep the interface claimed as long as the subscription object exists, an invalid subscription in case of an error
		 */
		[[nodiscard]] ScopedSubscription claimInterface(const int interfaceIndex);

		/**
		 * Returns the wrapped libusb device.
		 * @return The libusb device, nullptr if now device is wrapped
		 * @see isValid().
		 */
		inline libusb_device* usbDevice();

		/**
		 * Returns the handle to the wrapped libusb device.
		 * @return The libusb device handle, nullptr if the device is not yet opened
		 * @see isValid().
		 */
		inline libusb_device_handle* usbDeviceHandle();

		/**
		 * Explicitly releases the device.
		 * In case the device is opened, the device will be closed.
		 * Further, all claimed interfaces will be released and detached kernel drivers will re-attached.
		 */
		void release();

		/**
		 * The context which is associated with this device.
		 * @return The device's context, nullptr if the device is associated with the default context
		 */
		inline SharedContext context() const;

		/**
		 * Returns whether this device is valid.
		 * A valid device does not need to be open.
		 * @return True, of so
		 * @see isOpen().
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this device is open.
		 * A device is open if a corresponding device handle exists.
		 * @return True, if so
		 * @see isValid().
		 */
		inline bool isOpen() const;

		/**
		 * Returns whether this device is wrapping a device based on a given system device handle.
		 * @return True, if so
		 */
		inline bool isWrapped() const;

		/**
		 * Returns the class code of this device.
		 * The device needs to be valid.
		 * @return The device's class code
		 */
		inline libusb_class_code classCode() const;

		/**
		 * Move operator.
		 * @param device The device to be moved
		 * @return The reference to this device
		 */
		Device& operator=(Device&& device);

		/**
		 * Returns a string descriptor of an opened device.
		 * @param usbDeviceHandle The handle of the device for which the string descriptor will be returned
		 * @param index The index of the string descriptor to return
		 * @return The resulting string descriptor, empty in case of an error
		 */
		static std::string stringDescriptor(libusb_device_handle* usbDeviceHandle, const uint8_t index);

	protected:

		/**
		 * Initializes this device with a given opened libusb device.
		 * @param context The context which is associated with the device, nullptr to use the default context
		 * @param usbDevice The opened libusb device, must be valid
		 * @param usbDeviceHandle The handle to the opened libusb device, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(SharedContext context, libusb_device* usbDevice, libusb_device_handle* usbDeviceHandle);

		/**
		 * Re-attaches a detached kernel driver for a specified interface.
		 * @param interfaceIndex The index of the interface
		 */
		void reattachKernelDriver(const int interfaceIndex);

		/**
		 * Releases a claimed interface.
		 * @param interfaceIndex The index of the interface
		 */
		void releaseInterface(const int interfaceIndex);

		/**
		 * Disabled copy constructor.
		 */
		Device(const Device&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		Device& operator=(const Device&) = delete;

		/**
		 * Extracts the payload buffers from a given USB transfer object.
		 * The transfer can be an isochronous transfer or a bulk transfer.<br>
		 * The memory is not copied, only pointers and size are extracted from the transfer.<br>
		 * Thus, the resulting buffers are only valid as long the transfer object is not released.<br>
		 * @param usbTransfer The transfer object from which the payload buffers will be extracted, must have status LIBUSB_TRANSFER_COMPLETED
		 * @param bufferPointers The resulting payload buffers, one in case of bulk mode, multiple buffers in case of isochronous mode
		 * @return True, if succeeded
		 */
		static bool extractPayload(struct libusb_transfer& usbTransfer, BufferPointers& bufferPointers);

		/**
		 * Determines the layout of an isochronous transfer.
		 * @param usbContext The context to be used, nullptr to use the default context
		 * @param interface The interface on which the transfer will happen
		 * @param endpointAddress The address of the endpoint on which the transfer will happen
		 * @param maxVideoFrameSize The maximal size of a video frame, in bytes, with range [1, infinity)
		 * @param maxPayloadTransferSize The maximal transfer size of payload, in bytes, with range [1, infinity)
		 * @param transferSize The resulting size of an entire transfer, in bytes
		 * @param packetsPerTransfer The resulting number of packets per transfer
		 * @param bytesPerPacket The resulting number of bytes per packet
		 * @return The index of the altsetting which supports the specified maximal video frame size and payload size, -1 if no matching altsetting could be found
		 */
		static int determineIsochronousTransferLayout(libusb_context* usbContext, const libusb_interface& interface, const uint8_t endpointAddress, const uint32_t maxVideoFrameSize, const uint32_t maxPayloadTransferSize, size_t& transferSize, size_t& packetsPerTransfer, size_t& bytesPerPacket);

	protected:

		/// The context which is associated with this device, nullptr if the default context is used.
		SharedContext context_;

		/// Optional system device handle in case this object is wrapped (e.g., on Android platforms).
		int64_t systemDeviceHandle_ = 0;

		/// The actual libusb device.
		libusb_device* usbDevice_ = nullptr;

		/// The handle to the opened libusb device.
		libusb_device_handle* usbDeviceHandle_ = nullptr;

		/// The device descriptor of this device.
		libusb_device_descriptor usbDeviceDescriptor_ = {};

		/// The usage counter for detached kernel drivers.
		UsageMap detachedInterfaceUsageMap_;

		/// The usage counter for claimed interfaces.
		UsageMap claimedInterfaceUsageMap_;

		/// The device's lock.
		mutable Lock lock_;
};

inline Device::Device(Device&& device)
{
	*this = std::move(device);
}

inline libusb_device* Device::usbDevice()
{
	const ScopedLock scopedLock(lock_);

	return usbDevice_;
}

inline libusb_device_handle* Device::usbDeviceHandle()
{
	const ScopedLock scopedLock(lock_);

	return usbDeviceHandle_;
}

inline SharedContext Device::context() const
{
	return context_;
}

inline bool Device::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return usbDevice_ != nullptr;
}

inline bool Device::isOpen() const
{
	const ScopedLock scopedLock(lock_);

	return usbDeviceHandle_ != nullptr;
}

inline bool Device::isWrapped() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(systemDeviceHandle_ == 0 || isValid());

	return systemDeviceHandle_ != 0;
}

inline libusb_class_code Device::classCode() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return libusb_class_code(usbDeviceDescriptor_.bDeviceClass);
}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_DEVICE_H
