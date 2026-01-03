/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/slamtracker/win/SLAMTrackerMainWindow.h"

#include "ocean/cv/Canvas.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

SLAMTrackerMainWindow::SLAMTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	slamTrackerWrapper_(commandArguments)
{
	initialize();

	start();
}

SLAMTrackerMainWindow::~SLAMTrackerMainWindow()
{
	slamTrackerWrapper_.release();
}

void SLAMTrackerMainWindow::onIdle()
{
	Frame resultingTrackerFrame;

	if (slamTrackerWrapper_.trackNewFrame(resultingTrackerFrame))
	{
		if (isRecording_)
		{
			CV::Canvas::drawText(resultingTrackerFrame, "Recording...", 5, int(resultingTrackerFrame.height()) - 25, CV::Canvas::black());
		}

		setFrame(resultingTrackerFrame);

		repaint(false);
	}
	else
	{
		Sleep(1);
	}
}

void SLAMTrackerMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "R")
		{
			if (!isRecording_)
			{
				if (slamTrackerWrapper_.startRecording())
				{
					isRecording_ = true;
				}
			}
			else
			{
				if (slamTrackerWrapper_.stopRecording())
				{
					isRecording_ = false;
				}
			}
		}
	}
}
