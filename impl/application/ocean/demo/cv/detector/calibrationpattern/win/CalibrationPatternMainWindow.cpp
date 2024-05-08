/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/calibrationpattern/win/CalibrationPatternMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

CalibrationPatternMainWindow::CalibrationPatternMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file),
	patternDetector_(5u, 7u)
{
	// nothing to do here
}

CalibrationPatternMainWindow::~CalibrationPatternMainWindow()
{
	// nothing to do here
}

void CalibrationPatternMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
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

void CalibrationPatternMainWindow::onIdle()
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

void CalibrationPatternMainWindow::onKeyDown(const int /*key*/)
{
	ignoreTimestamp_ = !ignoreTimestamp_;
}

void CalibrationPatternMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void CalibrationPatternMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	if (patternDetector_.detectPattern(topLeft, WorkerPool::get().scopedWorker()()))
	{
		const CV::Detector::CalibrationPatternDetector::Pattern& pattern = patternDetector_.pattern();

		CV::Canvas::line<1u>(topLeft, pattern.corners()[0], pattern.corners()[1], CV::Canvas::red());
		CV::Canvas::line<1u>(topLeft, pattern.corners()[1], pattern.corners()[2], CV::Canvas::red());
		CV::Canvas::line<1u>(topLeft, pattern.corners()[2], pattern.corners()[3], CV::Canvas::red());
		CV::Canvas::line<1u>(topLeft, pattern.corners()[3], pattern.corners()[0], CV::Canvas::red());

		setFrame(topLeft);

		Vectors2 corners;
		unsigned int index = 0u;

		for (const Vectors2& points : pattern.rows())
		{
			for (const Vector2& point : points)
			{
				Platform::Win::Utilities::textOutput(bitmap().dc(), int(point.x()), int(point.y()), String::toAString(index));
				++index;

				corners.emplace_back(point);
			}
		}
	}
	else
	{
		setFrame(topLeft);
	}

	repaint(false);
}
