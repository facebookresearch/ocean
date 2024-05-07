// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_MEDIA_DS_LIVE_AUDIO_H
#define META_OCEAN_MEDIA_DS_LIVE_AUDIO_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSLiveMedium.h"
#include "ocean/media/directshow/DSSoundMedium.h"

#include "ocean/media/LiveAudio.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow live audio medium.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSLiveAudio :
	public virtual DSLiveMedium,
	public virtual DSSoundMedium,
	public virtual LiveAudio
{
	friend class DSLibrary;

	protected:

		/**
		 * Creates a new live audio by a given url.
		 * @param url Url of the live audio
		 */
		explicit DSLiveAudio(const std::string& url);

		/**
		 * Destructs a live audio object.
		 */
		~DSLiveAudio() override;

		/**
		 * @see DSGraphObject::buildGraph().
		 */
		bool buildGraph() override;

		/**
		 * @see DSGraphObject::releaseGraph().
		 */
		void releaseGraph() override;

		/**
		 * Creates the DirectShow audio source filter.
		 * @return True, if succeeded
		 */
		bool createAudioSourceFilter();

		/**
		 * Collect the preferred sound formats.
		 * @param sortableSoundTypes Resulting sound types sorted by their popularity
		 * @return True, if succeeded
		 */
		bool collectPreferredAudioFormats(DSSortableSoundTypes& sortableSoundTypes);

		/**
		 * Releases the DirectShow audio source filter.
		 */
		void releaseAudioSourceFilter();

		/**
		 * Starts a configuration possibility.
		 * @see ConfigMedium::configuration().
		 */
		bool configuration(const std::string& name, long long data) override;

		/**
		 * Returns the names of the configuration possibilities.
		 * @see ConfigMedium::configs().
		 */
		ConfigNames configs() const override;

	protected:

		/// DirectShow audio source filter.
		ScopedIBaseFilter audioSourceFilter_;

		/// DirectShow stream config interface.
		ScopedIAMStreamConfig audioSourceFilterStreamConfigInterface_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_LIVE_AUDIO_H
