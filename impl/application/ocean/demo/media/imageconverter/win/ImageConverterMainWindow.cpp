/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imageconverter/win/ImageConverterMainWindow.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/win/Utilities.h"

ImageConverterMainWindow::ImageConverterMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& fileInput) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFileInput_(fileInput),
	mediaFileOutputType_("png")
{
	// nothing to do here
}

ImageConverterMainWindow::~ImageConverterMainWindow()
{
	// nothing to do here
}

void ImageConverterMainWindow::onInitialized()
{
	if (!mediaFileInput_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFileInput_, Media::Medium::IMAGE_SEQUENCE);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFileInput_, Media::Medium::IMAGE);
	}

	if (frameMedium_)
	{
		frameMedium_->setPreferredFrameFrequency(30);

		const Media::ImageSequenceRef imageSequence(frameMedium_);
		if (imageSequence)
		{
			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}

		const Media::FiniteMediumRef finiteMedium(frameMedium_);
		if (finiteMedium)
		{
			finiteMedium->setLoop(false);
		}

		frameMedium_->start();
	}
}

void ImageConverterMainWindow::onIdle()
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

void ImageConverterMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	IO::File file(mediaFileInput_);

	const Media::ImageSequenceRef imageSequence(frameMedium_);
	if (imageSequence)
	{
		file = IO::File(imageSequence->currentUrl());
	}

	const std::string outputFilename(file.base() + std::string(".") + mediaFileOutputType_);

	Media::Utilities::saveImage(frame, outputFilename, false);

	if (imageSequence)
	{
		imageSequence->forceNextFrame();
	}

	repaint();
}
