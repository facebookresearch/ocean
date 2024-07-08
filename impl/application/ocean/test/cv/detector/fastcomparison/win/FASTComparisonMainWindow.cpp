/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/detector/fastcomparison/win/FASTComparisonMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

#include <fast/fast.h>

MainWindow::MainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	originalWindow_(instance, L"Original implementation"),
	mediaFile_(file)
{
	// nothing to do here
}

MainWindow::~MainWindow()
{
	// nothing to do here
}

void MainWindow::onInitialized()
{
	if (!mediaFile_.empty())
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_);

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");

	if (frameMedium_.isNull())
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");

	if (frameMedium_)
	{
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
		frameMedium_->start();
	}

	originalWindow_.setParent(handle());
	originalWindow_.initialize();
	originalWindow_.show();
}

void MainWindow::onIdle()
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

void MainWindow::onKeyDown(const int /*key*/)
{
	std::string keyString;

	ignoreTimestamp_ = !ignoreTimestamp_;
}

void MainWindow::onFrame(const Frame& frame)
{
	// we need a BGR24 bit frame to display it
	Frame bgrFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_BGR24, frame.pixelOrigin(), bgrFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return;
	}

	// display the frame
	setFrame(bgrFrame);
	originalWindow_.setFrame(bgrFrame);

	// we need a Y8 bit frame for feature determination
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "Unsupported pixel format!");
		return;
	}

	const unsigned int fastThreshold = 20u;

	// Performance measurement for the Ocean implementation of FAST
	static HighPerformanceStatistic oceanPerformance;
	if (oceanPerformance.measurements() > 200u)
		oceanPerformance.reset();

	oceanPerformance.start();

		CV::Detector::FASTFeatures features;
		CV::Detector::FASTFeatureDetector::Comfort::detectFeatures(yFrame, fastThreshold, false, true, features, &worker_);

	oceanPerformance.stop();

	// Display all FAST features
	for (unsigned int n = 0; n < features.size(); ++n)
		Rectangle(bitmap().dc(), int(features[n].observation().x()) - 2, int(features[n].observation().y()) - 2, int(features[n].observation().x()) + 2, int(features[n].observation().y()) + 2);

	// Display performance information
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(oceanPerformance.averageMseconds()) + std::string("ms"));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, String::toAString(features.size()));


	// Performance measurement for the original implementation of FAST
	static HighPerformanceStatistic originalPerformance;
	if (originalPerformance.measurements() > 200u)
		originalPerformance.reset();

	originalPerformance.start();

		int numberCorners = 0;
		xy* corners = fast12_detect(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.strideBytes(), fastThreshold, &numberCorners);
		int* scores = fast12_score(yFrame.constdata<uint8_t>(), yFrame.strideBytes(), corners, numberCorners, fastThreshold);

		int nonMaxNumberCorners = 0;
		xy* nonMaxCorners = nonmax_suppression(corners, scores, numberCorners, &nonMaxNumberCorners);

	originalPerformance.stop();

	// Display all FAST features
	for (int n = 0; n < nonMaxNumberCorners; ++n)
	{
		Rectangle(originalWindow_.bitmap().dc(), nonMaxCorners[n].x - 2, nonMaxCorners[n].y - 2, nonMaxCorners[n].x + 2, nonMaxCorners[n].y + 2);
	}

	// Display performance information
	Platform::Win::Utilities::textOutput(originalWindow_.bitmap().dc(), 5, 5, String::toAString(originalPerformance.averageMseconds()) + std::string("ms"));
	Platform::Win::Utilities::textOutput(originalWindow_.bitmap().dc(), 5, 25, String::toAString(nonMaxNumberCorners));

	// free resources of the original FAST implementation
	free(scores);
	free(corners);
	free(nonMaxCorners);

	// invoke window repaint
	repaint();
	originalWindow_.repaint();
}
