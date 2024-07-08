/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/barcodes/detector2d/win/MainWindow.h"
#include "application/ocean/demo/cv/detector/barcodes/detector2d/win/Resource.h"

#include "ocean/platform/win/Utilities.h"

MainWindow::MainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	wrapper_(commandArguments)
{
	initialize(instance, IDI_ICON);

	start();
}

MainWindow::~MainWindow()
{
	wrapper_.release();
}

void MainWindow::onIdle()
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;
	std::vector<std::string> messages;

	const bool allSucceeded = wrapper_.detectAndDecode(resultingDetectorFrame, resultingDetectorPerformance, messages);

	if (resultingDetectorFrame.isValid())
	{
		setFrame(resultingDetectorFrame);

		if (resultingDetectorPerformance >= 0.0)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingDetectorPerformance * 1000.0, 2u) + std::string("ms"));
		}

		if (allSucceeded && !messages.empty())
		{
			for (size_t i = 0; i < messages.size(); ++i)
			{
				const std::string& message = messages[i];
				Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 20 * int(i + 1u) + 5, String::toAString(i + 1u) + std::string(": ") + message);
			}
		}
		else
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, "Point the camera at a barcode");
		}


		repaint(false);
	}
}
