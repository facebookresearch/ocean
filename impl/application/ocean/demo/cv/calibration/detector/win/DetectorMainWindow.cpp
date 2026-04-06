/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/detector/win/DetectorMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/cv/calibration/CalibrationBoardDetector.h"
#include "ocean/cv/calibration/CalibrationDebugElements.h"
#include "ocean/cv/calibration/PointDetector.h"
#include "ocean/cv/calibration/Utilities.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/LiveVideo.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"

using namespace Ocean::CV::Calibration;

DetectorMainWindow::DetectorMainWindow(HINSTANCE instance, const std::wstring& name, const MetricCalibrationBoard& calibrationBoard, const std::string& file, const unsigned int gaussianFilterSize) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file),
	calibrationBoard_(calibrationBoard),
	gaussianFilterSize_(gaussianFilterSize)
{
#ifdef ENABLE_DEBUG_ELEMENTS
	static_assert(CV::Calibration::CalibrationDebugElements::allowDebugging_);
	CV::Calibration::CalibrationDebugElements::get().setElementUpdateCallback(CV::Calibration::CalibrationDebugElements::ElementUpdateCallback::create(*this, &DetectorMainWindow::onDebugElement));
	CV::Calibration::CalibrationDebugElements::get().activateAllElements();
#endif
}

DetectorMainWindow::~DetectorMainWindow()
{
	// nothing to do here
}

void DetectorMainWindow::onInitialized()
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

	if (Media::LiveVideoRef liveVideo = frameMedium_)
	{
		// to avoid compression artifacts, we try to use a video stream without video encoding

		if (!liveVideo->setPreferredFramePixelFormat(FrameType::FORMAT_YUYV16))
		{
			Log::warning() << "Failed to set preferred pixel format of camera to YUYV16";
		}

		if (!liveVideo->setFocus(0.65f))
		{
			Log::warning() << "Failed to fix the camera's focus";
		}
	}

	if (frameMedium_)
	{
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
		frameMedium_->start();
	}
}

void DetectorMainWindow::onIdle()
{
	if (frameMedium_)
	{
		SharedAnyCamera camera;
		const FrameRef frame(frameMedium_->frame(&camera));

		if (frame && *frame && (frame->timestamp() != frameTimestamp_))
		{
			ocean_assert(camera && "The camera profile needs to be known, set it manually if needed");

			if (camera)
			{
				onFrame(*frame, camera);
			}

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void DetectorMainWindow::onKeyDown(const int key)
{
	std::string keyValue;
	if (!Platform::Win::Keyboard::translateVirtualkey(key, keyValue))
	{
		return;
	}

	if (keyValue == " ")
	{
		applicationMode_ = ApplicationMode((applicationMode_ + 1u) % AM_END);
	}
	else if (keyValue == "S")
	{
		saveImage_ = true;
	}
}

void DetectorMainWindow::onFrame(const Frame& frame, const SharedAnyCamera& camera)
{
	Frame frameToProcess(frame, Frame::ACM_USE_KEEP_LAYOUT);

	if (gaussianFilterSize_ != 0u)
	{
		frameToProcess.makeOwner();

		if (!CV::FrameFilterGaussian::filter(frameToProcess, gaussianFilterSize_, WorkerPool::get().scopedWorker()()))
		{
			Log::warning() << "Failed to apply Gaussian filter.";
		}
	}

	if (saveImage_)
	{
		saveImage_= false;

		static unsigned int counter = 0u;

		IO::Image::writeImage(frameToProcess, "image" + String::toAString(counter++, 5u) + ".png");
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frameToProcess, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame outputFrame;

	if (applicationMode_ == AM_POINT_DETECTION)
	{
		if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, outputFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		CV::Canvas::drawText(outputFrame, "Point detection:", 5, 5, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));

		HighPerformanceTimer timer;

		PointDetector pointDetector;

		if (pointDetector.detectPoints(yFrame, WorkerPool::get().scopedWorker()()))
		{
			const double time = timer.mseconds();

			const Points& points = pointDetector.points();

			for (const Point& point : points)
			{
				const uint8_t* color = point.sign() ? CV::Canvas::green() : CV::Canvas::blue();

				CV::Canvas::point<7u>(outputFrame, point.observation(), color);
			}

			CV::Canvas::drawText(outputFrame, String::toAString(time, 2u) + "ms, for " + String::toAString(points.size()) + " points", 5, 25, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));
		}
	}
	else if (applicationMode_ == AM_BOARD_DETECTION)
	{
		if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, outputFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		if (camera)
		{
			if (calibrationBoard_.isValid())
			{
				CalibrationBoardObservation observation;

				constexpr Scalar maximalProjectionError = Scalar(3.5);

				HighPerformanceTimer timer;

				const bool detected = CalibrationBoardDetector::detectCalibrationBoard(*camera, yFrame, calibrationBoard_, observation, maximalProjectionError, WorkerPool::get().scopedWorker()());

				const double time = timer.mseconds();

				if (detected)
				{
					CV::Calibration::Utilities::paintCalibrationBoardObservation(outputFrame, calibrationBoard_, observation, true);
				}

				const size_t detectedPoints = detected ? observation.objectPointIds().size() : 0;
				const size_t maxPoints = calibrationBoard_.numberPoints();
				const double percentage = NumericD::ratio(double(detectedPoints), double(maxPoints), 0.0) * 100.0;

				CV::Canvas::drawText(outputFrame, "Board detection: " + String::toAString(time, 2u) + "ms", 5, 5, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));
				CV::Canvas::drawText(outputFrame, "Detected points: " + String::toAString(detectedPoints) + " / " + String::toAString(maxPoints) + " (" + String::toAString(percentage, 1u) + "%)", 5, 25, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));
			}
			else
			{
				CV::Canvas::drawText(outputFrame, "No valid calibration board!", 5, 5, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));
			}
		}
		else
		{
			ocean_assert(false && "This should never happen!");

			CV::Canvas::drawText(outputFrame, "No camera profile available!", 5, 5, CV::Canvas::white(outputFrame.pixelFormat()), CV::Canvas::black(outputFrame.pixelFormat()));
		}
	}

	if (outputFrame)
	{
		setFrame(outputFrame);
	}

	repaint();
}

void DetectorMainWindow::onDebugElement(const uint32_t elementId, const Frame* frame, const DebugElements::Hierarchy* hierarchy)
{
#ifdef ENABLE_DEBUG_ELEMENTS
	Log::debug() << "New debug element id: " << elementId;

	OCEAN_SUPPRESS_UNUSED_WARNING(frame);
	OCEAN_SUPPRESS_UNUSED_WARNING(hierarchy);
#endif
}
