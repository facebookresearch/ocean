/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/empty/EPYLibrary.h"

#include "ocean/base/Exception.h"

namespace Ocean
{

namespace Interaction
{

namespace Empty
{

EPYLibrary::EPYLibrary() :
	Library("Empty")
{
	// registry any file type if necessary for this interaction plugin
	//registerFileExtension("fileextension", "File extension description");
}

EPYLibrary::~EPYLibrary()
{
	// don't forget to clean up all the data associated with this interaction library
}

void EPYLibrary::registerLibrary()
{
	EPYLibrary* library = new EPYLibrary();
	if (library == nullptr)
	{
		throw OutOfMemoryException("Not enough memory to create a new interaction library.");
	}

	if (Library::registerFactory(*library) == false)
	{
		ocean_assert(false && "The library has been registered before!");
		delete library;
	}
}

bool EPYLibrary::unregisterLibrary()
{
	return Library::unregisterLibrary("Empty");
}

bool EPYLibrary::load(const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	/*
	if (filename.empty())
		return false;

	ScopedLock scopedLock(lock_);

	// do the file handling here
	*/

	return false;
}

bool EPYLibrary::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	/*
	if (filename.empty())
		return false;

	ScopedLock scopedLock(lock_);

	// do the file handling here
	*/

	return false;
}

void EPYLibrary::unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	/*
	ScopedLock scopedLock(lock_);

	// unload all loaded files (if any)
	*/
}

void EPYLibrary::preFileLoad(const UserInterface& userInterface, const std::string& filename)
{
	/*
	ScopedLock scopedLock(lock_);

	// handle pre file load events
	*/
}

void EPYLibrary::postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded)
{
	/*
	ScopedLock scopedLock(lock_);

	// handle post file load events
	*/
}

Timestamp EPYLibrary::preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	/*
	ScopedLock scopedLock(lock_);

	Timestamp changingTimestamp = timestamp;

	// handle pre update events

	return changingTimestamp;
	*/

	return timestamp;
}

void EPYLibrary::postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	/*
	ScopedLock scopedLock(lock_);

	// handle post update events
	*/
}

void EPYLibrary::onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(key.length() != 0);

	/*
	ScopedLock scopedLock(lock_);

	// handle on key events

	Log::info() << key;
	*/
}

void EPYLibrary::onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(key.length() != 0);

	/*
	ScopedLock scopedLock(lock_);

	// handle on key events

	Log::info() << key;
	*/
}

}

}

}
