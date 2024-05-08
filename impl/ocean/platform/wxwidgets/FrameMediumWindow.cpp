/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/FrameMediumWindow.h"

#include "ocean/platform/wxwidgets/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

FrameMediumWindow::FrameMediumWindow(const wxString& title, wxWindow* parent, const wxPoint& pos, const wxSize& size) :
	FrameWindow(title, parent, pos, size),
	hasFinished_(false)
{
	// nothing to do here
}

FrameMediumWindow::~FrameMediumWindow()
{
	stopThread();
	stopThreadExplicitly();

	const ScopedLock scopedLock(lock_);
	frameMedium_.release();
}

void FrameMediumWindow::setFrameMedium(const Media::FrameMediumRef& medium)
{
	const ScopedLock scopedLock(lock_);

	frameMedium_ = medium;

	if (frameMedium_.isNull())
	{
		setFrame(Frame());
	}

	if (frameMedium_ && !isThreadActive())
	{
		startThread();
	}

	if (!frameMedium_)
	{
		stopThread();
	}
}

void FrameMediumWindow::threadRun()
{
	while (!shouldThreadStop())
	{
		FrameRef frame;

		TemporaryScopedLock scopedLock(lock_);

		if (frameMedium_)
		{
			const FrameRef currentFrame = frameMedium_->frame();
			scopedLock.release();

			if (currentFrame && currentFrame->timestamp() > frameTimestamp_)
			{
				frame = currentFrame;
				frameTimestamp_ = currentFrame->timestamp();
				hasFinished_ = false;
			}
			else if (frameTimestamp_.isValid() && Timestamp(true) > frameTimestamp_ + 0.15)
			{
				if (!hasFinished_ && callback_)
				{
					hasFinished_ = true;
					callback_();
				}
			}
		}

		if (frame)
		{
			setFrame(*frame);
			frame.release();

			Refresh();
		}
		else
		{
			sleep(1);
		}
	}
}

}

}

}
