/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/QRCodeDetector3D.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/P3P.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

bool QRCodeDetector3D::detectQRCodes(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, QRCodes& codes, HomogenousMatrices4& world_T_codes, Scalars& codeSizes, Worker* worker, const bool allow2DCodes)
{
	ocean_assert(sharedAnyCameras.size() == yFrames.size());
	ocean_assert(device_T_cameras.size() == yFrames.size());
	ocean_assert(world_T_device.isValid());

#if defined(OCEAN_DEBUG)
	for (size_t i = 0; i < sharedAnyCameras.size(); ++i)
	{
		ocean_assert(yFrames[i].isValid());
		ocean_assert(yFrames[i].width() >= 29u && yFrames[i].height() >= 29u);
		ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[i].pixelFormat(), FrameType::FORMAT_Y8));

		ocean_assert(sharedAnyCameras[i] != nullptr && sharedAnyCameras[i]->isValid());

		ocean_assert(yFrames[i].width() == sharedAnyCameras[i]->width() && yFrames[i].height() == sharedAnyCameras[i]->height());
	}

	for (const HomogenousMatrix4& device_T_camera : device_T_cameras)
	{
		ocean_assert(device_T_camera.isValid());
	}
#endif

	if (yFrames.size() != 2 || yFrames.size() != sharedAnyCameras.size())
	{
		ocean_assert(false && "TODO Currently this detector only supports cases with exactly two cameras, not the more general case of N > 2.");
		return false;
	}

	codes.clear();
	world_T_codes.clear();
	codeSizes.clear();

	std::vector<FinderPatterns> finderPatterns(yFrames.size());
	std::vector<IndexTriplets> indexTriplets(yFrames.size());

	for (size_t iCamera = 0; iCamera < yFrames.size(); ++iCamera)
	{
		const Frame& yFrame = yFrames[iCamera];

		finderPatterns[iCamera] = FinderPatternDetector::detectFinderPatterns(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), /* minimumDistance */ 10u, yFrame.paddingElements(), worker);

		constexpr size_t maximumNumberOfDetectableCodes = 5;
		constexpr size_t maximumNumberOfFinderPatterns = 3 * maximumNumberOfDetectableCodes;

		if (finderPatterns[iCamera].size() >= 3 && finderPatterns[iCamera].size() <= maximumNumberOfFinderPatterns)
		{
			indexTriplets[iCamera] = FinderPatternDetector::extractIndexTriplets(finderPatterns[iCamera]);
		}
	}

	for (size_t iCameraA = 0; iCameraA < indexTriplets.size() - 1; ++iCameraA)
	{
		const FinderPatterns& finderPatternsA = finderPatterns[iCameraA];

		const Frame& yFrameA = yFrames[iCameraA];
		const SharedAnyCamera& sharedAnyCameraA = sharedAnyCameras[iCameraA];

		const HomogenousMatrix4 device_T_cameraA = device_T_cameras[iCameraA];

		for (size_t iPatternA = 0; iPatternA < indexTriplets[iCameraA].size(); ++iPatternA)
		{
			const IndexTriplet& indexTripletA = indexTriplets[iCameraA][iPatternA];

			const bool isNormalReflectance = finderPatternsA[indexTripletA[0]].isNormalReflectance();

			if (finderPatternsA[indexTripletA[1]].isNormalReflectance() != isNormalReflectance ||
				finderPatternsA[indexTripletA[2]].isNormalReflectance() != isNormalReflectance)
			{
				continue;
			}

			const FinderPatternTriplet finderPatternTripletA =
			{
				finderPatternsA[indexTripletA[0]],
				finderPatternsA[indexTripletA[1]],
				finderPatternsA[indexTripletA[2]],
			};

			// Detection

			for (size_t iCameraB = iCameraA + 1; iCameraB < indexTriplets.size(); ++iCameraB)
			{
				const FinderPatterns& finderPatternsB = finderPatterns[iCameraB];

				const Frame& yFrameB = yFrames[iCameraB];
				const SharedAnyCamera& sharedAnyCameraB = sharedAnyCameras[iCameraB];

				const HomogenousMatrix4 device_T_cameraB = device_T_cameras[iCameraB];

				for (size_t iPatternB = 0; iPatternB < indexTriplets[iCameraB].size(); ++iPatternB)
				{
					const IndexTriplet& indexTripletB = indexTriplets[iCameraB][iPatternB];

					if (finderPatternsB[indexTripletB[0]].isNormalReflectance() != isNormalReflectance ||
						finderPatternsB[indexTripletB[1]].isNormalReflectance() != isNormalReflectance ||
						finderPatternsB[indexTripletB[2]].isNormalReflectance() != isNormalReflectance)
					{
						continue;
					}

					const FinderPatternTriplet finderPatternTripletB =
					{
						finderPatternsB[indexTripletB[0]],
						finderPatternsB[indexTripletB[1]],
						finderPatternsB[indexTripletB[2]],
					};

					Vectors3 worldPoints;
					Scalar averageFinderPatternCenterDistance;

					if (!triangulateFinderPatternsStereo(sharedAnyCameraA, sharedAnyCameraB, world_T_device, device_T_cameraA, device_T_cameraB, finderPatternTripletA, finderPatternTripletB, worldPoints, averageFinderPatternCenterDistance))
					{
						continue;
					}

					ocean_assert(worldPoints.size() == 3);
					ocean_assert(averageFinderPatternCenterDistance > Scalar(0));

					CV::Detector::QRCodes::QRCode code;
					HomogenousMatrix4 world_T_code;
					Scalar codeSize;

					if (!computePoseAndExtractQRCodeStereo(yFrameA, yFrameB, sharedAnyCameraA, sharedAnyCameraB, world_T_device, device_T_cameraA, device_T_cameraB, finderPatternTripletA, finderPatternTripletB, worldPoints, averageFinderPatternCenterDistance, code, world_T_code, codeSize, /* version */ (unsigned int)(-1)))
					{
						continue;
					}

					ocean_assert(code.isValid());
					ocean_assert(world_T_code.isValid());
					ocean_assert(codeSize > Scalar(0));

					codes.emplace_back(std::move(code));
					world_T_codes.emplace_back(std::move(world_T_code));
					codeSizes.emplace_back(codeSize);
				}
			}
		}
	}

	ocean_assert(codes.size() == world_T_codes.size());
	ocean_assert(codes.size() == codeSizes.size());

	if (codes.empty() && allow2DCodes)
	{
		for (const size_t iCamera : {0, 1})
		{
			const AnyCamera& camera = *sharedAnyCameras[iCamera];
			const Frame& yFrame = yFrames[iCamera];

			QRCodes codes2D = QRCodeDetector2D::detectQRCodes(camera, yFrame, /* observations */ nullptr, worker);

			// TODO Use the observations to see if it's possible to estimate a rough size and pose of the current code (e.g. using the other camera image)

			for (QRCode& code2D : codes2D)
			{
				if (!Utilities::containsCode(codes, code2D))
				{
					codes.emplace_back(std::move(code2D));
					world_T_codes.emplace_back(getInvalidWorld_T_code());
					codeSizes.emplace_back(getInvalidCodeSize());
				}
			}
		}
	}

	ocean_assert(codes.size() == world_T_codes.size());
	ocean_assert(codes.size() == codeSizes.size());

#if defined(OCEAN_DEBUG)
	for (size_t codeIndex = 0; codeIndex < codes.size(); ++codeIndex)
	{
		ocean_assert(codes[codeIndex].isValid());

		if (allow2DCodes)
		{
			ocean_assert((codeSizes[codeIndex] > Scalar(0) && world_T_codes[codeIndex].isValid())
				|| (codeSizes[codeIndex] <= Scalar(0) && !world_T_codes[codeIndex].isValid()));
		}
		else
		{
			ocean_assert(codeSizes[codeIndex] > Scalar(0) && world_T_codes[codeIndex].isValid());
		}
	}
#endif // OCEAN_DEBUG

	return true;
}

bool QRCodeDetector3D::detectQRCodesWithPyramids(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, QRCodes& codes, HomogenousMatrices4& world_T_codes, Scalars& codeSizes, Worker* worker, const bool allow2DCodes)
{
	ocean_assert(sharedAnyCameras.size() == 2);
	ocean_assert(sharedAnyCameras.size() == yFrames.size());
	ocean_assert(sharedAnyCameras.size() == device_T_cameras.size());

#if defined(OCEAN_DEBUG)
	for (size_t cameraIndex = 0; cameraIndex < sharedAnyCameras.size(); ++cameraIndex)
	{
		ocean_assert(yFrames[cameraIndex].isValid());
		ocean_assert(yFrames[cameraIndex].width() >= 29u && yFrames[cameraIndex].height() >= 29u);
		ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[cameraIndex].pixelFormat(), FrameType::FORMAT_Y8));

		ocean_assert(sharedAnyCameras[cameraIndex] != nullptr && sharedAnyCameras[cameraIndex]->isValid());
		ocean_assert(sharedAnyCameras[cameraIndex]->width() == yFrames[cameraIndex].width());
		ocean_assert(sharedAnyCameras[cameraIndex]->height() == yFrames[cameraIndex].height());

		ocean_assert(device_T_cameras[cameraIndex].isValid());
	}
#endif

	Frame firstLayers[2];

	if (yFrames[0].width() <= 1920u && yFrames[0].height() <= 1920u)
	{
		// Use the input frames as-is (for backward-compatibility with devices like Quest 2 and Quest Pro)
		firstLayers[0] = Frame(yFrames[0], Frame::ACM_USE_KEEP_LAYOUT);
		firstLayers[1] = Frame(yFrames[1], Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
/*
		// Disabled for now, but may be enabled in the future
		constexpr bool useFullSizeYFrames = false;

		if (useFullSizeYFrames)
		{
			// Use the input frames but because of their size apply some light blur to reduce noise
			CV::FrameFilterGaussian::filter(yFrames[0], firstLayers[0], 3u, worker);
			CV::FrameFilterGaussian::filter(yFrames[1], firstLayers[1], 3u, worker);
		}
		else
*/
		{
			// Down-sample the input frames for performance reasons
			CV::FrameShrinker::downsampleByTwo14641(yFrames[0], firstLayers[0], worker);
			CV::FrameShrinker::downsampleByTwo14641(yFrames[1], firstLayers[1], worker);

			while (firstLayers[0].width() > 1920u || firstLayers[0].height() > 1920u)
			{
				Frame downsampledFrames[2];

				CV::FrameShrinker::downsampleByTwo14641(firstLayers[0], downsampledFrames[0], worker);
				CV::FrameShrinker::downsampleByTwo14641(firstLayers[1], downsampledFrames[1], worker);

				firstLayers[0] = std::move(downsampledFrames[0]);
				firstLayers[1] = std::move(downsampledFrames[1]);

				ocean_assert(firstLayers[0].isValid());
				ocean_assert(firstLayers[1].isValid());
			}
		}
	}

	ocean_assert(firstLayers[0].isValid());
	ocean_assert(firstLayers[1].isValid());

	constexpr unsigned int numberLayers = 3u;
	const FramePyramid framePyramids[2] =
	{
		FramePyramid(firstLayers[0], FramePyramid::DM_FILTER_14641, numberLayers, false /*copyFirstLayer*/, worker),
		FramePyramid(firstLayers[1], FramePyramid::DM_FILTER_14641, numberLayers, false /*copyFirstLayer*/, worker)
	};

	ocean_assert(framePyramids[0].isValid() && framePyramids[0].layers() == numberLayers);
	ocean_assert(framePyramids[1].isValid() && framePyramids[1].layers() == numberLayers);

	QRCodes filteredCodes;
	Scalars filteredCodeSizes;
	HomogenousMatrices4 world_T_filteredCodes;

	QRCodes filteredCodes2D;

	for (unsigned int layerIndex = 0u; layerIndex < framePyramids[0].layers(); ++layerIndex)
	{
		ocean_assert(layerIndex < framePyramids[1].layers());

		const unsigned int layerWidth = framePyramids[0].layer(layerIndex).width();
		const unsigned int layerHeight = framePyramids[0].layer(layerIndex).height();

		const Frames layerFrames =
		{
			Frame(framePyramids[0].layer(layerIndex), Frame::ACM_USE_KEEP_LAYOUT),
			Frame(framePyramids[1].layer(layerIndex), Frame::ACM_USE_KEEP_LAYOUT),
		};

		const SharedAnyCameras layerCameras =
		{
			sharedAnyCameras[0]->clone(layerWidth, layerHeight),
			sharedAnyCameras[1]->clone(layerWidth, layerHeight),
		};

		if (layerCameras[0] == nullptr || !layerCameras[0]->isValid() ||
			layerCameras[1] == nullptr || !layerCameras[1]->isValid())
		{
			Log::error() << "Invalid camera(s) in pyramid layer " << layerIndex;
			continue;
		}

		QRCodes newCodes;
		Scalars newCodeSizes;
		HomogenousMatrices4 world_T_newCodes;
		if (detectQRCodes(layerCameras, layerFrames, world_T_device, device_T_cameras, newCodes, world_T_newCodes, newCodeSizes, worker, allow2DCodes))
		{
			for (size_t newCodeIndex = 0; newCodeIndex < newCodes.size(); ++newCodeIndex)
			{
				QRCode& newCode = newCodes[newCodeIndex];
				ocean_assert(newCode.isValid());

				const Scalar newCodeSize = newCodeSizes[newCodeIndex];
				HomogenousMatrix4& world_T_newCode = world_T_newCodes[newCodeIndex];

				const bool is2DCode = newCodeSize <= Scalar(0) && !world_T_newCode.isValid();

				if (is2DCode)
				{
					if (!Utilities::containsCode(filteredCodes, newCode))
					{
						filteredCodes2D.emplace_back(std::move(newCode));
					}
				}
				else
				{
					if (!Utilities::containsCodeStereo(sharedAnyCameras, world_T_device, device_T_cameras, filteredCodes, world_T_filteredCodes, filteredCodeSizes, newCode, world_T_newCode, newCodeSize))
					{
						filteredCodes.emplace_back(std::move(newCode));
						filteredCodeSizes.emplace_back(newCodeSize);
						world_T_filteredCodes.emplace_back(world_T_newCode);
					}
				}
			}
		}
	}

	// Now add those 2D codes if they had not been detected before already.
	for (QRCode& filteredCode2D : filteredCodes2D)
	{
		if (!Utilities::containsCode(filteredCodes, filteredCode2D))
		{
			filteredCodes.emplace_back(std::move(filteredCode2D));
			filteredCodeSizes.emplace_back(getInvalidCodeSize());
			world_T_filteredCodes.emplace_back(getInvalidWorld_T_code());
		}
	}

	codes = std::move(filteredCodes);
	codeSizes = std::move(filteredCodeSizes);
	world_T_codes = std::move(world_T_filteredCodes);

	return true;
}

bool QRCodeDetector3D::triangulateFinderPatternsStereo(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, Vectors3& worldPoints, Scalar& averageFinderPatternCenterDistance)
{
	ocean_assert(sharedAnyCameraA != nullptr && sharedAnyCameraA->isValid());
	ocean_assert(sharedAnyCameraB != nullptr && sharedAnyCameraB->isValid());
	ocean_assert(world_T_device.isValid() && device_T_cameraA.isValid() && device_T_cameraB.isValid());

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

	const Vector2 imagePointsA[3] =
	{
		finderPatternTripletA[0].position(),
		finderPatternTripletA[1].position(),
		finderPatternTripletA[2].position(),
	};

	const Vector2 imagePointsB[3] =
	{
		finderPatternTripletB[0].position(),
		finderPatternTripletB[1].position(),
		finderPatternTripletB[2].position(),
	};

	Indices32 invalidIndices;
	worldPoints = Geometry::EpipolarGeometry::triangulateImagePoints(world_T_cameraA, world_T_cameraB, *sharedAnyCameraA, *sharedAnyCameraB, imagePointsA, imagePointsB, /* numberPoints */ 3, /* onlyFrontObjectPoints */ true, /* invalidObjectPoint */ Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), &invalidIndices);

	if (worldPoints.size() != 3 || !invalidIndices.empty())
	{
		return false;
	}

#if defined(OCEAN_DEBUG)
	const HomogenousMatrix4 flippedCameraA_T_world = PinholeCamera::standard2InvertedFlipped(world_T_cameraA);
	const HomogenousMatrix4 flippedCameraB_T_world = PinholeCamera::standard2InvertedFlipped(world_T_cameraB);

	for (size_t iPoint = 0; iPoint < 3; ++iPoint)
	{
		ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedCameraA_T_world, worldPoints[iPoint]));
		ocean_assert(PinholeCamera::isObjectPointInFrontIF(flippedCameraB_T_world, worldPoints[iPoint]));
	}
#endif

	// TODO Check the reprojection error of the triangulated points because Geometry::EpipolarGeometry::triangulateImagePoints() does not handle outliers.

	// In the object space, check if the three centers of the finder patterns roughly form a square by checking
	// the distances between them, i.e. a^2 + b^2 = c^2

	const Scalar sqrVerticalDistance = worldPoints[0].sqrDistance(worldPoints[1]);
	ocean_assert(sqrVerticalDistance > Scalar(0));

	const Scalar minSqrDistance = Scalar(0.85) * sqrVerticalDistance;
	const Scalar maxSqrDistance = Scalar(1.15) * sqrVerticalDistance;

	const Scalar sqrHorizontalDistance = worldPoints[0].sqrDistance(worldPoints[2]);
	ocean_assert(sqrHorizontalDistance > Scalar(0));

	if (sqrHorizontalDistance < minSqrDistance || sqrHorizontalDistance > maxSqrDistance)
	{
		return false;
	}

	const Scalar sqrDiagonalDistance = worldPoints[1].sqrDistance(worldPoints[2]);
	ocean_assert(sqrDiagonalDistance > Scalar(0));

	if (sqrDiagonalDistance < Scalar(2) * minSqrDistance || sqrDiagonalDistance > Scalar(2) * maxSqrDistance)
	{
		return false;
	}

	// Both triplets appear to be roughly square.
	//
	// Now, determine the average vertical/horizontal distance between the centers of the finder patterns. From the test above, we know that a = b = x:
	//
	//     a^2 + b^2 = c^2
	// <=> x^2 + x^2 = 2 * x^2
	//
	//  => avg = sqrt((a^2 + b^2 + c^2) / 4)
	averageFinderPatternCenterDistance = Numeric::sqrt(Scalar(0.25) * (sqrVerticalDistance + sqrHorizontalDistance + sqrDiagonalDistance));
	ocean_assert(averageFinderPatternCenterDistance > Scalar(0));

	return true;
}

bool QRCodeDetector3D::computePoseAndExtractQRCodeStereo(const Frame& yFrameA, const Frame& yFrameB, const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, const Vectors3& worldPoints, const Scalar& averageFinderPatternCenterDistance, CV::Detector::QRCodes::QRCode& code, HomogenousMatrix4& world_T_code, Scalar& codeSize, const unsigned int version)
{
	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

	const unsigned int grayThresholdA = ((finderPatternTripletA[0].grayThreshold() + finderPatternTripletA[1].grayThreshold() + finderPatternTripletA[2].grayThreshold()) + 1u) / 3u;
	const unsigned int grayThresholdB = ((finderPatternTripletB[0].grayThreshold() + finderPatternTripletB[1].grayThreshold() + finderPatternTripletB[2].grayThreshold()) + 1u) / 3u;
	ocean_assert(grayThresholdA < 256u && grayThresholdB < 256u);

	const bool isNormalReflectance = finderPatternTripletA[0].isNormalReflectance();
	// TODO double-check against B

	unsigned int versionLow = (unsigned int)(-1);
	unsigned int versionHigh = (unsigned int)(-1);

	if (version == 0u || version > 40u)
	{
		constexpr unsigned int maxAllowedVersionDifference = 5u;

		unsigned int versionLowA = (unsigned int)(-1);
		unsigned int versionHighA = 0u;

		unsigned int versionLowB = (unsigned int)(-1);
		unsigned int versionHighB = 0u;

		if (!computeProvisionalVersionRange(*sharedAnyCameraA, finderPatternTripletA.data(), maxAllowedVersionDifference, versionLowA, versionHighA) &&
			!computeProvisionalVersionRange(*sharedAnyCameraB, finderPatternTripletB.data(), maxAllowedVersionDifference, versionLowB, versionHighB))
		{
			return false;
		}

		ocean_assert(versionLowA <= versionHighA || versionLowB <= versionHighB);

		versionLow = std::min(versionLowA, versionLowB);
		versionHigh = std::max(versionHighA, versionHighB);
	}
	else
	{
		versionLow = version;
		versionHigh = version;
	}

	ocean_assert(versionLow >= 1u && versionHigh <= 40u && versionLow <= versionHigh);

	// The true version number is likely somewhere in the middle between the endpoints of the version range.
	// So, iterate through the version range from the center to its ends in an alternating fashion.
	//
	// Example:
	//   versionLow = 2
	//   versionHigh = 7
	//   version order = { 4, 5, 3, 6, 2, 7 }
	unsigned int versionDown = (versionLow + versionHigh) / 2u;
	unsigned int versionUp = versionDown + 1u;

	bool goDown = true;

	while (versionDown >= versionLow || versionUp <= versionHigh) // TODO Replace with alternating for-loop
	{
		ocean_assert(versionLow != 0u || versionHigh < 40u);

		if (goDown && versionDown == 0u)
		{
			goDown = false;
		}
		else if (!goDown && versionUp > 40u)
		{
			goDown = true;
		}

		const unsigned int currentVersion = goDown ? versionDown : versionUp;
		ocean_assert(currentVersion >= 1u && currentVersion <= 40u);

		// Estimate the code size based on the current version and the distance between the finder
		// patterns (as measured by the triangulated points); it's 3.5 modules from the center of
		// finder pattern to the nearest border.
		const unsigned int modulesPerSide = QRCode::modulesPerSide(currentVersion);
		ocean_assert(modulesPerSide >= 8u);

		const Scalar estimatedCodeSize = averageFinderPatternCenterDistance / Scalar(modulesPerSide - 7u) * Scalar(modulesPerSide);

		// Compute a pose
		HomogenousMatrix4 internalWorld_T_code(false);

		if (computePoseStereo(sharedAnyCameraA, sharedAnyCameraB, yFrameA, yFrameB, world_T_device, device_T_cameraA, device_T_cameraB, finderPatternTripletA, finderPatternTripletB, worldPoints.data(), currentVersion, estimatedCodeSize, internalWorld_T_code))
		{
			const Scalar estimatedCodeSize_2 = Scalar(0.5) * estimatedCodeSize;

			for (const bool useCameraA : { true, false })
			{
				const SharedAnyCamera& anyCamera = useCameraA ? sharedAnyCameraA : sharedAnyCameraB;

				const Frame& yFrame = useCameraA ? yFrameA : yFrameB;

				const HomogenousMatrix4& world_T_camera = useCameraA ? world_T_cameraA : world_T_cameraB;

				const HomogenousMatrix4 code_T_camera = internalWorld_T_code.inverted() * world_T_camera;

				const unsigned int grayThreshold = useCameraA ? grayThresholdA : grayThresholdB;

				std::vector<uint8_t> modules;
				if (!extractModulesFromImage(*anyCamera, yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), currentVersion, code_T_camera, isNormalReflectance, grayThreshold, modules, estimatedCodeSize_2))
				{
					continue;
				}

				QRCode internalCode;
				if (QRCodeDecoder::decodeQRCode(modules, internalCode))
				{
					ocean_assert(internalCode.isValid());

					code = std::move(internalCode);
					world_T_code = std::move(internalWorld_T_code);
					codeSize = estimatedCodeSize;

					return true;
				}
			}
		}

		if (goDown)
		{
			versionDown -= 1u;
		}
		else
		{
			versionUp += 1u;
		}
	}

	return false;
}

bool QRCodeDetector3D::computePoseStereo(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, const Vector3* worldPoints, const unsigned int version, const Scalar codeSize, HomogenousMatrix4& world_T_code)
{
	ocean_assert(sharedAnyCameraA != nullptr && sharedAnyCameraA->isValid());
	ocean_assert(sharedAnyCameraB != nullptr && sharedAnyCameraB->isValid());
	ocean_assert(yFrameA.isValid() && FrameType::arePixelFormatsCompatible(yFrameA.pixelFormat(), FrameType::FORMAT_Y8));
	ocean_assert(yFrameB.isValid() && FrameType::arePixelFormatsCompatible(yFrameB.pixelFormat(), FrameType::FORMAT_Y8));
	ocean_assert(world_T_device.isValid() && device_T_cameraA.isValid() && device_T_cameraB.isValid());
	ocean_assert_and_suppress_unused(worldPoints != nullptr, worldPoints);
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(codeSize > 0);

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

	const HomogenousMatrix4 cameraA_T_world = world_T_cameraA.inverted();
	const HomogenousMatrix4 cameraB_T_world = world_T_cameraB.inverted();

	const Vectors3 normalizedObjectPoints = Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(version);
	ocean_assert(normalizedObjectPoints.size() == 3);

	const Scalar codeSize_2 = Scalar(0.5) * codeSize;

	const Vectors3 objectPoints =
	{
		normalizedObjectPoints[0] * codeSize_2,
		normalizedObjectPoints[1] * codeSize_2,
		normalizedObjectPoints[2] * codeSize_2,
	};

	// Compute an initial pose

	HomogenousMatrix4 minWorld_T_code(false);
	Scalar minSquaredProjectionError = Numeric::maxValue();

	for (const bool useCameraA : { true, false })
	{
		const SharedAnyCamera& anyCamera0 = useCameraA ? sharedAnyCameraA : sharedAnyCameraB;
		const SharedAnyCamera& anyCamera1 = useCameraA ? sharedAnyCameraB : sharedAnyCameraA;

		const Frame& yFrame0 = useCameraA ? yFrameA : yFrameB;

		const HomogenousMatrix4& world_T_camera0 = useCameraA ? world_T_cameraA : world_T_cameraB;
		const HomogenousMatrix4& world_T_camera1 = useCameraA ? world_T_cameraB : world_T_cameraA;

		const HomogenousMatrix4 camera0_T_world = useCameraA ? cameraA_T_world : cameraB_T_world;
		const HomogenousMatrix4 camera1_T_world = useCameraA ? cameraB_T_world : cameraA_T_world;

		const FinderPatternTriplet finderPatternTriplet0 = useCameraA ? finderPatternTripletA : finderPatternTripletB;
		const FinderPatternTriplet finderPatternTriplet1 = useCameraA ? finderPatternTripletB : finderPatternTripletA;

		HomogenousMatrices4 code_T_cameras0;

		if (computePoses(*anyCamera0, yFrame0.constdata<uint8_t>(), yFrame0.width(), yFrame0.height(), yFrame0.paddingElements(), finderPatternTriplet0.data(), version, code_T_cameras0, codeSize_2) && !code_T_cameras0.empty())
		{
			Scalar minSquaredProjectionErrorCurrent = Numeric::maxValue();
			size_t minIndex = 0;

			for (size_t i = 0; i < code_T_cameras0.size(); ++i)
			{
				const Scalar squareProjectionError = computeSquaredProjectionError(anyCamera1, world_T_camera1, world_T_camera0 * code_T_cameras0[i].inverted(), finderPatternTriplet1, version, codeSize_2);

				if (squareProjectionError < minSquaredProjectionErrorCurrent)
				{
					minSquaredProjectionErrorCurrent = squareProjectionError;
					minIndex = i;
				}
			}

			if (minSquaredProjectionErrorCurrent < minSquaredProjectionError)
			{
				minWorld_T_code = world_T_camera0 * code_T_cameras0[minIndex].inverted();
				minSquaredProjectionError = minSquaredProjectionErrorCurrent;
			}
		}
	}

	if (minWorld_T_code.isValid())
	{
		// TODO Optimize the pose using stereo

		world_T_code = minWorld_T_code;

		return true;
	}

	return false;
}

Scalar QRCodeDetector3D::computeSquaredProjectionError(const SharedAnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const FinderPatternTriplet& finderPatternTriplet, const unsigned int version, const Scalar codeSize_2)
{
	ocean_assert(anyCamera != nullptr && anyCamera->isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(codeSize_2 > Scalar(0));

	const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);
	ocean_assert(flippedCamera_T_world.isValid());

	const Vectors3 objectFinderPatternCenters = Utilities::CoordinateSystem::computeFinderPatternCentersInObjectSpace(version, codeSize_2);

	Scalar sqrSum = Scalar(0);

	for (size_t i = 0; i < 3; ++i)
	{
		const Vector2 projectImagePoint = anyCamera->projectToImageIF(flippedCamera_T_world * world_T_code * objectFinderPatternCenters[i]);

		sqrSum += finderPatternTriplet[i].position().sqrDistance(projectImagePoint);
	}

	return sqrSum;
};


} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
