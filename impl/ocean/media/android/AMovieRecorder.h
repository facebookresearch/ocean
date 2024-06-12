/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_A_MOVIE_RECORDER_H
#define META_OCEAN_MEDIA_A_MOVIE_RECORDER_H

#include "ocean/media/android/Android.h"
#include "ocean/media/android/NativeMediaLibrary.h"

#include "ocean/base/ScopedObject.h"

#include "ocean/media/MovieRecorder.h"

#include <cstdio>

namespace Ocean
{

namespace Media
{

namespace Android
{

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21

/**
 * This class implements a movie recorder class for Android.
 * NOTE: The supported pixel formats for recording may vary from device to device.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT AMovieRecorder : virtual public MovieRecorder
{
	friend class ALibrary;

	protected:

		/// Bitrate at which videos are saved.
		static constexpr int DEFAULT_BITRATE = 2000000; // 2 Mbps

		/// Frequency at which I-frames are saved to the output.
		static constexpr int DEFAULT_IFRAME_INTERVAL_SECONDS = 1;

		/**
		 * Definition of a scoped object for FILE pointers.
		 */
		using ScopedFILE = ScopedObjectCompileTimeT<FILE*, FILE*, int, fclose>;

	public:

		/**
		 * Sets the filename of this recorder.
		 * The filename must be set before recordings starts.
		 * @see MovieRecorder::setFilename().
		 */
		bool setFilename(const std::string& filename) override;

		/**
		 * Sets the preferred frame type of this recorder.
		 * @see FrameRecorder::setPreferredFrameType().
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
		 * Returns a pointer to the most recent buffer to be filled immediately and locks it.
		 * Beware: Call unlockBufferToFill() once the image data is written to the frame.
		 * @param recorderFrame The resulting frame in which the image data can be copied, the frame type of this frame must not be changed
		 * @param respectFrameFrequency Flag determining that a buffer will be returned if it is time for a new frame only
		 * @return True if the buffer was successfully locked, otherwise false
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * Beware: The buffer has to be locked by lockBufferToFill() before.
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new movie recorder object.
		 */
		AMovieRecorder();

		/**
		 * Destroys a movie recorder object.
		 */
		~AMovieRecorder() override;

		/**
		 * Creates a new media muxer and all associated resources.
		 * @return True, if succeeded
		 */
		bool createNewMediaCodec();

		/**
		 * Releases all resources.
		 */
		void release();

	protected:

		/**
		 * Reads output from the codec and writes the resulting buffer to the muxer. This function should only be called if input data was previously submitted to the codec.
		 * @param loopUntilEndOfStream If true, continuously read from the buffer until an end-of-stream flag is presented; this should only be used when the recording is complete and a buffer with the EOS flag set has been submitted as input to the codec
		 * @return True if the read and write succeeded, false otherwise.
		 */
		bool readCodecOutputBufferAndWriteToMuxer(const bool loopUntilEndOfStream = false);

	protected:

		/// The underlying media format
		AMediaFormat* mediaFormat_ = nullptr;

		/// The underlying media codec that takes frame data as input and outputs encoded video frames.
		AMediaCodec* mediaCodec_ = nullptr;

		/// The underlying media muxer that will save the result to a file containing the codec output.
		AMediaMuxer* mediaMuxer_ = nullptr;

		/// The underlying file being written to.
		ScopedFILE file_;

		/// Current input buffer index of the codec, valid (>= 0) when the buffer is locked.
		ssize_t bufferIndex_ = -1;

		/// Current size of the input buffer for the codec, valid (> 0) when the buffer is locked.
		size_t bufferSize_ = 0;

		/// Output track for the muxer.
		ssize_t trackIndex_ = -1;

		/// The timestamp of the next frame.
		double nextFrameTimestamp_ = 0.0;

		/// True, if the recorder is actively recording frames; false, otherwise.
		bool isRecording_ = false;

		/// True, if this recorder is stopped.
		bool isStopped_ = false;
};

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 21

}

}

}

#endif // META_OCEAN_MEDIA_A_MOVIE_RECORDER_H
