/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/Medium.h"
#include "ocean/media/MediumRef.h"

namespace Ocean
{

namespace Media
{

Medium::Medium(const std::string& url) :
	url_(url)
{
	// nothing to do here
}

Medium::~Medium()
{
	// nothing to do here
}

bool Medium::isType(const Type type) const
{
	return (type_ & type) == type;
}

bool Medium::isExclusive() const
{
	return !MediumRefManager::get().isRegistered(this);
}

MediumRef Medium::clone() const
{
	return MediumRef();
}

std::string Medium::convertType(const Type type)
{
	switch (type)
	{
		case MEDIUM:
			return "Medium";

		case FRAME_MEDIUM:
			return "FrameMedium";

		case SOUND_MEDIUM:
			return "SoundMedium";

		case FINITE_MEDIUM:
			return "FiniteMedium";

		case LIVE_MEDIUM:
			return "LiveMedium";

		case CONFIG_MEDIUM:
			return "ConfigMedium";

		case AUDIO:
			return "Audio";

		case IMAGE:
			return "Image";

		case LIVE_AUDIO:
			return "LiveAudio";

		case LIVE_VIDEO:
			return "LiveVideo";

		case MOVIE:
			return "Movie";

		default:
			break;
	}

	return "Medium";
}

Medium::Type Medium::convertType(const std::string& type)
{
	if (type == "FrameMedium")
		return FRAME_MEDIUM;
	else if (type == "SoundMedium")
		return SOUND_MEDIUM;
	else if (type == "FiniteMedium")
		return FINITE_MEDIUM;
	else if (type == "LiveMedium")
		return LIVE_MEDIUM;
	else if (type == "ConfigMedium")
		return CONFIG_MEDIUM;
	else if (type == "Audio")
		return AUDIO;
	else if (type == "Image")
		return IMAGE;
	else if (type == "LiveAudio")
		return LIVE_AUDIO;
	else if (type == "LiveVideo")
		return LIVE_VIDEO;
	else if (type == "Movie")
		return MOVIE;

	return MEDIUM;
}

}

}
