/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreviewBitmapWindow.h"

using namespace Ocean;

MoviePreviewBitmapWindow::MoviePreviewBitmapWindow(const wxString& title, wxWindow* parent, const wxPoint& pos, const wxSize& size) :
	Platform::WxWidgets::BitmapWindow(title, parent, pos, size)
{
	Scheduler::get().registerFunction(Scheduler::Callback(*this, &MoviePreviewBitmapWindow::onScheduler));
}

MoviePreviewBitmapWindow::~MoviePreviewBitmapWindow()
{
	Scheduler::get().unregisterFunction(Scheduler::Callback(*this, &MoviePreviewBitmapWindow::onScheduler));

	if (frameProvider_)
	{
		frameProvider_->removeFrameCallback(Media::MovieFrameProvider::FrameCallback(*this, &MoviePreviewBitmapWindow::onFrame));
	}

	frameProvider_.release();
}

void MoviePreviewBitmapWindow::setMovieFrameProvider(const Ocean::Media::MovieFrameProviderRef& frameProvider)
{
	if (frameProvider_)
	{
		frameProvider_->removeFrameCallback(Media::MovieFrameProvider::FrameCallback(*this, &MoviePreviewBitmapWindow::onFrame));
	}

	frameProvider_ = frameProvider;

	if (frameProvider_)
	{
		frameProvider_->addFrameCallback(Media::MovieFrameProvider::FrameCallback(*this, &MoviePreviewBitmapWindow::onFrame));
	}
}

void MoviePreviewBitmapWindow::setSelectionIndex(const unsigned int index)
{
	selectionIndex_ = index;
}

void MoviePreviewBitmapWindow::onFrame(const unsigned int index, const bool /*isRequested*/)
{
	ocean_assert(index != (unsigned int)(-1));

	if (index == selectionIndex_)
	{
		newFrameIndex_ = index;
	}
}

void MoviePreviewBitmapWindow::onScheduler()
{
	if (selectionIndex_ != frameIndex_ && newFrameIndex_ == selectionIndex_)
	{
		const FrameRef frame(frameProvider_->frameRequest(newFrameIndex_));

		if (frame)
		{
			frame_.copy(*frame);

			setFrame(*frame);

			frameIndex_ = newFrameIndex_;
		}
	}
}
