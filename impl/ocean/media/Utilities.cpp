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
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + 8 + imageType.size());

	((unsigned long long*)(buffer.data() + offset))[0] = (unsigned long long)imageType.size();
	memcpy(buffer.data() + offset + 8, imageType.c_str(), imageType.size());

	if (frame.isValid())
	{
		Media::Utilities::saveImage(frame, imageType, buffer);
	}

	((unsigned long long*)(buffer.data() + offset + 8 + imageType.size()))[0] = (unsigned long long)(buffer.size() - offset - 8 - 8 - imageType.size());
}

bool Utilities::decodeFrame(const uint8_t*& data, size_t& size, Frame& frame)
{
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

	if (size < 16)
		return false;

	const size_t bytesType = size_t(((unsigned long long*)data)[0]);

	if (bytesType > size - 16)
		return false;

	std::string frameType((char*)data + 8, bytesType);

	const size_t bytesData = size_t(((unsigned long long*)(data + 8 + bytesType))[0]);

	if (8 + bytesType + 8 + bytesData > size)
	{
		return false;
	}

	frame = loadImage(data + 8 + bytesType + 8, bytesData, frameType);

	data += 8 + bytesType + 8 + bytesData;
	size -= 8 + bytesType + 8 + bytesData;

	return true;
}

}

}
