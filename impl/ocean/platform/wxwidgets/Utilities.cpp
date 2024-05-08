/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/Utilities.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

void Utilities::textOutput(wxDC& dc, const int x, const int y, const wxString& text)
{
	ocean_assert(dc.IsOk());

	const wxColour oldColor = dc.GetTextBackground();
	const int oldMode = dc.GetBackgroundMode();

	dc.SetTextBackground(*wxWHITE);
	dc.SetBackgroundMode(wxPENSTYLE_SOLID);

	dc.DrawText(text, x, y);

	dc.SetTextBackground(oldColor);
	dc.SetBackgroundMode(oldMode);
}

void Utilities::textOutput(wxDC& dc, const int x, const int y, const std::string& text)
{
	textOutput(dc, x, y, wxString(String::toWString(text).c_str()));
}

void Utilities::desktopTextOutput(const int x, const int y, const wxString& text)
{
	wxScreenDC dc;

	const wxColour oldColor = dc.GetTextBackground();
	const int oldMode = dc.GetBackgroundMode();

	dc.SetTextBackground(*wxWHITE);
	dc.SetBackgroundMode(wxPENSTYLE_SOLID);

	dc.DrawText(text, x, y);

	dc.SetTextBackground(oldColor);
	dc.SetBackgroundMode(oldMode);
}

void Utilities::desktopTextOutput(const int x, const int y, const std::string& text)
{
	desktopTextOutput(x, y, wxString(String::toWString(text).c_str()));
}

void Utilities::bitmapOutput(wxDC& dc, const int x, const int y, const wxBitmap& bitmap)
{
	ocean_assert(dc.IsOk() && bitmap.IsOk());

	wxMemoryDC bitmapDC;
	bitmapDC.SelectObjectAsSource(bitmap);

	dc.Blit(x, y, bitmap.GetWidth(), bitmap.GetHeight(), &bitmapDC, 0, 0);
}

void Utilities::frameOutput(wxDC& dc, const int x, const int y, const Frame& frame)
{
	Frame rgbFrame;

	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
	{
		return;
	}

	rgbFrame.makeContinuous();

	const wxImage windowImage = wxImage(int(rgbFrame.width()), int(rgbFrame.height()), const_cast<uint8_t*>(rgbFrame.constdata<uint8_t>()), true);

	bitmapOutput(dc, x, y, wxBitmap(windowImage));
}

void Utilities::desktopBitmapOutput(const int x, const int y, const wxBitmap& bitmap)
{
	wxScreenDC dc;

	bitmapOutput(dc, x, y, bitmap);
}

void Utilities::desktopFrameOutput(const int x, const int y, const Frame& frame)
{
	wxScreenDC dc;

	frameOutput(dc, x, y, frame);
}

void Utilities::bitmapOutput(wxDC& dc, const int xOutput, const int yOutput, const int widthOutput, const int heightOutput, const int xInput, const int yInput, const int widthInput, const int heightInput, const wxBitmap& bitmap)
{
	if (widthOutput <= 0 || heightOutput <= 0 || widthInput <= 0 || heightInput <= 0)
		return;

	ocean_assert(dc.IsOk() && bitmap.IsOk());

	double oldScaleX = 1;
	double oldScaleY = 1;
	dc.GetUserScale(&oldScaleX, &oldScaleY);

	const double scaleX = double(widthOutput) / double(widthInput);
	const double scaleY = double(heightOutput) / double(heightInput);
	dc.SetUserScale(scaleX, scaleY);

	ocean_assert(scaleX != 0 && scaleY != 0);
	const double xOutputScaled = double(xOutput) / scaleX;
	const double yOutputScaled = double(yOutput) / scaleY;

	const int xOutputScaledR = int(floor(xOutputScaled + 0.5));
	const int yOutputScaledR = int(floor(yOutputScaled + 0.5));

	wxMemoryDC bitmapDC;
	bitmapDC.SelectObjectAsSource(bitmap);

	dc.Blit(xOutputScaledR, yOutputScaledR, widthInput, heightInput, &bitmapDC, xInput, yInput);

	dc.SetUserScale(oldScaleX, oldScaleY);
}

CV::PixelBoundingBox Utilities::textBoundingBox(const std::string& value, const std::string& font, const unsigned int size)
{
	return textBoundingBox(String::toWString(value), String::toWString(font), size);
}

CV::PixelBoundingBox Utilities::textBoundingBox(const std::wstring& value, const std::wstring& font, const unsigned int size)
{
	wxScreenDC dc;

	if (!font.empty())
	{
		wxFont fontObject(int(size), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxString(font));

		wxCoord width = 0;
		wxCoord height = 0;
		dc.GetTextExtent(value, &width, &height, nullptr, nullptr, &fontObject);

		ocean_assert(width >= 0 && height >= 0);
		return CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), (unsigned int)width, (unsigned int)height);
	}
	else
	{
		wxCoord width = 0;
		wxCoord height = 0;
		dc.GetTextExtent(value, &width, &height);

		ocean_assert(width >= 0 && height >= 0);
		return CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), (unsigned int)width, (unsigned int)height);
	}
}

}

}

}
