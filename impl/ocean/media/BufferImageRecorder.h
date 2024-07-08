/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_BUFFER_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_BUFFER_IMAGE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameRecorder.h"
#include "ocean/media/BufferRecorder.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class BufferImageRecorder;

/**
 * Definition of a object reference holding an buffer recorder.
 * @see SmartObjectRef, BufferRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<BufferImageRecorder, Recorder> BufferImageRecorderRef;

/**
 * This class is the base class for all buffer recorders.
 * A buffer recorder can store (compressed) images in memory buffers.<br>
 * The recorder supports two different save modes: An explicit mode and an implicit one.<br>
 * The explicit mode takes a frame and a memory buffer and saves the image directly.<br>
 * Use the saveImage() function for the explicit mode.<br>
 * The implicit mode provides an internal buffer to be filled. After the internal buffer is filled the image memory buffer is stored.<br>
 * Use the lockBufferToFill() and unlockBufferToFill() functions for the implicit mode.<br>
 * @see saveImage(), lockBufferToFill(), unlockBuffertoFill().
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT BufferImageRecorder :
	virtual public FrameRecorder,
	virtual public BufferRecorder
{
	public:

		/**
		 * Encodes a given frame into a memory buffer.
		 * @param frame The frame to be saved, must be valid
		 * @param imageType The file extension of the image to be created (e.g. bmp, jpg, png, ...)
		 * @param buffer Resulting memory buffer which will be extended so that already existing data stays untouched
		 * @return True, if succeeded
		 * @see releaseBuffer().
		 */
		virtual bool saveImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer) = 0;

		/**
		 * Saves an image the next time a requested framebuffer is filled with valid image data.
		 * @return True, if at this point no image save process is pending
		 * @see lockBufferToFill().
		 */
		bool start() override;

		/**
		 * Stops the recorder, this functions is unused for image recorders.
		 * @return False always
		 * @see BufferRecorder::stop().
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
		 * Creates a new buffer image recorder object.
		 */
		BufferImageRecorder();

		/**
		 * Destructs a buffer image recorder object.
		 */
		~BufferImageRecorder() override;

	protected:

		/// State determining whether the buffer recorder is asked to save a new image.
		bool recorderSaveImage;
};

}

}

#endif // META_OCEAN_MEDIA_BUFFER_IMAGE_RECORDER_H
