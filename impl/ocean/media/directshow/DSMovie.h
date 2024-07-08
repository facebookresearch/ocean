/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_MOVIE_H
#define META_OCEAN_MEDIA_DS_MOVIE_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSFiniteMedium.h"
#include "ocean/media/directshow/DSFrameMedium.h"
#include "ocean/media/directshow/DSSoundMedium.h"

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow movie object.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSMovie :
	public virtual DSFiniteMedium,
	public virtual DSFrameMedium,
	public virtual DSSoundMedium,
	public virtual Movie
{
	friend class DSLibrary;

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
		 * Creates a new movie by a given url.
		 * @param url Url of the movie
		 */
		explicit DSMovie(const std::string& url);

		/**
		 * Destructs a DSMovie object.
		 */
		~DSMovie() override;

		/**
		 * @see DSGraphObject::buildGraph().
		 */
		bool buildGraph() override;

		/**
		 * @see DSGraphObject::releaseGraph().
		 */
		void releaseGraph() override;

	private:

		/// Sound use state.
		bool movieUseSound_ = true;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_MOVIE_H
