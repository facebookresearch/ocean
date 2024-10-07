/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/MicroQRCodeDetector.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"
#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/TransitionDetector.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

bool MicroQRCodeDetector::getTimingPatternModules(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern& finderPattern, const unsigned int topLeftCorner, const unsigned int timingAdjacentFinderCorner, Vectors2& moduleCenters)
{
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(topLeftCorner < 4u && timingAdjacentFinderCorner < 4u);

	if (!finderPattern.cornersKnown())
	{
		return false;
	}

	if ((topLeftCorner ^ 2u) == timingAdjacentFinderCorner)
	{
		ocean_assert(false && "Corners must be adjacent.");
		return false;
	}

	const Vector2 edge = (finderPattern.corners()[timingAdjacentFinderCorner] - finderPattern.corners()[topLeftCorner]);
	Scalar moduleSize = edge.length() / Scalar(7);
	const unsigned int minStepSize = Numeric::round32(moduleSize * Scalar(0.35));
	const unsigned int maxStepSize = Numeric::round32(moduleSize * Scalar(1.5));
	Vector2 direction = edge.normalized();

	unsigned int columns = (unsigned int)(-1);
	unsigned int rows = (unsigned int)(-1);

	// First check in the backward direction for a clear quiet zone

	const unsigned int otherCorner = timingAdjacentFinderCorner ^ 2u;

	// Start half a module away from the corner
	const Vector2 startPointBack = finderPattern.corners()[topLeftCorner] + (finderPattern.corners()[otherCorner] - finderPattern.corners()[timingAdjacentFinderCorner]) * Scalar(1.0 / 14.0);
	const Vector2 farPointBack = startPointBack - direction * Scalar(width);

	const unsigned int xBack = Numeric::round32(startPointBack.x());
	const unsigned int yBack = Numeric::round32(startPointBack.y());

	Bresenham bresenhamBack(xBack, yBack, Numeric::round32(farPointBack.x()), Numeric::round32(farPointBack.y()));

	VectorT2<unsigned int> lastIn, firstOut;
	if (xBack < width && yBack < height && TransitionDetector::findNextPixel<true>(yFrame, xBack, yBack, width, height, paddingElements, bresenhamBack, maxStepSize, finderPattern.grayThreshold(), columns, rows, lastIn, firstOut))
	{
		// Found dark pixel in quiet zone
		return false;
	}

	// Now check for alternating light/dark modules in the forward direction

	unsigned int oppositeStartCorner = topLeftCorner ^ 2u;

	// Start halfway between the edge of the finder pattern and the first dark module of the timing pattern
	const Vector2 startPoint = finderPattern.corners()[timingAdjacentFinderCorner] + (finderPattern.corners()[oppositeStartCorner] - finderPattern.corners()[topLeftCorner]) * Scalar(1.0 / 14.0);
	const Vector2 farPoint = startPoint + direction * Scalar(width);

	unsigned int x = Numeric::round32(startPoint.x());
	unsigned int y = Numeric::round32(startPoint.y());

	Bresenham bresenham(x, y, Numeric::round32(farPoint.x()), Numeric::round32(farPoint.y()));

	unsigned int darkModule = 0u;

	Vectors2 moduleCentersTmp;
	moduleCentersTmp.reserve(10);
	moduleCentersTmp.push_back(startPoint);

	while (darkModule <= 5)
	{
		// Find the start and end of the next dark module

		bool foundDark = true;

		for (const bool start : {true, false})
		{
			if (x >= width || y >= height
					|| (start && !TransitionDetector::findNextPixel<true>(yFrame, x, y, width, height, paddingElements, bresenham, maxStepSize, finderPattern.grayThreshold(), columns, rows, lastIn, firstOut))
					|| (!start && !TransitionDetector::findNextPixel<false>(yFrame, x, y, width, height, paddingElements, bresenham, maxStepSize, finderPattern.grayThreshold(), columns, rows, lastIn, firstOut)))
			{
				foundDark = false;
				break;
			}

			unsigned int squareDistance = columns * columns + rows * rows;
			if (squareDistance < minStepSize * minStepSize || squareDistance > maxStepSize * maxStepSize)
			{
				foundDark = false;
				break;
			}

			if (darkModule > 0u || !start)
			{
				moduleCentersTmp.emplace_back(Scalar(x) * 0.5 + Scalar(lastIn.x()) * 0.5, Scalar(y) * 0.5 + Scalar(lastIn.y()) * 0.5);
			}

			x = firstOut.x();
			y = firstOut.y();
		}

		if (!foundDark)
		{
			break;
		}

		darkModule++;
	}

	moduleCenters = std::move(moduleCentersTmp);
	return true;
}

bool MicroQRCodeDetector::computePosesAndProvisionalVersions(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern& finderPattern, HomogenousMatrices4& code_T_cameras, std::vector<unsigned int>& provisionalVersions, const Scalar scale)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);

	std::vector<unsigned int> candidateVersions;
	HomogenousMatrices4 candidatePoses;

	if (!finderPattern.cornersKnown())
	{
		return false;
	}

	for (unsigned int corner = 0u; corner < 4u; corner++)
	{
		Vectors2 horizontalTimingCenters, verticalTimingCenters;
		if (!(getTimingPatternModules(yFrame, width, height, paddingElements, finderPattern, corner, (corner + 3) % 4, horizontalTimingCenters) && getTimingPatternModules(yFrame, width, height, paddingElements, finderPattern, corner, (corner + 1) % 4, verticalTimingCenters)))
		{
			// Timing patterns should be found in both directions
			continue;
		}

		if (horizontalTimingCenters.size() < 2u && verticalTimingCenters.size() < 2u)
		{
			// Need at least one timing pattern dark module to consider this a candidate orientation
			continue;
		}

		const unsigned int minVersion = std::max(2u, (unsigned int)(std::min(horizontalTimingCenters.size(), verticalTimingCenters.size()) >> 1u)) - 1u;

		if (minVersion < 1u || minVersion > 4u)
		{
			// Version should be between 1 and 4
			Log::error() << "Invalid version: " << minVersion;
			ocean_assert(false && "This should never happen!");
			continue;
		}

		unsigned int maxVersion = std::min(4u, std::max(2u, (unsigned int)(std::max(horizontalTimingCenters.size(), verticalTimingCenters.size()) >> 1u)) - 1u);

		if (maxVersion < 1u || maxVersion > 4u)
		{
			// Version should be between 1 and 4
			Log::error() << "Invalid version: " << maxVersion;
			ocean_assert(false && "This should never happen!");
			continue;
		}

		for (unsigned int provisionalVersion = minVersion; provisionalVersion <= maxVersion; provisionalVersion++)
		{
			// Compute poses using timing patterns and finder pattern
			const Utilities::MicroQRCoordinateSystem coordinateSystem(provisionalVersion, scale);
			const Vector3 finderPatternObjectPoint = coordinateSystem.computeFinderPatternCenterInObjectSpace();
			const Vectors3 finderPatternCornerObjectPoints = coordinateSystem.computeFinderPatternCornersInObjectSpace();
			const Vectors3 horizontalTimingPatternObjectPoints = coordinateSystem.computeHorizontalTimingPatternModulesInObjectSpace();
			const Vectors3 verticalTimingPatternObjectPoints = coordinateSystem.computeVerticalTimingPatternModulesInObjectSpace();

			const unsigned int maxTimingModules = MicroQRCode::modulesPerSide(provisionalVersion) - 7u;
			const unsigned int numHorizontalTimingModules = std::min((unsigned int)horizontalTimingCenters.size(), maxTimingModules);
			const unsigned int numVerticalTimingModules = std::min((unsigned int)verticalTimingCenters.size(), maxTimingModules);
			const unsigned int numPoints = 5u + numHorizontalTimingModules + numVerticalTimingModules;

			Vectors3 objectPoints;
			objectPoints.reserve(numPoints);
			objectPoints.push_back(finderPatternObjectPoint);
			objectPoints.insert(objectPoints.end(), finderPatternCornerObjectPoints.begin(), finderPatternCornerObjectPoints.end());
			objectPoints.insert(objectPoints.end(), horizontalTimingPatternObjectPoints.begin(), horizontalTimingPatternObjectPoints.begin() + numHorizontalTimingModules);
			objectPoints.insert(objectPoints.end(), verticalTimingPatternObjectPoints.begin(), verticalTimingPatternObjectPoints.begin() + numVerticalTimingModules);

			Vectors2 imagePoints;
			imagePoints.reserve(numPoints);
			imagePoints.push_back(finderPattern.position());
			imagePoints.insert(imagePoints.end(), finderPattern.corners(), finderPattern.corners() + 4u);
			imagePoints.insert(imagePoints.end(), horizontalTimingCenters.begin(), horizontalTimingCenters.begin() + numHorizontalTimingModules);
			imagePoints.insert(imagePoints.end(), verticalTimingCenters.begin(), verticalTimingCenters.begin() + numVerticalTimingModules);

			if (objectPoints.size() != numPoints)
			{
				Log::error() << "Invalid number of object points: " << objectPoints.size() << " != " << numPoints;
				ocean_assert(false && "This should never happen!");
				continue;
			}

			if (objectPoints.size() != imagePoints.size())
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}

			HomogenousMatrix4 code_T_camera;
			RandomGenerator randomGenerator;
			if (Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, code_T_camera, numPoints / 2u + 1u))
			{
				candidateVersions.push_back(provisionalVersion);
				candidatePoses.push_back(code_T_camera);
			}
		}
	}

	provisionalVersions = std::move(candidateVersions);
	code_T_cameras = std::move(candidatePoses);

	return !code_T_cameras.empty();
}

bool MicroQRCodeDetector::extractModulesFromImage(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int version, const HomogenousMatrix4& code_T_camera, const bool isNormalReflectance, const unsigned int grayThreshold, std::vector<uint8_t>& modules, const Scalar scale)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert_and_suppress_unused(yFrame != nullptr && width != 0u && height != 0u, height);
	ocean_assert(version  >= 1u && version <= 4u);
	ocean_assert(code_T_camera.isValid());
	ocean_assert(grayThreshold <= 255u);
	ocean_assert(scale > Scalar(0));

	const unsigned int strideElements = width + paddingElements;

	TransitionDetector::PixelBinaryThresholdFunc isForegroundPixel = isNormalReflectance ? TransitionDetector::isBlackPixel : TransitionDetector::isWhitePixel;

	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(grayThreshold <= 255u);
	ocean_assert(version != 0u && version <= 4u);

	const unsigned int modulesPerSide = MicroQRCode::modulesPerSide(version);
	ocean_assert(modulesPerSide >= 11u);

	modules.resize(size_t(modulesPerSide) * size_t(modulesPerSide), 0u);

	const HomogenousMatrix4 flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(code_T_camera);
	ocean_assert(flippedCamera_T_code.isValid());

	const Utilities::MicroQRCoordinateSystem coordinateSystem(version, scale);

	unsigned int moduleIndex = 0u;
	for (unsigned int yModule = 0u; yModule < modulesPerSide; ++yModule)
	{
		const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(yModule) + Scalar(0.5));
		ocean_assert(y > Scalar(-1) && y < Scalar(1));

		for (unsigned int xModule = 0u; xModule < modulesPerSide; ++xModule)
		{
			const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(xModule) + Scalar(0.5));
			ocean_assert(x > Scalar(-1) && x < Scalar(1));

			const Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_code, Vector3(x, y, Scalar(0)));

			if (anyCamera.isInside(imagePoint, Scalar(0.5)))
			{
				const unsigned int pixelOffset = (unsigned int)(imagePoint.y() + Scalar(0.5)) * strideElements + (unsigned int)(imagePoint.x() + Scalar(0.5));
				modules[moduleIndex] = isForegroundPixel(yFrame + pixelOffset, uint8_t(grayThreshold)) ? 1u : 0u;
			}

			moduleIndex++;
		}
	}

	return true;
}

unsigned int MicroQRCodeDetector::extractVersionFromModules(const unsigned int provisionalVersion, const std::vector<uint8_t>& modules)
{
	ocean_assert(provisionalVersion >= 1u && provisionalVersion <= 4u);

	const unsigned int modulesPerSide = MicroQRCode::modulesPerSide(provisionalVersion);

	uint32_t formatBits = 0u;

	for (unsigned int bit = 0u, x = 8u, y = 1u; bit < 15u; ++bit)
	{
		const unsigned int index = modulesPerSide * y + x;

		if (index < modules.size() && modules[index] != 0)
		{
			formatBits |= (1u << bit);
		}

		if (bit < 7u)
		{
			++y;
		}
		else
		{
			--x;
		}
	}

	unsigned int version;
	MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity;
	MicroQRCodeEncoder::MaskingPattern maskingPattern;
	if (MicroQRCodeEncoder::decodeFormatBits(formatBits, version, errorCorrectionCapacity, maskingPattern))
	{
		return version;
	}
	else
	{
		return 0;
	}
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
