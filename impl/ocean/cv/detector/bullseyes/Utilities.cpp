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

bool Utilities::createBullseyeImage(const unsigned int diameter, const unsigned int quietZone, Frame& rgbFrame, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	ocean_assert(diameter >= 5u && diameter % 5u == 0u);
	if (diameter < 5u || diameter % 5u != 0u)
	{
		return false;
	}

	const unsigned int imageSize = diameter + 2u * quietZone;

	foregroundColor = foregroundColor != nullptr ? foregroundColor : CV::Canvas::black(FrameType::FORMAT_RGB24);
	backgroundColor = backgroundColor != nullptr ? backgroundColor : CV::Canvas::white(FrameType::FORMAT_RGB24);

	rgbFrame = Frame(FrameType(imageSize, imageSize, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(rgbFrame.isValid());

	rgbFrame.setValue(backgroundColor, 3u);

	const CV::PixelPosition center(imageSize / 2u, imageSize / 2u);

	const unsigned int centerDiscDiameter = diameter / 5u;
	const unsigned int innerDiscDiameter = 3u * centerDiscDiameter;
	const unsigned int outerDiscDiameter = 5u * centerDiscDiameter;

	CV::Canvas::ellipse(rgbFrame, center, outerDiscDiameter, outerDiscDiameter, foregroundColor);
	CV::Canvas::ellipse(rgbFrame, center, innerDiscDiameter, innerDiscDiameter, backgroundColor);
	CV::Canvas::ellipse(rgbFrame, center, centerDiscDiameter, centerDiscDiameter, foregroundColor);

	return true;
}

void Utilities::drawBullseye(Frame& frame, const Bullseye& bullseye, const uint8_t* color)
{
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24));
	ocean_assert(bullseye.isValid());

	const Vector2& center = bullseye.position();
	const Scalar radius = bullseye.radius();

	CV::Canvas::line<3u>(frame, center.x() - radius, center.y(), center.x() + radius, center.y(), color);
	CV::Canvas::line<3u>(frame, center.x(), center.y() - radius, center.x(), center.y() + radius, color);
}

void Utilities::drawBullseyes(Frame& frame, const Bullseye* bullseyes, const size_t numberBullseyes, const uint8_t* color)
{
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24));
	ocean_assert(bullseyes != nullptr || numberBullseyes == 0);

	for (size_t n = 0; n < numberBullseyes; ++n)
	{
		drawBullseye(frame, bullseyes[n], color);
	}
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
