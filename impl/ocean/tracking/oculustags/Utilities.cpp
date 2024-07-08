/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/oculustags/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/oculustags/OculusTagTracker.h"

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	#include "ocean/base/WorkerPool.h"
	#include "ocean/cv/FrameConverterY8.h"
	#include "ocean/geometry/Homography.h"
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

void Utilities::drawQuad(Frame& frame, const QuadDetector::Quad& boundaryPattern, const uint8_t* color)
{
	ocean_assert(frame.isValid());

	const uint8_t* black = CV::Canvas::black(frame.pixelFormat());
	const uint8_t* green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* red = CV::Canvas::red(frame.pixelFormat());

	if (color == nullptr)
	{
		color = green;
	}

	for (size_t indexCurrent = 0; indexCurrent < 4; ++indexCurrent)
	{
		const size_t indexNext = (indexCurrent + 1) & 0b0011; // & 0b0011 ~ % 4

		CV::Canvas::line8BitPerChannel<3u, 3u>(frame.data<uint8_t>(), frame.width(), frame.height(), boundaryPattern[indexCurrent], boundaryPattern[indexNext], black, frame.paddingElements());
		CV::Canvas::line8BitPerChannel<3u, 1u>(frame.data<uint8_t>(), frame.width(), frame.height(), boundaryPattern[indexCurrent], boundaryPattern[indexNext], color, frame.paddingElements());
	}

	for (size_t indexCurrent = 0; indexCurrent < 4; ++indexCurrent)
	{
		CV::Canvas::point<3u>(frame, boundaryPattern[indexCurrent], black);
		CV::Canvas::point<1u>(frame, boundaryPattern[indexCurrent], red);
	}
}

void Utilities::drawQuads(Frame& frame, const QuadDetector::Quads& boundaryPatterns)
{
	ocean_assert(frame.isValid());

	for (const QuadDetector::Quad& boundaryPattern : boundaryPatterns)
	{
		drawQuad(frame, boundaryPattern);
	}
}

void Utilities::drawOculusTag(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, const uint8_t* colorOutline)
{
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>()));
	ocean_assert(anyCamera.isValid() && anyCamera.width() == frame.width() && anyCamera.height() == frame.height());
	ocean_assert(device_T_camera.isValid());

	const Scalar tagSize = tag.tagSize();
	ocean_assert(tagSize > 0);

	const uint8_t* black = CV::Canvas::black(frame.pixelFormat());
	const uint8_t* green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* red = CV::Canvas::red(frame.pixelFormat());

	if (colorOutline == nullptr)
	{
		colorOutline = green;
	}

	const Scalar moduleSize = tagSize / Scalar(OculusTag::numberOfModules);

	const HomogenousMatrix4 tag_T_camera = tag.world_T_tag().inverted() * world_T_device * device_T_camera;

	if (tag_T_camera.isValid() == false)
	{
		ocean_assert(false && "This should never happen");
		return;
	}

	// Draw the outline of the tag

	const HomogenousMatrix4 flippedCamera_T_tag = PinholeCamera::standard2InvertedFlipped(tag_T_camera);
	const Vectors3 cornerObjectPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_0_TO_3, tag.tagSize());

	Vectors2 cornerImagePoints;
	cornerImagePoints.reserve(cornerObjectPoints.size());

	for (const Vector3& cornerObjectPoint : cornerObjectPoints)
	{
		cornerImagePoints.emplace_back(anyCamera.projectToImageIF(flippedCamera_T_tag * cornerObjectPoint));
	}

	for (size_t i = 0; i < 4; ++i)
	{
		const size_t nextIndex = (i + 1) & 0b0011; // & 0b0011 ~ % 4

		CV::Canvas::line8BitPerChannel<3u, 5u>(frame.data<uint8_t>(), frame.width(), frame.height(), cornerImagePoints[i], cornerImagePoints[nextIndex], black, frame.paddingElements());
		CV::Canvas::line8BitPerChannel<3u, 3u>(frame.data<uint8_t>(), frame.width(), frame.height(), cornerImagePoints[i], cornerImagePoints[nextIndex], colorOutline, frame.paddingElements());
	}

	// Draw the modules of the data matrix

	OculusTag::DataMatrix dataMatrix = 0u;

	if (!encode(tag.tagID(), dataMatrix))
	{
		ocean_assert(false && "This should never happen!");
	}

	uint32_t i = 0u;
	for (uint32_t y = 2u; y < OculusTag::numberOfModules - 2u; ++y)
	{
		for (uint32_t x = 2u; x < OculusTag::numberOfModules - 2u; ++x)
		{
			const Vector2 moduleCenter(Scalar(x) + Scalar(0.5), -Scalar(y) - Scalar(0.5));

			const Vector2 imageModuleCenter = anyCamera.projectToImage(tag_T_camera, Vector3(moduleCenter * moduleSize, 0));
			const uint8_t binaryModuleValue = (dataMatrix >> (15u - i)) & 1u;

			CV::Canvas::point<5u>(frame, imageModuleCenter, black);
			CV::Canvas::point<3u>(frame, imageModuleCenter, binaryModuleValue == 1u ? red : green);

			++i;
		}
	}

	drawCoordinateSystem(frame, anyCamera, tag_T_camera, tagSize);
}

void Utilities::drawOculusTags(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTags& tags)
{
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>()));
	ocean_assert(anyCamera.isValid() && anyCamera.width() == frame.width() && anyCamera.height() == frame.height());

	for (const OculusTag& tag : tags)
	{
		drawOculusTag(frame, anyCamera, world_T_device, device_T_camera, tag);
	}
}

void Utilities::drawOculusTag(Frame& frameA, Frame& frameB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTag& tag)
{
	ocean_assert(frameA.isValid() && FrameType::arePixelFormatsCompatible(frameA.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>()));
	ocean_assert(frameA.frameType() == frameB.frameType());
	ocean_assert(anyCameraA.isValid() && anyCameraA.width() == frameA.width() && anyCameraA.height() == frameA.height());
	ocean_assert(anyCameraB.isValid() && anyCameraB.width() == frameB.width() && anyCameraB.height() == frameB.height());
	ocean_assert(device_T_cameraA.isValid());
	ocean_assert(device_T_cameraB.isValid());

	drawOculusTag(frameA, anyCameraA, world_T_device, device_T_cameraA, tag);
	drawOculusTag(frameB, anyCameraB, world_T_device, device_T_cameraB, tag);
}

void Utilities::drawOculusTags(Frame& frameA, Frame& frameB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTags& tags)
{
	ocean_assert(frameA.isValid() && FrameType::arePixelFormatsCompatible(frameA.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>()));
	ocean_assert(frameA.frameType() == frameB.frameType());
	ocean_assert(anyCameraA.isValid() && anyCameraA.width() == frameA.width() && anyCameraA.height() == frameA.height());
	ocean_assert(anyCameraB.isValid() && anyCameraB.width() == frameB.width() && anyCameraB.height() == frameB.height());
	ocean_assert(device_T_cameraA.isValid());
	ocean_assert(device_T_cameraB.isValid());

	for (const OculusTag& tag : tags)
	{
		drawOculusTag(frameA, frameB, anyCameraA, anyCameraB, world_T_device, device_T_cameraA, device_T_cameraB, tag);
	}
}

void Utilities::drawCoordinateSystem(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const Vector3& offset)
{
	const unsigned char* red = CV::Canvas::red(frame.pixelFormat());
	const unsigned char* green = CV::Canvas::green(frame.pixelFormat());
	const unsigned char* blue = CV::Canvas::blue(frame.pixelFormat());

	const Scalar axisLength = Scalar(5) * tagSize;

	const Vector3 center3d = offset;
	const Vector3 xAxis3d = offset + Vector3(axisLength, 0.00, 0.00);
	const Vector3 yAxis3d = offset + Vector3(0.00, axisLength, 0.00);
	const Vector3 zAxis3d = offset + Vector3(0.00, 0.00, axisLength);

	const Vector2 center2d = anyCamera.projectToImage(tag_T_camera, center3d);
	const Vector2 xAxis2d = anyCamera.projectToImage(tag_T_camera, xAxis3d);
	const Vector2 yAxis2d = anyCamera.projectToImage(tag_T_camera, yAxis3d);
	const Vector2 zAxis2d = anyCamera.projectToImage(tag_T_camera, zAxis3d);

	CV::Canvas::line<3u>(frame, center2d, xAxis2d, red);
	CV::Canvas::line<3u>(frame, center2d, yAxis2d, green);
	CV::Canvas::line<3u>(frame, center2d, zAxis2d, blue);
}

bool Utilities::drawOculusLogo(Frame& frame, const CV::PixelPosition& position, const uint32_t width, const uint8_t* color)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(frame.channels() <= 4u);
	ocean_assert(width != 0u);

	constexpr uint8_t defaultColor[4] = { 0u, 0u, 0u, 0u };

	if (color == nullptr)
	{
		color = defaultColor;
	}

	// Primitives of the Oculus logo

	const Scalar logoHeight = Scalar(width) * Scalar(0.60362);
	ocean_assert(logoHeight > 0);

	const Vector2 circleCenterLeft(Scalar(width) * Scalar(0.30181), Scalar(width) * Scalar(0.30181));
	const Vector2 circleCenterRight(Scalar(width) * Scalar(0.69181), circleCenterLeft.y());

	const Scalar diameter0 = Scalar(width) * Scalar(0.60632);
	const Scalar diameter1 = Scalar(width) * Scalar(0.20212);

	// Scale factor to map the above locations to pixels in a mask image

	const uint32_t height = uint32_t(std::ceil(logoHeight)); // TODO ceil still necessary?
	ocean_assert(height != 0u);

	// Currently, the drawing functions require integer-accurary. Also, the values of radii have to odd so drawing is slightly inaccurate at the moment (which is fine for now)
	// TODO Add subpixel support to CV::Canvas::ellipse() and CV::Canvas::rectangle(). Maybe a more general add CV::Canvas::quadrilateral()?!?

	if (position.x() + width > frame.width() || position.y() + height > frame.height())
	{
		ocean_assert(false && "Currently, the logo has to be completely inside the area of the image");
		return false;
	}

	const CV::PixelPosition circleCenterLeftI(uint32_t(circleCenterLeft.x()), uint32_t(circleCenterLeft.y()));
	const CV::PixelPosition circleCenterRightI(uint32_t(circleCenterRight.x()), uint32_t(circleCenterRight.y()));

	const uint32_t diameter0I = uint32_t(diameter0) | 0x01u; // Forcing odd values
	const uint32_t diameter1I = uint32_t(diameter1) | 0x01u;

	const VectorI2 rectangle0PositionI(circleCenterLeftI.x(), circleCenterLeftI.y() - (diameter0I / 2u));
	uint32_t rectangle0WidthI = circleCenterRightI.x() - circleCenterLeftI.x() + 1u;
	uint32_t rectangle0HeightI = diameter0I;

	const VectorI2 rectangle1PositionI(circleCenterLeftI.x(), circleCenterLeftI.y() - (diameter1I / 2u));
	uint32_t rectangle1WidthI = rectangle0WidthI;
	uint32_t rectangle1HeightI = diameter1I;

	// Create a grayscale mask into which the logo will be drawn before it's blended into the actual destination frame

	constexpr uint8_t maskBackgroundValue = 0x00u;
	constexpr uint8_t maskForegroundValue = 0xFFu;

	Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	mask.setValue(maskBackgroundValue);

	// Outer part of the logo

	CV::Canvas::rectangle8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), rectangle0PositionI.x(), rectangle0PositionI.y(), rectangle0WidthI, rectangle0HeightI, &maskForegroundValue, mask.paddingElements());
	CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), circleCenterLeftI, diameter0I, diameter0I, &maskForegroundValue, mask.paddingElements());
	CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), circleCenterRightI, diameter0I, diameter0I, &maskForegroundValue, mask.paddingElements());

	// Inner part of the logo

	CV::Canvas::rectangle8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), rectangle1PositionI.x(), rectangle1PositionI.y(), rectangle1WidthI, rectangle1HeightI, &maskBackgroundValue, mask.paddingElements());
	CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), circleCenterLeftI, diameter1I, diameter1I, &maskBackgroundValue, mask.paddingElements());
	CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), circleCenterRightI, diameter1I, diameter1I, &maskBackgroundValue, mask.paddingElements());

	// Use the mask to blend the logo into the specified frame

	for (uint32_t y = 0u; y < mask.height(); ++y)
	{
		const uint8_t* sourceRow = mask.constrow<uint8_t>(y);
		uint8_t* targetRow = frame.pixel<uint8_t>(position.x(), position.y() + y);

		for (uint32_t x = 0u; x < mask.width(); ++x)
		{
			const Scalar factor = Scalar(*sourceRow) * (Scalar(1) / Scalar(255));

			for (uint32_t c = 0u; c < frame.channels(); ++c)
			{
				*targetRow = uint8_t(Numeric::round32(factor * Scalar(color[c]) + (Scalar(1) - factor) * Scalar(*targetRow)));

				++targetRow;
			}

			++sourceRow;
		}
	}

	return true;
}

Frame Utilities::generateTagImage(const uint32_t& tagID, const OculusTag::ReflectanceType reflectanceType, const uint32_t width, const uint32_t borderModuleMultiple)
{
	if (tagID < 1024u && reflectanceType != OculusTag::RT_REFLECTANCE_UNDEFINED)
	{
		const uint8_t bit1Color = reflectanceType != OculusTag::RT_REFLECTANCE_NORMAL ? 0u : 255u;
		const uint8_t bit0Color = 255u - bit1Color;

		return generateTagImage<1u>(tagID, &bit0Color, &bit1Color, width, borderModuleMultiple);
	}

	return Frame();
}

template <uint8_t tChannels>
Frame Utilities::generateTagImage(const uint32_t& tagID, const uint8_t* bit1Color, const uint8_t* bit0Color, const uint32_t width, const uint32_t borderModuleMultiple)
{
	static_assert(tChannels >= 1u && tChannels <= 4u, "Number of channels must be in the range [1, 4]");
	ocean_assert(width >= OculusTag::numberOfModules);
	ocean_assert(bit1Color != nullptr && bit0Color != nullptr);

	if (tagID >= 1024u)
	{
		ocean_assert(false && "The tag ID must be < 1024");
		return Frame();
	}

	const Scalar moduleSize = Scalar(width) / Scalar(OculusTag::numberOfModules);
	ocean_assert(moduleSize > 0);

	const uint32_t frameSize = width + 2u * uint32_t(Numeric::round32(borderModuleMultiple * moduleSize));
	ocean_assert(frameSize >= width);

	const Scalar offset = borderModuleMultiple * moduleSize;

	Frame frame;

	OculusTag::DataMatrix dataMatrix;
	if (encode(tagID, dataMatrix))
	{
		// Initialize the frame

		if (!frame.set(FrameType(frameSize, frameSize, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), /* forceOwner */ true, /* forceWritable */ true))
		{
			ocean_assert(false && "This should never happen!");
			return Frame();
		}

		ocean_assert(frame.isContinuous());

		uint8_t* frameData = frame.data<uint8_t>();
		const uint8_t* frameDataEnd = frameData + frame.width() * frame.height() * frame.channels();

		while (frameData < frameDataEnd)
		{
			for (uint32_t iC = 0u; iC < tChannels; ++iC)
			{
				ocean_assert(frameData < frameDataEnd);
				*frameData = bit0Color[iC];

				++frameData;
			}
		}

		// Draw the modules

		const std::vector<uint8_t> modulesValues = OculusTagTracker::generateModuleValues(dataMatrix);

		for (uint32_t y = 0u; y < OculusTag::numberOfModules; ++y)
		{
			for (uint32_t x = 0u; x < OculusTag::numberOfModules; ++x)
			{
				const uint32_t moduleIndex = y * OculusTag::numberOfModules + x;
				const uint8_t* moduleColor = modulesValues[moduleIndex] == 1u ? bit1Color : bit0Color;

				const int32_t left = int32_t(offset + x * moduleSize);
				const int32_t top = int32_t(offset + y * moduleSize);

				CV::Canvas::rectangle(frame, left, top, uint32_t(Numeric::round32(moduleSize)), uint32_t(Numeric::round32(moduleSize)), moduleColor);
			}
		}

		// Draw the Oculus logo

		const uint32_t logoPositionX = uint32_t(Numeric::round32(offset + Scalar(moduleSize) * Scalar(3.5)));
		const uint32_t logoPositionY = uint32_t(Numeric::round32(offset + Scalar(moduleSize) * Scalar(0.2)));

		Tracking::OculusTags::Utilities::drawOculusLogo(frame, CV::PixelPosition(logoPositionX, logoPositionY), uint32_t(Numeric::round32(moduleSize)), bit0Color);
	}

	return frame;
}

bool Utilities::encode(const uint32_t tagID, OculusTag::DataMatrix& dataMatrix)
{
	if (tagID >= 1024u)
	{
		ocean_assert(false && "The tag ID must be < 1024");
		return false;
	}

	// Block length of the BCH codes in bits (must correspond to the total number of bits in the code)
	static constexpr uint32_t bchBlockLength = 16u;

	// Number of bits used in the BCH code to encode data, must be <= bchBlockLength
	static constexpr uint32_t bchDataLength = 10u;

	constexpr uint32_t bchShift = bchBlockLength - bchDataLength;

	// G(x) = x ^ 6 + x ^ 5 + x ^ 2 + x ~ 1100110.

	const uint32_t remainder = CV::Detector::QRCodes::QRCodeEncoder::computePolynomialDivisonRemainderBCH<bchBlockLength, bchDataLength, 0b1100110u>(tagID << bchShift);

	// Append the remainder to the format and XOR it with 1100110 TODO What's a good XOR-mask?
	const uint32_t encodedUnmasked = (tagID << bchShift) ^ remainder;
	const uint32_t encodedMasked = encodedUnmasked ^ 0b1101010000010010u;
	ocean_assert(encodedMasked >> bchBlockLength == 0u);

	dataMatrix = OculusTag::DataMatrix(encodedMasked);

	return true;
}

bool Utilities::decode(const OculusTag::DataMatrix& dataMatrix, uint32_t& tagID)
{
	// Number of bits used in the BCH code to encode data, must be <= bchBlockLength
	static constexpr uint32_t bchDataLength = 10u;

	const uint32_t encodedUnmasked = uint32_t(dataMatrix) ^ 0b1101010000010010u;

	uint32_t minDistanceData = uint32_t(-1);
	uint32_t minDistance = uint32_t(-1);
	uint32_t duplicates = 0u;

	constexpr uint16_t maxData = 1u << bchDataLength;
	for (uint16_t data = 0u; data < maxData; ++data)
	{
		OculusTag::DataMatrix reference;
		if (encode(data, reference) == false)
		{
			return false;
		}

		uint16_t referenceUnmasked = reference ^ 0b1101010000010010u;

		const uint32_t distance = CV::Detector::QRCodes::QRCodeEncoder::computeHammingWeight(uint32_t(encodedUnmasked ^ referenceUnmasked));

		if (distance < minDistance)
		{
			minDistance = distance;
			minDistanceData = data;
			duplicates = 0u;
		}
		else if (distance == minDistance)
		{
			duplicates++;
		}
	}

	// Check if the result is unambiguous, i.e. if at least two reference formats have the same Hamming distance the input format cannot be decoded unambiguously (>= 4 bits wrong).

	if (duplicates != 0u || minDistance >= 2u)
	{
		return false;
	}

	tagID = minDistanceData;
	ocean_assert(tagID < 1024u);

	return true;
}

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)

bool Utilities::debugCreateRectifiedFrameFromHomograpy(const Frame& frame, const QuadDetector::Quad& boundaryPattern, const uint32_t rectifiedTagImageWidth, const uint32_t rectifiedTagImageHeight, Frame& rectifiedFrame, const uint32_t extraBorder, SquareMatrix3* rectifiedFrame_H_boundaryPattern)
{
	ocean_assert(frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(rectifiedTagImageWidth != 0u && rectifiedTagImageHeight != 0u);


	const QuadDetector::Quad rectifiedPoints =
	{
		Vector2(extraBorder, extraBorder),
		Vector2(extraBorder, extraBorder + rectifiedTagImageHeight),
		Vector2(extraBorder + rectifiedTagImageWidth, extraBorder + rectifiedTagImageHeight),
		Vector2(extraBorder + rectifiedTagImageWidth, extraBorder),
	};

	SquareMatrix3 localRectifiedFrame_H_boundaryPattern;
	if (Geometry::Homography::homographyMatrixLinearWithOptimizations(rectifiedPoints.data(), boundaryPattern.data(), 4u, localRectifiedFrame_H_boundaryPattern) == false)
	{
		return false;
	}

	if (rectifiedFrame_H_boundaryPattern)
	{
		*rectifiedFrame_H_boundaryPattern = localRectifiedFrame_H_boundaryPattern;
	}

	rectifiedFrame = Frame(FrameType(frame, rectifiedTagImageWidth + 2u * extraBorder, rectifiedTagImageHeight + 2u * extraBorder));

	switch (frame.channels())
	{
		case 1u:
			CV::FrameInterpolatorBilinear::homography<uint8_t, 1u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), localRectifiedFrame_H_boundaryPattern, nullptr, rectifiedFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), rectifiedFrame.width(), rectifiedFrame.height(), frame.paddingElements(), rectifiedFrame.paddingElements(), WorkerPool::get().scopedWorker()());
			break;

		case 2u:
			CV::FrameInterpolatorBilinear::homography<uint8_t, 2u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), localRectifiedFrame_H_boundaryPattern, nullptr, rectifiedFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), rectifiedFrame.width(), rectifiedFrame.height(), frame.paddingElements(), rectifiedFrame.paddingElements(), WorkerPool::get().scopedWorker()());
			break;

		case 3u:
			CV::FrameInterpolatorBilinear::homography<uint8_t, 3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), localRectifiedFrame_H_boundaryPattern, nullptr, rectifiedFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), rectifiedFrame.width(), rectifiedFrame.height(), frame.paddingElements(), rectifiedFrame.paddingElements(), WorkerPool::get().scopedWorker()());
			break;

		case 4u:
			CV::FrameInterpolatorBilinear::homography<uint8_t, 4u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), localRectifiedFrame_H_boundaryPattern, nullptr, rectifiedFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), rectifiedFrame.width(), rectifiedFrame.height(), frame.paddingElements(), rectifiedFrame.paddingElements(), WorkerPool::get().scopedWorker()());
			break;

		default:
			ocean_assert(false && "This should never happen");
			return false;
	}

	return true;
}

bool Utilities::debugFisheyeImagePointsToRectifiedImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Vectors3& tagObjectCorners, const Vectors2& rectifiedImageCorners, const Vectors2& fisheyeImagePoints, Vectors2& rectifiedImagePoints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(tag_T_camera.isValid());
	ocean_assert(tagObjectCorners.size() == 4);
	ocean_assert(rectifiedImageCorners.size() == 4);

	const Triangle2 rectifiedImageTriangles[2] =
	{
		Triangle2(rectifiedImageCorners[0], rectifiedImageCorners[1], rectifiedImageCorners[2]),
		Triangle2(rectifiedImageCorners[0], rectifiedImageCorners[2], rectifiedImageCorners[3])
	};

	const Triangle3 objectTriangles[2] =
	{
		Triangle3(tagObjectCorners[0], tagObjectCorners[1], tagObjectCorners[2]),
		Triangle3(tagObjectCorners[0], tagObjectCorners[2], tagObjectCorners[3])
	};

	rectifiedImagePoints.resize(fisheyeImagePoints.size());
	for (size_t n = 0; n < fisheyeImagePoints.size(); ++n)
	{
		const Line3 ray = anyCamera.ray(fisheyeImagePoints[n], tag_T_camera);

		Vector3 barycentric0;
		Vector3 barycentric1;
		if (!objectTriangles[0].planeIntersection(ray, barycentric0) || !objectTriangles[1].planeIntersection(ray, barycentric1))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		rectifiedImagePoints[n] = (rectifiedImageTriangles[0].barycentric2cartesian(barycentric0) + rectifiedImageTriangles[1].barycentric2cartesian(barycentric1)) * Scalar(0.5);
	}

	return true;
}

bool Utilities::debugRectifiedImagePointsToFisheyeImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Vectors3& tagObjectCorners, const Vectors2& rectifiedImageCorners, const Vectors2& rectifiedImagePoints, Vectors2& fisheyeImagePoints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(tag_T_camera.isValid());
	ocean_assert(tagObjectCorners.size() == 4);
	ocean_assert(rectifiedImageCorners.size() == 4);

	const Triangle2 rectifiedImageTriangles[2] =
	{
		Triangle2(rectifiedImageCorners[0], rectifiedImageCorners[1], rectifiedImageCorners[2]),
		Triangle2(rectifiedImageCorners[0], rectifiedImageCorners[2], rectifiedImageCorners[3])
	};

	const Triangle3 objectTriangles[2] =
	{
		Triangle3(tagObjectCorners[0], tagObjectCorners[1], tagObjectCorners[2]),
		Triangle3(tagObjectCorners[0], tagObjectCorners[2], tagObjectCorners[3])
	};

	fisheyeImagePoints.resize(rectifiedImagePoints.size());
	for (size_t n = 0; n < rectifiedImagePoints.size(); ++n)
	{
		Vector3 barycentric0 = rectifiedImageTriangles[0].cartesian2barycentric(rectifiedImagePoints[n]);
		Vector3 barycentric1 = rectifiedImageTriangles[1].cartesian2barycentric(rectifiedImagePoints[n]);

		const Vector3 fisheyeObjectPoint = (objectTriangles[0].barycentric2cartesian(barycentric0) + objectTriangles[1].barycentric2cartesian(barycentric1)) * Scalar(0.5);
		fisheyeImagePoints[n] = anyCamera.projectToImage(tag_T_camera, fisheyeObjectPoint);
	}

	return true;
}

#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

bool Utilities::serializeOculusTags(const OculusTags& tags, std::string& buffer)
{
	if (tags.empty())
	{
		buffer = "";
		return false;
	}

	std::ostringstream stream;

	// Add an identifier for the version of the format
	stream << "v1 ";

	for (const OculusTag& tag : tags)
	{
		if (!tag.isValid())
		{
			ocean_assert(false && "This should never happen");
			continue;
		}

		// 1. tag ID
		stream << int(tag.tagID()) << " ";

		// 2. reflectance type
		switch (tag.reflectanceType())
		{
			case OculusTag::RT_REFLECTANCE_NORMAL:
				stream << "RT_REFLECTANCE_NORMAL";
				break;

			case OculusTag::RT_REFLECTANCE_INVERTED:
				stream << "RT_REFLECTANCE_INVERTED";
				break;

			default:
				stream << "RT_REFLECTANCE_UNDEFINED";
				break;
		}

		// 3. intensity threshold
		stream << " " << int(tag.intensityThreshold()) << " ";

		// 4. - 19. world_T_tag
		const HomogenousMatrix4& world_T_tag = tag.world_T_tag();

		for (unsigned int i = 0u; i < 16u; ++i)
		{
			stream << String::toAString(world_T_tag[i], 10u) << " ";
		}

		// intentionally no space added after world_T_tag (that was already done in the loop above)

		// 20. tag size
		stream << String::toAString(tag.tagSize(), 10u) << " ";
	}

	buffer = stream.str();

	return true;
}

bool Utilities::deserializeOculusTags(const std::string& buffer, OculusTags& tags)
{
	tags.clear();

	if (buffer.empty())
	{
		return false;
	}

	std::istringstream stream(buffer);

	bool succeeded = true;

	if (stream.good())
	{
		std::string versionString;
		stream >> versionString;

		if (String::toLower(versionString) != "v1")
		{
			ocean_assert(false && "Invalid version");
			return false;
		}
	}

	while (stream.good())
	{
		std::string tokens[20];

		for (size_t t = 0; t < 20; ++t)
		{
			stream >> tokens[t];
			if (stream.eof() || stream.fail())
			{
				if (t == 0 && stream.eof())
				{
					// It's fine if the stream ends while reading the first element, otherwise that would indicate an invalid stream
					break;
				}

				succeeded = false;
				break;
			}
		}

		if (!succeeded || stream.eof())
		{
			break;
		}

		double value;

		// 1. tag ID
		if (!String::isNumber(tokens[0], /* acceptInteger */ true, &value))
		{
			succeeded = false;
			break;
		}

		ocean_assert(value >= 0 && value < 1024.0);
		const uint32_t tagID = uint32_t(value);

		// 2. reflectance type
		OculusTag::ReflectanceType reflectanceType;

		if (tokens[1] == "RT_REFLECTANCE_NORMAL")
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_NORMAL;
		}
		else if (tokens[1] == "RT_REFLECTANCE_INVERTED")
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_INVERTED;
		}
		else
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_UNDEFINED;
		}

		if (reflectanceType == OculusTag::RT_REFLECTANCE_UNDEFINED)
		{
			succeeded = false;
			break;
		}

		// 3. intensity threshold
		if (!String::isNumber(tokens[2], /* acceptInteger */ true, &value))
		{
			succeeded = false;
			break;
		}

		ocean_assert(value >= 0 && value < 256.0);
		const uint8_t intensityThreshold = uint8_t(value);

		// 4. - 19. world_T_tag
		HomogenousMatrix4 world_T_tag(false);

		for (unsigned int i = 0u; i < 16u; ++i)
		{
			if (!String::isNumber(tokens[3u + i], /* acceptInteger */ true, &value))
			{
				world_T_tag = HomogenousMatrix4(false);
				succeeded = false;
				break;
			}

			world_T_tag[i] = Scalar(value);
		}

		if (!world_T_tag.isValid())
		{
			succeeded = false;
			break;
		}

		// 20. tag size
		if (!String::isNumber(tokens[19], /* acceptInteger */ true, &value))
		{
			succeeded = false;
			break;
		}

		const Scalar tagSize = Scalar(value);
		ocean_assert(tagSize > 0);

		tags.emplace_back(tagID, reflectanceType, intensityThreshold, world_T_tag, tagSize);
	}

	if (!succeeded)
	{
		tags.clear();
	}

	return succeeded;
}

bool Utilities::serializeTagSizeMap(const std::unordered_map<uint32_t, Scalar>& tagSizeMap, const Scalar defaultTagSize, std::string& buffer)
{
	ocean_assert(defaultTagSize > 0);

	// The format of the serialized data is (delimiter is a single space ' '):
	// VERSION DEFAULT_TAG_SIZE TAG_ID0 TAG_SIZE0 TAG_ID1 TAG_SIZE1 ...

	buffer = "V1";
	buffer += " ";
	buffer += String::toAString(defaultTagSize, 10u);

	for (const std::unordered_map<uint32_t, Scalar>::value_type& iter : tagSizeMap)
	{
		buffer += " " + String::toAString(iter.first) + " " + String::toAString(iter.second, 10u);
	}

	return true;
}

bool Utilities::deserializeTagSizeMap(const std::string& buffer, std::unordered_map<uint32_t, Scalar>& tagSizeMap, Scalar& defaultTagSize)
{
	tagSizeMap.clear();

	if (buffer.empty())
	{
		return false;
	}

	// The format of the serialized data is (delimiter is a single space ' '):
	// VERSION DEFAULT_TAG_SIZE TAG_ID0 TAG_SIZE0 TAG_ID1 TAG_SIZE1 ...

	std::istringstream stream(buffer);

	bool succeeded = true;

	if (stream.good())
	{
		std::string versionString;
		stream >> versionString;

		if (versionString != "V1")
		{
			ocean_assert(false && "Invalid version found in serialized data!");
			return false;
		}

		if (!stream.good())
		{
			ocean_assert(false && "Invalid serialized data!");
			return false;
		}

		std::string defaultTagSizeString;
		stream >> defaultTagSizeString;

		double value;
		if (!String::isNumber(defaultTagSizeString, /* acceptInteger */ true, &value))
		{
			ocean_assert(false && "Invalid serialized data!");
			return false;
		}

		ocean_assert(value > 0);
		defaultTagSize = Scalar(value);
	}

	while (stream.good())
	{
		std::string tokens[2];

		for (size_t t = 0; t < 2; ++t)
		{
			stream >> tokens[t];

			if (stream.eof())
			{
				break;
			}
			else if (stream.fail())
			{
				succeeded = false;
				break;
			}
		}

		double value;

		// 1. tag ID
		if (!String::isNumber(tokens[0], /* acceptInteger */ true, &value))
		{
			succeeded = false;
			break;
		}

		ocean_assert(value >= 0 && value < 1024.0);
		const uint32_t tagID = uint32_t(value);

		// 2. tag size
		if (!String::isNumber(tokens[1], /* acceptInteger */ true, &value))
		{
			succeeded = false;
			break;
		}

		ocean_assert(value > 0);
		const Scalar tagSize = Scalar(value);

		bool addedToMap;
		std::tie(std::ignore, addedToMap) = tagSizeMap.emplace(tagID, tagSize);

		if (!addedToMap)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	return succeeded;
}

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean
