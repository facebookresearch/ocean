/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/picturetaker/win/PictureTakerMainWindow.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"

#include "ocean/io/File.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

#include <fstream>

PictureTakerMainWindow::PictureTakerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& inputSource, unsigned int preferredWidth, unsigned int preferredHeight, FrameType::PixelFormat preferredPixelFormat, const std::string& outputDirectory) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	inputSource_(inputSource),
	preferredWidth_(preferredWidth),
	preferredHeight_(preferredHeight),
	preferredPixelFormat_(preferredPixelFormat)
{
	directory_ = IO::Directory(outputDirectory) + IO::Directory("PictureTaker_" + DateTime::localStringForFile());
}

PictureTakerMainWindow::~PictureTakerMainWindow()
{
	// nothing to do here
}

void PictureTakerMainWindow::onInitialized()
{
	if (!inputSource_.empty())
	{
		liveVideo_ = Media::Manager::get().newMedium(inputSource_, Media::Medium::LIVE_VIDEO);
	}

	if (!liveVideo_)
	{
		Log::warning() << "Failed to open input source: '" << inputSource_ << "'";
		return;
	}

	if (!liveVideo_->setPreferredFramePixelFormat(preferredPixelFormat_))
	{
		Log::warning() << "Failed to set preferred pixel format: " << FrameType::translatePixelFormat(preferredPixelFormat_);
	}

	if (!liveVideo_->setPreferredFrameDimension(preferredWidth_, preferredHeight_))
	{
		Log::warning() << "Failed to set preferred resolution: " << preferredWidth_ << "x" << preferredHeight_;
	}

	if (!liveVideo_->setFocus(currentFocus_))
	{
		Log::warning() << "Failed to set focus: " << currentFocus_;
	}

	if (!liveVideo_->start())
	{
		Log::error() << "Failed to start the live video";
		return;
	}

	Log::info() << "Using input: " << liveVideo_->url();
	Log::info() << "Preferred resolution: " << preferredWidth_ << "x" << preferredHeight_;
	Log::info() << "Preferred pixel format: " << FrameType::translatePixelFormat(preferredPixelFormat_);

	if (!directory_.exists())
	{
		directory_.create();
	}

	Log::info() << "Press any key to take a picture";
	Log::info() << "Images will be saved to: " << directory_();
}

void PictureTakerMainWindow::onIdle()
{
	if (liveVideo_)
	{
		const FrameRef frame(liveVideo_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
		}
	}

	if (countdownActive_)
	{
		const double elapsedSeconds = double(Timestamp(true) - countdownTimestamp_);

		if (elapsedSeconds >= 0.5)
		{
			countdownValue_--;
			countdownTimestamp_.toNow();

			if (countdownValue_ < 0)
			{
				countdownActive_ = false;
				takeImage_ = true;
			}

			repaint(false);
		}
	}

	Thread::sleep(1);
}

void PictureTakerMainWindow::onFrame(const Frame& frame)
{
	if (takeImage_)
	{
		takeImage_ = false;

		if (takePicture(frame))
		{
			Log::info() << "Picture " << pictureCounter_ << " saved successfully";
		}
	}

	setFrame(frame);

	repaint(false);
}

void PictureTakerMainWindow::onPaint()
{
	BitmapWindow::onPaint();

	if (countdownActive_ && countdownValue_ >= 0)
	{
		constexpr int32_t foregroundColor = 0xFFFFFF;
		constexpr int32_t backgroundColor = -1;
		constexpr int32_t shadowColor = 0x000000;

		constexpr unsigned int shadowOffset = 3u;

		Platform::Win::Utilities::textOutput(dc(), String::toWString(countdownValue_), L"Arial", 120u, true, Platform::Win::Utilities::AP_CENTER, clientWidth(), clientHeight(), foregroundColor, backgroundColor, shadowColor, shadowOffset, shadowOffset);
	}

	if (pictureCounter_ > 0u)
	{
		constexpr int32_t foregroundColor = 0xFFFFFF;
		constexpr int32_t backgroundColor = -1;
		constexpr int32_t shadowColor = 0x000000;

		constexpr unsigned int shadowOffset = 2u;
		constexpr unsigned int margin = 20u;

		Platform::Win::Utilities::textOutput(dc(), String::toWString(pictureCounter_), L"Arial", 36u, true, Platform::Win::Utilities::AP_BOTTOM_RIGHT, clientWidth(), clientHeight(), foregroundColor, backgroundColor, shadowColor, shadowOffset, shadowOffset, margin, margin);
	}
}

void PictureTakerMainWindow::onKeyDown(const int /*key*/)
{
	if (!countdownActive_)
	{
		countdownActive_ = true;
		countdownValue_ = 3;
		countdownTimestamp_.toNow();

		Log::info() << "Starting countdown...";

		repaint(false);
	}
}

bool PictureTakerMainWindow::takePicture(const Frame& frame)
{
	if (!directory_.isValid())
	{
		Log::error() << "No valid output directory";
		return false;
	}

	// Write settings file on first image capture
	if (!settingsFileWritten_)
	{
		writeCameraSettings(frame);
	}

	const IO::File filename = directory_ + IO::File("image_" + String::toAString(frame.width()) + "x" + String::toAString(frame.height()) + "_" + String::toAString(pictureCounter_++, 3u) + ".png");

	if (!IO::Image::Comfort::writeImage(frame, filename(), true))
	{
		Log::error() << "Failed to write the picture to '" << filename() << "'";
		return false;
	}

	Log::info() << "Wrote picture to '" << filename() << "'";

	return true;
}

void PictureTakerMainWindow::writeCameraSettings(const Frame& frame)
{
	const IO::File settingsFile = directory_ + IO::File("camera_settings.txt");

	std::ofstream settingsStream(settingsFile());

	if (settingsStream.is_open())
	{
		if (liveVideo_)
		{
			settingsStream << "Camera: " << liveVideo_->url() << std::endl;
		}

		settingsStream << "Resolution: " << frame.width() << "x" << frame.height() << std::endl;
		settingsStream << "Focus: " << currentFocus_ << std::endl;

		Log::info() << "Wrote camera settings to '" << settingsFile() << "'";
		settingsFileWritten_ = true;
	}
	else
	{
		Log::error() << "Failed to write camera settings file";
	}
}
