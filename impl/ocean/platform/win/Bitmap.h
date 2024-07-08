/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_BITMAP_H
#define META_OCEAN_PLATFORM_WIN_BITMAP_H

#include "ocean/platform/win/Win.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements a Windows device independent bitmap.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Bitmap
{
	public:

		/**
		 * Creates an invalid bitmap.
		 */
		Bitmap() = default;

		/**
		 * Copies a bitmap.
		 * @param bitmap The bitmap to copy
		 */
		Bitmap(const Bitmap& bitmap);

		/**
		 * Move constructor.
		 * @param bitmap The bitmap to move
		 */
		Bitmap(Bitmap&& bitmap);

		/**
		 * Creates a new bitmap by a given frame holding the image data.
		 * @param frame The frame to create a bitmap from
		 */
		explicit Bitmap(const Frame& frame);

		/**
		 * Creates a new bitmap with given size and pixel format.
		 * @param frameType The type of the bitmap that will be created
		 */
		explicit Bitmap(const FrameType& frameType);

		/**
		 * Creates a new bitmap with given size and pixel format.
		 * @param width The width of the bitmap in pixel, with range [1, infinity)
		 * @param height The height of the bitmap in pixel, with range [1, infinity)
		 * @param preferredPixelFormat Pixel format of the bitmap
		 * @param pixelOrigin Frame origin
		 */
		Bitmap(const unsigned int width, const unsigned int height, const FrameType::PixelFormat preferredPixelFormat, const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Creates a new bitmap with given size and pixel data.
		 * @param width The width of the bitmap in pixel, with range [1, infinity)
		 * @param height The height of the bitmap in pixel, with range [1, infinity)
		 * @param pixelFormat The pixel format of the given data, must be supported
		 * @param pixelOrigin The origin of the given frame data
		 * @param frame The frame data to be copied, must be valid
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @see internalPixelFormat().
		 */
		Bitmap(unsigned int width, unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const uint8_t* frame, const unsigned int framePaddingElements);

		/**
		 * Destructs a bitmap.
		 */
		~Bitmap();

		/**
		 * Returns the width of the bitmap.
		 * @return The bitmap's width, in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the bitmap.
		 * @return The bitmap's height, in pixel with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the bitmap's pixel format.
		 * @return Pixel format
		 * @see internalPixelFormat().
		 */
		inline FrameType::PixelFormat pixelFormat() const;

		/**
		 * Returns the origin of the bitmap.
		 * @return Frame origin
		 */
		inline FrameType::PixelOrigin pixelOrigin() const;

		/**
		 * Returns the device context of the bitmap.
		 * @return Device context
		 */
		inline HDC dc() const;

		/**
		 * Returns the pixel data of the bitmap.
		 * @return Pixel data
		 */
		inline const uint8_t* data() const;

		/**
		 * Returns the pixel data of the bitmap.
		 * @return Pixel data
		 */
		inline uint8_t* data();

		/**
		 * Returns the size of the bitmap's internal image buffer.
		 * @return The bitmap's internal image buffer, in bytes, with range [0, infinity)
		 */
		inline unsigned int size() const;

		/**
		 * Returns the number of padding elements at the end of each bitmap row.
		 * @return The number of padding elements, in elements, with range [0, 3]
		 */
		inline unsigned int paddingElements() const;

		/**
		 * Returns the number of elements per bitmap row including optional padding elements at the end of each row.
		 * Windows bitmap strides are 4-byte aligned.
		 * @return The number of elements per bitmap row, in elements, with range [width * channels, infinity)
		 */
		inline unsigned int strideElements() const;

		/**
		 * Sets the pixel data of the bitmap.
		 * The pixel format of the given frame must match with the bitmap's internal pixel format.
		 * Both pixel formats match if bitmap.internalPixelFormat(pixelFormat) == bitmap.pixelFormat().
		 * @param frame The frame data to be set, must fit the resolution of this bitmap, must be valid
		 * @param pixelFormat The pixel format of the given data, must be a supported pixel format, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @see internalPixelFormat().
		 */
		bool setData(const uint8_t* frame, const FrameType::PixelFormat pixelFormat, const unsigned int framePaddingElements);

		/**
		 * Sets or changes the format of the bitmap.
		 * @param width The width of the bitmap in pixel, with range [1, infinity)
		 * @param height The height of the bitmap in pixel, with range [1, infinity)
		 * @param preferredPixelFormat Preferred pixel format of the bitmap, this may be changed internally
		 * @param pixelOrigin The origin of the bitmap
		 * @return True, if the format could be changed
		 */
		bool set(const unsigned int width, const unsigned int height, const FrameType::PixelFormat preferredPixelFormat, const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Sets or changes the bitmap data.
		 * @param frame The frame to set
		 * @return True, if succeeded
		 */
		bool set(const Frame& frame);

		/**
		 * Releases the bitmap data and resets all values to default values.
		 */
		void release();

		/**
		 * Returns whether the bitmap holds valid data.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Move operator.
		 * @param bitmap The bitmap to move
		 * @return Reference to this object
		 */
		Bitmap& operator=(Bitmap&& bitmap);

		/**
		 * Assigns a bitmap.
		 * @param bitmap Right bitmap
		 * @return Reference to this bitmap
		 */
		Bitmap& operator=(const Bitmap& bitmap);

		/**
		 * Returns which (internal) pixel format the bitmap uses to store the image data for a given pixel format.
		 * @param pixelFormat The pixel format for which the bitmap's internal pixel format will be returned, must be valid
		 * @return The bitmap's internal pixel format for the specified pixel format, FORMAT_UNDEFINED if no internal pixel format exists
		 */
		static FrameType::PixelFormat internalPixelFormat(const FrameType::PixelFormat pixelFormat);

	protected:

		/**
		 * Creates a new bitmap object.
		 * @param width The width of the bitmap in pixel, with range [1, infinity)
		 * @param height The height of the bitmap in pixel, with range [1, infinity)
		 * @param supportedPixelFormat The pixel format of the bitmap, must be supported by the bitmap, must be valid
		 * @param pixelOrigin The origin of the bitmap
		 * @return True, if succeeded
		 */
		bool createBitmap(const unsigned int width, const unsigned int height, const FrameType::PixelFormat supportedPixelFormat, const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Creates the bitmap info for a specified bitmap.
		 * @param width The width of the bitmap in pixel, with range [1, infinity)
		 * @param height The height of the bitmap in pixel, with range [1, infinity)
		 * @param pixelFormat The pixel format of the bitmap, must be supported, must be valid
		 * @param pixelOrigin The origin of the bitmap
		 * @param bitmapInfoMemory The resulting bitmap info
		 * @return True, if succeeded
		 */
		static bool createBitmapInfo(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, Memory& bitmapInfoMemory);

	protected:

		/// Width of the bitmap in pixel, with range [0, infinity)
		unsigned int bitmapWidth_ = 0u;

		/// Height of the bitmap in pixel, with range [0, infinity)
		unsigned int bitmapHeight_ = 0u;

		/// Pixel format of the bitmap.
		FrameType::PixelFormat bitmapPixelFormat_ = FrameType::FORMAT_UNDEFINED;

		/// Origin of the frame
		FrameType::PixelOrigin bitmapPixelOrigin_ = FrameType::ORIGIN_INVALID;

		/// Pixel data of the bitmap.
		uint8_t* bitmapData_ = nullptr;

		/// Size of the pixel buffer in bytes.
		unsigned int bitmapSize_ = 0u;

		/// The number of padding elements at the end of each bitmap row, with range [0, 3].
		unsigned int bitmapPaddingElements_ = 0u;

		/// Number of elements per bitmap row including optional padding elements at the end of each row, with range [width * channels, infinity)
		unsigned int bitmapStrideElements_ = 0u;

		/// Device context of the bitmap.
		HDC bitmapDC_ = nullptr;

		/// Handle of the bitmap.
		HBITMAP bitmapHandle_ = nullptr;
};

inline unsigned int Bitmap::width() const
{
	return bitmapWidth_;
}

inline unsigned int Bitmap::height() const
{
	return bitmapHeight_;
}

inline FrameType::PixelFormat Bitmap::pixelFormat() const
{
	return bitmapPixelFormat_;
}

inline FrameType::PixelOrigin Bitmap::pixelOrigin() const
{
	return bitmapPixelOrigin_;
}

inline HDC Bitmap::dc() const
{
	return bitmapDC_;
}

inline const uint8_t* Bitmap::data() const
{
	return bitmapData_;
}

inline uint8_t* Bitmap::data()
{
	return bitmapData_;
}

inline unsigned int Bitmap::size() const
{
	return bitmapSize_;
}

inline unsigned int Bitmap::paddingElements() const
{
	return bitmapPaddingElements_;
}


inline unsigned int Bitmap::strideElements() const
{
	return bitmapStrideElements_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_BITMAP_H
