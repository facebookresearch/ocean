/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_SQUENCE_RECORDER_H
#define META_OCEAN_MEDIA_IMAGE_SQUENCE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/FileRecorder.h"
#include "ocean/media/FrameRecorder.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ImageSequenceRecorder;

/**
 * Definition of a object reference holding an image sequence recorder.
 * @see SmartObjectRef, ImageSequenceRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<ImageSequenceRecorder, Recorder> ImageSequenceRecorderRef;

/**
 * This class implements an image sequence recorder.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageSequenceRecorder :
	public FileRecorder,
	public FrameRecorder
{
	public:

		/**
		 * Definition of individual recorder modes.
		 */
		enum RecorderMode
		{
			/// Invalid recorder mode.
			RM_INVALID,
			/// Immediate mode for immediate image saving.
			RM_IMMEDIATE,
			/// Parallel mode for image saving in parallel
			RM_PARALLEL,
			/// Explicit mode for image saving due to an explicit invocation.
			RM_EXPLICIT
		};

	public:

		/**
		 * Returns the mode of this recorder.
		 * The default is RM_IMMEDIATE.
		 * @return Current recorder mode
		 * @see setMode().
		 */
		virtual RecorderMode mode() const;

		/**
		 * Returns the stat index with which the filename of the first frame starts.
		 * @return The start index of the first frame, the default is 0u
		 * @see setStartIndex().
		 */
		virtual unsigned int startIndex() const;

		/**
		 * Returns the number of currently buffered images.
		 * @return Number of images
		 */
		virtual unsigned int pendingImages() const = 0;

		/**
		 * Sets the mode of this recorder.
		 * @param mode Mode to be set
		 * @return True, if succeeded
		 * @see mode().
		 */
		virtual bool setMode(const RecorderMode mode);

		/**
		 * Sets the start index with which the filename of the first frame will start.
		 * @param index Start index
		 * @return True, if succeeded
		 * @see startIndex().
		 */
		virtual bool setStartIndex(const unsigned int index);

		/**
		 * Adds a given frame explicity.
		 * @param frame The frame to be added
		 * @return True, if succeeded
		 */
		virtual bool addImage(const Frame& frame);

		/**
		 * Explicitly invokes the image saving if the recorder is in RM_EXPLICIT mode.
		 * This function returns if all images have been saved.
		 * @return True, if all images could be saved successfully
		 */
		virtual bool forceSaving() = 0;

	protected:

		/**
		 * Creates a new image sequence recorder.
		 */
		ImageSequenceRecorder();

		/**
		 * Destructs an image sequence recorder.
		 */
		~ImageSequenceRecorder() override;

	protected:

		/// Mode of this image sequence recorder.
		RecorderMode recorderMode;

		/// Start index of the first frame.
		unsigned int recorderStartIndex;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_SQUENCE_RECORDER_H
