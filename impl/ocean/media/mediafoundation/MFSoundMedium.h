/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_SOUND_MEDIUM_H
#define META_OCEAN_MEDIA_MF_SOUND_MEDIUM_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFMedium.h"

#include "ocean/math/Numeric.h"

#include "ocean/media/SoundMedium.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class is the base class for all MediaFoundation sound mediums.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFSoundMedium :
	public virtual MFMedium,
	public virtual SoundMedium
{
	public:

		/**
		 * Returns the volume of the sound in db.
		 * @see SoundMedium::soundVolume().
		 */
		float soundVolume() const override;

		/**
		 * Returns whether the sound medium is in a mute state.
		 * @see SoundMedium::soundMute().
		 */
		bool soundMute() const override;

		/**
		 * Sets the volume of the sound in db.
		 * @see SoundMedium::setSoundVolume().
		 */
		bool setSoundVolume(const float volume) override;

		/**
		 * Sets or un-sets the sound medium to a mute state.
		 * @see SoundMedium::setSoundMute().
		 */
		bool setSoundMute(const bool mute) override;

		/**
		 * Extracts the sound format of a given Media Foundation media type.
		 * @param mediaType Media Foundation media type
		 * @param soundType Resulting sound type
		 * @return True, if succeeded
		 */
		static bool extractSoundFormat(IMFMediaType* mediaType, SoundType& soundType);

	protected:

		/**
		 * Creates a new sound medium by a given URL.
		 * @param url The URL of the sound medium
		 */
		explicit MFSoundMedium(const std::string& url);

		/**
		 * Destructs a MFSoundMedium object.
		 */
		~MFSoundMedium() override;

		/**
		 * Builds the sound topology of this object.
		 * @return True, if succeeded
		 */
		virtual bool buildSoundTopology();

		/**
		 * Releases the sound topology of this object.
		 */
		virtual void releaseSoundTopology();

	protected:

		/// The sound volume before mute state.
		float nonMuteVolume_ = NumericF::minValue();
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_SOUND_MEDIUM_H
