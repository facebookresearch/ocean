/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
