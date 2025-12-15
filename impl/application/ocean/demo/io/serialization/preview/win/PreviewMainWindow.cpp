/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/serialization/preview/win/PreviewMainWindow.h"

#include "ocean/base/Messenger.h"

#include "ocean/cv/FrameTransposer.h"

#include "ocean/io/File.h"

#include "ocean/platform/win/Keyboard.h"

#include <commdlg.h>

PreviewMainWindow::PreviewMainWindow(HINSTANCE instance, const std::wstring& name, const std::wstring& filename) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	filename_(filename)
{
	// nothing to do here
}

PreviewMainWindow::~PreviewMainWindow()
{
	stopSerializer();
}

void PreviewMainWindow::onInitialized()
{
	setEnableDropAndDrop(true);

	if (filename_.empty())
	{
		filename_ = openFileDialog();
	}

	if (!filename_.empty())
	{
		loadFile(filename_);
	}
}

void PreviewMainWindow::onIdle()
{
	if (serializer_ && serializer_->isStarted())
	{
		IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();
		IO::Serialization::UniqueDataSample sample = serializer_->sample(channelId, 1.0);

		if (sample)
		{
			IO::Serialization::MediaSerializer::DataSampleFrame* frameSample = dynamic_cast<IO::Serialization::MediaSerializer::DataSampleFrame*>(sample.get());

			if (frameSample != nullptr)
			{
				Frame frame = frameSample->frame();

				if (frame.isValid())
				{
					if (rotationAngle_ != 0)
					{
						CV::FrameTransposer::Comfort::rotate(frame, rotationAngle_);
					}

					onFrame(frame);
					return;
				}
			}
		}
	}

	Sleep(1);
}

void PreviewMainWindow::onKeyDown(const int key)
{
	std::string keyString;
	if (Platform::Win::Keyboard::translateVirtualkey((unsigned int)(key), keyString))
	{
		if (keyString == "R")
		{
			rotationAngle_ = (rotationAngle_ + 90) % 360;

			Log::info() << "Rotation angle: " << rotationAngle_ << " degrees";
		}
		else if (keyString == "L")
		{
			rotationAngle_ = (rotationAngle_ - 90 + 360) % 360;

			Log::info() << "Rotation angle: " << rotationAngle_ << " degrees";
		}
	}
}

void PreviewMainWindow::onDragAndDrop(const Files& files)
{
	if (files.empty())
	{
		return;
	}

	const std::wstring& filename = files.front();

	if (IO::File(String::toAString(filename)).extension() != "osn")
	{
		Log::warning() << "Dropped file is not an .osn file";
		return;
	}

	stopSerializer();

	loadFile(filename);
}

bool PreviewMainWindow::loadFile(const std::wstring& filename)
{
	const std::string filenameString = String::toAString(filename);
	const IO::File file(filenameString);

	if (!file.exists())
	{
		Log::error() << "The input file does not exist: '" << file() << "'";
		return false;
	}

	Log::info() << "Opening serialization file: '" << file() << "'";

	// Create a fresh serializer instance
	serializer_ = std::make_unique<IO::Serialization::FileInputDataSerializer>();

	if (!serializer_->setFilename(file()))
	{
		Log::error() << "Failed to set the filename";
		return false;
	}

	if (!serializer_->registerSample<IO::Serialization::MediaSerializer::DataSampleFrame>())
	{
		Log::error() << "Failed to register factory function";
		return false;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!serializer_->initialize(&channels))
	{
		Log::error() << "Failed to initialize the serializer";
		return false;
	}

	Log::info() << "Found " << channels.size() << " channel(s)";

	for (size_t n = 0; n < channels.size(); ++n)
	{
		const IO::Serialization::DataSerializer::Channel& channel = channels[n];
		Log::info() << "Channel #" << (n + 1) << ": " << channel.name() << " (" << channel.sampleType() << ")";
	}

	if (!serializer_->start())
	{
		Log::error() << "Failed to start the serializer";
		return false;
	}

	filename_ = filename;

	return true;
}

void PreviewMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	repaint(false);
}

void PreviewMainWindow::stopSerializer()
{
	if (serializer_ && serializer_->isStarted())
	{
		serializer_->stop();

		serializer_->stopAndWait();
	}
}

std::wstring PreviewMainWindow::openFileDialog()
{
	wchar_t filename[MAX_PATH] = L"";

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = handle();
	ofn.lpstrFilter = L"Serialization Files (*.osn)\0*.osn\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = L"Open Serialization File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (GetOpenFileNameW(&ofn))
	{
		return std::wstring(filename);
	}

	return std::wstring();
}
