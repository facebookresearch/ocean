/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_ANDROID_OCEAN_USB_MANAGER_H
#define META_OCEAN_SYSTEM_USB_ANDROID_OCEAN_USB_MANAGER_H

#include "ocean/system/usb/android/Android.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#include "ocean/platform/android/ScopedJNIObject.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Android
{

/**
 * This class implements the native version of the USB manager for Android.
 * The class has a corresponding Java class which is used/necessary to access the manager from Java.
 * @ingroup systemusbandroid
 */
class OCEAN_SYSTEM_USB_ANDROID_EXPORT OceanUSBManager : public Singleton<OceanUSBManager>
{
	friend class Singleton<OceanUSBManager>;

	public:

		/**
		 * This class holds the relevant information of a USB device.
		 * However, it does not hold the actual device object or any reference to the device.
		 */
		class DeviceDescriptor
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				DeviceDescriptor() = default;

				/**
				 * Returns the device information as a string.
				 * @return The string holding the device information, 'Invalid' if the object is invalid
				 */
				std::string toString() const;

				/**
				 * Returns whether this object holds valid device information.
				 * @return True, if so
				 */
				inline bool isValid() const;

			public:

				/// The device's name as defined by the system (may not be human readable).
				std::string deviceName_;

				/// The product name of the device (human readable).
				std::string productName_;

				/// The manufacturer name of the device (human readable), empty if unknown.
				std::string manufacturerName_;

				/// The vendor id of the device.
				unsigned int vendorId_ = 0u;

				/// The product id of the device.
				unsigned int productId_ = 0u;

				/// The USB device class of the device.
				unsigned int deviceClass_ = 0u;

				/// The USB device subclass of the device.
				unsigned int deviceSubclass_ = 0u;

				/// The USB device protocol of the device.
				unsigned int deviceProtocol_ = 0u;
		};

		/**
		 * Definition of a vector holding device descriptor objects.
		 */
		using DeviceDescriptors = std::vector<DeviceDescriptor>;

		/**
		 * Definition of a callback functio for permission events.
		 * @param deviceName The name of the device, must be valid
		 * @param granted True, if the permission was granted; False, if the permission was denied
		 */
		using PermissionCallback = std::function<void(const std::string& deviceName, bool granted)>;

		/**
		 * Definition of a pair combining a unique id with a device name
		 */
		using DevicePair = std::pair<uint32_t, std::string>;

		/**
		 * Definition of a scoped subscription object.
		 */
		using ScopedPermissionSubscription = ScopedSubscriptionT<DevicePair, OceanUSBManager>;

	protected:

		/**
		 * Definition of a pair combining a unique id with a permission callback.
		 */
		using PermissionCallbackPair = std::pair<uint32_t, PermissionCallback>;

		/**
		 * Definition of a vector holding permission callback pairs.
		 */
		using PermissionCallbackPairs = std::vector<PermissionCallbackPair>;

		/**
		 * Definition of an unordered map mapping device names to permission callback functions.
		 */
		using PermissionCallbackMap = std::unordered_map<std::string, PermissionCallbackPairs>;

	public:

		/**
		 * Initializes the manager.
		 * This function should be called once before any other function is called, from the main thread.
		 * @param jniEnv The JNI environment, must be valid
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(JNIEnv* jniEnv);

		/**
		 * Returns whether the manager is initialized.
		 * @return True, if so
		 * @see initialize().
		 */
		bool isInitialized();

		/**
		 * Enumerates all currently available USB devices.
		 * @param jniEnv The JNI environment, must be valid
		 * @param deviceDescriptors The resulting list of available USB devices, empty if no device is available
		 * @param deviceClass Optional USB class a device must have (can be an interface class) to be enumerated, -1 to enumerate all devices
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool enumerateDevices(JNIEnv* jniEnv, DeviceDescriptors& deviceDescriptors, const unsigned int deviceClass = (unsigned int)(-1));

		/**
		 * Returns whether the application has permission to access the specified device.
		 * @param jniEnv The JNI environment, must be valid
		 * @param deviceName The name of the device, must be valid
		 * @param permissionGranted True, if the application has already been granted access to this device; False, if the application has not yet been granted access to this device
		 * @return True, if succeeded
		 * @see requestPermission().
		 */
		bool hasPermission(JNIEnv* jniEnv, const std::string& deviceName, bool& permissionGranted);

		/**
		 * Requests permission to access a specified device.
		 * This function returns a subscription object which will keep the permission request active as long as the subscription object exists.<br>
		 * Once the provided permission callback function is called, the scoped permission object is meaningless and can be relased at any time.
		 * @param jniEnv The JNI environment, must be valid
		 * @param deviceName The name of the device, must be valid
		 * @param permissionCallback Optional callback function which is called when the permission request has been finished, nullptr to avoid getting informed automatically
		 * @return The resulting permission subscription object, invalid if the permission request could not be started
		 */
		[[nodiscard]] ScopedPermissionSubscription requestPermission(JNIEnv* jniEnv, const std::string& deviceName, PermissionCallback permissionCallback = nullptr);

		/**
		 * Opens a specified device.
		 * On Android, a USB device cannot be opened/accessed without using the Java USB API.
		 * The Java API will return a file descriptor which can be used to access the device.
		 * @param jniEnv The JNI environment, must be valid
		 * @param deviceName The name of the device, must be valid
		 * @param fileDescriptor The resulting device's file descriptor
		 * @return True, if succeeded
		 */
		bool openDevice(JNIEnv* jniEnv, const std::string& deviceName, int64_t& fileDescriptor);

		/**
		 * Closes a specified device.
		 * @param jniEnv The JNI environment, must be valid
		 * @param deviceName The name of the device, must be valid
		 * @return True, if succeeded
		 */
		bool closeDevice(JNIEnv* jniEnv, const std::string& deviceName);

		/**
		 * Event functions for device permission events.
		 * Don't call this function manually, this function is called via the JNI bridge from Java.
		 * @param deviceName The name of the device, must be valid
		 * @param granted True, if the permission was granted; False, if the permission was denied
		 */
		void onDevicePermission(const std::string& deviceName, bool granted);

	protected:

		/**
		 * Creates a default manager object which is not yet initialized.
		 * @see initialize(), isInitialized().
		 */
		OceanUSBManager() = default;

		/**
		 * Releases a permission request which has been created before.
		 * @param devicePair The pair combining a unique id and the name of the device for which the request was created, must be valid
		 */
		void releasePermissionRequest(const DevicePair& devicePair);

	protected:

		/// The JNI class object of the Java OceanUSBManager class, invalid if not yet initialized.
		Platform::Android::ScopedJClass javaClassOceanUSBManager_;

		/// The map mapping device names to permission callback functions.
		PermissionCallbackMap permissionCallbackMap_;

		/// The counter for unique ids.
		uint32_t uniqueRequestIdCounter_ = 0u;

		/// The manager's lock.
		Lock lock_;
};

inline bool OceanUSBManager::DeviceDescriptor::isValid() const
{
	return vendorId_ != 0u && productId_ != 0u;
}

}

}

}

}

/**
 * Native interface function to initialize the manager, this function should be called from the main thread.
 * @param env Native interface environment, must be valid
 * @param javaThis JNI object, must be valid
 * @return True, if succeeded
 */
extern "C" bool Java_com_meta_ocean_system_usb_android_OceanUSBManager_initialize(JNIEnv* env, jobject javaThis);

/**
 * Native interface function for device permission events.
 * @param env Native interface environment, must be valid
 * @param javaThis JNI object, must be valid
 * @param deviceName The name of the device, must be valid
 * @param granted True, if the permission was granted; False, if the permission was denied
 */
extern "C" void Java_com_meta_ocean_system_usb_android_OceanUSBManager_onDevicePermission(JNIEnv* env, jobject javaThis, jstring deviceName, jboolean granted);

#endif // META_OCEAN_SYSTEM_USB_ANDROID_OCEAN_USB_MANAGER_H
