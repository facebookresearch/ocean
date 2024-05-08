/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_PIXEL_BUFFER_ACCESSOR_H
#define META_OCEAN_MEDIA_AVF_PIXEL_BUFFER_ACCESSOR_H

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/base/Frame.h"

#include <AVFoundation/AVFoundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class allows to access a Core Video's CVPixelBuffer.
 * @ingroup mediaavf
 */
class PixelBufferAccessor
{
	public:

		/**
		 * Creates an invalid accessor object.
		 */
		PixelBufferAccessor() = default;

		/**
		 * Move constructor.
		 * @param pixelBufferAccessor The accessor to be moved
		 */
		PixelBufferAccessor(PixelBufferAccessor&& pixelBufferAccessor);

		/**
		 * Creates a new accessor object based on a given pixel buffer.
		 * @param pixelBuffer The pixel buffer to be accessed
		 * @param readOnly True, access a read-only buffer; False, to access a writable buffer
		 */
		explicit PixelBufferAccessor(CVPixelBufferRef pixelBuffer, const bool readOnly);

		/*
		 * Creates a new accessor object based on a given pixel buffer.
		 * @param pixelBuffer The pixel buffer to be accessed
		 * @param readOnly True, access a read-only buffer; False, to access a writable buffer
		 * @papram accessYPlaneOnly True, to access the grayscale data only; False, to access the entire image data; if the underlying data does not have a Y plane, this flag is ignored, and the entire image is accessed
		 */
		explicit PixelBufferAccessor(CVPixelBufferRef pixelBuffer, const bool readOnly, const bool accessYPlaneOnly);

		/**
		 * Destructs an accessor object.
		 */
		~PixelBufferAccessor();

		/**
		 * Returns the read-only frame providing access to the pixel buffer.
		 * @return The frame with read-only access, if any
		 */
		inline const Frame& frame() const;

		/**
		 * Returns the writable frame providing access to the pixel buffer.
		 * @return The frame with writable access, if any
		 */
		inline Frame& frame();

		/**
		 * Explicitly releases this accessor object.
		 * This function is also called in the constructor of this accessor.
		 */
		void release();

		/**
		 * Returns whether this accessor has access to a pixel buffer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator
		 * @param pixelBufferAccessor The accessor to be moved
		 * @return Reference to this object
		 */
		PixelBufferAccessor& operator=(PixelBufferAccessor&& pixelBufferAccessor);

		/**
		 * Translates an Apple platform pixel format type to an Ocean pixel format.
		 * @param pixelFormat The pixel format to translate
		 * @return The translated pixel format, FORMAT_UNDEFINED if the value could not be translated
		 */
		static FrameType::PixelFormat translatePixelFormat(const OSType pixelFormat);

		/**
		 * Translates an Ocean pixel format to an Apple platform pixel format type.
		 * @param pixelFormat The pixel format to translate
		 * @return The translated pixel format type, 0 if the value could not be translated
		 */
		static OSType translatePixelFormat(const FrameType::PixelFormat pixelFormat);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param pixelBufferAccessor The accessor which would be copied
		 */
		PixelBufferAccessor(const PixelBufferAccessor& pixelBufferAccessor) = delete;

		/**
		 * Disabled assign operator.
		 * @param pixelBufferAccessor The accessor which would be assigned
		 * @return Reference to this object
		 */
		PixelBufferAccessor& operator=(const PixelBufferAccessor& pixelBufferAccessor) = delete;

	protected:

		/// The pixel buffer to be accessed.
		CVPixelBufferRef pixelBuffer_ = nullptr;

		/// The lock flags.
		CVPixelBufferLockFlags lockFlags_ = 0;

		/// The padding frame wrapping the pixel buffer.
		Frame frame_;

#ifdef OCEAN_DEBUG

		/// Pointer to the frame's memory.
		const void* debugFrameData_ = nullptr;

#endif
};

inline const Frame& PixelBufferAccessor::frame() const
{
	return frame_;
}

inline Frame& PixelBufferAccessor::frame()
{
	return frame_;
}

inline PixelBufferAccessor::operator bool() const
{
	return frame_.isValid();
}

}

}

}

#endif // META_OCEAN_MEDIA_AVF_PIXEL_BUFFER_ACCESSOR_H
