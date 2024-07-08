/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICObject.h"

#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

FrameType::PixelFormat WICObject::translatePixelFormat(const WICPixelFormatGUID& format, FrameType::PixelOrigin* pixelOrigin)
{
	if (format == GUID_WICPixelFormat24bppBGR)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_BGR24;
	}
	else if (format == GUID_WICPixelFormat32bppBGRA)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_BGRA32;
	}
	else if (format == GUID_WICPixelFormat24bppRGB)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_RGB24;
	}
	else if (format == GUID_WICPixelFormat32bppRGBA)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_RGBA32;
	}
	else if (format == GUID_WICPixelFormat8bppGray)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_Y8;
	}
	else if (format == GUID_WICPixelFormat16bppGray)
	{
		if (pixelOrigin)
		{
			*pixelOrigin = FrameType::ORIGIN_UPPER_LEFT;
		}

		return FrameType::FORMAT_Y16;
	}

	return FrameType::FORMAT_UNDEFINED;
}

WICPixelFormatGUID WICObject::matchingPixelFormat(const FrameType::PixelFormat format, FrameType::PixelFormat& adjustedFormat, const std::vector<GUID>& supportedPixelFormats)
{
	adjustedFormat = FrameType::FORMAT_UNDEFINED;

	switch (format)
	{
		case FrameType::FORMAT_BGR24:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppBGR))
			{
				adjustedFormat = FrameType::FORMAT_BGR24;
				return GUID_WICPixelFormat24bppBGR;
			}

			if (hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppRGB))
			{
				adjustedFormat = FrameType::FORMAT_RGB24;
				return GUID_WICPixelFormat24bppRGB;
			}

			break;
		}

		case FrameType::FORMAT_BGRA32:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppBGRA))
			{
				adjustedFormat = FrameType::FORMAT_BGRA32;
				return GUID_WICPixelFormat32bppBGRA;
			}

			if (hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppRGBA))
			{
				adjustedFormat = FrameType::FORMAT_RGBA32;
				return GUID_WICPixelFormat32bppRGBA;
			}

			break;
		}

		case FrameType::FORMAT_RGB24:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppRGB))
			{
				adjustedFormat = FrameType::FORMAT_RGB24;
				return GUID_WICPixelFormat24bppRGB;
			}

			if (hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppBGR))
			{
				adjustedFormat = FrameType::FORMAT_BGR24;
				return GUID_WICPixelFormat24bppBGR;
			}

			break;
		}

		case FrameType::FORMAT_RGBA32:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppRGBA))
			{
				adjustedFormat = FrameType::FORMAT_RGBA32;
				return GUID_WICPixelFormat32bppRGBA;
			}

			if (hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppBGRA))
			{
				adjustedFormat = FrameType::FORMAT_BGRA32;
				return GUID_WICPixelFormat32bppBGRA;
			}

			break;
		}

		case FrameType::FORMAT_Y8:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat8bppGray))
			{
				adjustedFormat = FrameType::FORMAT_Y8;
				return GUID_WICPixelFormat8bppGray;
			}

			break;
		}

		case FrameType::FORMAT_Y16:
		{
			if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat16bppGray))
			{
				adjustedFormat = FrameType::FORMAT_Y16;
				return GUID_WICPixelFormat16bppGray;
			}

			break;
		}

		default:
			break;
	}

	if (FrameType::formatHasAlphaChannel(format))
	{
		if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppRGBA))
		{
			adjustedFormat = FrameType::FORMAT_RGBA32;
			return GUID_WICPixelFormat32bppRGBA;
		}

		if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat32bppBGRA))
		{
			adjustedFormat = FrameType::FORMAT_BGRA32;
			return GUID_WICPixelFormat32bppBGRA;
		}
	}
	else
	{
		if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppRGB))
		{
			adjustedFormat = FrameType::FORMAT_RGB24;
			return GUID_WICPixelFormat24bppRGB;
		}

		if (supportedPixelFormats.empty() || hasElement(supportedPixelFormats, GUID_WICPixelFormat24bppBGR))
		{
			adjustedFormat = FrameType::FORMAT_BGR24;
			return GUID_WICPixelFormat24bppBGR;
		}
	}

	return GUID_NULL;
}

bool WICObject::hasAlphaChannel(IWICImagingFactory* imagingFactor, IWICBitmapDecoder* bitmapDecoder, IWICBitmapFrameDecode* frameDecode)
{
	ocean_assert(imagingFactor && bitmapDecoder && frameDecode);

	bool noError = imagingFactor && bitmapDecoder;

	WICPixelFormatGUID format = GUID_NULL;
	if (noError && S_OK != frameDecode->GetPixelFormat(&format))
	{
		noError = false;
	}

	if (noError && (format == GUID_WICPixelFormat32bppBGRA || format == GUID_WICPixelFormat32bppRGBA))
	{
		return true;
	}

#if defined(OCEAN_COMPILER_MSC) && OCEAN_MSC_VERSION >= 1800
	else if (noError && (format == GUID_WICPixelFormat24bppBGR || format == GUID_WICPixelFormat24bppRGB || format == GUID_WICPixelFormat32bppBGR || format == GUID_WICPixelFormat32bppRGB))
	{
		return false;
	}
#else
	else if (noError && (format == GUID_WICPixelFormat24bppBGR || format == GUID_WICPixelFormat24bppRGB || format == GUID_WICPixelFormat32bppBGR))
	{
		return false;
	}
#endif

	else if (noError && (format == GUID_WICPixelFormat1bppIndexed || format == GUID_WICPixelFormat2bppIndexed || format == GUID_WICPixelFormat4bppIndexed || format == GUID_WICPixelFormat8bppIndexed))
	{
		IWICPalette* palette = nullptr;
		if (noError && S_OK != imagingFactor->CreatePalette(&palette))
		{
			noError = false;
		}

		// the palette can be stored in the decoder or in the individual frames

		if (noError && S_OK != bitmapDecoder->CopyPalette(palette))
		{
			if (S_OK != frameDecode->CopyPalette(palette))
			{
				noError = false;
			}
		}

		BOOL value = FALSE;
		if (noError && S_OK != palette->HasAlpha(&value))
		{
			noError = true;
		}

		const bool result = value == TRUE;

		if (palette)
		{
			palette->Release();
		}

		return result;
	}

	ocean_assert(false && "Unknown format!");
	return false;
}

GUID WICObject::findContainerFormat(const std::string& fileExtension)
{
	ocean_assert(!fileExtension.empty());
	if (fileExtension.empty())
	{
		return GUID_NULL;
	}

	const std::string lowerExtension = String::toLower(fileExtension);

	if (lowerExtension == std::string("bmp"))
	{
		return GUID_ContainerFormatBmp;
	}

	if (lowerExtension == std::string("gif"))
	{
		return GUID_ContainerFormatGif;
	}

	if (lowerExtension == std::string("jpg") || lowerExtension == std::string("jpeg"))
	{
		return GUID_ContainerFormatJpeg;
	}

	if (lowerExtension == std::string("png"))
	{
		return GUID_ContainerFormatPng;
	}

	if (lowerExtension == std::string("tif") || lowerExtension == std::string("tiff"))
	{
		return GUID_ContainerFormatTiff;
	}

	if (lowerExtension == std::string("wmp"))
	{
		return GUID_ContainerFormatWmp;
	}

	return GUID_NULL;
}

std::string WICObject::translateContainerFormat(const GUID& containerFormat)
{
	if (IsEqualGUID(containerFormat, GUID_ContainerFormatBmp))
	{
		return std::string("bmp");
	}

	if (IsEqualGUID(containerFormat, GUID_ContainerFormatGif))
	{
		return std::string("gif");
	}

	if (IsEqualGUID(containerFormat, GUID_ContainerFormatJpeg))
	{
		return std::string("jpg");
	}

	if (IsEqualGUID(containerFormat, GUID_ContainerFormatPng))
	{
		return std::string("png");
	}

	if (IsEqualGUID(containerFormat, GUID_ContainerFormatTiff))
	{
		return std::string("tif");
	}

	if (IsEqualGUID(containerFormat, GUID_ContainerFormatWmp))
	{
		return std::string("wmp");
	}

	return std::string();
}

Frame WICObject::loadFrameFromBitmapDecoder(IWICImagingFactory* imagingFactory, IWICBitmapDecoder* bitmapDecoder)
{
	ocean_assert(imagingFactory != nullptr && bitmapDecoder != nullptr);
	if (imagingFactory == nullptr || bitmapDecoder == nullptr)
	{
		return Frame();
	}

	Frame result;

	bool noError = true;

	UINT frameCount = 0u;

	if (noError && S_OK != bitmapDecoder->GetFrameCount(&frameCount))
	{
		noError = false;
	}

	if (noError && frameCount >= 1u)
	{
		IWICBitmapFrameDecode* bitmapFrameDecode = nullptr;
		if (noError && S_OK != bitmapDecoder->GetFrame(0u, &bitmapFrameDecode))
		{
			noError = false;
		}

		WICPixelFormatGUID wicPixelFormat = GUID_NULL;
		if (noError && S_OK != bitmapFrameDecode->GetPixelFormat(&wicPixelFormat))
		{
			noError = false;
		}

		FrameType::PixelOrigin pixelOrigin = FrameType::ORIGIN_INVALID;
		FrameType::PixelFormat pixelFormat = translatePixelFormat(wicPixelFormat, &pixelOrigin);

		if (pixelFormat != FrameType::FORMAT_UNDEFINED && pixelOrigin != FrameType::ORIGIN_INVALID)
		{
			UINT width = 0u;
			UINT height = 0u;
			if (noError && S_OK != bitmapFrameDecode->GetSize(&width, &height))
			{
				noError = false;
			}

			if (noError && width >= 1u && height >= 1u)
			{
				Frame tmpFrame(FrameType(width, height, pixelFormat, pixelOrigin), Indices32(), Timestamp(true));
				ocean_assert(tmpFrame.numberPlanes() == 1u);

				if (noError && S_OK != bitmapFrameDecode->CopyPixels(nullptr, tmpFrame.strideBytes(0u), tmpFrame.size(), tmpFrame.data<BYTE>()))
				{
					noError = false;
				}

				if (noError)
				{
					result = std::move(tmpFrame);
				}
			}
		}
		else
		{
			const bool hasAlpha = hasAlphaChannel(imagingFactory, bitmapDecoder, bitmapFrameDecode);

			IWICFormatConverter* formatConverter = nullptr;
			if (noError && S_OK != imagingFactory->CreateFormatConverter(&formatConverter))
			{
				noError = false;
			}

			const WICPixelFormatGUID wicTargetPixelFormat = hasAlpha ? GUID_WICPixelFormat32bppBGRA : GUID_WICPixelFormat24bppBGR;

			BOOL canConvert = FALSE;
			if (noError && S_OK != formatConverter->CanConvert(wicPixelFormat, wicTargetPixelFormat, &canConvert))
			{
				noError = false;
			}

			if (noError && S_OK != formatConverter->Initialize(bitmapFrameDecode, wicTargetPixelFormat, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))
			{
				noError = false;
			}

			UINT width = 0u;
			UINT height = 0u;
			if (noError && S_OK != formatConverter->GetSize(&width, &height))
			{
				noError = false;
			}

			if (noError)
			{
				pixelOrigin = FrameType::ORIGIN_INVALID;
				pixelFormat = translatePixelFormat(wicTargetPixelFormat, &pixelOrigin);

				if (noError && width >= 1u && height >= 1u)
				{
					Frame tmpFrame(FrameType(width, height, pixelFormat, pixelOrigin), Indices32(), Timestamp(true));
					ocean_assert(tmpFrame.numberPlanes() == 1u);

					if (noError && S_OK != formatConverter->CopyPixels(nullptr, tmpFrame.strideBytes(0u), tmpFrame.size(), tmpFrame.data<BYTE>()))
					{
						noError = false;
					}

					if (noError)
					{
						result = std::move(tmpFrame);
					}
				}
			}

			if (formatConverter)
			{
				formatConverter->Release();
			}
		}

		if (bitmapFrameDecode)
		{
			bitmapFrameDecode->Release();
		}
	}

	return result;
}

bool WICObject::writeFrameToBitmapDecoder(IWICImagingFactory* imagingFactory, IWICBitmapEncoder* bitmapEncoder, const Frame& frame, const bool allowConversion, bool* hasBeenConverted)
{
	ocean_assert(imagingFactory != nullptr && bitmapEncoder != nullptr && frame.isValid());

	bool noError = true;

	IWICBitmapEncoderInfo* bitmapEncoderInfo = nullptr;
	if (noError && S_OK != bitmapEncoder->GetEncoderInfo(&bitmapEncoderInfo))
	{
		noError = false;
	}

	UINT numberSupportedPixelFormats = 0u;
	if (noError && S_OK != bitmapEncoderInfo->GetPixelFormats(0, nullptr, &numberSupportedPixelFormats))
	{
		noError = false;
	}

	std::vector<GUID> supportedPixelFormats(numberSupportedPixelFormats);
	if (noError && (S_OK != bitmapEncoderInfo->GetPixelFormats(numberSupportedPixelFormats, supportedPixelFormats.data(), &numberSupportedPixelFormats) || supportedPixelFormats.size() != numberSupportedPixelFormats))
	{
		noError = false;
	}

	FrameType::PixelFormat targetPixelFormat = frame.pixelFormat();
	WICPixelFormatGUID wicPixelFormat = matchingPixelFormat(frame.pixelFormat(), targetPixelFormat, supportedPixelFormats);

	if (wicPixelFormat == GUID_NULL)
	{
		noError = false;
	}

	ocean_assert(frame.numberPlanes() == 1u);

	IWICBitmapFrameEncode* bitmapFrameEncode = nullptr;
	IPropertyBag2* propertyBag = nullptr;

	if (noError && S_OK != bitmapEncoder->CreateNewFrame(&bitmapFrameEncode, &propertyBag))
	{
		noError = false;
	}

	if (noError && S_OK != bitmapFrameEncode->Initialize(propertyBag))
	{
		noError = false;
	}

	if (noError && S_OK != bitmapFrameEncode->SetSize(frame.width(), frame.height()))
	{
		noError = false;
	}

	const FrameType targetFrameType(frame, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	if (allowConversion == false && frame.frameType() != targetFrameType)
	{
		noError = false;
	}

	Frame targetFrame;
	if (noError && !CV::FrameConverter::Comfort::convert(frame, targetFrameType.pixelFormat(), targetFrameType.pixelOrigin(), targetFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr))
	{
		noError = false;
	}

	if (noError && S_OK != bitmapFrameEncode->SetPixelFormat(&wicPixelFormat))
	{
		noError = false;
	}

	if (noError)
	{
		const HRESULT result = bitmapFrameEncode->WritePixels(targetFrame.height(), targetFrame.strideBytes(0u), targetFrame.size(), targetFrame.data<BYTE>());

		if (result == WINCODEC_ERR_PALETTEUNAVAILABLE)
		{
			IWICPalette* palette = nullptr;
			if (noError && S_OK != imagingFactory->CreatePalette(&palette))
			{
				noError = false;
			}

			WICBitmapPaletteType paletteType = WICBitmapPaletteTypeCustom;
			bool paletteTypeAlpha = false;

			switch (targetFrame.pixelFormat())
			{
				case FrameType::FORMAT_BGR24:
				case FrameType::FORMAT_RGB24:
					paletteType = WICBitmapPaletteTypeFixedHalftone256;
					break;

				case FrameType::FORMAT_BGRA32:
				case FrameType::FORMAT_RGBA32:
					paletteType = WICBitmapPaletteTypeFixedHalftone256;
					paletteTypeAlpha = true;
					break;

				case FrameType::FORMAT_Y8:
					paletteType = WICBitmapPaletteTypeFixedGray256;
					break;

				case FrameType::FORMAT_YA16:
					paletteType = WICBitmapPaletteTypeFixedGray256;
					paletteTypeAlpha = true;
					break;

				default:
					noError = false;
					break;
			}

			if (noError && S_OK != palette->InitializePredefined(paletteType, paletteTypeAlpha ? TRUE : FALSE))
			{
				noError = false;
			}

			if (noError && S_OK != bitmapFrameEncode->SetPalette(palette))
			{
				noError = false;
			}

			if (noError && S_OK != bitmapFrameEncode->WritePixels(targetFrame.height(), targetFrame.strideBytes(0u), targetFrame.size(), targetFrame.data<BYTE>()))
			{
				noError = false;
			}
		}
		else if (result != S_OK)
		{
			noError = false;
		}
	}

	if (noError && S_OK != bitmapFrameEncode->Commit())
	{
		noError = false;
	}

	if (noError && S_OK != bitmapEncoder->Commit())
	{
		noError = false;
	}

	if (propertyBag)
	{
		propertyBag->Release();
	}

	if (bitmapFrameEncode)
	{
		bitmapFrameEncode->Release();
	}

	if (hasBeenConverted)
	{
		*hasBeenConverted = noError && frame.frameType() != targetFrameType;
	}

	return noError;
}

}

}

}
