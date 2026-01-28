/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/detector/bullseyes/BullseyeDetectorMono.h"

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

	color = color != nullptr ? color : CV::Canvas::green(rgbFrame.pixelFormat());

	const Vector2& center = bullseye.position();

	// Draw center point
	CV::Canvas::point<3u>(rgbFrame, center + Vector2(0.5, 0.5), color);

	const Diameters& diameters = bullseye.diameters();

	if (!diameters.empty())
	{
		// Collect all half-rays for drawing (positive first, then negative for each diameter)
		std::vector<const HalfRay*> halfRays;
		halfRays.reserve(diameters.size() * 2);

		for (const Diameter& diameter : diameters)
		{
			halfRays.push_back(&diameter.halfRayPositive);
		}
		for (const Diameter& diameter : diameters)
		{
			halfRays.push_back(&diameter.halfRayNegative);
		}

		for (size_t i = 0; i < halfRays.size(); ++i)
		{
			const HalfRay* currentRay = halfRays[i];
			const HalfRay* nextRay = halfRays[(i + 1) % halfRays.size()];

			// Draw the circumference line (outermost transition point)
			const Vector2& currentOuter = currentRay->transitionPoints[2];
			const Vector2& nextOuter = nextRay->transitionPoints[2];

			if (currentOuter != HalfRay::invalidTransitionPoint() && nextOuter != HalfRay::invalidTransitionPoint())
			{
				CV::Canvas::line<1u>(rgbFrame, currentOuter + Vector2(0.5, 0.5), nextOuter + Vector2(0.5, 0.5), color);
			}

			// Draw the transition points themselves
			for (const Vector2& point : currentRay->transitionPoints)
			{
				if (point != HalfRay::invalidTransitionPoint())
				{
					CV::Canvas::point<3u>(rgbFrame, point + Vector2(0.5, 0.5), color);
				}
			}
		}
	}
	else
	{
		// No diameter data - draw circle outline
		constexpr unsigned int numSegments = 72u;
		constexpr Scalar angleStep = Numeric::pi2() / Scalar(numSegments);

		const Scalar radius = bullseye.radius();

		for (unsigned int i = 0u; i < numSegments; ++i)
		{
			const Scalar angle0 = Scalar(i) * angleStep;
			const Scalar angle1 = Scalar(i + 1u) * angleStep;

			const Scalar x0 = center.x() + radius * Numeric::cos(angle0);
			const Scalar y0 = center.y() + radius * Numeric::sin(angle0);
			const Scalar x1 = center.x() + radius * Numeric::cos(angle1);
			const Scalar y1 = center.y() + radius * Numeric::sin(angle1);

			CV::Canvas::line<1u>(rgbFrame, x0, y0, x1, y1, color);
		}
	}
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
