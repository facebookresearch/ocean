/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/FrameMediumView.h"
#include "ocean/platform/apple/macos/Image.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

FrameMediumView::FrameMediumView(FrameMediumView&& view) noexcept :
	FrameView(std::move(view)),
	frameMedium_(std::move(view.frameMedium_)),
	frameTimestamp_(view.frameTimestamp_)
{
	// nothing to do here
}

FrameMediumView::~FrameMediumView()
{
	setFrameMedium(Media::FrameMediumRef());
}

FrameMediumView::FrameMediumView(const NSRect& rect) :
	FrameView(rect)
{
	// nothing to do here
}

FrameMediumView::FrameMediumView(const int left, const int top, const unsigned int width, const unsigned int height) :
	FrameView(left, top, width, height)
{
	// nothing to do here
}

void FrameMediumView::setFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	const ScopedLock scopedLock(lock_);

	if (frameMedium_.isNull() && frameMedium)
	{
		startThread();
	}

	const bool wasRegistered = bool(frameMedium_);

	frameMedium_ = frameMedium;

	if (wasRegistered && frameMedium_.isNull())
	{
		stopThread();
	}
}

void FrameMediumView::threadRun()
{
	while (!shouldThreadStop())
	{
		@autoreleasepool
		{
			bool haveNewFrame = false;
			FrameRef frame;

			{
				const ScopedLock scopedLock(lock_);

				if (frameMedium_)
				{
					frame = frameMedium_->frame();
					haveNewFrame = frame && frame->timestamp() != frameTimestamp_;

					if (haveNewFrame)
					{
						frameTimestamp_ = frame->timestamp();
					}
				}
			}

			if (haveNewFrame)
			{
				setFrame(*frame);
				continue;
			}
		}

		sleep(1u);
	}
}

}

}

}

}
