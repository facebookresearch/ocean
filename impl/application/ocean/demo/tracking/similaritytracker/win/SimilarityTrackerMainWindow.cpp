/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/similaritytracker/win/SimilarityTrackerMainWindow.h"

#include "ocean/platform/win/Utilities.h"

SimilarityTrackerMainWindow::SimilarityTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	similarityTrackerWrapper_(commandArguments),
	recentTouchPosition_(Numeric::minValue(), Numeric::minValue())
{
	initialize();
	start();
}

SimilarityTrackerMainWindow::~SimilarityTrackerMainWindow()
{
	similarityTrackerWrapper_.release();
}

void SimilarityTrackerMainWindow::onIdle()
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	if (similarityTrackerWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance, recentTouchPosition_))
	{
		setFrame(resultingTrackerFrame);

		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));

		repaint(false);

		recentTouchPosition_ = Vector2(Numeric::minValue(), Numeric::minValue());
	}
	else
	{
		Sleep(1);
	}
}

void SimilarityTrackerMainWindow::onMouseDown(const MouseButton /*button*/, const int x, const int y)
{
	recentTouchPosition_ = Vector2(Scalar(x), Scalar(y));
}
