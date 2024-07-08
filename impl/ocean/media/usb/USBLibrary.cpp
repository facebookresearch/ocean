/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/usb/USBLibrary.h"
#include "ocean/media/usb/USBLiveVideo.h"

#include "ocean/system/usb/Manager.h"
#include "ocean/system/usb/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/NativeInterfaceManager.h"
#endif

namespace Ocean
{

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	using namespace System::USB::Android;
#endif

namespace Media
{

namespace USB
{

USBLibrary::USBLibrary() :
	Library(nameUSBLibrary(), 10u)
{
	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("gif");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("tiff");
	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("mp4");
}

USBLibrary::~USBLibrary()
{
	// nothing to do here
}

bool USBLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<USBLibrary>(nameUSBLibrary());
}

bool USBLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameUSBLibrary());
}

USBLibrary::Definitions USBLibrary::enumerateCameraDefinitions()
{
	Definitions definitions;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	OceanUSBManager::DeviceDescriptors androidDeviceDescriptors = USBLibrary::androidEnumerateCameraDevices();

	definitions.reserve(androidDeviceDescriptors.size());

	for (OceanUSBManager::DeviceDescriptor& deviceDescriptor : androidDeviceDescriptors)
	{
		std::string url = std::move(deviceDescriptor.productName_);

		if (url.empty())
		{
			// in case we don't have product name, we use the device name instead
			url = deviceDescriptor.deviceName_;
		}

		definitions.emplace_back(std::move(url), Medium::LIVE_VIDEO, nameUSBLibrary(), std::move(deviceDescriptor.deviceName_));
	}

#else // OCEAN_PLATFORM_BUILD_ANDROID

	/// we are enumerating all devices with either video class (either the device base class or the device has an interface with this class)
	constexpr int videoClass = 0x0E; // USB video class

	System::USB::SharedDevices devices;
	if (System::USB::Manager::get().enumerateDevices(devices, videoClass))
	{
		for (const System::USB::SharedDevice& device : devices)
		{
			ocean_assert(device && device->isValid());

			std::string url = device->productName();
			std::string deviceName = device->name();
			ocean_assert(!deviceName.empty());

			if (url.empty())
			{
				url = System::USB::Utilities::productName(device->vendorId(), device->productId());
			}

			if (url.empty())
			{
				url = deviceName;
			}

			definitions.emplace_back(std::move(url), Medium::LIVE_VIDEO, nameUSBLibrary(), std::move(deviceName));
		}
	}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	return definitions;
}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

OceanUSBManager::DeviceDescriptors USBLibrary::androidEnumerateCameraDevices()
{
	if (!OceanUSBManager::get().isInitialized())
	{
		Log::warning() << "The Android OceanUSBManager is not initialized.";

		return OceanUSBManager::DeviceDescriptors();
	}

	JNIEnv* jniEnv = Platform::Android::NativeInterfaceManager::get().environment();

	if (jniEnv == nullptr)
	{
		Log::warning() << "The Android native interface manager is not initialized.";

		return OceanUSBManager::DeviceDescriptors();
	}

	OceanUSBManager::DeviceDescriptors deviceDescriptors;

	/// we are enumerating all devices with either video class (either the device base class or the device has an interface with this class)
	constexpr unsigned int usbClass = 0x0Eu;

	if (OceanUSBManager::get().enumerateDevices(jniEnv, deviceDescriptors, usbClass))
	{
#ifdef OCEAN_DEBUG
		Log::debug() << "USBLibrary: Found " << deviceDescriptors.size() << " USB camera device(s):";

		for (size_t n = 0; n < deviceDescriptors.size(); ++n)
		{
			Log::debug() << "USB device " << n << ":\n" << deviceDescriptors[n].toString() + "\n";
		}
#endif // OCEAN_DEBUG

		// let's try to lookup product names in case the product name could not be determined automatically

		for (OceanUSBManager::DeviceDescriptor& deviceDescriptor : deviceDescriptors)
		{
			ocean_assert(deviceDescriptor.isValid());

			if (deviceDescriptor.productName_.empty())
			{
				// trying to lookup the device name
				deviceDescriptor.productName_ = System::USB::Utilities::productName(deviceDescriptor.vendorId_, deviceDescriptor.productId_);
			}
		}
	}

	return deviceDescriptors;
}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

MediumRef USBLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	MediumRef medium = newLiveVideo(url, useExclusive);

	return medium;
}

MediumRef USBLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (type == Medium::LIVE_VIDEO || type == Medium::FRAME_MEDIUM)
	{
		MediumRef medium = newLiveVideo(url, useExclusive);

		if (medium)
		{
			return medium;
		}
	}

	return MediumRef();
}

RecorderRef USBLibrary::newRecorder(const Recorder::Type /*type*/)
{
	// the library does not support any recorder
	return RecorderRef();
}

USBLibrary::Definitions USBLibrary::selectableMedia() const
{
	const ScopedLock scopedLock(lock);

	updateAvailableDevices();

	return availableDevices_;
}

USBLibrary::Definitions USBLibrary::selectableMedia(const Medium::Type type) const
{
	if (type & Medium::LIVE_VIDEO)
	{
		return selectableMedia();
	}

	return Definitions();
}

Medium::Type USBLibrary::supportedTypes() const
{
	return Medium::Type(Medium::LIVE_VIDEO);
}

void USBLibrary::updateAvailableDevices() const
{
	// we will re-enumerate the devices every 0.5 seconds
	if (lastAvailableDevicesTimestamp_.isValid() && lastAvailableDevicesTimestamp_ + 0.5 >= Timestamp(true))
	{
		return;
	}

	availableDevices_ = enumerateCameraDefinitions();
}

std::string USBLibrary::resolveUrl(const std::string& url, std::string& deviceName) const
{
	if (url.empty())
	{
		return std::string();
	}

	size_t index = size_t(-1);

	if (url.find("LiveVideoId:") == 0 && url.size() >= 13)
	{
		int value = -1;
		if (String::isInteger32(url.substr(12), &value) && value >= 0)
		{
			index = size_t(value);
		}
	}

	const ScopedLock scopedLock(lock);

	updateAvailableDevices();

#ifdef OCEAN_DEBUG
	Log::debug() << "USBLibrary: " << availableDevices_.size() << " USB devices available";
	for (size_t n = 0; n < availableDevices_.size(); ++n)
	{
		Log::debug() << n << ": " << availableDevices_[n].url() << ", " << availableDevices_[n].uniqueIdentifier();
	}
#endif

	for (size_t n = 0; n < availableDevices_.size(); ++n)
	{
		if (index == n || url == availableDevices_[n].url() || url == availableDevices_[n].uniqueIdentifier())
		{
			deviceName = availableDevices_[n].uniqueIdentifier();

			return availableDevices_[n].url();
		}
	}

	return std::string();
}

MediumRef USBLibrary::newLiveVideo(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameUSBLibrary(), Medium::LIVE_VIDEO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	std::string deviceName;
	const std::string resolvedUrl = resolveUrl(url, deviceName);

	if (resolvedUrl.empty() || deviceName.empty())
	{
		Log::debug() << "USBLibrary: Failed to resolve the URL '" << url << "'";
		return MediumRef();
	}

	USBLiveVideo* medium = new USBLiveVideo(url, deviceName);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

LibraryRef USBLibrary::create()
{
	return LibraryRef(new USBLibrary());
}

}

}

}
