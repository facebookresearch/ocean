/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/base/Median.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

void AdvancedMotion::PointCorrespondences::startForwardTracking(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid)
{
	ocean_assert(previousPyramid.isValid());
	ocean_assert(nextPyramid.isValid());
	ocean_assert(previousPoints_ != nullptr);

	const unsigned int maximalPyramidLayers = std::min({previousPyramid.layers(), nextPyramid.layers(), pyramidLayers_}); // maximal number of pyramid layers which can be used for tracking

	ocean_assert(maximalPyramidLayers >= 1u);
	ocean_assert(coarsestLayerIndex_ == (unsigned int)(-1));
	coarsestLayerIndex_ = maximalPyramidLayers - 1u;

	nextPyramidFinestLayerWidth_ = nextPyramid.finestWidth();
	nextPyramidFinestLayerHeight_ = nextPyramid.finestHeight();

	const unsigned int nextLayerWidth = nextPyramid.width(coarsestLayerIndex_);
	const unsigned int nextLayerHeight = nextPyramid.height(coarsestLayerIndex_);

	const Scalar coarsestLayerFactor = Scalar(1) / Scalar(CV::FramePyramid::sizeFactor(coarsestLayerIndex_));

	for (size_t pointIndex = 0; pointIndex < correspondences_; ++pointIndex)
	{
		const int32_t downsampledX = Numeric::round32(nextPoints_[pointIndex].x() * coarsestLayerFactor);
		const int32_t downsampledY = Numeric::round32(nextPoints_[pointIndex].y() * coarsestLayerFactor);

		const int32_t clampedX = std::min(downsampledX, int32_t(nextLayerWidth) - 1);
		const int32_t clampedY = std::min(downsampledY, int32_t(nextLayerHeight) - 1);

		ocean_assert(clampedX >= 0 && clampedY >= 0);

		nextPoints_[pointIndex] = Vector2(Scalar(clampedX), Scalar(clampedY));
	}
}

void AdvancedMotion::PointCorrespondences::startBackwardTracking(const CV::FramePyramid& /*previousPyramid*/, const CV::FramePyramid& nextPyramid)
{
	ocean_assert(layerIndex_ != (unsigned int)(-1));
	layerIndex_ = (unsigned int)(-1);

	internalBackwardNextPoints_.resize(correspondences_);

	ocean_assert(coarsestLayerIndex_ != (unsigned int)(-1));
	const Scalar coarsestLayerFactor = Scalar(1) / Scalar(CV::FramePyramid::sizeFactor(coarsestLayerIndex_));

	const unsigned int nextLayerWidth = nextPyramid.width(coarsestLayerIndex_);
	const unsigned int nextLayerHeight = nextPyramid.height(coarsestLayerIndex_);

	for (size_t pointIndex = 0; pointIndex < correspondences_; ++pointIndex)
	{
		const int32_t downsampledX = Numeric::round32(previousPoints_[pointIndex].x() * coarsestLayerFactor);
		const int32_t downsampledY = Numeric::round32(previousPoints_[pointIndex].y() * coarsestLayerFactor);

		const int32_t clampedX = std::min(downsampledX, int32_t(nextLayerWidth) - 1);
		const int32_t clampedY = std::min(downsampledY, int32_t(nextLayerHeight) - 1);

		ocean_assert(clampedX >= 0 && clampedY >= 0);

		internalBackwardNextPoints_[pointIndex] = Vector2(Scalar(clampedX), Scalar(clampedY));
	}

	std::fill(validCorrespondences_, validCorrespondences_ + correspondences_, 1u);

	ocean_assert(forwardTracking_);
	forwardTracking_ = false;
}

bool AdvancedMotion::PointCorrespondences::startLayer(const unsigned int layerIndex, const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid)
{
	ocean_assert(pyramidLayers_ != 0u);

	if (layerIndex >= pyramidLayers_)
	{
		return false;
	}

	const bool coarsestLayer = layerIndex_ == (unsigned int)(-1);

	layerIndex_ = layerIndex;

	invLayerFactor_ = Scalar(1) / Scalar(CV::FramePyramid::sizeFactor(layerIndex_));

	layerRadius_ = coarsestLayer ? coarsestLayerRadius_ : 2u;

	previousLayerWidth_ = previousPyramid.width(layerIndex_);
	previousLayerHeight_ = previousPyramid.height(layerIndex_);

	nextLayerWidth_ = nextPyramid.width(layerIndex_);
	nextLayerHeight_ = nextPyramid.height(layerIndex_);

	return true;
}

void AdvancedMotion::PointCorrespondences::propagateNextPositionDownsampled(const size_t pointIndex, const CV::PixelPosition& nextPoint)
{
	ocean_assert(layerIndex_ != (unsigned int)(-1));
	ocean_assert(layerIndex_ != 0u);

	ocean_assert(pointIndex < correspondences_);

	const Vector2 upsampledPoint(Scalar(nextPoint.x() * 2u), Scalar(nextPoint.y() * 2u)); // no clamping needed

	ocean_assert(upsampledPoint.x() >= 0 && upsampledPoint.y() >= 0);
	ocean_assert(upsampledPoint.x() < Scalar(nextPyramidFinestLayerWidth_) / Scalar(CV::FramePyramid::sizeFactor(layerIndex_ - 1u)));
	ocean_assert(upsampledPoint.y() < Scalar(nextPyramidFinestLayerHeight_) / Scalar(CV::FramePyramid::sizeFactor(layerIndex_ - 1u)));

	if (forwardTracking_)
	{
		nextPoints_[pointIndex] = upsampledPoint;
	}
	else
	{
		// early reject: let's see whether we are already too far away from our starting point

		const Vector2& previousPoint = previousPoints_[pointIndex];

		const Vector2 previousPointOnLayer = previousPoint * invLayerFactor_;

		const Scalar sqrDistance = Vector2(Scalar(nextPoint.x()), Scalar(nextPoint.y())).sqrDistance(previousPointOnLayer);

		if (sqrDistance <= maximalSqrErrorLayer_)
		{
			internalBackwardNextPoints_[pointIndex] = upsampledPoint;
		}
		else
		{
			validCorrespondences_[pointIndex] = uint8_t(0);
		}
	}
}

void AdvancedMotion::PointCorrespondences::propagateNextPosition(const size_t pointIndex, const Vector2& nextPoint)
{
	ocean_assert(layerIndex_ == 0u);

	ocean_assert(pointIndex < correspondences_);

	ocean_assert(nextPoint.x() >= 0 && nextPoint.y() >= 0);
	ocean_assert(nextPoint.x() < Scalar(nextPyramidFinestLayerWidth_));
	ocean_assert(nextPoint.y() < Scalar(nextPyramidFinestLayerHeight_));

	if (forwardTracking_)
	{
		nextPoints_[pointIndex] = nextPoint;
	}
	else
	{
		// we finished the sub-pixel tracking iteration in the backwards tracking step
		// let's see whether the backwards tracking result is close enough to the starting point

		const Vector2& internalBackwardNextPoint = nextPoint;

		const Vector2 forwardBackwardOffset = previousPoints_[pointIndex] - internalBackwardNextPoint;

		const Scalar sqrDistance = forwardBackwardOffset.sqr();

		// let's check whether forward and backward motion is almost identical

		uint8_t validCorrespondence = uint8_t(0);

		if (sqrDistance <= maximalSqrError_)
		{
			const Vector2 nextImagePoint(nextPoints_[pointIndex] + forwardBackwardOffset * Scalar(0.5));

			if (nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramidFinestLayerWidth_) && nextImagePoint.y() < Scalar(nextPyramidFinestLayerHeight_))
			{
				nextPoints_[pointIndex] = nextImagePoint;

				validCorrespondence = uint8_t(1);
			}
		}

		validCorrespondences_[pointIndex] = validCorrespondence;
	}
}

unsigned int AdvancedMotion::PointCorrespondences::coarsestPyramidLayer(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const PointCorrespondences* pointCorrespondenceGroups, const size_t numberCorrespondenceGroups)
{
	ocean_assert(previousPyramid.isValid());
	ocean_assert(nextPyramid.isValid());

	const unsigned int maximalPyramidLayers = std::min(previousPyramid.layers(), nextPyramid.layers()); // maximal number of pyramid layers which can be used for tracking

	unsigned int coarsestLayerIndex = 0u;

	for (size_t nGroup = 0; nGroup < numberCorrespondenceGroups; ++nGroup)
	{
		const PointCorrespondences& pointCorrespondences = pointCorrespondenceGroups[nGroup];

		if (pointCorrespondences.pyramidLayers() <= maximalPyramidLayers)
		{
			ocean_assert(pointCorrespondences.pyramidLayers() >= 1u);
			coarsestLayerIndex = std::max(coarsestLayerIndex, pointCorrespondences.pyramidLayers() - 1u);
		}
	}

	return coarsestLayerIndex;
}

AdvancedMotion::TrackingStatistic::TrackingStatistic(const unsigned int width, const unsigned int height) :
	width_(width),
	height_(height)
{
	ocean_assert(width_ > 0u);
	ocean_assert(height_ > 0u);
}

void AdvancedMotion::TrackingStatistic::addCorrespondences(const Vector2* previousImagePoints, const Vector2* nextImagePoints, const size_t size)
{
	ocean_assert(isValid());

	ocean_assert(previousImagePoints != nullptr);
	ocean_assert(nextImagePoints != nullptr);

	sqrDistances_.reserve(sqrDistances_.size() + size);

	for (size_t i = 0; i < size; ++i)
	{
		const Scalar sqrDistance = previousImagePoints[i].sqrDistance(nextImagePoints[i]);
		sqrDistances_.push_back(sqrDistance);
	}

	++measurements_;
}

void AdvancedMotion::TrackingStatistic::addCorrespondences(const Vector2* previousImagePoints, const Vector2* nextImagePoints, const uint8_t* validCorrespondences, const size_t size)
{
	ocean_assert(isValid());

	ocean_assert(previousImagePoints != nullptr);
	ocean_assert(nextImagePoints != nullptr);
	ocean_assert(validCorrespondences != nullptr);

	sqrDistances_.reserve(sqrDistances_.size() + size);

	for (size_t i = 0; i < size; ++i)
	{
		if (validCorrespondences[i] != 0u)
		{
			const Scalar sqrDistance = previousImagePoints[i].sqrDistance(nextImagePoints[i]);
			sqrDistances_.push_back(sqrDistance);
		}
	}

	++measurements_;
}

std::string AdvancedMotion::TrackingStatistic::toString() const
{
	ocean_assert(isValid());

	if (sqrDistances_.empty())
	{
		return std::string();
	}

	const Scalar diagonal = Numeric::sqrt(Scalar(width_ * width_ + height_ * height_));

	Scalars sqrDistancesCopy = sqrDistances_;

	Scalar sqrSum = Scalar(0);
	Scalar sqrMax = Numeric::minValue();

	for (const Scalar sqrDistance : sqrDistances_)
	{
		sqrSum += sqrDistance;

		if (sqrDistance > sqrMax)
		{
			sqrMax = sqrDistance;
		}
	}

	const Scalar average = Numeric::sqrt(sqrSum / Scalar(sqrDistances_.size()));

	const Scalar median = Numeric::sqrt(Median::percentile(sqrDistancesCopy.data(), sqrDistancesCopy.size(), 0.5));
	const Scalar p95 = Numeric::sqrt(Median::percentile(sqrDistancesCopy.data(), sqrDistancesCopy.size(), 0.95));
	const Scalar p99 = Numeric::sqrt(Median::percentile(sqrDistancesCopy.data(), sqrDistancesCopy.size(), 0.99));
	const Scalar p995 = Numeric::sqrt(Median::percentile(sqrDistancesCopy.data(), sqrDistancesCopy.size(), 0.995));
	const Scalar p999 = Numeric::sqrt(Median::percentile(sqrDistancesCopy.data(), sqrDistancesCopy.size(), 0.999));
	const Scalar maximum = Numeric::sqrt(sqrMax);

	const Scalar averagePercent = (average / diagonal) * Scalar(100);
	const Scalar medianPercent = (median / diagonal) * Scalar(100);
	const Scalar p95Percent = (p95 / diagonal) * Scalar(100);
	const Scalar p99Percent = (p99 / diagonal) * Scalar(100);
	const Scalar p995Percent = (p995 / diagonal) * Scalar(100);
	const Scalar p999Percent = (p999 / diagonal) * Scalar(100);
	const Scalar maximumPercent = (maximum / diagonal) * Scalar(100);

	std::string result = String::toAString(measurements_) + " measurements, " + String::toAString(sqrDistances_.size()) + " correspondences:";
	result += "\nAverage: " + String::toAString(average, 2u) + "px (" + String::toAString(averagePercent, 1u) + "%), ";
	result += "\nMedian: " + String::toAString(median, 2u) + "px (" + String::toAString(medianPercent, 1u) + "%), ";
	result += "\nP95: " + String::toAString(p95, 2u) + "px (" + String::toAString(p95Percent, 1u) + "%), ";
	result += "\nP99: " + String::toAString(p99, 2u) + "px (" + String::toAString(p99Percent, 1u) + "%), ";
	result += "\nP995: " + String::toAString(p995, 2u) + "px (" + String::toAString(p995Percent, 1u) + "%), ";
	result += "\nP999: " + String::toAString(p999, 2u) + "px (" + String::toAString(p999Percent, 1u) + "%), ";
	result += "\nMax: " + String::toAString(maximum, 2u) + "px (" + String::toAString(maximumPercent, 1u) + "%)";

	return result;
}

}

}

}
