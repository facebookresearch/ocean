/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MOVIE_RECORDER_H
#define META_OCEAN_MEDIA_MOVIE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/FileRecorder.h"
#include "ocean/media/FrameRecorder.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class MovieRecorder;

/**
 * Definition of a object reference holding a movie recorder.
 * @see SmartObjectRef, MovieRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<MovieRecorder, Recorder> MovieRecorderRef;

/**
 * This class is the base class for all movie recorder.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT MovieRecorder :
	virtual public FileRecorder,
	virtual public FrameRecorder
{
	protected:

		/**
		 * Creates a new movie recorder.
		 */
		MovieRecorder();

		/**
		 * Destructs a movie recorder.
		 */
		~MovieRecorder() override;
};

}

}

#endif // META_OCEAN_MEDIA_MOVIE_RECORDER_H
