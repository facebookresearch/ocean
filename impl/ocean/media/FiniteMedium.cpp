// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/FiniteMedium.h"

namespace Ocean
{

namespace Media
{

FiniteMedium::FiniteMedium(const std::string& url) :
	Medium(url)
{
	type_ = Type(type_ | FINITE_MEDIUM);
}

bool FiniteMedium::setPosition(const double /*position*/)
{
	// must be implemented in a derived class

	return false;
}

bool FiniteMedium::setSpeed(const float /*speed*/)
{
	// must be implemented in a derived class

	return false;
}

bool FiniteMedium::setLoop(const bool value)
{
	const ScopedLock scopedLock(lock_);

	loop_ = value;
	return true;
}

bool FiniteMedium::taskFinished() const
{
	return !isStarted();
}

}

}
