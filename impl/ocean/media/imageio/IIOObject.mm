/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/IIOObject.h"

#include "ocean/base/Memory.h"
#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/platform/apple/Apple.h"

#import <CoreFoundation/CoreFoundation.h>

#import <CoreGraphics/CoreGraphics.h>

#include <ImageIO/ImageIO.h>

namespace Ocean
{

using namespace Platform::Apple;

namespace Media
{

namespace ImageIO
{

Frame IIOObject::loadFrameFromImageSource(CGImageSourceRef imageSource, Image::Properties* properties)
{
	ocean_assert(imageSource);
	if (imageSource == nullptr)
	{
		return Frame();
	}

	const CFStringRef imageSourceType = CGImageSourceGetType(imageSource);

	if (imageSourceType != nullptr)
	{
		if (StringApple::toUTF8(imageSourceType) == "public.pbm")
		{
			// we do not use ImageIO to load pbm as Apple seems to normalize the float data,
			// instead, Media::Special::ImagePfm should be used
			return Frame();
		}
	}

	Frame result;

	if (imageSource)
	{
		const size_t imageCount = CGImageSourceGetCount(imageSource);

		if (imageCount >= 1)
		{
			if (properties != nullptr)
			{
				*properties = Image::Properties();

				const ScopedCFDictionaryRef dictionary(CGImageSourceCopyPropertiesAtIndex(imageSource, 0, nullptr));

				if (dictionary.object() != nullptr)
				{
					const CFStringRef value = CFStringRef(CFDictionaryGetValue(dictionary.object(), kCGImagePropertyProfileName));

					if (value != nullptr)
					{
						properties->colorProfileName_ = StringApple::toUTF8(value);
					}
				}
			}

			const ScopedCGImageRef cgImage(CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr));

			result = loadFrameFromImage(cgImage.object());
		}
	}

	return result;
}

Frame IIOObject::loadFrameFromImage(CGImageRef cgImage)
{
	ocean_assert(cgImage);
	if (cgImage == nullptr)
	{
		return Frame();
	}

	const size_t width = CGImageGetWidth(cgImage);
	const size_t height = CGImageGetHeight(cgImage);

	const size_t bitsPerPixel = CGImageGetBitsPerPixel(cgImage);
	const size_t bitsPerComponent = CGImageGetBitsPerComponent(cgImage);

	const size_t bytesPerRow = CGImageGetBytesPerRow(cgImage);

	const CGImageAlphaInfo alphaInfo = CGImageGetAlphaInfo(cgImage);
	CGColorSpaceRef cgColorSpace = CGImageGetColorSpace(cgImage);

	const CGColorSpaceModel cgColorSpaceMode = CGColorSpaceGetModel(cgColorSpace);

	const CGBitmapInfo cgBitmapInfo = CGImageGetBitmapInfo(cgImage);

#ifdef OCEAN_DEBUG
	const bool notBigEndian16 = (cgBitmapInfo & kCGImageByteOrder16Big) != kCGImageByteOrder16Big;
	const bool notBigEnding32 = (cgBitmapInfo & kCGImageByteOrder32Big) != kCGImageByteOrder32Big;
	const bool bigEnding32_butRGB32 = (cgBitmapInfo & kCGImageByteOrder32Big) == kCGImageByteOrder32Big && cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 32;

	ocean_assert(notBigEndian16); // we do not support big endian
	ocean_assert(notBigEnding32 || bigEnding32_butRGB32);
#endif // OCEAN_DEBUG

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	switch (alphaInfo)
	{
		case kCGImageAlphaNone:
		{
			if (cgBitmapInfo == (kCGBitmapFloatComponents | kCGBitmapByteOrder32Little))
			{
				if (cgColorSpaceMode == kCGColorSpaceModelMonochrome && bitsPerPixel == 32)
				{
					pixelFormat = FrameType::FORMAT_F32;
				}
				else
				{
					ocean_assert(false && "The floating point pixel format is not supported!");
					return Frame();
				}
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelMonochrome && bitsPerPixel == 8)
			{
				pixelFormat = FrameType::FORMAT_Y8;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelMonochrome && bitsPerPixel == 16)
			{
				pixelFormat = FrameType::FORMAT_Y16;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelMonochrome && bitsPerPixel == 32)
			{
				pixelFormat = FrameType::FORMAT_Y32;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 24)
			{
				pixelFormat = FrameType::FORMAT_RGB24;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 48)
			{
				pixelFormat = FrameType::FORMAT_RGB48;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelIndexed)
			{
				if (bitsPerComponent == 8)
				{
					const size_t channels = bitsPerPixel / bitsPerComponent;

					if (channels == 1)
					{
						pixelFormat = FrameType::FORMAT_Y8;
					}
				}
			}

			break;
		}

		case kCGImageAlphaNoneSkipFirst:
		{
			break;
		}

		case kCGImageAlphaNoneSkipLast:
		{
			if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 32)
			{
				pixelFormat = FrameType::FORMAT_RGB32;
			}

			break;
		}

		case kCGImageAlphaLast:
		case kCGImageAlphaPremultipliedLast: // currently, Ocean does not distinguish between non-premultiplied and pre-multiplied, so we map both cases to the corresponding pixel format with alpha channel
		{
			if (cgColorSpaceMode == kCGColorSpaceModelMonochrome && bitsPerPixel == 16)
			{
				pixelFormat = FrameType::FORMAT_YA16;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 32)
			{
				pixelFormat = FrameType::FORMAT_RGBA32;
			}
			else if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 64 && bitsPerComponent == 16)
			{
				pixelFormat = FrameType::FORMAT_RGBA64;
			}

			break;
		}

		case kCGImageAlphaFirst:
		case kCGImageAlphaPremultipliedFirst: // currently, Ocean does not distinguish between non-premultiplied and pre-multiplied, so we map both cases to the corresponding pixel format with alpha channel
		{
			if (cgColorSpaceMode == kCGColorSpaceModelRGB && bitsPerPixel == 32)
			{
				pixelFormat = FrameType::FORMAT_ARGB32;
			}

			break;
		}

		default:
			break;
	}

	ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);
	if (FrameType::FORMAT_UNDEFINED == pixelFormat)
	{
		return Frame();
	}

	const bool isPremultiplied = alphaInfo == kCGImageAlphaPremultipliedLast || alphaInfo == kCGImageAlphaPremultipliedFirst;

	const ScopedCGDataProviderRef provider(CGImageGetDataProvider(cgImage), false /*needsRelease*/); // We must not release the data provider here CGDataProviderRelease(provider), this would cause segmetation faults

	const ScopedCFDataRef data(CGDataProviderCopyData(provider.object()));

	if (data.object() == nullptr || CFDataGetLength(data.object()) < height * bytesPerRow)
	{
		return Frame();
	}

	Frame result;
	if (cgColorSpaceMode == kCGColorSpaceModelIndexed)
	{
		// we have an image with index color space, so we cannot simply copy the memory

		const size_t tableCount = CGColorSpaceGetColorTableCount(cgColorSpace);

		Memory colorTableMemory(sizeof(uint8_t) * tableCount * 3); // the table seems to store 3 channels in any case
		uint8_t* colorTable = colorTableMemory.data<uint8_t>();
		CGColorSpaceGetColorTable(cgColorSpace, colorTable);

		const uint8_t* dataPtr = (const uint8_t*)(CFDataGetBytePtr(data.object()));

		ocean_assert(pixelFormat == FrameType::FORMAT_Y8);
		if (!result.set(FrameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/, Indices32() /*paddingElements*/))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		result.setTimestamp(Timestamp(true));

		for (unsigned int y = 0u; y < result.height(); ++y)
		{
			const uint8_t* const dataRow = dataPtr + y * bytesPerRow;
			uint8_t* const resultRow = result.row<uint8_t>(y);

			for (unsigned int x = 0u; x < result.width(); ++x)
			{
				if (dataRow[x] < tableCount)
				{
					resultRow[x] = colorTable[dataRow[x] * 3]; // again, three channels in any case
				}
				else
				{
					ocean_assert(false && "Invalid lookup table!");

					// we have an invalid lookup table
					return Frame();
				}
			}
		}
	}
	else if (pixelFormat == FrameType::FORMAT_RGB32)
	{
		if (!result.set(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/, Indices32() /*paddingElements*/))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		result.setTimestamp(Timestamp(true));

		const void* dataPtr = CFDataGetBytePtr(data.object());

		unsigned int dataPaddingElements = 0u;
		if (dataPtr != nullptr && Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), (unsigned int)(bytesPerRow), dataPaddingElements))
		{
			CV::FrameChannels::removeLastChannel<uint8_t, 4u>((const uint8_t*)(dataPtr), result.data<uint8_t>(), result.width(), result.height(), CV::FrameConverter::CONVERT_NORMAL, dataPaddingElements, result.paddingElements());
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			result.release();
		}
	}
	else if (pixelFormat == FrameType::FORMAT_RGB48)
	{
		// we convert FORMAT_RGB48 images to FORMAT_RGB24 images (as for now nobody needs 16 bit depth)

		if (!result.set(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/, Indices32() /*paddingElements*/))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		result.setTimestamp(Timestamp(true));

		const void* dataPtr = CFDataGetBytePtr(data.object());

		unsigned int dataPaddingElements = 0u;
		if (dataPtr != nullptr && FrameType::dataIsAligned<uint16_t>(dataPtr) && Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), (unsigned int)(bytesPerRow), dataPaddingElements))
		{
			CV::FrameChannels::narrow16BitPerChannelTo8BitPerChannel<3u>((const uint16_t*)(dataPtr), result.data<uint8_t>(), result.width(), result.height(), CV::FrameConverter::CONVERT_NORMAL, dataPaddingElements, result.paddingElements());
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			result.release();
		}
	}
	else if (pixelFormat == FrameType::FORMAT_RGBA64)
	{
		// we convert FORMAT_RGBA64 images to FORMAT_RGBA32 images (as for now nobody needs 16 bit depth)

		if (!result.set(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/, Indices32() /*paddingElements*/))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		result.setTimestamp(Timestamp(true));

		const void* dataPtr = CFDataGetBytePtr(data.object());

		unsigned int dataPaddingElements = 0u;
		if (dataPtr != nullptr && FrameType::dataIsAligned<uint16_t>(dataPtr) && Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), (unsigned int)(bytesPerRow), dataPaddingElements))
		{
			CV::FrameChannels::narrow16BitPerChannelTo8BitPerChannel<4u>((const uint16_t*)(dataPtr), result.data<uint8_t>(), result.width(), result.height(), CV::FrameConverter::CONVERT_NORMAL, dataPaddingElements, result.paddingElements());
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			result.release();
		}
	}
	else
	{
		const FrameType frameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

		ocean_assert(frameType.numberPlanes() == 1u);
		ocean_assert(frameType.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 || frameType.dataType() == FrameType::DT_UNSIGNED_INTEGER_16 || frameType.dataType() == FrameType::DT_UNSIGNED_INTEGER_32 || frameType.dataType() == FrameType::DT_SIGNED_FLOAT_32);

		const void* dataPtr = CFDataGetBytePtr(data.object());
		unsigned int dataPaddingElements = 0u;

		if (dataPtr == nullptr || !Frame::strideBytes2paddingElements(pixelFormat, (unsigned int)(width), (unsigned int)(bytesPerRow), dataPaddingElements))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		result = Frame(frameType, dataPtr, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, dataPaddingElements, Timestamp(true));

		if (isPremultiplied)
		{
			switch (pixelFormat)
			{
				case FrameType::FORMAT_YA16:
					CV::FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<2u, 1u>(result.data<uint8_t>(), result.width(), result.height(), result.paddingElements());
					break;

				case FrameType::FORMAT_RGBA32:
					CV::FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 3u>(result.data<uint8_t>(), result.width(), result.height(), result.paddingElements());
					break;

				case FrameType::FORMAT_ARGB32:
					CV::FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 0u>(result.data<uint8_t>(), result.width(), result.height(), result.paddingElements());
					break;

				default:
					ocean_assert(false && "Not supported!");
			}
		}
	}

	return result;
}

bool IIOObject::writeFrameToImageDestination(CGImageDestinationRef imageDestination, const Frame& frame, const Media::Image::Properties& properties)
{
	ocean_assert(frame.isValid());
	ocean_assert(properties.isValid());

	if (!frame || !properties.isValid())
	{
		return false;
	}

	ScopedCGColorSpaceRef colorSpace;
	CGBitmapInfo bitmapInfo;
	FrameType::PixelFormat targetPixelFormat;

	if (!translatePixelFormat(frame.pixelFormat(), properties.colorProfileName_, colorSpace, bitmapInfo, targetPixelFormat) || frame.pixelFormat() != targetPixelFormat)
	{
		return false;
	}

	ocean_assert(FrameType::numberPlanes(targetPixelFormat) == 1u);
	if (FrameType::numberPlanes(targetPixelFormat) != 1u)
	{
		return false;
	}

	// although we do not convert the pixel format, we may need to flip the image (to ensure that the origin is in the top left corner)

	Frame targetFrame;

	if (frame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		if (!targetFrame.set(FrameType(frame, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const unsigned int rowBytes = targetFrame.planeWidthBytes(0u);

		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			memcpy(targetFrame.row<void>(y), frame.constrow<void>(frame.height() - y - 1u), rowBytes);
		}
	}
	else
	{
		targetFrame = Frame(frame, Frame::ACM_USE_KEEP_LAYOUT);
	}

	bool result = false;

	const ScopedCFDataRef data(CFDataCreateWithBytesNoCopy(nullptr, targetFrame.constdata<uint8_t>(), targetFrame.size(), kCFAllocatorNull));

	const ScopedCGDataProviderRef dataProvider(CGDataProviderCreateWithCFData(data.object()));

	const unsigned int bitsPerDataType = targetFrame.bytesPerDataType() * 8u;
	const unsigned int bitsPerPixel = targetFrame.planeBytesPerPixel(0u) * 8u;
	ocean_assert(bitsPerPixel != 0u);

	const ScopedCGImageRef image(CGImageCreate(targetFrame.width(), targetFrame.height(), bitsPerDataType, bitsPerPixel, targetFrame.strideBytes(0u), colorSpace.object(), bitmapInfo, dataProvider.object(), nullptr, false, kCGRenderingIntentDefault));

	if (image.object() != nullptr)
	{
		ScopedCFDictionaryRef optionsDictionary;
		CFStringRef key = nullptr;
		ScopedCFNumberRef value;

		if (properties.quality_ >= 0.0f && properties.quality_ <= 1.0f)
		{
			key = kCGImageDestinationLossyCompressionQuality;
			value = ScopedCFNumberRef(CFNumberCreate(nullptr, kCFNumberFloat32Type, &properties.quality_));

			optionsDictionary = ScopedCFDictionaryRef(CFDictionaryCreate(nullptr, (const void**)(&key), (const void**)(&value.object()), 1, nullptr, nullptr));
		}

		CGImageDestinationAddImage(imageDestination, image.object(), optionsDictionary.object());
		result = CGImageDestinationFinalize(imageDestination);
	}

	return result;
}

bool IIOObject::writeFrameToImageDestination(CGImageDestinationRef imageDestination, const Frame& frame, const bool allowConversion, bool* hasBeenConverted, const Media::Image::Properties& properties)
{
	ocean_assert(frame.isValid());
	ocean_assert(properties.isValid());

	if (allowConversion == false)
	{
		return writeFrameToImageDestination(imageDestination, frame, properties);
	}

	if (!frame || !properties.isValid())
	{
		return false;
	}

	ScopedCGColorSpaceRef colorSpace;
	CGBitmapInfo bitmapInfo;
	FrameType::PixelFormat targetPixelFormat;

	if (!translatePixelFormat(frame.pixelFormat(), properties.colorProfileName_, colorSpace, bitmapInfo, targetPixelFormat))
	{
		return false;
	}

	Frame targetFrame;
	const bool conversionSuccess = CV::FrameConverter::Comfort::convert(frame, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT, targetFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 200u * 200u)());

	if (hasBeenConverted)
	{
		*hasBeenConverted = conversionSuccess && frame.pixelFormat() != targetPixelFormat;
	}

	if (!conversionSuccess)
	{
		return false;
	}

	CFDataRef data = CFDataCreateWithBytesNoCopy(nullptr, targetFrame.constdata<uint8_t>(), targetFrame.size(), kCFAllocatorNull);

	const ScopedCGDataProviderRef dataProvider(CGDataProviderCreateWithCFData(data));

	const unsigned int bitsPerDataType = targetFrame.bytesPerDataType() * 8u;
	const unsigned int bitsPerPixel = targetFrame.planeBytesPerPixel(0u) * 8u;
	ocean_assert(bitsPerPixel != 0u);

	const ScopedCGImageRef image(CGImageCreate(targetFrame.width(), targetFrame.height(), bitsPerDataType, bitsPerPixel, targetFrame.strideBytes(0u), colorSpace.object(), bitmapInfo, dataProvider.object(), nullptr, false, kCGRenderingIntentDefault));

	if (image.object() == nullptr)
	{
		return false;
	}

	CFDictionaryRef optionsDictionary = nullptr;
	CFStringRef key = nullptr;
	ScopedCFNumberRef value;

	if (properties.quality_ >= 0.0f && properties.quality_ <= 1.0f)
	{
		key = kCGImageDestinationLossyCompressionQuality;
		value = ScopedCFNumberRef(CFNumberCreate(nullptr, kCFNumberFloat32Type, &properties.quality_));
		optionsDictionary = CFDictionaryCreate(nullptr, (const void**)(&key), (const void**)(&value.object()), 1, nullptr, nullptr);
	}

	CGImageDestinationAddImage(imageDestination, image.object(), optionsDictionary);
	return CGImageDestinationFinalize(imageDestination);
}

ScopedCFStringRef IIOObject::findUniformTypeIdentifier(const std::string& fileExtension)
{
	ocean_assert(!fileExtension.empty());
	if (fileExtension.empty())
	{
		return ScopedCFStringRef(nullptr);
	}

	const std::string lowerExtension = String::toLower(fileExtension);

	// You may retain and release it, similar to other immutable CFString objects,
	// but are not required to do so—it will remain valid until the program terminates.
	constexpr bool needsRelease = false;

	if (lowerExtension == "bmp")
	{
		return ScopedCFStringRef(CFSTR("com.microsoft.bmp"), needsRelease);
	}

	if (lowerExtension == "jpg" || lowerExtension == "jpeg")
	{
		return ScopedCFStringRef(CFSTR("public.jpeg"), needsRelease);
	}

	if (lowerExtension == "gif")
	{
		return ScopedCFStringRef(CFSTR("com.compuserve.gif"), needsRelease);
	}

	if (lowerExtension == "png")
	{
		return ScopedCFStringRef(CFSTR("public.png"), needsRelease);
	}

	if (lowerExtension == "tif" || lowerExtension == "tiff")
	{
		return ScopedCFStringRef(CFSTR("public.tiff"), needsRelease);
	}

	if (lowerExtension == "heic")
	{
		return ScopedCFStringRef(CFSTR("public.heic"), needsRelease);
	}

	return ScopedCFStringRef();
}

std::string IIOObject::translateUniformTypeIdentifier(CFStringRef typeIdentifier)
{
	ocean_assert(typeIdentifier != nullptr);

	const std::string identifier = StringApple::toUTF8(typeIdentifier);

	if (identifier.empty())
	{
		return std::string();
	}

	if (identifier == std::string("com.microsoft.bmp"))
	{
		return "bmp";
	}

	if (identifier == std::string("public.jpeg"))
	{
		return "jpg";
	}

	if (identifier == std::string("com.compuserve.gif"))
	{
		return "gif";
	}

	if (identifier == std::string("public.png"))
	{
		return "png";
	}

	if (identifier == std::string("public.tiff"))
	{
		return "tif";
	}

	if (identifier == std::string("public.heic"))
	{
		return "heic";
	}

	return std::string();
}

bool IIOObject::translatePixelFormat(const FrameType::PixelFormat pixelFormat, const std::string& colorProfileName, ScopedCGColorSpaceRef& colorSpace, CGBitmapInfo& bitmapInfo, FrameType::PixelFormat& targetPixelFormat)
{
	CGColorSpaceRef internalColorSpace = nullptr;
	CGBitmapInfo internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaNone) | uint32_t(kCGBitmapByteOrderDefault));
	FrameType::PixelFormat internalTargetPixelFormat = pixelFormat;

	switch (pixelFormat)
	{
		case FrameType::FORMAT_ARGB32:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaFirst) | uint32_t(kCGBitmapByteOrderDefault));
			break;

		case FrameType::FORMAT_BGR24:
			// we need to convert BGR24 to RGB24 before writing
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			internalTargetPixelFormat = FrameType::FORMAT_RGB24;
			break;

		case FrameType::FORMAT_BGRA32:
			// we need to convert BGRA32 to RGBA32 before writing
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaLast) | uint32_t(kCGBitmapByteOrderDefault));
			internalTargetPixelFormat = FrameType::FORMAT_RGBA32;
			break;

		case FrameType::FORMAT_RGB24:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			break;

		case FrameType::FORMAT_RGB32:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaNoneSkipLast) | uint32_t(kCGBitmapByteOrderDefault));
			break;

		case FrameType::FORMAT_RGBA32:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaLast) | uint32_t(kCGBitmapByteOrderDefault));
			break;

		case FrameType::FORMAT_Y8:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_Y8, colorProfileName);
			break;

		case FrameType::FORMAT_Y16:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_Y8, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaNone) | uint32_t(kCGBitmapByteOrder16Little));
			break;

		case FrameType::FORMAT_YA16:
			internalColorSpace = determineColorSpace(FrameType::FORMAT_Y8, colorProfileName);
			internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaLast) | uint32_t(kCGBitmapByteOrderDefault));
			break;

		default:
		{
			// the pixel format of the frame cannot be written immediately
			// thus, we try to convert it to either RGB24 or RGBA32 (in case an alpha channel exists)

			if (FrameType::formatHasAlphaChannel(pixelFormat))
			{
				internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
				internalTargetPixelFormat = FrameType::FORMAT_RGBA32;
				internalBitmapInfo = CGBitmapInfo(uint32_t(kCGImageAlphaLast) | uint32_t(kCGBitmapByteOrderDefault));
			}
			else
			{
				internalColorSpace = determineColorSpace(FrameType::FORMAT_RGB24, colorProfileName);
				internalTargetPixelFormat = FrameType::FORMAT_RGB24;
			}

			break;
		}
	}

	if (internalColorSpace == nullptr)
	{
		return false;
	}

	colorSpace = ScopedCGColorSpaceRef(internalColorSpace);
	bitmapInfo = internalBitmapInfo;
	targetPixelFormat = internalTargetPixelFormat;

	return true;
}

CGColorSpaceRef IIOObject::determineColorSpace(const FrameType::PixelFormat mainPixelFormat, const std::string& colorProfileName)
{
	if (!colorProfileName.empty())
	{
		ocean_assert(!colorProfileName.empty());
		ocean_assert(mainPixelFormat != FrameType::FORMAT_UNDEFINED);

		static ColorProfileMap colorProfileMap =
		{
			{"DCI-P3 D65 Gamut with sRGB Transfer", {FrameType::FORMAT_RGB24, kCGColorSpaceDisplayP3}},
			{"Display P3", {FrameType::FORMAT_RGB24, kCGColorSpaceDisplayP3}},
			{"kCGColorSpaceDisplayP3", {FrameType::FORMAT_RGB24, kCGColorSpaceDisplayP3}},

			{"sRGB IEC61966-2.1", {FrameType::FORMAT_RGB24, kCGColorSpaceSRGB}},
			{"kCGColorSpaceSRGB", {FrameType::FORMAT_RGB24, kCGColorSpaceSRGB}},

			{"kCGColorSpaceExtendedSRGB", {FrameType::FORMAT_RGB24, kCGColorSpaceExtendedSRGB}},

			{"sRGB IEC61966-2.1 Linear", {FrameType::FORMAT_RGB24, kCGColorSpaceLinearSRGB}},
			{"kCGColorSpaceLinearSRGB", {FrameType::FORMAT_RGB24, kCGColorSpaceLinearSRGB}},

			{"kCGColorSpaceExtendedLinearSRGB", {FrameType::FORMAT_RGB24, kCGColorSpaceExtendedLinearSRGB}},

			{"Adobe RGB (1998)", {FrameType::FORMAT_RGB24, kCGColorSpaceAdobeRGB1998}},
			{"kCGColorSpaceAdobeRGB1998", {FrameType::FORMAT_RGB24, kCGColorSpaceAdobeRGB1998}},

			{"Generic HDR Profile", {FrameType::FORMAT_RGB24, kCGColorSpaceGenericRGBLinear}},
			{"kCGColorSpaceGenericRGBLinear", {FrameType::FORMAT_RGB24, kCGColorSpaceGenericRGBLinear}},

			{"ROMM RGB: ISO 22028-2:2013", {FrameType::FORMAT_RGB24, kCGColorSpaceROMMRGB}},
			{"kCGColorSpaceROMMRGB", {FrameType::FORMAT_RGB24, kCGColorSpaceROMMRGB}},

			{"Generic Gray Gamma 2.2 Profile", {FrameType::FORMAT_Y8, kCGColorSpaceGenericGrayGamma2_2}},
			{"kCGColorSpaceGenericGrayGamma2_2", {FrameType::FORMAT_Y8, kCGColorSpaceGenericGrayGamma2_2}},
			{"kCGColorSpaceExtendedGray", {FrameType::FORMAT_Y8, kCGColorSpaceExtendedGray}},

			{"Linear Gray", {FrameType::FORMAT_Y8, kCGColorSpaceLinearGray}},
			{"kCGColorSpaceLinearGray", {FrameType::FORMAT_Y8, kCGColorSpaceLinearGray}},
			{"kCGColorSpaceExtendedLinearGray", {FrameType::FORMAT_Y8, kCGColorSpaceExtendedLinearGray}},
		};

		ColorProfileMap::const_iterator i = colorProfileMap.find(colorProfileName);

		if (i != colorProfileMap.cend())
		{
			if (i->second.first == mainPixelFormat)
			{
				CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(i->second.second);
				ocean_assert(colorSpace != nullptr);

				return colorSpace;
			}
		}

		Log::warning() << "Color profile name '" << colorProfileName << "' could not be set, using default profile instead";
	}

	switch (mainPixelFormat)
	{
		case FrameType::FORMAT_RGB24:
			return CGColorSpaceCreateDeviceRGB();

		case FrameType::FORMAT_Y8:
			return CGColorSpaceCreateDeviceGray();

		default:
			break;
	}

	ocean_assert(false && "This should never happen!");
	return CGColorSpaceCreateDeviceRGB();
}

} // namespace ImageIO

} // namespace Media

} // namespace Ocean
