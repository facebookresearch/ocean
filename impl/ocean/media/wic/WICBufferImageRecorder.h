/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_RECORDER_H

#include "ocean/media/wic/WIC.h"
#include "ocean/media/wic/WICObject.h"

#include "ocean/media/BufferImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * This class implements a buffer image recorder for WIC.
 * This recorder can be used to save images to files.
 * @ingroup mediawic
 */
class OCEAN_MEDIA_WIC_EXPORT WICBufferImageRecorder :
	virtual public WICObject,
	virtual public BufferImageRecorder
{
	friend class WICLibrary;

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
		WICBufferImageRecorder();

		/**
		 * Destructs a buffer image recorder object.
		 */
		virtual ~WICBufferImageRecorder();

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;

		/// Internal image buffer if the image is not saved explicitly but by a buffer request.
		std::vector<uint8_t> recorderBuffer;
};

}

}

}

#endif // META_OCEAN_MEDIA_WIC_WIC_BUFFER_IMAGE_RECORDER_H
