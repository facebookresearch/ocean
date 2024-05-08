/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_SOUND_MEDIUM_H
#define META_OCEAN_MEDIA_DS_SOUND_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSMediaType.h"
#include "ocean/media/directshow/DSMedium.h"

#include "ocean/math/Numeric.h"

#include "ocean/media/SoundMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all DirectShow sound mediums.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSSoundMedium :
	public virtual DSMedium,
	public virtual SoundMedium
{
	protected:

		/**
		 * Class allowing the sorting of several audio types according their popularity.
		 */
		class DSSortableSoundType : public SortableSoundType
		{
			public:

				/**
				 * Creates a new sortable sound type.
				 * The given sound type has to be freed after the entire sorting process by the caller.
				 * @param dsMediaType The DirectShow media type
				 * @param soundType Preferable sound type
				 */
				DSSortableSoundType(DSMediaType&& dsMediaType, const SoundType& soundType);

				/**
				 * Returns the DirectShow media type.
				 * @return Media type
				 */
				const AM_MEDIA_TYPE& type() const;

			protected:

				/// DirectShow media type
				DSMediaType dsMediaType_;
		};

		/**
		 * Definition of a vector holding sortable sound type objects.
		 */
		typedef std::vector<DSSortableSoundType> DSSortableSoundTypes;

		/**
		 * Definition of a vector holding filters.
		 */
		typedef std::vector<ScopedIBaseFilter> Filters;

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
		 * Extracts the sound format of a given DirectShow media type.
		 * @param mediaType DirectShow media type
		 * @param soundType Resulting sound type
		 * @return True, if succeeded
		 */
		static bool extractSoundFormat(const AM_MEDIA_TYPE& mediaType, SoundType& soundType);

	protected:

		/**
		 * Creates a new sound medium by a given url.
		 * @param url Url of the sound medium
		 */
		explicit DSSoundMedium(const std::string& url);

		/**
		 * Destructs a DSSoundMedium object.
		 */
		~DSSoundMedium() override;

		/**
		 * Creates the sound interface.
		 * @return True, if succeeded
		 */
		bool createSoundInterface();

		/**
		 * Initializes the audio interface.
		 * @param filter Audio renderer filter
		 * @return True, if succeeded
		 */
		bool initializeSoundInterface(IBaseFilter* filter);

		/**
		 * Releases the audio interface.
		 */
		void releaseSoundInterface();

		/**
		 * Removes the video branch from the filter graph.
		 * @return True, if succeeded
		 */
		bool removeVideoBranch();

		/**
		 * Removes the sound branch from the filter graph.
		 * @return True, if succeeded
		 */
		bool removeSoundBranch();

	protected:

		/// DirectShow basic audio interface.
		ScopedIBasicAudio soundBasicAudioInterface_;

		/// DirectShow null renderer for video suppression.
		ScopedIBaseFilter soundVideoSuppressionNullRendererFilter_;

		/// DirectShow null renderer for video suppression.
		ScopedIBaseFilter soundVideoSuppressionNullRendererFilter2_;

		/// Sound volume before mute state.
		float soundNonMuteVolume_ = NumericF::minValue();
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_SOUND_MEDIUM_H
