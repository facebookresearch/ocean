// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/Manager.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace UnifiedFeatures
{

std::shared_ptr<UnifiedObject> Manager::create(const std::string& name, const std::shared_ptr<Parameters>& parameters)
{
	const ScopedLock scopedLock(lock_);

	const UnifiedObjectMap::const_iterator iter = unifiedObjectMap_.find(name);

	if (iter == unifiedObjectMap_.cend())
	{
		return nullptr;
	}

	return iter->second.first(parameters);
}

std::vector<std::string> Manager::unifiedObjectNames() const
{
	const ScopedLock scopedLock(lock_);

	std::vector<std::string> names;
	names.reserve(unifiedObjectMap_.size());

	for (const UnifiedObjectMap::value_type& unifiedObject : unifiedObjectMap_)
	{
		ocean_assert(!unifiedObject.first.empty());
		names.emplace_back(unifiedObject.first);
	}

	return names;
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(unifiedObjectMap_.empty() && "Not all object have been unregistered!");
	unifiedObjectMap_.clear();
}

bool Manager::registerUnifiedObject(const std::string& name, const CreateUnifiedObjectCallback& createUnifiedObjectCallback)
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	// First check if the unified object has been registered before
	UnifiedObjectMap::iterator iter = unifiedObjectMap_.find(name);

	if (iter != unifiedObjectMap_.end())
	{
		// Increment the reference counter
		++(iter->second.second);

		return false;
	}

	unifiedObjectMap_.emplace(name, std::make_pair(createUnifiedObjectCallback, 1u));

	return true;
}

bool Manager::unregisterUnifiedObject(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	UnifiedObjectMap::iterator iter = unifiedObjectMap_.find(name);

	if (iter == unifiedObjectMap_.end())
	{
		ocean_assert(false && "Unknown name of unified object!");
		return false;
	}

	// Decrement the reference counter
	ocean_assert(iter->second.second != 0u);
	--(iter->second.second);

	if (iter->second.second == 0u)
	{
		unifiedObjectMap_.erase(iter);
		return true;
	}

	return false;
}

Manager::Manager()
{
	// Nothing else to do here
}

Manager::~Manager()
{
	release();
}

} // namespace UnfiedFeatures

} // namespace Ocean
