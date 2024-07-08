/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imagesequencerecorder/win/ImageSequenceRecorderMainWindow.h"

#include "ocean/base/String.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/File.h"

#include "ocean/media/ConfigMedium.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

ImageSequenceRecorderMainWindow::ImageSequenceRecorderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

ImageSequenceRecorderMainWindow::~ImageSequenceRecorderMainWindow()
{
	// nothing to do here
}

bool ImageSequenceRecorderMainWindow::setFilename(const std::string& filename)
{
	if (imageSequenceRecorder_ && imageSequenceRecorder_->setFilename(filename))
	{
		imageSequenceRecorder_->start();
		return true;
	}

	return false;
}

void ImageSequenceRecorderMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);

	if (frameMedium_)
		frameMedium_->start();

	imageSequenceRecorder_ = Media::Manager::get().newRecorder(Media::Recorder::IMAGE_SEQUENCE_RECORDER);
	if (imageSequenceRecorder_)
	{
		imageSequenceRecorder_->setFrameFrequency(30.0);
		imageSequenceRecorder_->setFilenameSuffixed(false);
		imageSequenceRecorder_->setMode(Media::ImageSequenceRecorder::RM_PARALLEL);
	}
}

void ImageSequenceRecorderMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ImageSequenceRecorderMainWindow::onKeyUp(const int key)
{
	switch (key)
	{
		case '1':
		{
			const Media::ConfigMediumRef configMedium(frameMedium_);

			if (configMedium)
			{
				const Media::ConfigMedium::ConfigNames configs(configMedium->configs());

				if (configs.size() > 0)
					configMedium->configuration(configs[0], (long long)(handle()));
			}
			break;
		}

		case '2':
		{
			const Media::ConfigMediumRef configMedium(frameMedium_);

			if (configMedium)
			{
				const Media::ConfigMedium::ConfigNames configs(configMedium->configs());

				if (configs.size() > 1)
					configMedium->configuration(configs[1], (long long)(handle()));
			}
			break;
		}

		case 'A':
		{
			const ScopedLock scopedLock(lock_);

			if (imageSequenceRecorder_)
				imageSequenceRecorder_->start();

			break;
		}

		case 'B':
		{
			const ScopedLock scopedLock(lock_);

			if (imageSequenceRecorder_)
			{
				imageSequenceRecorder_->stop();

				if (imageSequenceRecorder_->mode() == Media::ImageSequenceRecorder::RM_EXPLICIT)
					imageSequenceRecorder_->forceSaving();
			}

			break;
		}

		case 'I':
		{
			const ScopedLock scopedLock(lock_);

			if (imageSequenceRecorder_)
				imageSequenceRecorder_->setMode(Media::ImageSequenceRecorder::RM_IMMEDIATE);

			break;
		}

		case 'E':
		{
			const ScopedLock scopedLock(lock_);

			if (imageSequenceRecorder_)
				imageSequenceRecorder_->setMode(Media::ImageSequenceRecorder::RM_EXPLICIT);

			break;
		}

		case 'P':
		{
			const ScopedLock scopedLock(lock_);

			if (imageSequenceRecorder_)
				imageSequenceRecorder_->setMode(Media::ImageSequenceRecorder::RM_PARALLEL);

			break;
		}
	}
}

void ImageSequenceRecorderMainWindow::onFrame(const Frame& frame)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, false))
	{
		return;
	}

	if (imageSequenceRecorder_)
	{
		imageSequenceRecorder_->addImage(rgbFrame);
	}

	setFrame(rgbFrame);

	if (imageSequenceRecorder_)
	{
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(imageSequenceRecorder_->pendingImages()) + std::string(" images"));
	}

	repaint();
}
