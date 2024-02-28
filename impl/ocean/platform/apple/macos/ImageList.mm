// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/apple/macos/ImageList.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

ImageList::ImageList(const Frame& frame, const double scaleFactor)
{
	ocean_assert(frame && frame.width() % frame.height() == 0u);
	ocean_assert(frame.numberPlanes() == 1u);
	ocean_assert(scaleFactor > 0.0);

	if (frame.isNull() || frame.numberPlanes() != 1u || scaleFactor <= 0.0)
	{
		return;
	}

	ocean_assert(frame.height() > 0u && frame.height() > 0u);
	const unsigned int images = frame.width() / frame.height();
	const unsigned int imageSize = frame.height();

	ocean_assert(imageSize * images == frame.width());
	if (imageSize * images != frame.width())
	{
		return;
	}

	listImages.reserve(imageSize);

	Frame intermediateFrame(FrameType(frame, imageSize, imageSize));

	for (unsigned int n = 0u; n < images; ++n)
	{
		intermediateFrame.copy(n * imageSize, 0u, frame);

		Image image(intermediateFrame, scaleFactor);

		if (!image.isValid())
		{
			ocean_assert(false && "Failed to create an image!");
			return;
		}

		listImages.push_back(std::move(image));
	}
}

}

}

}

}
