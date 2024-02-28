// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/interaction/experiences/Experience.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

bool Experience::load(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	// can be implemented in derived class

	return false;
}

bool Experience::unload(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived class

	return false;
}

Timestamp Experience::preUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	// can be implemented in derived class

	return timestamp;
}

void Experience::postUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Rendering::ViewRef& /*view*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived class
}

void Experience::onMousePress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived class
}

void Experience::onMouseMove(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived class
}

void Experience::onMouseRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived class
}

void Experience::onKeyPress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*key*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

void Experience::onKeyRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*key*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes
}

bool Experience::preLoad(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	// can be implemented in derived classes

	return true;
}

bool Experience::postUnload(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Timestamp /*timestamp*/)
{
	// can be implemented in derived classes

	return true;
}

void Experience::release()
{
	// can be implemented in derived class
}

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean
