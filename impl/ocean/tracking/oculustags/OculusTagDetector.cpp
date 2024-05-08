/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/oculustags/OculusTagDetector.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/geometry/NonLinearOptimizationTransformation.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/oculustags/OculusTagDebugElements.h"
#include "ocean/tracking/oculustags/QuadDetector.h"
#include "ocean/tracking/oculustags/Utilities.h"
#include "ocean/tracking/oculustags/OculusTagTracker.h"

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	#include "ocean/cv/Canvas.h"
	#include "ocean/cv/detector/Utilities.h"
#endif

#include <set>

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

OculusTags OculusTagDetector::detectOculusTags(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(yFrame.width() > 2u * yFrameBorder && yFrame.height() > 2u * yFrameBorder);
	ocean_assert(device_T_camera.isValid());
	ocean_assert(defaultTagSize > 0);

	const QuadDetector::Quads boundaryPatterns = QuadDetector::detectQuads(yFrame, yFrameBorder);

	OculusTags oculusTags;

	// Definition of the 3D coordinate system:
	//  * the x-axis points to the right
	//  * the y-axis points up
	//  * the z-axis points toward the camera (camera looks along the negative z-axis)
	//
	// The origin is defined at the first point of `boundaryPattern`:
	//
	// z   y
	//  \  ^
	//   \ |     x
	//     0---->---3
	//     |        |
	//     |        |
	//     |        |
	//     1--------2
	//
	// Note: in the figure above the z-axis points up, not down (i.e. it's a right-handed system)
	const Vectors3 tagPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_0_TO_3, defaultTagSize);

	Vectors2 imagePoints(4);

	for (const BoundaryPattern& unorientedBoundaryPattern : boundaryPatterns)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			imagePoints[i] = unorientedBoundaryPattern[i];
		}

		HomogenousMatrix4 unorientedTag_T_camera(false);

		if (computePose(anyCamera, imagePoints, tagPoints, unorientedTag_T_camera))
		{
			ocean_assert(unorientedTag_T_camera.isNull() == false);

			OculusTag::ReflectanceType reflectanceType;
			uint8_t intensityThreshold;
			uint8_t binaryModuleValueDark;

			if (determineReflectanceTypeAndIntensityThreshold(anyCamera, yFrame, unorientedTag_T_camera, defaultTagSize, reflectanceType, intensityThreshold, binaryModuleValueDark))
			{
				ocean_assert(binaryModuleValueDark <= 1u);

				HomogenousMatrix4 tag_T_camera;
				BoundaryPattern boundaryPattern;

				if (determineOrientation(anyCamera, yFrame, unorientedBoundaryPattern, unorientedTag_T_camera, defaultTagSize, boundaryPattern, tag_T_camera, intensityThreshold, binaryModuleValueDark))
				{
					ocean_assert(tag_T_camera.isValid());

					OculusTag::DataMatrix dataMatrix;

					if (readDataMatrix(anyCamera, yFrame, tag_T_camera, defaultTagSize, intensityThreshold, binaryModuleValueDark, (1u - binaryModuleValueDark), dataMatrix))
					{
						uint32_t tagID = uint32_t(-1);

						if (Utilities::decode(dataMatrix, tagID))
						{
							// Check if the default tag size applies for this tag or if a different size has been specified

							const TagSizeMap::const_iterator tagSizeIter = tagSizeMap.find(tagID);
							const Scalar tagSize = tagSizeIter != tagSizeMap.cend() ? tagSizeIter->second : defaultTagSize;
							ocean_assert(tagSize > 0);

							if (tagSize != defaultTagSize)
							{
								// The tag size has been changed. The rotational part of the pose does not change but the translation
								// has to be scaled by the ratio of the actual tag size and the default one.
								ocean_assert(defaultTagSize > 0);
								tag_T_camera.setTranslation(tag_T_camera.translation() * (tagSize / defaultTagSize));
							}

							if (optimizePose(anyCamera, yFrame, world_T_device, device_T_camera, tag_T_camera, boundaryPattern, tagSize, dataMatrix))
							{
								oculusTags.emplace_back(tagID, reflectanceType, intensityThreshold, world_T_device * device_T_camera * tag_T_camera.inverted(), tagSize);
							}
						}
					}
				}
			}
		}
	}

	return oculusTags;
}

OculusTags OculusTagDetector::detectOculusTags(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());

	ocean_assert(yFrameA.isValid() && FrameType::arePixelFormatsCompatible(yFrameA.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrameA.width() == anyCameraA.width() && yFrameA.height() == yFrameA.height());
	ocean_assert(yFrameA.width() > 2u * yFrameBorder && yFrameA.height() > 2u * yFrameBorder);

	ocean_assert(yFrameB.isValid() && FrameType::arePixelFormatsCompatible(yFrameB.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrameB.width() == anyCameraB.width() && yFrameB.height() == yFrameB.height());
	ocean_assert(yFrameB.width() > 2u * yFrameBorder && yFrameB.height() > 2u * yFrameBorder);

	ocean_assert(yFrameA.frameType() == yFrameB.frameType());

	ocean_assert(world_T_device.isNull() == false);
	ocean_assert(device_T_cameraA.isNull() == false);
	ocean_assert(device_T_cameraB.isNull() == false);

	ocean_assert(defaultTagSize > 0);

	// TODOX What's the 'dominant' camera, A or B?
	const QuadDetector::Quads boundaryPatternsA = QuadDetector::detectQuads(yFrameA, yFrameBorder);

	OculusTags oculusTags;

	// Definition of the 3D coordinate system:
	//  * the x-axis points to the right
	//  * the y-axis points up
	//  * the z-axis points toward the camera (camera looks along the negative z-axis)
	//
	// The origin is defined at the first point of `boundaryPattern`:
	//
	// z   y
	//  \  ^
	//   \ |     x
	//     0---->---3
	//     |        |
	//     |        |
	//     |        |
	//     1--------2
	const Vectors3 tagPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_0_TO_3, defaultTagSize);

	Vectors2 imagePoints;
	imagePoints.resize(4);

	for (const BoundaryPattern& unorientedBoundaryPattern : boundaryPatternsA)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			imagePoints[i] = unorientedBoundaryPattern[i];
		}

		HomogenousMatrix4 unorientedTag_T_camera(false);

		if (computePose(anyCameraA, imagePoints, tagPoints, unorientedTag_T_camera))
		{
			ocean_assert(unorientedTag_T_camera.isNull() == false);

			OculusTag::ReflectanceType reflectanceType;
			uint8_t intensityThreshold;
			uint8_t binaryModuleValueDark;

			if (determineReflectanceTypeAndIntensityThreshold(anyCameraA, yFrameA, unorientedTag_T_camera, defaultTagSize, reflectanceType, intensityThreshold, binaryModuleValueDark))
			{
				ocean_assert(binaryModuleValueDark <= 1u);

				HomogenousMatrix4 tag_T_cameraA;
				BoundaryPattern boundaryPattern;

				if (determineOrientation(anyCameraA, yFrameA, unorientedBoundaryPattern, unorientedTag_T_camera, defaultTagSize, boundaryPattern, tag_T_cameraA, intensityThreshold, binaryModuleValueDark))
				{
					ocean_assert(tag_T_cameraA.isNull() == false);

					OculusTag::DataMatrix dataMatrix;

					if (readDataMatrix(anyCameraA, yFrameA, tag_T_cameraA, defaultTagSize, intensityThreshold, binaryModuleValueDark, (1u - binaryModuleValueDark), dataMatrix))
					{
						uint32_t tagID = uint32_t(-1);

						if (Utilities::decode(dataMatrix, tagID))
						{
							// Check if the default tag size applies for this tag or if a different size has been specified

							const TagSizeMap::const_iterator tagSizeIter = tagSizeMap.find(tagID);
							const Scalar tagSize = tagSizeIter != tagSizeMap.cend() ? tagSizeIter->second : defaultTagSize;
							ocean_assert(tagSize > 0);

							if (tagSize != defaultTagSize)
							{
								// The tag size has been changed. The rotational part of the pose does not change but the translation
								// has to be scaled by the ratio of the actual tag size and the default one.
								ocean_assert(defaultTagSize > 0);
								tag_T_cameraA.setTranslation(tag_T_cameraA.translation() * (tagSize / defaultTagSize));
							}

							if (optimizePose(anyCameraA, anyCameraB, yFrameA, yFrameB, world_T_device, device_T_cameraA, device_T_cameraB, boundaryPattern, tagSize, dataMatrix, tag_T_cameraA))
							{
								oculusTags.emplace_back(tagID, reflectanceType, intensityThreshold, world_T_device * device_T_cameraA * tag_T_cameraA.inverted(), tagSize);
							}
						}
					}
				}
			}
		}
	}

	return oculusTags;
}

bool OculusTagDetector::confirmDetectionInFrame(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid());
	ocean_assert(world_T_device.isValid() && device_T_camera.isValid());

	if (tag.isValid() == false)
	{
		return false;
	}

	const HomogenousMatrix4 tag_T_camera = tag.world_T_tag().inverted() * world_T_device * device_T_camera;
	ocean_assert(tag_T_camera.isValid());

	if (OculusTagTracker::isTagVisible(anyCamera, tag_T_camera, tag.tagSize(), Scalar(yFrameBorder)) == false)
	{
		return false;
	}

	const uint8_t binaryModuleValueDark = tag.reflectanceType() == OculusTag::RT_REFLECTANCE_NORMAL ? 1u : 0u;
	ocean_assert(binaryModuleValueDark <= 1u);

	// TODO The intensity threshold may change over time - should this value be updated regularly?

	OculusTag::DataMatrix dataMatrix;

	if (readDataMatrix(anyCamera, yFrame, tag_T_camera, tag.tagSize(), tag.intensityThreshold(), binaryModuleValueDark, 1u - binaryModuleValueDark, dataMatrix) == false)
	{
		return false;
	}

	uint32_t tagID = uint32_t(-1);
	if (Utilities::decode(dataMatrix, tagID) == false)
	{
		return false;
	}

	ocean_assert(tagID < 1024u);

	return tagID == tag.tagID();
}

bool OculusTagDetector::determineReflectanceTypeAndIntensityThreshold(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, OculusTag::ReflectanceType& reflectanceType, uint8_t& intensityThreshold, uint8_t& moduleValueDark)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(tag_T_camera.isNull() == false);
	ocean_assert(tagSize > 0);

	reflectanceType = OculusTag::RT_REFLECTANCE_UNDEFINED;
	intensityThreshold = 255u;
	moduleValueDark = 255u;

	const Scalar moduleSize = tagSize / Scalar(8);
	ocean_assert(moduleSize > 0);

	constexpr size_t foregroundTagPointsSize = 20u;
	const Vector2 foregroundTagPoints[foregroundTagPointsSize] =
	{
		// Top row
		Vector2(0.5, -0.5),
		Vector2(1.5, -0.5),
		Vector2(2.5, -0.5),
		Vector2(5.5, -0.5),
		Vector2(6.5, -0.5),
		Vector2(7.5, -0.5),

		// Left column
		Vector2(0.5, -1.5),
		Vector2(0.5, -2.5),
		Vector2(0.5, -5.5),
		Vector2(0.5, -6.5),

		// Right column
		Vector2(7.5, -1.5),
		Vector2(7.5, -2.5),
		Vector2(7.5, -5.5),
		Vector2(7.5, -6.5),

		// Bottom row
		Vector2(0.5, -7.5),
		Vector2(1.5, -7.5),
		Vector2(2.5, -7.5),
		Vector2(5.5, -7.5),
		Vector2(6.5, -7.5),
		Vector2(7.5, -7.5),
	};

	const size_t backgroundTagPointsSize = 16u;
	const Vector2 backgroundTagPoints[backgroundTagPointsSize] =
	{
		// Top
		Vector2(2.5, -1.5),
		Vector2(3.5, -1.5),
		Vector2(4.5, -1.5),
		Vector2(5.5, -1.5),

		// Left
		Vector2(1.5, -2.5),
		Vector2(1.5, -3.5),
		Vector2(1.5, -4.5),
		Vector2(1.5, -5.5),

		// Right
		Vector2(6.5, -2.5),
		Vector2(6.5, -3.5),
		Vector2(6.5, -4.5),
		Vector2(6.5, -5.5),

		// Bottom
		Vector2(2.5, -6.5),
		Vector2(3.5, -6.5),
		Vector2(4.5, -6.5),
		Vector2(5.5, -6.5),
	};

	uint32_t foregroundValueSum = 0u;
	uint8_t foregroundValues[foregroundTagPointsSize];

	for (size_t i = 0; i < foregroundTagPointsSize; ++i)
	{
		uint8_t moduleValue = 0u;
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, Vector3(foregroundTagPoints[i] * moduleSize, Scalar(0)));
		ocean_assert(anyCamera.isInside(moduleCenterImage));

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		foregroundValues[i] = moduleValue;
		foregroundValueSum += uint32_t(moduleValue);
	}

	uint32_t backgroundValueSum = 0u;
	uint8_t backgroundValues[backgroundTagPointsSize];

	for (size_t i = 0; i < backgroundTagPointsSize; ++i)
	{
		uint8_t moduleValue = 0u;
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, Vector3(backgroundTagPoints[i] * moduleSize, Scalar(0)));
		ocean_assert(anyCamera.isInside(moduleCenterImage));

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		backgroundValues[i] = moduleValue;
		backgroundValueSum += uint32_t(moduleValue);
	}

	// Determine the reflectance type and the binary values of the dark modules

	const uint32_t avgForegroundValue = uint32_t(Numeric::round32(Scalar(foregroundValueSum) / Scalar(foregroundTagPointsSize)));
	const uint32_t avgBackgroundValue = uint32_t(Numeric::round32(Scalar(backgroundValueSum) / Scalar(backgroundTagPointsSize)));
	const uint32_t avgIntensityDifference = uint32_t(std::abs(int32_t(avgForegroundValue) - int32_t(avgBackgroundValue)));

	if (avgIntensityDifference >= minIntensityThreshold)
	{
		ocean_assert((avgForegroundValue + avgBackgroundValue + 1u) / 2u <= 255u);
		intensityThreshold = uint8_t((avgForegroundValue + avgBackgroundValue + 1u) / 2u);

		uint32_t foregroundValuesCorrect = 0u;
		uint32_t backgroundValuesCorrect = 0u;

		if (avgForegroundValue < avgBackgroundValue)
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_NORMAL;
			moduleValueDark = 1u;

			for (const uint8_t value : foregroundValues)
			{
				if (value < intensityThreshold)
				{
					++foregroundValuesCorrect;
				}
			}

			for (const uint8_t value : backgroundValues)
			{
				if (value >= intensityThreshold)
				{
					++backgroundValuesCorrect;
				}
			}
		}
		else
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_INVERTED;
			moduleValueDark = 0u;

			for (const uint8_t value : foregroundValues)
			{
				if (value >= intensityThreshold)
				{
					++foregroundValuesCorrect;
				}
			}

			for (const uint8_t value : backgroundValues)
			{
				if (value < intensityThreshold)
				{
					++backgroundValuesCorrect;
				}
			}
		}

		const Scalar foregroundCorrectRatio = Scalar(foregroundValuesCorrect) / Scalar(foregroundTagPointsSize);
		const Scalar backgroundCorrectRatio = Scalar(backgroundValuesCorrect) / Scalar(backgroundTagPointsSize);
		ocean_assert(foregroundCorrectRatio >= 0 && foregroundCorrectRatio <= Scalar(1));
		ocean_assert(backgroundCorrectRatio >= 0 && backgroundCorrectRatio <= Scalar(1));

		if (foregroundCorrectRatio >= 0.9 && backgroundCorrectRatio >= 0.9)
		{
			return true;
		}
	}

	return false;
}

bool OculusTagDetector::determineOrientation(const AnyCamera& anyCamera, const Frame& yFrame, const BoundaryPattern& unorientedBoundaryPattern, const HomogenousMatrix4& unorientedTag_T_camera, const Scalar tagSize, BoundaryPattern& orientedBoundaryPattern, HomogenousMatrix4& orientedTag_T_camera, const uint8_t& intensityThreshold, const uint8_t& moduleValueDark)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(unorientedTag_T_camera.isValid());
	ocean_assert(tagSize > 0);
	ocean_assert(moduleValueDark <= 1u);

	static_assert(OculusTag::numberOfModules == 8u, "This function is hard-coded to 8 modules in horizontal and vertical dimension");

	const Vector2 tagPoints[4] =
	{
		Vector2(1.5, -1.5), // TL
		Vector2(1.5, -6.5), // BL
		Vector2(6.5, -6.5), // BR
		Vector2(6.5, -1.5), // TR
	};

	const Scalar moduleSize = tagSize / Scalar(8);
	ocean_assert(moduleSize > 0);

	uint8_t moduleValueSum = 0u;
	uint8_t topLeftIndex = uint8_t(-1);

	for (uint32_t i = 0u; i < 4u; ++i)
	{
		const Vector2 framePoint = anyCamera.projectToImage(unorientedTag_T_camera, Vector3(tagPoints[i] * moduleSize, Scalar(0)));

		uint8_t pixelValue;
		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), framePoint, &pixelValue);

		const uint8_t moduleValue = pixelValue < intensityThreshold ? moduleValueDark : (1u - moduleValueDark);

		if (moduleValue == 0u)
		{
			topLeftIndex = uint8_t(i);
		}

		moduleValueSum += moduleValue;
	}

	ocean_assert(moduleValueSum <= 4u);
	ocean_assert(topLeftIndex < 4u || moduleValueSum == 4u);

	if (moduleValueSum == 3u && topLeftIndex < 4u)
	{
		if (topLeftIndex == 0u)
		{
			orientedBoundaryPattern = unorientedBoundaryPattern;
			orientedTag_T_camera = unorientedTag_T_camera;

			return true;
		}
		else
		{
			// Shift the corner points so that they start with the corner that has been identified as the top-left corner

			for (size_t i = 0; i < 4; ++i)
			{
				orientedBoundaryPattern[i] = unorientedBoundaryPattern[(topLeftIndex + i) & 0b0011]; // (i + 1) & 0b0111 ~ (i + 1) % 4
			}

			// Rotate the transformation so that it maps the top-left corner of the tag to the corresponding location in the image

			const Scalar angles[4] =
			{
				Numeric::deg2rad(Scalar(0)),
				Numeric::deg2rad(Scalar(270)),
				Numeric::deg2rad(Scalar(180)),
				Numeric::deg2rad(Scalar(90))
			};

			const Scalar center = Scalar(4) * moduleSize;

			const HomogenousMatrix4 translation = HomogenousMatrix4(Vector3(-center, center, 0));
			const HomogenousMatrix4 translationInv = HomogenousMatrix4(Vector3(center, -center, 0));
			const HomogenousMatrix4 rotation = HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), angles[topLeftIndex]));

			orientedTag_T_camera = translationInv * rotation * translation * unorientedTag_T_camera;

			return orientedTag_T_camera.isValid();
		}
	}

	return false;
}

bool OculusTagDetector::readDataMatrix(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark, const uint8_t& binaryModuleValueLight, OculusTag::DataMatrix& dataMatrix)
{
	static_assert(OculusTag::numberOfModules == 8u, "This function is hard-coded to 8 modules in horizontal and vertical dimension");
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(tag_T_camera.isNull() == false);
	ocean_assert(tagSize > 0);
	ocean_assert(binaryModuleValueDark != binaryModuleValueLight && binaryModuleValueDark <= 1u && binaryModuleValueLight <= 1u);

	dataMatrix = 0u;

	const Vector2 moduleCenters[16] =
	{
		// clang-format off
		Vector2(2.5, -2.5), Vector2(3.5, -2.5), Vector2(4.5, -2.5), Vector2(5.5, -2.5),
		Vector2(2.5, -3.5), Vector2(3.5, -3.5), Vector2(4.5, -3.5), Vector2(5.5, -3.5),
		Vector2(2.5, -4.5), Vector2(3.5, -4.5), Vector2(4.5, -4.5), Vector2(5.5, -4.5),
		Vector2(2.5, -5.5), Vector2(3.5, -5.5), Vector2(4.5, -5.5), Vector2(5.5, -5.5),
		// clang-format on
	};

	const Scalar moduleSize = tagSize / Scalar(OculusTag::numberOfModules);
	ocean_assert(moduleSize > 0);

	for (size_t i = 0; i < 16; ++i)
	{
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, Vector3(moduleCenters[i] * moduleSize, 0));
		uint8_t moduleValue;

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		dataMatrix |= (moduleValue >= intensityThreshold ? binaryModuleValueLight : binaryModuleValueDark) << (15 - i);
	}

	return true;
}

bool OculusTagDetector::computePose(const AnyCamera& anyCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& object_T_camera, const uint32_t minPoints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(imagePoints.size() >= minPoints && imagePoints.size() == objectPoints.size());
	ocean_assert(minPoints >= 3u);

	constexpr Scalar maximalProjectionError = Scalar(2.5);
	RandomGenerator randomGenerator;
	Indices32 usedIndices;

	return Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, object_T_camera, minPoints, /* refine */ true, /* iterations */ 10u, Numeric::sqr(maximalProjectionError), &usedIndices);
}

bool OculusTagDetector::optimizePose(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, HomogenousMatrix4& tag_T_camera, const BoundaryPattern& boundaryPattern, const Scalar tagSize, const OculusTag::DataMatrix& dataMatrix)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(yFrame.width() > 2u * yFrameBorder && yFrame.height() > 2u * yFrameBorder);
	ocean_assert(tag_T_camera.isNull() == false);
	ocean_assert(tagSize > 0);

	// First, add those corners that will always be available (on the outside and the inside of the outline)

	Vectors3 objectPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_0_TO_3, tagSize);
	ocean_assert(objectPoints.size() == 4);
	Vectors2 imagePoints;
	imagePoints.reserve(objectPoints.size());

	const HomogenousMatrix4 flippedCamera_T_tag = PinholeCamera::standard2InvertedFlipped(tag_T_camera);
	ocean_assert(flippedCamera_T_tag.isValid());

	for (size_t i = 0; i < 4; ++i)
	{
		imagePoints.emplace_back(boundaryPattern[i]);
	}

	// Use additional points if the image of the current tag is large enough.

	Scalar edgeLengthSum = Scalar(0);
	for (size_t i = 0; i < 4; ++i)
	{
		edgeLengthSum += imagePoints[i].distance(imagePoints[(i + 1) & 0b0011]); // (i + 1) & 0b0111 ~ (i + 1) % 4
	}
	const Scalar averageEdgeLength = Scalar(0.25) * edgeLengthSum;

	if (averageEdgeLength > Scalar(30))
	{
		const Vectors3 tagAllCornerObjectPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_ALL_AVAILABLE, tagSize, dataMatrix);

		for (size_t i = 4; i < objectPoints.size(); ++i)
		{
			Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_tag * tagAllCornerObjectPoints[i]);
			ocean_assert(anyCamera.isInside(imagePoint));

			Utilities::refineCorner(yFrame, imagePoint, /* search window */ 2u);
			imagePoints.emplace_back(imagePoint);
			objectPoints.emplace_back(tagAllCornerObjectPoints[i]);
		}
	}

	ocean_assert(objectPoints.size() >= 4 && objectPoints.size() == imagePoints.size());

	const HomogenousMatrix4 world_T_camera = world_T_device * device_T_camera;
	HomogenousMatrices4 world_T_cameras = { world_T_camera };
	Geometry::ObjectPointGroups objectPointGroups = { std::move(objectPoints) };
	Geometry::ImagePointGroups imagePointGroups = { std::move(imagePoints) };

	const HomogenousMatrix4 world_T_tag = world_T_camera * tag_T_camera.inverted();
	HomogenousMatrix4 optimized_world_T_tag;

	constexpr uint32_t iterations = 200u;
	constexpr Geometry::Estimator::EstimatorType estimatorType = Geometry::Estimator::ET_SQUARE;
	const Scalar lambda = Scalar(0.0001);
	const Scalar lambdaFactor = Scalar(2.5);

	if (Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformation(anyCamera, world_T_cameras, world_T_tag, objectPointGroups, imagePointGroups, optimized_world_T_tag, iterations, estimatorType, lambda, lambdaFactor))
	{
		ocean_assert(optimized_world_T_tag.isValid());
		tag_T_camera = optimized_world_T_tag.inverted() * world_T_camera;
	}

	return tag_T_camera.isValid();
}

bool OculusTagDetector::optimizePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const BoundaryPattern& boundaryPattern, const Scalar tagSize, const OculusTag::DataMatrix& dataMatrix, HomogenousMatrix4& tag_T_cameraA)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());

	ocean_assert(yFrameA.isValid() && FrameType::arePixelFormatsCompatible(yFrameA.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrameA.width() == anyCameraA.width() && yFrameA.height() == yFrameA.height());
	ocean_assert(yFrameA.width() > 2u * yFrameBorder && yFrameA.height() > 2u * yFrameBorder);

	ocean_assert(yFrameB.isValid() && FrameType::arePixelFormatsCompatible(yFrameB.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrameB.width() == anyCameraB.width() && yFrameB.height() == yFrameB.height());
	ocean_assert(yFrameB.width() > 2u * yFrameBorder && yFrameB.height() > 2u * yFrameBorder);

	ocean_assert(yFrameA.frameType() == yFrameB.frameType());

	ocean_assert(world_T_device.isNull() == false);
	ocean_assert(device_T_cameraA.isNull() == false);
	ocean_assert(device_T_cameraB.isNull() == false);

	ocean_assert(tagSize > 0);

	// First, add those corners that will always be available (on the outside and the inside of the outline)

	const Vectors3 objectPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_ALL_AVAILABLE, tagSize, dataMatrix);
	ocean_assert(objectPoints.size() >= 4);

	Vectors2 imagePoints;
	imagePoints.reserve(objectPoints.size());

	for (size_t i = 0; i < 4; ++i)
	{
		ocean_assert(anyCameraA.isInside(boundaryPattern[i]));
		imagePoints.emplace_back(boundaryPattern[i]);
	}

	for (size_t i = 4; i < objectPoints.size(); ++i)
	{
		Vector2 imagePoint = anyCameraA.projectToImage(tag_T_cameraA, objectPoints[i]);
		ocean_assert(anyCameraA.isInside(imagePoint));

		Utilities::refineCorner(yFrameA, imagePoint, /* search window */ 2u);
		imagePoints.emplace_back(imagePoint);
	}

	ocean_assert(objectPoints.size() >= 4u && objectPoints.size() == imagePoints.size());

	if (objectPoints.size() < 4u)
	{
		return false;
	}

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;
	const HomogenousMatrix4 cameraA_T_tag = tag_T_cameraA.inverted();
	const HomogenousMatrix4 world_T_tag = world_T_cameraA * cameraA_T_tag;

	const HomogenousMatrix4 cameraB_T_tag = device_T_cameraB.inverted() * device_T_cameraA * cameraA_T_tag;

	const Vectors2& imagePointsA = imagePoints;
	const Vectors3& objectPointsA = objectPoints;

	ocean_assert(imagePointsA.size() > 4 && imagePointsA.size() == objectPointsA.size());

	Vectors2 imagePointsB;
	Vectors3 objectPointsB;

	for (size_t i = 0; i < objectPoints.size(); ++i)
	{
		const Vector3 cameraBObjectPoint = cameraB_T_tag * objectPoints[i];
		const Vector3 cameraBObjectPointIF(cameraBObjectPoint.x(), -cameraBObjectPoint.y(), -cameraBObjectPoint.z());

		const Vector2 approximageImagePointB = anyCameraB.projectToImageIF(cameraBObjectPointIF);

		if (approximageImagePointB.x() >= Scalar(yFrameBorder) && approximageImagePointB.x() < Scalar(yFrameB.width()) - Scalar(yFrameBorder)
			&& approximageImagePointB.y() >= Scalar(yFrameBorder) && approximageImagePointB.y() < Scalar(yFrameB.height()) - Scalar(yFrameBorder))
		{
			const unsigned int searchRadius = 5u;
			const Scalar maxError = Scalar(5);
			Vector2 imagePointB;

			const Scalar error = Utilities::verifyStereoLocations(yFrameA, yFrameB, imagePoints[i], approximageImagePointB, searchRadius, imagePointB);

			if (error < maxError)
			{
				imagePointsB.emplace_back(imagePointB);
				objectPointsB.emplace_back(objectPoints[i]);
			}
		}
	}

	ocean_assert(imagePointsA.size() == objectPointsA.size());
	ocean_assert(imagePointsB.size() == objectPointsB.size());

	if (objectPointsA.size() < 4u || imagePointsB.empty())
	{
		return true;
	}

	ocean_assert(objectPointsA.size() >= 4 && objectPointsA.size() == imagePointsA.size());

	const HomogenousMatrices4 world_T_camerasA = { world_T_cameraA };
	const HomogenousMatrices4 world_T_camerasB = { world_T_cameraB };

	static_assert(std::is_same<Geometry::ImagePoints, Vectors2>::value, "ImagePoints != Vectors2");
	static_assert(std::is_same<Geometry::ObjectPoints, Vectors3>::value, "ObjectPoints != Vectors3");

	const Geometry::ObjectPointGroups objectPointGroupsA(1, std::move(objectPointsA));
	const Geometry::ObjectPointGroups objectPointGroupsB(1, std::move(objectPointsB));
	const Geometry::ImagePointGroups imagePointGroupsA(1, std::move(imagePointsA));
	const Geometry::ImagePointGroups imagePointGroupsB(1, std::move(imagePointsB));

	HomogenousMatrix4 optimized_world_T_tag;
	if (Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformationStereo(anyCameraA, anyCameraB, world_T_camerasA, world_T_camerasB, world_T_tag, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, optimized_world_T_tag, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
	{
		// TODO Return the pose relative to the world coordinate system
		const HomogenousMatrix4 cameraA_T_world = world_T_cameraA.inverted();
		tag_T_cameraA = (cameraA_T_world * optimized_world_T_tag).inverted();
	}

	return tag_T_cameraA.isValid();
}

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean
