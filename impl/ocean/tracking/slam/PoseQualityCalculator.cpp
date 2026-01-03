/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/PoseQualityCalculator.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

void PoseQualityCalculator::addObjectPoint(const LocalizedObjectPoint::LocalizationPrecision localizationPrecision)
{
	switch (localizationPrecision)
	{
		case LocalizedObjectPoint::LP_INVALID:
			ocean_assert(false && "This should never happen!");
			break;

		case LocalizedObjectPoint::LP_UNKNOWN:
			++numberUnknownPrecision_;
			break;

		case LocalizedObjectPoint::LP_LOW:
			++numberLowPrecision_;
			break;

		case LocalizedObjectPoint::LP_MEDIUM:
			++numberMediumPrecision_;
			break;

		case LocalizedObjectPoint::LP_HIGH:
			++numberHighPrecision_;
			break;
	}
}

CameraPose::PoseQuality PoseQualityCalculator::poseQuality() const
{
	const size_t numberObjectPoints = size();

	if (numberObjectPoints <= 10)
	{
		return CameraPose::PQ_INVALID;
	}

	if (numberHighPrecision_ >= 30 || (numberHighPrecision_ >= 10 && numberMediumPrecision_ >= 30))
	{
		return CameraPose::PQ_HIGH;
	}

	if (numberMediumPrecision_ >= 30)
	{
		return CameraPose::PQ_MEDIUM;
	}

	return CameraPose::PQ_LOW;
}

std::string PoseQualityCalculator::toString() const
{
	const size_t total = numberHighPrecision_ + numberMediumPrecision_ + numberLowPrecision_ + numberUnknownPrecision_;

	return CameraPose::translatePoseQuality(poseQuality()) + ", with " + String::toAString(total) + " total correspondences, high: " + String::toAString(numberHighPrecision_) + ", medium: " + String::toAString(numberMediumPrecision_) + ", low: " + String::toAString(numberLowPrecision_) + ", unknown: " + String::toAString(numberUnknownPrecision_);
}

}

}

}
