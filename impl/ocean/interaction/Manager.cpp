/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/Manager.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

Manager::Manager()
{
	// nothing to do here
}

Manager::~Manager()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(librarySet_.empty() && "Manager::release() has to be called once before the termination of this object.");
	release();
}

bool Manager::load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	const IO::File file(filename);

	const std::string fileExtension = String::toLower(file.extension());

	const ScopedLock scopedLock(lock_);

	const IO::Files files(IO::FileResolver::get().resolve(file, true));

	if (!files.empty())
	{
		for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
		{
			if ((*i)->isFileExtensionSupported(fileExtension) && (*i)->load(userInterface, engine, timestamp, files.front()()))
			{
				return true;
			}
		}
	}
	else
	{
		// we do not have an existing file, so we check whether the filename is actually the name of an experience

		for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
		{
			if ((*i)->isFileExtensionSupported(fileExtension) && (*i)->load(userInterface, engine, timestamp, filename))
			{
				return true;
			}
		}
	}

	return false;
}

bool Manager::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		if ((*i)->unload(userInterface, engine, timestamp, filename))
		{
			return true;
		}
	}

	return false;
}

void Manager::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->unload(userInterface, engine, timestamp);
	}
}

void Manager::preFileLoad(const UserInterface& userInterface, const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->preFileLoad(userInterface, filename);
	}
}

void Manager::postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->postFileLoad(userInterface, filename, succeeded);
	}
}

Timestamp Manager::preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	Timestamp changingTimestamp(timestamp);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		changingTimestamp = (*i)->preUpdate(userInterface, engine, view, changingTimestamp);
	}

	return changingTimestamp;
}

void Manager::postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->postUpdate(userInterface, engine, view, timestamp);
	}
}

void Manager::onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->onMousePress(userInterface, engine, button, screenPosition, ray, pickedObject, pickedPosition, timestamp);
	}
}

void Manager::onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->onMouseMove(userInterface, engine, button, screenPosition, ray, pickedObject, pickedPosition, timestamp);
	}
}

void Manager::onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->onMouseRelease(userInterface, engine, button, screenPosition, ray, pickedObject, pickedPosition, timestamp);
	}
}

void Manager::onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->onKeyPress(userInterface, engine, key, timestamp);
	}
}

void Manager::onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->onKeyRelease(userInterface, engine, key, timestamp);
	}
}

Manager::FileExtensions Manager::supportedExtensions()
{
	const ScopedLock scopedLock(lock_);

	FileExtensions extensions;
	for (LibrarySet::const_iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		const Library::FileExtensions libraryExtensions((*i)->registeredFileExtensions());

		for (Library::FileExtensions::const_iterator iL = libraryExtensions.begin(); iL != libraryExtensions.end(); ++iL)
		{
			extensions.insert(std::make_pair(iL->first, iL->second));
		}
	}

	return extensions;
}

Manager::Names Manager::libraries() const
{
	const ScopedLock scopedLock(lock_);

	Names result;
	for (LibrarySet::const_iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		result.push_back((*i)->name());
	}

	return result;
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		(*i)->release();
		delete *i;
	}

	librarySet_.clear();
}

void Manager::registerLibrary(Library& library)
{
	const ScopedLock scopedLock(lock_);
	ocean_assert(librarySet_.find(&library) == librarySet_.end());

	librarySet_.insert(&library);
}

void Manager::registerMouseEventLibrary(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(mouseEventLibraries_.find(name) == mouseEventLibraries_.cend());

	mouseEventLibraries_.insert(name);
}

bool Manager::unregisterLibrary(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	for (LibrarySet::iterator i = librarySet_.begin(); i != librarySet_.end(); ++i)
	{
		if ((*i)->name() == name)
		{
			(*i)->release();
			delete *i;

			librarySet_.erase(i);

			return true;
		}
	}

	return true;
}

bool Manager::unregisterMouseEventLibrary(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	mouseEventLibraries_.erase(name);
	return true;
}

}

}
