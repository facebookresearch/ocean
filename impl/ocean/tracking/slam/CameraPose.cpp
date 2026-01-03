/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/CameraPose.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

std::string CameraPose::translatePoseQuality(const PoseQuality poseQuality)
{
	switch (poseQuality)
	{
		case PQ_INVALID:
			return std::string("Invalid");

		case PQ_LOW:
			return std::string("Low");

		case PQ_MEDIUM:
			return std::string("Medium");

		case PQ_HIGH:
			return std::string("High");
	}

	ocean_assert(false && "Invalid pose quality!");
	return std::string("Invalid");
}

CameraPose::EstimatedMotion CameraPose::motionFromOpticalFlow(const Scalar* imagePointSqrDistances, const size_t size, const unsigned int width, const unsigned int height)
{
	ocean_assert(imagePointSqrDistances != nullptr);
	ocean_assert(size >= 1);

	ocean_assert(width != 0u && height != 0u);

	if (size <= 1 || width == 0u || height == 0u)
	{
		return EM_UNKNOWN;
	}

	Scalar sumSqrOpticalFlow = 0;
	for (size_t n = 0; n < size; n++)
	{
		sumSqrOpticalFlow += imagePointSqrDistances[n];
	}

	const Scalar averageSqrOpticalFlow = sumSqrOpticalFlow / Scalar(size);
	const Scalar averageOpticalFlow = Numeric::sqrt(averageSqrOpticalFlow);

	const unsigned int minResolution = std::min(width, height);
	const Scalar averageOpticalFlowRatio = averageOpticalFlow / Scalar(minResolution);

	if (averageOpticalFlowRatio >= 0.005) // 5%
	{
		return EM_TRANSLATIONAL;
	}

	return EM_STATIONARY;
}

}

}

}
