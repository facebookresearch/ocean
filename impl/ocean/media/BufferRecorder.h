/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_BUFFER_FILE_RECORDER_H
#define META_OCEAN_MEDIA_BUFFER_FILE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/ExplicitRecorder.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class BufferRecorder;

/**
 * Definition of a object reference holding a buffer recorder.
 * @see SmartObjectRef, BufferRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<BufferRecorder, Recorder> BufferRecorderRef;

/**
 * This class is the base class for all buffer recorders.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT BufferRecorder : virtual public ExplicitRecorder
{
	public:

		/**
		 * Returns the buffer of the most recently saved image.
		 * @return Buffer of most recently saved image
		 */
		virtual bool buffer(std::vector<unsigned char>& data) const = 0;

		/**
		 * Returns the buffer type of this recorder.
		 * @return Buffer type
		 * @see setBufferType().
		 */
		virtual const std::string& bufferType() const;

		/**
		 * Sets the type of the buffer.
		 * For images this type will be the file extension of a corresponding image file, e.g. bmp, jpg, png, etc.
		 * @param bufferType Buffer type to be set
		 * @return True, if succeeded
		 */
		virtual bool setBufferType(const std::string& bufferType);

	protected:

		/**
		 * Creates a new buffer recorder.
		 */
		BufferRecorder();

		/**
		 * Destructs a buffer recorder.
		 */
		~BufferRecorder() override;

	protected:

		/// Buffer type of this recorder.
		std::string recorderBufferType;
};

}

}

#endif // META_OCEAN_MEDIA_BUFFER_FILE_RECORDER_H
