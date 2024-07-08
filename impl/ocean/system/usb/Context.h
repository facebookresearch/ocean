/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_CONTEXT_H
#define META_OCEAN_SYSTEM_USB_CONTEXT_H

#include "ocean/system/usb/USB.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

namespace System
{

namespace USB
{

// Forward declaration.
class Context;

/**
 * Definition of a shared pointer holding a context.
 * @see Context
 * @ingroup systemusb
 */
using SharedContext = std::shared_ptr<Context>;

/**
 * This class wraps a libusb context.
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_EXPORT Context
{
	public:

		/** 
		 * Move constructor.
		 * @param context The context to be moved
		 */
		inline Context(Context&& context) noexcept;

		/**
		 * Creates a new custom context.
		 * Commonly there is no need to create a custom context, instead use the Manager's context.
		 * @param noDeviceDiscovery True, to disable device discovery (e.g., on Android platforms devices cannot be discovered but need to be provided explicitly from the Java side); False, to enable/allow device discovery
		 * @param usbDk True, to use the USB-Dk driver on Windows platforms; False, otherwise
		 * @see Manager::context().
		 */
		explicit Context(const bool noDeviceDiscovery = false, const bool usbDk = false);

		/**
		 * Destructs and releases the context.
		 */
		~Context();

		/**
		 * Returns the actual libusb context.
		 * @return The actual libusb context, may be nullptr which indicates the global default context
		 */
		inline libusb_context* usbContext() const;

		/**
		 * Explicitly releases the context.
		 */
		void release();

		/**
		 * Returns whether this object wraps a valid context (which is not nullptr).
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Move operator.
		 * @param context The context to be moved
		 * @return Reference to this object
		 */
		Context& operator=(Context&& context) noexcept;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		Context(const Context&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		Context& operator=(const Context&) = delete;

	protected:

		/// The actual libusb context.
		libusb_context* usbContext_ = nullptr;

		/// The context's lock.
		mutable Lock lock_;
};

inline Context::Context(Context&& context) noexcept
{
	*this = std::move(context);
}

inline libusb_context* Context::usbContext() const
{
	return usbContext_;
}

inline bool Context::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return usbContext_ != nullptr;
}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_CONTEXT_H
