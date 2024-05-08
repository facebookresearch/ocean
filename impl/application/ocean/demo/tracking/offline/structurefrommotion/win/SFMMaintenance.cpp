/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMMaintenance.h"

#include "ocean/io/Utilities.h"

#include "ocean/media/wic/Image.h"

Maintenance::Buffer SFMMaintenance::encodeEnvironment(const Frame& frame, const Vectors2& frameImagePoints, const Vectors3& frameObjectPoints, const HomogenousMatrix4& framePose, const Vectors3& objectPoints)
{
	Maintenance::Buffer buffer;

	encodeFrame(frame, "jpg", buffer);
	IO::Utilities::encodeVectors2(frameImagePoints.data(), frameImagePoints.size(), buffer);
	IO::Utilities::encodeVectors3(frameObjectPoints.data(), frameObjectPoints.size(), buffer);
	IO::Utilities::encodeHomogenousMatrix4(framePose, buffer);
	IO::Utilities::encodeVectors3(objectPoints.data(), objectPoints.size(), buffer);

	return buffer;
}

void SFMMaintenance::encodeFrame(const Frame& frame, const std::string& imageType, Buffer& buffer)
{
	static_assert(sizeof(uint64_t) == 8, "Invalid data type!");

	// 8 byte: size of image type
	// image type
	// 8 byte: size of image buffer
	// image buffer

	Buffer imageBuffer;

	if (frame.isValid())
	{
		Media::WIC::Image::encodeImage(frame, imageType, imageBuffer);
	}

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + imageType.size() + 8 + imageBuffer.size());

	const uint64_t imageTypeSize = uint64_t(imageType.size());
	const uint64_t imageBufferSize = uint64_t(imageBuffer.size());

	memcpy(buffer.data() + offset, &imageTypeSize, 8);
	memcpy(buffer.data() + offset + 8, imageType.c_str(), imageType.size());

	if (!NumericT<size_t>::isInsideValueRange(imageBufferSize))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	memcpy(buffer.data() + offset + 8 + imageTypeSize, &imageBufferSize, 8);
	memcpy(buffer.data() + offset + 8 + imageTypeSize + 8, imageBuffer.data(), size_t(imageBufferSize));

	ocean_assert(buffer.data() + offset + 8 + imageTypeSize + 8 + imageBuffer.size() == buffer.data() + buffer.size());
}
