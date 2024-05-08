/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/TrackerMono.h"

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

TrackerMono::TrackerMono(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction) :
	RelocalizerMono(std::move(imageFeaturePointDetectorFunction))
{
	// nothing to do here
}

bool TrackerMono::setFeatureMap(SharedUnifiedFeatureMap featureMap)
{
	const ScopedLock scopedLock(lock_);

	if (!RelocalizerMono::setFeatureMap(std::move(featureMap)))
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

bool TrackerMono::track(const AnyCamera& anyCamera, const Frame& yFrame, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const HomogenousMatrix4& world_T_roughCamera, Worker* worker)
{
	ocean_assert(anyCamera.isValid() && yFrame.isValid());
	ocean_assert(anyCamera.width() == yFrame.width() && anyCamera.height() == yFrame.height());
	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	ocean_assert(minimalNumberCorrespondences >= 4u);
	ocean_assert(maximalProjectionError >= 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid() || !anyCamera.isValid() || !yFrame.isValid())
	{
		return false;
	}

	constexpr unsigned int pyramidLayers = 1u;
	constexpr unsigned int coarseLayerRadius = 8u;

	if (!previousImagePoints_.empty())
	{
		ocean_assert(yPreviousFramePyramid_.isValid());
		ocean_assert(previousImagePoints_.size() == previousObjectPoints_.size());

		const size_t minimalNumberCorrespondencesTracking = std::max(4u, minimalNumberCorrespondences / 2u);

		yCurrentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*copyfirstLayer*/, worker);

		Vectors2 roughCurrentImagePoints;

		if (world_T_roughCamera.isValid())
		{
			const HomogenousMatrix4 flippedRoughCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughCamera));

			roughCurrentImagePoints.resize(previousObjectPoints_.size());
			anyCamera.projectToImageIF(flippedRoughCamera_T_world, previousObjectPoints_.data(), previousObjectPoints_.size(), roughCurrentImagePoints.data());

			const Scalar width = Scalar(yFrame.width()) - Scalar(0.1);
			const Scalar height = Scalar(yFrame.height()) - Scalar(0.1);

			for (Vector2& roughCurrentImagePoint : roughCurrentImagePoints)
			{
				roughCurrentImagePoint.x() = minmax(Scalar(0), roughCurrentImagePoint.x(), width);
				roughCurrentImagePoint.y() = minmax(Scalar(0), roughCurrentImagePoint.y(), height);
			}
		}
		else
		{
			roughCurrentImagePoints = previousImagePoints_;
		}

		Vectors2 currentImagePoints;

		Indices32 validIndices;
		if (CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(yPreviousFramePyramid_, yCurrentFramePyramid_, coarseLayerRadius, previousImagePoints_, roughCurrentImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 4u) && validIndices.size() >= minimalNumberCorrespondencesTracking)
		{
			if (validIndices.size() != previousImagePoints_.size())
			{
				previousImagePoints_ = Subset::subset(currentImagePoints, validIndices);
				previousObjectPoints_ = Subset::subset(previousObjectPoints_, validIndices);
			}
			else
			{
				std::swap(previousImagePoints_, currentImagePoints);
			}

			validIndices.clear();
			if (Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(previousObjectPoints_), ConstArrayAccessor<Vector2>(previousImagePoints_), randomGenerator_, world_T_camera, Index32(previousObjectPoints_.size() / 2), true, 40u, Numeric::sqr(maximalProjectionError), &validIndices) && validIndices.size() >= minimalNumberCorrespondencesTracking)
			{
				if (validIndices.size() != previousImagePoints_.size())
				{
					previousImagePoints_ = Subset::subset(previousImagePoints_, validIndices);
					previousObjectPoints_ = Subset::subset(previousObjectPoints_, validIndices);
				}

				std::swap(yPreviousFramePyramid_, yCurrentFramePyramid_);

				if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES))
				{
					createDebuggingElementFeatureCorrespondences(anyCamera, world_T_camera);
				}

				return true;
			}
		}

		previousImagePoints_.clear();
	}

	Indices32 usedObjectPointIds;
	Vectors2 usedImagePoints;

	constexpr Scalar relocalizerInlierRate = Scalar(0.15);

	if (!relocalize(anyCamera, yFrame, world_T_camera, minimalNumberCorrespondences, maximalProjectionError, relocalizerInlierRate, world_T_roughCamera, worker, &usedObjectPointIds, &usedImagePoints))
	{
		return false;
	}

	ocean_assert(usedObjectPointIds.size() >= size_t(minimalNumberCorrespondences));
	ocean_assert(usedObjectPointIds.size() == usedImagePoints.size());

	previousObjectPoints_.clear();
	previousObjectPoints_.reserve(usedObjectPointIds.size());

	const Vectors3& objectPoints = featureMap_->objectPoints();

	for (size_t n = 0; n < usedObjectPointIds.size(); ++n)
	{
		const ObjectPointIdMap::const_iterator iObjectPointId = objectPointIdMap_.find(usedObjectPointIds[n]);
		ocean_assert(iObjectPointId != objectPointIdMap_.cend());

		const Index32& objectPointIndex = iObjectPointId->second;

		ocean_assert(objectPointIndex < objectPoints.size());
		previousObjectPoints_.emplace_back(objectPoints[objectPointIndex]);
	}

	previousImagePoints_ = std::move(usedImagePoints);

	yPreviousFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), true /*copyFirstLayer*/, worker);

	if (RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES))
	{
		createDebuggingElementFeatureCorrespondences(anyCamera, world_T_camera);
	}

	return true;
}

void TrackerMono::createDebuggingElementFeatureCorrespondences(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera)
{
	ocean_assert(anyCamera.isValid() && world_T_camera.isValid());

	ocean_assert(RelocalizerDebugElements::get().isElementActive(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES));

	Frame debugFrame;

	Frame yFrame(yPreviousFramePyramid_.finestLayer(), Frame::ACM_USE_KEEP_LAYOUT);
	yFrame.setPixelFormat(FrameType::FORMAT_Y8);

	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Tracking::Utilities::paintCorrespondences<7u, 3u>(debugFrame, anyCamera, world_T_camera, previousObjectPoints_.data(), previousImagePoints_.data(), previousObjectPoints_.size(), Scalar(3), CV::Canvas::blue(), CV::Canvas::green(), CV::Canvas::red(), CV::Canvas::green(), true, true, false);

	RelocalizerDebugElements::get().updateElement(RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES, std::move(debugFrame));
}

}

}

}
