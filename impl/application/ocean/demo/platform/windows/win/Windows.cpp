// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/windows/win/Windows.h"
#include "application/ocean/demo/platform/windows/win/WindowsMainWindow.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	WindowsMainWindow mainWindow(hInstance, L"MainWindow");
	mainWindow.initialize();

	mainWindow.start();

	return 0;
}
