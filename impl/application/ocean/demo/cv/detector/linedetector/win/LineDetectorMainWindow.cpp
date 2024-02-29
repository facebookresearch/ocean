// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/linedetector/win/LineDetectorMainWindow.h"

#include "ocean/platform/win/Utilities.h"

LineDetectorMainWindow::LineDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	lineDetectorWrapper_(commandArguments)
{
	initialize();
	start();
}

LineDetectorMainWindow::~LineDetectorMainWindow()
{
	lineDetectorWrapper_.release();
}

void LineDetectorMainWindow::onIdle()
{
	Frame resultingFrame;
	double resultingPerformance;

	if (lineDetectorWrapper_.detectNewFrame(resultingFrame, resultingPerformance))
	{
		setFrame(resultingFrame);

		if (resultingPerformance >= 0.0)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingPerformance * 1000.0, 2u) + std::string("ms"));
		}

		repaint(false);
	}
	else
	{
		Sleep(1);
	}
}
