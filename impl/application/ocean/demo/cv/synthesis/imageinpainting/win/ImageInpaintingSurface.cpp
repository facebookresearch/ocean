/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingSurface.h"
#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingMainWindow.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/advanced/AdvancedFrameConverter.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/cv/synthesis/SynthesisPyramidI1.h"

#include "ocean/platform/wxwidgets/Utilities.h"

using namespace Ocean;

Surface::Surface(wxWindow* parent) :
	BitmapWindow(L"Surface", parent)
{
	// nothing to do here
}

bool Surface::setFrame(const Frame& frame)
{
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, frame_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	image_ = wxImage(int(frame.width()), int(frame.height()), false);

	mask_.set(FrameType(frame_, FrameType::FORMAT_Y8), true, true);
	mask_.setValue(0xFFu);

	updateBitmap();
	Refresh();

	return true;
}

bool Surface::setMask(const Frame& mask)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(mask, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	if (!mask_.isFrameTypeCompatible(topLeft.frameType(), false))
	{
		return false;
	}

	mask_.copy(0, 0, topLeft);

	updateBitmap();
	Refresh();

	return true;
}

bool Surface::executeInpainting(const unsigned int quality)
{
	const WorkerPool::ScopedWorker worker(WorkerPool::get().scopedWorker());

	CV::Synthesis::SynthesisPyramidI1 synthesisPyramid;
	synthesisPyramid.arrange(frame_, mask_, worker(), false, false);

	switch (quality)
	{
		case 0u:
		{
			if (lines_.empty())
			{
				synthesisPyramid.applyInpainting(CV::Synthesis::SynthesisPyramid::SQ_VERY_HIGH, randomGenerator_, 5u, 25u, 0xFFFFFFFF, worker());
			}
			else
			{
				CV::Synthesis::Constraints constraints;

				for (Lines::const_iterator i = lines_.begin(); i != lines_.end(); ++i)
					constraints.addConstraint(std::make_unique<CV::Synthesis::FiniteLineConstraint>(i->first, i->second, 200, 40, 500, true, true));

				synthesisPyramid.applyInpainting(constraints, randomGenerator_, 10u, 25u, 0xFFFFFFFF, 4u, 2u, worker());
			}

			break;
		}

		case 1u:
		{
			synthesisPyramid.applyInpainting(CV::Synthesis::SynthesisPyramidI1::SQ_HIGH, randomGenerator_, 10u, 25u, 0xFFFFFFFF, worker());
			break;
		}

		case 2u:
		{
			synthesisPyramid.applyInpainting(CV::Synthesis::SynthesisPyramidI1::SQ_MODERATE, randomGenerator_, 10u, 25u, 0xFFFFFFFF, worker());
			break;
		}

		default:
		{
			synthesisPyramid.applyInpainting(CV::Synthesis::SynthesisPyramidI1::SQ_LOW, randomGenerator_, 10u, 25u, 0xFFFFFFFF, worker());
			break;
		}
	}

	synthesisPyramid.createInpaintingResult(frame_, worker());

	previousMask_ = Frame(mask_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	mask_.setValue(0xFFu);

	lineMode_ = LM_NONE;
	lines_.clear();

	updateBitmap();
	Refresh();

	return true;
}

bool Surface::executeInpaintingTexture()
{
	const WorkerPool::ScopedWorker worker(WorkerPool::get().scopedWorker());

	Frame textureFrame;
	if (!CV::Advanced::AdvancedFrameConverter::convertToYUVT32ScharrMagnitude(frame_, textureFrame, 21u, worker()))
	{
		return false;
	}

	CV::Synthesis::SynthesisPyramidI1 synthesisPixel;
	synthesisPixel.arrange(textureFrame, mask_, worker());

	synthesisPixel.applyInpainting(CV::Synthesis::SynthesisPyramidI1::IT_PATCH_FULL_AREA_HEURISTIC_2, randomGenerator_, 10u, 25u, 0xFFFFFFFFu, 2u, 1u, 1u, worker());
	synthesisPixel.createInpaintingResult(frame_, worker());

	mask_.setValue(0xFFu);

	updateBitmap();
	Refresh();

	return true;
}

void Surface::onMouseLeftDown(wxMouseEvent& event)
{
	BitmapWindow::onMouseLeftDown(event);

	switch (interactionMode_)
	{
		case IM_MARK:
		{
			unsigned int factor = 2u;

			if (event.AltDown())
			{
				factor = 1u;
			}
			else if (event.ControlDown())
			{
				factor = 4u;
			}

			if (updateMask(event.GetPosition().x, event.GetPosition().y, factor, 0x00))
			{
				previousMask_.release();
				updateBitmap();

				Update();
				Refresh();
			}

			break;
		}

		case IM_LINE:
		{
			switch (lineMode_)
			{
				case LM_NONE:
				{
					Scalar xBitmap, yBitmap;

					if (window2bitmap(event.GetPosition().x, event.GetPosition().y, xBitmap, yBitmap))
					{
						lineMode_ = LM_FIRST;
						lineStartPoint_ = Vector2(xBitmap, yBitmap);
					}

					break;
				}

				case LM_FIRST:
				{
					Scalar xBitmap, yBitmap;

					if (window2bitmap(event.GetPosition().x, event.GetPosition().y, xBitmap, yBitmap))
					{
						lineMode_ = LM_NONE;
						lines_.push_back(Line(lineStartPoint_, Vector2(xBitmap, yBitmap)));
					}

					break;
				}
			}

			Update();
			Refresh();

			break;
		}

		case IM_NONE:
			break;
	}
}

void Surface::onMouseRightDown(wxMouseEvent& event)
{
	BitmapWindow::onMouseRightDown(event);

	switch (interactionMode_)
	{
		case IM_MARK:
		{
			unsigned int factor = 2u;

			if (event.AltDown())
			{
				factor = 1u;
			}
			else if (event.ControlDown())
			{
				factor = 4u;
			}

			if (updateMask(event.GetPosition().x, event.GetPosition().y, factor, 0xFF))
			{
				updateBitmap();

				Update();
				Refresh();
			}
			break;
		}

		case IM_LINE:
		{
			lines_.clear();
			lineMode_ = LM_NONE;

			break;
		}

		case IM_NONE:
			break;
	}
}

void Surface::onMouseRightDblClick(wxMouseEvent& event)
{
	BitmapWindow::onMouseRightDblClick(event);

	switch (interactionMode_)
	{
		case IM_MARK:
		{
			mask_.setValue(0xFFu);

			updateBitmap();
			Update();
			Refresh();
			break;
		}

		case IM_NONE:
		case IM_LINE:
			break;
	}
}

void Surface::onMouseMove(wxMouseEvent& event)
{
	if (event.LeftIsDown() || event.RightIsDown())
	{
		switch (interactionMode_)
		{
			case IM_MARK:
			{
				unsigned int factor = 2u;

				if (event.AltDown())
				{
					factor = 1u;
				}
				else if (event.ControlDown())
				{
					factor = 4u;
				}

				if (updateMask(event.GetPosition().x, event.GetPosition().y, factor, event.LeftIsDown() ? 0x00 : 0xFF))
				{
					updateBitmap();

					Update();
					Refresh();
				}

				break;
			}

			case IM_NONE:
			case IM_LINE:
				break;
		}
	}

	BitmapWindow::onMouseMove(event);
}

void Surface::onPaintOverlay(wxPaintEvent& /*event*/, wxPaintDC& dc)
{
	for (Lines::const_iterator i = lines_.begin(); i != lines_.end(); ++i)
	{
		Scalar xWindow0, yWindow0, xWindow1, yWindow1;

		if (bitmap2virtualWindow(i->first.x(), i->first.y(), xWindow0, yWindow0) && bitmap2virtualWindow(i->second.x(), i->second.y(), xWindow1, yWindow1))
		{
			dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00), 3, wxPENSTYLE_SOLID));
			dc.DrawLine(int(xWindow0), int(yWindow0), int(xWindow1), int(yWindow1));

			dc.SetPen(wxPen(wxColour(0xFF, 0xFF, 0xFF), 1, wxPENSTYLE_SOLID));
			dc.DrawLine(int(xWindow0), int(yWindow0), int(xWindow1), int(yWindow1));

			dc.DrawCircle(int(xWindow0), int(yWindow0), 4);
			dc.DrawCircle(int(xWindow1), int(yWindow1), 4);
		}
	}


	if (lineMode_ == LM_FIRST)
	{
		Scalar xWindow, yWindow;
		if (bitmap2virtualWindow(lineStartPoint_.x(), lineStartPoint_.y(), xWindow, yWindow))
		{
			dc.DrawCircle(int(xWindow), int(yWindow), 4);
		}
	}
}

void Surface::updateBitmap()
{
	const WorkerPool::ScopedWorker worker(WorkerPool::get().scopedWorker());

	if (worker)
	{
		worker()->executeFunction(Worker::Function::create(*this, &Surface::updateBitmapSubset, 0u, 0u), 0u, mask_.height(), 0u, 1u, 20u);
	}
	else
	{
		updateBitmapSubset(0u, mask_.height());
	}

	bitmap_ = wxBitmap(image_);
}

void Surface::updateBitmapSubset(const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(firstRow + numberRows <= frame_.height());

	ocean_assert(image_.GetWidth() == int(frame_.width()));
	ocean_assert(image_.GetHeight() == int(frame_.height()));

	ocean_assert(frame_.width() == mask_.width());
	ocean_assert(frame_.height() == mask_.height());

	ocean_assert(frame_.isPixelFormatCompatible(FrameType::FORMAT_RGB24));

	unsigned char* image = image_.GetData() + firstRow * frame_.planeWidthBytes(0u);
	static_assert(std::is_same<unsigned char, uint8_t>::value, "Invalid data type!");

	if (frame_.isContinuous())
	{
		memcpy(image, frame_.constrow<void>(firstRow), numberRows * frame_.planeWidthBytes(0u));
	}
	else
	{
		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			memcpy(image + y * frame_.planeWidthBytes(0u), frame_.constrow<void>(y), frame_.planeWidthBytes(0u));
		}
	}

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* maskRow = mask_.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < mask_.width(); ++x)
		{
			if (maskRow[x] == 0x00)
			{
				*(image + 0) = 0xFF;
				*(image + 1) >>= 1;
				*(image + 2) >>= 1;
			}

			image += 3;
		}
	}
}

bool Surface::updateMask(const int xWindow, const int yWindow, const unsigned int factor, const unsigned char value)
{
	Scalar xBitmap, yBitmap;
	if (window2bitmap(xWindow, yWindow, xBitmap, yBitmap))
	{
		const CV::PixelPosition position((unsigned int)(xBitmap + Scalar(0.5)), (unsigned int)(yBitmap + Scalar(0.5)));

		if (position.x() < mask_.width() && position.y() < mask_.height())
		{
			const unsigned int radius = max(5u, min(mask_.width(), mask_.height()) / 30u) * factor;
			const unsigned int radiusOdd = radius + 1u - radius % 2u;

			CV::Canvas::ellipse(mask_, position, radiusOdd, radiusOdd, &value);

			if (GetParent())
			{
				const unsigned int maskPixels = CV::Segmentation::MaskAnalyzer::countMaskPixels(mask_.constdata<uint8_t>(), mask_.width(), mask_.height(), mask_.paddingElements());

				std::string maskString = String::toAString(maskPixels);
				const unsigned int length = ((unsigned int)maskString.length() - 1u) / 3;

				for (unsigned int n = length - 1u; n != (unsigned int)-1; --n)
				{
					maskString.insert(maskString.length() - (n + 1u) * 3u, 1, '.');
				}

				MainWindow* mainWindow = dynamic_cast<MainWindow*>(GetParent());
				mainWindow->SetStatusText(std::wstring(L"Mask pixels: ") + String::toWString(maskString) + std::wstring(L", ") + String::toWString(maskPixels * 100u / (mask_.width() * mask_.height())) + std::wstring(L"%"), 1);
			}

			return true;
		}
	}

	return false;
}
