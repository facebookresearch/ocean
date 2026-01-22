/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/rmv/RMVFeatureMap.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameShrinker.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

RMVFeatureMap::RMVFeatureMap() :
	mapDetectorType_(RMVFeatureDetector::DT_INVALID),
	mapInitializationDetectorType_(RMVFeatureDetector::DT_INVALID)
{
	// nothing to do here
}

void RMVFeatureMap::setFeatures(const Vector3* points, const size_t number, const SharedAnyCamera& camera, const RMVFeatureDetector::DetectorType detectorType)
{
	mapCamera_ = camera;
	mapDetectorType_ = detectorType;

	mapObjectPoints_.resize(number);

	if (number != 0)
	{
		memcpy(mapObjectPoints_.data(), points, sizeof(Vector3) * number);
	}

	mapRecentStrongObjectPointIndices_.clear();
	mapRecentSemiStrongObjectPointIndices_.clear();
	mapRecentUsedObjectPointIndices_.clear();

	mapBoundingBox_ = Box3(mapObjectPoints_);
}

bool RMVFeatureMap::setFeatures(const Frame& pattern, const Vector3& dimension, const SharedAnyCamera& camera, const size_t numberFeatures, const RMVFeatureDetector::DetectorType detectorType, Worker* worker)
{
	ocean_assert(pattern);

	Frame yPattern;
	if (!CV::FrameConverter::Comfort::convert(pattern, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yPattern, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	ocean_assert(yPattern);

	Frame yAdjustedPattern(yPattern, Frame::ACM_USE_KEEP_LAYOUT);
	if (RMVFeatureDetector::needSmoothedFrame(detectorType))
	{
		if (!CV::FrameFilterGaussian::filter(yPattern, yAdjustedPattern, 3u, worker))
		{
			return false;
		}
	}

	ocean_assert(yAdjustedPattern);

	const Vectors2 points(RMVFeatureDetector::detectFeatures(yAdjustedPattern, detectorType, Scalar(15), numberFeatures, true, worker));

	const Scalar patternDimensionX = dimension.x();
	const Scalar patternDimensionY = dimension.y() > 0 ? dimension.y() : dimension.x() * Scalar(yAdjustedPattern.height()) / Scalar(yAdjustedPattern.width());

	const Scalar patternFactorX = patternDimensionX / Scalar(yAdjustedPattern.width());
	const Scalar patternFactorY = patternDimensionY / Scalar(yAdjustedPattern.height());

	Vectors3 objectPoints;
	objectPoints.reserve(points.size());

	for (const Vector2& point : points)
	{
		const Vector3 position3(point.x() * patternFactorX - patternDimensionX * Scalar(0.5), 0, point.y() * patternFactorY - patternDimensionY * Scalar(0.5));
		objectPoints.push_back(position3);
	}

	ocean_assert(Box3(objectPoints).xDimension() <= patternDimensionX);
	ocean_assert(Box3(objectPoints).zDimension() <= patternDimensionY);

	setFeatures(objectPoints.data(), objectPoints.size(), camera, detectorType);

	return true;
}

void RMVFeatureMap::setInitializationFeatures(const Vector3* objectPoints, const size_t number, const SharedAnyCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType)
{
	mapInitializationCamera_ = initializationCamera;
	mapInitializationDetectorType_ = initializationDetectorType;

	mapInitializationObjectPoints_.resize(number);

	if (number != 0)
	{
		memcpy(mapInitializationObjectPoints_.data(), objectPoints, sizeof(Vector3) * number);
	}

	mapInitializationBoundingBox_ = Box3(mapInitializationObjectPoints_);
	ocean_assert(mapInitializationBoundingBox_);
}

void RMVFeatureMap::setInitializationFeatures(Vectors3&& objectPoints, const SharedAnyCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType)
{
	mapInitializationCamera_ = initializationCamera;
	mapInitializationDetectorType_ = initializationDetectorType;

	mapInitializationObjectPoints_ = std::move(objectPoints);

	mapInitializationBoundingBox_ = Box3(mapInitializationObjectPoints_);
	ocean_assert(mapInitializationBoundingBox_);
}

bool RMVFeatureMap::setInitializationFeatures(const Frame& pattern, const Vector3& dimension, const SharedAnyCamera& camera, const size_t numberInitializationObjectPoints, const RMVFeatureDetector::DetectorType& initializationDetectorType, Worker* worker)
{
	ocean_assert(pattern && dimension.x() > Numeric::eps() && camera && numberInitializationObjectPoints >= 10);

	Frame yPattern;
	if (!CV::FrameConverter::Comfort::convert(pattern, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yPattern, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	SharedAnyCamera adjustedCamera = camera;

	Frame yAdjustedPattern(yPattern, Frame::ACM_USE_KEEP_LAYOUT);
	if (RMVFeatureDetector::needPyramidInitialization(initializationDetectorType) && camera->width() >= 640u)
	{
		if (!CV::FrameShrinker::downsampleByTwo11(yPattern, yAdjustedPattern, worker))
		{
			return false;
		}

		// we will use a smaller camera frame during the initialization
		adjustedCamera = camera->clone(yAdjustedPattern.width(), yAdjustedPattern.height());
	}

	ocean_assert(yAdjustedPattern);
	ocean_assert(adjustedCamera);

	if (RMVFeatureDetector::needSmoothedFrame(initializationDetectorType))
	{
		if (!CV::FrameFilterGaussian::filter(yAdjustedPattern, 3u, worker))
		{
			return false;
		}
	}

	ocean_assert(yAdjustedPattern);

	const Vectors2 initializationPoints(RMVFeatureDetector::detectFeatures(yAdjustedPattern, initializationDetectorType, Scalar(35), numberInitializationObjectPoints, true, worker));

	if (initializationPoints.size() < 10)
	{
		ocean_assert(false && "The pattern does not have enough feature points!");
		return false;
	}

	const Scalar patternDimensionX = dimension.x();
	const Scalar patternDimensionY = dimension.y() > Numeric::eps() ? dimension.y() : dimension.x() * Scalar(yAdjustedPattern.height()) / Scalar(yAdjustedPattern.width());

	const Scalar patternFactorX = patternDimensionX / Scalar(yAdjustedPattern.width());
	const Scalar patternFactorY = patternDimensionY / Scalar(yAdjustedPattern.height());

	Vectors3 objectPoints;
	objectPoints.reserve(initializationPoints.size());

	for (const Vector2& point : initializationPoints)
	{
		const Vector3 position3(point.x() * patternFactorX - patternDimensionX * Scalar(0.5), 0, point.y() * patternFactorY - patternDimensionY * Scalar(0.5));
		objectPoints.push_back(position3);
	}

	ocean_assert(Box3(objectPoints).xDimension() <= patternDimensionX);
	ocean_assert(Box3(objectPoints).zDimension() <= patternDimensionY);

	setInitializationFeatures(std::move(objectPoints), adjustedCamera, initializationDetectorType);

	Log::info() << "Initialization with camera dimension: " << adjustedCamera->width() << "x" << adjustedCamera->height();
	Log::info() << "Initialization with pattern size: " << yAdjustedPattern.width() << "x" << yAdjustedPattern.height();

	return true;
}

void RMVFeatureMap::clear()
{
	mapObjectPoints_.clear();

	mapRecentStrongObjectPointIndices_.clear();
	mapRecentSemiStrongObjectPointIndices_.clear();
	mapRecentUsedObjectPointIndices_.clear();
}

Box2 RMVFeatureMap::projectToImage(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Box3& objectBoundingBox)
{
	return projectToImageIF(camera, Camera::standard2InvertedFlipped(world_T_camera), objectBoundingBox);
}

Box2 RMVFeatureMap::projectToImageIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Box3& objectBoundingBox)
{
	Vector3 boxObjectCorners[8];
	const unsigned int numberBoxImagePoints = objectBoundingBox.corners(boxObjectCorners);

	Box2 projectedBox;
	for (unsigned int n = 0; n < numberBoxImagePoints; ++n)
	{
		projectedBox += camera.projectToImageIF(flippedCamera_T_world, boxObjectCorners[n]);
	}

	return projectedBox;
}

}

}

}
