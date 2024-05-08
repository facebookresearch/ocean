/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/win/Detector2DMainWindow.h"

#include "ocean/platform/win/Utilities.h"

Detector2DMainWindow::Detector2DMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	detector2dWrapper_(commandArguments)
{
	initialize();
	start();
}

Detector2DMainWindow::~Detector2DMainWindow()
{
	detector2dWrapper_.release();
}

void Detector2DMainWindow::onIdle()
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;
	std::vector<std::string> messages;

	const bool foundQRCodes = detector2dWrapper_.detectAndDecode(resultingDetectorFrame, resultingDetectorPerformance, messages);

	if (resultingDetectorFrame.isValid())
	{
		setFrame(resultingDetectorFrame);

		if (resultingDetectorPerformance >= 0.0)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingDetectorPerformance * 1000.0, 2u) + std::string("ms"));
		}

		if (foundQRCodes)
		{
			for (unsigned int i = 0u; i < (unsigned int)messages.size(); ++i)
			{
				const std::string& message = messages[i];
				Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 20 * int(i + 1u) + 5, String::toAString(i + 1u) + std::string(": ") + message);
			}
		}
		else
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, "Point the camera at a QR code");
		}


		repaint(false);
	}
}
