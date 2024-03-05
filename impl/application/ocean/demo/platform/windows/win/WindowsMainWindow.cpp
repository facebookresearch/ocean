// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/windows/win/WindowsMainWindow.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

WindowsMainWindow::WindowsMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name)
{
	// nothing to do here
}

WindowsMainWindow::~WindowsMainWindow()
{
	// nothing to do here
}

void WindowsMainWindow::onPaint()
{
	Platform::Win::Utilities::textOutput(dc(), 10, 10, "Hallo world.");
}
