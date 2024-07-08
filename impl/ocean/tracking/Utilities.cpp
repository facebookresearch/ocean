/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/PanoramaFrame.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/Utilities.h"

#include "ocean/media/Utilities.h"

namespace Ocean
{

namespace Tracking
{

Frame Utilities::blendFrames(const Frame& frame0, const Frame& frame1, Worker* worker)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.frameType() == frame1.frameType());

	Frame result(frame1, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	if (!CV::FrameBlender::blend<true>(frame0, result, 0x80, worker))
	{
		return Frame();
	}

	return result;
}

Frame Utilities::blendFrames(const Frame& frame0, const Frame& frame1, Vector2& offset0, Vector2& offset1, const FrameType::PixelFormat pixelFormat, Worker* worker)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.pixelOrigin() == frame1.pixelOrigin());

	Frame internalFrame0(frame0, Frame::ACM_USE_KEEP_LAYOUT);
	Frame internalFrame1(frame1, Frame::ACM_USE_KEEP_LAYOUT);

	if (pixelFormat != FrameType::FORMAT_UNDEFINED)
	{
		if (!CV::FrameConverter::Comfort::change(internalFrame0, pixelFormat) || !CV::FrameConverter::Comfort::change(internalFrame1, pixelFormat))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}
	}

	const unsigned int maxWidth = std::max(internalFrame0.width(), internalFrame1.width());
	const unsigned int maxHeight = std::max(internalFrame0.height(), internalFrame1.height());

	if (maxWidth != internalFrame0.width() || maxHeight != internalFrame0.height())
	{
		Frame intermediateFrame(FrameType(internalFrame0, maxWidth, maxHeight));
		intermediateFrame.setValue(0x00u);

		const int offsetX = int(maxWidth - internalFrame0.width()) / 2;
		const int offsetY = int(maxHeight - internalFrame0.height()) / 2;

		intermediateFrame.copy(offsetX, offsetY, internalFrame0);

		offset0 = Vector2(Scalar(offsetX), Scalar(offsetY));

		internalFrame0 = std::move(intermediateFrame);
	}
	else
	{
		offset0 = Vector2(0, 0);
	}

	if (maxWidth != internalFrame1.width() || maxHeight != internalFrame1.height())
	{
		Frame intermediateFrame(FrameType(internalFrame1, maxWidth, maxHeight));
		intermediateFrame.setValue(0x00u);

		const int offsetX = int(maxWidth - internalFrame1.width()) / 2;
		const int offsetY = int(maxHeight - internalFrame1.height()) / 2;

		intermediateFrame.copy(offsetX, offsetY, internalFrame1);

		offset1 = Vector2(Scalar(offsetX), Scalar(offsetY));

		internalFrame1 = std::move(intermediateFrame);
	}
	else
	{
		offset1 = Vector2(0, 0);
	}

	internalFrame1.makeOwner();

	if (!CV::FrameBlender::blend<true>(internalFrame0, internalFrame1, 0x80, worker))
	{
		return Frame();
	}

	return internalFrame1;
}

void Utilities::paintFeaturePoint(Frame& frame, const Vector2& position, const Scalar radius, const Scalar orientation, const uint8_t* color, const uint8_t* shadowColor)
{
	ocean_assert(frame.isValid());

	if (frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				paintFeaturePoint8BitPerChannel<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), position, radius, orientation, color, shadowColor, frame.paddingElements());
				break;

			case 2u:
				paintFeaturePoint8BitPerChannel<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), position, radius, orientation, color, shadowColor, frame.paddingElements());
				break;

			case 3u:
				paintFeaturePoint8BitPerChannel<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), position, radius, orientation, color, shadowColor, frame.paddingElements());
				break;

			case 4u:
				paintFeaturePoint8BitPerChannel<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), position, radius, orientation, color, shadowColor, frame.paddingElements());
				break;

			default:
				ocean_assert(false && "Invalid channel number!");
		}
	}
}

void Utilities::paintFeaturePoints(Frame& frame, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const size_t size, const uint8_t* color, const uint8_t* shadowColor, const Vector2& explicitOffset, Worker* worker)
{
	ocean_assert(frame.isValid());

	if (size == 0)
	{
		return;
	}

	if (frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				paintFeaturePoints8BitPerChannel<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, radii, orientations, size, color, shadowColor, explicitOffset, frame.paddingElements(), worker);
				break;

			case 2u:
				paintFeaturePoints8BitPerChannel<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, radii, orientations, size, color, shadowColor, explicitOffset, frame.paddingElements(), worker);
				break;

			case 3u:
				paintFeaturePoints8BitPerChannel<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, radii, orientations, size, color, shadowColor, explicitOffset, frame.paddingElements(), worker);
				break;

			case 4u:
				paintFeaturePoints8BitPerChannel<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, radii, orientations, size, color, shadowColor, explicitOffset, frame.paddingElements(), worker);
				break;

			default:
				ocean_assert(false && "Invalid channel number!");
		}
	}
}

Frame Utilities::paintCorrespondencesBlend(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* rgbColor, Worker* worker)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.numberPlanes() == 1u);
	ocean_assert(frame0.pixelOrigin() == frame1.pixelOrigin());

	// the given frames can have an unspecified pixel format (e.g., a generic pixel format with 3 channels, but without specified channel order like RGB or BGR),
	// therefore, we simply cast those pixel formats to one of four pixel formats: Y8, YA16, RGB8, RGBA32

	Frame adjustedFrame0(frame0, Frame::ACM_USE_KEEP_LAYOUT);
	Frame adjustedFrame1(frame1, Frame::ACM_USE_KEEP_LAYOUT);

	if (Frame::formatIsPureGeneric(adjustedFrame0.pixelFormat()))
	{
		adjustedFrame0.setPixelFormat(FrameType::findPixelFormat(adjustedFrame0.dataType(), adjustedFrame0.channels()));
	}

	if (Frame::formatIsPureGeneric(adjustedFrame1.pixelFormat()))
	{
		adjustedFrame1.setPixelFormat(FrameType::findPixelFormat(adjustedFrame1.dataType(), adjustedFrame1.channels()));
	}

	Vector2 offset0;
	Vector2 offset1;
	Frame result = blendFrames(adjustedFrame0, adjustedFrame1, offset0, offset1, FrameType::FORMAT_RGB24, worker);
	if (!result.isValid())
	{
		return Frame();
	}

	paintLines(result, points0, points1, numberPoints, rgbColor, worker, true /*subpixel*/, offset0, offset1);

	return result;
}

Frame Utilities::paintCorrespondencesHorizontal(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* color, Worker* worker)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.numberPlanes() == 1u);
	ocean_assert(frame0.pixelOrigin() == frame1.pixelOrigin());

	// the given frames can have an unspecified pixel format (e.g., a generic pixel format with 3 channels, but without specified channel order like RGB or BGR),
	// therefore, we simply cast those pixel formats to one of four pixel formats: Y8, YA16, RGB8, RGBA32

	Frame adjustedFrame0(frame0, Frame::ACM_USE_KEEP_LAYOUT);
	Frame adjustedFrame1(frame1, Frame::ACM_USE_KEEP_LAYOUT);

	if (Frame::formatIsPureGeneric(adjustedFrame0.pixelFormat()))
	{
		adjustedFrame0.setPixelFormat(FrameType::findPixelFormat(adjustedFrame0.dataType(), adjustedFrame0.channels()));
	}

	if (Frame::formatIsPureGeneric(adjustedFrame1.pixelFormat()))
	{
		adjustedFrame1.setPixelFormat(FrameType::findPixelFormat(adjustedFrame1.dataType(), adjustedFrame1.channels()));
	}

	Frame rgbFrame0, rgbFrame1;
	if (!CV::FrameConverter::Comfort::convert(adjustedFrame0, FrameType::FORMAT_RGB24, rgbFrame0, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker)
			|| !CV::FrameConverter::Comfort::convert(adjustedFrame1, FrameType::FORMAT_RGB24, rgbFrame1, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return Frame();
	}

	Frame result(FrameType(rgbFrame0, rgbFrame0.width() + rgbFrame1.width(), max(rgbFrame0.height(), rgbFrame1.height())));
	result.setValue(0x00u);

	result.copy(0u, 0u, rgbFrame0);
	result.copy(rgbFrame0.width(), 0u, rgbFrame1);

	if (numberPoints != 0)
	{
		Vectors2 offsetPoints1(numberPoints);
		for (size_t n = 0; n < numberPoints; ++n)
		{
			offsetPoints1[n] = Vector2(points1[n].x() + Scalar(rgbFrame0.width()), points1[n].y());
		}

		paintLines(result, points0, offsetPoints1.data(), numberPoints, color, worker);
	}

	return result;
}

Frame Utilities::paintCorrespondencesVertical(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* color, Worker* worker)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.numberPlanes() == 1u);
	ocean_assert(frame0.pixelOrigin() == frame1.pixelOrigin());

	// the given frames can have an unspecified pixel format (e.g., a generic pixel format with 3 channels, but without specified channel order like RGB or BGR),
	// therefore, we simply cast those pixel formats to one of four pixel formats: Y8, YA16, RGB8, RGBA32

	Frame adjustedFrame0(frame0, Frame::ACM_USE_KEEP_LAYOUT);
	Frame adjustedFrame1(frame1, Frame::ACM_USE_KEEP_LAYOUT);

	if (Frame::formatIsPureGeneric(adjustedFrame0.pixelFormat()))
	{
		adjustedFrame0.setPixelFormat(FrameType::findPixelFormat(adjustedFrame0.dataType(), adjustedFrame0.channels()));
	}

	if (Frame::formatIsPureGeneric(adjustedFrame1.pixelFormat()))
	{
		adjustedFrame1.setPixelFormat(FrameType::findPixelFormat(adjustedFrame1.dataType(), adjustedFrame1.channels()));
	}

	Frame rgbFrame0, rgbFrame1;
	if (!CV::FrameConverter::Comfort::convert(adjustedFrame0, FrameType::FORMAT_RGB24, rgbFrame0, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker)
			|| !CV::FrameConverter::Comfort::convert(adjustedFrame1, FrameType::FORMAT_RGB24, rgbFrame1, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return Frame();
	}

	Frame result(FrameType(rgbFrame0, max(rgbFrame0.width(), rgbFrame1.width()), rgbFrame0.height() + rgbFrame1.height()));
	result.setValue(0x00u);

	result.copy(0u, 0u, rgbFrame0);
	result.copy(rgbFrame0.width(), 0u, rgbFrame1);

	if (numberPoints != 0)
	{
		Vectors2 offsetPoints1(numberPoints);
		for (size_t n = 0; n < numberPoints; ++n)
		{
			offsetPoints1[n] = Vector2(points1[n].x(), points1[n].y() + Scalar(rgbFrame0.height()));
		}

		paintLines(result, points0, offsetPoints1.data(), numberPoints, color, worker);
	}

	return result;
}

bool Utilities::paintCorrespondencesHomography(const Frame& frame0, const Frame& frame1, const SquareMatrix3& points1_H_points0, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const bool fullCoverage, Frame& result, const uint8_t* foregroundColor, const uint8_t* backgroundColor, const uint8_t* startColor, Worker* worker)
{
	ocean_assert(frame0 && frame1);
	ocean_assert(frame0.pixelFormat() == frame1.pixelFormat());
	ocean_assert(frame0.pixelOrigin() == frame1.pixelOrigin());
	ocean_assert(!points1_H_points0.isSingular());

	// framePoint1 = H * framePoint0
	// framePoint0 = H^-1 * framePoint1

	SquareMatrix3 invHomography;
	if (!points1_H_points0.invert(invHomography))
	{
		return false;
	}

	unsigned int frame0Left = 0u;
	unsigned int frame0Top = 0u;

	if (fullCoverage)
	{
		if (!alignFramesHomographyFullCoverage(frame0, frame1, points1_H_points0, result, true, worker, 16384u, 16384u, &frame0Left, &frame0Top))
		{
			return false;
		}
	}
	else
	{
		if (!alignFramesHomography(frame0, frame1, points1_H_points0, result, true, worker))
		{
			return false;
		}
	}

	Vectors2 adjustedPoints0(numberPoints);
	Vectors2 adjustedPoints1(numberPoints);

	ocean_assert(result.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && result.channels() <= 4u);

	for (size_t n = 0; n < numberPoints; ++n)
	{
		ocean_assert(points0[n].x() >= Scalar(0) && points0[n].x() < Scalar(frame0.width()));
		ocean_assert(points0[n].y() >= Scalar(0) && points0[n].y() < Scalar(frame0.height()));

		ocean_assert(points1[n].x() >= Scalar(0) && points1[n].x() < Scalar(frame1.width()));
		ocean_assert(points1[n].y() >= Scalar(0) && points1[n].y() < Scalar(frame1.height()));

		// we need to adjust the location of the points from the first frame as the first frame may have been shifted
		adjustedPoints0[n] = points0[n] + Vector2(Scalar(frame0Left), Scalar(frame0Top));

		// we need the points from the second frame visible in the first frame
		adjustedPoints1[n] = invHomography * points1[n] + Vector2(Scalar(frame0Left), Scalar(frame0Top));
	}

	if (backgroundColor)
	{
		paintLines<1u, 3u>(result, adjustedPoints0.data(), adjustedPoints1.data(), numberPoints, foregroundColor, backgroundColor, worker);
	}
	else
	{
		paintLines<1u>(result, adjustedPoints0.data(), adjustedPoints1.data(), numberPoints, foregroundColor, worker);
	}

	if (startColor)
	{
		paintImagePoints<5u>(result, adjustedPoints0.data(), adjustedPoints0.size(), startColor, worker);
	}

	return true;
}

bool Utilities::paintCorrespondencesOrientations(const PinholeCamera& pinholeCamera, const Frame& frame0, const Frame& frame1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, Frame& result, const uint8_t* foregroundColor, const uint8_t* backgroundColor, const uint8_t* startColor, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(frame0 && frame1);
	ocean_assert(frame0.width() == pinholeCamera.width() && frame0.height() == pinholeCamera.height());
	ocean_assert(frame0.frameType() == frame1.frameType());
	ocean_assert(!orientation0.isSingular() && !orientation1.isSingular());

	ocean_assert(points0 || numberPoints == 0);
	ocean_assert(points1 || numberPoints == 0);

	const unsigned int panoramaWidth = max(2u, ((unsigned int)(Scalar(pinholeCamera.width()) * Numeric::pi2() / pinholeCamera.fovX() + Scalar(0.5)) / 2u) * 2u);
	ocean_assert(panoramaWidth % 2u == 0u);

	CV::Advanced::PanoramaFrame panoramaFrame(panoramaWidth, panoramaWidth / 2u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_LOCAL);

	// we set the orientation of the first frame to a identity orientation and adjust the second orientation accordingly

	SquareMatrix3 invOrientation0;
	if (!orientation0.invert(invOrientation0))
	{
		return false;
	}

	const SquareMatrix3 adjustedOrientation0(true);
	const SquareMatrix3 adjustedOrientation1(invOrientation0 * orientation1);

	panoramaFrame.addFrame(pinholeCamera, adjustedOrientation0, frame0, Frame(), 20u, worker);
	panoramaFrame.addFrame(pinholeCamera, adjustedOrientation1, frame1, Frame(), 20u, worker);

	const Vector2 topLeft(Scalar(panoramaFrame.frameTopLeft().x()), Scalar(panoramaFrame.frameTopLeft().y()));

	result = Frame(panoramaFrame.frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	Vectors2 startPoints, endPoints;
	startPoints.reserve(numberPoints);
	endPoints.reserve(numberPoints);

	for (size_t n = 0; n < numberPoints; ++n)
	{
		const Vector2 startPoint = panoramaFrame.cameraPixel2panoramaPixel(pinholeCamera, adjustedOrientation0, points0[n], Vector2(0, 0)) - topLeft;
		const Vector2 endPoint = panoramaFrame.cameraPixel2panoramaPixel(pinholeCamera, adjustedOrientation1, points1[n], Vector2(0, 0)) - topLeft;

		startPoints.push_back(startPoint);
		endPoints.push_back(endPoint);
	}

	if (backgroundColor)
	{
		paintLines<1u, 3u>(result, startPoints.data(), endPoints.data(), numberPoints, foregroundColor, backgroundColor, worker);
	}
	else
	{
		paintLines<1u>(result, startPoints.data(), endPoints.data(), numberPoints, foregroundColor, worker);
	}

	if (startColor)
	{
		paintImagePoints<5u>(result, startPoints.data(), startPoints.size(), startColor, worker);
	}

	return true;
}

void Utilities::paintPoints(Frame& frame, const Vector2* imagePoints, const size_t number, const unsigned int radius, const uint8_t* colorInner, const uint8_t* colorOuter)
{
	ocean_assert(frame.isValid() && imagePoints != nullptr);

	if (number == 0)
	{
		return;
	}

  // Ellipses with radius = 0 are not supported, this case must be handled separately.
  if (radius == 0)
  {
    for (size_t n = 0; n < number; ++n)
    {
      CV::Canvas::point<1>(frame, imagePoints[n], colorInner);
    }
    return;
  }

	const uint8_t* const white = CV::Canvas::white(frame.pixelFormat());
	const uint8_t* const black = CV::Canvas::black(frame.pixelFormat());

	for (size_t n = 0; n < number; ++n)
	{
		const unsigned int x = (unsigned int)(Numeric::round32(imagePoints[n].x()));
		const unsigned int y = (unsigned int)(Numeric::round32(imagePoints[n].y()));

		if (x < frame.width() && y < frame.height())
		{
			CV::Canvas::ellipse(frame, CV::PixelPosition(x, y), radius * 2u + 1u, radius * 2u + 1u, colorOuter ? colorOuter : black);

			if (radius > 1u)
			{
				CV::Canvas::ellipse(frame, CV::PixelPosition(x, y), (radius - 1u) * 2u + 1u, (radius - 1u) * 2u + 1u, colorInner ? colorInner : white);
			}
		}
	}
}

void Utilities::paintPointsIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const bool distortProjectedObjectPoints, const unsigned int radiusObjectPoints, const unsigned int radiusImagePoints, const uint8_t* colorObjectPoints, const uint8_t* colorImagePoints)
{
	ocean_assert(frame && flippedCamera_T_world.isValid() && pinholeCamera.isValid());

	if (numberObjectPoints > 0u && radiusObjectPoints >= 1u)
	{
		ocean_assert(objectPoints);

		Vectors2 projectedObjectPoints(numberObjectPoints);
		pinholeCamera.projectToImageIF<true>(flippedCamera_T_world, objectPoints, numberObjectPoints, distortProjectedObjectPoints, projectedObjectPoints.data());

		paintPoints(frame, projectedObjectPoints.data(), projectedObjectPoints.size(), radiusObjectPoints, colorObjectPoints);
	}

	if (numberImagePoints >= 1u && radiusImagePoints >= 1u)
	{
		ocean_assert(imagePoints);
		paintPoints(frame, imagePoints, numberImagePoints, radiusImagePoints, colorImagePoints);
	}
}

void Utilities::paintBoundingBoxIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Box3& boundingBox, const uint8_t* foregroundColor, const uint8_t* backgroundColor, const bool edgesOnly)
{
	ocean_assert(frame && flippedCamera_T_world.isValid() && anyCamera.isValid() && boundingBox);
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == anyCamera.height());

	Vector3 corners[8];
	const unsigned int numberCorners = boundingBox.corners(corners);

	for (unsigned int a = 0u; a < numberCorners; ++a)
	{
		for (unsigned int b = a + 1u; b < numberCorners; ++b)
		{
			const Vector3& startPoint = corners[a];
			const Vector3& endPoint = corners[b];

			unsigned int identical = 0u;

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				if (Numeric::isEqual(startPoint[n], endPoint[n]))
				{
					identical++;
				}
			}

			if (!edgesOnly || identical == 2u)
			{
				for (unsigned int n = 0; n < 5u; ++n)
				{
					const Vector3 point0((startPoint * Scalar(5u - n) + endPoint * Scalar(n)) * Scalar(0.2));
					const Vector3 point1((startPoint * Scalar(5u - n - 1u) + endPoint * Scalar(n + 1u)) * Scalar(0.2));

					if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, point0) && AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, point1))
					{
						const Vector2 projected0(anyCamera.projectToImageIF(flippedCamera_T_world, point0));
						const Vector2 projected1(anyCamera.projectToImageIF(flippedCamera_T_world, point1));

						if (backgroundColor)
						{
							CV::Canvas::line<3u>(frame, projected0.x(), projected0.y(), projected1.x(), projected1.y(), backgroundColor);
						}

						if (foregroundColor)
						{
							CV::Canvas::line<1u>(frame, projected0.x(), projected0.y(), projected1.x(), projected1.y(), foregroundColor);
						}
					}
				}
			}
		}
	}
}

void Utilities::paintWireframeConeIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_cone, const PinholeCamera& pinholeCamera, const Cone3& cone, const bool distortProjectedObjectPoints, Worker* worker, const uint8_t* color, const unsigned int numCircles, const unsigned int numVerticalLines, const unsigned int numSamples)
{
	ocean_assert(frame && flippedCamera_T_cone.isValid() && pinholeCamera && cone.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());
	ocean_assert(numCircles >= 2u && numSamples >= 3u);

	// The drawing procedure below assumes that the pose is given w.r.t a cone with the axis [0, 0, 1].
	// If this isn't the case, we'll first compute a corrected pose.
	HomogenousMatrix4 flippedCamera_T_canonicalCone = flippedCamera_T_cone;
	Vector3 apex = cone.apex();

	if (cone.axis() != Vector3(Scalar(0.0), Scalar(0.0), Scalar(1.0)))
	{
		const Quaternion quaternion_cone_T_canonicalCone(Vector3(Scalar(0.0), Scalar(0.0), Scalar(1.0)), cone.axis());
		flippedCamera_T_canonicalCone = flippedCamera_T_cone * HomogenousMatrix4(quaternion_cone_T_canonicalCone);
		apex = quaternion_cone_T_canonicalCone.inverted() * apex;
	}

	const Scalar angleStep = Numeric::pi2() / Scalar(numSamples);

	std::vector<Vectors2> paths;
	paths.reserve(numCircles + numVerticalLines);

	const Scalar tanHalfApexAngle = Numeric::tan(Scalar(0.5) * cone.apexAngle());

	const Scalar zStep = (cone.maxSignedDistanceAlongAxis() - cone.minSignedDistanceAlongAxis()) / Scalar(numCircles - 1u);

	// Draw circles.
	for (unsigned int i = 0u; i < numCircles; ++i)
	{
		paths.emplace_back();

		Vectors2& path = paths.back();
		path.reserve(numSamples + 1u);

		const Scalar z = cone.minSignedDistanceAlongAxis() + Scalar(i) * zStep;
		const Scalar radius = z * tanHalfApexAngle;

		for (unsigned int j = 0u; j < numSamples; ++j)
		{
			const Scalar theta = Scalar(j) * angleStep;
			const Vector3 point(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);

			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCone, point + apex, distortProjectedObjectPoints));
		}

		path.push_back(path[0]);
	}

	// Draw some vertical lines down the sides of the cone.
	if (numVerticalLines > 0u)
	{
		const Scalar verticalLineAngleStep = Numeric::pi2() / Scalar(numVerticalLines);
		const Scalar verticalLineAngleOffset = Scalar(0.5) * verticalLineAngleStep;
		for (unsigned int i = 0u; i < numVerticalLines; ++i)
		{
			Vectors2 path;
			path.reserve(2u);

			const Scalar theta = verticalLineAngleOffset + Scalar(i) * verticalLineAngleStep;

			Scalar z = cone.minSignedDistanceAlongAxis();
			Scalar radius = z * tanHalfApexAngle;
			Vector3 point(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);
			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCone, point + apex, distortProjectedObjectPoints));

			z = cone.maxSignedDistanceAlongAxis();
			radius = z * tanHalfApexAngle;
			point = Vector3(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);
			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCone, point + apex, distortProjectedObjectPoints));

			paths.push_back(path);
		}
	}

	Tracking::Utilities::paintPaths<3>(frame, paths.data(), paths.size(), color, worker);
}

void Utilities::paintWireframeCylinderIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_cylinder, const PinholeCamera& pinholeCamera, const Cylinder3& cylinder, const bool distortProjectedObjectPoints, Worker* worker, const uint8_t* color, const unsigned int numCircles, const unsigned int numVerticalLines, const unsigned int numSamples)
{
	ocean_assert(frame && flippedCamera_T_cylinder.isValid() && pinholeCamera && cylinder.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());
	ocean_assert(numCircles >= 2u && numSamples >= 3u);

	// The drawing procedure below assumes that the pose is given w.r.t a cylinder with the axis [0, 0, 1].
	// If this isn't the case, we'll first compute a corrected pose.
	HomogenousMatrix4 flippedCamera_T_canonicalCylinder = flippedCamera_T_cylinder;
	Vector3 origin = cylinder.origin();

	if (cylinder.axis() != Vector3(Scalar(0.0), Scalar(0.0), Scalar(1.0)))
	{
		const Quaternion quaternion_cylinder_T_canonicalCylinder(Vector3(Scalar(0.0), Scalar(0.0), Scalar(1.0)), cylinder.axis());
		flippedCamera_T_canonicalCylinder = flippedCamera_T_cylinder * HomogenousMatrix4(quaternion_cylinder_T_canonicalCylinder);
		origin = quaternion_cylinder_T_canonicalCylinder.inverted() * origin;
	}

	const Scalar angleStep = Numeric::pi2() / Scalar(numSamples);

	std::vector<Vectors2> paths;
	paths.reserve(numCircles + numVerticalLines);

	const Scalar zStep = (cylinder.maxSignedDistanceAlongAxis() - cylinder.minSignedDistanceAlongAxis()) / Scalar(numCircles - 1u);

	// Draw circles.
	for (unsigned int i = 0u; i < numCircles; ++i)
	{
		paths.emplace_back();

		Vectors2& path = paths.back();
		path.reserve(numSamples + 1u);

		const Scalar z = cylinder.minSignedDistanceAlongAxis() + Scalar(i) * zStep;

		for (unsigned int j = 0u; j < numSamples; ++j)
		{
			const Scalar theta = Scalar(j) * angleStep;
			const Vector3 point(cylinder.radius() * Numeric::cos(theta), cylinder.radius() * Numeric::sin(theta), z);

			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCylinder, point + origin, distortProjectedObjectPoints));
		}

		path.push_back(path[0]);
	}

	// Draw some vertical lines down the sides of the cylinder.
	if (numVerticalLines > 0u)
	{
		const Scalar verticalLineAngleStep = Numeric::pi2() / Scalar(numVerticalLines);
		const Scalar verticalLineAngleOffset = Scalar(0.5) * verticalLineAngleStep;

		for (unsigned int i = 0u; i < numVerticalLines; ++i)
		{
			Vectors2 path;
			path.reserve(2u);

			const Scalar theta = verticalLineAngleOffset + Scalar(i) * verticalLineAngleStep;

			Vector3 point(cylinder.radius() * Numeric::cos(theta), cylinder.radius() * Numeric::sin(theta), cylinder.minSignedDistanceAlongAxis());

			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCylinder, point + origin, distortProjectedObjectPoints));

			point.z() = cylinder.maxSignedDistanceAlongAxis();
			path.push_back(pinholeCamera.projectToImageIF<true>(flippedCamera_T_canonicalCylinder, point + origin, distortProjectedObjectPoints));

			paths.push_back(path);
		}
	}

	Tracking::Utilities::paintPaths<3>(frame, paths.data(), paths.size(), color, worker);
}

void Utilities::paintLineIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Vector3& objectPoint0, const Vector3& objectPoint1, const unsigned int segments, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	ocean_assert(frame && flippedCamera_T_world.isValid() && anyCamera.isValid());
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == anyCamera.height());
	ocean_assert(segments >= 1u);

	const Scalar segmentFactor = Scalar(1) / Scalar(segments);

	if (backgroundColor)
	{
		Vector2 projectedStart = anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint0);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			const Vector3 end = objectPoint0 + (objectPoint1 - objectPoint0) * Scalar(n + 1) * segmentFactor;
			const Vector2 projectedEnd = anyCamera.projectToImageIF(flippedCamera_T_world, end);

			CV::Canvas::line<3u>(frame, projectedStart.x(), projectedStart.y(), projectedEnd.x(), projectedEnd.y(), backgroundColor);

			projectedStart = projectedEnd;
		}
	}

	if (foregroundColor)
	{
		Vector2 projectedStart = anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint0);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			const Vector3 end = objectPoint0 + (objectPoint1 - objectPoint0) * Scalar(n + 1) * segmentFactor;
			const Vector2 projectedEnd = anyCamera.projectToImageIF(flippedCamera_T_world, end);

			CV::Canvas::line<1u>(frame, projectedStart.x(), projectedStart.y(), projectedEnd.x(), projectedEnd.y(), foregroundColor);

			projectedStart = projectedEnd;
		}
	}
}

void Utilities::paintCoordinateSystemIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_coordinateSystem, const Scalar length)
{
	ocean_assert(frame && flippedCamera_T_world.isValid() && anyCamera.isValid() && world_T_coordinateSystem.isValid());
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == frame.height());

	const uint8_t* const red = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const blue = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const black = CV::Canvas::black(frame.pixelFormat());

	if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, world_T_coordinateSystem.translation()))
	{
		const Vector3 xAxis = world_T_coordinateSystem * Vector3(length, 0, 0);
		const Vector3 yAxis = world_T_coordinateSystem * Vector3(0, length, 0);
		const Vector3 zAxis = world_T_coordinateSystem * Vector3(0, 0, length);

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, xAxis))
		{
			paintLineIF(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), xAxis, /* segments */ 5u, red, black);
		}

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, yAxis))
		{
			paintLineIF(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), yAxis, /* segments */ 5u, green, black);
		}

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, zAxis))
		{
			paintLineIF(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), zAxis, /* segments */ 5u, blue, black);
		}
	}
}

bool Utilities::paintPlane(Frame& frame, const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const HomogenousMatrix4& planeTransformation, const unsigned int bins, const uint8_t* foregroundColor, const uint8_t* backgroundColor, Scalar* expansion)
{
	ocean_assert(frame && world_T_camera.isValid() && camera.isValid() && planeTransformation.isValid());
	ocean_assert(frame.width() == camera.width() && frame.height() == camera.height());

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	// we first need to determine a worthwhile expansion for the scene

	const Vector2 projectedOrigin = camera.projectToImageIF(flippedCamera_T_world, planeTransformation.translation());

	Vector2 expansionPoints[4] =
	{
		projectedOrigin - Vector2(Scalar(camera.width()) * Scalar(0.125), 0),
		projectedOrigin + Vector2(Scalar(camera.width()) * Scalar(0.125), 0),
		projectedOrigin - Vector2(0, Scalar(camera.height()) * Scalar(0.125)),
		projectedOrigin + Vector2(0, Scalar(camera.height()) * Scalar(0.125))
	};

	const Plane3 plane(planeTransformation);

	Vector3 planeExpansionPoints[4];

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		const Line3 planeExpansion = camera.ray(expansionPoints[n], world_T_camera);

		if (!plane.intersection(planeExpansion, planeExpansionPoints[n]))
		{
			return false;
		}
	}

	Scalar minimalSqrDistance = Numeric::maxValue();

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		for (unsigned int i = n + 1u; i < 4u; ++i)
		{
			minimalSqrDistance = min(minimalSqrDistance, planeExpansionPoints[n].sqrDistance(planeExpansionPoints[i]));
		}
	}

	const Scalar minimalDistance = Numeric::sqrt(minimalSqrDistance);

	if (Numeric::isEqualEps(minimalDistance))
	{
		return false;
	}

	if (!paintPlaneIF(frame, flippedCamera_T_world, camera, planeTransformation, minimalDistance, bins, foregroundColor, backgroundColor))
	{
		return false;
	}

	if (expansion)
	{
		*expansion = minimalDistance;
	}

	return true;
}

bool Utilities::paintPlaneIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const HomogenousMatrix4& planeTransformation, const Scalar expansion, const unsigned int bins, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	ocean_assert(frame && flippedCamera_T_world.isValid() && camera.isValid() && planeTransformation.isValid());
	ocean_assert(frame.width() == camera.width() && frame.height() == camera.height());
	ocean_assert(bins >= 1u && expansion > 0);

	const Vector3 origin = planeTransformation.translation();

	const Scalar halfExpansion = expansion * Scalar(0.5);

	const Vector3 xAxis(planeTransformation.xAxis());
	const Vector3 yAxis(planeTransformation.yAxis());

	const Scalar binFactor = Scalar(1) / Scalar(bins);

	for (unsigned int x = 0; x <= bins; ++x)
	{
		const Scalar xx = expansion * (Scalar(x) * binFactor - Scalar(0.5));

		paintLineIF(frame, flippedCamera_T_world, camera, origin + xAxis * xx + yAxis * -halfExpansion, origin + xAxis * xx + yAxis * halfExpansion, bins * 2u, foregroundColor, backgroundColor);
	}

	for (unsigned int y = 0; y <= bins; ++y)
	{
		const Scalar yy = expansion * (Scalar(y) * binFactor - Scalar(0.5));

		paintLineIF(frame, flippedCamera_T_world, camera, origin + xAxis * -halfExpansion + yAxis * yy, origin + xAxis * halfExpansion + yAxis * yy, bins * 2u, foregroundColor, backgroundColor);
	}

	paintCoordinateSystemIF(frame, flippedCamera_T_world, camera, planeTransformation, expansion * binFactor * Scalar(1.5));

	return true;
}

void Utilities::paintQuadsIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const Vector3& quadOrigin, const Vector3& quadHorizontal, const Vector3& quadVertical, const unsigned int horizontalBins, const unsigned int verticalBins, const uint8_t* color)
{
	ocean_assert(frame.isValid() && flippedCamera_T_world.isValid() && camera.isValid());
	ocean_assert(frame.width() == camera.width() && frame.height() == camera.height());
	ocean_assert(!quadHorizontal.isNull() && !quadVertical.isNull());

	ocean_assert(horizontalBins >= 1u && verticalBins >= 1u);

	const Scalar iHorizontalBins = Scalar(1) / Scalar(horizontalBins);
	const Scalar iVerticalBins = Scalar(1) / Scalar(verticalBins);

	const Vector3 binHorizontal(quadHorizontal * iHorizontalBins);
	const Vector3 binVertical(quadVertical * iVerticalBins);

	for (unsigned int vBin = 0u; vBin <= verticalBins; ++vBin)
	{
		for (unsigned int hBin = 0u; hBin <= horizontalBins; ++hBin)
		{
			const Vector3 start(quadOrigin + binHorizontal * Scalar(hBin) + binVertical * Scalar(vBin));
			const Vector3 endHorizontal(start + binHorizontal);
			const Vector3 endVertical(start + binVertical);

			const Vector2 projectedStart(camera.projectToImageIF(flippedCamera_T_world, start));

			if (hBin < horizontalBins)
			{
				const Vector2 projectedHorizontal(camera.projectToImageIF(flippedCamera_T_world, endHorizontal));
				paintLine(frame, projectedStart, projectedHorizontal, color);
			}

			if (vBin < verticalBins)
			{
				const Vector2 projectedVertical(camera.projectToImageIF(flippedCamera_T_world, endVertical));
				paintLine(frame, projectedStart, projectedVertical, color);
			}
		}
	}
}

Frame Utilities::paintMask(const Frame& frame, const Frame& mask, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(frame && mask);
	ocean_assert(FrameType(mask, frame.pixelFormat()) == frame.frameType());
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(maskValue == 0x00 || maskValue == 0xFF);

	Frame result(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	switch (FrameType::channels(frame.pixelFormat()))
	{
		case 1u:
			if (maskValue == 0xFF)
			{
				CV::FrameBlender::blend8BitPerChannel<1u, true>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 1u>, mask.paddingElements(), result.paddingElements(), worker);
			}
			else
			{
				CV::FrameBlender::blend8BitPerChannel<1u, false>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 1u>, mask.paddingElements(), result.paddingElements(), worker);
			}

			return result;

		case 2u:
			if (maskValue == 0xFF)
			{
				CV::FrameBlender::blend8BitPerChannel<2u, true>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 2u>, mask.paddingElements(), result.paddingElements(), worker);
			}
			else
			{
				CV::FrameBlender::blend8BitPerChannel<2u, false>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 2u>, mask.paddingElements(), result.paddingElements(), worker);
			}

			return result;

		case 3u:
			if (maskValue == 0xFF)
			{
				CV::FrameBlender::blend8BitPerChannel<3u, true>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 3u>, mask.paddingElements(), result.paddingElements(), worker);
			}
			else
			{
				CV::FrameBlender::blend8BitPerChannel<3u, false>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 3u>, mask.paddingElements(), result.paddingElements(), worker);
			}

			return result;

		case 4u:
			if (maskValue == 0xFF)
			{
				CV::FrameBlender::blend8BitPerChannel<4u, true>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 4u>, mask.paddingElements(), result.paddingElements(), worker);
			}
			else
			{
				CV::FrameBlender::blend8BitPerChannel<4u, false>(mask.constdata<uint8_t>(), result.data<uint8_t>(), result.width(), result.height(), blendPixel<0u, 4u>, mask.paddingElements(), result.paddingElements(), worker);
			}

			return result;
	};

	ocean_assert(false && "Invalid channel number!");
	return Frame();
}

Frame Utilities::paintBoundingBox(const Frame& frame, const CV::PixelBoundingBox& boundingBox, Worker* worker)
{
	ocean_assert(frame && boundingBox);

	ocean_assert(boundingBox.left() + boundingBox.width() <= frame.width());
	ocean_assert(boundingBox.top() + boundingBox.height() <= frame.height());

	Frame mask(FrameType(frame, FrameType::FORMAT_Y8));
	mask.setValue(0xFFu);

	CV::Canvas::rectangle8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), boundingBox.left(), boundingBox.top(), boundingBox.width(), boundingBox.height(), nullptr, mask.paddingElements());

	return paintMask(frame, mask, 0x00, worker);
}

bool Utilities::alignFramesHomography(const Frame& fixedFrame, const Frame& dynamicFrame, const SquareMatrix3& dynamic_H_fixed, Frame& result, const bool blend, Worker* worker)
{
	ocean_assert(fixedFrame && dynamicFrame);
	ocean_assert(fixedFrame.pixelFormat() == dynamicFrame.pixelFormat() && fixedFrame.pixelOrigin() == dynamicFrame.pixelOrigin());
	ocean_assert(!dynamic_H_fixed.isSingular());

	if (blend)
	{
		Frame transformedDynamicFrame(fixedFrame.frameType());

		if (!CV::FrameInterpolatorBilinear::Comfort::homography(dynamicFrame, transformedDynamicFrame, dynamic_H_fixed, nullptr, worker))
		{
			return false;
		}

		Frame internalResult(fixedFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (!CV::FrameBlender::blend<true>(transformedDynamicFrame, internalResult, 0x80, worker))
		{
			return false;
		}

		result = std::move(internalResult);
	}
	else
	{
		Frame transformedDynamicFrame(fixedFrame.frameType());
		Frame transformedDynamicMask(FrameType(transformedDynamicFrame, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(dynamicFrame, transformedDynamicFrame, transformedDynamicMask, dynamic_H_fixed, worker, 0x00))
		{
			return false;
		}

		Frame internalResult(fixedFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		const unsigned int channels = internalResult.channels();
		ocean_assert(channels >= 1u);

		for (unsigned int y = 0u; y < internalResult.height(); ++y)
		{
			const uint8_t* const sourceData = transformedDynamicFrame.constrow<uint8_t>(y);
			const uint8_t* const sourceMask = transformedDynamicMask.constrow<uint8_t>(y);

			uint8_t* const targetData = internalResult.row<uint8_t>(y);

			for (unsigned int x = 0u; x < internalResult.width(); ++x)
			{
				if (sourceMask[x] == 0x00)
				{
					for (unsigned int n = 0u; n < channels; ++n)
					{
						targetData[x * channels + n] = sourceData[x * channels + n];
					}
				}
			}
		}

		result = std::move(internalResult);
	}

	return true;
}

bool Utilities::alignFramesHomographyFullCoverage(const Frame& fixedFrame, const Frame& dynamicFrame, const SquareMatrix3& dynamic_H_fixed, Frame& result, const bool blend, Worker* worker, unsigned int maximalWidth, const unsigned int maximalHeight, unsigned int* fixedFrameLeft, unsigned int* fixedFrameTop, Scalar* dynamicFrameLeft, Scalar* dynamicFrameTop, Frame* fullFixedFrame, Frame* fullDynamicFrame)
{
	ocean_assert(fixedFrame && dynamicFrame);
	ocean_assert(fixedFrame.pixelFormat() == dynamicFrame.pixelFormat() && fixedFrame.pixelOrigin() == dynamicFrame.pixelOrigin());
	ocean_assert(!dynamic_H_fixed.isSingular());

	SquareMatrix3 invHomography;
	if (!dynamic_H_fixed.invert(invHomography))
	{
		return false;
	}

	const Box2 boundingBoxFixedFrame(Vector2(0, 0), Vector2(Scalar(fixedFrame.width()), Scalar(fixedFrame.height())));

	Box2 boundingBoxTransformedDynamicFrame;
	boundingBoxTransformedDynamicFrame += invHomography * Vector2(0, 0);
	boundingBoxTransformedDynamicFrame += invHomography * Vector2(Scalar(dynamicFrame.width()), 0);
	boundingBoxTransformedDynamicFrame += invHomography * Vector2(0, Scalar(dynamicFrame.height()));
	boundingBoxTransformedDynamicFrame += invHomography * Vector2(Scalar(dynamicFrame.width()), Scalar(dynamicFrame.height()));

	const Box2 unionBoundingBox(boundingBoxFixedFrame + boundingBoxTransformedDynamicFrame);

	int virtualLeft, virtualTop;
	unsigned int alignedWidth, alignedHeight;
	if (!unionBoundingBox.box2integer(NumericT<int>::minValue(), NumericT<int>::minValue(), NumericT<int>::maxValue(), NumericT<int>::maxValue(), virtualLeft, virtualTop, alignedWidth, alignedHeight))
	{
		return false;
	}

	if (alignedWidth > maximalWidth || alignedHeight > maximalHeight)
	{
		return false;
	}

	ocean_assert(alignedWidth >= fixedFrame.width() && alignedHeight >= fixedFrame.height());

	Frame enlargedFixedFrame(FrameType(fixedFrame, alignedWidth, alignedHeight));
	enlargedFixedFrame.setValue(0x00u);

	const unsigned int internalFixedFrameLeft = abs(virtualLeft);
	const unsigned int internalFixedFrameTop = abs(virtualTop);

	if (!enlargedFixedFrame.copy(internalFixedFrameLeft, internalFixedFrameTop, fixedFrame))
	{
		return false;
	}

	if (fixedFrameLeft)
	{
		*fixedFrameLeft = internalFixedFrameLeft;
	}

	if (fixedFrameTop)
	{
		*fixedFrameTop = internalFixedFrameTop;
	}

	if (fullFixedFrame)
	{
		fullFixedFrame->copy(enlargedFixedFrame);
	}

	if (dynamicFrameLeft)
	{
		*dynamicFrameLeft = boundingBoxTransformedDynamicFrame.left();
	}

	if (dynamicFrameTop)
	{
		*dynamicFrameTop = boundingBoxTransformedDynamicFrame.top();
	}

	if (blend)
	{
		Frame transformedDynamicFrame(enlargedFixedFrame.frameType());
		transformedDynamicFrame.setValue(0x00u);

		if (!CV::FrameInterpolatorBilinear::Comfort::homography(dynamicFrame, transformedDynamicFrame, dynamic_H_fixed, nullptr, worker, CV::PixelPositionI(virtualLeft, virtualTop)))
		{
			return false;
		}

		if (fullDynamicFrame)
		{
			fullDynamicFrame->copy(transformedDynamicFrame);
		}

		if (!CV::FrameBlender::blend<true>(transformedDynamicFrame, enlargedFixedFrame, 0x80, worker))
		{
			return false;
		}

		result = std::move(enlargedFixedFrame);
	}
	else
	{
		Frame transformedDynamicFrame(enlargedFixedFrame.frameType());
		transformedDynamicFrame.setValue(0x00u);

		Frame transformedDynamicMask(FrameType(transformedDynamicFrame, FrameType::FORMAT_Y8));

		if (!CV::FrameInterpolatorBilinear::Comfort::homographyMask(dynamicFrame, transformedDynamicFrame, transformedDynamicMask, dynamic_H_fixed, worker, 0x00, CV::PixelPositionI(virtualLeft, virtualTop)))
		{
			return false;
		}

		if (fullDynamicFrame)
		{
			fullDynamicFrame->copy(transformedDynamicFrame);
		}

		const unsigned int channels = transformedDynamicFrame.channels();
		ocean_assert(channels >= 1u);

		for (unsigned int y = 0u; y < enlargedFixedFrame.height(); ++y)
		{
			const uint8_t* const sourceData = transformedDynamicFrame.constrow<uint8_t>(y);
			const uint8_t* const sourceMask = transformedDynamicMask.constrow<uint8_t>(y);

			uint8_t* const targetData = enlargedFixedFrame.row<uint8_t>(y);

			for (unsigned int x = 0u; x < enlargedFixedFrame.width(); ++x)
			{
				if (sourceMask[x] == 0x00)
				{
					for (unsigned int n = 0u; n < channels; ++n)
					{
						targetData[x * channels + n] = sourceData[x * channels + n];
					}
				}
			}
		}

		result = std::move(enlargedFixedFrame);
	}

	return true;
}

void Utilities::visualizeDatabase(const Database& database, const Index32 poseId, Frame& frame, const uint8_t* colorImagePoints, const uint8_t* colorImagePointsInstable, const uint8_t* colorImagePointsStable, const unsigned int maximalPathLength, const unsigned int stablePathLength, const SquareMatrix3& transformation, Worker* worker)
{
	ocean_assert(frame && colorImagePoints && colorImagePointsInstable && colorImagePointsStable);
	ocean_assert(stablePathLength >= 1);

	if (!database.hasPose<false>(poseId))
	{
		return;
	}

	Indices32 imagePointIds;
	Vectors2 imagePoints = database.imagePoints<false>(poseId, &imagePointIds);

	if (!transformation.isIdentity())
	{
		for (Vectors2::iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
		{
			*i = transformation * *i;
		}
	}

	Utilities::paintImagePoints<3u>(frame, imagePoints.data(), imagePoints.size(), colorImagePoints, worker);

	if (maximalPathLength < 2u)
	{
		return;
	}

	const Database::IdIdPointPairsMap featurePointPairsMap = database.imagePoints<false>(poseId, true, 1);

	std::vector<Vectors2> imagePointGroups;
	imagePointGroups.reserve(featurePointPairsMap.size());

	Scalars factors;
	factors.reserve(featurePointPairsMap.size());

	const Scalar invStablePathLength = Numeric::ratio(1, Scalar(stablePathLength));

	for (Database::IdIdPointPairsMap::const_iterator i = featurePointPairsMap.begin(); i != featurePointPairsMap.end(); ++i)
	{
		Vectors2 imagePointsGroup;
		imagePoints.reserve(i->second.size());

		for (Database::IdPointPairs::const_iterator iI = i->second.begin(); imagePointsGroup.size() < maximalPathLength && iI != i->second.end(); ++iI)
		{
			imagePointsGroup.push_back(transformation * iI->second);
		}

		imagePointGroups.push_back(std::move(imagePointsGroup));

		factors.push_back(min(Scalar(i->second.size()) * invStablePathLength, Scalar(1)));
	}

	paintPaths<1u>(frame, imagePointGroups.data(), imagePointGroups.size(), colorImagePointsInstable, colorImagePointsStable, factors.data(), worker);
}

bool Utilities::writeCamera(const PinholeCamera& pinholeCamera, IO::OutputBitstream& outputStream)
{
	if (!outputStream.write<unsigned int>(pinholeCamera.width()))
	{
		return false;
	}

	if (!outputStream.write<unsigned int>(pinholeCamera.height()))
	{
		return false;
	}

	Scalar elements[8];
	pinholeCamera.copyElements(elements);
	for (unsigned int n = 0u; n < 8u; ++n)
	{
		if (!outputStream.write<double>(double(elements[n])))
		{
			return false;
		}
	}

	return true;
}

bool Utilities::readCamera(IO::InputBitstream& inputStream, PinholeCamera& pinholeCamera)
{
	unsigned int cameraWidth = 0u;
	if (!inputStream.read<unsigned int>(cameraWidth))
	{
		return false;
	}

	unsigned int cameraHeight = 0u;
	if (!inputStream.read<unsigned int>(cameraHeight))
	{
		return false;
	}

	Scalar cameraElements[8];
	for (unsigned int n = 0u; n < 8u; ++n)
	{
		double element = NumericD::minValue();
		if (!inputStream.read<double>(element))
		{
			return false;
		}

		cameraElements[n] = Scalar(element);
	}

	if (cameraWidth == 0u && cameraHeight == 0u && cameraElements[0] == 0.0 && cameraElements[1] == 0.0 && cameraElements[2] == 0.0 && cameraElements[3] == 0.0 && cameraElements[4] == 0.0 && cameraElements[5] == 0.0 && cameraElements[6] == 0.0 && cameraElements[7] == 0.0)
	{
		pinholeCamera = PinholeCamera();
	}
	else
	{
		pinholeCamera = PinholeCamera(cameraWidth, cameraHeight, cameraElements);
	}

	return true;
}

bool Utilities::writeDatabase(const Database& database, IO::OutputBitstream& outputStream)
{
	if (!outputStream.write<std::string>("OCN_TRACKING_DATABASE"))
	{
		return false;
	}

	constexpr unsigned int version = 1u;

	if (!outputStream.write<unsigned int>(version))
	{
		return false;
	}

	// write all camera poses

	HomogenousMatrices4 poses;
	const Indices32 poseIds = database.poseIds<false>(&poses);

	if (!outputStream.write<unsigned int>((unsigned int)(poseIds.size())))
	{
		return false;
	}

	if (!outputStream.write(poseIds.data(), sizeof(Index32) * poseIds.size()))
	{
		return false;
	}

	if (std::is_same<Scalar, double>::value)
	{
		if (!outputStream.write(poses.data(), sizeof(HomogenousMatrixD4) * poses.size()))
		{
			return false;
		}
	}
	else
	{
		HomogenousMatricesD4 posesD;
		posesD.reserve(poses.size());

		for (const HomogenousMatrix4& pose : poses)
		{
			posesD.emplace_back(pose);
		}

		if (!outputStream.write(posesD.data(), sizeof(HomogenousMatrixD4) * posesD.size()))
		{
			return false;
		}
	}


	// write all object points

	Vectors3 objectPoints;
	Scalars priorities;
	const Indices32 objectPointIds = database.objectPointIds<false>(&objectPoints, &priorities);

	if (!outputStream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!outputStream.write(objectPointIds.data(), sizeof(Index32) * objectPointIds.size()))
	{
		return false;
	}

	if (std::is_same<Scalar, double>::value)
	{
		if (!outputStream.write(objectPoints.data(), sizeof(VectorD3) * objectPoints.size()))
		{
			return false;
		}

		if (!outputStream.write(priorities.data(), sizeof(double) * priorities.size()))
		{
			return false;
		}
	}
	else
	{
		VectorsD3 objectPointsD;
		objectPointsD.reserve(objectPoints.size());

		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsD.emplace_back(objectPoint);
		}

		if (!outputStream.write(objectPointsD.data(), sizeof(VectorD3) * objectPointsD.size()))
		{
			return false;
		}

		std::vector<double> prioritiesD;
		prioritiesD.reserve(priorities.size());

		for (const Scalar& priority : priorities)
		{
			prioritiesD.emplace_back(priority);
		}

		if (!outputStream.write(prioritiesD.data(), sizeof(double) * prioritiesD.size()))
		{
			return false;
		}
	}


	// write all image points

	Vectors2 imagePoints;
	const Indices32 imagePointIds = database.imagePointIds<false>(&imagePoints);

	if (!outputStream.write<unsigned int>((unsigned int)(imagePointIds.size())))
	{
		return false;
	}

	if (!outputStream.write(imagePointIds.data(), sizeof(Index32) * imagePointIds.size()))
	{
		return false;
	}

	if (std::is_same<Scalar, double>::value)
	{
		if (!outputStream.write(imagePoints.data(), sizeof(VectorD2) * imagePoints.size()))
		{
			return false;
		}
	}
	else
	{
		VectorsD2 imagePointsD;
		imagePointsD.reserve(imagePoints.size());

		for (const Vector2& imagePoint : imagePoints)
		{
			imagePointsD.emplace_back(imagePoint);
		}

		if (!outputStream.write(imagePointsD.data(), sizeof(VectorD2) * imagePointsD.size()))
		{
			return false;
		}
	}


	// write the topology

	Indices32 topologyPoseIds;
	topologyPoseIds.reserve(imagePointIds.size());

	Indices32 topologyObjectPointIds;
	topologyObjectPointIds.reserve(imagePointIds.size());

	for (const Index32& imagePointId : imagePointIds)
	{
		const Index32 poseId = database.poseFromImagePoint<false>(imagePointId);
		const Index32 objectPointId = database.objectPointFromImagePoint<false>(imagePointId);

		topologyPoseIds.emplace_back(poseId);
		topologyObjectPointIds.emplace_back(objectPointId);
	}

	if (!outputStream.write(topologyPoseIds.data(), sizeof(Index32) * topologyPoseIds.size()))
	{
		return false;
	}

	if (!outputStream.write(topologyObjectPointIds.data(), sizeof(Index32) * topologyObjectPointIds.size()))
	{
		return false;
	}

	return true;
}

bool Utilities::readDatabase(IO::InputBitstream& inputStream, Database& database)
{
	std::string tag;
	if (!inputStream.read<std::string>(tag) || tag != "OCN_TRACKING_DATABASE")
	{
		return false;
	}

	unsigned int version = 0u;
	if (!inputStream.read<unsigned int>(version) || version != 1u)
	{
		return false;
	}

	// read all camera poses

	unsigned int numberPoses = 0u;
	if (!inputStream.read<unsigned int>(numberPoses))
	{
		return false;
	}

	Indices32 poseIds(numberPoses);
	if (!inputStream.read(poseIds.data(), sizeof(Index32) * poseIds.size()))
	{
		return false;
	}

	HomogenousMatricesD4 poses(numberPoses);
	if (!inputStream.read(poses.data(), sizeof(HomogenousMatrixD4) * poses.size()))
	{
		return false;
	}


	// write all object points

	unsigned int numberObjectPoints = 0u;
	if (!inputStream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	Indices32 objectPointIds(numberObjectPoints);
	if (!inputStream.read(objectPointIds.data(), sizeof(Index32) * objectPointIds.size()))
	{
		return false;
	}

	VectorsD3 objectPoints(numberObjectPoints);
	if (!inputStream.read(objectPoints.data(), sizeof(VectorD3) * objectPoints.size()))
	{
		return false;
	}

	std::vector<double> objectPointPriorities(numberObjectPoints);
	if (!inputStream.read(objectPointPriorities.data(), sizeof(double) * objectPointPriorities.size()))
	{
		return false;
	}


	// write all image points

	unsigned int numberImagePoints = 0u;
	if (!inputStream.read<unsigned int>(numberImagePoints))
	{
		return false;
	}

	Indices32 imagePointIds(numberImagePoints);
	if (!inputStream.read(imagePointIds.data(), sizeof(Index32) * imagePointIds.size()))
	{
		return false;
	}

	VectorsD2 imagePoints(numberImagePoints);
	if (!inputStream.read(imagePoints.data(), sizeof(VectorD2) * imagePoints.size()))
	{
		return false;
	}


	// read the topology

	Indices32 topologyPoseIds(numberImagePoints);
	if (!inputStream.read(topologyPoseIds.data(), sizeof(Index32) * topologyPoseIds.size()))
	{
		return false;
	}

	Indices32 topologyObjectPointIds(numberImagePoints);
	if (!inputStream.read(topologyObjectPointIds.data(), sizeof(Index32) * topologyObjectPointIds.size()))
	{
		return false;
	}

	database.reset<double, false>(poseIds.size(), poseIds.data(), poses.data(), objectPointIds.size(), objectPointIds.data(), objectPoints.data(), objectPointPriorities.data(), imagePointIds.size(), imagePointIds.data(), imagePoints.data(), topologyPoseIds.data(), topologyObjectPointIds.data());

	return true;
}

Maintenance::Buffer Utilities::encodeEnvironment(const Frame& frame, const Vectors2& frameImagePoints, const Vectors3& frameObjectPoints, const HomogenousMatrix4& framePose, const Vectors3& objectPoints)
{
	Maintenance::Buffer buffer;

	Media::Utilities::encodeFrame(frame, "jpg", buffer);
	IO::Utilities::encodeVectors2(frameImagePoints.data(), frameImagePoints.size(), buffer);
	IO::Utilities::encodeVectors3(frameObjectPoints.data(), frameObjectPoints.size(), buffer);
	IO::Utilities::encodeHomogenousMatrix4(framePose, buffer);
	IO::Utilities::encodeVectors3(objectPoints.data(), objectPoints.size(), buffer);

	return buffer;
}

}

}
