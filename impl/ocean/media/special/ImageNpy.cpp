/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/special/ImageNpy.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameTransposer.h"

namespace Ocean
{

namespace Media
{

namespace Special
{

Frame ImageNpy::decodeImage(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size > 0);

	if (buffer == nullptr || size == 0)
	{
		return Frame();
	}

	const uint8_t* remainingBuffer = (const uint8_t*)(buffer);
	size_t remainingSize = size;

	FrameType frameType;
	bool isRowMajor = false;

	if (!readHeader(remainingBuffer, remainingSize, frameType, isRowMajor))
	{
		return Frame();
	}

	ocean_assert(frameType.channels() == 1u);

	// **TODO** hot fix to ensure that images cannot be larger than 2^32 bytes

	if (frameType.bytesPerDataType() == 0u || uint64_t(frameType.width()) * uint64_t(frameType.height()) >= uint64_t(4294967294u) / uint64_t(frameType.bytesPerDataType())) // width * height * bytesPerDataType < 2^32
	{
		return Frame();
	}

	const size_t expectedPayload = frameType.frameTypeSize();

	if (remainingSize < expectedPayload)
	{
		return Frame();
	}

	constexpr unsigned int sourcePaddingElements = 0u;

	if (isRowMajor)
	{
		return Frame(frameType, (const void*)(remainingBuffer), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, sourcePaddingElements);
	}

	Frame frame(frameType);

	switch (frameType.bytesPerDataType())
	{
		case 1u:
			CV::FrameTransposer::transpose<uint8_t, 1u>((const uint8_t*)(remainingBuffer), frame.data<uint8_t>(), frame.width(), frame.height(), sourcePaddingElements, frame.paddingElements(), nullptr);
			break;

		case 2u:
			CV::FrameTransposer::transpose<uint16_t, 1u>((const uint16_t*)(remainingBuffer), frame.data<uint16_t>(), frame.width(), frame.height(), sourcePaddingElements, frame.paddingElements(), nullptr);
			break;

		case 4u:
			CV::FrameTransposer::transpose<uint32_t, 1u>((const uint32_t*)(remainingBuffer), frame.data<uint32_t>(), frame.width(), frame.height(), sourcePaddingElements, frame.paddingElements(), nullptr);
			break;

		case 8u:
			CV::FrameTransposer::transpose<uint64_t, 1u>((const uint64_t*)(remainingBuffer), frame.data<uint64_t>(), frame.width(), frame.height(), sourcePaddingElements, frame.paddingElements(), nullptr);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return Frame();
	}

	return frame;
}

bool ImageNpy::encodeImage(const Frame& frame, std::vector<uint8_t>& buffer)
{
	ocean_assert(frame);

	if (!isFrameTypeSupported(frame.pixelFormat(), frame.pixelOrigin()))
	{
		return false;
	}

	std::string header = "\x93NUMPY\x01\xFF";
	header.back() = '\x00';

	const std::string pixelFormat = translatePixelFormat(frame.pixelFormat());
	ocean_assert(!pixelFormat.empty());

	std::string headerData = "{'descr': '<" + pixelFormat + "', 'fortran_order': False, 'shape': (" + String::toAString(frame.height()) + ", " + String::toAString(frame.width()) + "), }";

	const unsigned int headerPadding = (64u - ((unsigned int)(header.size() + 2 + headerData.size() + 1) % 64u)) % 64u;

	headerData.resize(headerData.size() + headerPadding + 1, ' ');
	headerData.back() = '\n';

	ocean_assert((header.size() + 2 + headerData.size()) % 64 == 0);

	const size_t totalBufferSize = header.size() + 2 + headerData.size() + frame.frameTypeSize();

	buffer.resize(totalBufferSize);
	uint8_t* data = buffer.data();

	memcpy(data, header.c_str(), header.length());
	data += header.length();

	if (headerData.length() >= 65536)
	{
		return false;
	}

	const uint16_t headerDataSize = uint16_t(headerData.length());

	memcpy(data, &headerDataSize, 2);
	data += 2;

	memcpy(data, headerData.c_str(), headerData.length());
	data += headerData.length();

	if (frame.isContinuous() && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT)
	{
		memcpy(data, frame.constdata<void>(), frame.frameTypeSize());
	}
	else
	{
		const unsigned int frameWidthBytes = frame.planeWidthBytes(0u);

		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			if (frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT)
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

bool ImageNpy::readHeader(const uint8_t*& data, size_t& size, FrameType& frameType, bool& isRowMajor)
{
	if (size < 10)
	{
		return false;
	}

	if (data[0] != 0x93 || data[1] != 'N' || data[2] != 'U' || data[3] != 'M' || data[4] != 'P' || data[5] != 'Y')
	{
		return false;
	}

	const uint8_t majorVersion = data[6];
	const uint8_t minorVersion = data[7];

	OCEAN_SUPPRESS_UNUSED_WARNING(majorVersion);
	OCEAN_SUPPRESS_UNUSED_WARNING(minorVersion);

	uint16_t headerLength;
	memcpy(&headerLength, data + 8, 2);

	const int32_t littleEndianValue = 1;
	if (*(int8_t*)&littleEndianValue != int8_t(1))
	{
		ocean_assert(false && "This must never happen as we expect always a little endian system!");
		return false;
	}

	if (headerLength == 0 || headerLength >= size - 10)
	{
		return false;
	}

	static_assert(sizeof(uint8_t) == sizeof(char), "Invalid datatype!");

	const std::string headerData((char*)(data + 10), (char*)(data + 10 + headerLength));

	if (headerData.back() != '\n')
	{
		return false;
	}

	// parsing descr:
	// 'descr:' '<f4'

	const std::string::size_type positionDescr = headerData.find("'descr': '<");
	if (positionDescr == std::string::npos)
	{
		return false;
	}

	if (positionDescr + 14 >= headerData.size() || headerData[14] != '\'')
	{
		return false;
	}

	const FrameType::PixelFormat pixelFormat = translatePixelFormat(String::toLower(headerData.substr(12, 2)));

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return false;
	}

	// parsing fortran_order:
	// 1) 'fortran_order': False
	// 2) 'fortran_order': True

	const std::string::size_type positionFortranOrder = headerData.find("'fortran_order': ");
	if (positionFortranOrder == std::string::npos)
	{
		return false;
	}

	bool fortranOrder = true;

	if (headerData.substr(positionFortranOrder + 17, 4) == "True")
	{
		// nothing to do here
	}
	else if (headerData.substr(positionFortranOrder + 17, 5) == "False")
	{
		fortranOrder = false;
	}
	else
	{
		return false;
	}

	// parsing shape:
	// 'shape': (740, 1280)

	const std::string::size_type positionShape = headerData.find("'shape': (");
	if (positionShape == std::string::npos)
	{
		return false;
	}

	std::string::size_type shapeRight = std::string::npos;

	int rows = -1;
	int columns = -1;

	std::string::size_type valueStart = std::string::npos;

	for (std::string::size_type n = positionShape + 10; n < headerData.size(); ++n)
	{
		bool extractValue = false;

		if (headerData[n] == '(')
		{
			return false;
		}
		else if (headerData[n] == ')')
		{
			if (shapeRight != std::string::npos)
			{
				return false;
			}

			if (columns == -1 && valueStart != std::string::npos)
			{
				extractValue = true;
			}
			else
			{
				break;
			}
		}
		else if (headerData[n] >= '0' && headerData[n] <= '9')
		{
			if (valueStart == std::string::npos)
			{
				valueStart = n;
			}
		}
		else if (headerData[n] == ',')
		{
			if (valueStart == std::string::npos)
			{
				return false;
			}

			extractValue = true;
		}
		else if (headerData[n] == ' ')
		{
			if (valueStart != std::string::npos)
			{
				extractValue = true;
			}
		}
		else
		{
			// unexpected character
			return false;
		}

		if (extractValue)
		{
			int value = -1;
			if (!String::isInteger32(headerData.substr(valueStart, n - valueStart), &value) || value <= 0)
			{
				return false;
			}

			if (rows == -1)
			{
				rows = value;
			}
			else if (columns == -1)
			{
				columns = value;
				break;
			}
			else
			{
				return false;
			}

			valueStart = std::string::npos;
		}

		if (n + 1 ==  headerData.size())
		{
			// need closing ')'
			return false;
		}
	}

	if (rows <= 0 || columns <= 0)
	{
		return false;
	}

	frameType = FrameType((unsigned int)(columns), (unsigned int)(rows), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	data += 10 + headerLength;
	size -= 10 + headerLength;

	isRowMajor = !fortranOrder;

	return true;
}

FrameType::PixelFormat ImageNpy::translatePixelFormat(const std::string& pixelFormat)
{
	if (pixelFormat == "f4")
	{
		return FrameType::FORMAT_F32;
	}

	if (pixelFormat == "f8")
	{
		return FrameType::FORMAT_F64;
	}

	if (pixelFormat == "i1")
	{
		return FrameType::genericPixelFormat<int8_t, 1u>();
	}

	if (pixelFormat == "i2")
	{
		return FrameType::genericPixelFormat<int16_t, 1u>();
	}

	if (pixelFormat == "i4")
	{
		return FrameType::genericPixelFormat<int32_t, 1u>();
	}

	if (pixelFormat == "i8")
	{
		return FrameType::genericPixelFormat<int64_t, 1u>();
	}

	if (pixelFormat == "u1")
	{
		return FrameType::FORMAT_Y8;
	}

	if (pixelFormat == "u2")
	{
		return FrameType::FORMAT_Y16;
	}

	if (pixelFormat == "u4")
	{
		return FrameType::FORMAT_Y32;
	}

	if (pixelFormat == "u8")
	{
		return FrameType::FORMAT_Y64;
	}

	return FrameType::FORMAT_UNDEFINED;
}

std::string ImageNpy::translatePixelFormat(const FrameType::PixelFormat pixelFormat)
{
	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_F32))
	{
		return std::string("f4");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_F64))
	{
		return std::string("f8");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<int8_t, 1u>()))
	{
		return std::string("i1");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<int16_t, 1u>()))
	{
		return std::string("i2");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<int32_t, 1u>()))
	{
		return std::string("i4");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::genericPixelFormat<int64_t, 1u>()))
	{
		return std::string("i8");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_Y8))
	{
		return std::string("u1");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_Y16))
	{
		return std::string("u2");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_Y32))
	{
		return std::string("u4");
	}

	if (FrameType::arePixelFormatsCompatible(pixelFormat, FrameType::FORMAT_Y64))
	{
		return std::string("u8");
	}

	return std::string();
}

}

}

}
