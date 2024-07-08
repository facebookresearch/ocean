/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/contourfinder/win/ContourFinderMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/segmentation/ContourFinder.h"
#include "ocean/cv/segmentation/MaskCreator.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

ContourFinderMainWindow::ContourFinderMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	contourWindow_(instance, L"Resulting contour"),
	maskWindow_(instance, L"Resulting mask"),
	mediaFile(media)
{
	// nothing to do here
}

ContourFinderMainWindow::~ContourFinderMainWindow()
{
	// nothing to do here
}

void ContourFinderMainWindow::onInitialized()
{
	if (!mediaFile.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile, Media::Medium::IMAGE_SEQUENCE);
		if (frameMedium_)
		{
			const Media::ImageSequenceRef imageSequence(frameMedium_);
			ocean_assert(imageSequence);

			// in the case we have an image sequence, we set the explicit mode allowing to decide when we receive a new image
			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}

		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFile, Media::Medium::FRAME_MEDIUM);
		}
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
	{
		finiteMedium->setSpeed(1);
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}

	contourWindow_.setParent(handle());
	contourWindow_.initialize();
	contourWindow_.show();

	maskWindow_.setParent(handle());
	maskWindow_.initialize();
	maskWindow_.show();
}

void ContourFinderMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && frame->isValid())
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ContourFinderMainWindow::onMouseDown(const MouseButton button, const int /*x*/, const int /*y*/)
{
	gatherRoughContour_ = true;
	pixelPositions_.clear();

	if (button == BUTTON_RIGHT)
	{
		const Media::ImageSequenceRef imageSequence(frameMedium_);
		if (imageSequence)
		{
			imageSequence->forceNextFrame();
		}
	}
}

void ContourFinderMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	if (pixelPositions_.size() > 2)
	{
		const CV::PixelPosition first = pixelPositions_.front();
		CV::PixelPosition last = pixelPositions_.back();

		int x = int(last.x());
		int y = int(last.y());

		CV::Bresenham bresenham(x, y, int(first.x()), int(first.y()));

		while (x != int(first.x()) || y != int(first.y()))
		{
			ocean_assert(x >= 0 && y >= 0);

			const CV::PixelPosition testPosition(x, y);

			if (last.sqrDistance(testPosition) >= 15u * 15u)
			{
				pixelPositions_.emplace_back(testPosition);
				last = testPosition;
			}

			bresenham.findNext(x, y);
		}
	}

	gatherRoughContour_ = false;
	createFineContour_ = true;
}

void ContourFinderMainWindow::onMouseMove(const MouseButton /*buttons*/, const int x, const int y)
{
	if (gatherRoughContour_)
	{
		int bitmapX, bitmapY;

		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			pixelPositions_.emplace_back((unsigned int)(bitmapX), (unsigned int)(bitmapY));
		}
	}
}

void ContourFinderMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	setFrame(topLeft);

	if (createFineContour_)
	{
		createFineContour_ = false;

		for (unsigned int n = 1; n <= pixelPositions_.size(); /* noop */)
		{
			const unsigned int nModulo = modulo((int)n, (int)pixelPositions_.size());

			if (pixelPositions_[n - 1].sqrDistance(pixelPositions_[nModulo]) < 1u * 1u)
			{
				pixelPositions_.erase(pixelPositions_.begin() + nModulo);
			}
			else
			{
				++n;
			}
		}

		if (pixelPositions_.size() >= 3)
		{
			MoveToEx(contourWindow_.bitmap().dc(), int(pixelPositions_.back().x()), int(pixelPositions_.back().y()), nullptr);
			for (CV::PixelPositions::const_iterator i = pixelPositions_.begin(); i != pixelPositions_.end(); ++i)
			{
				const int x = int(i->x());
				const int y = int(i->y());

				LineTo(contourWindow_.bitmap().dc(), x, y);
				Ellipse(contourWindow_.bitmap().dc(), x - 4, y - 4, x + 4, y + 4);
			}
			contourWindow_.repaint();


			CV::Segmentation::PixelContour roughContour(pixelPositions_);
			roughContour.simplify();

			HighPerformanceTimer timer;
			const CV::Segmentation::PixelContour fineContour(CV::Segmentation::ContourFinder::similarityContour(topLeft, 21u, roughContour, 10u, randomGenerator_, &worker_));
			const double time = timer.mseconds();

			Platform::Win::Utilities::textOutput(contourWindow_.bitmap().dc(), 5, 5, String::toAString(time) + std::string("ms"));

			const double contourTime = timer.mseconds();
			contourWindow_.setFrame(topLeft);

			Platform::Win::Utilities::textOutput(contourWindow_.bitmap().dc(), 5, 5, String::toAString(contourTime) + std::string("ms"));

			const CV::PixelPositions& finePositions = fineContour.pixels();
			ocean_assert(finePositions.size() >= 3);

			if (finePositions.size() >= 3)
			{
				MoveToEx(contourWindow_.bitmap().dc(), int(pixelPositions_.back().x()), int(pixelPositions_.back().y()), nullptr);
				for (const CV::PixelPosition& pixelPosition : pixelPositions_)
				{
					LineTo(contourWindow_.bitmap().dc(), int(pixelPosition.x()), int(pixelPosition.y()));
				}

				for (const CV::PixelPosition& pixelPosition : pixelPositions_)
				{
					const int x = int(pixelPosition.x());
					const int y = int(pixelPosition.y());

					Ellipse(contourWindow_.bitmap().dc(), x - 2, y - 2, x + 2, y + 2);
				}

				MoveToEx(contourWindow_.bitmap().dc(), int(finePositions.back().x()), int(finePositions.back().y()), nullptr);
				for (const CV::PixelPosition& finePosition : finePositions)
				{
					LineTo(contourWindow_.bitmap().dc(), int(finePosition.x()), int(finePosition.y()));
				}

				for (const CV::PixelPosition& finePosition : finePositions)
				{
					const int x = int(finePosition.x());
					const int y = int(finePosition.y());

					Ellipse(contourWindow_.bitmap().dc(), x - 1, y - 1, x + 1, y + 1);
				}

				contourWindow_.repaint();

				maskWindow_.setFrame(topLeft);

				Frame mask(FrameType(topLeft, FrameType::FORMAT_Y8));
				mask.setValue(0xFF);

				timer.start();
				CV::Segmentation::MaskCreator::contour2inclusiveMaskByTriangulation(mask.data<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), fineContour.simplified(), 0x00, &worker_);
				const double maskTime = timer.mseconds();

				for (unsigned int y = 0; y < mask.height(); ++y)
				{
					const uint8_t* maskRow = mask.constrow<uint8_t>(y);

					for (unsigned int x = 0; x < mask.width(); ++x)
					{
						if (maskRow[x] == 0x00)
						{
							SetPixel(maskWindow_.bitmap().dc(), x, y, 0xFF);
						}
					}
				}

				Platform::Win::Utilities::textOutput(maskWindow_.bitmap().dc(), 5, 5, String::toAString(maskTime) + std::string("ms"));

				maskWindow_.repaint();

				const Media::ImageSequenceRef imageSequence(frameMedium_);
				if (imageSequence)
				{
					imageSequence->forceNextFrame();
				}
			}
		}
	}
	else if (gatherRoughContour_ && !pixelPositions_.empty())
	{
		MoveToEx(bitmap().dc(), int(pixelPositions_.back().x()), int(pixelPositions_.back().y()), nullptr);

		for (const CV::PixelPosition& pixelPosition : pixelPositions_)
		{
			const int x = int(pixelPosition.x());
			const int y = int(pixelPosition.y());

			LineTo(bitmap().dc(), x, y);

			Ellipse(bitmap().dc(), x - 2, y - 2, x + 2, y + 2);
		}
	}

	if (contourWindow_.bitmap().width() == 0 || contourWindow_.bitmap().height() == 0)
	{
		contourWindow_.setFrame(topLeft);
		contourWindow_.adjustToBitmapSize();
		contourWindow_.repaint();
	}

	if (maskWindow_.bitmap().width() == 0 || maskWindow_.bitmap().height() == 0)
	{
		maskWindow_.setFrame(topLeft);
		maskWindow_.adjustToBitmapSize();
		maskWindow_.repaint();
	}

	repaint();

	Sleep(1);
}
