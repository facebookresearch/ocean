// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/fbmessengercodes/win/FBMessengerCodesMainWindow.h"

#include "ocean/platform/win/Utilities.h"

FBMessengerCodesMainWindow::FBMessengerCodesMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	messengerCodesWrapper_(commandArguments)
{
	initialize();
	start();
}

FBMessengerCodesMainWindow::~FBMessengerCodesMainWindow()
{
	messengerCodesWrapper_.release();
}

void FBMessengerCodesMainWindow::onIdle()
{
	Frame resultingFrame;
	double resultingPerformance;
	std::vector<std::string> messages;

	messengerCodesWrapper_.detectAndDecode(resultingFrame, resultingPerformance, messages);

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
