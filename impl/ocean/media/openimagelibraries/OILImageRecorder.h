/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_RECORDER_H
#define META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_RECORDER_H

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/media/ImageRecorder.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

/**
 * This class implements an image recorder for OpenImageLibraries.
 * This recorder can be used to save images to files.
 * @ingroup mediaoil
 */
class OCEAN_MEDIA_OIL_EXPORT OILImageRecorder : virtual public ImageRecorder
{
	friend class OILLibrary;
	friend class OILImageSequenceRecorder;

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
		 * @see OILImageRecorder::unlockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new image recorder object.
		 */
		OILImageRecorder();

		/**
		 * Destructs an image recorder object.
		 */
		~OILImageRecorder() override;

	protected:

		/// Image buffer if the image is not saved explicitly but by a buffer request.
		Frame recorderFrame_;
};

}

}

}

#endif // META_OCEAN_MEDIA_OPEN_IMAGE_LIBRARIES_OL_IMAGE_RECORDER_H
