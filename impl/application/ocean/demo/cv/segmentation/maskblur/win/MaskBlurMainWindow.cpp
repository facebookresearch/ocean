/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/maskblur/win/MaskBlurMainWindow.h"
#include "application/ocean/demo/cv/segmentation/maskblur/win/Resource.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/segmentation/FrameFilterBlur.h"

#include "ocean/io/image/Image.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

MaskBlurMainWindow::MaskBlurMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name)
{
	initialize(instance, IDI_ICON);

	setEnableDropAndDrop();

	start();
}

MaskBlurMainWindow::~MaskBlurMainWindow()
{
	// nothing to do here
}

void MaskBlurMainWindow::onInitialized()
{
	adjustToBitmapSize();
}

void MaskBlurMainWindow::onIdle()
{
	onPaint();

	Sleep(1);
}

void MaskBlurMainWindow::onMouseDown(const MouseButton button, const int x, const int y)
{
	int bitmapX = -1;
	int bitmapY = -1;

	if (window2bitmap(x, y, bitmapX, bitmapY))
	{
		if (button == BUTTON_LEFT)
		{
			drawMask(bitmapX, bitmapY, 0x00);
		}
		else if (button == BUTTON_RIGHT)
		{
			drawMask(bitmapX, bitmapY, 0xFF);
		}
	}
}

void MaskBlurMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	int bitmapX = -1;
	int bitmapY = -1;

	if (window2bitmap(x, y, bitmapX, bitmapY))
	{
		if (buttons & BUTTON_LEFT)
		{
			drawMask(bitmapX, bitmapY, 0x00);
		}
		else if (buttons & BUTTON_RIGHT)
		{
			drawMask(bitmapX, bitmapY, 0xFF);
		}
	}
}

void MaskBlurMainWindow::onKeyDown(const int key)
{
	std::string keyType;
	if (Platform::Win::Keyboard::translateVirtualkey(key, keyType) && keyType == "B")
	{
		blurImage();
	}
}

void MaskBlurMainWindow::onPaint()
{
	if (maskHasChanged_)
	{
		maskHasChanged_ = false;

		Frame image(image_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		for (unsigned int y = 0u; y < mask_.height(); ++y)
		{
			const uint8_t* rowMask = mask_.constrow<uint8_t>(y);
			uint8_t* rowImage = image.row<uint8_t>(y);

			if (image_.pixelFormat() == FrameType::FORMAT_RGB24)
			{
				for (unsigned int x = 0u; x < mask_.width(); ++x)
				{
					if (rowMask[x] == 0x00u)
					{
						rowImage[x * 3u + 0u] = 0xFFu;
						rowImage[x * 3u + 1u] /= 2u;
						rowImage[x * 3u + 2u] /= 2u;
					}
				}
			}
			else
			{
				for (unsigned int x = 0u; x < mask_.width(); ++x)
				{
					if (rowMask[x] == 0x00u)
					{
						rowImage[x] = ~(rowImage[x]);
					}
				}
			}
		}

		setFrame(std::move(image));
	}

	BitmapWindow::onPaint();
}

void MaskBlurMainWindow::onDragAndDrop(const Files& files)
{
	if (files.empty())
	{
		return;
	}

	Frame image = IO::Image::readImage(String::toAString(files.front()));

	if (!image.isValid())
	{
		return;
	}

	const FrameType::PixelFormat targetPixelFormat = image.channels() == 1u ? FrameType::FORMAT_Y8 : FrameType::FORMAT_RGB24;

	if (!CV::FrameConverter::Comfort::change(image, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT))
	{
		return;
	}

	setFrame(image);

	image_ = std::move(image);

	mask_ = Frame(FrameType(image_, FrameType::FORMAT_Y8));
	mask_.setValue(0xFFu);

	maskHasChanged_ = true;
}

void MaskBlurMainWindow::drawMask(const int x, const int y, const uint8_t value)
{
	if (!mask_.isValid())
	{
		return;
	}

	if (x >= 0 && x < int(mask_.width()) && y >= 0 && y < int(mask_.height()))
	{
		const unsigned int size = (GetKeyState(VK_CONTROL) & 0x8000) ? 91u : 31u;

		CV::Canvas::ellipse(mask_, CV::PixelPosition(x, y), size, size, &value);

		maskHasChanged_ = true;
	}
}

bool MaskBlurMainWindow::blurImage()
{
	const HighPerformanceTimer timer;

	if (!CV::Segmentation::FrameFilterBlur::Comfort::blurMaskRegions(image_, mask_, 21u, nullptr))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const double ms = timer.mseconds();

	setText(L"Performance: " + String::toWString(ms, 1u) + L"ms");

	mask_.setValue(0xFFu);
	maskHasChanged_ = true;

	return true;
}
