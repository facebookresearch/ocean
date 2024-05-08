/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_SEQUENCE_RECORDER_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_SEQUENCE_RECORDER_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
#include "ocean/media/openimagelibraries/OILImageRecorder.h"

#include "ocean/media/ImageSequenceRecorder.h"

#include "ocean/base/Thread.h"

#include <queue>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements an OpenImageLibraries image sequence recorder.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT OILImageSequenceRecorder :
	public ImageSequenceRecorder,
	public Thread
{
	friend class OILLibrary;

	protected:

		/**
		 * Definition of a queue holding frame references and frame indices.
		 */
		typedef std::queue<std::pair<FrameRef, unsigned int>> FrameQueue;

	public:

		/**
		 * Returns the mode of this recorder.
		 * @see ImageSequenceRecorder::mode().
		 */
		RecorderMode mode() const override;

		/**
		 * Returns the number of currently buffered images.
		 * @see ImageSequenceRecorder::pendingImages().
		 */
		unsigned int pendingImages() const override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @see FrameRecorder::frameEncoders().
		 */
		Encoders frameEncoders() const override;

		/**
		 * Sets the mode of this recorder.
		 * @see ImageSequenceRecorder::setMode().
		 */
		bool setMode(const RecorderMode mode) override;

		/**
		 * Sets the start index with witch the filename of the first frame will start.
		 * @see ImageSequenceRecorder::setStartIndex().
		 */
		bool setStartIndex(const unsigned int index) override;

		/**
		 * Adds a given frame explicity.
		 * @see ImageSequenceRecorder::addImage().
		 */
		bool addImage(const Frame& frame) override;

		/**
		 * Sets the recorder.
		 * @see FileRecorder::start().
		 */
		bool start() override;

		/**
		 * Stops the recorder.
		 * @see FileRecorder::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether this recorder is currently recording.
		 * @see Recorder::isRecording().
		 */
		bool isRecording() const override;

		/**
		 * Explicitly invokes the image saving if the recorder is in RM_EXPLICIT mode.
		 * @see ImageSequenceRecorder::invokeSaving().
		 */
		bool forceSaving() override;

		/**
		 * Returns a pointer to the most recent buffer to be filled immediately and locks it.
		 * @see FrameRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * @see FrameBuffer::unlockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new image sequence recorder object.
		 */
		OILImageSequenceRecorder();

		/**
		 * Destructs an image sequence recorder object.
		 */
		~OILImageSequenceRecorder() override;

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// Recorder for single frames.
		OILImageRecorder imageRecorder_;

		/// Queue holding the frames to be saved.
		FrameQueue frameQueue_;

		/// Frame counter.
		unsigned int frameCounter_ = 0u;

		/// Start timestamp.
		Timestamp startTimestamp_;

		/// Intermediate frame receiving the frame data.
		Frame frame_;

		/// State determining whether the recorder is currently recording.
		bool isRecording_ = false;

		/// Frame queue lock.
		mutable Lock frameQueueLock_;
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_SEQUENCE_RECORDER_H
