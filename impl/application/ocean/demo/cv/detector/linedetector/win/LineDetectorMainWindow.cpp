/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/linedetector/win/LineDetectorMainWindow.h"
#include "application/ocean/demo/cv/detector/linedetector/win/Resource.h"

#include "ocean/platform/win/Utilities.h"

LineDetectorMainWindow::LineDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	lineDetectorWrapper_(commandArguments)
{
	initialize(instance, IDI_ICON);

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
