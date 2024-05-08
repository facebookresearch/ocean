/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/Relocalizer.h"

#include "ocean/cv/detector/FeatureDetector.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

Relocalizer::Relocalizer(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction) :
	imageFeaturePointDetectorFunction_(std::move(imageFeaturePointDetectorFunction))
{
	// nothing to do here
}

bool Relocalizer::setImageFeaturePointDetectorFunction(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction)
{
	const ScopedLock scopedLock(lock_);

	imageFeaturePointDetectorFunction_ = std::move(imageFeaturePointDetectorFunction);

	return true;
}

bool Relocalizer::setFeatureMap(SharedUnifiedFeatureMap featureMap)
{
	const ScopedLock scopedLock(lock_);

	featureMap_ = std::move(featureMap);

	return true;
}

bool Relocalizer::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return imageFeaturePointDetectorFunction_ && featureMap_ && featureMap_->isValid();
}

bool Relocalizer::detectFreakFeatures(const AnyCamera& camera, const Frame& yFrame, Vectors2& imagePoints, SharedUnifiedDescriptors& imagePointDescriptors)
{
	ocean_assert(camera.isValid() && yFrame.isValid());
	ocean_assert(yFrame.isFrameTypeCompatible(FrameType(camera.width(), camera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), false));

	const unsigned int maxFrameArea = yFrame.width() * yFrame.height();
	const unsigned int minFrameArea = std::max(40u * 40u, maxFrameArea / 64u);

	constexpr unsigned int expectedHarrisCorners640x480 = 1600u;
	constexpr Scalar harrisCornersReductionScale = Scalar(0.4);
	constexpr unsigned int harrisCornerThreshold = 10u;

	const float inverseFocalLength = float(camera.inverseFocalLengthX());

	const CV::Detector::FREAKDescriptor32::AnyCameraDerivativeFunctor cameraFunctor(camera.clone(), 8u); // **TODO** avoid fixed layer number

	CV::Detector::HarrisCorners harrisCorners;
	Indices32 cornerPyramidLevels;

	CV::Detector::FREAKDescriptors32 freakImagePointDescriptors;

	constexpr bool removeInvalid = true;
	constexpr Scalar border = Scalar(20);
	constexpr bool determineExactHarrisCornerPositions = true;
	const bool yFrameIsUndistorted = false;

	if (!CV::Detector::FREAKDescriptor32::extractHarrisCornersAndComputeDescriptors(yFrame, maxFrameArea, minFrameArea, expectedHarrisCorners640x480, harrisCornersReductionScale, harrisCornerThreshold, inverseFocalLength, cameraFunctor, harrisCorners, cornerPyramidLevels, freakImagePointDescriptors, removeInvalid, border, determineExactHarrisCornerPositions, yFrameIsUndistorted))
	{
		return false;
	}

	ocean_assert(harrisCorners.size() == cornerPyramidLevels.size());

	imagePoints.clear();
	imagePoints.reserve(harrisCorners.size());

	for (size_t nFeature = 0; nFeature < harrisCorners.size(); ++nFeature)
	{
		const Scalar levelFactor = Scalar(1u << cornerPyramidLevels[nFeature]);
		imagePoints.emplace_back(harrisCorners[nFeature].observation() * levelFactor);
	}

	imagePointDescriptors = std::make_shared<UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256>(std::move(freakImagePointDescriptors));

	return true;
}

Relocalizer& Relocalizer::operator=(Relocalizer&& relocalizer)
{
	if (this != &relocalizer)
	{
		imageFeaturePointDetectorFunction_ = std::move(relocalizer.imageFeaturePointDetectorFunction_);

		featureMap_ = std::move(relocalizer.featureMap_);

		randomGenerator_ = std::move(relocalizer.randomGenerator_);
	}

	return *this;
}

}

}

}
