/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_USB_USB_LIBRARY_H
#define META_OCEAN_MEDIA_USB_USB_LIBRARY_H

#include "ocean/media/usb/USB.h"

#include "ocean/media/Library.h"
#include "ocean/media/Manager.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/system/usb/android/OceanUSBManager.h"
#endif

namespace Ocean
{

namespace Media
{

namespace USB
{

/**
 * This class implements the USB library.
 * @ingroup mediausb
 */
class OCEAN_MEDIA_USB_EXPORT USBLibrary : public Library
{
	friend class USBLiveVideo;
    friend class Media::Manager;

	public:

		/**
		 * Creates a new medium by a given url.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, bool useExclusive = false) override;

		/**
		 * Creates a new medium by a given url and an expected type.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, const Medium::Type type, bool useExclusive = false) override;

        /**
		 * Creates a new recorder specified by the recorder type.
		 * @see Library::newRecorder().
		 */
		RecorderRef newRecorder(const Recorder::Type type) override;

		/**
		 * Returns a list of selectable mediums.
		 * @see Library::selectableMedia().
		 */
		Definitions selectableMedia() const override;

		/**
		 * Returns a list of specific selectable mediums.
		 * @see Library::selectableMedia().
		 */
		Definitions selectableMedia(const Medium::Type type) const override;

		/**
		 * Returns the supported medium types.
		 * @see Library::supportedTypes().
		 */
		Medium::Type supportedTypes() const override;

		/**
		 * Registers this library at the global media manager.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library has not been registered before
		 * @see Manager, unregisterLibrary()
		 */
		static bool registerLibrary();

		/**
		 * Unregisters this library at the global media manager.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		static bool unregisterLibrary();

		/**
		 * Returns the definitions of all available USB camera devices.
		 * @return The definitions of the available USB cameras, empty if no camera is connected or an error occurred
		 */
		static Definitions enumerateCameraDefinitions();

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Returns device descriptors for all available USB camera devices on Android platforms.
		 * On Android, we cannot enumerate all USB devices through libusb, instead we need to enumerate devices through Java.
		 * @return The device descriptors for all available USB camera devices, empty if no camera is connected or an error occurred
		 */
		static System::USB::Android::OceanUSBManager::DeviceDescriptors androidEnumerateCameraDevices();

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	protected:

		/**
		 * Creates a new USBLibrary object.
		 */
		USBLibrary();

		/**
		 * Destructs an USBLibrary object.
		 */
		~USBLibrary() override;

		/**
		 * Ensures that the available devices are up-to-date.
		 */
		void updateAvailableDevices() const;

		/**
		 * Resolves a medium url to a device name.
		 * @param url The URL of the medium, must be valid
		 * @param deviceName The resulting device name
		 * @return True, if succeeded
		 */
		std::string resolveUrl(const std::string& url, std::string& deviceName) const;

		/**
		 * Creates a new live video medium.
		 * @param url The URL of the live video medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		MediumRef newLiveVideo(const std::string& url, bool useExclusive);

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

	protected:

		/// The definitions of available USB devices.
		mutable Definitions availableDevices_;

		/// The timestamp when the available devices have been queried the last time.
		mutable Timestamp lastAvailableDevicesTimestamp_;
};

}

}

}

#endif // META_OCEAN_MEDIA_USB_USB_LIBRARY_H
