/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_RECORDER_H
#define META_OCEAN_MEDIA_RECORDER_H

#include "ocean/media/Media.h"

#include "ocean/base/ObjectRef.h"

#include <vector>

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Recorder;

/**
 * Definition of a object reference holding recorder objects.
 * @ingroup media
 */
typedef Ocean::ObjectRef<Recorder> RecorderRef;

/**
 * This class is the base class for all recorder.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Recorder
{
	friend class ObjectRef<Recorder>;

	public:

		/**
		 * Definition of different recorder types.
		 */
		enum Type
		{
			/// Simple recorder.
			RECORDER = 0,
			/// Frame recorder.
			FRAME_RECORDER = 1,
			/// Sound recorder.
			SOUND_RECORDER = 2,
			/// Explicit recorder.
			EXPLICIT_RECORDER = 4,
			/// Implicit recorder.
			IMPLICIT_RECORDER = 8,
			/// File recorder.
			FILE_RECORDER = 16 | EXPLICIT_RECORDER,
			/// Buffer recorder.
			BUFFER_RECORDER = 32 | EXPLICIT_RECORDER,
			/// Streaming recorder.
			STREAMING_RECORDER = 64 | IMPLICIT_RECORDER,
			/// Movie recorder.
			MOVIE_RECORDER = 128 | FRAME_RECORDER | FILE_RECORDER,
			/// Frame streaming recorder.
			FRAME_STREAMING_RECORDER = 256 | FRAME_RECORDER | STREAMING_RECORDER,
			/// Image recorder.
			IMAGE_RECORDER = 512 | FRAME_RECORDER | FILE_RECORDER,
			/// Buffer image recorder.
			BUFFER_IMAGE_RECORDER = 1024 | FRAME_RECORDER | BUFFER_RECORDER,
			/// Image sequence recorder.
			IMAGE_SEQUENCE_RECORDER = 2048 | FRAME_RECORDER | FILE_RECORDER,
			/// Frame memory recorder.
			FRAME_MEMORY_RECORDER = 4096 | FRAME_RECORDER
		};

		/**
		 * Definition of a vector holding encoder names.
		 */
		typedef std::vector<std::string> Encoders;

	public:

		/**
		 * Returns whether this recorder is currently recording.
		 * @return True, if so
		 */
		virtual bool isRecording() const = 0;

		/**
		 * Returns the type of this recorder.
		 * @return Recorder type
		 */
		inline Type type() const;

	protected:

		/**
		 * Creates a new recorder.
		 */
		Recorder();

		/**
		 * Destructs a recorder.
		 */
		virtual ~Recorder();

	protected:

		/// Type of this recorder.
		Type recorderType;

		/// Lock of this recorder.
		mutable Lock recorderLock;
};

inline Recorder::Type Recorder::type() const
{
	return recorderType;
}

}

}

#endif // META_OCEAN_MEDIA_RECORDER_H
