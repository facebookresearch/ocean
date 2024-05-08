/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/special/ImagePfm.h"

#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

Frame ImagePfm::decodeImage(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	const uint8_t* remainingBuffer = (const uint8_t*)(buffer);
	size_t remainingSize = size;

	FrameType frameType;
	bool isLittleEndian = true;

	if (!readHeader(remainingBuffer, remainingSize, frameType, isLittleEndian))
	{
		return Frame();
	}

	const size_t expectedPayload = frameType.frameTypeSize();

	if (remainingSize < expectedPayload)
	{
		return Frame();
	}

	constexpr unsigned int sourcePaddingElements = 0u;

	if (isLittleEndian)
	{
		return Frame(frameType, (const void*)(remainingBuffer), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, sourcePaddingElements);
	}
	else
	{
		Frame frame(frameType);
		CV::FrameChannels::reverseChannelOrder<uint8_t, 4u>(remainingBuffer, (uint8_t*)(frame.data<void>()), frame.width(), frame.height(), CV::FrameConverter::CONVERT_NORMAL, sourcePaddingElements, frame.paddingElements());

		return frame;
	}
}

bool ImagePfm::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer)
{
	ocean_assert(frame);

	std::string header;

	if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::genericPixelFormat<float, 1u>()))
	{
		header = "Pf\n" + String::toAString(frame.width()) + " " + String::toAString(frame.height()) + "\n-1.0\n";
	}
	else if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::genericPixelFormat<float, 3u>()))
	{
		header = "PF\n" + String::toAString(frame.width()) + " " + String::toAString(frame.height()) + "\n-1.0\n";
	}
	else
	{
		return false;
	}

	buffer.resize(header.length() + frame.frameTypeSize());

	uint8_t* data = buffer.data();
	memcpy(data, header.c_str(), header.length());

	data += header.length();

	if (frame.isContinuous() && frame.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
	{
		memcpy(data, frame.constdata<void>(), frame.frameTypeSize());
	}
	else
	{
		const unsigned int frameWidthBytes = frame.planeWidthBytes(0u);

		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			if (frame.pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
			{
				memcpy(data, frame.constrow<void>(y), frameWidthBytes);
			}
			else
			{
				memcpy(data, frame.constrow<void>(frame.height() - y - 1u), frameWidthBytes);
			}

			data += frameWidthBytes;
		}
	}

	return true;
}

bool ImagePfm::readHeader(const uint8_t*& data, size_t& size, FrameType& frameType, bool& isLittleEndian)
{
	if (size < 3)
	{
		return false;
	}

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

	if (data[0] == 'P' && data[1] == 'F' && data[2] == '\n')
	{
		pixelFormat = FrameType::genericPixelFormat<float, 3u>();
	}
	else if (data[0] == 'P' && data[1] == 'f' && data[2] == '\n')
	{
		pixelFormat = FrameType::genericPixelFormat<float, 1u>();
	}
	else
	{
		return false;
	}

	data += 3;
	size -= 3;

	std::string stringWidth;
	std::string stringHeight;

	if (!readValue(data, size, stringWidth) || !readValue(data, size, stringHeight))
	{
		return false;
	}

	int width = 0;
	int height = 0;
	if (!String::isInteger32(stringWidth, &width) || !String::isInteger32(stringHeight, &height))
	{
		return false;
	}

	if (width <= 0 || width > 1024 * 1024 || height <= 0 || height > 1024 * 1024)
	{
		return false;
	}

	std::string stringByteOrder;
	if (!readValue(data, size, stringByteOrder))
	{
		return false;
	}

	double byteOrder = 0.0;
	if (!String::isNumber(stringByteOrder, true, &byteOrder))
	{
		return false;
	}

	frameType = FrameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_LOWER_LEFT);

	isLittleEndian = byteOrder < 0.0;

	return true;
}

bool ImagePfm::readValue(const uint8_t*&data, size_t& size, std::string& value)
{
	size_t position = 0;

	while (position < size)
	{
		if (data[position] == ' ' || data[position] == '\n')
		{
			if (position > 0)
			{
				value = std::string((const char*)(data), position);

				data += position + 1;

				ocean_assert(size >= position + 1);
				size -= position + 1;

				return true;
			}
			else
			{
				return false;
			}
		}

		++position;
	}

	return false;
}

}

}

}
