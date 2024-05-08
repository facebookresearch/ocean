/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/fastcornerpyramid/win/FastCornerPyramidMainWindow.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/FastFeatureDetector.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

FASTCornerPyramidMainWindow::FASTCornerPyramidMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

FASTCornerPyramidMainWindow::~FASTCornerPyramidMainWindow()
{
	// nothing to do here
}

void FASTCornerPyramidMainWindow::onInitialized()
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

void FASTCornerPyramidMainWindow::onIdle()
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

void FASTCornerPyramidMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "up")
		{
			if (fastCornerThreshold_ < 200u)
			{
				fastCornerThreshold_ += 5u;
			}
		}
		else if (keyString == "down")
		{
			if (fastCornerThreshold_ > 5u)
			{
				fastCornerThreshold_ -= 5u;
			}
		}
		else if (keyString == "right")
		{
			if (fastCornerNumberVisible_ < 2000u)
			{
				fastCornerNumberVisible_ += 5u;
			}
		}
		else if (keyString == "left")
		{
			if (fastCornerNumberVisible_ > 5u)
			{
				fastCornerNumberVisible_ -= 5u;
			}
		}
		else if (keyString == "page up")
		{
			if (fastCornerPyramidLayers_ < 20)
			{
				++fastCornerPyramidLayers_;
			}
		}
		else if (keyString == "page down")
		{
			if (fastCornerPyramidLayers_ > 1u)
			{
				--fastCornerPyramidLayers_;
			}
		}
		else if (keyString == "F")
		{
			fastCornerFrameSmoothing_ = !fastCornerFrameSmoothing_;
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

void FASTCornerPyramidMainWindow::onFrame(const Frame& frame)
{
	ocean_assert(frame.isValid());

	Frame pyramidFrame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	const unsigned int width = frame.width() + frame.width() / 2u;

	bitmap_.set(width, frame.height(), frame.pixelFormat(), FrameType::ORIGIN_UPPER_LEFT);

	int xPos = 0;
	int yPos = 0;

	for (unsigned int nLayer = 0u; nLayer < fastCornerPyramidLayers_; ++nLayer)
	{
		const Platform::Win::Bitmap featureBitmap(detectFeatures(pyramidFrame));
		BitBlt(bitmap().dc(), xPos, yPos, featureBitmap.width(), featureBitmap.height(), featureBitmap.dc(), 0, 0, SRCCOPY);

		if (nLayer == 0u)
		{
			xPos = int(pyramidFrame.width());
		}
		else
		{
			yPos += int(featureBitmap.height());
		}

		CV::FrameShrinker::downsampleByTwo11(pyramidFrame, &worker_);
	}

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Threshold: ") + String::toAString(fastCornerThreshold_));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, std::string("Visible: ") + String::toAString(fastCornerNumberVisible_));

	if (fastCornerPreviousPyramidLayers_ != fastCornerPyramidLayers_)
	{
		adjustToBitmapSize();
		fastCornerPreviousPyramidLayers_ = fastCornerPyramidLayers_;
	}

	repaint();
}

Platform::Win::Bitmap FASTCornerPyramidMainWindow::detectFeatures(const Frame& frame)
{
	Frame tmpFrame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	if (fastCornerFrameSmoothing_)
	{
		CV::FrameFilterGaussian::filter(tmpFrame, 3u, &worker_);
	}

	Platform::Win::Bitmap bitmap(tmpFrame);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(tmpFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return bitmap;
	}

	CV::Detector::FASTFeatures features;
	CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(yFrame, fastCornerThreshold_, false, true, features, &worker_);

	std::sort(features.begin(), features.end());

	for (unsigned int n = 0; n < features.size() && n < fastCornerNumberVisible_; ++n)
	{
		const int x = int(features[n].observation().x() + 0.5);
		const int y = int(features[n].observation().y() + 0.5);

		Rectangle(bitmap.dc(), x - 2, y - 2, x + 2, y + 2);
	}

	return bitmap;
}
