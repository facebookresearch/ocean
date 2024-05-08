/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/pointmotion/win/PointMotionMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/Motion.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/math/Numeric.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

PointMotionMainWindow::PointMotionMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file)
{
	// nothing to do here
}

PointMotionMainWindow::~PointMotionMainWindow()
{
	// nothing to do here
}

void PointMotionMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::IMAGE_SEQUENCE);

		if (frameMedium_)
		{
			const Media::ImageSequenceRef imageSequence(frameMedium_);
			ocean_assert(imageSequence);

			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
			imageSequence->setLoop(false);
		}

		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
		}
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);
	if (finiteMedium)
	{
		finiteMedium->setLoop(true);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}
}

void PointMotionMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && frame->isValid() && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void PointMotionMainWindow::onMouseUp(const MouseButton button, const int x, const int y)
{
	if (button == BUTTON_RIGHT)
	{
		previousPositions_.clear();
		accuratePreviousPositions_.clear();
	}

	if (button == BUTTON_LEFT)
	{
		int bitmapX, bitmapY;
		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapY >= 0)
		{
			ocean_assert(previousPositions_.empty());

			previousPositions_.push_back(CV::PixelPosition((unsigned int)bitmapX, (unsigned int)bitmapY));
			accuratePreviousPositions_.push_back(Vector2(Scalar(bitmapX), Scalar(bitmapY)));

			initialFramePyramid_.clear();

			initialPositions_ = previousPositions_;
			accurateInitialPositions_ = accuratePreviousPositions_;
		}
	}
}

void PointMotionMainWindow::onFrame(const Frame& frame)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	setFrame(rgbFrame);

	constexpr unsigned int patchSize = 5u;

	constexpr unsigned int maximalOffset = 40u;
	constexpr unsigned int coarsestLayerRadius = 2u;

	const unsigned int maximalLayers = CV::FramePyramid::idealLayers(rgbFrame.width(), rgbFrame.height(), patchSize / 2u, patchSize / 2u, 2u, maximalOffset, coarsestLayerRadius);

	if (maximalLayers == 0u)
	{
		ocean_assert(false && "Image too small!");
		return;
	}

	if (!currentFramePyramid_.replace(rgbFrame, CV::FramePyramid::DM_FILTER_14641, maximalLayers, true /*copyFirstLayer*/, &worker_))
	{
		return;
	}

	if (!previousPositions_.empty())
	{
		static unsigned int frameNumber = 0u;
		const static unsigned int frameSwap = 10;

		if (initialFramePyramid_)
		{
			CV::PixelPositions currentPositions;
			if (CV::Motion<>::trackPointsInPyramidMirroredBorder<patchSize>(initialFramePyramid_, currentFramePyramid_, initialPositions_, previousPositions_, currentPositions, coarsestLayerRadius, coarsestLayerRadius, &worker_))
			{
				for (CV::PixelPositions::const_iterator i = currentPositions.begin(); i != currentPositions.end(); ++i)
				{
					const int x = int(i->x() + Scalar(0.5));
					const int y = int(i->y() + Scalar(0.5));

					Rectangle(bitmap().dc(), x - 4, y - 4, x + 4, y + 4);
				}

				previousPositions_ = currentPositions;
			}

			Vectors2 accurateCurrentPositions;
			if (CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<patchSize>(initialFramePyramid_, currentFramePyramid_, accurateInitialPositions_, accuratePreviousPositions_, accurateCurrentPositions, coarsestLayerRadius, 4u, &worker_))
			{
				for (Vectors2::const_iterator i = accurateCurrentPositions.begin(); i != accurateCurrentPositions.end(); ++i)
				{
					const int x = int(i->x() + Scalar(0.5));
					const int y = int(i->y() + Scalar(0.5));

					Ellipse(bitmap().dc(), x - 3, y - 3, x + 3, y + 3);
				}

				accuratePreviousPositions_ = accurateCurrentPositions;
			}
		}

		if (frameNumber % frameSwap == 0u || !initialFramePyramid_)
		{
			std::swap(initialFramePyramid_, currentFramePyramid_);

			initialPositions_ = previousPositions_;
			accurateInitialPositions_ = accuratePreviousPositions_;
		}

		++frameNumber;
		ocean_assert(initialFramePyramid_);
	}

	const Media::ImageSequenceRef imageSequence(frameMedium_);
	if (imageSequence)
	{
		imageSequence->forceNextFrame();
	}

	repaint();
}
