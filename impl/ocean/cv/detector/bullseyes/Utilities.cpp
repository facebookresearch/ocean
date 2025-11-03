/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

bool Utilities::createBullseyeImage(const unsigned int diameter, const unsigned int emptyBorder, Frame& rgbFrame, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	if (diameter < 15u || ((diameter & 1u) == 0u))
	{
		return false;
	}

	const unsigned int imageSize = diameter + 2u * emptyBorder;

	foregroundColor = foregroundColor != nullptr ? foregroundColor : CV::Canvas::black(FrameType::FORMAT_RGB24);
	backgroundColor = backgroundColor != nullptr ? backgroundColor : CV::Canvas::white(FrameType::FORMAT_RGB24);

	rgbFrame = Frame(FrameType(imageSize, imageSize, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(rgbFrame.isValid());

	rgbFrame.setValue(backgroundColor, 3u);

	return drawBullseyeWithOffset(rgbFrame, PixelPosition(0, 0), diameter, emptyBorder, foregroundColor, backgroundColor);
}

bool Utilities::drawBullseyeWithOffset(Frame& rgbFrame, const PixelPosition& offset, const unsigned int diameter, const unsigned int emptyBorder, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	if (!rgbFrame.isValid() || !FrameType::arePixelFormatsCompatible(rgbFrame.pixelFormat(), FrameType::FORMAT_RGB24))
	{
		return false;
	}

	if (diameter < 15u || ((diameter & 1u) == 0u))
	{
		return false;
	}

	const unsigned int bullseyeSize = diameter + 2u * emptyBorder;

	if (offset.x() + bullseyeSize > rgbFrame.width() || offset.y() + bullseyeSize > rgbFrame.height())
	{
		// The bullseye won't fit into the frame
		return false;
	}

	foregroundColor = foregroundColor != nullptr ? foregroundColor : CV::Canvas::black(FrameType::FORMAT_RGB24);
	backgroundColor = backgroundColor != nullptr ? backgroundColor : CV::Canvas::white(FrameType::FORMAT_RGB24);

	Frame subFrame = rgbFrame.subFrame(offset.x(), offset.y(), bullseyeSize, bullseyeSize);
	ocean_assert(subFrame.isValid());

	const CV::PixelPosition center(bullseyeSize / 2u, bullseyeSize / 2u);

	// All diameters must be odd values for CV::Canvas::ellipse()
	const unsigned int centerDiscDiameter = (diameter / 5u) | 1u;
	const unsigned int innerDiscDiameter = ((3u * diameter) / 5u) | 1u;
	const unsigned int outerDiscDiameter = diameter;
	const unsigned int emptyBorderDiameter = (diameter + emptyBorder) | 1u;

	CV::Canvas::ellipse(subFrame, center, emptyBorderDiameter, emptyBorderDiameter, backgroundColor);
	CV::Canvas::ellipse(subFrame, center, outerDiscDiameter, outerDiscDiameter, foregroundColor);
	CV::Canvas::ellipse(subFrame, center, innerDiscDiameter, innerDiscDiameter, backgroundColor);
	CV::Canvas::ellipse(subFrame, center, centerDiscDiameter, centerDiscDiameter, foregroundColor);

	return true;
}

void Utilities::drawBullseye(Frame& rgbFrame, const Bullseye& bullseye, const uint8_t* color)
{
	ocean_assert(rgbFrame.isValid() && FrameType::arePixelFormatsCompatible(rgbFrame.pixelFormat(), FrameType::FORMAT_RGB24));
	ocean_assert(bullseye.isValid());

	const Vector2& center = bullseye.position();
	const Scalar radius = bullseye.radius();

	CV::Canvas::line<3u>(rgbFrame, center.x() - radius, center.y(), center.x() + radius, center.y(), color);
	CV::Canvas::line<3u>(rgbFrame, center.x(), center.y() - radius, center.x(), center.y() + radius, color);
}

void Utilities::drawBullseyes(Frame& rgbFrame, const Bullseye* bullseyes, const size_t numberBullseyes, const uint8_t* color)
{
	ocean_assert(rgbFrame.isValid() && FrameType::arePixelFormatsCompatible(rgbFrame.pixelFormat(), FrameType::FORMAT_RGB24));
	ocean_assert(bullseyes != nullptr || numberBullseyes == 0);

	for (size_t n = 0; n < numberBullseyes; ++n)
	{
		drawBullseye(rgbFrame, bullseyes[n], color);
	}
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
