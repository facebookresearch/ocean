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

void Utilities::textOutput(HDC deviceContext, const std::wstring& text, const std::wstring& font, const unsigned int fontSize, const bool bold, const AnchorPosition anchorPosition, const unsigned int windowWidth, const unsigned int windowHeight, const int32_t foregroundColor, const int32_t backgroundColor, const int32_t shadowColor, const unsigned int shadowOffsetX, const unsigned int shadowOffsetY, const unsigned int marginX, const unsigned int marginY)
{
	if (text.empty() || fontSize == 0u)
	{
		return;
	}

	HFONT textFont = CreateFontW(int(fontSize), 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, font.c_str());

	HFONT oldFont = HFONT(SelectObject(deviceContext, textFont));

	SIZE textSize;
	GetTextExtentPoint32W(deviceContext, text.c_str(), int(text.length()), &textSize);

	int positionX = 0;
	int positionY = 0;

	switch (anchorPosition)
	{
		case AP_TOP_LEFT:
			positionX = int(marginX);
			positionY = int(marginY);
			break;

		case AP_TOP_RIGHT:
			positionX = int(windowWidth) - textSize.cx - int(marginX);
			positionY = int(marginY);
			break;

		case AP_BOTTOM_LEFT:
			positionX = int(marginX);
			positionY = int(windowHeight) - textSize.cy - int(marginY);
			break;

		case AP_BOTTOM_RIGHT:
			positionX = int(windowWidth) - textSize.cx - int(marginX);
			positionY = int(windowHeight) - textSize.cy - int(marginY);
			break;

		case AP_CENTER:
			positionX = int(windowWidth) / 2 - textSize.cx / 2;
			positionY = int(windowHeight) / 2 - textSize.cy / 2;
			break;
	}

	const int oldBkMode = SetBkMode(deviceContext, backgroundColor >= 0 ? OPAQUE : TRANSPARENT);
	const COLORREF oldBkColor = backgroundColor >= 0 ? SetBkColor(deviceContext, COLORREF(backgroundColor)) : 0;

	if (shadowColor >= 0)
	{
		SetTextColor(deviceContext, COLORREF(shadowColor));
		TextOutW(deviceContext, positionX + int(shadowOffsetX), positionY + int(shadowOffsetY), text.c_str(), int(text.length()));
	}

	SetTextColor(deviceContext, COLORREF(foregroundColor));
	TextOutW(deviceContext, positionX, positionY, text.c_str(), int(text.length()));

	if (backgroundColor >= 0)
	{
		SetBkColor(deviceContext, oldBkColor);
	}

	SetBkMode(deviceContext, oldBkMode);
	SelectObject(deviceContext, oldFont);

	if (textFont)
	{
		DeleteObject(textFont);
	}
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
	{
		return CV::PixelBoundingBox();
	}

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
		{
			memcpy(logFont.lfFaceName, font.data(), font.size() * sizeof(font[0]));
		}

		newFont = CreateFontIndirectW(&logFont);
		SelectObject(dc, newFont);
	}

	SIZE boxSize;
	if (GetTextExtentPoint32W(dc, value.c_str(), int(value.length()), &boxSize) == FALSE)
	{
		return CV::PixelBoundingBox();
	}

	if (previousFont)
	{
		SelectObject(dc, previousFont);
	}

	if (newFont)
	{
		DeleteObject(newFont);
	}

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
