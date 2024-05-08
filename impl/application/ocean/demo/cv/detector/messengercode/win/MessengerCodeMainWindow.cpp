/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/messengercode/win/MessengerCodeMainWindow.h"

#include "ocean/platform/win/Utilities.h"

MessengerCodeMainWindow::MessengerCodeMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	messengerCodeWrapper_(commandArguments)
{
	initialize();
	start();
}

MessengerCodeMainWindow::~MessengerCodeMainWindow()
{
	messengerCodeWrapper_.release();
}

void MessengerCodeMainWindow::onIdle()
{
	Frame resultingFrame;
	double resultingPerformance;
	std::vector<std::string> messages;

	messengerCodeWrapper_.detectAndDecode(resultingFrame, resultingPerformance, messages);

	if (resultingFrame.isValid())
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
