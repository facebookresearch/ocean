/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
