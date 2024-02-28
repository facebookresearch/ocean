// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/Audio.h"

namespace Ocean
{

namespace Media
{

Audio::Audio(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	SoundMedium(url)
{
	type_ = Type(type_ | AUDIO);
}

}

}
