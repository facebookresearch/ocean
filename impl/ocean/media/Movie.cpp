// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Media
{

Movie::Movie(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	SoundMedium(url)
{
	type_ = Type(type_ | MOVIE);
}

bool Movie::setUseSound(const bool /*state*/)
{
	// must be implemented in a derived class

	return false;
}

bool Movie::useSound() const
{
	return true;
}

}

}
