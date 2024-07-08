/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/Library.h"
#include "ocean/interaction/Manager.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

Library::Library(const std::string& name) :
	libraryName(name)
{
	// nothing to do here
}

Library::~Library()
{
	// nothing to do here
}

bool Library::isFileExtensionSupported(const std::string& extension) const
{
	return libraryFileExtensions.find(String::toLower(extension)) != libraryFileExtensions.end();
}

void Library::preFileLoad(const UserInterface& /*userInterface*/, const std::string& /*filename*/)
{
	// can be implemented in derived classes
}

void Library::postFileLoad(const UserInterface& /*userInterface*/, const std::string& /*filename*/, const bool /*succeeded*/)
{
	// can be implemented in derived classes
}

Timestamp Library::preUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	// can be implemented in derived classes
	return timestamp;
}

void Library::postUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Rendering::ViewRef& /*view*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::onMousePress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::onMouseMove(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::onMouseRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Rendering::ObjectId /*pickedObject*/, const Vector3& /*pickedPosition*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::onKeyPress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*key*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::onKeyRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*key*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Library::release()
{
	// nothing to do here
}

void Library::registerFileExtension(const std::string& extension, const std::string& description)
{
	ocean_assert(libraryFileExtensions.find(extension) == libraryFileExtensions.end());
	libraryFileExtensions.insert(std::make_pair(String::toLower(extension), description));
}

bool Library::registerFactory(Library& library)
{
	Manager::get().registerLibrary(library);
	return true;
}

bool Library::unregisterLibrary(const std::string& library)
{
	return Manager::get().unregisterLibrary(library);
}

}

}
