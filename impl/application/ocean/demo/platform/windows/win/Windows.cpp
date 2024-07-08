/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/windows/win/Windows.h"
#include "application/ocean/demo/platform/windows/win/WindowsMainWindow.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	WindowsMainWindow mainWindow(hInstance, L"MainWindow");
	mainWindow.initialize();

	mainWindow.start();

	return 0;
}
