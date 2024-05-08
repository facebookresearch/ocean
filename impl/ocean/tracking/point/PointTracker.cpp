/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/point/PointTracker.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

PointTracker::PointTracker()
{
	// nothing to do here
}

PointTracker::PointTracker(PointTracker&& pointTracker) noexcept
{
	*this = std::move(pointTracker);
}

PointTracker::PointTracker(const PointTracker& pointTracker) :
	database_(pointTracker.database_),
	previousFramePyramid_(pointTracker.previousFramePyramid_, true /*copyData*/),
	currentFramePyramid_(pointTracker.currentFramePyramid_, true /*copyData*/),
	previousFrameIndex_(pointTracker.previousFrameIndex_),
	featurePointStrengthThreshold_(pointTracker.featurePointStrengthThreshold_),
	binSize_(pointTracker.binSize_)
{
	// nothing to do here
}

Index32 PointTracker::newFrame(const Frame& yFrame, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(!previousFramePyramid_.isValid() || previousFramePyramid_.frameType() == FrameType(yFrame, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));

	if (!yFrame.isValid() || !FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u) || (previousFramePyramid_ && previousFramePyramid_.frameType() != FrameType(yFrame, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>())))
	{
		return invalidFrameIndex;
	}

	const ScopedLock scopedLock(lock_);

	const unsigned int currentFrameIndex = previousFrameIndex_ + 1u;

	if (!database_.addPose<false>(currentFrameIndex))
	{
		ocean_assert(false && "Should never happen!");
		return invalidFrameIndex;
	}

	constexpr bool copyFirstLayer = true; // we need to make a copy of the first layer, as this pyramid will be used as 'previousPyramid' in the next call of newFrame()

	const unsigned int pyramidLayers = 6u; // **TODO**
	currentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), copyFirstLayer, worker);

	ocean_assert(binSize_ != 0u);
	const unsigned int horizontalBins = minmax(1u, (yFrame.width() + binSize_ / 2u) / binSize_, yFrame.width() / 4u);
	const unsigned int verticalBins = minmax(1u, (yFrame.height() + binSize_ / 2u) / binSize_, yFrame.height() / 4u);

	Geometry::SpatialDistribution::OccupancyArray occupancyArray(Box2(Scalar(0), Scalar(0), Scalar(yFrame.width()), Scalar(yFrame.height())), horizontalBins, verticalBins);

	if (previousFramePyramid_)
	{
		// we have a valid frame pyramid of a previous frame

		// we need to gather all image points (observations of feature points) visible in the previous image

		Indices32 previousImagePointIds;
		Vectors2 previousImagePoints(database_.imagePoints<false>(previousFrameIndex_, &previousImagePointIds));

		if (!previousImagePoints.empty())
		{
			Vectors2 currentImagePoints;
			Indices32 validCorrespondences;
			if (!trackFeaturePoints(trackingMode_, previousFramePyramid_, currentFramePyramid_, previousImagePoints, currentImagePoints, validCorrespondences, worker))
			{
				ocean_assert(false && "Should never happen!");
				return invalidFrameIndex;
			}

			// now we have to update our database based on the new tracked points

			ocean_assert(previousImagePoints.size() == currentImagePoints.size());
			ocean_assert(validCorrespondences.size() <= currentImagePoints.size());

			for (const Index32& validIndex : validCorrespondences)
			{
				ocean_assert(validIndex < (unsigned int)previousImagePoints.size());

				const Vector2& currentImagePoint = currentImagePoints[validIndex];

				const Index32 previousImagePointId = previousImagePointIds[validIndex];
				const Index32 currentObjectPointId = database_.objectPointFromImagePoint<false>(previousImagePointId);

				// let's add the new point to our database and let's connect the point with the corresponding object point (feature point)

				const Index32 currentImagePointId = database_.addImagePoint<false>(currentImagePoint);

				database_.attachImagePointToPose<false>(currentImagePointId, currentFrameIndex);
				database_.attachImagePointToObjectPoint<false>(currentImagePointId, currentObjectPointId);

				occupancyArray.addPoint(currentImagePoint);
			}
		}
	}

	// now we can check whether we have regions in our current frame in which we currently do not have any tracked object points (feature points)
	// we can add new image points to those locations

	if (occupancyArray.freeBins() > occupancyArray.bins() * 10u / 100u) // 10%
	{
		Vectors2 newFeaturePoints;
		detectNewFeaturePoints(yFrame, occupancyArray, newFeaturePoints, worker);

		// now let's add the new feature points to the data base
		// each new feature points represents a new object point (feature point) with corresponding image point

		for (const Vector2& newFeaturePoint : newFeaturePoints)
		{
			const Index32 newObjectPointId = database_.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			const Index32 newImagePointId = database_.addImagePoint<false>(newFeaturePoint);

			database_.attachImagePointToPose<false>(newImagePointId, currentFrameIndex);
			database_.attachImagePointToObjectPoint<false>(newImagePointId, newObjectPointId);
		}
	}

	// we prepare our data for the next call of newFrame()

	ocean_assert(previousFrameIndex_ + 1u == currentFrameIndex);
	previousFrameIndex_++;

	std::swap(previousFramePyramid_, currentFramePyramid_);

	return currentFrameIndex;
}

void PointTracker::clearUpTo(const unsigned int frameIndex)
{
	ocean_assert(frameIndex != invalidFrameIndex);

	const ScopedLock scopedLock(lock_);

	Index32 lowestFrameIndex;
	Index32 highestFrameIndex;

	if (!database_.poseBorders<false>(lowestFrameIndex, highestFrameIndex) || frameIndex <= lowestFrameIndex)
	{
		// nothing to remove
		return;
	}

	if (highestFrameIndex < frameIndex)
	{
		// we can clear the entire database

		database_.clear<false>();
		return;
	}

	// we have to remove individual elements from the database

	for (unsigned int n = lowestFrameIndex; n < frameIndex; ++n)
	{
		const IndexSet32 imagePointIds = database_.imagePointIds<false>(n);

		for (const Index32& imagePointId : imagePointIds)
		{
			const Index32 objectPointId = database_.objectPointFromImagePoint<false>(imagePointId);

			database_.removeImagePoint<false>(imagePointId);

			if (database_.imagePointsFromObjectPoint<false>(objectPointId).empty())
			{
				database_.removeObjectPoint<false>(objectPointId);
			}
		}

		database_.removePose<false>(n);
	}

#ifdef OCEAN_DEBUG
	Index32 debugLowestFrameIndex;
	Index32 debugHighestFrameIndex;

	ocean_assert(database_.poseBorders<false>(debugLowestFrameIndex, debugHighestFrameIndex));
	ocean_assert_and_suppress_unused(debugLowestFrameIndex == frameIndex, debugLowestFrameIndex);
	ocean_assert_and_suppress_unused(debugHighestFrameIndex == highestFrameIndex, debugHighestFrameIndex);
#endif
}

PointTracker::PointTracks PointTracker::pointTracks(const Index32 imageIndex, const unsigned int maximalLength)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(imageIndex <= previousFrameIndex_);

	const Tracking::Database::IdIdPointPairsMap featurePointsMap = database_.imagePoints<false>(imageIndex, true, 1, size_t(maximalLength));

	PointTracks pointTracks;
	pointTracks.reserve(featurePointsMap.size());

	for (Tracking::Database::IdIdPointPairsMap::const_iterator i = featurePointsMap.begin(); i != featurePointsMap.end(); ++i)
	{
		Vectors2 imagePoints;
		imagePoints.reserve(i->second.size());

		for (Tracking::Database::IdPointPairs::const_iterator iI = i->second.begin(); iI != i->second.end(); ++iI)
		{
			imagePoints.push_back(iI->second);
		}

		pointTracks.push_back(std::move(imagePoints));
	}

	return pointTracks;
}

PointTracker& PointTracker::operator=(PointTracker&& pointTracker) noexcept
{
	if (this != &pointTracker)
	{
		database_ = std::move(pointTracker.database_);

		previousFramePyramid_ = std::move(pointTracker.previousFramePyramid_);
		currentFramePyramid_ = std::move(pointTracker.currentFramePyramid_);

		previousFrameIndex_ = pointTracker.previousFrameIndex_;
		pointTracker.previousFrameIndex_ = invalidFrameIndex;
		featurePointStrengthThreshold_ = pointTracker.featurePointStrengthThreshold_;
		pointTracker.featurePointStrengthThreshold_ = 15u;
		binSize_ = pointTracker.binSize_;
		pointTracker.binSize_ = 40u;
	}

	return *this;
}

PointTracker& PointTracker::operator=(const PointTracker& pointTracker)
{
	if (this != &pointTracker)
	{
		database_ = pointTracker.database_;

		previousFramePyramid_ = CV::FramePyramid(pointTracker.previousFramePyramid_, true /*copyData*/);
		currentFramePyramid_ = CV::FramePyramid(pointTracker.currentFramePyramid_, true);

		previousFrameIndex_ = pointTracker.previousFrameIndex_;
		featurePointStrengthThreshold_ = pointTracker.featurePointStrengthThreshold_;
		binSize_ = pointTracker.binSize_;
	}

	return *this;
}

void PointTracker::detectNewFeaturePoints(const Frame& yFrame, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Vectors2& newFeaturePoints, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(newFeaturePoints.empty());

	ocean_assert(occupancyArray.width() == Scalar(yFrame.width()));
	ocean_assert(occupancyArray.height() == Scalar(yFrame.height()));

	CV::Detector::HarrisCorners harrisCorners;
	CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), featurePointStrengthThreshold_, true, harrisCorners, true, worker);

	// we sort the corners according their strength
	std::sort(harrisCorners.begin(), harrisCorners.end());

	ocean_assert(harrisCorners.empty() || harrisCorners.front().strength() >= harrisCorners.back().strength());

	newFeaturePoints.reserve(min(size_t(occupancyArray.freeBins()), harrisCorners.size()));

	for (const CV::Detector::HarrisCorner& corner : harrisCorners)
	{
		if (occupancyArray.addPoint(corner.observation()))
		{
			newFeaturePoints.push_back(corner.observation());
		}
	}
}

bool PointTracker::trackFeaturePoints(const TrackingMode trackingMode, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker)
{
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.frameType() == currentFramePyramid.frameType());

	switch (trackingMode)
	{
		case TM_SSD_7:
			return CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_SSD_15:
			return CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_SSD_31:
			return CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_ZM_SSD_7:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_ZM_SSD_15:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_ZM_SSD_31:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, 4u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), worker, &validIndices, 2u);

		case TM_END:
			break;
	}

	ocean_assert(false && "Invalid tracking mode!");
	return false;
}

}

}

}
