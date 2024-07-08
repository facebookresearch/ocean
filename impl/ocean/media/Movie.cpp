/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
