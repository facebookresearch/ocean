/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/ImageGif.h"

#include "ocean/base/ScopedFunction.h"

#include "ocean/cv/FrameConverter.h"

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF
	#include <gif_lib.h>
#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF

class ImageGif::SourceBuffer
{
	public:

		/**
		 * Creates a new source buffer with memory pointer and memory size.
		 * @param buffer The buffer of the image, must be valid
		 * @param size The size of the buffer, in bytes, with range [1, infinity)
		 */
		inline SourceBuffer(const void* buffer, const size_t size);

		/**
		 * Reads bytes from this buffer (at the current location) and copies them in a target buffer.
		 * @param targetBuffer The target buffer to which the bytes will be copied, must be valid
		 * @param length The number of bytes to read, with range [0, infinity)
		 * @return The number of bytes which could be read, -1 if an error occured
		 */
		inline int readBytesFromBuffer(GifByteType* targetBuffer, const size_t length);

		/**
		 * Reads bytes from this buffer (at the current location) and copies them in a target buffer.
		 * @param gifFile The gif file for which the buffer will be read, must be valid
		 * @param targetBuffer The target buffer to which the bytes will be copied, must be valid
		 * @param length The number of bytes to read, with range [0, infinity)
		 * @return The number of bytes which could be read, -1 if an error occured
		 */
		static inline int staticReadBytesFromBuffer(GifFileType* gifFile, GifByteType* targetBuffer, int length);

		/**
		 * Writes one pixel of the gif image.
		 * @param colorMap The color map holding the color palette, must be valid
		 * @param rasterBit The color map entry
		 * @param transparentColor the color map entry with the transparent color, -1 if no transparent color exist
		 * @param disposalMode The disposal mode to be applied
		 * @param pixel The target pixel to which the color will be written
		 */
		static inline void colorMapToRGBA32(const ColorMapObject* colorMap, const GifByteType rasterBit, const int transparentColor, uint8_t* const pixel);

	protected:

		/// The source buffer, must be valid.
		const void* buffer_;

		/// The size of the buffer, in bytes, with range [1, infinity).
		const size_t size_;

		/// The current position within the source buffer, with range [0, size_ - 1].
		size_t currentPosition_;
};

inline ImageGif::SourceBuffer::SourceBuffer(const void* buffer, const size_t size) :
	buffer_(buffer),
	size_(size),
	currentPosition_(0)
{
	ocean_assert(buffer_ != nullptr && size_ > 0);
}

inline int ImageGif::SourceBuffer::readBytesFromBuffer(GifByteType* targetBuffer, const size_t length)
{
	ocean_assert(targetBuffer != nullptr);

	const size_t remainingBytes = size_ - currentPosition_;
	ocean_assert(remainingBytes <= size_);

	if (length <= remainingBytes)
	{
		memcpy(targetBuffer, (const uint8_t*)buffer_ + currentPosition_, length);

		currentPosition_ += length;

		return int(length);
	}

	return -1;
}

inline int ImageGif::SourceBuffer::staticReadBytesFromBuffer(GifFileType* gifFile, GifByteType* targetBuffer, int length)
{
	ocean_assert(gifFile != nullptr && targetBuffer != nullptr);

	SourceBuffer* sourceBuffer = reinterpret_cast<SourceBuffer*>(gifFile->UserData);

	if (sourceBuffer && length >= 0)
	{
		return sourceBuffer->readBytesFromBuffer(targetBuffer, size_t(length));
	}

	return -1;
}

inline void ImageGif::SourceBuffer::colorMapToRGBA32(const ColorMapObject* colorMap, const GifByteType rasterBit, const int transparentColor, uint8_t* const pixel)
{
	ocean_assert(colorMap != nullptr);
	ocean_assert(pixel != nullptr);

	if (int(rasterBit) == transparentColor)
	{
		return;
	}

	if (rasterBit < colorMap->ColorCount)
	{
		const GifColorType& color = colorMap->Colors[rasterBit];

		pixel[0] = color.Red;
		pixel[1] = color.Green;
		pixel[2] = color.Blue;
		pixel[3] = 0xFF;
	}
	else
	{
		ocean_assert(false && "Invalid color bit!");
	}
}

#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

Frames ImageGif::decodeImages(const void* buffer, const size_t size, const size_t maximalImages)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return Frames();
	}

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF

	// DGifSlurp can contain an interger overflow, so we need to apply a manual pre-check to ensure that the gif file is not corrupt

	if (!verifyGif(buffer, size))
	{
		ocean_assert(false && "The gif is corrupt");
		return Frames();
	}

	SourceBuffer sourceBuffer(buffer, size);

	int error = 0;
	GifFileType* gifFile = DGifOpen(&sourceBuffer, &SourceBuffer::staticReadBytesFromBuffer, &error);

	if (gifFile == nullptr)
	{
		return Frames();
	}

	const ScopedFunctionVoid scopedCloseFileFunction(std::bind(&DGifCloseFile, gifFile, &error));

	if (DGifSlurp(gifFile) != GIF_OK)
	{
		return Frames();
	}

	if (gifFile->ImageCount <= 0)
	{
		return Frames();
	}

	size_t numberImages = size_t(gifFile->ImageCount);

	if (maximalImages > 0u)
	{
		numberImages = std::min(numberImages, maximalImages);
	}

	Frames frames;

	const int maxWidth = gifFile->SWidth;
	const int maxHeight = gifFile->SHeight;

	if (maxWidth >= 1 && maxHeight >= 1)
	{
		if (uint64_t(maxWidth) * uint64_t(maxHeight) >= uint64_t(1073741823ull)) // width * height * 4 < 2^32
		{
			return Frames();
		}

		const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGBA32;

		const FrameType frameType = FrameType((unsigned int)(maxWidth), (unsigned int)(maxHeight), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

		frames.reserve(numberImages);

		// adding the first frame
		frames.emplace_back(frameType);

		// we are not using the background color as specified in the gif, but we use a full transparent background
		const Frame::PixelType<uint8_t, 4u> backgroundColor = {0x00};
		frames.back().setValue<uint8_t, 4u>(backgroundColor);

		for (size_t imageIndex = 0; imageIndex < numberImages; ++imageIndex)
		{
			const ColorMapObject* colorMap = gifFile->SavedImages[imageIndex].ImageDesc.ColorMap ? gifFile->SavedImages[imageIndex].ImageDesc.ColorMap : gifFile->SColorMap;

			if (colorMap == nullptr)
			{
				ocean_assert(false && "Missing color map!");
				continue;
			}

			int transparentColor = -1;
			int disposalMode = DISPOSAL_UNSPECIFIED;

			GraphicsControlBlock graphicsControlBlock;
			if (DGifSavedExtensionToGCB(gifFile, int(imageIndex), &graphicsControlBlock) == GIF_OK)
			{
				transparentColor = graphicsControlBlock.TransparentColor;
				disposalMode = graphicsControlBlock.DisposalMode;
			}

			Frame& frame = frames.back();

			SavedImage& savedImage = gifFile->SavedImages[imageIndex];

			const unsigned int subFrameLeft = (unsigned int)(savedImage.ImageDesc.Left);
			const unsigned int subFrameTop = (unsigned int)(savedImage.ImageDesc.Top);
			const unsigned int subFrameWidth = (unsigned int)(savedImage.ImageDesc.Width);
			const unsigned int subFrameHeight = (unsigned int)(savedImage.ImageDesc.Height);

			if (uint64_t(subFrameLeft) + uint64_t(subFrameWidth) > uint64_t(maxWidth)
					|| uint64_t(subFrameTop) + uint64_t(subFrameHeight) > uint64_t(maxHeight))
			{
				// the sub-frame must not be larger than the actual frame

				return Frames();
			}

			ocean_assert(uint64_t(maxWidth) * uint64_t(maxHeight) <= uint64_t(4294967292ull)); // width * height < 2^32

			const Frame subFrame(FrameType(subFrameWidth, subFrameHeight, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), savedImage.RasterBits, Frame::CM_USE_KEEP_LAYOUT);

			for (unsigned int y = 0u; y < subFrame.height(); ++y)
			{
				static_assert(std::is_same<GifByteType, uint8_t>::value, "Invalid data type!");

				const GifByteType* const rowRasterBits = subFrame.constrow<GifByteType>(y);
				uint8_t* const targetRow = frame.pixel<uint8_t>(subFrameLeft, subFrameTop + y);

				for (unsigned int x = 0u; x < subFrame.width(); ++x)
				{
					SourceBuffer::colorMapToRGBA32(colorMap, rowRasterBits[x], transparentColor, targetRow + x * 4u);
				}
			}

			// now, we prepare the next image

			if (imageIndex + 1 >= numberImages)
			{
				break;
			}

			switch (disposalMode)
			{
				case DISPOSAL_UNSPECIFIED:
				case DISPOSE_DO_NOT:
				{
					// we create a copy of the current image
					frames.emplace_back(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

					break;
				}

				case DISPOSE_BACKGROUND:
				{
					if (subFrame.width() == (unsigned int)(maxWidth) && subFrame.height() == (unsigned int)(maxHeight))
					{
						// the entire image will be set to the background color (full transparent)

						frames.emplace_back(frameType);
						frames.back().setValue<uint8_t, 4u>(backgroundColor);
					}
					else
					{
						// we make a copy of the current image, but set the region of the current subFrame to the background color

						frames.emplace_back(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
						frames.back().subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue<uint8_t, 4u>(backgroundColor);
					}

					break;
				}

				case DISPOSE_PREVIOUS:
				{
					if (imageIndex > 0)
					{
						if (subFrame.width() == (unsigned int)(maxWidth) && subFrame.height() == (unsigned int)(maxHeight))
						{
							// we make a copy of the previous image (not the current one)

							frames.emplace_back(frames[imageIndex - 1], Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
						}
						else
						{
							// we make a copy of the current image, but set the region of the current subFrame to the previous image

							frames.emplace_back(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
							frames.back().copy(subFrameLeft, subFrameTop, frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight));
						}
					}

					break;
				}
			}
		}
	}

	return frames;

#else

	ocean_assert(false && "OCEAN_MEDIA_OIL_SUPPORT_GIF not defined");
	return Frames();

#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

}

Frames ImageGif::readImages(const std::string& filename, const size_t maximalImages)
{
	const std::string::size_type fileExtensionPos = filename.rfind('.');

	if (fileExtensionPos == std::string::npos || fileExtensionPos + 1 == filename.size() || String::toLower(filename.substr(fileExtensionPos + 1)) != "gif")
	{
		return Frames();
	}

	std::ifstream inputStream(filename.c_str(), std::ios::binary);

	if (!inputStream.is_open())
	{
		Log::warning() << "Could not open image file \"" << filename << "\"";
		return Frames();
	}

	inputStream.seekg(0, inputStream.end);
	const uint64_t fileSize = uint64_t(inputStream.tellg());

	if (fileSize >= uint64_t(NumericT<size_t>::maxValue()))
	{
		// the file is too large for the memory
		return Frames();
	}

	if (fileSize == 0)
	{
		Log::warning() << "The image \"" << filename << "\" does not contain any data";
		return Frames();
	}

	inputStream.seekg(0, inputStream.beg);

	std::vector<uint8_t> buffer = std::vector<uint8_t>(size_t(fileSize));
	inputStream.read((char*)buffer.data(), buffer.size());

	if (inputStream.bad())
	{
		return Frames();
	}

	return decodeImages(buffer.data(), buffer.size(), maximalImages);
}

bool ImageGif::verifyGif(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return false;
	}

#ifdef OCEAN_MEDIA_OIL_SUPPORT_GIF

	SourceBuffer sourceBuffer(buffer, size);

	int error = 0;
	GifFileType* gifFile = DGifOpen(&sourceBuffer, &SourceBuffer::staticReadBytesFromBuffer, &error);

	if (gifFile == nullptr)
	{
		return false;
	}

	const ScopedFunctionVoid scopedCloseFileFunction(std::bind(&DGifCloseFile, gifFile, &error));

	GifRecordType gifRecordType = UNDEFINED_RECORD_TYPE;

	do
	{
		if (DGifGetRecordType(gifFile, &gifRecordType) == GIF_ERROR)
		{
			ocean_assert(false && "Failed to get record type!");
			return false;
		}

		switch (gifRecordType)
		{
			case IMAGE_DESC_RECORD_TYPE:
			{
				if (DGifGetImageDesc(gifFile) == GIF_ERROR)
				{
					ocean_assert(false && "Failed to get image description!");
					return false;
				}

				if (gifFile->ImageCount == 0)
				{
					return false;
				}

				SavedImage& lastSavedImage = gifFile->SavedImages[gifFile->ImageCount - 1];

				const GifWord width = lastSavedImage.ImageDesc.Width;
				const GifWord height = lastSavedImage.ImageDesc.Height;

				static_assert(std::is_same<GifWord, int>::value, "Invalid data type!");
				static_assert(sizeof(GifWord) <= sizeof(int64_t), "Invalid data type!");
				static_assert(std::is_same<GifPixelType, uint8_t>::value, "Invalid data type!");

				const int64_t maximalImageSize = 32768;

				if (width < 0 || height < 0 || width > maximalImageSize || height > maximalImageSize)
				{
					ocean_assert(false && "Invalid image resolution");
					return false;
				}

				return true;
			}

			case EXTENSION_RECORD_TYPE:
			{
				int gifExtensionFunction = 0;
				GifByteType* gifExtensionData = nullptr;

				if (DGifGetExtension(gifFile, &gifExtensionFunction, &gifExtensionData) == GIF_ERROR)
				{
					return false;
				}

				// create an extension block with our data

				if (gifExtensionData != nullptr)
				{
					if (GifAddExtensionBlock(&gifFile->ExtensionBlockCount, &gifFile->ExtensionBlocks, gifExtensionFunction, gifExtensionData[0], &gifExtensionData[1]) == GIF_ERROR)
					{
						return false;
					}
				}

				while (gifExtensionData != nullptr)
				{
					if (DGifGetExtensionNext(gifFile, &gifExtensionData) == GIF_ERROR)
					{
						return false;
					}

					// continue the extension block

					if (gifExtensionData != nullptr)
					{
						if (GifAddExtensionBlock(&gifFile->ExtensionBlockCount, &gifFile->ExtensionBlocks, CONTINUE_EXT_FUNC_CODE, gifExtensionData[0], &gifExtensionData[1]) == GIF_ERROR)
						{
							return false;
						}
					}
				}

				break;
			}

			default:
				break;
		}
	}
	while (gifRecordType != TERMINATE_RECORD_TYPE);

	ocean_assert(false && "Gif did not contain descriptor record");
	return false;

#else

	ocean_assert(false && "Not supported");
	return false;

#endif // OCEAN_MEDIA_OIL_SUPPORT_GIF

}

}

}

}
