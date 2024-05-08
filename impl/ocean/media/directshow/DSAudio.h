/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_AUDIO_H
#define META_OCEAN_MEDIA_DS_AUDIO_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSFiniteMedium.h"
#include "ocean/media/directshow/DSSoundMedium.h"

#include "ocean/media/Audio.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow audio object.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSAudio :
	public virtual DSFiniteMedium,
	public virtual DSSoundMedium,
	public virtual Audio
{
	friend class DSLibrary;

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
		explicit DSAudio(const std::string& url);

		/**
		 * Destructs a DSAudio object.
		 */
		~DSAudio() override;

		/**
		 * Builds the entire filter graph.
		 * @see DSGraphObject::buildGraph().
		 */
		bool buildGraph() override;

		/**
		 * Releases the entire filter graph.
		 * @see DSGraphObject::releaseGraph().
		 */
		void releaseGraph() override;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_AUDIO_H
