/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/frameinterpolatortrilinear/win/FITMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorTrilinear.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

FITMainWindow::FITMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	bilinearInterpolationWindow_(instance, L"Full Border Bi-linear Interpolation", FITBitmapWindow::Callback::create(*this, &FITMainWindow::onInterpolateBilinearNativeFrame)),
	trilinearInterpolationWindow_(instance, L"Full Border Tri-linear Interpolation", FITBitmapWindow::Callback::create(*this, &FITMainWindow::onInterpolateTrilinearNativeFrame)),
	mediaFile_(file)
{
	// nothing to do here
}

FITMainWindow::~FITMainWindow()
{
	// nothing to do here
}

void FITMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}

	bilinearInterpolationWindow_.setParent(handle());
	bilinearInterpolationWindow_.initialize();
	bilinearInterpolationWindow_.show();

	trilinearInterpolationWindow_.setParent(handle());
	trilinearInterpolationWindow_.initialize();
	trilinearInterpolationWindow_.show();
}

void FITMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && frame->timestamp() != frameTimestamp_)
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void FITMainWindow::onFrame(const Frame& frame)
{
	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	setFrame(rgbFrame);

	bilinearInterpolationWindow_.setFrame(rgbFrame);
	trilinearInterpolationWindow_.setFrame(rgbFrame);

	repaint();
	bilinearInterpolationWindow_.repaint();
	trilinearInterpolationWindow_.repaint();
}

void FITMainWindow::onInterpolateBilinearNativeFrame(const Frame& source, Frame& target)
{
	if (!source.isValid() || !target.isValid() || source.pixelFormat() != FrameType::FORMAT_RGB24)
	{
		return;
	}

	const unsigned int targetWidth = target.width();
	const unsigned int targetHeight = target.height();

	const unsigned int sourceWidth = source.width();
	const unsigned int sourceHeight = source.height();

	for (unsigned int y = 0u; y < targetHeight; ++y)
	{
		const Scalar ySource = Scalar(y * sourceHeight) / Scalar(targetHeight - 1u);

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Scalar xSource = Scalar(x * sourceWidth) / Scalar(targetWidth - 1u);

			CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_CENTER>(source.constdata<uint8_t>(), sourceWidth, sourceHeight, source.paddingElements(), Vector2(xSource, ySource), target.pixel<uint8_t>(x, y));
		}
	}
}

void FITMainWindow::onInterpolateTrilinearNativeFrame(const Frame& source, Frame& target)
{
	if (!source.isValid() || !target.isValid() || source.pixelFormat() != FrameType::FORMAT_RGB24)
	{
		return;
	}

	CV::FrameInterpolatorTrilinear::resize(source, target, WorkerPool::get().scopedWorker()());
}
