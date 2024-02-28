// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/blob/BlobDevice.h"
#include "ocean/devices/blob/BlobFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

BlobDevice::BlobDevice(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	// nothing to do here
}

const std::string& BlobDevice::library() const
{
	static const std::string deviceStaticLibraryName(nameBlobLibrary());
	return deviceStaticLibraryName;
}

}

}

}
