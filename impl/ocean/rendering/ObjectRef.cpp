/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

ObjectRefManager::~ObjectRefManager()
{
	ocean_assert(objectMap_.empty());
	ocean_assert(nameMap_.empty());
}

bool ObjectRefManager::hasEngineObject(const std::string& engine, const bool writeObjectToLog) const
{
	ocean_assert(!engine.empty());

	const ScopedLock scopedLock(lock_);

	bool result = false;

	for (ObjectMap::const_iterator i = objectMap_.begin(); i != objectMap_.end(); ++i)
	{
		ocean_assert(i->second);

		if (i->second->engineName() == engine)
		{
			result = true;

			if (writeObjectToLog)
			{
				Log::warning() << "Remaining " << *i->second;
			}
			else
			{
				break;
			}
		}
	}

	return result;
}

ObjectRef ObjectRefManager::object(const std::string& name) const
{
	ocean_assert(name.empty() == false);

	const ScopedLock scopedLock(lock_);

	NameMap::const_iterator iN = nameMap_.find(name);
	if (iN == nameMap_.end())
	{
		return ObjectRef();
	}

	ObjectMap::const_iterator iO = objectMap_.find(iN->second);
	if (iO != objectMap_.end())
	{
		return iO->second;
	}

	return ObjectRef();
}

ObjectRef ObjectRefManager::object(const ObjectId objectId) const
{
	if (objectId == invalidObjectId)
	{
		return ObjectRef();
	}

	const ScopedLock scopedLock(lock_);

	ObjectMap::const_iterator i = objectMap_.find(objectId);
	if (i == objectMap_.end())
	{
		return ObjectRef();
	}

	return i->second;
}

ObjectRefs ObjectRefManager::objects(const std::string& name) const
{
	ocean_assert(name.empty() == false);

	const ScopedLock scopedLock(lock_);

	ObjectRefs objects;

	std::pair<NameMap::const_iterator, NameMap::const_iterator> equalRange = nameMap_.equal_range(name);

	while (equalRange.first != equalRange.second)
	{
		const ObjectMap::const_iterator iO = objectMap_.find(equalRange.first->second);

		if (iO != objectMap_.end())
		{
			objects.push_back(iO->second);
		}

		++equalRange.first;
	}

	return objects;
}

ObjectRef ObjectRefManager::registerObject(Object* object)
{
	ocean_assert(object != nullptr);

	const ScopedLock scopedLock(lock_);

	ObjectRef objectRef(object, ObjectRef::ReleaseCallback(ObjectRefManager::get(), &ObjectRefManager::unregisterObject));

	const std::string& name(object->name());
	const ObjectId objectId(object->id());
	ocean_assert(objectId != invalidObjectId);

	ocean_assert(objectMap_.find(objectId) == objectMap_.cend());

	if (!name.empty())
	{
		nameMap_.insert(std::make_pair(name, objectId));
	}

	objectMap_.insert(std::make_pair(objectId, objectRef));

	return objectRef;
}

void ObjectRefManager::changeRegisteredObject(ObjectId objectId, const std::string& oldName, const std::string& newName)
{
	ocean_assert(objectId != invalidObjectId);
	ocean_assert(oldName != newName);

	const ScopedLock scopedLock(lock_);

	ocean_assert(objectMap_.find(objectId) != objectMap_.end());

	if (!oldName.empty())
	{
#ifdef OCEAN_DEBUG
		bool debugFound = false;
#endif
		ocean_assert(nameMap_.find(oldName) != nameMap_.end());

		std::pair<NameMap::const_iterator, NameMap::const_iterator> equalRange = nameMap_.equal_range(oldName);
		while (equalRange.first != equalRange.second)
		{
			if (equalRange.first->second == objectId)
			{
				nameMap_.erase(equalRange.first);

#ifdef OCEAN_DEBUG
				debugFound = true;
#endif

				break;
			}

			++equalRange.first;
		}

		ocean_assert(debugFound);
	}

	if (!newName.empty())
	{
		nameMap_.insert(std::make_pair(newName, objectId));
	}
}

void ObjectRefManager::unregisterObject(const Object* object)
{
	ocean_assert(object);

	const ScopedLock scopedLock(lock_);

	ocean_assert(object->id() != invalidObjectId);
	ObjectMap::iterator iO(objectMap_.find(object->id()));
	ocean_assert(iO != objectMap_.end());
	ocean_assert(iO->second);

	if (!object->name().empty())
	{
		std::pair<NameMap::const_iterator, NameMap::const_iterator> equalRange = nameMap_.equal_range(object->name());
		ocean_assert(equalRange.first != equalRange.second);

#ifdef OCEAN_DEBUG
		bool debugFound = false;
#endif

		while (equalRange.first != equalRange.second)
		{
			if (equalRange.first->second == object->id())
			{
				nameMap_.erase(equalRange.first);

#ifdef OCEAN_DEBUG
				debugFound = true;
#endif
				break;
			}

			++equalRange.first;
		}

#ifdef OCEAN_DEBUG
		ocean_assert(debugFound);
#endif
	}

	objectMap_.erase(iO);
}

}

}
