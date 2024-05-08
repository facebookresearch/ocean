/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Manager.h"

namespace Ocean
{

namespace Devices
{

Manager::AdhocFactory::AdhocFactory() :
	Factory(nameAdhocFactory())
{
	// nothing to do here
}

Manager::Manager()
{
	registerFactory(std::unique_ptr<Factory>(new AdhocFactory()));
}

Manager::~Manager()
{
	release();
}

DeviceRef Manager::device(const std::string& name, const bool useExclusive)
{
	const ScopedLock scopedLock(lock_);

	for (std::unique_ptr<Factory>& factory : factories_)
	{
		const DeviceRef deviceRef(factory->device(name, useExclusive));

		if (deviceRef)
		{
			return deviceRef;
		}
	}

	return DeviceRef();
}

DeviceRef Manager::device(const Device::DeviceType type, const bool useExclusive)
{
	const ScopedLock scopedLock(lock_);

	for (std::unique_ptr<Factory>& factory : factories_)
	{
		const DeviceRef deviceRef(factory->device(type, useExclusive));

		if (deviceRef)
		{
			return deviceRef;
		}
	}

	return DeviceRef();
}

Strings Manager::devices() const
{
	const ScopedLock scopedLock(lock_);

	Strings results;
	results.reserve(factories_.size() * 8);

	for (const std::unique_ptr<Factory>& factory : factories_)
	{
		const Strings result(factory->devices());

		results.insert(results.cend(), result.cbegin(), result.cend());
	}

	return results;
}

Strings Manager::devices(const Device::DeviceType type) const
{
	const ScopedLock scopedLock(lock_);

	Strings results;
	results.reserve(factories_.size() * 8);

	for (const std::unique_ptr<Factory>& factory : factories_)
	{
		const Strings result(factory->devices(type));

		results.insert(results.cend(), result.cbegin(), result.cend());
	}

	return results;
}

bool Manager::isRegistered(const std::string& library)
{
	const ScopedLock scopedLock(lock_);

	for (const std::unique_ptr<Factory>& factory : factories_)
	{
		if (factory->name() == library)
		{
			return true;
		}
	}

	return false;
}

Strings Manager::libraries() const
{
	const ScopedLock scopedLock(lock_);

	Strings result;
	result.reserve(factories_.size());

	for (const std::unique_ptr<Factory>& factory : factories_)
	{
		result.push_back(factory->name());
	}

	return result;
}

std::string Manager::objectDescription(const Measurement::ObjectId objectId) const
{
	const ScopedLock scopedLock(lock_);

	const ObjectIdMultimap::const_iterator i = objectIdMultimap_.find(objectId);

	if (i == objectIdMultimap_.cend())
	{
		return std::string();
	}

	return i->second;
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	factories_.clear();
}

bool Manager::registerAdhocDevice(const std::string& deviceName, const Device::DeviceType& deviceType, const AdhocInstanceFunction& deviceInstanceFunction)
{
	if (deviceName.empty() || !deviceType || !deviceInstanceFunction)
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (factories_.empty())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const std::unique_ptr<Factory>& adhocFactor = factories_.front();

	if (adhocFactor->name() != nameAdhocFactory())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return adhocFactor->registerDevice(deviceName, deviceType, deviceInstanceFunction);
}

bool Manager::unregisterAdhocDevice(const std::string& deviceName)
{
	if (deviceName.empty())
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (factories_.empty())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const std::unique_ptr<Factory>& adhocFactor = factories_.front();

	if (adhocFactor->name() != nameAdhocFactory())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return adhocFactor->unregisterDevice(deviceName);
}

std::string Manager::nameAdhocFactory()
{
	return std::string("Adhoc");
}

Measurement::ObjectId Manager::createUniqueObjectId(const std::string& description)
{
	const ScopedLock scopedLock(lock_);

	const Measurement::ObjectId newObjectId(uniqueObjectIdCounter_++);
	ocean_assert(newObjectId != Measurement::invalidObjectId());

	objectIdMultimap_.emplace(newObjectId, description);

	return newObjectId;
}

bool Manager::registerFactory(std::unique_ptr<Factory>&& factory)
{
	if (!factory)
	{
		ocean_assert(false && "Invalid factory!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	for (const std::unique_ptr<Factory>& existingFactory : factories_)
	{
		ocean_assert(existingFactory);

		if (existingFactory->name() == factory->name())
		{
			return false;
		}
	}

	std::string devices;
	for (const std::string& device : factory->devices())
	{
		if (!devices.empty())
		{
			devices += ", ";
		}

		devices += device;
	}

	Log::info() << "New device module registered \"" << factory->name() << "\" and supports the following devices: " << devices;

	factories_.emplace_back(std::move(factory));

	return true;
}

bool Manager::unregisterFactory(const std::string& factory)
{
	ocean_assert(factory.empty() == false);

	const ScopedLock scopedLock(lock_);

	if (DeviceRefManager::get().isEmpty(factory) == false)
	{
#ifdef OCEAN_DEBUG
		const std::vector<std::string> remainingDevices = DeviceRefManager::get().devicesFromLibrary(factory);

		Log::warning() << "<debug> The following devices of library '" << factory << "' are still in use:";
		for (const std::string& remainingDevice : remainingDevices)
		{
			Log::warning() << "<debug> " << remainingDevice;
		}

		ocean_assert(false && "Devices which have been created by the specified factory are still in use!");

#endif
		return false;
	}

	for (FactoryUniquePointers::iterator i = factories_.begin(); i != factories_.end(); ++i)
	{
		if ((*i)->name() == factory)
		{
			factories_.erase(i);

#ifdef OCEAN_DEBUG
			Log::info() << "Device module unregistered \"" << factory << "\" successfully.";
#endif

			return true;
		}
	}

	ocean_assert(false && "Unknown factory!");
	return false;
}

}

}
