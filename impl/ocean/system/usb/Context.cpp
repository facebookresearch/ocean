/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/Context.h"

namespace Ocean
{

namespace System
{

namespace USB
{

Context::Context(const bool noDeviceDiscovery, const bool usbDk)
{
	ocean_assert(usbContext_ == nullptr);

// Temporarily disabling path for LIBUSB_API_VERSION >= 0x0100010A.
//
// #if defined(LIBUSB_API_VERSION) && LIBUSB_API_VERSION >= 0x0100010A
//
// 	std::vector<libusb_init_option> options;
//
// 	if (noDeviceDiscovery)
// 	{
// 		options.emplace_back(LIBUSB_OPTION_WEAK_AUTHORITY);
// 	}
//
// 	const int initResult = libusb_init_context(&usbContext_, options.data(), int(options.size()));
//
// #else

	if (noDeviceDiscovery)
	{
		const int result = libusb_set_option(nullptr, LIBUSB_OPTION_WEAK_AUTHORITY, nullptr);

		if (result != LIBUSB_SUCCESS)
		{
			Log::error() << "Failed to set no device discovery (weak authority), error " << libusb_error_name(result);
		}
	}

	const int initResult = libusb_init(&usbContext_);

// #endif // LIBUSB_API_VERSION >= 0x0100010A

	if (initResult != LIBUSB_SUCCESS)
	{
		ocean_assert(!isValid());

		Log::error() << "Failed to initialize context, error " << libusb_error_name(initResult);

		return;
	}

	if (usbDk)
	{
		// for 'reasons' the option for UsbDK needs to be set after the context has been created

		const int openResult = libusb_set_option(usbContext_, LIBUSB_OPTION_USE_USBDK);

		if (openResult != LIBUSB_SUCCESS)
		{
			Log::error() << "Failed to set usage of UsbDk, error " << libusb_error_name(openResult);
		}
	}

	ocean_assert(isValid());
}

Context::~Context()
{
	release();
}

void Context::release()
{
	const ScopedLock scopedLock(lock_);

	if (usbContext_ != nullptr)
	{
		libusb_exit(usbContext_);

		usbContext_ = nullptr;
	}
}

Context& Context::operator=(Context&& context) noexcept
{
	if (this != &context)
	{
		release();

		usbContext_ = context.usbContext_;
		context.usbContext_ = nullptr;
	}

	return *this;
}

}

}

}
