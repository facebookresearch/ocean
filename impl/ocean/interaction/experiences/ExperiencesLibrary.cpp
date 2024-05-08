/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/experiences/ExperiencesLibrary.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

ExperiencesLibrary::Instance::Instance()
{
	// nothing to do here
}

ExperiencesLibrary*& ExperiencesLibrary::Instance::experiencesLibrary()
{
	return experiencesLibrary_;
}

Lock& ExperiencesLibrary::Instance::lock()
{
	return lock_;
}

ExperiencesLibrary::ExperiencesLibrary() :
	Library(nameExperiencesLibrary())
{
	registerFileExtension("experience", "Experience interaction file");
}

ExperiencesLibrary::~ExperiencesLibrary()
{
	ocean_assert(nameToExperienceMap_.empty());
}

void ExperiencesLibrary::registerLibrary()
{
	const ScopedLock scopedLock(Instance::get().lock());

	ExperiencesLibrary* library = new ExperiencesLibrary();
	if (library == nullptr)
	{
		throw OutOfMemoryException("Not enough memory to create a new content interaction library.");
	}

	if (Library::registerFactory(*library))
	{
		ocean_assert(Instance::get().experiencesLibrary() == nullptr);
		Instance::get().experiencesLibrary() = library;
	}
	else
	{
		ocean_assert(false && "The library has been registered before!");
		delete library;
	}
}

bool ExperiencesLibrary::unregisterLibrary()
{
	const ScopedLock scopedLock(Instance::get().lock());

	ocean_assert(Instance::get().experiencesLibrary() != nullptr);
	Instance::get().experiencesLibrary() = nullptr;

	return Library::unregisterLibrary(nameExperiencesLibrary());
}

bool ExperiencesLibrary::registerNewExperience(std::string experienceName, CreateExperienceFunction createExperienceFunction)
{
	const ScopedLock instanceLock(Instance::get().lock());

	ExperiencesLibrary* library = Instance::get().experiencesLibrary();

	if (library != nullptr)
	{
		const ScopedLock libraryLock(library->lock_);

		if (experienceName.rfind(".experience") != 0)
		{
			// ensuring that the name has `.experience` as extension
			experienceName += ".experience";
		}

		if (library->nameToFunctionMap_.find(experienceName) == library->nameToFunctionMap_.cend())
		{
			library->nameToFunctionMap_.emplace(std::move(experienceName), std::move(createExperienceFunction));
			return true;
		}
		else
		{
			ocean_assert(false && "The experience has been registered already!");
			return false;
		}
	}

	ocean_assert(false && "This must never happen!");
	return false;
}

bool ExperiencesLibrary::load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (nameToExperienceMap_.find(filename) != nameToExperienceMap_.cend())
	{
		ocean_assert(false && "The experience has been loaded already!");
		return false;
	}

	const std::string::size_type position = filename.find("@");

	std::string experienceName;
	std::string properties;

	if (position == std::string::npos)
	{
		experienceName = filename;
	}
	else
	{
		if (position == filename.size() - 1)
		{
			return false;
		}

		experienceName = filename.substr(position + 1);
		properties = filename.substr(0, position);
	}

	NameToFunctionMap::const_iterator iFunction = nameToFunctionMap_.find(experienceName);

	if (iFunction == nameToFunctionMap_.cend())
	{
		return false;
	}

	UniqueExperience newExperience(iFunction->second());

	ocean_assert(newExperience);

	if (!newExperience)
	{
		return false;
	}

	if (!newExperience->preLoad(userInterface, engine, timestamp, properties) || !newExperience->load(userInterface, engine, timestamp, properties))
	{
		Log::warning() << "Failed to load experience '" << filename << "'";
		return false;
	}

	nameToExperienceMap_.emplace(filename, std::move(newExperience));

	return true;
}

bool ExperiencesLibrary::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	NameToExperienceMap::iterator iExperience = nameToExperienceMap_.find(filename);

	if (iExperience == nameToExperienceMap_.cend())
	{
		return false;
	}

	ocean_assert(iExperience->second);
	iExperience->second->unload(userInterface, engine, timestamp);
	iExperience->second->postUnload(userInterface, engine, timestamp);

	nameToExperienceMap_.erase(iExperience);

	return true;
}

void ExperiencesLibrary::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->unload(userInterface, engine, timestamp);
	}

	nameToExperienceMap_.clear();
}

void ExperiencesLibrary::preFileLoad(const UserInterface& /*userInterface*/, const std::string& /*filename*/)
{
	// nothing to do here
}

void ExperiencesLibrary::postFileLoad(const UserInterface& /*userInterface*/, const std::string& /*filename*/, const bool /*succeeded*/)
{
	// nothing to do here
}

Timestamp ExperiencesLibrary::preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(engine && view);

	const ScopedLock scopedLock(lock_);

	Timestamp localTimestamp(timestamp);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		localTimestamp = iExperience->second->preUpdate(userInterface, engine, view, localTimestamp);
	}

	return localTimestamp;
}

void ExperiencesLibrary::postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(engine && view);

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->postUpdate(userInterface, engine, view, timestamp);
	}
}

void ExperiencesLibrary::onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp timestamp)
{
	ocean_assert(engine && ray.isValid());

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->onMousePress(userInterface, engine, button, screenPosition, ray, timestamp);
	}
}

void ExperiencesLibrary::onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp timestamp)
{
	ocean_assert(engine && ray.isValid());

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->onMouseMove(userInterface, engine, button, screenPosition, ray, timestamp);
	}
}

void ExperiencesLibrary::onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp timestamp)
{
	ocean_assert(engine && ray.isValid());

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->onMouseRelease(userInterface, engine, button, screenPosition, ray, timestamp);
	}
}

void ExperiencesLibrary::onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->onKeyPress(userInterface, engine, key, timestamp);
	}
}

void ExperiencesLibrary::onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator iExperience = nameToExperienceMap_.begin(); iExperience != nameToExperienceMap_.end(); ++iExperience)
	{
		ocean_assert(iExperience->second);
		iExperience->second->onKeyRelease(userInterface, engine, key, timestamp);
	}
}

void ExperiencesLibrary::release()
{
	const ScopedLock scopedLock(lock_);

	for (NameToExperienceMap::iterator i = nameToExperienceMap_.begin(); i != nameToExperienceMap_.end(); ++i)
	{
		ocean_assert(i->second);
		i->second->release();

		i->second = nullptr;
	}

	nameToExperienceMap_.clear();
	nameToFunctionMap_.clear();

}

}

}

}
