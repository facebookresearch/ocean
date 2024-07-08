/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/Bitmap.h"

#include "ocean/platform/wxwidgets/Screen.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

Bitmap::Bitmap(const Frame& frame, const bool convertIfNecessary, const double scaleFactor) :
	wxBitmap()
{
	ocean_assert(scaleFactor > 0.0);
	setFrame(frame, convertIfNecessary, scaleFactor);
}

Bitmap::Bitmap(const Frame& frame, const unsigned char* backgroundColor, const bool convertIfNecessary, const double scaleFactor) :
	wxBitmap()
{
	ocean_assert(scaleFactor > 0.0);
	setFrame(frame, backgroundColor, convertIfNecessary, scaleFactor);
}

Bitmap::Bitmap(const wxImage& image, const double scaleFactor) :
#if defined(wxABI_VERSION) && wxABI_VERSION <= 20899
	wxBitmap(image, image.HasAlpha() ? 32 : 24)
{
	ocean_assert(scaleFactor == 1.0 && "This WxWidgets version does not support scaling factors in images!");
}
#else
	wxBitmap(image, image.HasAlpha() ? 32 : 24, scaleFactor)
{
	ocean_assert(scaleFactor > 0.0);
}
#endif

bool Bitmap::setFrame(const Frame& frame, const bool convertIfNecessary, const double scaleFactor)
{
	ocean_assert(scaleFactor > 0.0);

	if (frame.isValid())
	{
		if (frame.hasAlphaChannel())
		{
			return setFrameWithAlpha(frame, convertIfNecessary, scaleFactor);
		}
		else
		{
			return setFrameWithoutAlpha(frame, convertIfNecessary, scaleFactor);
		}
	}
	else
	{
		*this = Bitmap();
		return true;
	}
}

bool Bitmap::setFrame(const Frame& frame, const unsigned char* backgroundColor, const bool convertIfNecessary, const double scaleFactor)
{
	ocean_assert(scaleFactor > 0.0);

	if (frame.isValid())
	{
		if (frame.hasAlphaChannel())
		{
			Frame intermediateFrame;
			CV::FrameBlender::blend<false, CV::FrameBlender::ATM_BLEND>(frame, intermediateFrame, backgroundColor, WorkerPool::get().conditionalScopedWorker(frame.pixels() < 400u * 400u)());

			return setFrameWithoutAlpha(intermediateFrame, convertIfNecessary, scaleFactor);
		}
		else
		{
			return setFrameWithoutAlpha(frame, convertIfNecessary, scaleFactor);
		}
	}
	else
	{
		*this = Bitmap();

		return true;
	}
}

bool Bitmap::setFrameWithoutAlpha(const Frame& frame, const bool convertIfNecessary, const double scaleFactor)
{
	ocean_assert(frame && !frame.hasAlphaChannel() && scaleFactor > 0.0);

	Frame rgbFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

	if (convertIfNecessary)
	{
		rgbFrame.release();
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
		{
			return false;
		}
	}

	if (rgbFrame.pixelFormat() != FrameType::FORMAT_RGB24 || rgbFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		return false;
	}

	Frame rgbFrameContinuous(rgbFrame, Frame::ACM_USE_KEEP_LAYOUT);
	rgbFrameContinuous.makeContinuous();

	wxImage image = wxImage(int(rgbFrameContinuous.width()), int(rgbFrameContinuous.height()), const_cast<uint8_t*>(rgbFrameContinuous.constdata<uint8_t>()), true);

#if defined(wxABI_VERSION) && wxABI_VERSION <= 20899
	*this = Bitmap(image);
#else
	*this = Bitmap(image, scaleFactor);
#endif

	return true;
}

bool Bitmap::setFrameWithAlpha(const Frame& frame, const bool convertIfNecessary, const double scaleFactor)
{
	ocean_assert(frame && frame.hasAlphaChannel() && scaleFactor > 0.0);

	Frame rgbaFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

	if (convertIfNecessary)
	{
		rgbaFrame.release();

		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT, rgbaFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
		{
			return false;
		}
	}

	if (rgbaFrame.pixelFormat() != FrameType::FORMAT_RGBA32 || rgbaFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		return false;
	}

	// we need to separate the RGB channels from the alpha channel

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(rgbaFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)()))
	{
		return false;
	}

	Frame aFrame(FrameType(rgbaFrame, FrameType::FORMAT_Y8));
	CV::FrameChannels::copyChannel<uint8_t, 4u, 1u, 3u, 0u>(rgbaFrame.constdata<uint8_t>(), aFrame.data<uint8_t>(), rgbaFrame.width(), rgbaFrame.height(), rgbaFrame.paddingElements(), aFrame.paddingElements());

	rgbFrame.makeContinuous();
	aFrame.makeContinuous();

	wxImage image = wxImage(int(rgbFrame.width()), int(rgbFrame.height()), rgbFrame.data<uint8_t>(), aFrame.data<uint8_t>(), true);

#if defined(wxABI_VERSION) && wxABI_VERSION <= 20899
	*this = Bitmap(image);
#else
	*this = Bitmap(image, scaleFactor);
#endif

	return true;
}

}

}

}
