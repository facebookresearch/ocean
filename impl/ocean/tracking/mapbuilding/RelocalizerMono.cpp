/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/RelocalizerMono.h"
#include "ocean/tracking/mapbuilding/PoseEstimation.h"

#include "ocean/cv/Canvas.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

RelocalizerMono::RelocalizerMono(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction) :
	Relocalizer(std::move(imageFeaturePointDetectorFunction))
{
	// nothing to do here
}

bool RelocalizerMono::relocalize(const AnyCamera& camera, const Frame& yFrame, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondence, const Scalar maximalProjectionError, const Scalar inlierRate, const HomogenousMatrix4& world_T_roughCamera, Worker* worker, Indices32* usedObjectPointIds, Vectors2* usedImagePoints)
{
	ocean_assert(camera.isValid() && yFrame.isValid());
	ocean_assert(camera.width() == yFrame.width() && camera.height() == yFrame.height());
	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(minimalNumberCorrespondence >= 4u);
	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid() || !camera.isValid() || !yFrame.isValid())
	{
		return false;
	}

	Vectors2 imagePoints;
	SharedUnifiedDescriptors imagePointDescriptors;

	ocean_assert(imageFeaturePointDetectorFunction_);
	if (!imageFeaturePointDetectorFunction_(camera, yFrame, imagePoints, imagePointDescriptors))
	{
		return false;
	}

	ocean_assert(imagePointDescriptors && !imagePoints.empty() && imagePoints.size() == imagePointDescriptors->numberDescriptors());

	const SharedUnifiedFeatureMap featureMap(featureMap_);

	if (!featureMap || !featureMap->isValid())
	{
		return false;
	}

	SharedUnifiedUnguidedMatching unifiedUnguidedMatching;
	SharedUnifiedGuidedMatching unifiedGuidedMatching;

	if (!featureMap->createMatchingObjects(imagePoints.data(), imagePointDescriptors.get(), unifiedUnguidedMatching, unifiedGuidedMatching))
	{
		return false;
	}

	constexpr unsigned int binaryDistanceThreshold = 256u * 20u / 100u; // **TODO**
	constexpr float floatDistanceThreshold = 0.5f; // **TODO**

	const UnifiedMatching::DistanceValue maximalDescriptorDistance(binaryDistanceThreshold, floatDistanceThreshold);

	ocean_assert(unifiedUnguidedMatching && unifiedGuidedMatching);

	Indices32 imagePointIndices;

	world_T_camera.toNull();
	if (!Tracking::MapBuilding::PoseEstimation::determinePose(camera, *unifiedUnguidedMatching, *unifiedGuidedMatching, randomGenerator_, world_T_camera, minimalNumberCorrespondence, maximalDescriptorDistance, maximalProjectionError, inlierRate, usedObjectPointIds, &imagePointIndices, world_T_roughCamera, worker))
	{
		return false;
	}

	if (usedImagePoints != nullptr)
	{
		for (const Index32& imagePointIndex : imagePointIndices)
		{
			usedImagePoints->emplace_back(imagePoints[imagePointIndex]);
		}
	}

	if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURES))
	{
		Frame debugFrame;
		CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, CV::FrameConverter::CP_ALWAYS_COPY);

		Vectors2 features;
		features.reserve(imagePointIndices.size());

		for (const Index32& imagePointIndex : imagePointIndices)
		{
			features.emplace_back(imagePoints[imagePointIndex]);
		}

		CV::Canvas::points<5u>(debugFrame, features, CV::Canvas::black());

		RelocalizerDebugElements::get().updateElement(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURES, std::move(debugFrame));
	}

	return true;
}

}

}

}
