/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_RECORDER_H

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/media/BufferImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements a buffer image recorder for ImageIO.
 * This recorder can be used to save images to files.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOBufferImageRecorder :
	virtual public IIOObject,
	virtual public BufferImageRecorder
{
	friend class IIOLibrary;

	public:

		/**
		 * Saves a given frame as file explicity.
		 * @see BufferRecorder::saveImage().
		 */
		bool saveImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer) override;

		/**
		 * Returns the buffer of the most recently saved image.
		 * @see BufferRecorder::buffer().
		 */
		bool buffer(std::vector<uint8_t>& data) const override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @see FrameRecorder::frameEncoders().
		 */
		Encoders frameEncoders() const override;

		/**
		 * Returns a pointer to a framebuffer to be sfilled if a image save task has been asked before.
		 * @see BufferImageRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * @see BufferImageRecorder::unlockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new buffer image recorder object.
		 */
		IIOBufferImageRecorder();

		/**
		 * Destructs a buffer image recorder object.
		 */
		~IIOBufferImageRecorder() override;

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;

		/// Internal image buffer if the image is not saved explicitly but by a buffer request.
		std::vector<uint8_t> recorderBuffer;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_BUFFER_IMAGE_RECORDER_H
