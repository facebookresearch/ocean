// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/harriscorner/win/HarrisCornerMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/media/Manager.h"
#include "ocean/media/FiniteMedium.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

HarrisCornerMainWindow::HarrisCornerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

HarrisCornerMainWindow::~HarrisCornerMainWindow()
{
	// nothing to do here
}

void HarrisCornerMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
	{
		finiteMedium->setLoop(true);
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}
}

void HarrisCornerMainWindow::onIdle()
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

void HarrisCornerMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "up")
		{
			if (harrisCornerThreshold_ < 200)
			{
				harrisCornerThreshold_ += 5;
			}
		}
		else if (keyString == "down")
		{
			if (harrisCornerThreshold_ > 0)
			{
				harrisCornerThreshold_ -= 5;
			}
		}
		else if (keyString == "E")
		{
			harrisExactPosition_ = !harrisExactPosition_;
		}
		else if (keyString == "S")
		{
			harrisSmoothing_ = !harrisSmoothing_;
		}
		else
		{
			ignoreTimestamp_ = !ignoreTimestamp_;
		}
	}
	else
	{
		ignoreTimestamp_ = !ignoreTimestamp_;
	}
}

void HarrisCornerMainWindow::onMouseUp(const MouseButton /*button*/, const int x, const int y)
{
	if (harrisSubFrameSelectionIndex_ == 0)
	{
		harrisSubFrameLeft_ = x;
		harrisSubFrameTop_ = y;
	}
	else if (harrisSubFrameSelectionIndex_ == 1)
	{
		harrisSubFrameRight_ = x;
		harrisSubFrameBottom_ = y;
	}

	if (++harrisSubFrameSelectionIndex_ >= 3)
	{
		harrisSubFrameSelectionIndex_ = 0;
	}
}

void HarrisCornerMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), topLeft, false, &worker_))
	{
		return;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(topLeft, FrameType(topLeft, FrameType::FORMAT_Y8), yFrame, true, &worker_))
	{
		return;
	}

	if (harrisSmoothing_)
	{
		CV::FrameFilterGaussian::filter(yFrame, 3u, &worker_);
	}

	HighPerformanceTimer timer;
	CV::Detector::HarrisCorners corners;

	if (harrisSubFrameSelectionIndex_ == 2)
	{
		const unsigned int left = min(harrisSubFrameLeft_, harrisSubFrameRight_);
		const unsigned int top = min(harrisSubFrameTop_, harrisSubFrameBottom_);

		const unsigned int right = max(harrisSubFrameLeft_, harrisSubFrameRight_);
		const unsigned int bottom = max(harrisSubFrameTop_, harrisSubFrameBottom_);

		CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), left, top, right - left, bottom - top, (unsigned int)(harrisCornerThreshold_), false, corners, harrisExactPosition_, &worker_);
	}
	else
	{
		CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), (unsigned int)(harrisCornerThreshold_), false, corners, harrisExactPosition_, &worker_);
	}

	const double time = timer.mseconds();

	std::sort(corners.begin(), corners.end());

#ifdef OCEAN_DEBUG
	if (!harrisExactPosition_)
	{
		for (unsigned int n = 0; n < corners.size(); ++n)
		{
			const Vector2& observation = corners[n].observation();

			const unsigned int response = CV::Detector::HarrisCornerDetector::harrisVotePixel(yFrame.constdata<uint8_t>(), yFrame.width(), Numeric::round32(observation.x()), Numeric::round32(observation.y()), yFrame.paddingElements());
			ocean_assert(response == corners[n].strength());
		}
	}
#endif // OCEAN_DEBUG

	paintCorners(corners, topLeft, &worker_);

	setFrame(topLeft);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Performance: ") + String::toAString(time) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, std::string("Threshold: ") + String::toAString(harrisCornerThreshold_));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 45, std::string("Features: ") + String::toAString(corners.size()));

	repaint(false);
}

void HarrisCornerMainWindow::paintCorners(const CV::Detector::HarrisCorners& corners, Frame& frame, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&HarrisCornerMainWindow::paintCornersSubset, corners.data(), &frame, 0u, 0u), 0u, (unsigned int)(corners.size()), 2u, 3u, 50u);
	}
	else
	{
		paintCornersSubset(corners.data(), &frame, 0u, (unsigned int)(corners.size()));
	}
}

void HarrisCornerMainWindow::paintCornersSubset(const CV::Detector::HarrisCorner* corners, Frame* frame, const unsigned int firstCorner, const unsigned int numberCorners)
{
	ocean_assert(corners != nullptr && frame != nullptr);

	for (unsigned int n = firstCorner; n < firstCorner + numberCorners; ++n)
	{
		const Vector2& observation = corners[n].observation();

		CV::Canvas::point<7u>(*frame, observation, CV::Canvas::red(frame->pixelFormat()));
		CV::Canvas::point<3u>(*frame, observation, CV::Canvas::blue(frame->pixelFormat()));
	}
}
