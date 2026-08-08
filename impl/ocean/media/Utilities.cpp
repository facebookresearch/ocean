/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/Utilities.h"
#include "ocean/media/BufferImage.h"
#include "ocean/media/BufferImageRecorder.h"
#include "ocean/media/Image.h"
#include "ocean/media/ImageRecorder.h"
#include "ocean/media/Manager.h"

#include <cstring>

namespace Ocean
{

namespace Media
{

Frame Utilities::loadImage(const std::string& url)
{
	const Media::ImageRef image(Media::Manager::get().newMedium(url, Media::Medium::IMAGE));

	if (image.isNull())
	{
		return Frame();
	}

	image->start();

	const FrameRef frame(image->frame());

	if (frame.isNull())
	{
		return Frame();
	}

	return Frame(*frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
}

Frame Utilities::loadImage(const void* imageBuffer, const size_t imageBufferSize, const std::string& imageBufferTypeIn, std::string* imageBufferTypeOut)
{
	const Media::BufferImageRef image(Media::Manager::get().newMedium("Buffer Image", Media::Medium::BUFFER_IMAGE, true));

	if (image.isNull())
	{
		return Frame();
	}

	if (!image->setBufferImage(imageBuffer, imageBufferSize, imageBufferTypeIn))
	{
		return Frame();
	}

	image->start();

	const FrameRef frame(image->frame());

	if (frame.isNull())
	{
		return Frame();
	}

	if (imageBufferTypeOut)
	{
		*imageBufferTypeOut = image->getImageBufferType();
	}

	return Frame(*frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
}

bool Utilities::saveImage(const Frame& frame, const std::string& url, const bool addTimeSuffix)
{
	const Media::ImageRecorderRef imageRecorder(Media::Manager::get().newRecorder(Media::Recorder::IMAGE_RECORDER));

	if (imageRecorder.isNull())
	{
		return false;
	}

	imageRecorder->setFilenameSuffixed(addTimeSuffix);
	return imageRecorder->saveImage(frame, url);
}

bool Utilities::saveImage(const Frame& frame, const std::string& imageType, std::vector<uint8_t>& buffer)
{
	const Media::BufferImageRecorderRef imageRecorder(Media::Manager::get().newRecorder(Media::Recorder::BUFFER_IMAGE_RECORDER));

	if (imageRecorder.isNull())
	{
		return false;
	}

	return imageRecorder->saveImage(frame, imageType, buffer);
}

void Utilities::encodeFrame(const Frame& frame, const std::string& imageType, Buffer& buffer)
{
	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + 8 + imageType.size());

	const uint64_t bytesType = uint64_t(imageType.size());
	memcpy(buffer.data() + offset, &bytesType, sizeof(bytesType));

	memcpy(buffer.data() + offset + 8, imageType.c_str(), imageType.size());

	if (frame.isValid())
	{
		Media::Utilities::saveImage(frame, imageType, buffer);
	}

	const uint64_t bytesData = uint64_t(buffer.size() - offset - 8 - 8 - imageType.size());
	memcpy(buffer.data() + offset + 8 + imageType.size(), &bytesData, sizeof(bytesData));
}

bool Utilities::decodeFrame(const uint8_t*& data, size_t& size, Frame& frame)
{
	if (size < 16)
	{
		return false;
	}

	uint64_t bytesTypeValue = 0ull;
	memcpy(&bytesTypeValue, data, sizeof(bytesTypeValue));

	if (bytesTypeValue > uint64_t(size - 16))
	{
		return false;
	}

	const size_t bytesType = size_t(bytesTypeValue);

	std::string frameType((char*)data + 8, bytesType);

	uint64_t bytesDataValue = 0ull;
	memcpy(&bytesDataValue, data + 8 + bytesType, sizeof(bytesDataValue));

	if (bytesDataValue > uint64_t(size - 16 - bytesType))
	{
		return false;
	}

	const size_t bytesData = size_t(bytesDataValue);

	frame = loadImage(data + 8 + bytesType + 8, bytesData, frameType);

	data += 8 + bytesType + 8 + bytesData;
	size -= 8 + bytesType + 8 + bytesData;

	return true;
}

bool Utilities::parseResolution(const std::string& resolution, unsigned int& width, unsigned int& height)
{
	ocean_assert(resolution.size() >= 3);
	if (resolution.size() < 3)
	{
		return false;
	}

	const std::string::size_type pos = resolution.find('x');
	if (pos == std::string::npos)
	{
		return false;
	}

	int32_t signedWidth = -1;
	if (!String::isInteger32(resolution.substr(0, pos), &signedWidth) || signedWidth <= 0)
	{
		return false;
	}

	int32_t signedHeight = -1;
	if (!String::isInteger32(resolution.substr(pos + 1), &signedHeight) || signedHeight <= 0)
	{
		return false;
	}

	width = (unsigned int)(signedWidth);
	height = (unsigned int)(signedHeight);

	return true;
}

}

}
