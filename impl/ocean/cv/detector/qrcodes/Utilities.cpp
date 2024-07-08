/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/AlignmentPatternDetector.h"
#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

Utilities::CoordinateSystem::CoordinateSystem(const unsigned int version, const Scalar scale) :
	scale_(scale)
{
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(scale_ > Scalar(0));

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);
	ocean_assert(modulesPerSide >= 21u);

	xScale_ = (Scalar(2) * scale_) / Scalar(modulesPerSide);
	yScale_ = -xScale_;
}

Frame Utilities::draw(const QRCode& code, const unsigned int border, const uint8_t foregroundColor, const uint8_t backgroundColor)
{
	ocean_assert(std::abs(int(foregroundColor) - int(backgroundColor)) >= 30);

	if (!code.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	const std::vector<uint8_t>& modules = code.modules();
	const unsigned int modulesPerSide = code.modulesPerSide();
	const unsigned int frameSize = modulesPerSide + 2u * border;

	Frame frame(FrameType(frameSize, frameSize, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	ocean_assert(frame.isValid());
	frame.setValue(backgroundColor);

	for (unsigned int y = 0u; y < modulesPerSide; ++y)
	{
		ocean_assert(y + border < frame.height());
		uint8_t* const frameRow = frame.row<uint8_t>(y + border);

		for (unsigned int x = 0u; x < modulesPerSide; ++x)
		{
			ocean_assert(x + border < frame.width());

			// 1-bit = black, 0-bit = white
			frameRow[border + x] = modules[y * modulesPerSide + x] == 0u ? backgroundColor : foregroundColor;
		}
	}

	return frame;
}

Frame Utilities::draw(const QRCode& code, const unsigned int frameSize, const bool allowTrueMultiple, const unsigned int border, Worker* worker, const uint8_t foregroundColor, const uint8_t backgroundColor)
{
	ocean_assert(std::abs(int(foregroundColor) - int(backgroundColor)) >= 30);

	if (!code.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	const unsigned int modulesPerSide = code.modulesPerSide();

	if (frameSize < modulesPerSide + 2u * border)
	{
		ocean_assert(false && "The frame size must equal or larger than the number of modules per side + twice the border");
		return Frame();
	}

	const Frame unscaledFrame = draw(code, border, foregroundColor, backgroundColor);
	ocean_assert(unscaledFrame.isValid());

	unsigned int finalFrameSize = frameSize;

	if (allowTrueMultiple)
	{
		const unsigned int remainder = frameSize % unscaledFrame.width();

		if (remainder != 0u)
		{
			finalFrameSize = frameSize + unscaledFrame.width() - remainder;
			ocean_assert(finalFrameSize > frameSize && (finalFrameSize % unscaledFrame.width() == 0u));
		}
	}

	ocean_assert(finalFrameSize >= unscaledFrame.width());

	Frame scaledFrame(FrameType(unscaledFrame, finalFrameSize, finalFrameSize));

	ocean_assert(FrameType::arePixelFormatsCompatible(scaledFrame.pixelFormat(), FrameType::FORMAT_Y8));
	CV::FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(unscaledFrame.constdata<uint8_t>(), scaledFrame.data<uint8_t>(), unscaledFrame.width(), unscaledFrame.height(), scaledFrame.width(), scaledFrame.height(), unscaledFrame.paddingElements(), scaledFrame.paddingElements(), worker);

	return scaledFrame;
}

void Utilities::drawObservation(Frame& frame, const SquareMatrix3& frame_H_code, const FinderPatternTriplet& finderPatterns, const unsigned int version, const std::vector<uint8_t>& modules)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame_H_code.isHomography());
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(modules.size() == (QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)));

	const uint8_t* const red = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const yellow = CV::Canvas::yellow(frame.pixelFormat());

	// First we draw all detected finder patterns (also those not belonging to a valid code)

	for (size_t i = 0; i < 3; ++i)
	{
		drawFinderPattern(frame, finderPatterns[i], green);
	}

	// Draw the modules of the QR code
	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);

	for (unsigned int y = 0u; y < modulesPerSide; ++y)
	{
		for (unsigned int x = 0u; x < modulesPerSide; ++x)
		{
			const Vector2 moduleImageLocation = frame_H_code * Vector2(Scalar(x) + Scalar(0.5), Scalar(y) + Scalar(0.5));
			const uint8_t* moduleColor = modules[y * modulesPerSide + x] == 1u ? green : red;
			CV::Canvas::point<3u>(frame, moduleImageLocation, moduleColor);
		}
	}

	// Draw the alignment patterns
	const VectorsI2 alignmentPatterns = QRCodeEncoder::computeAlignmentPatternPositions(version);

	for (const VectorI2& alignmentPattern : alignmentPatterns)
	{
		CV::Canvas::point<7u>(frame, frame_H_code * (Vector2(alignmentPattern) + Vector2(0.5, 0.5)), yellow);
	}

	// Draw a contour around the detected QR code
	const Vector2 qrcodeCorners[4] =
	{
		Vector2(0, 0),
		Vector2(0, Scalar(modulesPerSide)),
		Vector2(Scalar(modulesPerSide), Scalar(modulesPerSide)),
		Vector2(Scalar(modulesPerSide), 0)
	};

	Vector2 qrcodeCornerImageLocations[4];
	bool mappingSuccessful = true;
	for (size_t i = 0; i < 4; ++i)
	{
		if (!frame_H_code.multiply(qrcodeCorners[i], qrcodeCornerImageLocations[i]))
		{
			ocean_assert(false && "This should never happen");
			mappingSuccessful = false;
		};
	}

	if (mappingSuccessful)
	{
		CV::Canvas::line<1u>(frame, qrcodeCornerImageLocations[0].x(), qrcodeCornerImageLocations[0].y(), qrcodeCornerImageLocations[1].x(), qrcodeCornerImageLocations[1].y(), green);
		CV::Canvas::line<1u>(frame, qrcodeCornerImageLocations[1].x(), qrcodeCornerImageLocations[1].y(), qrcodeCornerImageLocations[2].x(), qrcodeCornerImageLocations[2].y(), green);
		CV::Canvas::line<1u>(frame, qrcodeCornerImageLocations[2].x(), qrcodeCornerImageLocations[2].y(), qrcodeCornerImageLocations[3].x(), qrcodeCornerImageLocations[3].y(), green);
		CV::Canvas::line<1u>(frame, qrcodeCornerImageLocations[3].x(), qrcodeCornerImageLocations[3].y(), qrcodeCornerImageLocations[0].x(), qrcodeCornerImageLocations[0].y(), green);
	}
}

void Utilities::drawObservations(const AnyCamera& anyCamera, Frame& frame, const QRCodeDetector2D::Observations& observations, const QRCodes& codes)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24) && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(observations.size() == codes.size());

	const uint8_t* color = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* foregroundColor = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* backgroundColor = CV::Canvas::green(frame.pixelFormat());

	typedef bool (*DrawPointFunc)(Frame& frame, const Vector2& position, const uint8_t* value);
	DrawPointFunc drawPointFunc = frame.width() >= 2000u ? CV::Canvas::point<9u> : CV::Canvas::point<5u>;

	for (size_t i = 0; i < observations.size(); ++i)
	{
		const QRCodeDetector2D::Observation& observation = observations[i];
		const QRCode& code = codes[i];

		ocean_assert(observation.isValid());
		ocean_assert(code.isValid());

		const HomogenousMatrix4 flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(observation.code_T_camera());
		ocean_assert(flippedCamera_T_code.isValid());

		const unsigned int version = code.version();
		const unsigned int modulesPerSide = QRCode::modulesPerSide(version);
		ocean_assert(modulesPerSide >= 21u);

		const Utilities::CoordinateSystem coordinateSystem(version);

		// Draw the modules
		const std::vector<uint8_t>& modules = code.modules();
		ocean_assert(modules.size() == modulesPerSide * modulesPerSide);

		for (unsigned int yModule = 0u; yModule < modulesPerSide; ++yModule)
		{
			const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(yModule) + Scalar(0.5));
			ocean_assert(y > Scalar(-1) && y < Scalar(1));

			for (unsigned int xModule = 0u; xModule < modulesPerSide; ++xModule)
			{
				const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(xModule) + Scalar(0.5));
				ocean_assert(x > Scalar(-1) && x < Scalar(1));

				const Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(x, y, Scalar(0)));

				const unsigned int moduleIndex = yModule * modulesPerSide + xModule;
				const uint8_t* moduleColor = modules[moduleIndex] == 0u ? backgroundColor : foregroundColor;

				drawPointFunc(frame, imagePoint, moduleColor);
			}
		}

		const Vectors3 objectCornerPoints = Utilities::CoordinateSystem::computeCornersInObjectSpace();
		ocean_assert(objectCornerPoints.size() == 4);

		const Vector2 imageCornerPoints[4] =
		{
			anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[0]),
			anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[1]),
			anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[2]),
			anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[3]),
		};

		// Draw the outline
		drawLine<5u>(anyCamera, frame, imageCornerPoints[0], imageCornerPoints[1], color);
		drawLine<5u>(anyCamera, frame, imageCornerPoints[1], imageCornerPoints[2], color);
		drawLine<5u>(anyCamera, frame, imageCornerPoints[2], imageCornerPoints[3], color);
		drawLine<5u>(anyCamera, frame, imageCornerPoints[3], imageCornerPoints[0], color);

		// Draw the centers of the finder patterns - their centers is 3.5 modules away from the border of the code; normalize that offset to range [0, 2]
		const Vectors3 objectFinderPatternCenters = Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(version);

		for (size_t iPoint = 0; iPoint < 3; ++iPoint)
		{
			CV::Canvas::point<9u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectFinderPatternCenters[iPoint]), color);
		}

		// Draw the horizontal and vertical timing patterns
		ocean_assert(modulesPerSide >= 21u);
		const unsigned int timingPatternLength = modulesPerSide - 14u;

		const Scalar normalizedModuleSize = Scalar(2) / Scalar(modulesPerSide);

		for (const bool isHorizontal : {true, false})
		{
			Vector3 objectTimingPatternStart;
			Vector3 objectTimingPatternEnd;

			Vector3 objectTimingPatternStep;

			if (isHorizontal)
			{
				objectTimingPatternStart = objectFinderPatternCenters[0] + Vector3(Scalar(4) * normalizedModuleSize, Scalar(-3) * normalizedModuleSize, Scalar(0));
				objectTimingPatternEnd = objectFinderPatternCenters[2] + Vector3(Scalar(-4) * normalizedModuleSize, Scalar(-3) * normalizedModuleSize, Scalar(0));

				objectTimingPatternStep = Vector3(normalizedModuleSize, Scalar(0), Scalar(0));
			}
			else
			{
				objectTimingPatternStart = objectFinderPatternCenters[0] + Vector3(Scalar(3) * normalizedModuleSize, Scalar(-4) * normalizedModuleSize, Scalar(0));
				objectTimingPatternEnd = objectFinderPatternCenters[1] + Vector3(Scalar(3) * normalizedModuleSize, Scalar(4) * normalizedModuleSize, Scalar(0));

				objectTimingPatternStep = Vector3(Scalar(0), -normalizedModuleSize, Scalar(0));
			}

			const Vector2 imageTimingPatternStart = anyCamera.projectToImageIF(flippedCamera_T_code, objectTimingPatternStart);
			const Vector2 imageTimingPatternEnd = anyCamera.projectToImageIF(flippedCamera_T_code, objectTimingPatternEnd);

			drawLine<1u>(anyCamera, frame, imageTimingPatternStart, imageTimingPatternEnd, color);

			Vector3 objectTimerPatternPoint = objectTimingPatternStart;
			for (size_t iPoint = 0; iPoint < timingPatternLength; ++iPoint)
			{
				CV::Canvas::point<3u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectTimerPatternPoint), color);

				objectTimerPatternPoint += objectTimingPatternStep;
			}
		}

		// Draw the alignment patterns
		const std::vector<Vectors3> objectAlignmentPatterns = Utilities::CoordinateSystem::computeAlignmentPatternsInObjectSpace(code.version());

		for (const Vectors3& objectAlignmentPatternsRow : objectAlignmentPatterns)
		{
			for (const Vector3& objectAlignmentPattern : objectAlignmentPatternsRow)
			{
				CV::Canvas::point<9u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectAlignmentPattern), color);
			}
		}
	}
}

void Utilities::drawFinderPattern(Frame& frame, const FinderPattern& finderPattern, const uint8_t* color)
{
	ocean_assert(frame.isValid());

	const uint8_t* const yellow = CV::Canvas::yellow(frame.pixelFormat());

	const Vector2 center = finderPattern.position();
	CV::Canvas::point<7u>(frame, center, yellow);

	// Draw the edges and the corners of the finder pattern
	if (finderPattern.cornersKnown())
	{
		const Vector2* corners = finderPattern.corners();
		ocean_assert(corners != nullptr);

		for (unsigned int i = 0u; i < 4u; ++i)
		{
			CV::Canvas::line<1u>(frame, corners[i].x(), corners[i].y(), corners[(i + 1u) % 4u].x(), corners[(i + 1u) % 4u].y(), color);
		}

		for (unsigned int i = 0u; i < 4u; ++i)
		{
			CV::Canvas::point<3u>(frame, corners[i], color);
		}
	}
}

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)

void Utilities::drawQRCodeOutline(const AnyCamera& anyCamera, Frame& frame, const unsigned int version, const HomogenousMatrix4& code_T_camera)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(frame.isValid() && FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24) && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(code_T_camera.isValid());

	const uint8_t* color = CV::Canvas::red(frame.pixelFormat());

	const HomogenousMatrix4 flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(code_T_camera);

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);

	const Scalar normalizedModuleSize = Scalar(2) / Scalar(modulesPerSide);

	const Vectors3 objectCornerPoints = Utilities::CoordinateSystem::computeCornersInObjectSpace();
	ocean_assert(objectCornerPoints.size() == 4);

	const Vector2 imageCornerPoints[4] =
	{
		anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[0]),
		anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[1]),
		anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[2]),
		anyCamera.projectToImageIF(flippedCamera_T_code, objectCornerPoints[3]),
	};

	Utilities::drawLine<1u>(anyCamera, frame, imageCornerPoints[0], imageCornerPoints[2], color);
	Utilities::drawLine<1u>(anyCamera, frame, imageCornerPoints[1], imageCornerPoints[3], color);

	// Draw the outline
	Utilities::drawLine<3u>(anyCamera, frame, imageCornerPoints[0], imageCornerPoints[1], color);
	Utilities::drawLine<3u>(anyCamera, frame, imageCornerPoints[1], imageCornerPoints[2], color);
	Utilities::drawLine<3u>(anyCamera, frame, imageCornerPoints[2], imageCornerPoints[3], color);
	Utilities::drawLine<3u>(anyCamera, frame, imageCornerPoints[3], imageCornerPoints[0], color);

	// Draw the four outer corners
	CV::Canvas::point<5u>(frame, imageCornerPoints[0], color);
	CV::Canvas::point<5u>(frame, imageCornerPoints[1], color);
	CV::Canvas::point<5u>(frame, imageCornerPoints[2], color);
	CV::Canvas::point<5u>(frame, imageCornerPoints[3], color);

	// Draw the centers of the finder patterns
	const Vectors3 objectFinderPatternCenters = Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(version);

	for (size_t i = 0; i < 3; ++i)
	{
		CV::Canvas::point<9u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectFinderPatternCenters[i]), color);
	}

	// Draw the center of the code
	CV::Canvas::point<11u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(0, 0, 0)), color);

	// Draw the horizontal and vertical timing patterns
	ocean_assert(modulesPerSide >= 21u);
	const unsigned int timingPatternLength = modulesPerSide - 14u;

	for (const bool isHorizontal : {true, false})
	{
		Vector3 objectTimingPatternStart;
		Vector3 objectTimingPatternEnd;

		Vector3 objectTimingPatternStep;

		if (isHorizontal)
		{
			objectTimingPatternStart = objectFinderPatternCenters[0] + Vector3(Scalar(4) * normalizedModuleSize, Scalar(-3) * normalizedModuleSize, Scalar(0));
			objectTimingPatternEnd = objectFinderPatternCenters[2] + Vector3(Scalar(-4) * normalizedModuleSize, Scalar(-3) * normalizedModuleSize, Scalar(0));

			objectTimingPatternStep = Vector3(normalizedModuleSize, Scalar(0), Scalar(0));
		}
		else
		{
			objectTimingPatternStart = objectFinderPatternCenters[0] + Vector3(Scalar(3) * normalizedModuleSize, Scalar(-4) * normalizedModuleSize, Scalar(0));
			objectTimingPatternEnd = objectFinderPatternCenters[1] + Vector3(Scalar(3) * normalizedModuleSize, Scalar(4) * normalizedModuleSize, Scalar(0));

			objectTimingPatternStep = Vector3(Scalar(0), -normalizedModuleSize, Scalar(0));
		}

		const Vector2 imageTimingPatternStart = anyCamera.projectToImageIF(flippedCamera_T_code, objectTimingPatternStart);
		const Vector2 imageTimingPatternEnd = anyCamera.projectToImageIF(flippedCamera_T_code, objectTimingPatternEnd);

		Utilities::drawLine<1u>(anyCamera, frame, imageTimingPatternStart, imageTimingPatternEnd, color);

		Vector3 objectTimerPatternPoint = objectTimingPatternStart;
		for (size_t i = 0; i < timingPatternLength; ++i)
		{
			CV::Canvas::point<3u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectTimerPatternPoint), color);

			objectTimerPatternPoint += objectTimingPatternStep;
		}
	}

	// Draw the alignment patterns
	const std::vector<Vectors3> objectAlignmentPatterns = Utilities::CoordinateSystem::computeAlignmentPatternsInObjectSpace(version);

	for (const Vectors3& objectAlignmentPatternsRow : objectAlignmentPatterns)
	{
		for (const Vector3& objectAlignmentPattern : objectAlignmentPatternsRow)
		{
			CV::Canvas::point<9u>(frame, anyCamera.projectToImageIF(flippedCamera_T_code, objectAlignmentPattern), color);
		}
	}
}

#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

std::string Utilities::toString(const QRCode& code, const unsigned int border)
{
	if (!code.isValid())
	{
		return std::string();
	}

	const std::vector<uint8_t>& modules = code.modules();
	const unsigned int modulesPerSide = code.modulesPerSide();

	std::ostringstream oss;

	for (unsigned int b = 0u; b < border; ++b)
	{
		oss << "\n";
	}

	for (unsigned int y = 0u; y < modulesPerSide; ++y)
	{
		// Using a 2:1 ratio for horizontal and vertical offsets
		oss << std::string(2u * border, ' ');

		for (unsigned int x = 0u; x < modulesPerSide; ++x)
		{
			oss << (modules[y * modulesPerSide + x] ? "##" : "  ");
		}

		oss << "\n";
	}

	for (unsigned int b = 0u; b < border; ++b)
	{
		oss << "\n";
	}

	return oss.str();
}

Vectors3 Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(const unsigned int version, const Scalar scale)
{
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(scale > Scalar(0));

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);
	ocean_assert(modulesPerSide >= 21u);

	const Scalar normalizedModuleSize = Scalar(2) / Scalar(modulesPerSide);

	// The centers of the finder patterns are 3.5 modules away from the border of the code.
	const Scalar finderPatternOffset = Scalar(3.5) * normalizedModuleSize;

	const Vectors3 objectPoints =
	{
		Vector3(scale * (-Scalar(1) + finderPatternOffset), scale * (Scalar(1) - finderPatternOffset), Scalar(0)), // top-left
		Vector3(scale * (-Scalar(1) + finderPatternOffset), scale * (-Scalar(1) + finderPatternOffset), Scalar(0)), // bottom-left
		Vector3(scale * (Scalar(1) - finderPatternOffset), scale * (Scalar(1) - finderPatternOffset), Scalar(0)), // top-right
	};

	return objectPoints;
}

std::vector<Vectors3> Utilities::CoordinateSystem::computeAlignmentPatternsInObjectSpace(const unsigned int version, const Scalar scale)
{
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(scale > Scalar(0));

	if (version == 1u)
	{
		return std::vector<Vectors3>();
	}

	// Locations of the alignment patterns as defined in ISO/IEC 18004:2015(E), Annex E
	//
	// Alignment patterns are distributed symmetrically around the axis that points from
	// the top-left finder pattern to the bottom-right corner, i.e. for each pattern at
	// (x, y) there is also one at (y, x).
	//
	// Alignment patterns that would overlap with the location of any of the finder
	// patterns are ignored.

	// clang-format off
	//                                                   v1,  v2,  v3,  v4,  v5,  v6,  v7,  v8,  v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40
	constexpr uint8_t numberAlignmentPatternAxes[40] = { 0u,  2u,  2u,  2u,  2u,  2u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  4u,  4u,  4u,  4u,  4u,  4u,  4u,  5u,  5u,  5u,  5u,  5u,  5u,  5u,  6u,  6u,  6u,  6u,  6u,  6u,  6u,  7u,  7u,  7u,  7u,  7u,  7u };
#if defined(OCEAN_DEBUG)
	constexpr uint8_t numberAlignmentPatterns[40]    = { 0u,  1u,  1u,  1u,  1u,  1u,  6u,  6u,  6u,  6u,  6u,  6u,  6u, 13u, 13u, 13u, 13u, 13u, 13u, 13u, 22u, 22u, 22u, 22u, 22u, 22u, 22u, 33u, 33u, 33u, 33u, 33u, 33u, 33u, 46u, 46u, 46u, 46u, 46u, 46u };
#endif

	constexpr uint8_t alignmentPatternAxes[40][7] =
	{
		{ 0u,  0u,  0u,  0u,   0u,   0u,   0u }, //  v1
		{ 6u, 18u,  0u,  0u,   0u,   0u,   0u }, //  v2
		{ 6u, 22u,  0u,  0u,   0u,   0u,   0u }, //  v3
		{ 6u, 26u,  0u,  0u,   0u,   0u,   0u }, //  v4
		{ 6u, 30u,  0u,  0u,   0u,   0u,   0u }, //  v5
		{ 6u, 34u,  0u,  0u,   0u,   0u,   0u }, //  v6
		{ 6u, 22u, 38u,  0u,   0u,   0u,   0u }, //  v7
		{ 6u, 24u, 42u,  0u,   0u,   0u,   0u }, //  v8
		{ 6u, 26u, 46u,  0u,   0u,   0u,   0u }, //  v9
		{ 6u, 28u, 50u,  0u,   0u,   0u,   0u }, // v10
		{ 6u, 30u, 54u,  0u,   0u,   0u,   0u }, // v11
		{ 6u, 32u, 58u,  0u,   0u,   0u,   0u }, // v12
		{ 6u, 34u, 62u,  0u,   0u,   0u,   0u }, // v13
		{ 6u, 26u, 46u, 66u,   0u,   0u,   0u }, // v14
		{ 6u, 26u, 48u, 70u,   0u,   0u,   0u }, // v15
		{ 6u, 26u, 50u, 74u,   0u,   0u,   0u }, // v16
		{ 6u, 30u, 54u, 78u,   0u,   0u,   0u }, // v17
		{ 6u, 30u, 56u, 82u,   0u,   0u,   0u }, // v18
		{ 6u, 30u, 58u, 86u,   0u,   0u,   0u }, // v19
		{ 6u, 34u, 62u, 90u,   0u,   0u,   0u }, // v20
		{ 6u, 28u, 50u, 72u,  94u,   0u,   0u }, // v21
		{ 6u, 26u, 50u, 74u,  98u,   0u,   0u }, // v22
		{ 6u, 30u, 54u, 78u, 102u,   0u,   0u }, // v23
		{ 6u, 28u, 54u, 80u, 106u,   0u,   0u }, // v24
		{ 6u, 32u, 58u, 84u, 110u,   0u,   0u }, // v25
		{ 6u, 30u, 58u, 86u, 114u,   0u,   0u }, // v26
		{ 6u, 34u, 62u, 90u, 118u,   0u,   0u }, // v27
		{ 6u, 26u, 50u, 74u,  98u, 122u,   0u }, // v28
		{ 6u, 30u, 54u, 78u, 102u, 126u,   0u }, // v29
		{ 6u, 26u, 52u, 78u, 104u, 130u,   0u }, // v30
		{ 6u, 30u, 56u, 82u, 108u, 134u,   0u }, // v31
		{ 6u, 34u, 60u, 86u, 112u, 138u,   0u }, // v32
		{ 6u, 30u, 58u, 86u, 114u, 142u,   0u }, // v33
		{ 6u, 34u, 62u, 90u, 118u, 146u,   0u }, // v34
		{ 6u, 30u, 54u, 78u, 102u, 126u, 150u }, // v35
		{ 6u, 24u, 50u, 76u, 102u, 128u, 154u }, // v36
		{ 6u, 28u, 54u, 80u, 106u, 132u, 158u }, // v37
		{ 6u, 32u, 58u, 84u, 110u, 136u, 162u }, // v38
		{ 6u, 26u, 54u, 82u, 110u, 138u, 166u }, // v39
		{ 6u, 30u, 58u, 86u, 114u, 142u, 170u }, // v40
	};
	// clang-format on

	// Convert 2D locations to 3D locations in object space

	const CoordinateSystem coordinateSystem(version);

	const unsigned int numberAxes = (unsigned int)numberAlignmentPatternAxes[version - 1u];
	const uint8_t* axes = alignmentPatternAxes[version - 1u];

	std::vector<Vectors3> objectAlignmentPatterns;
	objectAlignmentPatterns.reserve(numberAxes);

	for (unsigned int i = 0u; i < numberAxes; ++i)
	{
		Vectors3 objectAlignmentPatternsRow;
		objectAlignmentPatternsRow.reserve(numberAxes);

		// Flip and shift the y-axis and also shift the pixel origin by 0.5
		const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(axes[i]) + Scalar(0.5));

		for (unsigned int j = 0u; j < numberAxes; ++j)
		{
			// Ignore alignment patterns that overlap with finder patterns:
			//
			//         top-left                    bottom-left                          top-right
			if ((i == 0u && j == 0u) || (i == numberAxes - 1u && j == 0u) || (i == 0u && j == numberAxes - 1u))
			{
				continue;
			}

			// Shift the x-axis and also shift the pixel origin by 0.5
			const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(axes[j]) + Scalar(0.5));

			// Add (x, y, 0) but not (y, x, 0) to preserve the row-wise order
			objectAlignmentPatternsRow.emplace_back(scale * x, scale * y, Scalar(0));
		}

		if (!objectAlignmentPatternsRow.empty())
		{
			objectAlignmentPatterns.emplace_back(std::move(objectAlignmentPatternsRow));
		}
	}

#if defined(OCEAN_DEBUG)
	size_t countedAlignmentPatterns = 0;

	for (const Vectors3& objectAlignmentPatternsRow : objectAlignmentPatterns)
	{
		countedAlignmentPatterns += objectAlignmentPatternsRow.size();
	}

	ocean_assert(countedAlignmentPatterns == size_t(numberAlignmentPatterns[version - 1u]));
#endif

	return objectAlignmentPatterns;
}

Vectors3 Utilities::CoordinateSystem::computeVersionInformationModulesInObjectSpace(const unsigned int version, const bool versionInformation1)
{
	ocean_assert(version >= 1u && version <= 40u);

	if (version < 7u)
	{
		return Vectors3();
	}

	const CoordinateSystem coordinateSystem(version);

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);

	Vectors3 objectPoints;
	objectPoints.reserve(18);

	if (versionInformation1)
	{
		const Scalar x[3] =
		{
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(modulesPerSide - 11u) + Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(modulesPerSide - 10u) + Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(modulesPerSide - 9u) + Scalar(0.5)),
		};

		const Scalar y[6] =
		{
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(1.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(2.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(3.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(4.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(5.5)),
		};

		for (size_t i = 0; i < 6; ++i)
		{
			objectPoints.emplace_back(x[0], y[i], Scalar(0));
			objectPoints.emplace_back(x[1], y[i], Scalar(0));
			objectPoints.emplace_back(x[2], y[i], Scalar(0));
		}
	}
	else
	{
		const Scalar x[6] =
		{
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(1.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(2.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(3.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(4.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(5.5)),
		};

		const Scalar y[3] =
		{
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(modulesPerSide - 11u) + Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(modulesPerSide - 10u) + Scalar(0.5)),
			coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(modulesPerSide - 9u) + Scalar(0.5)),
		};

		for (size_t i = 0; i < 6; ++i)
		{
			objectPoints.emplace_back(x[i], y[0], Scalar(0));
			objectPoints.emplace_back(x[i], y[1], Scalar(0));
			objectPoints.emplace_back(x[i], y[2], Scalar(0));
		}
	}

	ocean_assert(objectPoints.size() == 18);

	return objectPoints;
}

bool Utilities::computeNumberPixelsPerModule(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const Scalar codeSize, const unsigned int version, Scalar* minNumberPixelsPerModule, Scalar* maxNumberPixelsPerModule, Scalar* medianNumberPixelsPerModule, Scalar* avgNumberPixelsPerModule)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(codeSize > Scalar(0));
	ocean_assert(version != 0u && version <= 40u);

	const HomogenousMatrix4& flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(world_T_code.inverted() * world_T_camera);

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);
	ocean_assert(modulesPerSide >= 21u);

	const CV::Detector::QRCodes::Utilities::CoordinateSystem coordinateSystem(version, codeSize * Scalar(0.5));

	Vectors2 topCorners;
	topCorners.reserve(modulesPerSide + 1u);

	for (unsigned int iX = 0u; iX <= modulesPerSide; ++iX)
	{
		const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(iX));
		const Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(x, Scalar(0), Scalar(0)));

		if (!anyCamera.isInside(imagePoint))
		{
			return false;
		}

		topCorners.emplace_back(imagePoint);
	}

	Vectors2 bottomCorners(modulesPerSide + 1);

	Scalars moduleAreas;
	moduleAreas.reserve(modulesPerSide * modulesPerSide);

	Scalar sumArea = Scalar(0);
	Scalar minAreaModule = Numeric::maxValue();
	Scalar maxAreaModule = Scalar(0);

	for (unsigned int iY = 1u; iY <= modulesPerSide; ++iY)
	{
		const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(iY));

		bottomCorners[0] = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(Scalar(0), y, Scalar(0)));

		if (!anyCamera.isInside(bottomCorners[0]))
		{
			return false;
		}

		for (unsigned int iX = 1u; iX <= modulesPerSide; ++iX)
		{
			const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(iX));

			bottomCorners[iX] = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(x, y, Scalar(0)));

			if (!anyCamera.isInside(bottomCorners[iX]))
			{
				return false;
			}

			//                 ...
			//                 |   |   |   |
			//     topCorners: *---*---*---*...
			//                 | \ | \ | \ |
			//  bottomCorners: *---*---*---*...
			//                 |   |   |   |
			//                 ...
			const Scalar areaTriangle0 = Triangle2(topCorners[iX - 1u], bottomCorners[iX - 1], bottomCorners[iX]).area();
			const Scalar areaTriangle1 = Triangle2(topCorners[iX - 1u], bottomCorners[iX], topCorners[iX]).area();

			const Scalar areaModule = areaTriangle0 + areaTriangle1;

			minAreaModule = std::min(minAreaModule, areaModule);
			maxAreaModule = std::max(maxAreaModule, areaModule);

			sumArea += areaModule;

			moduleAreas.emplace_back(areaModule);
		}

		std::swap(topCorners, bottomCorners);
	}

	if (minNumberPixelsPerModule)
	{
		*minNumberPixelsPerModule = minAreaModule;
	}

	if (maxNumberPixelsPerModule)
	{
		*maxNumberPixelsPerModule = maxAreaModule;
	}

	if (medianNumberPixelsPerModule)
	{
		ocean_assert(moduleAreas.size() == size_t(modulesPerSide * modulesPerSide));
		std::partial_sort(moduleAreas.begin(), moduleAreas.begin() + moduleAreas.size() / 2 + 1, moduleAreas.end());

		*medianNumberPixelsPerModule = moduleAreas[moduleAreas.size() / 2];
	}

	if (avgNumberPixelsPerModule)
	{
		*avgNumberPixelsPerModule = sumArea / Scalar(modulesPerSide * modulesPerSide);
	}

	return true;
}

Scalar Utilities::computeModuleDiagonalLength(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_code, const CoordinateSystem& coordinateSystem, const unsigned int xModule, const unsigned int yModule)
{
	const Scalar xTop = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(xModule));
	const Scalar yTop = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(yModule));

	const Scalar xBottom = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(xModule + 1u));
	const Scalar yBottom = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(yModule + 1u));

	const Vector2 topLeft = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(xTop, yTop, Scalar(0)));
	const Vector2 topRight = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(xBottom, yTop, Scalar(0)));
	const Vector2 bottomLeft = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(xTop, yBottom, Scalar(0)));
	const Vector2 bottomRight = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(xBottom, yBottom, Scalar(0)));

	const Scalar averageDiagonalDistance = Scalar(0.5) * (bottomRight.distance(topLeft) + bottomLeft.distance(topRight));

	return averageDiagonalDistance;
}

bool Utilities::computeContrast(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_camera, const QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, unsigned int* medianContrast, unsigned int* averageContrast)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8));
	ocean_assert(world_T_camera.isValid());
	ocean_assert(code.isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(codeSize > Scalar(0));

	const HomogenousMatrix4& flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(world_T_code.inverted() * world_T_camera);

	const unsigned int modulesPerSide = code.modulesPerSide();
	ocean_assert(modulesPerSide >= 21u);

	const CV::Detector::QRCodes::Utilities::CoordinateSystem coordinateSystem(code.version(), codeSize * Scalar(0.5));

	const std::vector<uint8_t>& modules = code.modules();
	ocean_assert(modules.size() == size_t(modulesPerSide * modulesPerSide));

	std::vector<uint8_t> intensitiesModule0;
	std::vector<uint8_t> intensitiesModule1;

	unsigned int sumIntensityModule0 = 0u;
	unsigned int numberIntensitiesModule0 = 0u;

	unsigned int sumIntensityModule1 = 0u;
	unsigned int numberIntensitiesModule1 = 0u;

	intensitiesModule0.reserve((modulesPerSide * modulesPerSide) / 2u);
	intensitiesModule1.reserve((modulesPerSide * modulesPerSide) / 2u);

	for (unsigned int iY = 0u; iY < modulesPerSide; ++iY)
	{
		const Scalar yCenter = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(iY) + Scalar(0.5));

		const uint8_t* const modulesRow = modules.data() + iY * modulesPerSide;

		for (unsigned int iX = 0u; iX < modulesPerSide; ++iX)
		{
			const Scalar xCenter = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(iX) + Scalar(0.5));

			const Vector2 center = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(xCenter, yCenter, Scalar(0)));

			if (!anyCamera.isInside(center, /* signedBorder */ Scalar(3)))
			{
				return false;
			}

			uint8_t intensity;
			CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), center, &intensity);

			if (modulesRow[iX] == 0u)
			{
				sumIntensityModule0 += intensity;
				numberIntensitiesModule0++;

				intensitiesModule0.emplace_back(intensity);
			}
			else
			{
				sumIntensityModule1 += intensity;
				numberIntensitiesModule1++;

				intensitiesModule1.emplace_back(intensity);
			}
		}
	}

	if (intensitiesModule0.empty() || intensitiesModule1.empty())
	{
		return false;
	}

	if (medianContrast)
	{
		std::partial_sort(intensitiesModule0.begin(), intensitiesModule0.begin() + intensitiesModule0.size() / 2, intensitiesModule0.end());
		std::partial_sort(intensitiesModule1.begin(), intensitiesModule1.begin() + intensitiesModule1.size() / 2, intensitiesModule1.end());

		const uint8_t medianIntensityModule0 = intensitiesModule0[intensitiesModule0.size() / 2];
		const uint8_t medianIntensityModule1 = intensitiesModule1[intensitiesModule1.size() / 2];

		*medianContrast = (unsigned int)(std::abs(int(medianIntensityModule0) - int(medianIntensityModule1)));
	}

	if (averageContrast)
	{
		const uint8_t averageIntensityModule0 = uint8_t(Numeric::round32(Scalar(sumIntensityModule0) / Scalar(numberIntensitiesModule0)));
		const uint8_t averageIntensityModule1 = uint8_t(Numeric::round32(Scalar(sumIntensityModule1) / Scalar(numberIntensitiesModule1)));

		*averageContrast = (unsigned int)(std::abs(int(averageIntensityModule0) - int(averageIntensityModule1)));
	}

	return true;
}

bool Utilities::computeCodeTiltAndViewAngles(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, Scalar& tiltAngle, Scalar& viewAngle, Scalar* distance)
{
	ocean_assert(world_T_camera.isValid());
	ocean_assert(world_T_code.isValid());

	const HomogenousMatrix4 camera_T_code = world_T_camera.inverted() * world_T_code;

	const Vector3 codeCenter = camera_T_code.translation();
	const Vector3 codeNormalPoint = camera_T_code * Vector3(Scalar(0), Scalar(0), Scalar(1)); // a point 1 meter above the code center
	const Vector3 codeNormal = codeNormalPoint - codeCenter;

	if (distance != nullptr)
	{
		*distance = codeCenter.length();
	}

	tiltAngle = codeNormal.angle(-codeCenter);

	viewAngle = codeCenter.angle(/* camera view direction */ Vector3(Scalar(0), Scalar(0), Scalar(-1)));

	return true;
}

bool Utilities::containsCodeStereo(const SharedAnyCameras& sharedAnyCameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const QRCodes& codes, const HomogenousMatrices4& world_T_codes, const Scalars& codeSizes, const QRCode& newCode, const HomogenousMatrix4& world_T_newCode, const Scalar newCodeSize, unsigned int* index)
{
	ocean_assert(sharedAnyCameras.size() == 2);
	ocean_assert(sharedAnyCameras.size() == device_T_cameras.size());
	ocean_assert(world_T_device.isValid());

#if defined(OCEAN_DEBUG)
	for (size_t cameraIndex = 0; cameraIndex < sharedAnyCameras.size(); ++cameraIndex)
	{
		ocean_assert(sharedAnyCameras[cameraIndex] != nullptr && sharedAnyCameras[cameraIndex]->isValid());
		ocean_assert(device_T_cameras[cameraIndex].isValid());
	}
#endif

	ocean_assert(codes.size() == world_T_codes.size());
	ocean_assert(codes.size() == codeSizes.size());

#if defined(OCEAN_DEBUG)
	for (size_t codeIndex = 0; codeIndex < codes.size(); ++codeIndex)
	{
		ocean_assert(codes[codeIndex].isValid());
		ocean_assert(world_T_codes[codeIndex].isValid());
		ocean_assert(codeSizes[codeIndex] > Scalar(0));
	}
#endif

	ocean_assert(newCode.isValid());
	ocean_assert(world_T_newCode.isValid());
	ocean_assert(newCodeSize > Scalar(0));

	if (codes.empty())
	{
		return false;
	}

	for (size_t cameraIndex = 0; cameraIndex < 2; ++cameraIndex)
	{
		if (containsCodeMono(*sharedAnyCameras[cameraIndex], world_T_device * device_T_cameras[cameraIndex], codes, world_T_codes, codeSizes, newCode, world_T_newCode, newCodeSize, index))
		{
			return true;
		}
	}

	return false;
}

bool Utilities::containsCodeMono(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const QRCodes& codes, const HomogenousMatrices4& world_T_codes, const Scalars& codeSizes, const QRCode& newCode, const HomogenousMatrix4& world_T_newCode, const Scalar newCodeSize, unsigned int* index)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(codes.size() == world_T_codes.size());
	ocean_assert(codes.size() == codeSizes.size());

#if defined(OCEAN_DEBUG)
	for (size_t codeIndex = 0; codeIndex < codes.size(); ++codeIndex)
	{
		ocean_assert(codes[codeIndex].isValid());
		ocean_assert(world_T_codes[codeIndex].isValid());
		ocean_assert(codeSizes[codeIndex] > Scalar(0));
	}
#endif

	ocean_assert(newCode.isValid());
	ocean_assert(world_T_newCode.isValid());
	ocean_assert(newCodeSize > Scalar(0));

	if (codes.empty())
	{
		return false;
	}

	Scalar newCodeSquareRadius = Scalar(0);
	Vector2 imageNewCodeCenter = Vector2(Scalar(0), Scalar(0));

	if (!computeCodeCenterInImage(anyCamera, world_T_camera, newCode, world_T_newCode, newCodeSize, imageNewCodeCenter, &newCodeSquareRadius))
	{
		return false;
	}

	ocean_assert(newCodeSquareRadius > Scalar(0) && !imageNewCodeCenter.isNull());

	for (size_t codeIndex = 0; codeIndex < codes.size(); ++codeIndex)
	{
		if (newCode.isSame(codes[codeIndex], /* ignoreModules */ true))
		{
			// Found a code with identical payload and metadata

			Scalar codeSquareRadius = Scalar(0);
			Vector2 imageCodeCenter;

			if (!computeCodeCenterInImage(anyCamera, world_T_camera, codes[codeIndex], world_T_codes[codeIndex], codeSizes[codeIndex], imageCodeCenter, &codeSquareRadius))
			{
				continue;
			}

			ocean_assert(codeSquareRadius > Scalar(0) && !imageCodeCenter.isNull());

			const Scalar squareDistance = imageNewCodeCenter.sqrDistance(imageCodeCenter);

			if (squareDistance < codeSquareRadius || squareDistance < newCodeSquareRadius)
			{
				// Found a code that is identical and close enough to be from the same object instance

				if (index != nullptr)
				{
					*index = (unsigned int)codeIndex;
				}

				return true;
			}
		}
	}

	return false;
}

bool Utilities::containsCode(const QRCodes& codes, const QRCode& newCode)
{
	ocean_assert(newCode.isValid());

	for (const QRCode& code : codes)
	{
		ocean_assert(code.isValid());

		if (code.isSame(newCode, /* ignoreModules */ true))
		{
			return true;
		}
	}

	return false;
}

Utilities::ParsingStatus Utilities::parseWifiConfig(const std::string& configString, std::string& ssid, std::string& password, std::string* encryption, bool* isSsidHidden)
{
	ocean_assert(!configString.empty());

	const std::string prefix = "WIFI:";

	if (configString.size() <= prefix.size() || configString.substr(0, prefix.size()) != prefix)
	{
		return PS_ERROR_INVALID_PREFIX;
	}

	bool didParsePassword = false;
	bool didParseEncryption = false;
	bool didParseIsHiddenSsid = false;

	std::string parsedSsid;
	std::string parsedPassword;
	std::string parsedEncryption;
	bool parsedIsSsidHidden = false;

	size_t fieldStart = prefix.size();

	while (fieldStart + 1 < configString.size())
	{
		// Find the end of the current field and check for cases with multiple backslashes, for example "...\\\\\\\\;..."
		size_t fieldEnd;
		size_t neighboringBackslashes = 0;

		for (fieldEnd = fieldStart; fieldEnd < configString.size(); ++fieldEnd)
		{
			if (configString[fieldEnd] == '\\')
			{
				// Check for consecutive backslashes
				++neighboringBackslashes;

				if (neighboringBackslashes == 2)
				{
					neighboringBackslashes = 0;
				}

				continue;
			}
			else if (configString[fieldEnd] == ';')
			{
				if (neighboringBackslashes == 0)
				{
					break;
				}
			}

			neighboringBackslashes = 0;
		}

		if (fieldEnd >= configString.size())
		{
			return PS_ERROR_INVALID_FIELD_FORMAT;
		}

		ocean_assert(fieldStart <= fieldEnd);
		const size_t fieldLength = fieldEnd - fieldStart;

		if (fieldLength < 2)
		{
			return PS_ERROR_INVALID_FIELD_FORMAT;
		}

		const std::string field = configString.substr(fieldStart, fieldLength);
		const std::string fieldPrefix = field.substr(0, 2);

		if (fieldPrefix == "S:")
		{
			// Parse the SSID

			if (!parsedSsid.empty())
			{
				return PS_ERROR_DUPLICATE_FIELD_TYPE;
			}

			if (!unescapeSpecialCharacters(field.substr(2, field.size() - 2), parsedSsid))
			{
				return PS_ERROR_INVALID_FIELD_DATA;
			}

			if (parsedSsid.empty())
			{
				return PS_ERROR_INVALID_FIELD_DATA;
			}
		}
		else if (fieldPrefix == "T:")
		{
			// Parse the encryption type

			if (didParseEncryption)
			{
				return PS_ERROR_DUPLICATE_FIELD_TYPE;
			}

			didParseEncryption = true;
			parsedEncryption = field.substr(2, field.size() - 2);

			if (!parsedEncryption.empty() && parsedEncryption != "WPA" && parsedEncryption != "WEP")
			{
				return PS_ERROR_INVALID_FIELD_DATA;
			}
		}
		else if (fieldPrefix == "P:")
		{
			// Parse the password

			if (didParsePassword)
			{
				return PS_ERROR_DUPLICATE_FIELD_TYPE;
			}

			didParsePassword = true;

			if (!unescapeSpecialCharacters(field.substr(2, field.size() - 2), parsedPassword))
			{
				return PS_ERROR_INVALID_FIELD_DATA;
			}
		}
		else if (fieldPrefix == "H:")
		{
			// Parse the hidden SSID flag

			if (didParseIsHiddenSsid)
			{
				return PS_ERROR_DUPLICATE_FIELD_TYPE;
			}

			didParseIsHiddenSsid = true;

			const std::string value = field.substr(2, field.size() - 2);
			if (value.empty() || value == "false")
			{
				parsedIsSsidHidden = false;
			}
			else if (value == "true")
			{
				parsedIsSsidHidden = true;
			}
			else
			{
				return PS_ERROR_INVALID_FIELD_DATA;
			}
		}
		else
		{
			return PS_ERROR_INVALID_FIELD_TYPE;
		}

		fieldStart = fieldEnd + 1;
	}

	if (parsedSsid.empty())
	{
		return PS_ERROR_INVALID_FIELD_DATA;
	}

	if (parsedPassword.empty() && !parsedEncryption.empty())
	{
		return PS_ERROR_INVALID_FIELD_DATA;
	}

	ocean_assert(configString.size() >= 2);
	if (configString.substr(configString.size() - 2, 2) != ";;")
	{
		return PS_ERROR_INVALID_TERMINATION;
	}

	ssid = parsedSsid;
	password = parsedPassword;

	if (encryption != nullptr)
	{
		*encryption = parsedEncryption;
	}

	if (isSsidHidden!= nullptr)
	{
		*isSsidHidden = parsedIsSsidHidden;
	}

	return PS_SUCCESS;
}

std::string Utilities::parsingStatusToString(const Utilities::ParsingStatus status)
{
	switch (status)
	{
		case PS_SUCCESS:
			return "SUCCESS";

		case PS_ERROR_INVALID_PREFIX:
			return "ERROR_INVALID_PREFIX";

		case PS_ERROR_INVALID_FIELD_FORMAT:
			return "ERROR_INVALID_FIELD_FORMAT";

		case PS_ERROR_INVALID_FIELD_DATA:
			return "ERROR_INVALID_FIELD_DATA";

		case PS_ERROR_INVALID_FIELD_TYPE:
			return "ERROR_INVALID_FIELD_TYPE";

		case PS_ERROR_DUPLICATE_FIELD_TYPE:
			return "ERROR_DUPLICATE_FIELD_TYPE";

		case PS_ERROR_INVALID_TERMINATION:
			return "ERROR_INVALID_TERMINATION";
	}

	ocean_assert(false && "This should never happen!");
	return std::string();
}

std::string Utilities::escapeSpecialCharacters(const std::string& rawString, const std::string& specialCharacters)
{
	if (specialCharacters.empty())
	{
		ocean_assert(false && "No special characters specified");
		return rawString;
	}

	const std::unordered_set<char> specialCharactersSet(specialCharacters.cbegin(), specialCharacters.cend());

	std::string escapedString;
	escapedString.reserve(rawString.size());

	for (const char character : rawString)
	{
		if (specialCharactersSet.find(character) != specialCharactersSet.cend())
		{
			escapedString.push_back('\\');
		}

		escapedString.push_back(character);
	}

	return escapedString;
}

bool Utilities::unescapeSpecialCharacters(const std::string& escapedString, std::string& rawString, const std::string& specialCharacters)
{
	if (specialCharacters.empty())
	{
		ocean_assert(false && "No special characters specified");
		return false;
	}

	const std::unordered_set<char> specialCharactersSet(specialCharacters.cbegin(), specialCharacters.cend());

	if (escapedString.size() == 1)
	{
		if (specialCharactersSet.find(escapedString.front()) != specialCharactersSet.cend())
		{
			return false;
		}

		rawString = escapedString;
		return true;
	}

	std::string localRawString;
	localRawString.reserve(escapedString.size());

	for (size_t index = 0; index < escapedString.size(); /* increments in loop */)
	{
		if (escapedString[index] == '\\')
		{
			if (index + 1 < escapedString.size())
			{
				const size_t nextIndex = index + 1;
				const char nextCharacter = escapedString[nextIndex];

				if (specialCharactersSet.find(nextCharacter) != specialCharactersSet.cend())
				{
					localRawString.push_back(nextCharacter);
					index = nextIndex + 1;

					continue;
				}
			}
		}

		localRawString.push_back(escapedString[index]);
		++index;
	}

	rawString = localRawString;

	return true;
}

bool Utilities::computeCodeCenterInImage(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, Vector2& imageCodeCenter, Scalar* maxSquareRadius)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(code.isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(codeSize > Scalar(0));

	const HomogenousMatrix4 flippedCamera_T_code = AnyCamera::standard2InvertedFlipped(world_T_code.inverted() * world_T_camera);
	ocean_assert(flippedCamera_T_code.isValid());

	const Vector3 codeCenter(Scalar(0), Scalar(0), Scalar(0));

	if (!anyCamera.isObjectPointInFrontIF(flippedCamera_T_code, codeCenter))
	{
		return false;
	}

	imageCodeCenter = anyCamera.projectToImageIF(flippedCamera_T_code, codeCenter);

	if (!anyCamera.isInside(imageCodeCenter))
	{
		return false;
	}

	if (maxSquareRadius != nullptr)
	{
		const unsigned int modulesPerSide = code.modulesPerSide();
		const Vector2 cornersInModules[4] =
		{
			Vector2(Scalar(0), Scalar(0)), // TL
			Vector2(Scalar(0), Scalar(modulesPerSide)), // BL
			Vector2(Scalar(modulesPerSide), Scalar(modulesPerSide)), // BR
			Vector2(Scalar(modulesPerSide), Scalar(0)), // TR
		};

		const CoordinateSystem coordinateSystem(code.version(), Scalar(0.5) * codeSize);

		Scalar squareRadius = Scalar(0);

		for (size_t cornerIndex = 0; cornerIndex < 4; ++cornerIndex)
		{
			const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(cornersInModules[cornerIndex].x());
			const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(cornersInModules[cornerIndex].y());

			const Vector2 imageCodeCorner = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(x, y, Scalar(0)));

			squareRadius = std::max(squareRadius, imageCodeCenter.sqrDistance(imageCodeCorner));
		}

		*maxSquareRadius = squareRadius;
	}

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
