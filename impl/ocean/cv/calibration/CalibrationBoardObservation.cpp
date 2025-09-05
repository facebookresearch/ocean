/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/CalibrationBoardObservation.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

CalibrationBoardObservation::CalibrationBoardObservation(const size_t imageId, SharedAnyCamera camera, const HomogenousMatrix4& board_T_camera, CalibrationBoard::ObjectPointIds&& objectPointIds, Vectors3&& objectPoints, Vectors2&& imagePoints) :
	imageId_(imageId),
	camera_(std::move(camera)),
	board_T_camera_(board_T_camera),
	objectPointIds_(std::move(objectPointIds)),
	objectPoints_(std::move(objectPoints)),
	imagePoints_(std::move(imagePoints))
{
	ocean_assert(camera_ && camera_->isValid());
	ocean_assert(board_T_camera_.isValid());

	ocean_assert(objectPoints_.size() >= 4);
	ocean_assert(objectPointIds_.size() == objectPoints_.size());
	ocean_assert(objectPoints_.size() == imagePoints_.size());
}

void CalibrationBoardObservation::addCorrespondences(const CalibrationBoard::ObjectPointIds& objectPointIds, const Vectors3& objectPoints, const Vectors2& imagePoints)
{
	ocean_assert(!objectPointIds.empty());
	ocean_assert(objectPointIds.size() == objectPoints.size());
	ocean_assert(objectPointIds.size() == imagePoints.size());

	ocean_assert(objectPointIds_.size() == objectPoints_.size());
	ocean_assert(objectPointIds_.size() == imagePoints_.size());

#ifdef OCEAN_DEBUG
	{
		// let's ensure that we do not add the same object point twice

		CalibrationBoard::ObjectPointIdSet debugObjectPointIdSet(objectPointIds_.cbegin(), objectPointIds_.cend());

		for (const CalibrationBoard::ObjectPointId& objectPointId : objectPointIds)
		{
			debugObjectPointIdSet.insert(objectPointId);
		}

		ocean_assert(debugObjectPointIdSet.size() == objectPointIds_.size() + objectPointIds.size());
	}
#endif

	objectPointIds_.insert(objectPointIds_.cend(), objectPointIds.cbegin(), objectPointIds.cend());
	objectPoints_.insert(objectPoints_.cend(), objectPoints.cbegin(), objectPoints.cend());
	imagePoints_.insert(imagePoints_.cend(), imagePoints.cbegin(), imagePoints.cend());

	coverage_ = -1.0f;
}

float CalibrationBoardObservation::coverage() const
{
	ocean_assert(isValid());

	if (coverage_ < 0.0f)
	{
		coverage_ = determineCoverage(40u);
	}

	return coverage_;
}

float CalibrationBoardObservation::determineCoverage(const unsigned int pixelsPerBin) const
{
	ocean_assert(isValid());
	ocean_assert(pixelsPerBin >= 1u);

	if (!isValid() || pixelsPerBin == 0u)
	{
		return -1.0f;
	}

	if (imagePoints_.empty())
	{
		return 0.0f;
	}

	const unsigned int horizontalBins = std::max(1u, (camera_->width() + pixelsPerBin / 2u) / pixelsPerBin);
	const unsigned int verticalBins = std::max(1u, (camera_->height() + pixelsPerBin / 2u) / pixelsPerBin);

	if (occupancyArray_)
	{
		occupancyArray_.reset();
	}
	else
	{
		occupancyArray_ = Geometry::SpatialDistribution::OccupancyArray(Scalar(0), Scalar(0), Scalar(camera_->width()), Scalar(camera_->height()), horizontalBins, verticalBins);
	}

	size_t usedBins = 0;

	for (const Vector2& imagePoint : imagePoints_)
	{
		if (occupancyArray_.addPoint(imagePoint))
		{
			++usedBins;
		}
	}

	const size_t totalBins = occupancyArray_.bins();
	ocean_assert(totalBins != 0);

	return float(usedBins) / float(totalBins);
}

}

}

}
