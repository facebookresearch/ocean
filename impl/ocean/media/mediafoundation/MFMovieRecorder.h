/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MF_MOVIE_RECORDER_H
#define META_OCEAN_MEDIA_MF_MOVIE_RECORDER_H

#include "ocean/media/mediafoundation/MediaFoundation.h"

#include "ocean/media/MovieRecorder.h"

#include <mfreadwrite.h>

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

/**
 * This class implements a MediaFoundation movie recorder object.
 * @ingroup mediamf
 */
class OCEAN_MEDIA_MF_EXPORT MFMovieRecorder : public virtual MovieRecorder
{
	friend class MFLibrary;

	public:

		/**
		 * Definition of a scoped object holding a IMFSinkWriter object.
		 * The wrapped IMFSinkWriter object will be released automatically once the scoped object does not exist anymore.
		 */
		typedef ScopedMediaFoundationObject<IMFSinkWriter> ScopedIMFSinkWriter;

	public:

		/**
		 * Sets the filename of this recorder.
		 * The filename must be set before recordings starts.
		 * @see MovieRecorder::setFilename().
		 */
		bool setFilename(const std::string& filename) override;

		/**
		 * Sets the preferred frame type of this recorder.
		 * @see MovieRecorder::setPreferredFrameType().
		 */
		bool setPreferredFrameType(const FrameType& type) override;

		/**
		 * Sets the recorder.
		 * @see ExplicitRecorder::start().
		 */
		bool start() override;

		/**
		 * Stops the recorder.
		 * @see ExplicitRecorder::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether this recorder is currently recording.
		 * @return True, if so
		 */
		bool isRecording() const override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @return Encoder names
		 */
		Encoders frameEncoders() const override;

		/**
		 * Locks the most recent frame and returns it so that the image data can be written to this frame.
		 * @see FrameRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * Beware: The buffer has to be locked by lockBufferToFill() before.
		 */
		void unlockBufferToFill() override;

		/**
		 * Translate an encoder name to the corresponding video format.
		 * @param encoder The name of the encoder, must be valid
		 * @param videoFormat The resulting corresponding encoder type
		 * @return True, if succeeded
		 */
		static bool encoderToVideoFormat(const std::string& encoder, GUID& videoFormat);

	protected:

		/**
		 * Creates a new movie recorder object.
		 */
		MFMovieRecorder();

		/**
		 * Destructs a movie recorder object.
		 */
		~MFMovieRecorder() override;

		/**
		 * Creates the sink writer writing the movie.
		 * @return True, if succeeded
		 */
		bool createSinkWriter();

		/**
		 * Releases the wink writer.
		 */
		void releaseSinkWriter();

	protected:

		/// The sink writer to be used.
		ScopedIMFSinkWriter sinkWriter_;

		/// The index of the stream which is used.
		DWORD streamIndex_ = DWORD(-1);

		/// The media buffer in which the current frame will be copied.
		ScopedIMFMediaBuffer mediaBuffer_;

		/// The timestamp of the next frame.
		double nextFrameTimestamp_ = 0.0;

		/// True, if the recorder is currently actively recording.
		bool isRecording_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_MF_MOVIE_RECORDER_H
