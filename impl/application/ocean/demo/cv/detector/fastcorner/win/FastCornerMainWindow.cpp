/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/fastcorner/win/FastCornerMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/FastFeatureDetector.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

FASTCornerMainWindow::FASTCornerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

FASTCornerMainWindow::~FASTCornerMainWindow()
{
	// nothing to do here
}

void FASTCornerMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_);

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
		finiteMedium->setLoop(true);

	if (frameMedium_)
		frameMedium_->start();
}

void FASTCornerMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_ || ignoreTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void FASTCornerMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "up")
		{
			if (fastCornerThreshold_ < 200)
				fastCornerThreshold_ += 10;
		}
		else if (keyString == "down")
		{
			if (fastCornerThreshold_ > 10)
				fastCornerThreshold_ -= 10;
		}
		else if (keyString == "page up")
		{
			if (fastNumberVisible_ < 5000)
				fastNumberVisible_ += 10;
		}
		else if (keyString == "page down")
		{
			if (fastNumberVisible_ >= 20)
				fastNumberVisible_ -= 10;
		}
		else if (keyString == "F")
			fastCornerFrameSmoothing_ = !fastCornerFrameSmoothing_;
		else
			ignoreTimestamp_ = !ignoreTimestamp_;
	}
	else
		ignoreTimestamp_ = !ignoreTimestamp_;
}

void FASTCornerMainWindow::onMouseUp(const MouseButton /*button*/, const int x, const int y)
{
	if (fastSubFrameSelectionIndex_ == 0)
	{
		fastSubFrameLeft_ = x;
		fastSubFrameTop_ = y;
	}
	else if (fastSubFrameSelectionIndex_ == 1)
	{
		fastSubFrameRight_ = x;
		fastSubFrameBottom_ = y;
	}

	if (++fastSubFrameSelectionIndex_ >= 3)
	{
		fastSubFrameSelectionIndex_ = 0;
	}
}

void FASTCornerMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	HighPerformanceTimer timer;

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, &worker_))
	{
		ocean_assert(false && "Should never happen!");
		return;
	}

	if (fastCornerFrameSmoothing_)
	{
		CV::FrameFilterGaussian::filter(yFrame, 3u, &worker_);
	}

	CV::Detector::FASTFeatures features;

	if (fastSubFrameSelectionIndex_ == 2u)
	{
		const unsigned int left = min(fastSubFrameLeft_, fastSubFrameRight_);
		const unsigned int top = min(fastSubFrameTop_, fastSubFrameBottom_);

		const unsigned int right = max(fastSubFrameLeft_, fastSubFrameRight_);
		const unsigned int bottom = max(fastSubFrameTop_, fastSubFrameBottom_);

		CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(yFrame, left, top, right - left, bottom - top, fastCornerThreshold_, false, true, features, &worker_);
	}
	else
	{
		CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(yFrame, fastCornerThreshold_, false, true, features, &worker_);
	}

	const double time = timer.mseconds();

	std::sort(features.begin(), features.end());

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Performance: ") + String::toAString(time) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, std::string("Threshold: ") + String::toAString(fastCornerThreshold_));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 50, std::string("Features: ") + String::toAString(features.size()));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 70, std::string("Max visible: ") + String::toAString(fastNumberVisible_));

	for (unsigned int n = 0; n < features.size() && n < fastNumberVisible_; ++n)
	{
		const int x = int(features[n].observation().x() + 0.5);
		const int y = int(features[n].observation().y() + 0.5);

		Rectangle(bitmap().dc(), x - 2, y - 2, x + 2, y + 2);
	}

	repaint(false);
}
