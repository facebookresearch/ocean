/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/DeviceRef.h"

namespace Ocean
{

namespace Devices
{

DeviceRefManager::~DeviceRefManager()
{
	ocean_assert(deviceMap_.empty());
}

DeviceRef DeviceRefManager::registerDevice(Device* device, const bool exclusive)
{
	ocean_assert(device != nullptr);

	const ScopedLock scopedLock(lock_);

	DeviceRef deviceRef(device, DeviceRef::ReleaseCallback(DeviceRefManager::get(), &DeviceRefManager::unregisterDevice));

	deviceMap_.insert(std::make_pair(device->name(), std::make_pair(deviceRef, exclusive)));

	deviceCallbacks_(device, true);

	return deviceRef;
}

DeviceRef DeviceRefManager::device(const std::string& name) const
{
	ocean_assert(name.empty() == false);

	const ScopedLock scopedLock(lock_);

	const DeviceMap::const_iterator i = deviceMap_.find(name);
	if (i != deviceMap_.end())
	{
		return i->second.first;
	}

	return DeviceRef();
}

DeviceRef DeviceRefManager::device(const Device::DeviceType type) const
{
	const ScopedLock scopedLock(lock_);

	for (DeviceMap::const_iterator i = deviceMap_.begin(); i != deviceMap_.end(); ++i)
	{
		ocean_assert(i->second.first);

		if (i->second.second == false && i->second.first->type() >= type)
		{
			return i->second.first;
		}
	}

	return DeviceRef();
}

bool DeviceRefManager::isEmpty(const std::string& library) const
{
	const ScopedLock scopedLock(lock_);

	for (DeviceMap::const_iterator i = deviceMap_.cbegin(); i != deviceMap_.cend(); ++i)
	{
		if (i->second.first->library() == library)
		{
			return false;
		}
	}

	return true;
}

std::vector<std::string> DeviceRefManager::devicesFromLibrary(const std::string& library) const
{
	const ScopedLock scopedLock(lock_);

	std::vector<std::string> result;

	for (DeviceMap::const_iterator i = deviceMap_.cbegin(); i != deviceMap_.cend(); ++i)
	{
		if (i->second.first->library() == library)
		{
			result.emplace_back(i->second.first->name());
		}
	}

	return result;
}

void DeviceRefManager::addDeviceCallbackFunction(const DeviceCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(callback);
	deviceCallbacks_.addCallback(callback);

	// we call the callback function for each existing device

	for (DeviceMap::iterator i = deviceMap_.begin(); i != deviceMap_.end(); ++i)
	{
		const DeviceRef& device = i->second.first;
		ocean_assert(device);

		callback(&*device, true);
	}
}

bool DeviceRefManager::isExclusive(const Device* device)
{
	ocean_assert(device != nullptr);

	const ScopedLock scopedLock(lock_);

	for (DeviceMap::iterator i = deviceMap_.begin(); i != deviceMap_.end(); ++i)
	{
		if (&*i->second.first == device)
		{
			return i->second.second;
		}
	}

	ocean_assert(false && "This should never happen.");
	return false;
}

void DeviceRefManager::unregisterDevice(const Device* device)
{
	ocean_assert(device != nullptr);

	const ScopedLock scopedLock(lock_);

	for (DeviceMap::iterator i = deviceMap_.begin(); i != deviceMap_.end(); ++i)
	{
		if (&*i->second.first == device)
		{
			deviceCallbacks_(&*i->second.first, false);

			deviceMap_.erase(i);
			break;
		}
	}
}

}

}
