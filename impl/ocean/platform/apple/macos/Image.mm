/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/Image.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/platform/apple/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

Image::Image(Image&& image) noexcept
{
	*this = std::move(image);
}

Image::Image(const Frame& frame, const double scaleFactor)
{
	ocean_assert(scaleFactor > 0.0);

	if (frame && scaleFactor > 0.0)
	{
		imageScaleFactor_ = scaleFactor;

		const ScopedCGImageRef imageRef(Utilities::toCGImage(frame, true));

		if (imageRef.object() != nullptr)
		{
			imageObject_ = [[NSImage alloc] initWithCGImage:imageRef.object() size:NSZeroSize];
		}
	}
}

Image::~Image()
{
	// nothing to do here
}

Image& Image::operator=(Image&& image) noexcept
{
	if (this != &image)
	{
		imageObject_ = image.imageObject_;
		imageScaleFactor_ = image.imageScaleFactor_;

		image.imageObject_ = nullptr;
		image.imageScaleFactor_ = 1.0;
	}

	return *this;
}

}

}

}

}
