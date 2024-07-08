/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"
#include "ocean/tracking/mapbuilding/PoseEstimation.h"

#include "ocean/cv/Canvas.h"

#include "ocean/cv/detector/FeatureDetector.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

RelocalizerStereo::RelocalizerStereo(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction) :
	Relocalizer(std::move(imageFeaturePointDetectorFunction))
{
	// nothing to do here
}

bool RelocalizerStereo::relocalize(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Frame& yFrameA, const Frame& yFrameB, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const Scalar inlierRate, const HomogenousMatrix4& world_T_roughDevice, Worker* worker, size_t* usedFeatureCorrespondences, Indices32* usedObjectPointIdsA, Indices32* usedObjectPointIdsB, Vectors2* usedImagePointsA, Vectors2* usedImagePointsB)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(device_T_cameraA.isValid() && device_T_cameraB.isValid());
	ocean_assert(yFrameA.isValid() && yFrameB.isValid());
	ocean_assert(cameraA.width() == yFrameA.width() && cameraA.height() == yFrameA.height());
	ocean_assert(cameraB.width() == yFrameB.width() && cameraB.height() == yFrameB.height());
	ocean_assert(yFrameA.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(yFrameB.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(minimalNumberCorrespondences >= 4u);
	ocean_assert(maximalProjectionError >= Scalar(0));
	ocean_assert(inlierRate > Scalar(0) && inlierRate <= Scalar(1));

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid() || !cameraA.isValid() || !cameraB.isValid() || !yFrameA.isValid() || !yFrameB.isValid())
	{
		return false;
	}

	Vectors2 imagePointsA;
	SharedUnifiedDescriptors imagePointDescriptorsA;

	ocean_assert(imageFeaturePointDetectorFunction_);
	if (!imageFeaturePointDetectorFunction_(cameraA, yFrameA, imagePointsA, imagePointDescriptorsA))
	{
		return false;
	}

	Vectors2 imagePointsB;
	SharedUnifiedDescriptors imagePointDescriptorsB;

	ocean_assert(imageFeaturePointDetectorFunction_);
	if (!imageFeaturePointDetectorFunction_(cameraB, yFrameB, imagePointsB, imagePointDescriptorsB))
	{
		return false;
	}

	ocean_assert(imagePointDescriptorsA && !imagePointsA.empty() && imagePointsA.size() == imagePointDescriptorsA->numberDescriptors());
	ocean_assert(imagePointDescriptorsB && !imagePointsB.empty() && imagePointsB.size() == imagePointDescriptorsB->numberDescriptors());
	ocean_assert(imagePointDescriptorsA->descriptorType() == imagePointDescriptorsB->descriptorType());

	const SharedUnifiedFeatureMap featureMap(featureMap_);

	if (!featureMap || !featureMap->isValid())
	{
		return false;
	}

	if (imagePointDescriptorsA->descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_SINGLE_VIEW_256)
	{
		ocean_assert(false && "Other descriptors not yet supported!");
		return false;
	}

	const CV::Detector::FREAKDescriptor32* freakImagePointDescriptorsA = dynamic_cast<const UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256*>(imagePointDescriptorsA.get())->descriptors();
	const CV::Detector::FREAKDescriptor32* freakImagePointDescriptorsB = dynamic_cast<const UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256*>(imagePointDescriptorsB.get())->descriptors();

	if (featureMap->descriptorMap().descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		ocean_assert(false && "Other descriptors not yet supported!");
		return false;
	}

	if (featureMap->objectPointVocabularyDescriptors().descriptorType() != UnifiedDescriptor::binaryDescriptorType<false, false, 256u>())
	{
		ocean_assert(false && "Other descriptors not yet supported!");
		return false;
	}

	using Descriptor = UnifiedDescriptor::BinaryDescriptor<256u>;
	using VocabularyForest = Tracking::VocabularyForest<Descriptor, unsigned int, UnifiedDescriptorT<Descriptor>::determineDistance>;

	const UnifiedDescriptorsBinarySingleLevelSingleView<256u>* specializedObjectPointDescriptors = dynamic_cast<const UnifiedDescriptorsBinarySingleLevelSingleView<256u>*>(&featureMap->objectPointVocabularyDescriptors());
	ocean_assert(specializedObjectPointDescriptors != nullptr);

	const VocabularyForest* vocabularyForest = dynamic_cast<const VocabularyForest*>(&featureMap->objectPointDescriptorsForest());
	ocean_assert(vocabularyForest != nullptr);

	const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& specializedDescriptorMap = ((Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256&)(featureMap->descriptorMap())).descriptorMap();

	const Tracking::MapBuilding::UnifiedUnguidedMatchingFreakMultiLevelDescriptor256 unifiedUnguidedMatchingA(imagePointsA.data(), freakImagePointDescriptorsA, imagePointsA.size(), featureMap->objectPoints().data(), specializedObjectPointDescriptors->descriptors(), featureMap->objectPoints().size(), featureMap->objectPointIndices().data(), *vocabularyForest);
	const Tracking::MapBuilding::UnifiedGuidedMatchingFreakMultiLevelDescriptor256 unifiedGuidedMatchingA(imagePointsA.data(), freakImagePointDescriptorsA, imagePointsA.size(), featureMap->objectPoints().data(), featureMap->objectPoints().size(), featureMap->objectPointOctree(), featureMap->objectPointIds().data(), specializedDescriptorMap);

	const Tracking::MapBuilding::UnifiedUnguidedMatchingFreakMultiLevelDescriptor256 unifiedUnguidedMatchingB(imagePointsB.data(), freakImagePointDescriptorsB, imagePointsB.size(), featureMap->objectPoints().data(), specializedObjectPointDescriptors->descriptors(), featureMap->objectPoints().size(), featureMap->objectPointIndices().data(), *vocabularyForest);
	const Tracking::MapBuilding::UnifiedGuidedMatchingFreakMultiLevelDescriptor256 unifiedGuidedMatchingB(imagePointsB.data(), freakImagePointDescriptorsB, imagePointsB.size(), featureMap->objectPoints().data(), featureMap->objectPoints().size(), featureMap->objectPointOctree(), featureMap->objectPointIds().data(), specializedDescriptorMap);

	Indices32 usedImagePointIndicesA;
	Indices32 usedImagePointIndicesB;

	const UnifiedMatching::DistanceValue distanceThreshold(256u * 25u / 100u); // **TODO**

	world_T_device.toNull();
	if (!Tracking::MapBuilding::PoseEstimation::determinePose(cameraA, cameraB, device_T_cameraA, device_T_cameraB, unifiedUnguidedMatchingA, unifiedUnguidedMatchingB, unifiedGuidedMatchingA, unifiedGuidedMatchingB, randomGenerator_, world_T_device, minimalNumberCorrespondences, distanceThreshold, maximalProjectionError, inlierRate, usedObjectPointIdsA, usedObjectPointIdsB, &usedImagePointIndicesA, &usedImagePointIndicesB, world_T_roughDevice, worker))
	{
		return false;
	}

	if (usedFeatureCorrespondences != nullptr)
	{
		*usedFeatureCorrespondences = usedImagePointIndicesA.size() + usedImagePointIndicesB.size();
	}

	if (usedImagePointsA != nullptr)
	{
		ocean_assert(!usedImagePointIndicesA.empty());

		for (const Index32& usedImagePointIndexA : usedImagePointIndicesA)
		{
			usedImagePointsA->emplace_back(imagePointsA[usedImagePointIndexA]);
		}
	}

	if (usedImagePointsB != nullptr)
	{
		ocean_assert(!usedImagePointIndicesB.empty());

		for (const Index32& usedImagePointIndexB : usedImagePointIndicesB)
		{
			usedImagePointsB->emplace_back(imagePointsB[usedImagePointIndexB]);
		}
	}

	if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURES))
	{
		Frame debugFrameA;
		Frame debugFrameB;

		for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
		{
			const Frame& yFrame = nCamera == 0u ? yFrameA : yFrameB;
			Frame& debugFrame = nCamera == 0u ? debugFrameA : debugFrameB;

			CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, CV::FrameConverter::CP_ALWAYS_COPY);

			const Vectors2& imagePoints = nCamera == 0u ? imagePointsA : imagePointsB;
			const Indices32& usedImagePointIndices = nCamera == 0u ? usedImagePointIndicesA : usedImagePointIndicesB;

			Vectors2 features;
			features.reserve(usedImagePointIndices.size());

			for (const Index32& usedImagePointIndex : usedImagePointIndices)
			{
				features.emplace_back(imagePoints[usedImagePointIndex]);
			}

			CV::Canvas::points<5u>(debugFrame, features, CV::Canvas::black());
		}

		Frame debugFrame(FrameType(debugFrameA, debugFrameA.width() + debugFrameB.width(), std::max(debugFrameA.height(), debugFrameB.height())));
		debugFrame.setValue(0x00);

		debugFrame.copy(0, 0, debugFrameA);
		debugFrame.copy(int(debugFrameA.width()), 0, debugFrameB);

		RelocalizerDebugElements::get().updateElement(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURES, std::move(debugFrame));
	}

	return true;
}

}

}

}
