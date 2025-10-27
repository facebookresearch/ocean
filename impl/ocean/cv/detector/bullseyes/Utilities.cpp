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

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
