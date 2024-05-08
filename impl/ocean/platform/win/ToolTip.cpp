/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/ToolTip.h"

#include <CommCtrl.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

bool ToolTip::addToolTip(const HWND handle, const std::wstring& text)
{
	ocean_assert(handle);
	if (handle == nullptr)
		return false;

	HWND hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, GetParent(handle), nullptr, GetModuleHandle(nullptr), nullptr);

	ocean_assert(hwndTip);
	if (hwndTip == nullptr)
		return false;

	TOOLINFO toolInfo = {0};
	toolInfo.cbSize = TTTOOLINFOA_V1_SIZE;
	toolInfo.hwnd = GetParent(handle);
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)handle;
	toolInfo.lpszText = (LPWSTR)text.c_str();

	if (SendMessage(hwndTip, TTM_ACTIVATE, TRUE, 0))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// check whether we have a multiline tool tip
	if (text.find(L'\n') != std::string::npos)
	{
		HFONT fontHandle = (HFONT)SendMessage((HWND)hwndTip, WM_GETFONT, 0, (LPARAM)0);

		if (!fontHandle)
		{
			fontHandle = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			if (!fontHandle)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}
		}

		HDC hdc = GetDC(nullptr);

		if (hdc == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!SelectObject(hdc, fontHandle))
		{
			ReleaseDC(nullptr, hdc);
			return false;
		}

		int max = 0;

		std::string::size_type start = 0;
		std::string::size_type end = std::string::npos;

		while (true)
		{
			end = text.find(L'\n', start);
			std::wstring line = text.substr(start, end == std::string::npos ? end : end - start);

			SIZE sz;
			if (!GetTextExtentPoint32(hdc, line.c_str(), int(line.length()), &sz))
				ocean_assert(false && "This should never happen!");

			if (sz.cx > max)
				max = sz.cx;

			if (end == std::string::npos)
				break;

			start = end + 1;
		}

		if (max > SendMessage(hwndTip, TTM_GETMAXTIPWIDTH, 0, 0))
			SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, max);
	}

	return true;
}

}

}

}
