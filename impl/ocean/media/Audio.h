/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AUDIO_H
#define META_OCEAN_MEDIA_AUDIO_H

#include "ocean/media/Media.h"
#include "ocean/media/FiniteMedium.h"
#include "ocean/media/SoundMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Audio;

/**
 * Definition of a smart medium reference holding a audio object.
 * @see SmartMediumRef, Audio.
 * @ingroup media
 */
typedef SmartMediumRef<Audio> AudioRef;

/**
 * This class is the base class for all audio mediums.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Audio :
	public virtual FiniteMedium,
	public virtual SoundMedium
{
	protected:

		/**
		 * Creates a new audio by a given url.
		 * @param url Url of the audio
		 */
		explicit Audio(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_AUDIO_H
