/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/TrackerStereo.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

TrackerStereo::TrackerStereo(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction) :
	RelocalizerStereo(std::move(imageFeaturePointDetectorFunction))
{
	// nothing to do here
}

bool TrackerStereo::setFeatureMap(SharedUnifiedFeatureMap featureMap)
{
	const ScopedLock scopedLock(lock_);

	if (!RelocalizerStereo::setFeatureMap(std::move(featureMap)))
	{
		return false;
	}

	if (featureMap_)
	{
		objectPointIdMap_.clear();
		objectPointIdMap_.reserve(featureMap_->objectPointIds().size());

		const Indices32& objectPointIds = featureMap_->objectPointIds();

		for (Index32 n = 0u; n < Index32(objectPointIds.size()); ++n)
		{
			ocean_assert(objectPointIdMap_.find(objectPointIds[n]) == objectPointIdMap_.cend());

			objectPointIdMap_.emplace(objectPointIds[n], n);
		}
	}

	return true;
}

bool TrackerStereo::track(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Frame& yFrameA, const Frame& yFrameB, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const HomogenousMatrix4& world_T_roughDevice, Worker* worker)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(device_T_cameraA.isValid() && device_T_cameraB.isValid());
	ocean_assert(yFrameA.isValid() && yFrameB.isValid());
	ocean_assert(anyCameraA.width() == yFrameA.width() && anyCameraA.height() == yFrameA.height());
	ocean_assert(anyCameraB.width() == yFrameB.width() && anyCameraB.height() == yFrameB.height());
	ocean_assert(yFrameA.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(yFrameB.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(minimalNumberCorrespondences >= 4u);
	ocean_assert(maximalProjectionError >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid() || !anyCameraA.isValid() || !anyCameraB.isValid() || !yFrameA.isValid() || !yFrameB.isValid())
	{
		return false;
	}

	constexpr unsigned int pyramidLayers = 1u;
	constexpr unsigned int coarseLayerRadius = 4u;

	if (!previousImagePointsA_.empty())
	{
		ocean_assert(!previousImagePointsB_.empty());
		ocean_assert(yPreviousFramePyramidA_.isValid() && yPreviousFramePyramidB_.isValid());
		ocean_assert(previousImagePointsA_.size() == previousObjectPointsA_.size());
		ocean_assert(previousImagePointsB_.size() == previousObjectPointsB_.size());

		const unsigned int minimalNumberCorrespondencesTracking = std::max(4u, minimalNumberCorrespondences / 2u);

		yCurrentFramePyramidA_.replace8BitPerChannel11(yFrameA.constdata<uint8_t>(), yFrameA.width(), yFrameA.height(), 1u, yFrameA.pixelOrigin(), pyramidLayers, yFrameA.paddingElements(), true /*copyFirstLayer*/, worker);
		yCurrentFramePyramidB_.replace8BitPerChannel11(yFrameB.constdata<uint8_t>(), yFrameB.width(), yFrameB.height(), 1u, yFrameB.pixelOrigin(), pyramidLayers, yFrameB.paddingElements(), true /*copyFirstLayer*/, worker);

		Vectors2 roughCurrentImagePointsA;
		Vectors2 roughCurrentImagePointsB;

		if (world_T_roughDevice.isValid())
		{
			const HomogenousMatrix4 flippedRoughCameraA_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughDevice * device_T_cameraA));
			const HomogenousMatrix4 flippedRoughCameraB_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughDevice * device_T_cameraB));

			roughCurrentImagePointsA.resize(previousObjectPointsA_.size());
			roughCurrentImagePointsB.resize(previousObjectPointsB_.size());

			anyCameraA.projectToImageIF(flippedRoughCameraA_T_world, previousObjectPointsA_.data(), previousObjectPointsA_.size(), roughCurrentImagePointsA.data());
			anyCameraB.projectToImageIF(flippedRoughCameraB_T_world, previousObjectPointsB_.data(), previousObjectPointsB_.size(), roughCurrentImagePointsB.data());

			const Scalar widthA = Scalar(yFrameA.width()) - Scalar(0.1);
			const Scalar heightA = Scalar(yFrameA.height()) - Scalar(0.1);

			for (Vector2& roughCurrentImagePointA : roughCurrentImagePointsA)
			{
				roughCurrentImagePointA.x() = minmax(Scalar(0), roughCurrentImagePointA.x(), widthA);
				roughCurrentImagePointA.y() = minmax(Scalar(0), roughCurrentImagePointA.y(), heightA);
			}

			const Scalar widthB = Scalar(yFrameB.width()) - Scalar(0.1);
			const Scalar heightB = Scalar(yFrameB.height()) - Scalar(0.1);

			for (Vector2& roughCurrentImagePointB : roughCurrentImagePointsB)
			{
				roughCurrentImagePointB.x() = minmax(Scalar(0), roughCurrentImagePointB.x(), widthB);
				roughCurrentImagePointB.y() = minmax(Scalar(0), roughCurrentImagePointB.y(), heightB);
			}
		}
		else
		{
			roughCurrentImagePointsA = previousImagePointsA_;
			roughCurrentImagePointsB = previousImagePointsB_;
		}

		Vectors2 currentImagePointsA;

		if (CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<1u, 15u>(yPreviousFramePyramidA_, yCurrentFramePyramidA_, previousImagePointsA_, roughCurrentImagePointsA, currentImagePointsA, coarseLayerRadius, 4u, worker))
		{
			Vectors2 currentImagePointsB;

			if (CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<1u, 15u>(yPreviousFramePyramidB_, yCurrentFramePyramidB_, previousImagePointsB_, roughCurrentImagePointsB, currentImagePointsB, coarseLayerRadius, 4u, worker))
			{
				std::swap(previousImagePointsA_, currentImagePointsA);
				std::swap(previousImagePointsB_, currentImagePointsB);

				HomogenousMatrix4 device_T_world;

				Indices32 validIndicesA;
				Indices32 validIndicesB;

				validIndicesA.clear();
				validIndicesB.clear();
				if (Geometry::RANSAC::objectTransformationStereo(anyCameraA, anyCameraB, device_T_cameraA, device_T_cameraB, ConstArrayAccessor<Vector3>(previousObjectPointsA_), ConstArrayAccessor<Vector3>(previousObjectPointsB_), ConstArrayAccessor<Vector2>(previousImagePointsA_), ConstArrayAccessor<Vector2>(previousImagePointsB_), randomGenerator_, device_T_world, Index32((previousObjectPointsA_.size() + previousObjectPointsB_.size()) / 2), true, 40u, Numeric::sqr(maximalProjectionError), &validIndicesA, &validIndicesB))
				{
					if (!validIndicesA.empty() && !validIndicesB.empty() && (validIndicesA.size() + validIndicesB.size() >= minimalNumberCorrespondencesTracking))
					{
						world_T_device = device_T_world.inverted();

						if (validIndicesA.size() != previousImagePointsA_.size())
						{
							previousImagePointsA_ = Subset::subset(previousImagePointsA_, validIndicesA);
							previousObjectPointsA_ = Subset::subset(previousObjectPointsA_, validIndicesA);
						}

						if (validIndicesB.size() != previousImagePointsB_.size())
						{
							previousImagePointsB_ = Subset::subset(previousImagePointsB_, validIndicesB);
							previousObjectPointsB_ = Subset::subset(previousObjectPointsB_, validIndicesB);
						}

						std::swap(yPreviousFramePyramidA_, yCurrentFramePyramidA_);
						std::swap(yPreviousFramePyramidB_, yCurrentFramePyramidB_);

						if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES))
						{
							createDebuggingElementFeatureCorrespondences(anyCameraA, anyCameraB, world_T_device, device_T_cameraA, device_T_cameraB);
						}

						return true;
					}
				}
			}
		}

		previousImagePointsA_.clear();
		previousImagePointsB_.clear();
	}

	Indices32 usedObjectPointIdsA;
	Indices32 usedObjectPointIdsB;

	Vectors2 usedImagePointsA;
	Vectors2 usedImagePointsB;

	constexpr Scalar relocalizerInlierRate = Scalar(0.15);

	if (!relocalize(anyCameraA, anyCameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, world_T_device, minimalNumberCorrespondences, maximalProjectionError, relocalizerInlierRate, world_T_roughDevice, worker, nullptr, &usedObjectPointIdsA, &usedObjectPointIdsB, &usedImagePointsA, &usedImagePointsB))
	{
		return false;
	}

	if (usedObjectPointIdsA.empty() || usedObjectPointIdsB.empty())
	{
		return false;
	}

	if (usedObjectPointIdsA.size() + usedObjectPointIdsB.size() < size_t(minimalNumberCorrespondences))
	{
		return false;
	}

	ocean_assert(usedObjectPointIdsA.size() == usedImagePointsA.size());
	ocean_assert(usedObjectPointIdsB.size() == usedImagePointsB.size());

	previousObjectPointsA_.clear();
	previousObjectPointsA_.reserve(usedObjectPointIdsA.size());

	previousObjectPointsB_.clear();
	previousObjectPointsB_.reserve(usedObjectPointIdsB.size());

	const Vectors3& objectPoints = featureMap_->objectPoints();

	for (size_t n = 0; n < usedObjectPointIdsA.size(); ++n)
	{
		const ObjectPointIdMap::const_iterator iObjectPointId = objectPointIdMap_.find(usedObjectPointIdsA[n]);
		ocean_assert(iObjectPointId != objectPointIdMap_.cend());

		const Index32& objectPointIndex = iObjectPointId->second;

		ocean_assert(objectPointIndex < objectPoints.size());
		previousObjectPointsA_.emplace_back(objectPoints[objectPointIndex]);
	}

	for (size_t n = 0; n < usedObjectPointIdsB.size(); ++n)
	{
		const ObjectPointIdMap::const_iterator iObjectPointId = objectPointIdMap_.find(usedObjectPointIdsB[n]);
		ocean_assert(iObjectPointId != objectPointIdMap_.cend());

		const Index32& objectPointIndex = iObjectPointId->second;

		ocean_assert(objectPointIndex < objectPoints.size());
		previousObjectPointsB_.emplace_back(objectPoints[objectPointIndex]);
	}

	previousImagePointsA_ = std::move(usedImagePointsA);
	previousImagePointsB_ = std::move(usedImagePointsB);

	yPreviousFramePyramidA_.replace8BitPerChannel11(yFrameA.constdata<uint8_t>(), yFrameA.width(), yFrameA.height(), 1u, yFrameA.pixelOrigin(), pyramidLayers, yFrameA.paddingElements(), true /*copyFirstLayer*/, worker);
	yPreviousFramePyramidB_.replace8BitPerChannel11(yFrameB.constdata<uint8_t>(), yFrameB.width(), yFrameB.height(), 1u, yFrameB.pixelOrigin(), pyramidLayers, yFrameB.paddingElements(), true /*copyFirstLayer*/, worker);

	if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES))
	{
		createDebuggingElementFeatureCorrespondences(anyCameraA, anyCameraB, world_T_device, device_T_cameraA, device_T_cameraB);
	}

	return true;
}

void TrackerStereo::createDebuggingElementFeatureCorrespondences(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid() && world_T_device.isValid());

	ocean_assert(RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES));

	Frame debugFrameA;
	Frame debugFrameB;

	for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
	{
		Frame yFrame = nCamera == 0u ? Frame(yPreviousFramePyramidA_.finestLayer(), Frame::ACM_USE_KEEP_LAYOUT) : Frame(yPreviousFramePyramidB_.finestLayer(), Frame::ACM_USE_KEEP_LAYOUT);
		Frame& debugFrame = nCamera == 0u ? debugFrameA : debugFrameB;

		yFrame.setPixelFormat(FrameType::FORMAT_Y8);

		if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const Vectors2& imagePoints = nCamera == 0u ? previousImagePointsA_ : previousImagePointsB_;
		const Vectors3& objectPoints = nCamera == 0u ? previousObjectPointsA_ : previousObjectPointsB_;

		const AnyCamera& camera = nCamera == 0u ? anyCameraA : anyCameraB;

		const HomogenousMatrix4& device_T_camera = nCamera == 0u ? device_T_cameraA : device_T_cameraB;

		const HomogenousMatrix4 world_T_camera = world_T_device * device_T_camera;

		Tracking::Utilities::paintCorrespondences<7u, 3u>(debugFrame, camera, world_T_camera, objectPoints.data(), imagePoints.data(), objectPoints.size(), Scalar(3), CV::Canvas::blue(), CV::Canvas::green(), CV::Canvas::red(), CV::Canvas::green(), true, true, false);
	}

	ocean_assert(debugFrameA && debugFrameB);
	Frame debugFrame(FrameType(debugFrameA, debugFrameA.width() + debugFrameB.width(), std::max(debugFrameA.height(), debugFrameB.height())));

	debugFrame.copy(0, 0, debugFrameA);
	debugFrame.copy(int(debugFrameA.width()), 0, debugFrameB);

	RelocalizerDebugElements::get().updateElement(RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES, std::move(debugFrame));
}

}

}

}
