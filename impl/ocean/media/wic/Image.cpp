/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/Image.h"
#include "ocean/media/wic/WICLibrary.h"
#include "ocean/media/wic/WICObject.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

Frame Image::decodeImage(const void* buffer, const size_t size, const std::string& /*imageBufferTypeIn*/, std::string* imageBufferTypeOut)
{
	ocean_assert(buffer && size != 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	Frame result;

	bool noError = true;

	WICLibrary::ComInitializer::get().initialize();

	ScopedIWICImagingFactory imagingFactory;
	if (noError && S_OK != CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(&imagingFactory.resetObject())))
	{
		noError = false;
	}

	ScopedIWICStream stream;
	if (noError && S_OK != imagingFactory->CreateStream(&stream.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != stream->InitializeFromMemory(const_cast<BYTE*>(reinterpret_cast<const BYTE*>(buffer)), DWORD(size)))
	{
		noError = false;
	}

	ScopedIWICBitmapDecoder bitmapDecoder;
	if (noError && S_OK != imagingFactory->CreateDecoderFromStream(*stream, nullptr, WICDecodeMetadataCacheOnLoad, &bitmapDecoder.resetObject()))
	{
		noError = false;
	}

	if (noError)
	{
		result = WICObject::loadFrameFromBitmapDecoder(*imagingFactory, *bitmapDecoder);
	}

	if (noError && imageBufferTypeOut != nullptr)
	{
		GUID containerFormat;
		if (S_OK == bitmapDecoder->GetContainerFormat(&containerFormat))
		{
			*imageBufferTypeOut = WICObject::translateContainerFormat(containerFormat);
		}
	}

	return result;
}

bool Image::encodeImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer, const bool allowConversion, bool* hasBeenConverted)
{
	if (!frame.isValid())
	{
		return false;
	}

	bool noError = true;

	WICLibrary::ComInitializer::get().initialize();

	const GUID containerFormat = WICObject::findContainerFormat(imageType);

	if (containerFormat == GUID_NULL)
	{
		return false;
	}

	ScopedIWICImagingFactory imagingFactory;
	if (noError && S_OK != CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(&imagingFactory.resetObject())))
	{
		noError = false;
	}

	ScopedIWICStream stream;
	if (noError && S_OK != imagingFactory->CreateStream(&stream.resetObject()))
	{
		noError = false;
	}

	ScopedIStream memoryStream;
	if (noError && S_OK != CreateStreamOnHGlobal(nullptr, TRUE, &memoryStream.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != stream->InitializeFromIStream(*memoryStream))
	{
		noError = false;
	}

	ScopedIWICBitmapEncoder bitmapEncoder;
	if (noError && S_OK != imagingFactory->CreateEncoder(containerFormat, nullptr, &bitmapEncoder.resetObject()))
	{
		noError = false;
	}

	if (noError && S_OK != bitmapEncoder->Initialize(*stream, WICBitmapEncoderNoCache))
	{
		noError = false;
	}

	if (noError && !WICObject::writeFrameToBitmapDecoder(*imagingFactory, *bitmapEncoder, frame, allowConversion, hasBeenConverted))
	{
		noError = false;
	}

	if (noError)
	{
		static_assert(sizeof(ULARGE_INTEGER) == sizeof(unsigned long long), "Invalid data type!");

		LARGE_INTEGER offset = {};
		unsigned long long size = 0ull;
		if (noError && S_OK != memoryStream->Seek(offset, STREAM_SEEK_END, (ULARGE_INTEGER*)&size))
		{
			noError = false;
		}

		if (noError && S_OK != memoryStream->Seek(offset, STREAM_SEEK_SET, nullptr))
		{
			noError = false;
		}

		buffer.resize(size_t(size));

		ULONG bytes = 0;
		if (noError && (S_OK != memoryStream->Read(buffer.data(), ULONG(buffer.size()), &bytes) || bytes != buffer.size()))
		{
			buffer.clear();
			noError = false;
		}
	}

	return noError;
}

Frame Image::readImage(const std::string& filename)
{
	ocean_assert(!filename.empty());
	if (filename.empty())
	{
		return Frame();
	}

	Frame result;

	bool noError = true;

	WICLibrary::ComInitializer::get().initialize();

	ScopedIWICImagingFactory imagingFactory;
	if (noError && S_OK != CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(&imagingFactory.resetObject())))
	{
		noError = false;
	}

	ScopedIWICBitmapDecoder bitmapDecoder;
	if (noError && S_OK != imagingFactory->CreateDecoderFromFilename(String::toWString(filename).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder.resetObject()))
	{
		noError = false;
	}

	if (noError)
	{
		result = WICObject::loadFrameFromBitmapDecoder(*imagingFactory, *bitmapDecoder);
	}

	return result;
}

bool Image::writeImage(const Frame& frame, const std::string& filename, const bool allowConversion, bool* hasBeenConverted)
{
	if (!frame.isValid())
	{
		return false;
	}

	bool noError = true;

	const std::string::size_type pos = filename.rfind('.');

	if (pos == std::string::npos || pos == 0)
	{
		ocean_assert(false && "The filename does not contain any valid file extension");
		return false;
	}

	WICLibrary::ComInitializer::get().initialize();

	const std::string filenameExtension = filename.substr(pos + 1);
	const GUID containerFormat = WICObject::findContainerFormat(filenameExtension);

	if (containerFormat == GUID_NULL)
	{
		return false;
	}

	{
		// own scope to ensure that the stream is released before DeleteFileW() is called

		ScopedIWICImagingFactory imagingFactory;
		if (noError && S_OK != CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(&imagingFactory.resetObject())))
		{
			noError = false;
		}

		ScopedIWICStream stream;
		if (noError && S_OK != imagingFactory->CreateStream(&stream.resetObject()))
		{
			noError = false;
		}

		if (noError && S_OK != stream->InitializeFromFilename(String::toWString(filename).c_str(), GENERIC_WRITE))
		{
			noError = false;
		}

		ScopedIWICBitmapEncoder bitmapEncoder;
		if (noError && S_OK != imagingFactory->CreateEncoder(containerFormat, nullptr, &bitmapEncoder.resetObject()))
		{
			noError = false;
		}

		if (noError && S_OK != bitmapEncoder->Initialize(*stream, WICBitmapEncoderNoCache))
		{
			noError = false;
		}

		if (noError && !WICObject::writeFrameToBitmapDecoder(*imagingFactory, *bitmapEncoder, frame, allowConversion, hasBeenConverted))
		{
			noError = false;
		}
	}

	if (!noError)
	{
		DeleteFileW(String::toWString(filename).c_str());
	}

	return noError;
}

}

}

}
