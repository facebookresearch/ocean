/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/imageanalyzer/win/ImageAnalyzerMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/io/image/Image.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

ImageAnalyzerMainWindow::ImageAnalyzerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::string>& filenames) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	intensityGraphHorizontal_(instance, L"Horizontal pixel intensities"),
	intensityGraphVertical_(instance, L"Vertical pixel intensities")
{
	inputFiles_.reserve(filenames.size());

	for (const std::string& filename : filenames)
	{
		IO::File file(filename);

		if (file.exists())
		{
			inputFiles_.emplace_back(std::move(file));
		}
	}
}

ImageAnalyzerMainWindow::~ImageAnalyzerMainWindow()
{
	// nothing to do here
}

void ImageAnalyzerMainWindow::onInitialized()
{
	intensityGraphHorizontal_.setParent(handle());
	intensityGraphHorizontal_.initialize();
	intensityGraphHorizontal_.setDisplayMode(BitmapWindow::DM_STRETCH_TO_SCREEN);
	intensityGraphHorizontal_.show();

	intensityGraphVertical_.setParent(handle());
	intensityGraphVertical_.initialize();
	intensityGraphVertical_.setDisplayMode(BitmapWindow::DM_STRETCH_TO_SCREEN);
	intensityGraphVertical_.show();

	setEnableDropAndDrop(true);

	if (currentInputIndex_ < inputFiles_.size())
	{
		updateImage(inputFiles_[currentInputIndex_]());
	}
}

void ImageAnalyzerMainWindow::onPaint()
{
	BitmapWindow::onPaint();

	if (currentInputIndex_ < inputFiles_.size())
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, inputFiles_[currentInputIndex_].name());
	}
	else
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "No image");
	}

	if (NumericF::isNotEqual(linearGain_, 1.0f))
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 25, "Gain: " + String::toAString(linearGain_, 2u));
	}
}

void ImageAnalyzerMainWindow::onMouseMove(const MouseButton /*buttons*/, const int x, const int y)
{
	int bitmapX, bitmapY;
	if (!window2bitmap(x, y, bitmapX, bitmapY) || (unsigned int)(bitmapX) >= yImage_.width() || (unsigned int)(bitmapY) >= yImage_.height())
	{
		return;
	}

	analysisLocationX_ = (unsigned int)(bitmapX);
	analysisLocationY_ = (unsigned int)(bitmapY);

	updateAnalysis();
}

void ImageAnalyzerMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (!inputFiles_.empty())
		{
			const size_t previousInputIndex = currentInputIndex_;

			if (keyString == "left")
			{
				if (currentInputIndex_ == 0)
				{
					currentInputIndex_ = inputFiles_.size() - 1;
				}
				else
				{
					--currentInputIndex_;
				}
			}
			else if (keyString == "right")
			{
				currentInputIndex_ = (currentInputIndex_ + 1) % inputFiles_.size();
			}

			if (previousInputIndex != currentInputIndex_)
			{
				ocean_assert(currentInputIndex_ < inputFiles_.size());
				updateImage(inputFiles_[currentInputIndex_]());
			}
		}

		const float previousLinearGain = linearGain_;

		if (keyString == "up")
		{
			linearGain_ *= 1.025f;
		}
		else if (keyString == "down")
		{
			if (linearGain_ > 0.05f)
			{
				linearGain_ /= 1.025f;
			}
		}
		else if (keyString == "R")
		{
			linearGain_ = 1.0f;
		}
		else if (keyString == "Z")
		{
			zoomed_ = !zoomed_;
		}
		else if (keyString == "B")
		{
			blackWhiteVisualization_ = !blackWhiteVisualization_;
		}

		if (linearGain_ != previousLinearGain)
		{
			applyGain(yImage_, linearGain_, yAdjustedImage_);

			setFrame(yAdjustedImage_);
			repaint(false);
		}
	}

	updateAnalysis();
}

void ImageAnalyzerMainWindow::onDragAndDrop(const Files& files)
{
	if (!files.empty())
	{
		inputFiles_.clear();
		inputFiles_.reserve(files.size());

		for (const std::wstring& filename : files)
		{
			IO::File file(String::toAString(filename));

			if (file.exists())
			{
				inputFiles_.emplace_back(std::move(file));
			}
		}

		currentInputIndex_ = 0;

		if (currentInputIndex_ < inputFiles_.size())
		{
			updateImage(inputFiles_[currentInputIndex_]());
		}
	}
}

void ImageAnalyzerMainWindow::updateImage(const std::string& filename)
{
	ocean_assert(!filename.empty());

	const Frame image = IO::Image::readImage(filename);

	if (image.isValid() && CV::FrameConverter::Comfort::convert(image, FrameType::FORMAT_Y8, yImage_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		yAdjustedImage_.copy(yImage_);

		applyGain(yImage_, linearGain_, yAdjustedImage_);

		setFrame(yAdjustedImage_);
		repaint();

		if (analysisLocationX_ >= yAdjustedImage_.width() || analysisLocationY_ >= yAdjustedImage_.height())
		{
			analysisLocationX_ = yAdjustedImage_.width() / 2u;
			analysisLocationY_ = yAdjustedImage_.height() / 2u;
		}

		updateAnalysis();
	}
	else
	{
		yImage_.release();
		yAdjustedImage_.release();

		setFrame(yAdjustedImage_);
		repaint();

		intensityGraphHorizontal_.setFrame(Frame());
		intensityGraphVertical_.setFrame(Frame());
	}
}

void ImageAnalyzerMainWindow::updateAnalysis()
{
	if (analysisLocationX_ >= yImage_.width() || analysisLocationY_ >= yImage_.height())
	{
		return;
	}

	{
		// horizontal intensity graph

		const unsigned int xStart = zoomed_ ? (unsigned int)(std::max(0, int(analysisLocationX_) - 20)) : 0u;
		const unsigned int xEnd = zoomed_ ? std::min((unsigned int)(analysisLocationX_) + 21u, yAdjustedImage_.width()) : yAdjustedImage_.width();

		const unsigned int range = xEnd - xStart;

		Frame frameIntensityGraphHorizontal;

		if (blackWhiteVisualization_)
		{
			frameIntensityGraphHorizontal.set(FrameType(256u, range, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
			frameIntensityGraphHorizontal.setValue(0xFFu);

			for (unsigned int x = xStart; x < xEnd; ++x)
			{
				const unsigned int intensity = (unsigned int)(yAdjustedImage_.constpixel<uint8_t>(x, analysisLocationY_)[0]);

				const int value = x == analysisLocationX_ ? 0x80 : 0x00;
				memset(frameIntensityGraphHorizontal.row<uint8_t>(x - xStart) + 255u - intensity, value, intensity + 1u);
			}
		}
		else
		{
			frameIntensityGraphHorizontal.set(FrameType(256u, range, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
			frameIntensityGraphHorizontal.setValue(CV::Canvas::red(), 3u);

			for (unsigned int x = xStart; x < xEnd; ++x)
			{
				const unsigned int intensity = (unsigned int)(yAdjustedImage_.constpixel<uint8_t>(x, analysisLocationY_)[0]);

				const Frame::PixelType<uint8_t, 3u> value = (x == analysisLocationX_) ? Frame::PixelType<uint8_t, 3u>({0x00, 0x00, 0xFF}) : Frame::PixelType<uint8_t, 3u>({uint8_t(intensity), uint8_t(intensity), uint8_t(intensity)});
				frameIntensityGraphHorizontal.subFrame(255u - intensity, x - xStart, intensity + 1u, 1u).setValue<uint8_t, 3u>(value);
			}
		}

		CV::FrameTransposer::transpose(frameIntensityGraphHorizontal, WorkerPool::get().scopedWorker()());
		intensityGraphHorizontal_.setFrame(frameIntensityGraphHorizontal);
		intensityGraphHorizontal_.repaint();
	}

	{
		// vertical intensity graph

		const unsigned int yStart = zoomed_ ? (unsigned int)(std::max(0, int(analysisLocationY_) - 20)) : 0u;
		const unsigned int yEnd = zoomed_ ? std::min((unsigned int)(analysisLocationY_) + 21u, yAdjustedImage_.height()) : yAdjustedImage_.height();

		const unsigned int range = yEnd - yStart;

		Frame frameIntensityGraphVertical;

		if (blackWhiteVisualization_)
		{
			frameIntensityGraphVertical.set(FrameType(256u, range, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
			frameIntensityGraphVertical.setValue(0xFFu);

			for (unsigned int y = yStart; y < yEnd; ++y)
			{
				const unsigned int intensity = (unsigned int)(yAdjustedImage_.constpixel<uint8_t>(analysisLocationX_, y)[0]);

				const int value = y == analysisLocationY_ ? 0x80 : 0x00;
				memset(frameIntensityGraphVertical.row<uint8_t>(y - yStart), value, intensity + 1u);
			}
		}
		else
		{
			frameIntensityGraphVertical.set(FrameType(256u, range, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
			frameIntensityGraphVertical.setValue(CV::Canvas::red(), 3u);

			for (unsigned int y = yStart; y < yEnd; ++y)
			{
				const unsigned int intensity = (unsigned int)(yAdjustedImage_.constpixel<uint8_t>(analysisLocationX_, y)[0]);

				const Frame::PixelType<uint8_t, 3u> value = (y == analysisLocationY_) ? Frame::PixelType<uint8_t, 3u>({0x00, 0x00, 0xFF}) : Frame::PixelType<uint8_t, 3u>({uint8_t(intensity), uint8_t(intensity), uint8_t(intensity)});
				frameIntensityGraphVertical.subFrame(0u, y - yStart, intensity + 1u, 1u).setValue<uint8_t, 3u>(value);
			}
		}

		intensityGraphVertical_.setFrame(frameIntensityGraphVertical);
		intensityGraphVertical_.repaint();
	}
}

void ImageAnalyzerMainWindow::applyGain(const Frame& yImage, const float gain, Frame& yAdjustedImage)
{
	ocean_assert(yImage.isValid());
	ocean_assert(gain > 0.0f);

	if (NumericF::isEqual(gain, 1.0f))
	{
		yAdjustedImage = Frame(yImage, Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
		yAdjustedImage.set(yImage.frameType(), true, true);

		ocean_assert(yAdjustedImage.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

		for (unsigned int y = 0u; y < yAdjustedImage.height(); ++y)
		{
			uint8_t* const targetRow = yAdjustedImage.row<uint8_t>(y);
			const uint8_t* const sourceRow = yImage.constrow<uint8_t>(y);

			for (unsigned int n = 0u; n < yImage.planeWidthBytes(0u); ++n)
			{
				targetRow[n] = uint8_t(std::min((unsigned int)(float(sourceRow[n]) * gain + 0.5f), 255u));
			}
		}
	}
}
