/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/BullseyesDebugElements.h"

#include "ocean/cv/detector/bullseyes/Bullseye.h"
#include "ocean/cv/detector/bullseyes/BullseyeDetectorMono.h"
#include "ocean/cv/detector/bullseyes/Utilities.h"

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

void BullseyesDebugElements::setCameraFrames(const Frame& leftFrame, const Frame& rightFrame)
{
	// Only store frames if any element is active
	if (!isElementActive(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE) &&
	    !isElementActive(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD) &&
	    !isElementActive(EI_PIXEL_VALIDATION) &&
	    !isElementActive(EI_RADIAL_CONSISTENCY_PHASE1) &&
	    !isElementActive(EI_RADIAL_CONSISTENCY_PHASE2) &&
	    !isElementActive(EI_RADIAL_CONSISTENCY_PHASE3))
	{
		return;
	}

	ocean_assert(leftFrame.isValid() && leftFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(rightFrame.isValid() && rightFrame.pixelFormat() == FrameType::FORMAT_Y8);

	leftCameraFrame_ = Frame(leftFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	rightCameraFrame_ = Frame(rightFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	// Convert frames to RGB and initialize debug elements for each active element ID
	const std::array<std::pair<const Frame*, std::string>, 2> frameHierarchies =
	{{
		{&leftCameraFrame_, hierarchyNameLeftFrame()},
		{&rightCameraFrame_, hierarchyNameRightFrame()}
	}};

	for (const auto& [cameraFrame, hierarchyName] : frameHierarchies)
	{
		Frame rgbFrame;
		if (!CV::FrameConverter::Comfort::convert(*cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "Failed to convert camera frame to RGB!");
			continue;
		}

		if (isElementActive(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE, std::move(rgbFrameCopy), {hierarchyName});
		}

		if (isElementActive(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD, std::move(rgbFrameCopy), {hierarchyName});
		}

		if (isElementActive(EI_PIXEL_VALIDATION))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_PIXEL_VALIDATION, std::move(rgbFrameCopy), {hierarchyName});
		}

		if (isElementActive(EI_RADIAL_CONSISTENCY_PHASE1))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_RADIAL_CONSISTENCY_PHASE1, std::move(rgbFrameCopy), {hierarchyName});
		}

		if (isElementActive(EI_RADIAL_CONSISTENCY_PHASE2))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_RADIAL_CONSISTENCY_PHASE2, std::move(rgbFrameCopy), {hierarchyName});
		}

		if (isElementActive(EI_RADIAL_CONSISTENCY_PHASE3))
		{
			Frame rgbFrameCopy(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			updateElement(EI_RADIAL_CONSISTENCY_PHASE3, std::move(rgbFrameCopy), {hierarchyName});
		}
	}
}

Frame BullseyesDebugElements::getCameraFrame(const bool left) const
{
	return left ? leftCameraFrame_ : rightCameraFrame_;
}

void BullseyesDebugElements::drawBullseyeCandidateInRow(const unsigned int yRow, const unsigned int segmentStart, const unsigned int segment1Size, const unsigned int segment2Size, const unsigned int segment3Size, const unsigned int segment4Size, const unsigned int segment5Size, const Scalar scale)
{
	if (!isElementActive(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE))
	{
		return;
	}

	// Retrieve existing frame for current hierarchy if it already exists
	Frame rgbFrame = elementForCurrentHierarchy(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE);

	if (!rgbFrame.isValid())
	{
		// Determine which camera frame to use based on current hierarchy
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();

		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	// Scale coordinates from pyramid layer space to original frame space
	const unsigned int scaledRow = (unsigned int)(Scalar(yRow) * scale + Scalar(0.5));
	const unsigned int scaledSegmentStart = (unsigned int)(Scalar(segmentStart) * scale + Scalar(0.5));
	const unsigned int scaledSegment1Size = (unsigned int)(Scalar(segment1Size) * scale + Scalar(0.5));
	const unsigned int scaledSegment2Size = (unsigned int)(Scalar(segment2Size) * scale + Scalar(0.5));
	const unsigned int scaledSegment3Size = (unsigned int)(Scalar(segment3Size) * scale + Scalar(0.5));
	const unsigned int scaledSegment4Size = (unsigned int)(Scalar(segment4Size) * scale + Scalar(0.5));
	const unsigned int scaledSegment5Size = (unsigned int)(Scalar(segment5Size) * scale + Scalar(0.5));

	const unsigned int centerX = scaledSegmentStart + ((scaledSegment1Size + scaledSegment2Size + scaledSegment3Size + scaledSegment4Size + scaledSegment5Size + 1u) / 2u);

	// Ensure we don't draw outside frame bounds
	if (scaledRow >= rgbFrame.height() || centerX >= rgbFrame.width())
	{
		return;
	}

	const uint8_t* color = CV::Canvas::red(rgbFrame.pixelFormat());
	CV::Canvas::point<5u>(rgbFrame, Vector2(Scalar(centerX) + Scalar(0.5), Scalar(scaledRow) + Scalar(0.5)), color);

	updateElement(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE, std::move(rgbFrame));
}

void BullseyesDebugElements::drawCheckBullseyeInNeighborhood(const unsigned int yCenter, const unsigned int xCenter, const Scalar scale, const unsigned int diameter)
{
	if (!isElementActive(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD))
	{
		return;
	}

	// Retrieve existing frame for current hierarchy if it already exists
	Frame rgbFrame = elementForCurrentHierarchy(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD);

	if (!rgbFrame.isValid())
	{
		// Determine which camera frame to use based on current hierarchy
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();

		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	// Scale coordinates from pyramid layer space to original frame space
	const Scalar scaledX = Scalar(xCenter) * scale;
	const Scalar scaledY = Scalar(yCenter) * scale;
	const Scalar scaledRadius = Scalar(diameter) * scale * Scalar(0.5);

	// Create a temporary bullseye for drawing
	const Bullseye bullseye(Vector2(scaledX, scaledY), scaledRadius, 128u);

	// Draw the bullseye outline (center point + circle)
	Utilities::drawBullseye(rgbFrame, bullseye, CV::Canvas::red(rgbFrame.pixelFormat()));

	updateElement(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD, std::move(rgbFrame));
}

void BullseyesDebugElements::drawPixelValidation(const unsigned int y, const unsigned int x, const bool isInvalid)
{
	if (!isElementActive(EI_PIXEL_VALIDATION))
	{
		return;
	}

	// Retrieve existing frame for current hierarchy if it already exists
	Frame rgbFrame = elementForCurrentHierarchy(EI_PIXEL_VALIDATION);

	if (!rgbFrame.isValid())
	{
		// Determine which camera frame to use based on current hierarchy
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();

		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	// Ensure we don't draw outside frame bounds
	if (y >= rgbFrame.height() || x >= rgbFrame.width())
	{
		return;
	}

	// Draw point in green if valid, red if invalid
	const uint8_t* color = isInvalid ? CV::Canvas::red(rgbFrame.pixelFormat()) : CV::Canvas::green(rgbFrame.pixelFormat());
	CV::Canvas::point<1u>(rgbFrame, Vector2(Scalar(x) + Scalar(0.5), Scalar(y) + Scalar(0.5)), color);

	updateElement(EI_PIXEL_VALIDATION, std::move(rgbFrame));
}

void BullseyesDebugElements::drawRadialConsistencyPhase1(const unsigned int yCenter, const unsigned int xCenter, const Scalar scale, const Diameters& diameters, const bool passed)
{
	if (!isElementActive(EI_RADIAL_CONSISTENCY_PHASE1))
	{
		return;
	}

	Frame rgbFrame = elementForCurrentHierarchy(EI_RADIAL_CONSISTENCY_PHASE1);

	if (!rgbFrame.isValid())
	{
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();
		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	const Scalar scaledX = Scalar(xCenter) * scale;
	const Scalar scaledY = Scalar(yCenter) * scale;
	const Vector2 center(scaledX + Scalar(0.5), scaledY + Scalar(0.5));

	const uint8_t* greenColor = CV::Canvas::green(rgbFrame.pixelFormat());
	const uint8_t* redColor = CV::Canvas::red(rgbFrame.pixelFormat());

	// Draw transition points for each diameter
	for (const auto& diameter : diameters)
	{
		// Draw positive half-ray points
		for (const Vector2& transitionPoint : diameter.halfRayPositive.transitionPoints)
		{
			if (transitionPoint != HalfRay::invalidTransitionPoint())
			{
				CV::Canvas::point<3u>(rgbFrame, transitionPoint * scale + Vector2(Scalar(0.5), Scalar(0.5)), greenColor);
			}
		}

		// Draw negative half-ray points
		for (const Vector2& transitionPoint : diameter.halfRayNegative.transitionPoints)
		{
			if (transitionPoint != HalfRay::invalidTransitionPoint())
			{
				CV::Canvas::point<3u>(rgbFrame, transitionPoint * scale + Vector2(Scalar(0.5), Scalar(0.5)), greenColor);
			}
		}
	}

	// Draw center point
	const uint8_t* centerColor = passed ? greenColor : redColor;
	CV::Canvas::point<7u>(rgbFrame, center, centerColor);

	updateElement(EI_RADIAL_CONSISTENCY_PHASE1, std::move(rgbFrame));
}

void BullseyesDebugElements::drawRadialConsistencyPhase2(const unsigned int yCenter, const unsigned int xCenter, const Scalar scale, const Diameters& diameters, const bool passed)
{
	if (!isElementActive(EI_RADIAL_CONSISTENCY_PHASE2))
	{
		return;
	}

	Frame rgbFrame = elementForCurrentHierarchy(EI_RADIAL_CONSISTENCY_PHASE2);

	if (!rgbFrame.isValid())
	{
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();
		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	const Scalar scaledX = Scalar(xCenter) * scale;
	const Scalar scaledY = Scalar(yCenter) * scale;
	const Vector2 center(scaledX + Scalar(0.5), scaledY + Scalar(0.5));

	const uint8_t* greenColor = CV::Canvas::green(rgbFrame.pixelFormat());
	const uint8_t* redColor = CV::Canvas::red(rgbFrame.pixelFormat());
	const uint8_t* grayColor = CV::Canvas::gray(rgbFrame.pixelFormat());

	// Draw symmetry status for each diameter
	// Green = symmetric, Red = asymmetric, Gray = invalid diameter
	for (const auto& diameter : diameters)
	{
		const uint8_t* color = !diameter.areHalfRaysValid() ? grayColor : (diameter.isSymmetryValid ? greenColor : redColor);

		// Draw outer ring points (transitionPoints[2]) for each half-ray with symmetry status color
		if (diameter.halfRayPositive.transitionPoints[2] != HalfRay::invalidTransitionPoint())
		{
			CV::Canvas::point<3u>(rgbFrame, diameter.halfRayPositive.transitionPoints[2] * scale + Vector2(Scalar(0.5), Scalar(0.5)), color);
		}

		if (diameter.halfRayNegative.transitionPoints[2] != HalfRay::invalidTransitionPoint())
		{
			CV::Canvas::point<3u>(rgbFrame, diameter.halfRayNegative.transitionPoints[2] * scale + Vector2(Scalar(0.5), Scalar(0.5)), color);
		}
	}

	// Draw center point
	const uint8_t* centerColor = passed ? greenColor : redColor;
	CV::Canvas::point<7u>(rgbFrame, center, centerColor);

	updateElement(EI_RADIAL_CONSISTENCY_PHASE2, std::move(rgbFrame));
}

void BullseyesDebugElements::drawRadialConsistencyPhase3(const unsigned int yCenter, const unsigned int xCenter, const Scalar scale, const Diameters& diameters, const bool passed)
{
	if (!isElementActive(EI_RADIAL_CONSISTENCY_PHASE3))
	{
		return;
	}

	Frame rgbFrame = elementForCurrentHierarchy(EI_RADIAL_CONSISTENCY_PHASE3);

	if (!rgbFrame.isValid())
	{
		const bool isLeft = !hierarchy_.empty() && hierarchy_.back() == hierarchyNameLeftFrame();
		const Frame& cameraFrame = isLeft ? leftCameraFrame_ : rightCameraFrame_;

		if (!cameraFrame.isValid())
		{
			ocean_assert(false && "Camera frame not set - call setCameraFrames before detection!");
			return;
		}

		if (!CV::FrameConverter::Comfort::convert(cameraFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	const Scalar scaledX = Scalar(xCenter) * scale;
	const Scalar scaledY = Scalar(yCenter) * scale;
	const Vector2 center(scaledX + Scalar(0.5), scaledY + Scalar(0.5));

	const uint8_t* greenColor = CV::Canvas::green(rgbFrame.pixelFormat());
	const uint8_t* redColor = CV::Canvas::red(rgbFrame.pixelFormat());

	// Draw intensity check points for each diameter
	// Green = intensity correct, Red = intensity incorrect
	// Invalid half-rays are skipped
	for (const auto& diameter : diameters)
	{
		// Draw midpoints and outside points for both half-rays
		for (const HalfRay* ray : {&diameter.halfRayPositive, &diameter.halfRayNegative})
		{
			const uint8_t* color0 = ray->isIntensityValid[0] ? greenColor : redColor;
			CV::Canvas::point<3u>(rgbFrame, ray->intensityCheckPoints[0] * scale + Vector2(Scalar(0.5), Scalar(0.5)), color0);

			const uint8_t* color1 = ray->isIntensityValid[1] ? greenColor : redColor;
			CV::Canvas::point<3u>(rgbFrame, ray->intensityCheckPoints[1] * scale + Vector2(Scalar(0.5), Scalar(0.5)), color1);

			const uint8_t* color2 = ray->isIntensityValid[2] ? greenColor : redColor;
			CV::Canvas::point<3u>(rgbFrame, ray->intensityCheckPoints[2] * scale + Vector2(Scalar(0.5), Scalar(0.5)), color2);
		}
	}

	// Draw center point
	const uint8_t* centerColor = passed ? greenColor : redColor;
	CV::Canvas::point<7u>(rgbFrame, center, centerColor);

	updateElement(EI_RADIAL_CONSISTENCY_PHASE3, std::move(rgbFrame));
}

}

}

}

}
