/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_AUDIO_H
#define META_OCEAN_MEDIA_MF_AUDIO_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFFiniteMedium.h"
#include "ocean/media/mediafoundation/MFSoundMedium.h"

#include "ocean/media/Audio.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements a Media Foundation audio object.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFAudio :
	public virtual MFFiniteMedium,
	public virtual MFSoundMedium,
	public virtual Audio
{
	friend class MFLibrary;

	public:

		/**
		 * Clones this medium and returns a new independent instance of this medium.
		 * @see Medium::clone().
		 */
		MediumRef clone() const override;

	protected:

		/**
		 * Creates a new audio by a given url.
		 * @param url The URL of the audio
		 */
		explicit MFAudio(const std::string& url);

		/**
		 * Destructs a MFAudio object.
		 */
		~MFAudio() override;

		/**
		 * Creates and builds the topology of this object.
		 * @see MFMedium::createTopology().
		 */
		bool createTopology(const bool respectPlaybackTime) override;

		/**
		 * Releases the topology.
		 */
		void releaseTopology() override;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_AUDIO_H
