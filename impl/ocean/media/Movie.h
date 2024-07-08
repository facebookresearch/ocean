/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MOVIE_H
#define META_OCEAN_MEDIA_MOVIE_H

#include "ocean/media/Media.h"
#include "ocean/media/FiniteMedium.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/SoundMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Movie;

/**
 * Definition of a smart medium reference holding a movie object.
 * @see SmartMediumRef, Movie.
 * @ingroup media
 */
typedef SmartMediumRef<Movie> MovieRef;

/**
 * This class is the base class for all movies.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Movie :
	public virtual FiniteMedium,
	public virtual FrameMedium,
	public virtual SoundMedium
{
	public:

		/**
		 * Enables or disables the audio in this movie (has no effect if the movie does not have audio).
		 * The selection has to be applied once before the medium has been started for the first time.
		 * @param state True, to enable the audio in this movie, false otherwise
		 * @return True, if succeeded
		 */
		virtual bool setUseSound(const bool state);

		/**
		 * Returns whether the sound of this movie is enabled or disabled.
		 * @return True, if the audio of this movie is enabled
		 */
		virtual bool useSound() const;

	protected:

		/**
		 * Creates a new movie by a given url.
		 * @param url Url of the movie
		 */
		explicit Movie(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_MOVIE_H
