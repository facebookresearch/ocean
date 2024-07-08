/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/movierecorder/win/MovieRecorderMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/media/ConfigMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/ImageSequence.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

MovieRecorderMainWindow::MovieRecorderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& fileInput, const std::string& fileOutput) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFileInput_(fileInput),
	mediaFileOutput_(fileOutput)
{
	// nothing to do here
}

MovieRecorderMainWindow::~MovieRecorderMainWindow()
{
	// nothing to do here
}

void MovieRecorderMainWindow::onInitialized()
{
	if (!mediaFileInput_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFileInput_, Media::Medium::IMAGE_SEQUENCE);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_)
	{
		const Media::ConfigMediumRef configMedium(frameMedium_);

		if (configMedium)
		{
			const Media::ConfigMedium::ConfigNames configs(configMedium->configs());

			for (unsigned int n = 0; n < configs.size(); ++n)
			{
				configMedium->configuration(configs[n], (long long)handle());
			}
		}

		frameMedium_->setPreferredFrameFrequency(30);

		const Media::ImageSequenceRef imageSequence(frameMedium_);
		if (imageSequence)
		{
			imageSequence->setLoop(false);
			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}

		const Timestamp startTimestamp(true);
		while (startTimestamp + 1 > Timestamp(true) && frameMedium_->frame().isNull())
		{
			Thread::sleep(1);
		}

		// request the first frame to receive the frame dimension

		frameMedium_->start();
		while ((!frameMedium_->frame() || frameMedium_->frame().isNull()) && startTimestamp + 5 > Timestamp(true))
		{
			Thread::sleep(1);
		}

		const FrameRef frame(frameMedium_->frame());
		if (frame)
		{
			frameTargetType_ = FrameType(frame->frameType(), FrameType::FORMAT_BGR24);
		}

		frameMedium_->stop();

		if (mediaFileOutput_.empty())
		{
			mediaFileOutput_ = "output.avi";
		}

		movieRecorder_ = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

		if (movieRecorder_)
		{
			Media::MovieRecorder::Encoders encoders(movieRecorder_->frameEncoders());

			// workaround to select the installed divx encoder
			bool found = false;
			for (unsigned int n = 0; n < encoders.size(); ++n)
			{
				if (String::toLower(encoders[n]).find("divx") != std::string::npos)
				{
					movieRecorder_->setFrameEncoder(encoders[n]);
					found = true;
					break;
				}
			}

			if (!found && !encoders.empty())
			{
				movieRecorder_->setFrameEncoder(encoders.front());
			}

			movieRecorder_->setFilename(mediaFileOutput_);

			if (frameTargetType_.isValid())
			{
				movieRecorder_->setPreferredFrameType(frameTargetType_);
			}

			if (frameMedium_->frameFrequency() > 1)
			{
				movieRecorder_->setFrameFrequency(frameMedium_->frameFrequency());
			}
			else
			{
				movieRecorder_->setFrameFrequency(30);
			}

			if (movieRecorder_->frameEncoderHasConfiguration())
			{
				movieRecorder_->frameEncoderConfiguration((long long)handle());
			}
		}

		Log::info() << "Selected the following frame encoder:";
		Log::info() << movieRecorder_->frameEncoder();
		Log::info() << " ";
		Log::info() << "Width frame type:";
		Log::info() << movieRecorder_->frameType().width() << "x" << movieRecorder_->frameType().height();
		Log::info() << "and frame frequency:";
		Log::info() << movieRecorder_->frameFrequency() << "Hz";

		if (imageSequence)
		{
			movieRecorder_->start();
		}

		frameMedium_->start();
	}
}

void MovieRecorderMainWindow::onIdle()
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

		const Media::ImageSequenceRef imageSequence(frameMedium_);
		if (imageSequence)
		{
			imageSequence->forceNextFrame();
		}
	}

	Sleep(1);
}

void MovieRecorderMainWindow::onKeyDown(const int /*key*/)
{
	const Media::ImageSequenceRef imageSequence(frameMedium_);
	if (imageSequence)
	{
		return;
	}

	toggleRecording();
}

void MovieRecorderMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	toggleRecording();
}

void MovieRecorderMainWindow::toggleRecording()
{
	if (movieRecorder_)
	{
		if (!movieRecorder_->isRecording())
		{
			if (movieRecorder_->start())
			{
				Log::info() << "Movie recorder started.";
			}
			else
			{
				Log::error() << "FAILED to start the movie recorder!";
			}
		}
		else
		{
			if (movieRecorder_->stop())
			{
				Log::info() << "Movie recorder stopped.";
			}
			else
			{
				Log::error() << "FAILED to stop the movie recorder!";
			}
		}
	}
}

void MovieRecorderMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	if (movieRecorder_)
	{
		const Timestamp startTimestamp(true);

		const Media::ImageSequenceRef imageSequence(frameMedium_);

		if (imageSequence)
		{
			// each frame of the image sequence has to be encoded

			do
			{
				Frame recorderFrame;
				if (movieRecorder_->lockBufferToFill(recorderFrame, false))
				{
					ocean_assert(recorderFrame.isValid() && frameTargetType_ == recorderFrame.frameType());

					if (!CV::FrameConverter::Comfort::convertAndCopy(frame, recorderFrame, WorkerPool::get().scopedWorker()()))
					{
						Log::error() << "Failed to convert image.";
					}

					movieRecorder_->unlockBufferToFill();
					break;
				}

				Sleep(1);
			}
			while (startTimestamp + 10 > Timestamp(true));

			ocean_assert(false && "Missed to encode an image.");
		}
		else
		{
			Frame recorderFrame;
			if (movieRecorder_->lockBufferToFill(recorderFrame, true))
			{
				ocean_assert(recorderFrame.isValid() && frameTargetType_ == recorderFrame.frameType());

				if (!CV::FrameConverter::Comfort::convertAndCopy(frame, recorderFrame, WorkerPool::get().scopedWorker()()))
				{
					Log::error() << "Failed to convert image.";
				}

				movieRecorder_->unlockBufferToFill();
			}
		}
	}

	repaint(false);
}
