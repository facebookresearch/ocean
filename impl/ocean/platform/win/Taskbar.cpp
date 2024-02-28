// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/win/Taskbar.h"

#include <winsock2.h>
#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

bool Taskbar::show()
{
	HWND handle = FindWindowA("Shell_TrayWnd", "");
	if (handle == nullptr)
		return false;

	return ShowWindow(handle, SW_SHOW) == TRUE;
}

bool Taskbar::hide()
{
	HWND handle = FindWindowA("Shell_TrayWnd", "");
	if (handle == nullptr)
		return false;

	return ShowWindow(handle, SW_HIDE) == TRUE;
}

}

}

}
