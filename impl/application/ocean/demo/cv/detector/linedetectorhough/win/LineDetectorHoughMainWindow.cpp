/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/linedetectorhough/win/LineDetectorHoughMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterScharr.h"
#include "ocean/cv/FrameFilterSobel.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/LineDetectorHough.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

LineDetectorHoughMainWindow::LineDetectorHoughMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& filename) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFilename_(filename)
{
	// nothing to do here
}

LineDetectorHoughMainWindow::~LineDetectorHoughMainWindow()
{
	// nothing to do here
}

void LineDetectorHoughMainWindow::onInitialized()
{
	if (!mediaFilename_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFilename_);
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
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
		frameMedium_->start();
	}
}

void LineDetectorHoughMainWindow::onIdle()
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

void LineDetectorHoughMainWindow::onKeyDown(const int key)
{
	std::string keyString;
	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "up")
		{
			strengthThreshold_ = min(strengthThreshold_ + Scalar(0.5), Scalar(250));
		}
		else if (keyString == "down")
		{
			strengthThreshold_ = max(Scalar(0.5), strengthThreshold_ - Scalar(0.5));
		}
		else if (keyString == "1")
		{
			mode_ = mode_ ^ DM_FRAME_24BIT;
		}
		else if (keyString == "2")
		{
			mode_ = mode_ ^ DM_SCHARR;
		}
		else if (keyString == "3")
		{
			mode_ = mode_ ^ DM_HORIZONTAL_VERTICAL_DIAGONAL;
		}
		else if (keyString == "4")
		{
			mode_ = mode_ ^ DM_FINITE_LINES;
		}
		else if (keyString == "5")
		{
			mode_ = mode_ ^ DM_OPTIMIZED_LINES;
		}
		else if (keyString == "6")
		{
			mode_ = mode_ ^ DM_ADAPTIVE_THRESHOLD;
		}
		else if (keyString == "7")
		{
			mode_ = mode_ ^ DM_BIN_ACCURATE_PEAK;
		}
		else if (keyString == "+" || keyString == "=")
		{
			mode_ = (mode_ + 1u) % DM_INVALID;
		}
		else if (keyString == "-")
		{
			if (mode_ >= 0u)
			{
				mode_--;
			}
			else
			{
				mode_ = DM_INVALID - 1u;
			}
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

void LineDetectorHoughMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_ALWAYS_COPY, &worker_))
	{
		return;
	}

	CV::Detector::LineDetectorHough::InfiniteLines infiniteLines;
	FiniteLines2 finiteLines;

	Frame detectorFrame(topLeft, Frame::ACM_USE_KEEP_LAYOUT);

	if ((mode_ & DM_FRAME_24BIT) == 0)
	{
		if (!CV::FrameConverter::Comfort::convert(topLeft, FrameType::FORMAT_Y8, detectorFrame, CV::FrameChannels::CP_ALWAYS_COPY, &worker_))
		{
			ocean_assert(false && "Unsupported pixel format!");
			return;
		}
	}

	const CV::Detector::LineDetectorHough::FilterType filterType = (mode_ & DM_SCHARR) ? CV::Detector::LineDetectorHough::FT_SCHARR : CV::Detector::LineDetectorHough::FT_SOBEL;
	const CV::Detector::LineDetectorHough::FilterResponse filterResponse = (mode_ & DM_HORIZONTAL_VERTICAL_DIAGONAL) ? CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL_DIAGONAL : CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL;

	const unsigned int adaptiveThresholdWindow = (mode_ & DM_ADAPTIVE_THRESHOLD) ? 51u : 0u;

	FiniteLines2* optionalFiniteLines = (mode_ & DM_FINITE_LINES) ? &finiteLines : nullptr;

	const bool optimizeLines = (mode_ & DM_OPTIMIZED_LINES) ? true : false;

	const bool determineExactPeak = (mode_ & DM_BIN_ACCURATE_PEAK) ? false : true;

	HighPerformanceTimer timer;

	if (optionalFiniteLines && filterResponse != CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL_DIAGONAL)
	{
		// currently missing implementation

		setFrame(topLeft);
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, translateApplicationMode(mode_) + std::string(", Missing implementation..."));
		repaint(false);
		return;
	}

	if (adaptiveThresholdWindow == 0u)
	{
		// we do not apply the adaptive thresholding to rate the individual peaks
		CV::Detector::LineDetectorHough::detectLines(detectorFrame, filterType, filterResponse, infiniteLines, optionalFiniteLines, optimizeLines, (unsigned int)(strengthThreshold_), 4u, 5u, determineExactPeak, &worker_, 360u, (unsigned int)(-1), true, Scalar(10), Numeric::deg2rad(5));
	}
	else
	{
		// we use the adaptive thresholding (based on a surrounding window) to rate the individual peaks
		CV::Detector::LineDetectorHough::detectLinesWithAdaptiveThreshold(detectorFrame, filterType, filterResponse, infiniteLines, optionalFiniteLines, optimizeLines, strengthThreshold_, adaptiveThresholdWindow, 4u, 5u, determineExactPeak, &worker_, 360u, (unsigned int)(-1), true, Scalar(10), Numeric::deg2rad(5));
	}

	const double msTime = timer.mseconds();

	if (optionalFiniteLines)
	{
		paintLines(topLeft, *optionalFiniteLines, &worker_);
	}
	else
	{
		Scalars strengths(infiniteLines.size());
		const Lines2 lines(CV::Detector::LineDetectorHough::InfiniteLine::cornerAlignedLines(infiniteLines.data(), infiniteLines.size(), topLeft.width(), topLeft.height(), strengths.data()));

		paintLines(topLeft, lines, strengths, &worker_);
	}

	setFrame(topLeft);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, translateApplicationMode(mode_));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 35, std::string("Threshold: ") + String::toAString(strengthThreshold_, 1u) + std::string(", lines: ") + String::toAString(infiniteLines.size()));
	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 55, std::string("Performance: ") + String::toAString(msTime) + std::string("ms"));

	repaint(false);
}

void LineDetectorHoughMainWindow::paintLines(Frame& frame, const Lines2& lines, const Scalars& strengths, Worker* worker)
{
	ocean_assert(frame && FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 3u));
	ocean_assert(lines.size() == strengths.size());

	if (lines.empty())
	{
		return;
	}

	Scalar minStrength = Numeric::maxValue();
	Scalar maxStrength = Numeric::minValue();

	for (Scalars::const_iterator i = strengths.begin(); i != strengths.end(); ++i)
	{
		minStrength = min(*i, minStrength);
		maxStrength = max(*i, maxStrength);
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintLines, &frame, lines.data(), strengths.data(), minStrength, maxStrength, 0u, 0u), 0u, (unsigned int)(lines.size()), 5u, 6u, 20u);
	}
	else
	{
		paintLines(&frame, lines.data(), strengths.data(), minStrength, maxStrength, 0u, (unsigned int)(lines.size()));
	}
}

void LineDetectorHoughMainWindow::paintLines(Frame& frame, const FiniteLines2& lines, Worker* worker)
{
	ocean_assert(frame && FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 3u));

	if (lines.empty())
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintLines, &frame, lines.data(), 0u, 0u), 0u, (unsigned int)(lines.size()), 2u, 3u, 20u);
	}
	else
	{
		paintLines(&frame, lines.data(), 0u, (unsigned int)(lines.size()));
	}
}

void LineDetectorHoughMainWindow::paintLines(Frame* frame, const Line2* lines, const Scalar* strengths, const Scalar minStrength, const Scalar maxStrength, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(frame != nullptr && frame->isValid());
	ocean_assert(lines && strengths && numberLines != 0u);

	ocean_assert(maxStrength >= minStrength);

	ocean_assert(Numeric::isNotEqualEps(maxStrength));
	const Scalar strengthNormalization = Scalar(255) / (maxStrength - minStrength);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		const Line2& line = lines[n];

		ocean_assert(minStrength <= strengths[n] && strengths[n] <= maxStrength);

		const uint8_t colorChannel = (unsigned char)(minmax<Scalar>(0, (strengths[n] - minStrength) * strengthNormalization, 255));
		const uint8_t color[3] = {colorChannel, colorChannel, colorChannel};

		CV::Canvas::line<1u>(*frame, line, color);
	}
}

void LineDetectorHoughMainWindow::paintLines(Frame* frame, const FiniteLine2* lines, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(frame != nullptr && frame->isValid());
	ocean_assert(lines && numberLines != 0u);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		CV::Canvas::line<1u>(*frame, lines[n], CV::Canvas::black());
	}
}

std::string LineDetectorHoughMainWindow::translateApplicationMode(const unsigned int mode)
{
	ocean_assert(mode <= DM_INVALID);

	std::vector<std::string> strings;

	if (mode & DM_FRAME_24BIT)
	{
		strings.push_back("24 bit");
	}
	else
	{
		strings.push_back("8 bit");
	}

	if (mode & DM_SCHARR)
	{
		strings.push_back("Scharr");
	}
	else
	{
		strings.push_back("Sobel");
	}

	if (mode & DM_HORIZONTAL_VERTICAL_DIAGONAL)
	{
		strings.push_back("4 Responses");
	}
	else
	{
		strings.push_back("2 Responses");
	}

	if (mode & DM_FINITE_LINES)
	{
		strings.push_back("Finite");
	}
	else
	{
		strings.push_back("Infinite");
	}

	if (mode & DM_OPTIMIZED_LINES)
	{
		strings.push_back("Optimized");
	}
	else
	{
		strings.push_back("Not Optimized");
	}

	if (mode & DM_ADAPTIVE_THRESHOLD)
	{
		strings.push_back("Adaptive Threshold");
	}
	else
	{
		strings.push_back("Simple Threshold");
	}

	if (mode & DM_BIN_ACCURATE_PEAK)
	{
		strings.push_back("Pixel Accurate");
	}
	else
	{
		strings.push_back("Sub-pixel Accurate");
	}


	if (strings.empty())
	{
		return std::string();
	}

	std::string result(strings.front());

	for (size_t n = 1; n < strings.size(); ++n)
	{
		result += std::string(", ") + strings[n];
	}

	return result;
}
