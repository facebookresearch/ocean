/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/QRCodeDetector.h"

#include "ocean/cv/detector/qrcodes/AlignmentPatternDetector.h"
#include "ocean/cv/detector/qrcodes/TransitionDetector.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/P3P.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{


bool QRCodeDetector::computeProvisionalVersionRange(const AnyCamera& anyCamera, const FinderPattern* finderPatterns, const unsigned int maxAllowedVersionDifference, unsigned int& versionLow, unsigned int& versionHigh)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(finderPatterns != nullptr);

	ocean_assert(anyCamera.isInside(finderPatterns[0].position()));
	ocean_assert(anyCamera.isInside(finderPatterns[1].position()));
	ocean_assert(anyCamera.isInside(finderPatterns[2].position()));

	ocean_assert(!Numeric::isEqualEps((finderPatterns[1].position() - finderPatterns[0].position()).length()));
	ocean_assert(!Numeric::isEqualEps((finderPatterns[2].position() - finderPatterns[1].position()).length()));
	ocean_assert(!Numeric::isEqualEps((finderPatterns[0].position() - finderPatterns[2].position()).length()));

	// Ensure counter-clockwise order of the finder patterns
	ocean_assert((finderPatterns[1].position() - finderPatterns[0].position()).cross(finderPatterns[0].position() - finderPatterns[2].position()) >= Scalar(0));
	ocean_assert((finderPatterns[2].position() - finderPatterns[1].position()).cross(finderPatterns[1].position() - finderPatterns[0].position()) >= Scalar(0));
	ocean_assert((finderPatterns[0].position() - finderPatterns[2].position()).cross(finderPatterns[2].position() - finderPatterns[1].position()) >= Scalar(0));

	// Define a helper plane that is free of lens distortion (this is similar to plane Z=1 when the plane normal and the view axis of the camera are coinciding)

	const Vector3 vectors[3] =
	{
		anyCamera.vector(finderPatterns[0].position()),
		anyCamera.vector(finderPatterns[1].position()),
		anyCamera.vector(finderPatterns[2].position()),
	};

	const Vector3 normal = (vectors[0] + vectors[1] + vectors[2]).normalizedOrZero();

	if (!normal.isUnit())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const Plane3 plane(normal, /* distance */ Scalar(1));

	Vector3 planePoints[3];
	for (size_t i = 0; i < 3; ++i)
	{
		if (!plane.intersection(Line3(Vector3(0, 0, 0), vectors[i]), planePoints[i]))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	// Compute a provisional version from the distances along the x- and the y-direction.

	Scalar moduleSizeInPlane[3] = { Scalar(0), Scalar(0), Scalar(0) };

	for (size_t i = 0; i < 3; ++i)
	{
		if (!computeModuleSizeInPlane(anyCamera, plane, finderPatterns[i].position(), planePoints[i], finderPatterns[i].moduleSize(), moduleSizeInPlane[i]))
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	ocean_assert(moduleSizeInPlane[0] > Scalar(0));
	ocean_assert(moduleSizeInPlane[1] > Scalar(0));
	ocean_assert(moduleSizeInPlane[2] > Scalar(0));

	unsigned int versionXLow = 0u;
	unsigned int versionXHigh = 0u;

	unsigned int versionYLow = 0u;
	unsigned int versionYHigh = 0u;

	if (!computeProvisionalVersion(planePoints[0], planePoints[1], moduleSizeInPlane[0], moduleSizeInPlane[1], maxAllowedVersionDifference, versionYLow, versionYHigh) ||
		!computeProvisionalVersion(planePoints[0], planePoints[1], moduleSizeInPlane[0], moduleSizeInPlane[2], maxAllowedVersionDifference, versionXLow, versionXHigh))
	{
		return false;
	}

	ocean_assert(versionYLow >= 1u && versionYHigh <= 40u && versionYHigh >= versionYLow);
	ocean_assert(versionXLow >= 1u && versionXHigh <= 40u && versionXHigh >= versionXLow);

	versionLow = std::min(versionXLow, versionYLow);
	versionHigh = std::max(versionXHigh, versionYHigh);

	if (versionHigh - versionLow <= maxAllowedVersionDifference)
	{
		return true;
	}

	return false;
}

bool QRCodeDetector::computeProvisionalVersion(const Vector3& centerA, const Vector3& centerB, const Scalar moduleSizeA, const Scalar moduleSizeB, const unsigned int maxAllowedVersionDifference, unsigned int& versionLow, unsigned int& versionHigh)
{

	ocean_assert(moduleSizeA > Numeric::eps());
	ocean_assert(moduleSizeB > Numeric::eps());

	const Vector3 vectorAB = centerB - centerA;
	ocean_assert(!vectorAB.isNull());

	const Scalar distanceAB = vectorAB.length();

	unsigned int localVersionLow = (unsigned int)(Numeric::round32(((distanceAB / moduleSizeA) - Scalar(10)) / Scalar(4)));
	unsigned int localVersionHigh = (unsigned int)(Numeric::round32(((distanceAB / moduleSizeB) - Scalar(10)) / Scalar(4)));

	if (localVersionLow > localVersionHigh)
	{
		std::swap(localVersionLow, localVersionHigh);
	}

	if (localVersionHigh < 1u || localVersionLow > 40u || (localVersionHigh - localVersionLow) > maxAllowedVersionDifference)
	{
		return false;
	}

	// Clamp version values to the valid range
	versionLow = std::max(1u, localVersionLow);
	versionHigh = std::min(40u, localVersionHigh);
	ocean_assert(versionLow <= versionHigh);

	return true;
}

bool QRCodeDetector::computeModuleSizeInPlane(const AnyCamera& anyCamera, const Plane3& plane, const Vector2& imagePoint, const Vector3& planePoint, const Scalar moduleSizeInImage, Scalar& moduleSizeInPlane)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(plane.isValid() && plane.distance() == Scalar(1));
	ocean_assert(plane.isInPlane(planePoint));
	ocean_assert(moduleSizeInImage > Scalar(0));

	const Vector2 imagePointB = imagePoint + Vector2(moduleSizeInImage, Scalar(0));
	const Vector3 vectorB = anyCamera.vector(imagePointB);
	ocean_assert(vectorB.isUnit());

	Vector3 planePointB;
	if (!plane.intersection(Line3(Vector3(0, 0, 0), vectorB), planePointB))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	moduleSizeInPlane = (planePointB - planePoint).length();
	ocean_assert(moduleSizeInPlane > Scalar(0));

	return true;
}

bool QRCodeDetector::computeProvisionalVersionRange(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, unsigned int& versionX, unsigned int& versionY)
{
	ocean_assert(topLeft.position() != bottomLeft.position() && topLeft.position() != topRight.position());
	ocean_assert(bottomLeft.position() != topRight.position());

	versionX = (unsigned int)(-1);
	versionY = (unsigned int)(-1);

	// Provisional version number along the x-axis (between the top-left and the top-right finder patterns)
	const Scalar moduleSizeX = Scalar(0.5) * (topLeft.moduleSize() + topRight.moduleSize());
	ocean_assert(Numeric::isNotEqualEps(moduleSizeX));
	const Scalar modulesCountX = topLeft.position().distance(topRight.position()) / moduleSizeX;
	versionX = std::max(1u, std::min(40u, ((unsigned int)Numeric::round32((modulesCountX - Scalar(10)) * Scalar(0.25)))));

	// Provisional version number along the y-axis (between the top-left and the bottom-left finder patterns)
	const Scalar moduleSizeY = Scalar(0.5) * (topLeft.moduleSize() + bottomLeft.moduleSize());
	ocean_assert(Numeric::isNotEqualEps(moduleSizeY));
	const Scalar modulesCountY = topLeft.position().distance(bottomLeft.position()) / moduleSizeY;
	versionY = std::max(1u, std::min(40u, ((unsigned int)Numeric::round32((modulesCountY - Scalar(10)) * Scalar(0.25)))));

	// TODO Add a confidence value for `versionX` and `versionY`, e.g., based on the fractional part of the above computations. That could be used to test other versions, too, e.g., `versionX +/- 1`

	return std::abs(int(versionX) - int(versionY)) <= 2;
}

bool QRCodeDetector::extractModulesFromImage(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int version, const HomogenousMatrix4& code_T_camera, const bool isNormalReflectance, const unsigned int grayThreshold, std::vector<uint8_t>& modules, const Scalar scale)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert_and_suppress_unused(yFrame != nullptr && width != 0u && height != 0u, height);
	ocean_assert(version  >= 1u && version <= 40u);
	ocean_assert(code_T_camera.isValid());
	ocean_assert(grayThreshold <= 255u);
	ocean_assert(scale > Scalar(0));

	const unsigned int strideElements = width + paddingElements;

	TransitionDetector::PixelComparisonFunc isForegroundPixel = isNormalReflectance ? TransitionDetector::isLessOrEqual : TransitionDetector::isGreater;

	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(grayThreshold <= 255u);
	ocean_assert(version != 0u && version <= 40u);

	const unsigned int modulesPerSide = QRCode::modulesPerSide(version);
	ocean_assert(modulesPerSide >= 21u);

	modules.resize(modulesPerSide * modulesPerSide, 0u);

	const HomogenousMatrix4 flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(code_T_camera);
	ocean_assert(flippedCamera_T_code.isValid());

	const Utilities::CoordinateSystem coordinateSystem(version, scale);

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
				modules[moduleIndex] = isForegroundPixel(yFrame + pixelOffset, grayThreshold) ? 1u : 0u;
			}

			moduleIndex++;
		}
	}

	return true;
}

bool QRCodeDetector::computePoses(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern* finderPatterns, const unsigned int version, HomogenousMatrices4& code_T_cameras, const Scalar scale)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(finderPatterns != nullptr);
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(scale > Scalar(0));

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	Frame eiComputePosesFrame;

	if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_COMPUTE_POSES))
	{
		ocean_assert(QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24));
		eiComputePosesFrame = QRCodeDebugElements::get().element(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24);
		ocean_assert(eiComputePosesFrame.isValid());
	}
#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	// Compute an initial pose estimate using only the centers of the finder patterns

	// Coordinate system of the QR code object space
	//
	//                 y
	//     (-1, 1, 0)  ^    (1, 1, 0)
	//             +---|---+
	//             |#  |  #|
	//             |   o-----> x
	//             |#      |        z-axis points up
	//             +-------+
	//    (-1, -1, 0)

	// Point correspondences - initially use the 3 finder patterns and, if possible, add
	// correspondences for the alignment patterns that have been detected.
	Vectors2 imagePoints =
	{
		finderPatterns[0].position(), // top-left
		finderPatterns[1].position(), // bottom-left
		finderPatterns[2].position(), // top-right
	};

	Vectors3 objectPoints = Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(version, scale);

	const std::vector<Vectors3> objectAlignmentPatterns = Utilities::CoordinateSystem::computeAlignmentPatternsInObjectSpace(version, scale);
	ocean_assert(version == 1u || !objectAlignmentPatterns.empty());

	unsigned int numberPoses = 0u;
	HomogenousMatrix4 possible_code_T_cameras[4];

	{
		// Temporary work-around to avoid precision problems on mobile.
		HomogenousMatrixD4 possible_code_T_camerasD[4];

		const VectorD2 imagePointsD[3] =
		{
			VectorD2(double(imagePoints[0].x()), double(imagePoints[0].y())),
			VectorD2(double(imagePoints[1].x()), double(imagePoints[1].y())),
			VectorD2(double(imagePoints[2].x()), double(imagePoints[2].y())),
		};

		const VectorD3 objectPointsD[3] =
		{
			VectorD3(double(objectPoints[0].x()), double(objectPoints[0].y()), double(objectPoints[0].z())),
			VectorD3(double(objectPoints[1].x()), double(objectPoints[1].y()), double(objectPoints[1].z())),
			VectorD3(double(objectPoints[2].x()), double(objectPoints[2].y()), double(objectPoints[2].z())),
		};

		numberPoses = Geometry::P3P::poses(anyCamera, objectPointsD, imagePointsD, possible_code_T_camerasD);

		for (unsigned int i = 0u; i < numberPoses; ++i)
		{
			possible_code_T_cameras[i] = HomogenousMatrix4(possible_code_T_camerasD[i]);
			ocean_assert(possible_code_T_cameras[i].isValid());
		}
	}

	if (numberPoses == 0u)
	{
		return false;
	}

	const unsigned int grayThreshold = (unsigned int)(NumericF::round32(float(finderPatterns[0].grayThreshold() + finderPatterns[1].grayThreshold() + finderPatterns[2].grayThreshold()) / 3.0f));
	const bool isNormalReflectance = finderPatterns[0].isNormalReflectance();

	if (version > 1u)
	{
		// Try to determine the image location of the alignment pattern that is closest to the top-left finder pattern and use it as additional correspondence to compute a single pose, if possible
		ocean_assert(!objectAlignmentPatterns.empty() && !objectAlignmentPatterns[0].empty());

		const Vector3 objectAlignmentPattern = objectAlignmentPatterns[0][0];

		const Scalar normalizedModuleSize = Scalar(2) / Scalar(QRCode::modulesPerSide(version));

		Vector2 topLeft(Numeric::maxValue(), Numeric::maxValue());
		Vector2 bottomRight(Numeric::minValue(), Numeric::minValue());

		for (unsigned int iPose = 0u; iPose < numberPoses; ++iPose)
		{
			for (const Scalar offsetInModules : { scale * normalizedModuleSize * Scalar(-6), scale * normalizedModuleSize * Scalar(6) })
			{
				const Vector2 imageAlignmentPattern = anyCamera.projectToImage(possible_code_T_cameras[iPose], objectAlignmentPattern + Vector3(offsetInModules, offsetInModules, Scalar(0)));

				topLeft.x() = std::min(topLeft.x(), imageAlignmentPattern.x());
				topLeft.y() = std::min(topLeft.y(), imageAlignmentPattern.y());

				bottomRight.x() = std::max(bottomRight.x(), imageAlignmentPattern.x());
				bottomRight.y() = std::max(bottomRight.y(), imageAlignmentPattern.y());
			}

			if (!anyCamera.isInside(topLeft) || !anyCamera.isInside(bottomRight) || topLeft.x() >= bottomRight.x() || topLeft.y() >= bottomRight.y())
			{
				continue;
			}

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
			if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_COMPUTE_POSES))
			{
				ocean_assert(eiComputePosesFrame.isValid());
				Utilities::drawLine<1u>(anyCamera, eiComputePosesFrame, topLeft, Vector2(topLeft.x(), bottomRight.y()), CV::Canvas::red(eiComputePosesFrame.pixelFormat()));
				Utilities::drawLine<1u>(anyCamera, eiComputePosesFrame, Vector2(topLeft.x(), bottomRight.y()), bottomRight, CV::Canvas::red(eiComputePosesFrame.pixelFormat()));
				Utilities::drawLine<1u>(anyCamera, eiComputePosesFrame, bottomRight, Vector2(bottomRight.x(), topLeft.y()), CV::Canvas::red(eiComputePosesFrame.pixelFormat()));
				Utilities::drawLine<1u>(anyCamera, eiComputePosesFrame, Vector2(bottomRight.x(), topLeft.y()), topLeft, CV::Canvas::red(eiComputePosesFrame.pixelFormat()));
			}
#endif

			const unsigned int searchX = (unsigned int)Numeric::round32(topLeft.x());
			const unsigned int searchY = (unsigned int)Numeric::round32(topLeft.y());

			const unsigned int searchWidth = (unsigned int)Numeric::round32(bottomRight.x() - topLeft.x());
			const unsigned int searchHeight = (unsigned int)Numeric::round32(bottomRight.y() - topLeft.y());

			if (searchWidth < 5u || searchHeight < 5u)
			{
				continue;
			}

			ocean_assert(searchX + searchWidth <= width);
			ocean_assert(searchY + searchHeight <= height);

			const AlignmentPatterns imageAlignmentPatterns = AlignmentPatternDetector::detectAlignmentPatterns(yFrame, width, height, paddingElements, searchX, searchY, searchWidth, searchHeight, isNormalReflectance, grayThreshold);

			if (imageAlignmentPatterns.size() == 1)
			{
				imagePoints.push_back(imageAlignmentPatterns[0].center());
				objectPoints.push_back(objectAlignmentPattern);

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED) && defined(_WINDOWS)
				if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_COMPUTE_POSES))
				{
					for (const Vector2& imagePoint : imagePoints)
					{
						CV::Canvas::point<5u>(eiComputePosesFrame, imagePoint, CV::Canvas::red(FrameType::FORMAT_RGB24));
					}
				}
#endif

				ocean_assert(imagePoints.size() == objectPoints.size());
				ocean_assert(imagePoints.size() == 4);

				RandomGenerator randomGenerator;
				Indices32 usedIndices;
				HomogenousMatrix4 code_T_camera;

				if (!Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, code_T_camera, /* minimalValidCorrespondences */ 4u, /* refine */ true, /* iterations */ 10u, /* sqrPixelErrorThreshold */ Numeric::sqr(Scalar(5)), &usedIndices))
				{
					imagePoints.pop_back();
					objectPoints.pop_back();
				}
				else
				{
					possible_code_T_cameras[0] = code_T_camera;
					numberPoses = 1u;

					break;
				}
			}
		}
	}

	code_T_cameras.reserve(numberPoses);
	for (unsigned int i = 0u; i < numberPoses; ++i)
	{
		code_T_cameras.emplace_back(possible_code_T_cameras[i]);
	}

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_COMPUTE_POSES))
	{
		ocean_assert(eiComputePosesFrame.isValid());

		for (unsigned int i = 0u; i < numberPoses; ++i)
		{
			Utilities::drawQRCodeOutline(anyCamera, eiComputePosesFrame, version, possible_code_T_cameras[i]);
		}

		QRCodeDebugElements::get().updateElement(QRCodeDebugElements::EI_COMPUTE_POSES, eiComputePosesFrame);
	}
#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	return numberPoses != 0u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
