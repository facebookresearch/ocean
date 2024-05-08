/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imagerecorder/win/ImageRecorderMainWindow.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/media/ImageRecorder.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

ImageRecorderMainWindow::ImageRecorderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

ImageRecorderMainWindow::~ImageRecorderMainWindow()
{
	// nothing to do here
}

void ImageRecorderMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}
}

void ImageRecorderMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_) || recordFrame_)
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ImageRecorderMainWindow::onKeyDown(const int /*key*/)
{
	recordFrame_ = true;
}

void ImageRecorderMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	recordFrame_ = true;
}

void ImageRecorderMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	if (recordFrame_)
	{
		recordFrame_ = false;

		Media::ImageRecorderRef imageRecorder = Media::Manager::get().newRecorder(Media::Recorder::IMAGE_RECORDER);
		if (imageRecorder.isNull())
		{
			Log::error() << "Could not create an image recorder!";
			return;
		}

		imageRecorder->saveImage(frame, "image.bmp");
	}

	repaint();
}
