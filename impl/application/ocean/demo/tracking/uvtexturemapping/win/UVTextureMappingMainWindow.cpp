// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/demo/tracking/uvtexturemapping/win/UVTextureMappingMainWindow.h"

#include "ocean/platform/win/Utilities.h"

UVTextureMappingMainWindow::UVTextureMappingMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	uvTextureMappingWrapper_(commandArguments)
{
	initialize();
	start();
}

UVTextureMappingMainWindow::~UVTextureMappingMainWindow()
{
	uvTextureMappingWrapper_.release();
}

void UVTextureMappingMainWindow::onIdle()
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	if (uvTextureMappingWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance))
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
