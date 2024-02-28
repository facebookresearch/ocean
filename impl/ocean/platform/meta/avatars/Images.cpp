// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Images.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

bool Images::loadImages(const Rendering::Engine& engine, const ovrAvatar2ResourceId resourceId)
{
	ocean_assert(resourceId != ovrAvatar2ResourceId_Invalid);

	uint32_t imageCount = 0u;
	ovrAvatar2Asset_GetImageCount(resourceId, &imageCount);

	for (uint32_t nImage = 0u; nImage < imageCount; ++nImage)
	{
		ovrAvatar2Image image;
		if (ovrAvatar2Asset_GetImageByIndex(resourceId, nImage, &image) != ovrAvatar2Result_Success)
		{
			ocean_assert(false && "Failed to read image");
			continue;
		}

		if (imageMap_.find(image.id) != imageMap_.cend())
		{
			// already handled
			continue;
		}

		Log::debug() << "New image: " << image.sizeX << "x" << image.sizeY << ", " << translateImageFormat(image.format) << ", " << image.mipCount << " levels, id: " << image.id;

		Rendering::FrameTexture2DRef frameTexture = engine.factory().createFrameTexture2D();
		ocean_assert(frameTexture);

		const FrameType frameType(image.sizeX, image.sizeY, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT);

		if (image.format == ovrAvatar2ImageFormat_RGBA32)
		{
			if (frameType.frameTypeSize() > image.imageDataSize)
			{
				Log::error() << "Invalid image data size";
				continue;
			}

			constexpr unsigned int paddingElements = 0u;
			Frame rgbaFrame(frameType, paddingElements);

			if (ovrAvatar2Asset_GetImageDataByIndex(resourceId, nImage, rgbaFrame.data<uint8_t>(), rgbaFrame.size()) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to read image data";
				continue;
			}

			frameTexture->setTexture(std::move(rgbaFrame));
		}
		else
		{
			Rendering::FrameTexture2D::CompressedFormat compressedFormat = Rendering::FrameTexture2D::CF_INVALID;

			switch (image.format)
			{
				case ovrAvatar2ImageFormat_ASTC_RGBA_4x4:
					compressedFormat = Rendering::FrameTexture2D::CF_SRGBA_ASTC_4X4;
					break;

				case ovrAvatar2ImageFormat_ASTC_RGBA_6x6:
					compressedFormat = Rendering::FrameTexture2D::CF_SRGBA_ASTC_6X6;
					break;

				case ovrAvatar2ImageFormat_ASTC_RGBA_8x8:
					compressedFormat = Rendering::FrameTexture2D::CF_SRGBA_ASTC_8X8;
					break;

				default:
					Log::error() << "Image format '" << translateImageFormat(image.format) << "' is not supported";
					continue;
			}

			ocean_assert(compressedFormat != Rendering::FrameTexture2D::CF_INVALID);

			Rendering::FrameTexture2D::Buffer buffer(image.imageDataSize);

			if (ovrAvatar2Asset_GetImageDataByIndex(resourceId, nImage, buffer.data(), uint32_t(buffer.size())) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to read image data";
				continue;
			}

			frameTexture->setTexture(Rendering::FrameTexture2D::CompressedFrame(frameType, std::move(buffer), compressedFormat, (unsigned int)(image.mipCount)));
		}

		Log::debug() << "Added new image";

		imageMap_.emplace(image.id, std::move(frameTexture));
	}

	return true;
}

bool Images::unloadImages(const ovrAvatar2ResourceId resourceId)
{
	ocean_assert(resourceId != ovrAvatar2ResourceId_Invalid);

	uint32_t imageCount = 0u;
	ovrAvatar2Asset_GetImageCount(resourceId, &imageCount);

	Log::info() << "unloading " << imageCount << " images";

	bool allSucceeded = true;

	for (uint32_t nImage = 0u; nImage < imageCount; ++nImage)
	{
		ovrAvatar2Image image;
		if (ovrAvatar2Asset_GetImageByIndex(resourceId, nImage, &image) != ovrAvatar2Result_Success)
		{
			ocean_assert(false && "Failed to read image");
			continue;
		}

		if (imageMap_.find(image.id) != imageMap_.cend())
		{

		}
		else
		{
			Log::error() << "Image could not be unloaded as it did not exist";

			allSucceeded = false;
		}
	}

	return allSucceeded;
}

Rendering::TextureRef Images::texture(const ovrAvatar2ImageId imageId) const
{
	const ImageMap::const_iterator iImage = imageMap_.find(imageId);

	if (iImage != imageMap_.cend())
	{
		return iImage->second;
	}

	return Rendering::TextureRef();
}

void Images::release()
{
	imageMap_.clear();
}

std::string Images::translateImageFormat(const ovrAvatar2ImageFormat imageFormat)
{
	switch (imageFormat)
	{
		case ovrAvatar2ImageFormat_Invalid:
			return std::string("Invalid");

		case ovrAvatar2ImageFormat_RGBA32:
			return std::string("RGBA32");

		case ovrAvatar2ImageFormat_DXT1:
			return std::string("DXT1");

		case ovrAvatar2ImageFormat_DXT5:
			return std::string("DXT5");

		case ovrAvatar2ImageFormat_BC5U:
			return std::string("BC5U");

		case ovrAvatar2ImageFormat_BC5S:
			return std::string("BC5S");

		case ovrAvatar2ImageFormat_BC7U:
			return std::string("BC7U");

		case ovrAvatar2ImageFormat_ASTC_RGBA_4x4:
			return std::string("ASTC_RGBA_4x4");

		case ovrAvatar2ImageFormat_ASTC_RGBA_6x6:
			return std::string("ASTC_RGBA_6x6");

		case ovrAvatar2ImageFormat_ASTC_RGBA_8x8:
			return std::string("ASTC_RGBA_8x8");

		case ovrAvatar2ImageFormat_ASTC_RGBA_12x12:
			return std::string("ASTC_RGBA_12x12");

		case ovrAvatar2ImageFormat_EnumSize:
			ocean_assert(false && "Invalid input!");
			break;
	}

	ocean_assert(false && "Invalid image format!");
	return std::string("Unknown");
}

}

}

}

}
