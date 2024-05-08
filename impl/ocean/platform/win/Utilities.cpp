/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Utilities.h"
#include "ocean/platform/win/Bitmap.h"

#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

void Utilities::textOutput(HDC dc, const int x, const int y, const std::string& text)
{
	TextOutA(dc, x, y, text.c_str(), int(text.length()));
}

void Utilities::desktopTextOutput(const int x, const int y, const std::string& text)
{
	HDC dc = GetDC(0);
	textOutput(dc, x, y, text);
	ReleaseDC(0, dc);
}

void Utilities::frameOutput(HDC dc, const int x, const int y, const Frame& frame)
{
	Bitmap bitmap(frame);
	BitBlt(dc, x, y, bitmap.width(), bitmap.height(), bitmap.dc(), 0, 0, SRCCOPY);
}

void Utilities::frameOutput(HDC dc, const int x, const int y, const unsigned int width, const unsigned int height, const Frame& frame)
{
	Bitmap bitmap(frame);
	SetStretchBltMode(dc, HALFTONE);
	StretchBlt(dc, x, y, width, height, bitmap.dc(), 0, 0, bitmap.width(), bitmap.height(), SRCCOPY);
}

void Utilities::desktopFrameOutput(const int x, const int y, const Frame& frame)
{
	HDC dc = GetDC(0);
	frameOutput(dc, x, y, frame);
	ReleaseDC(0, dc);
}

void Utilities::desktopFrameOutput(const int x, const int y, const unsigned int width, const unsigned int height, const Frame& frame)
{
	HDC dc = GetDC(0);
	frameOutput(dc, x, y, width, height, frame);
	ReleaseDC(0, dc);
}

void Utilities::bitmapOutput(HDC dc, const int x, const int y, const Bitmap& bitmap)
{
	BitBlt(dc, x, y, bitmap.width(), bitmap.height(), bitmap.dc(), 0, 0, SRCCOPY);
}

void Utilities::bitmapOutput(HDC dc, const int x, const int y, const unsigned int width, const unsigned int height, const Bitmap& bitmap)
{
	SetStretchBltMode(dc, HALFTONE);
	StretchBlt(dc, x, y, width, height, bitmap.dc(), 0, 0, bitmap.width(), bitmap.height(), SRCCOPY);
}

void Utilities::desktopBitmapOutput(const int x, const int y, const Bitmap& bitmap)
{
	HDC dc = GetDC(0);
	bitmapOutput(dc, x, y, bitmap);
	ReleaseDC(0, dc);
}

void Utilities::desktopBitmapOutput(const int x, const int y, const unsigned int width, const unsigned int height, const Bitmap& bitmap)
{
	HDC dc = GetDC(0);
	bitmapOutput(dc, x, y, width, height, bitmap);
	ReleaseDC(0, dc);
}

CV::PixelBoundingBox Utilities::textBoundingBox(const std::string& value, const std::string& font, const unsigned int size)
{
	return textBoundingBox(String::toWString(value), String::toWString(font), size);
}

CV::PixelBoundingBox Utilities::textBoundingBox(const std::wstring& value, const std::wstring& font, const unsigned int size)
{
	if (value.empty() || size == 0u)
		return CV::PixelBoundingBox();

	HDC dc = GetDC(nullptr);

	HFONT previousFont = nullptr;
	HFONT newFont = nullptr;

	if (!font.empty())
	{
		LOGFONTW logFont;
		memset(&logFont, 0, sizeof(LOGFONT));

		logFont.lfHeight = LONG(size);
		logFont.lfWeight = FW_NORMAL;
		logFont.lfQuality = CLEARTYPE_QUALITY;

		if (font.size() < LF_FACESIZE)
			memcpy(logFont.lfFaceName, font.data(), font.size() * sizeof(font[0]));

		newFont = CreateFontIndirectW(&logFont);
		SelectObject(dc, newFont);
	}

	SIZE boxSize;
	if (GetTextExtentPoint32W(dc, value.c_str(), int(value.length()), &boxSize) == FALSE)
		return CV::PixelBoundingBox();

	if (previousFont)
		SelectObject(dc, previousFont);

	if (newFont)
		DeleteObject(newFont);

	ReleaseDC(nullptr, dc);

	ocean_assert(boxSize.cx >= 0 && boxSize.cy >= 0);
	return CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), (unsigned int)boxSize.cx, (unsigned int)boxSize.cy);
}

ScopedDisableWindow::ScopedDisableWindow(const HWND windowHandle) :
	handle_(windowHandle)
{
	const ScopedLock scopedLock(DisableWindowCounter::get().lock());

	ocean_assert(handle_);

	if (DisableWindowCounter::get().increase(handle_))
	{
		ocean_assert(IsWindowEnabled(handle_));
		EnableWindow(handle_, false);
	}
}

void ScopedDisableWindow::release()
{
	const ScopedLock scopedLock(DisableWindowCounter::get().lock());

	if (handle_)
	{
		if (DisableWindowCounter::get().decrease(handle_))
		{
			ocean_assert(!IsWindowEnabled(handle_));
			EnableWindow(handle_, true);
		}

		handle_ = nullptr;
	}
}

}

}

}
