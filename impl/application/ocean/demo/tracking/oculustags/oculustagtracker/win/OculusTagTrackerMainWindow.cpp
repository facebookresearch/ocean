/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/oculustags/oculustagtracker/win/OculusTagTrackerMainWindow.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/platform/win/Utilities.h"

OculusTagTrackerMainWindow::OculusTagTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	oculusTagTrackerWrapper_(commandArguments)
{
	initialize();
	start();
}

OculusTagTrackerMainWindow::~OculusTagTrackerMainWindow()
{
	oculusTagTrackerWrapper_.release();
}

void OculusTagTrackerMainWindow::onIdle()
{
	Frame resultingFrame;
	double resultingTrackerPerformance;

	if (oculusTagTrackerWrapper_.trackNewFrame(resultingFrame, resultingTrackerPerformance) && resultingFrame.isValid())
	{
		setFrame(resultingFrame);

		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingTrackerPerformance * 1000.0, 2u) + std::string("ms"));

		repaint(false);
	}
	else
	{
		Sleep(1);
	}
}
