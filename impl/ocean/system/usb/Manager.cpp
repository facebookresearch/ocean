/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/Manager.h"
//#include "ocean/system/usb/VideoDevice.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace System
{

namespace USB
{

Manager::Manager()
{
	const libusb_version* usbVersion = libusb_get_version();

	Log::info() << "LibUSB version: " << usbVersion->major << "." << usbVersion->minor << "." << usbVersion->micro;
	Log::info() << " ";

	bool noDeviceDiscovery = false;
	bool usbDk = false;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	/// on Android, we cannot discover devices e.g., via 'libusb_get_device_list'
	/// instead we need to use Android Java USB API to open USB device before accessing them through their file descriptor here in the native code
	noDeviceDiscovery = true;
#endif

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
	/// on Windows, we use UsbDk
	/// UsbDk (USB Development Kit) is a open-source library for Windows meant to provide user mode applications with direct and exclusive access to USB device
	usbDk = true;
#endif

	Context context(noDeviceDiscovery, usbDk);

	if (context.isValid())
	{
#ifdef OCEAN_DEBUG
		constexpr int mode = LIBUSB_LOG_LEVEL_WARNING;
#else
		constexpr int mode = LIBUSB_LOG_LEVEL_ERROR;
#endif

		libusb_set_option(context.usbContext(), LIBUSB_OPTION_LOG_LEVEL, mode);
		libusb_set_log_cb(context.usbContext(), libLogCallback, LIBUSB_LOG_CB_CONTEXT);

		context_ = std::make_shared<Context>(std::move(context));
	}

	startThread();
}

Manager::~Manager()
{
	release();
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	stopThreadExplicitly();

	context_ = nullptr;
}

bool Manager::enumerateDevices(SharedDevices& devices, const int deviceClass)
{
	const ScopedLock scopedLock(lock_);

	if (!context_)
	{
		return false;
	}

	libusb_device** usbDevices = nullptr;
	const ssize_t numberDevices = libusb_get_device_list(context_->usbContext(), &usbDevices);

	if (numberDevices < 0)
	{
		Log::error() << "Failed to enumerate devices, error: " << libusb_error_name(int(numberDevices));

		return false;
	}

	devices.clear();

	for (ssize_t n = 0; n < numberDevices; ++n)
	{
		libusb_device* usbDevice = usbDevices[n];

		struct libusb_device_descriptor usbDescriptor;
		const int result = libusb_get_device_descriptor(usbDevice, &usbDescriptor);

		if (result < 0)
		{
			Log::error() << "Failed to determine device description for device " << n;
			break;
		}

		bool foundMatch = deviceClass < 0;

		if (!foundMatch)
		{
#if defined(LIBUSB_API_VERSION) && LIBUSB_API_VERSION >= 0x0100010A
			libusb_interface_association_descriptor_array* usbInterfaceAssociationDescriptors = nullptr;

			const int associationResult = libusb_get_interface_association_descriptors(usbDevice, 0, &usbInterfaceAssociationDescriptors);

			if (associationResult == LIBUSB_SUCCESS)
			{
				for (int i = 0; i < usbInterfaceAssociationDescriptors->length; ++i)
				{
					const libusb_interface_association_descriptor& associationDescriptor = usbInterfaceAssociationDescriptors->iad[i];

					if (int(associationDescriptor.bFunctionClass) == deviceClass)
					{
						foundMatch = true;
					}
				}

				libusb_free_interface_association_descriptors(usbInterfaceAssociationDescriptors);
			}
#else
			Log::warning() << "This version of libusb does not support filtering for USB devices";
#endif // LIBUSB_API_VERSION
		}

		if (foundMatch)
		{
			SharedDevice device = std::make_shared<Device>(context_, usbDevice);
			ocean_assert(device->isValid());

			devices.emplace_back(std::move(device));
		}
	}

	libusb_free_device_list(usbDevices, 1 /*unref_devices */); // we can un-reference all devices, as the Device object holds an own reference

	return true;
}

SharedDevice Manager::findDevice(const std::string& deviceName)
{
	ocean_assert(!deviceName.empty());
	if (deviceName.empty())
	{
		return nullptr;
	}

	SharedDevices devices;
	if (!enumerateDevices(devices))
	{
		return nullptr;
	}

	for (const SharedDevice& device : devices)
	{
		ocean_assert(device && device->isValid());

		if (device->name() == deviceName)
		{
			return device;
		}
	}

	return nullptr;
}

void Manager::threadRun()
{
	ocean_assert(context_ && context_->isValid());

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = long(Timestamp::seconds2microseconds(0.01));

	while (shouldThreadStop() == false)
	{
		ocean_assert(context_ && context_->isValid());

		const int eventResult = libusb_handle_events_timeout_completed(context_->usbContext(), &timeout, nullptr);

		if (eventResult != LIBUSB_SUCCESS)
		{
			Log::error() << "Error during event handling, error " << libusb_error_name(eventResult);
		}

		Thread::sleep(1u);
	}

	ocean_assert(context_ && context_->isValid());
}

void Manager::libLogCallback(libusb_context* /*context*/, enum libusb_log_level level, const char* message)
{
	ocean_assert(message != nullptr);

	switch (level)
	{
		case LIBUSB_LOG_LEVEL_NONE:
			ocean_assert(false && "This should never happen!");
			break;

		case LIBUSB_LOG_LEVEL_ERROR:
			Log::error() << "USB::Manager: " << message;
			break;

		case LIBUSB_LOG_LEVEL_WARNING:
			Log::warning() << "USB::Manager: " << message;
			break;

		case LIBUSB_LOG_LEVEL_INFO:
			Log::warning() << "USB::Manager: " << message;
			break;

		case LIBUSB_LOG_LEVEL_DEBUG:
			Log::debug() << "USB::Manager: " << message;
			break;
	}
}

}

}

}
