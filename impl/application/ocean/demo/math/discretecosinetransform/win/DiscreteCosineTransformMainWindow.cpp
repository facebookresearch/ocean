/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/discretecosinetransform/win/DiscreteCosineTransformMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/Utilities.h"

DiscreteCosineTransformMainWindow::DiscreteCosineTransformMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFile) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	dctWindow_(instance, L"DCT result"),
	windowMediaFile_(mediaFile)
{
	// nothing to do here
}

DiscreteCosineTransformMainWindow::~DiscreteCosineTransformMainWindow()
{
	// nothing to do here
}

void DiscreteCosineTransformMainWindow::onInitialized()
{
	RandomI::initialize();

	dctWindow_.setParent(handle());
	dctWindow_.initialize();

	Frame yFrame;

	if (windowMediaFile_.empty())
	{
		yFrame = CV::CVUtilities::randomizedFrame(FrameType(800u, 800u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	}
	else
	{
		const Frame frame(Media::Utilities::loadImage(windowMediaFile_));

		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, true))
		{
			return;
		}
	}

	setFrame(yFrame);

	Scalars imageData(yFrame.pixels());
	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		const uint8_t* grayFrameRow = yFrame.row<uint8_t>(y);
		Scalar* imageDataRow = imageData.data() + y * yFrame.width();

		for (unsigned int x = 0u; x < yFrame.width(); ++x)
		{
			 imageDataRow[x] = Scalar(grayFrameRow[x]);
		}
	}

	Scalar coefficients[64];

	HighPerformanceTimer timer;

	for (unsigned int y = 0u; y < yFrame.height(); y += 8u)
	{
		for (unsigned int x = 0u; x < yFrame.width(); x += 8u)
		{
			const unsigned int xElements = min(yFrame.width() - x, 8u);
			const unsigned int yElements = min(yFrame.height() - y, 8u);

			ocean_assert(xElements * yElements <= 64u);

			DiscreteCosineTransform::transform2(imageData.data() + y * yFrame.width() + x, xElements, yElements, yFrame.width(), coefficients);

			// remove precision
			for (unsigned int ky = 0u; ky < yElements; ++ky)
			{
				for (unsigned int kx = 0u; kx < xElements; ++kx)
				{
					if (kx >= 3u || ky >= 3u)
					{
						coefficients[ky * xElements + kx] = 0;
					}
				}
			}

			DiscreteCosineTransform::iTransform2(coefficients, xElements, yElements, yFrame.width(), imageData.data() + y * yFrame.width() + x);
		}
	}

	const double time = timer.mseconds();

	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		uint8_t* grayFrameRow = yFrame.row<uint8_t>(y);
		const Scalar* imageDataRow = imageData.data() + y * yFrame.width();

		for (unsigned int x = 0u; x < yFrame.width(); ++x)
		{
			grayFrameRow[x] = uint8_t(minmax<int>(0, int(imageDataRow[x]), 255));
		}
	}

	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, String::toAString(time) + std::string("ms"));

	dctWindow_.setFrame(yFrame);
	dctWindow_.show();
}
