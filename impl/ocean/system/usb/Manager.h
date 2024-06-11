/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_MANAGER_H
#define META_OCEAN_SYSTEM_USB_MANAGER_H

#include "ocean/system/usb/USB.h"
#include "ocean/system/usb/Device.h"
#include "ocean/system/usb/Context.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace System
{

namespace USB
{

/**
 * This class implements a manager for USB devices.
 * The manager holds an own context and can be used to enumerate all USB devices (on platforms allowing to discover devices).
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_EXPORT Manager :
	public Singleton<Manager>,
	protected Thread
{
	friend class Singleton<Manager>;

	public:

		/**
		 * Enumerates all available USB devices.
		 * @param devices Resulting list of available USB devices
		 * @param deviceClass Optional USB class a device must have (can be an interface class) to be enumerated, -1 to enumerate all devices
		 * @return True, if succeeded
		 */
		bool enumerateDevices(SharedDevices& devices, const int deviceClass = -1);

		/**
		 * Finds a device by its name.
		 * @param deviceName The name of the USB device to find, must be valid
		 * @return The found device, nullptr if no such device exists
		 */
		SharedDevice findDevice(const std::string& deviceName);

		/**
		 * Returns the context of this manager.
		 * @return The manager's context
		 */
		inline SharedContext context() const;

		/**
		 * Explicitly releases the manager.
		 */
		void release();

	protected:

		/**
		 * Creates a new manager with an own context.
		 */
		Manager();

		/**
		 * Destructs the manager and releases connected resources.
		 */
		~Manager() override;

		/**
		 * The manager's thread function.
		 */
		void threadRun() override;

		/**
		 * Libusb log callback function.
		 * @param context The libusb context sending the log
		 * @param level The log level
		 * @param message The log message
		 */
		static void LIBUSB_CALL libLogCallback(libusb_context* context, enum libusb_log_level level, const char* message);

	protected:

		/// The manager's context.
		SharedContext context_;

		/// The manager's lock.
		Lock lock_;
};

inline SharedContext Manager::context() const
{
	return context_;
}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_MANAGER_H
