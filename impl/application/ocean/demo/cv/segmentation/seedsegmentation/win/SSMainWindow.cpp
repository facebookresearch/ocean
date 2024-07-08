/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/seedsegmentation/win/SSMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMedian.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/segmentation/SeedSegmentation.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

SSMainWindow::SSMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	segmentationWindow_(instance, L"Segmentation Result"),
	mediaFile_(file)
{
	// nothing to do here
}

SSMainWindow::~SSMainWindow()
{
	// nothing to do here
}

void SSMainWindow::onInitialized()
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

	if (frameMedium_)
	{
		frameMedium_->start();
	}

	segmentationWindow_.setParent(handle());
	segmentationWindow_.initialize();
	segmentationWindow_.show();
}

void SSMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && (*frame && frame->timestamp() != frameTimestamp_ || invokeSeedSegmentation_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void SSMainWindow::onMouseUp(const MouseButton /*button*/, const int x, const int y)
{
	int bitmapX, bitmapY;

	if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
	{
		seedPositionX_ = (unsigned int)bitmapX;
		seedPositionY_ = (unsigned int)bitmapY;

		invokeSeedSegmentation_ = true;
	}
}

void SSMainWindow::onKeyUp(const int key)
{
	std::string keyValue;
	Platform::Win::Keyboard::translateVirtualkey(key, keyValue);

	if (keyValue == "up")
	{
		++areaSizeHalf_;
	}
	else if (keyValue == "down")
	{
		if (areaSizeHalf_ > 0)
		{
			--areaSizeHalf_;
		}
	}
}

void SSMainWindow::onFrame(const Frame& frame)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	setFrame(rgbFrame);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Area: ") + String::toAString(areaSizeHalf_));

	if (invokeSeedSegmentation_)
	{
		invokeSeedSegmentation_ = false;

		Frame mask(FrameType(rgbFrame, FrameType::FORMAT_Y8));
		CV::PixelPositions centerPixels, borderPixels, outlinePixels;

		CV::PixelBoundingBox boundingBox;

		const HighPerformanceTimer timer;
		unsigned int pixels = 0u;

		if (areaSizeHalf_ == 0u)
		{
			pixels = CV::Segmentation::SeedSegmentation::Comfort::iterativeSeedSegmentation(rgbFrame, mask, CV::PixelPosition(seedPositionX_, seedPositionY_), 15u, 35u, 150u, 5u, &boundingBox);
		}
		else
		{
			rgbFrame.makeContinuous();

			const Frame integralFrame = CV::IntegralImage::Comfort::createBorderedImage(rgbFrame, areaSizeHalf_);
			ocean_assert(integralFrame.isContinuous());

			pixels = CV::Segmentation::SeedSegmentation::seedSegmentationArea8BitPerChannel<3u>(integralFrame.constdata<uint32_t>(), rgbFrame.width(), rgbFrame.height(), areaSizeHalf_, 2u * areaSizeHalf_ + 1u, mask.paddingElements(), CV::PixelPosition(seedPositionX_, seedPositionY_), 25u, 155u, mask.data<uint8_t>(), &boundingBox);
		}

		const double time = timer.mseconds();

		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			const uint8_t* maskRow = mask.constrow<uint8_t>(y);
			uint8_t* rgbRow = rgbFrame.row<uint8_t>(y);

			for (unsigned int x = 0u; x < mask.width(); ++x)
			{
				if (maskRow[x] == 0x00)
				{
					rgbRow[x * 3u + 0u] = 0xFF - rgbRow[x * 3u + 0u];
					rgbRow[x * 3u + 1u] = 0xFF;
					rgbRow[x * 3u + 2u] = 0xFF - rgbRow[x * 3u + 2u];
				}
			}
		}


		segmentationWindow_.setFrame(rgbFrame);

		MoveToEx(segmentationWindow_.bitmap().dc(), int(boundingBox.left()) - 2, int(boundingBox.top()) - 2, nullptr);
		LineTo(segmentationWindow_.bitmap().dc(), int(boundingBox.right()) + 2, int(boundingBox.top()) - 2);
		LineTo(segmentationWindow_.bitmap().dc(), int(boundingBox.right()) + 2, int(boundingBox.bottom()) + 2);
		LineTo(segmentationWindow_.bitmap().dc(), int(boundingBox.left()) - 2, int(boundingBox.bottom()) + 2);
		LineTo(segmentationWindow_.bitmap().dc(), int(boundingBox.left()) - 2, int(boundingBox.top()) - 2);

		Platform::Win::Utilities::textOutput(segmentationWindow_.bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));
		Platform::Win::Utilities::textOutput(segmentationWindow_.bitmap().dc(), 5, 25, String::toAString(pixels));

		segmentationWindow_.repaint();
	}

	if (segmentationWindow_.bitmap().width() == 0)
	{
		segmentationWindow_.setFrame(rgbFrame);
		segmentationWindow_.repaint();
	}

	repaint();
}
