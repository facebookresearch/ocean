/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/maintenanceviewer/win/MVMaintenance.h"

#include "ocean/io/Utilities.h"

#include "ocean/media/wic/Image.h"

bool MVMaintenance::decodeEnvironment(const Maintenance::Buffer& buffer, Frame& frame, Vectors2& frameImagePoints, Vectors3& frameObjectPoints, HomogenousMatrix4& framePose, Vectors3& objectPoints)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeFrame(data, size, frame) && IO::Utilities::decodeVectors2(data, size, frameImagePoints) && IO::Utilities::decodeVectors3(data, size, frameObjectPoints) && IO::Utilities::decodeHomogenousMatrix4(data, size, framePose) && IO::Utilities::decodeVectors3(data, size, objectPoints);
}

bool MVMaintenance::decodeFrame(const uint8_t*& data, size_t& size, Frame& frame)
{
	static_assert(sizeof(uint64_t) == 8, "Invalid data type!");

	// 8 byte: size of image type
	// image type
	// 8 byte: size of image buffer
	// image buffer

	if (size < 16)
	{
		return false;
	}

	uint64_t imageTypeSize;
	memcpy(&imageTypeSize, data, 8);

	if (imageTypeSize > size - 16)
	{
		return false;
	}

	if (!NumericT<size_t>::isInsideValueRange(imageTypeSize))
	{
		return false;
	}

	std::string frameType((char*)data + 8, size_t(imageTypeSize));

	uint64_t imageBufferSize;
	memcpy(&imageBufferSize, data + 8 + imageTypeSize, 8);

	if (8 + imageTypeSize + 8 + imageBufferSize > size)
	{
		return false;
	}

	if (!NumericT<size_t>::isInsideValueRange(imageBufferSize))
	{
		return false;
	}

	frame = Media::WIC::Image::decodeImage(data + 8 + imageTypeSize + 8, size_t(imageBufferSize), frameType);

	data += 8 + imageTypeSize + 8 + imageBufferSize;
	size -= 8 + size_t(imageTypeSize) + 8 + size_t(imageBufferSize);

	return true;
}
