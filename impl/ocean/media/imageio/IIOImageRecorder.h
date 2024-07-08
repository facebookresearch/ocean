/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_RECORDER_H

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/media/ImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements an image recorder for ImageIO.
 * This recorder can be used to save images to files.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOImageRecorder :
	virtual public IIOObject,
	virtual public ImageRecorder
{
	friend class IIOLibrary;

	public:

		/**
		 * Saves a given frame as file.
		 * @see ImageRecorder::saveImage().
		 */
		bool saveImage(const Frame& frame, const std::string& filename) override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @see FrameRecorder::frameEncoders().
		 */
		Encoders frameEncoders() const override;

		/**
		 * Returns a pointer to a framebuffer to be filled if a image save task has been asked before.
		 * @see ImageRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * @see ILImageRecorder::unlockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new image recorder object.
		 */
		IIOImageRecorder();

		/**
		 * Destructs an image recorder object.
		 */
		~IIOImageRecorder() override;

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_RECORDER_H
