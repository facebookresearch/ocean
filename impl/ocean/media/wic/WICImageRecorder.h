/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_WIC_WIC_IMAGE_RECORDER_H

#include "ocean/media/wic/WIC.h"
#include "ocean/media/wic/WICObject.h"

#include "ocean/media/ImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * This class implements an image recorder for WIC.
 * This recorder can be used to save images to files.
 * @ingroup mediawic
 */
class OCEAN_MEDIA_WIC_EXPORT WICImageRecorder :
	virtual public WICObject,
	virtual public ImageRecorder
{
	friend class WICLibrary;

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
		 * Locks the most recent frame and returns it so that the image data can be written to this frame.
		 * @see FrameRecorder::lockBufferToFill().
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
		WICImageRecorder();

		/**
		 * Destructs an image recorder object.
		 */
		virtual ~WICImageRecorder();

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;
};

}

}

}

#endif // META_OCEAN_MEDIA_WIC_WIC_IMAGE_RECORDER_H
