/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_MOVIE_RECORDER_H
#define META_OCEAN_MEDIA_DS_MOVIE_RECORDER_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSFileRecorder.h"
#include "ocean/media/directshow/DSFrameRecorder.h"

#include "ocean/media/MovieRecorder.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow movie recorder.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSMovieRecorder :
	virtual public DSFileRecorder,
	virtual public DSFrameRecorder,
	virtual public MovieRecorder
{
	friend class DSLibrary;

	protected:

		/**
		 * Creates a new DirectShow movie recorder.
		 */
		DSMovieRecorder();

		/**
		 * Destructs a DirectShow movie recorder.
		 */
		~DSMovieRecorder() override;

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

#endif // META_OCEAN_MEDIA_DS_MOVIE_RECORDER_H
