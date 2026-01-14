/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/BullseyesDebugElements.h"

#include "ocean/cv/detector/bullseyes/Bullseye.h"
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
	if (!isElementActive(EI_DETECT_BULLSEYE_IN_ROW_VALID_SEQUENCE) && !isElementActive(EI_CHECK_BULLSEYE_IN_NEIGHBORHOOD) && !isElementActive(EI_PIXEL_VALIDATION))
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
	Utilities::drawBullseyeOutline(rgbFrame, bullseye, CV::Canvas::red(rgbFrame.pixelFormat()));

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

}

}

}

}
