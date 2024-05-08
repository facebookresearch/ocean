/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

void Utilities::paintLines(Frame& frame, const FiniteLines2& finiteLines, const unsigned char* foregroundColor, const unsigned char* backgroundColor)
{
	ocean_assert(frame.isValid());
	ocean_assert(foregroundColor != nullptr);

	for (const FiniteLine2& finiteLine : finiteLines)
	{
		if (backgroundColor)
		{
			CV::Canvas::line<3u>(frame, finiteLine, backgroundColor);
		}

		CV::Canvas::line<1u>(frame, finiteLine, foregroundColor);
	}
}

void Utilities::paintLShapes(Frame& frame, const FiniteLines2& finiteLines, const ShapeDetector::LShapes& lShapes, const unsigned char* foregroundColor, const unsigned char* backgroundColor)
{
	ocean_assert(frame.isValid());
	ocean_assert(foregroundColor != nullptr);

	for (const ShapeDetector::LShape& lShape : lShapes)
	{
		const FiniteLine2& finiteLine0 = finiteLines[lShape.finiteLineIndex0()];
		const FiniteLine2& finiteLine1 = finiteLines[lShape.finiteLineIndex1()];

		if (backgroundColor)
		{
			CV::Canvas::line<3u>(frame, finiteLine0, backgroundColor);
			CV::Canvas::line<3u>(frame, finiteLine1, backgroundColor);
			CV::Canvas::point<5u>(frame, lShape.position(), backgroundColor);
		}

		CV::Canvas::line<1u>(frame, finiteLine0, foregroundColor);
		CV::Canvas::line<1u>(frame, finiteLine1, foregroundColor);
		CV::Canvas::point<3u>(frame, lShape.position(), foregroundColor);
	}
}

void Utilities::paintLShapes(Frame& frame, const ShapeDetector::LShapes& lShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor, const Vector2& offset)
{
	ocean_assert(frame.isValid());
	ocean_assert(length > Numeric::eps());
	ocean_assert(foregroundColor != nullptr);

	for (const ShapeDetector::LShape& lShape : lShapes)
	{
		const Vector2 pointLeft = lShape.position() + offset + lShape.edgeLeft() * length;
		const Vector2 pointRight = lShape.position() + offset + lShape.edgeRight() * length;

		if (backgroundColor)
		{
			CV::Canvas::line<3u>(frame, pointLeft, lShape.position() + offset, backgroundColor);
			CV::Canvas::line<3u>(frame, pointRight, lShape.position() + offset, backgroundColor);
			CV::Canvas::point<5u>(frame, lShape.position() + offset, backgroundColor);
		}

		CV::Canvas::line<1u>(frame, pointLeft, lShape.position() + offset, foregroundColor);
		CV::Canvas::line<1u>(frame, pointRight, lShape.position() + offset, foregroundColor);
		CV::Canvas::point<3u>(frame, lShape.position() + offset, foregroundColor);
	}
}

void Utilities::paintTShapes(Frame& frame, const ShapeDetector::TShapes& tShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor, const Vector2& offset)
{
	ocean_assert(frame.isValid());
	ocean_assert(length > Numeric::eps());
	ocean_assert(foregroundColor != nullptr);

	for (const ShapeDetector::TShape& tShape : tShapes)
	{
		const Vector2 point = tShape.position() + offset + tShape.direction() * length;

		if (backgroundColor)
		{
			CV::Canvas::line<3u>(frame, tShape.position() + offset, point, backgroundColor);
			CV::Canvas::point<5u>(frame, tShape.position() + offset, backgroundColor);
		}

		CV::Canvas::line<1u>(frame, tShape.position() + offset, point, foregroundColor);
		CV::Canvas::point<3u>(frame, tShape.position() + offset, foregroundColor);
	}
}

void Utilities::paintXShapes(Frame& frame, const ShapeDetector::XShapes& xShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor, const Vector2& offset)
{
	ocean_assert(frame.isValid());
	ocean_assert(length > Numeric::eps());
	ocean_assert(foregroundColor != nullptr);

	for (const ShapeDetector::XShape& xShape : xShapes)
	{
		const Vector2 pointA = xShape.position() + offset - xShape.direction0() * length;
		const Vector2 pointB = xShape.position() + offset + xShape.direction0() * length;

		const Vector2 pointC = xShape.position() + offset- xShape.direction1() * length;
		const Vector2 pointD = xShape.position() + offset + xShape.direction1() * length;

		if (backgroundColor)
		{
			CV::Canvas::line<3u>(frame, pointA, pointB, backgroundColor);
			CV::Canvas::line<3u>(frame, pointC, pointD, backgroundColor);
			CV::Canvas::point<5u>(frame, xShape.position() + offset, backgroundColor);
		}

		CV::Canvas::line<1u>(frame, pointA, pointB, foregroundColor);
		CV::Canvas::line<1u>(frame, pointC, pointD, foregroundColor);
		CV::Canvas::point<3u>(frame, xShape.position() + offset, foregroundColor);
	}
}

void Utilities::paintShapes(Frame& frame, const ShapeDetector::LShapes& lShapes, const ShapeDetector::TShapes& tShapes, const ShapeDetector::XShapes& xShapes, const Scalar length, const bool drawBackground, const Vector2& offset, const uint8_t* foregroundColor)
{
	ocean_assert(frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.channels() <= 4u);
	ocean_assert(length > Numeric::eps());

	const uint8_t* const red = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const blue = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const black= CV::Canvas::black(frame.pixelFormat());

	paintLShapes(frame, lShapes, length, foregroundColor != nullptr ? foregroundColor : blue, drawBackground ? black : nullptr, offset);
	paintTShapes(frame, tShapes, length, foregroundColor != nullptr ? foregroundColor : green, drawBackground ? black : nullptr, offset);
	paintXShapes(frame, xShapes, length, foregroundColor != nullptr ? foregroundColor : red, drawBackground ? black : nullptr, offset);
}

void Utilities::paintShapes(Frame& frame, const ShapeDetector::TwoLineShape* const * shapes, size_t size, const Scalar length, const bool drawBackground, const Vector2& offset, const uint8_t* foregroundColor)
{
	ocean_assert(frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.channels() <= 4u);
	ocean_assert(size == 0 || shapes != nullptr);
	ocean_assert(length > Numeric::eps());

	const uint8_t* const red = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const blue = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const black= CV::Canvas::black(frame.pixelFormat());

	for (size_t n = 0; n < size; ++n)
	{
		const ShapeDetector::TwoLineShape& shape = *shapes[n];

		switch (shape.type())
		{
			case ShapeDetector::TwoLineShape::ST_SHAPE_L:
				paintLShapes(frame, {static_cast<const ShapeDetector::LShape&>(shape)}, length, foregroundColor != nullptr ? foregroundColor : blue, drawBackground ? black : nullptr, offset);
				break;

			case ShapeDetector::TwoLineShape::ST_SHAPE_T:
				paintTShapes(frame, {static_cast<const ShapeDetector::TShape&>(shape)}, length, foregroundColor != nullptr ? foregroundColor : green, drawBackground ? black : nullptr, offset);
				break;

			case ShapeDetector::TwoLineShape::ST_SHAPE_X:
				paintXShapes(frame, {static_cast<const ShapeDetector::XShape&>(shape)}, length, foregroundColor != nullptr ? foregroundColor : red, drawBackground ? black : nullptr, offset);
				break;

			default:
				break;
		}
	}
}

void Utilities::paintRectangles(Frame& frame, const ShapeDetector::LShapes& lShapes, const ShapeDetector::IndexedRectangles& rectangles, const unsigned char* foregroundColor, const unsigned char* backgroundColor)
{
	ocean_assert(frame.isValid());
	ocean_assert(foregroundColor != nullptr);

	for (const ShapeDetector::IndexedRectangle& rectangle : rectangles)
	{
		for (unsigned int n = 0u; n < 4u; ++n)
		{
			const ShapeDetector::LShape& lShape0 = lShapes[rectangle[n]];
			const ShapeDetector::LShape& lShape1 = lShapes[rectangle[(n + 1u) & 0x03u]];

			if (backgroundColor)
			{
				CV::Canvas::line<3u>(frame, lShape0.position().x(), lShape0.position().y(), lShape1.position().x(), lShape1.position().y(), backgroundColor);
			}

			CV::Canvas::line<1u>(frame, lShape0.position().x(), lShape0.position().y(), lShape1.position().x(), lShape1.position().y(), foregroundColor);
		}
	}
}

Frame Utilities::visualizeShapeAlignment(const Frame& frame, unsigned int rectifiedWidth, unsigned int rectifiedHeight, const SquareMatrix3& frame_H_rectified, const std::vector<const ShapeDetector::TwoLineShape*>& shapeReferences, const std::vector<const ShapeDetector::TwoLineShape*>& shapeCandidates, const Vector2& referenceShapesOffset, const Vector2& candidateShapesOffset)
{
	ocean_assert(frame.isValid());
	ocean_assert(rectifiedWidth != 0u && rectifiedHeight != 0u);
	ocean_assert(frame_H_rectified.isHomography());

	Frame rectifiedFrame(FrameType(frame, rectifiedWidth, rectifiedHeight));
	const bool homographyResult = CV::FrameInterpolatorBilinear::Comfort::homography(frame, rectifiedFrame, frame_H_rectified);
	ocean_assert_and_suppress_unused(homographyResult, homographyResult);

	const bool changeResult = CV::FrameConverter::Comfort::change(rectifiedFrame, FrameType::FORMAT_RGB24);
	ocean_assert_and_suppress_unused(changeResult, changeResult);

	if (!candidateShapesOffset.isNull())
	{
		Frame intermediateFrame = Frame(rectifiedFrame.frameType());
		CV::FrameInterpolatorBilinear::Comfort::affine(rectifiedFrame, intermediateFrame, SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(-candidateShapesOffset, 1)), CV::Canvas::white());

		rectifiedFrame = std::move(intermediateFrame);
	}

	CV::Detector::Utilities::paintShapes(rectifiedFrame, shapeReferences.data(), shapeReferences.size(), 5.5, false, referenceShapesOffset);

	for (const ShapeDetector::TwoLineShape* shape : shapeReferences)
	{
		CV::Canvas::point<5u>(rectifiedFrame, shape->position() + referenceShapesOffset, CV::Canvas::black());
		CV::Canvas::point<1u>(rectifiedFrame, shape->position() + referenceShapesOffset, CV::Canvas::white());
	}

	CV::Detector::Utilities::paintShapes(rectifiedFrame, shapeCandidates.data(), shapeCandidates.size(), 5.5, false, candidateShapesOffset);

	return rectifiedFrame;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean
