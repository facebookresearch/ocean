/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/lookup/win/LookupMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"

#include "ocean/platform/win/Utilities.h"

LookupMainWindow::LookupMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	windowLookupCenter_(800u, 800u, 4u, 4u),
	windowLookupCorner_(800u, 800u, 4u, 4u),
	windowViewId_(0u)
{
	// nothing to do here
}

LookupMainWindow::~LookupMainWindow()
{
	// nothing to do here
}

void LookupMainWindow::onInitialized()
{
	RandomI::initialize();

	toggleView();
}

void LookupMainWindow::onKeyDown(const int /*key*/)
{
	toggleView();
}

void LookupMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	toggleView();
}

void LookupMainWindow::toggleView()
{
	Frame frame(FrameType((unsigned int)(windowLookupCenter_.sizeX()), (unsigned int)(windowLookupCenter_.sizeY()), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

	HighPerformanceTimer timer;

	switch (windowViewId_++ % 6u)
	{
		case 0u:
		{
			for (unsigned int y = 0u; y < windowLookupCenter_.binsY(); ++y)
			{
				for (unsigned int x = 0u; x < windowLookupCenter_.binsX(); ++x)
				{
					windowLookupCenter_.setBinCenterValue(x, y, Vector3(Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1), Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1), Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1)));
				}
			}


			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCenter_.nearestValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 1u] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 2u] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));
				}
			}

			break;
		}

		case 1u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCenter_.bilinearValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 1u] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 2u] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));
				}
			}

			break;
		}

		case 2u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCenter_.bicubicValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(minmax<Scalar>(0, color.x() * Scalar(255) + Scalar(0.5), 255));
					row[x * 3u + 1u] = uint8_t(minmax<Scalar>(0, color.y() * Scalar(255) + Scalar(0.5), 255));
					row[x * 3u + 2u] = uint8_t(minmax<Scalar>(0, color.z() * Scalar(255) + Scalar(0.5), 255));
				}
			}

			break;
		}

		case 3u:
		{
			for (unsigned int y = 0u; y <= windowLookupCenter_.binsY(); ++y)
			{
				for (unsigned int x = 0u; x <= windowLookupCenter_.binsX(); ++x)
				{
					windowLookupCorner_.setBinTopLeftCornerValue(x, y, Vector3(Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1), Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1), Scalar(RandomI::random(0, 1000) % 2 == 0 ? 0 : 1)));
				}
			}


			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCorner_.nearestValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 1u] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 2u] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));
				}
			}

			break;
		}

		case 4u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCorner_.bilinearValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(color.x() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 1u] = uint8_t(color.y() * Scalar(255) + Scalar(0.5));
					row[x * 3u + 2u] = uint8_t(color.z() * Scalar(255) + Scalar(0.5));
				}
			}

			break;
		}

		case 5u:
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				uint8_t* const row = frame.row<uint8_t>(y);

				for (unsigned int x = 0u; x < frame.width(); ++x)
				{
					const Vector3 color(windowLookupCorner_.bicubicValue(Scalar(x), Scalar(y)));

					row[x * 3u + 0u] = uint8_t(minmax<Scalar>(0, color.x() * Scalar(255) + Scalar(0.5), 255));
					row[x * 3u + 1u] = uint8_t(minmax<Scalar>(0, color.y() * Scalar(255) + Scalar(0.5), 255));
					row[x * 3u + 2u] = uint8_t(minmax<Scalar>(0, color.z() * Scalar(255) + Scalar(0.5), 255));
				}
			}

			break;
		}
	}

	setFrame(frame);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(timer.mseconds()) + std::string("ms"));

	repaint();
}
