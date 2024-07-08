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
	mapDetectorType(RMVFeatureDetector::DT_INVALID),
	mapInitializationDetectorType(RMVFeatureDetector::DT_INVALID)
{
	// nothing to do here
}

void RMVFeatureMap::setFeatures(const Vector3* points, const size_t number, const PinholeCamera& pinholeCamera, const RMVFeatureDetector::DetectorType detectorType)
{
	mapCamera = pinholeCamera;
	mapDetectorType = detectorType;

	mapObjectPoints.resize(number);

	if (number != 0)
		memcpy(mapObjectPoints.data(), points, sizeof(Vector3) * number);

	mapRecentStrongObjectPointIndices.clear();
	mapRecentSemiStrongObjectPointIndices.clear();
	mapRecentUsedObjectPointIndices.clear();

	mapBoundingBox = Box3(mapObjectPoints);
}

bool RMVFeatureMap::setFeatures(const Frame& pattern, const Vector3& dimension, const PinholeCamera& pinholeCamera, const size_t numberFeatures, const RMVFeatureDetector::DetectorType detectorType, Worker* worker)
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

	const Geometry::ImagePoints points(RMVFeatureDetector::detectFeatures(yAdjustedPattern, detectorType, Scalar(15), numberFeatures, true, worker));

	const Scalar patternDimensionX = dimension.x();
	const Scalar patternDimensionY = dimension.y() > 0 ? dimension.y() : dimension.x() * Scalar(yAdjustedPattern.height()) / Scalar(yAdjustedPattern.width());

	const Scalar patternFactorX = patternDimensionX / Scalar(yAdjustedPattern.width());
	const Scalar patternFactorY = patternDimensionY / Scalar(yAdjustedPattern.height());

	Vectors3 objectPoints;
	objectPoints.reserve(points.size());

	for (Geometry::ImagePoints::const_iterator i = points.begin(); i != points.end(); ++i)
	{
		const Vector3 position3(i->x() * patternFactorX - patternDimensionX * Scalar(0.5), 0, i->y() * patternFactorY - patternDimensionY * Scalar(0.5));
		objectPoints.push_back(position3);
	}

	ocean_assert(Box3(objectPoints).xDimension() <= patternDimensionX);
	ocean_assert(Box3(objectPoints).zDimension() <= patternDimensionY);

	setFeatures(objectPoints.data(), objectPoints.size(), pinholeCamera, detectorType);

	return true;
}

void RMVFeatureMap::setInitializationFeatures(const Vector3* objectPoints, const size_t number, const PinholeCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType)
{
	mapInitializationCamera = initializationCamera;
	mapInitializationDetectorType = initializationDetectorType;

	mapInitializationObjectPoints.resize(number);

	if (number != 0)
	{
		memcpy(mapInitializationObjectPoints.data(), objectPoints, sizeof(Vector3) * number);
	}

	mapInitializationBoundingBox = Box3(mapInitializationObjectPoints);
	ocean_assert(mapInitializationBoundingBox);
}

void RMVFeatureMap::setInitializationFeatures(Vectors3&& objectPoints, const PinholeCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType)
{
	mapInitializationCamera = initializationCamera;
	mapInitializationDetectorType = initializationDetectorType;

	mapInitializationObjectPoints = std::move(objectPoints);

	mapInitializationBoundingBox = Box3(mapInitializationObjectPoints);
	ocean_assert(mapInitializationBoundingBox);
}

bool RMVFeatureMap::setInitializationFeatures(const Frame& pattern, const Vector3& dimension, const PinholeCamera& pinholeCamera, const size_t numberInitializationObjectPoints, const RMVFeatureDetector::DetectorType& initializationDetectorType, Worker* worker)
{
	ocean_assert(pattern && dimension.x() > Numeric::eps() && pinholeCamera && numberInitializationObjectPoints >= 10);

	Frame yPattern;
	if (!CV::FrameConverter::Comfort::convert(pattern, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yPattern, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		return false;

	PinholeCamera adjustedCamera(pinholeCamera);

	Frame yAdjustedPattern(yPattern, Frame::ACM_USE_KEEP_LAYOUT);
	if (RMVFeatureDetector::needPyramidInitialization(initializationDetectorType) && pinholeCamera.width() >= 640u)
	{
		if (!CV::FrameShrinker::downsampleByTwo11(yPattern, yAdjustedPattern, worker))
		{
			return false;
		}

		// we will use a smaller camera frame during the initialization
		adjustedCamera = PinholeCamera(pinholeCamera.width() / 2u, pinholeCamera.height() / 2u, pinholeCamera);
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

	for (Geometry::ImagePoints::const_iterator i = initializationPoints.begin(); i != initializationPoints.end(); ++i)
	{
		const Vector3 position3(i->x() * patternFactorX - patternDimensionX * Scalar(0.5), 0, i->y() * patternFactorY - patternDimensionY * Scalar(0.5));
		objectPoints.push_back(position3);
	}

	ocean_assert(Box3(objectPoints).xDimension() <= patternDimensionX);
	ocean_assert(Box3(objectPoints).zDimension() <= patternDimensionY);

	setInitializationFeatures(std::move(objectPoints), adjustedCamera, initializationDetectorType);

	Log::info() << "Initialization with camera dimension: " << adjustedCamera.width() << "x" << adjustedCamera.height();
	Log::info() << "Initialization with pattern size: " << yAdjustedPattern.width() << "x" << yAdjustedPattern.height();

	return true;
}

void RMVFeatureMap::clear()
{
	mapObjectPoints.clear();

	mapRecentStrongObjectPointIndices.clear();
	mapRecentSemiStrongObjectPointIndices.clear();
	mapRecentUsedObjectPointIndices.clear();
}

}

}

}
