/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_IMAGE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameRecorder.h"
#include "ocean/media/FileRecorder.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ImageRecorder;

/**
 * Definition of a object reference holding an image recorder.
 * @see SmartObjectRef, ImageRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<ImageRecorder, Recorder> ImageRecorderRef;

/**
 * This class is the base class for all image recorders.
 * An image recorder can save simple image files.<br>
 * The recorder supports two different save modes: An explicit mode and an implicit one.<br>
 * The explicit mode takes a frame and a filename and saves the image directly.<br>
 * Use the saveImage() function for the explicit mode.<br>
 * The implicit mode provides an internal buffer to be filled. After the internal buffer is filled the image file is saved.<br>
 * Use the lockBufferToFill() and unlockBufferToFill() functions for the implicit mode.<br>
 * @see saveImage(), lockBufferToFill(), unlockBuffertoFill().
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageRecorder :
	virtual public FrameRecorder,
	virtual public FileRecorder
{
	public:

		/**
		 * Saves a given frame as file explicity.
		 * @param frame The frame to be saved
		 * @param filename Name of the resulting image file
		 * @return True, if succeeded
		 */
		virtual bool saveImage(const Frame& frame, const std::string& filename) = 0;

		/**
		 * Saves an image the next time a requested framebuffer is filled with valid image data.
		 * @return True, if at this point no image save process is pending
		 * @see lockBufferToFill().
		 */
		bool start() override;

		/**
		 * Stops the recorder, this functions is unused for image recorders.
		 * @return False always
		 * @see FileRecorder::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether this recorder will save an image the next time a framebuffer is requested.
		 * @return True, if so
		 */
		bool isRecording() const override;

		/**
		 * Returns a pointer to a framebuffer to be filled if a image save task has been requested before.
		 * @return True, if so
		 * @see start(), FrameRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * @see lockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new image recorder object.
		 */
		ImageRecorder();

		/**
		 * Destructs an image recorder object.
		 */
		~ImageRecorder() override;

	protected:

		/// State determining whether the file recorder is asked to save a new image.
		bool recorderSaveImage;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_RECORDER_H
