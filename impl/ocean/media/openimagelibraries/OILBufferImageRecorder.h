/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_RECORDER_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/media/BufferImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements a buffer image recorder for OpenImageLibraries.
 * This recorder can be used to save images to files.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT OILBufferImageRecorder : virtual public BufferImageRecorder
{
	friend class OILLibrary;

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
		bool buffer(std::vector<unsigned char>& data) const override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @see FrameRecorder::frameEncoders().
		 */
		Encoders frameEncoders() const override;

		/**
		 * Locks the most recent frame and returns it so that the image data can be written to this frame.
		 * @see FrameRecorder::lockBufferToFill().
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
		OILBufferImageRecorder();

		/**
		 * Destructs a buffer image recorder object.
		 */
		~OILBufferImageRecorder() override;

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;

		/// Internal image buffer if the image is not saved explicitly but by a buffer request.
		std::vector<unsigned char> recorderBuffer;
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_BUFFER_IMAGE_RECORDER_H
