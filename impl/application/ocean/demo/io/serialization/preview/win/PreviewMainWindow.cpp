/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/serialization/preview/win/PreviewMainWindow.h"

#include "ocean/base/Messenger.h"

#include "ocean/io/File.h"

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
	if (filename_.empty())
	{
		filename_ = openFileDialog();
	}

	if (filename_.empty())
	{
		return;
	}

	const std::string filename = String::toAString(filename_);
	const IO::File file(filename);

	if (!file.exists())
	{
		Log::error() << "The input file does not exist: '" << file() << "'";
		return;
	}

	Log::info() << "Opening serialization file: '" << file() << "'";

	if (!serializer_.setFilename(file()))
	{
		Log::error() << "Failed to set the filename";
		return;
	}

	if (!serializer_.registerSample<IO::Serialization::MediaSerializer::DataSampleFrame>())
	{
		Log::error() << "Failed to register factory function";
		return;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!serializer_.initialize(&channels))
	{
		Log::error() << "Failed to initialize the serializer";
		return;
	}

	Log::info() << "Found " << channels.size() << " channel(s)";

	for (size_t n = 0; n < channels.size(); ++n)
	{
		const IO::Serialization::DataSerializer::Channel& channel = channels[n];
		Log::info() << "Channel #" << (n + 1) << ": " << channel.name() << " (" << channel.sampleType() << ")";
	}

	if (!serializer_.start())
	{
		Log::error() << "Failed to start the serializer";
		return;
	}
}

void PreviewMainWindow::onIdle()
{
	if (serializer_.isStarted())
	{
		IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();
		IO::Serialization::UniqueDataSample sample = serializer_.sample(channelId, 1.0);

		if (sample)
		{
			IO::Serialization::MediaSerializer::DataSampleFrame* frameSample = dynamic_cast<IO::Serialization::MediaSerializer::DataSampleFrame*>(sample.get());

			if (frameSample != nullptr)
			{
				Frame frame = frameSample->frame();

				if (frame.isValid())
				{
					onFrame(frame);
					return;
				}
			}
		}
	}

	Sleep(1);
}

void PreviewMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	repaint(false);
}

void PreviewMainWindow::stopSerializer()
{
	if (serializer_.isStarted())
	{
		serializer_.stop();

		serializer_.stopAndWait();
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
