// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/pointtracker/win/PointTrackerMainWindow.h"

#include "ocean/platform/win/Utilities.h"

PointTrackerMainWindow::PointTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	pointTrackerWrapper_(commandArguments)
{
	initialize();
	start();
}

PointTrackerMainWindow::~PointTrackerMainWindow()
{
	pointTrackerWrapper_.release();
}

void PointTrackerMainWindow::onKeyDown(const int /*key*/)
{
	pointTrackerWrapper_.nextTrackingMode();
}

void PointTrackerMainWindow::onIdle()
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	if (pointTrackerWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance))
	{
		setFrame(resultingTrackerFrame);

		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, "Mode: " + pointTrackerWrapper_.trackingMode());

		repaint(false);
	}
	else
	{
		Sleep(1);
	}
}
