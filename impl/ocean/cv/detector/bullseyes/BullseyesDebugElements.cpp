/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/BullseyesDebugElements.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

void BullseyesDebugElements::drawBullseyeCandidateInRow(const Frame& yFrame, const unsigned int yRow, const unsigned int segmentStart, const unsigned int segment1Size, const unsigned int segment2Size, const unsigned int segment3Size, const unsigned int segment4Size, const unsigned int segment5Size)
{
	if (!isElementActive(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE))
	{
		return;
	}

	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yRow < yFrame.height());

	// Retrieve existing frame if it already exists, otherwise create new RGB image from the Y frame.
	Frame rgbFrame = element(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE);

	if (!rgbFrame.isValid())
	{
		if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	const uint8_t* foregroundColor = CV::Canvas::green(rgbFrame.pixelFormat());
	const uint8_t* backgroundColor = CV::Canvas::red(rgbFrame.pixelFormat());

	const std::array<unsigned int, 5> segmentSizes = {segment1Size, segment2Size, segment3Size, segment4Size, segment5Size};
	uint8_t* pixelData = rgbFrame.pixel<uint8_t>(segmentStart, yRow);

	unsigned int totalSize = 0u;
	for (size_t segmentIndex = 0; segmentIndex < segmentSizes.size(); ++segmentIndex)
	{
		totalSize += segmentSizes[segmentIndex];
	}

	// Make sure we don't write outside the frame bounds
	if (segmentStart + totalSize > rgbFrame.width())
	{
		return;
	}

	for (size_t segmentIndex = 0; segmentIndex < segmentSizes.size(); ++segmentIndex)
	{
		// Black segments (0, 2, 4) use foreground color (green), white segments (1, 3) use background color (red)
		const uint8_t* color = (segmentIndex % 2 == 0) ? foregroundColor : backgroundColor;
		const unsigned int segmentSize = segmentSizes[segmentIndex];

		for (unsigned int i = 0u; i < segmentSize; ++i)
		{
			pixelData[0] = color[0];
			pixelData[1] = color[1];
			pixelData[2] = color[2];

			pixelData += 3;
		}
	}

	updateElement(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE, std::move(rgbFrame));
}

}

}

}

}
