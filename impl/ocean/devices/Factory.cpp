/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Factory.h"
#include "ocean/devices/Manager.h"

namespace Ocean
{

namespace Devices
{

Factory::Factory(const std::string& name) :
	name_(name)
{
	// nothing to do here
}

Factory::~Factory()
{
	// nothing to do here
}

Strings Factory::devices() const
{
	const ScopedLock scopedLock(lock_);

	Strings result;
	result.reserve(deviceDescriptors_.size());

	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		result.emplace_back(deviceDescriptor.name_);
	}

	return result;
}

Strings Factory::devices(const Device::DeviceType type) const
{
	const ScopedLock scopedLock(lock_);

	Strings result;
	result.reserve(deviceDescriptors_.size());

	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.type_ == type)
		{
			result.emplace_back(deviceDescriptor.name_);
		}
	}

	return result;
}

Device::DeviceType Factory::deviceType(const std::string& device) const
{
	const ScopedLock scopedLock(lock_);

	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.name_ == device)
		{
			return deviceDescriptor.type_;
		}
	}

	return Device::DeviceType();
}

DeviceRef Factory::device(const std::string& name, const bool useExclusive) const
{
	const ScopedLock scopedLock(lock_);

	if (!useExclusive)
	{
		const DeviceRef deviceRef(DeviceRefManager::get().device(name));

		if (deviceRef)
		{
			return deviceRef;
		}
	}

	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.name_ == name)
		{
			return createDevice(deviceDescriptor, useExclusive);
		}
	}

	return DeviceRef();
}

DeviceRef Factory::device(const Device::DeviceType type, const bool useExclusive)
{
	const ScopedLock scopedLock(lock_);

	if (!useExclusive)
	{
		const DeviceRef deviceRef(DeviceRefManager::get().device(type));

		if (deviceRef)
		{
			return deviceRef;
		}
	}

	// create the first device with perfect matching device type
	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.type_ == type)
		{
			return createDevice(deviceDescriptor, useExclusive);
		}
	}

	// create the first device with best matching device type
	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.type_ >= type)
		{
			return createDevice(deviceDescriptor, useExclusive);
		}
	}

	return DeviceRef();
}

bool Factory::registerFactory(std::unique_ptr<Factory>&& factory)
{
	return Manager::get().registerFactory(std::move(factory));
}

bool Factory::unregisterFactory(const std::string& factory)
{
	return Manager::get().unregisterFactory(factory);
}

DeviceRef Factory::adapterDevice(const Device::DeviceType /*type*/, const std::string& /*name*/)
{
	return DeviceRef();
}

bool Factory::registerDevice(const std::string& deviceName, const Device::DeviceType deviceType, const InstanceFunction& deviceInstanceFunction)
{
	ocean_assert(!deviceName.empty());
	ocean_assert(deviceType);
	ocean_assert(deviceInstanceFunction);

	const ScopedLock scopedLock(lock_);

	for (const DeviceDescriptor& deviceDescriptor : deviceDescriptors_)
	{
		if (deviceDescriptor.name_ == deviceName)
		{
			Log::warning() << "Devices already registered: " << deviceName;
			return false;
		}
	}

	deviceDescriptors_.emplace_back(deviceName, deviceType, deviceInstanceFunction);

	return true;
}

bool Factory::unregisterDevice(const std::string& deviceName)
{
	ocean_assert(!deviceName.empty());

	const ScopedLock scopedLock(lock_);

	const DeviceRef deviceRef(DeviceRefManager::get().device(deviceName));

	if (deviceRef)
	{
		ocean_assert(false && "The device is still in use!");
		return false;
	}

	for (DeviceDescriptors::iterator i = deviceDescriptors_.begin(); i != deviceDescriptors_.end(); ++i)
	{
		if (i->name_ == deviceName)
		{
			deviceDescriptors_.erase(i);
			return true;
		}
	}

	ocean_assert(false && "The device is unknown!");

	return false;
}

DeviceRef Factory::createDevice(const DeviceDescriptor& deviceDescriptor, bool useExclusive)
{
	if (!deviceDescriptor.instanceFunction_)
	{
		ocean_assert(false && "Invalid instance function!");
		return DeviceRef();
	}

	Device* device = deviceDescriptor.instanceFunction_(deviceDescriptor.name_, deviceDescriptor.type_);

	if (device == nullptr)
	{
		return DeviceRef();
	}

	if (device->isValid() == false)
	{
		delete device;
		return DeviceRef();
	}

	return DeviceRefManager::get().registerDevice(device, useExclusive);
}

}

}
