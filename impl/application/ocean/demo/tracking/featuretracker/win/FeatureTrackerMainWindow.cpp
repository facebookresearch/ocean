// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "FeatureTrackerMainWindow.h"

#include "ocean/platform/win/Utilities.h"

FeatureTrackerMainWindow::FeatureTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	featureTrackerWrapper_(commandArguments)
{
	initialize();
	start();
}

FeatureTrackerMainWindow::~FeatureTrackerMainWindow()
{
	featureTrackerWrapper_.release();
}

void FeatureTrackerMainWindow::onIdle()
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	if (featureTrackerWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance))
	{
		setFrame(resultingTrackerFrame);

		if (resultingTrackerPerformance >= 0.0)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));
		}
		else
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, "Place the tracking pattern in front of the camera");
		}

		repaint(false);
	}
	else
	{
		Sleep(1);
	}
}