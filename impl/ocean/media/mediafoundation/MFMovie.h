/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_MOVIE_H
#define META_OCEAN_MEDIA_MF_MOVIE_H

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFFiniteMedium.h"
#include "ocean/media/mediafoundation/MFFrameMedium.h"
#include "ocean/media/mediafoundation/MFSoundMedium.h"

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements a MediaFoundation movie object.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFMovie :
	public virtual MFFiniteMedium,
	public virtual MFFrameMedium,
	public virtual MFSoundMedium,
	public virtual Movie
{
	friend class MFLibrary;

	public:

		/**
		 * Enables or disables the audio in this movie (has no effect if the movie does not have audio).
		 * @see Movie::setUseSound().
		 */
		bool setUseSound(const bool state) override;

		/**
		 * Returns whether the sound of this movie is enabled or disabled.
		 * @see Movie::useSound().
		 */
		bool useSound() const override;

		/**
		 * Clones this medium and returns a new independent instance of this medium.
		 * @see Medium::clone().
		 */
		MediumRef clone() const override;

	protected:

		/**
		 * Creates a new movie by a given URL.
		 * @param url The URL of the movie
		 */
		explicit MFMovie(const std::string& url);

		/**
		 * Destructs a MFMovie object.
		 */
		~MFMovie() override;

		/**
		 * Creates and builds the topology of this object.
		 * @see MFMedium::createTopology().
		 */
		bool createTopology(const bool respectPlaybackTime) override;

		/**
		 * Releases the topology.
		 */
		void releaseTopology() override;

	private:

		/// True, if the movie uses sound.
		bool useSound_ = true;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_MOVIE_H
