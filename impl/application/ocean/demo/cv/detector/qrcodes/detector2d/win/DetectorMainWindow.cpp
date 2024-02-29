// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/qrcodes/detector/win/DetectorMainWindow.h"

#include "ocean/platform/win/Utilities.h"

DetectorMainWindow::DetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	detectorWrapper_(commandArguments)
{
	initialize();
	start();
}

DetectorMainWindow::~DetectorMainWindow()
{
	detectorWrapper_.release();
}

void DetectorMainWindow::onIdle()
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;
	std::vector<std::string> messages;

	const bool foundQRCodes = detectorWrapper_.detectAndDecode(resultingDetectorFrame, resultingDetectorPerformance, messages);

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
