/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/special/ImageOcn.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

Frame ImageOcn::decodeImage(const void* buffer, const size_t size)
{
	if (buffer == nullptr || size <= 120 + 24) // header + 1 plane
	{
		return Frame();
	}

#ifndef OCEAN_LITTLE_ENDIAN
	static_assert(false, "Invalid platform!");
	return false;
#endif

	const uint8_t* data = (const uint8_t*)(buffer);
	size_t remainingSize = size;

	std::string magicNumber;
	if (!readValue(data, remainingSize, headerMagicNumberSize_, magicNumber))
	{
		return Frame();
	}

	uint32_t version = 0u;
	if (!readValue(data, remainingSize, version))
	{
		return Frame();
	}

	if (version != 1u)
	{
		return Frame();
	}

	uint64_t width = 0ull;
	if (!readValue(data, remainingSize, width))
	{
		return Frame();
	}

	uint64_t height = 0ull;
	if (!readValue(data, remainingSize, height))
	{
		return Frame();
	}

	std::string pixelFormatString;
	if (!readValue(data, remainingSize, headerPixelFormatSize_, pixelFormatString))
	{
		return Frame();
	}

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	if (pixelFormatString != "GENERIC")
	{
		pixelFormat = FrameType::translatePixelFormat(pixelFormatString);

		if (pixelFormat == FrameType::FORMAT_UNDEFINED)
		{
			return Frame();
		}
	}

	std::string pixelOriginString;
	if (!readValue(data, remainingSize, headerPixelOriginSize_, pixelOriginString))
	{
		return Frame();
	}

	const FrameType::PixelOrigin pixelOrigin = FrameType::translatePixelOrigin(pixelOriginString);

	if (pixelOrigin == FrameType::ORIGIN_INVALID)
	{
		return Frame();
	}

	std::string dataTypeString;
	if (!readValue(data, remainingSize, headerDataTypeSize_, dataTypeString))
	{
		return Frame();
	}

	const FrameType::DataType dataType = FrameType::translateDataType(dataTypeString);

	if (dataType == FrameType::DT_UNDEFINED)
	{
		return Frame();
	}

	uint64_t numberPlanes = 0ull;
	if (!readValue(data, remainingSize, numberPlanes))
	{
		return Frame();
	}

	if (numberPlanes == 0ull || numberPlanes > 32ull)
	{
		return Frame();
	}

	if (pixelFormat != FrameType::FORMAT_UNDEFINED)
	{
		// we need to ensure that the pixel format matches with the number of planes and the data type

		if (uint64_t(FrameType::numberPlanes(pixelFormat)) != numberPlanes || dataType != FrameType::dataType(pixelFormat))
		{
			return Frame();
		}
	}

	if (width >= uint64_t(NumericT<uint32_t>::maxValue()) || height >= uint64_t(NumericT<uint32_t>::maxValue()))
	{
		return Frame();
	}

	FrameType frameType;
	Frame frame;

	for (uint64_t planeIndex = 0ull; planeIndex < numberPlanes; ++planeIndex)
	{
		uint64_t planeWidth = 0ull;
		if (!readValue(data, remainingSize, planeWidth))
		{
			return Frame();
		}

		uint64_t planeHeight = 0ull;
		if (!readValue(data, remainingSize, planeHeight))
		{
			return Frame();
		}

		uint64_t planeChannels = 0ull;
		if (!readValue(data, remainingSize, planeChannels))
		{
			return Frame();
		}

		if (planeIndex == 0ull)
		{
			if (pixelFormat == FrameType::FORMAT_UNDEFINED)
			{
				if (planeChannels > 32ull || numberPlanes != 1ull)
				{
					return Frame();
				}

				pixelFormat = FrameType::genericPixelFormat(dataType, uint32_t(planeChannels));
			}

			frameType = FrameType((unsigned int)(width), (unsigned int)(height), pixelFormat, pixelOrigin);

			if (!frameType.isValid())
			{
				return Frame();
			}

			if (!frame.set(frameType, true /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return Frame();
			}
		}

		if (planeWidth != uint64_t(frame.planeWidth((unsigned int)(planeIndex)))
				|| planeHeight != uint64_t(frame.planeHeight((unsigned int)(planeIndex)))
				|| planeChannels != uint64_t(frame.planeChannels((unsigned int)(planeIndex))))
		{
			return Frame();
		}
	}

	if (!frame.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		ocean_assert(frame.paddingElements(planeIndex) == 0u);

		const size_t planeSize = size_t(frame.size(planeIndex));

		if (remainingSize < planeSize)
		{
			return Frame();
		}

		memcpy(frame.data<void>(planeIndex), data, planeSize);

		data += planeSize;
		remainingSize -= planeSize;
	}

	ocean_assert(remainingSize < size);

	return frame;
}

bool ImageOcn::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer)
{
	if (!frame.isValid())
	{
		return false;
	}

#ifndef OCEAN_LITTLE_ENDIAN
	static_assert(false, "Invalid platform!");
	return false;
#endif

	// data layout

	// Header:
	//  4: Magic number 'ocn\0'
	//  4: Version number
	//  8: Frame width in pixel
	//  8: Frame height in pixel
	// 40: Pixel format
	// 16: Pixel origin
	// 32: Data type
	//  8: Planes
	//  8: Plane width in pixel for plane 0
	//  8: Plane height in pixel for plane 0
	//  8: Channels for plane 0
	//  8: Plane width in pixel for plane 1
	//  8: Plane height in pixel for plane 1
	//  8: Channels for plane 1
	//  ...
	//  8: Plane width in pixel for plane n-1
	//  8: Plane height in pixel for plane n-1
	//  8: Channels for plane n-1

	// Payload
	//  X: bytes for plane 0 + bytes for plane 1, ..., bytes for plane n-1

	// Overall header:
	// 116 + 24 * number plane

	const size_t headerSize = 120 + 24 * frame.numberPlanes();
	const size_t payloadSize = frame.frameTypeSize();

	ocean_assert(headerMagicNumberSize_ + headerPixelFormatSize_ + headerPixelOriginSize_ + headerDataTypeSize_ < headerSize);

	buffer.resize(headerSize + payloadSize);

	uint8_t* data = buffer.data();
	size_t remainingSize = buffer.size();

	if (!writeValue(std::string("ocn"), headerMagicNumberSize_, data, remainingSize))
	{
		return false;
	}

	if (!writeValue(uint32_t(1u), data, remainingSize))
	{
		return false;
	}

	if (!writeValue(uint64_t(frame.width()), data, remainingSize))
	{
		return false;
	}

	if (!writeValue(uint64_t(frame.height()), data, remainingSize))
	{
		return false;
	}

	const std::string pixelFormat = FrameType::formatIsPureGeneric(frame.pixelFormat()) ? "GENERIC" : FrameType::translatePixelFormat(frame.pixelFormat());
	const std::string pixelOrigin = FrameType::translatePixelOrigin(frame.pixelOrigin());

	if (pixelFormat == "UNDEFINED" || pixelOrigin == "INVALID")
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!writeValue(pixelFormat, headerPixelFormatSize_, data, remainingSize))
	{
		return false;
	}

	if (!writeValue(pixelOrigin, headerPixelOriginSize_, data, remainingSize))
	{
		return false;
	}

	const std::string dataType = FrameType::translateDataType(frame.dataType());

	if (!writeValue(dataType, headerDataTypeSize_, data, remainingSize))
	{
		return false;
	}

	if (!writeValue(uint64_t(frame.numberPlanes()), data, remainingSize))
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		if (!writeValue(uint64_t(frame.planeWidth(planeIndex)), data, remainingSize))
		{
			return false;
		}

		if (!writeValue(uint64_t(frame.planeHeight(planeIndex)), data, remainingSize))
		{
			return false;
		}

		if (!writeValue(uint64_t(frame.planeChannels(planeIndex)), data, remainingSize))
		{
			return false;
		}
	}

	ocean_assert(headerSize == buffer.size() - remainingSize);

	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		const size_t planeSize = size_t(frame.planeHeight(planeIndex)) * size_t(frame.planeWidthBytes(planeIndex));

		if (remainingSize < planeSize)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (frame.paddingElements(planeIndex) == 0u)
		{
			memcpy(data, frame.constdata<void>(planeIndex), planeSize);

			data += planeSize;
		}
		else
		{
			const size_t planeWidthBytes = size_t(frame.planeWidthBytes(planeIndex));

			for (unsigned int y = 0u; y < frame.planeHeight(planeIndex); ++y)
			{
				memcpy(data, frame.constrow<void>(y, planeIndex), planeWidthBytes);

				data += planeWidthBytes;
			}
		}

		remainingSize -= planeSize;
	}

	if (remainingSize != 0)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

template <typename T>
bool ImageOcn::readValue(const uint8_t*& data, size_t& remainingSize, T& value)
{
	static_assert(!std::is_same<T, std::string>::value, "Invalid type!");

	ocean_assert(data != nullptr);

	if (remainingSize < sizeof(T))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	memcpy(&value, data, sizeof(T));
	data += sizeof(T);
	remainingSize -= sizeof(T);

	return true;
}

bool ImageOcn::readValue(const uint8_t*& data, size_t& remainingSize, const size_t numberCharacters, std::string& value)
{
	if (value.size() >= numberCharacters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (remainingSize < numberCharacters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	size_t charactersToCopy = 0;

	while (charactersToCopy < numberCharacters)
	{
		if (data[charactersToCopy] == uint8_t('\0'))
		{
			break;
		}

		++charactersToCopy;
	}

	value = std::string((const char*)(data), charactersToCopy);

	data += numberCharacters;
	remainingSize -= numberCharacters;

	return true;
}

template <typename T>
bool ImageOcn::writeValue(const T& value, uint8_t*& data, size_t& remainingSize)
{
	static_assert(!std::is_same<T, std::string>::value, "Invalid type!");

	ocean_assert(data != nullptr);

	if (remainingSize < sizeof(T))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	memcpy(data, &value, sizeof(T));
	data += sizeof(T);
	remainingSize -= sizeof(T);

	return true;
}

bool ImageOcn::writeValue(const std::string& value, const size_t numberCharacters, uint8_t*& data, size_t& remainingSize)
{
	if (value.size() >= numberCharacters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (remainingSize < numberCharacters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	memset(data, 0, numberCharacters);
	memcpy(data, value.c_str(), value.size());

	data += numberCharacters;
	remainingSize -= numberCharacters;

	return true;
}

}

}

}
