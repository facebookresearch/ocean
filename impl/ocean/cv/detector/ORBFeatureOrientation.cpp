/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/ORBFeatureOrientation.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

void ORBFeatureOrientation::determineFeatureOrientation(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures& featurePoints, Worker* worker)
{
	ocean_assert(linedIntegralFrame);
	ocean_assert(width > 31u && height > 31u);

	if (worker != nullptr)
	{
		worker->executeFunction(Ocean::Worker::Function::createStatic(&ORBFeatureOrientation::determineFeatureOrientationSubset, linedIntegralFrame, width, height, &featurePoints, 0u, 0u), 0u, (unsigned int)(featurePoints.size()));
	}
	else
	{
		determineFeatureOrientationSubset(linedIntegralFrame, width, height, &featurePoints, 0u, (unsigned int)(featurePoints.size()));
	}
}

void ORBFeatureOrientation::determineFeatureOrientationSubset(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures* const featurePoints, const unsigned int startIndex, const unsigned int range)
{
	ocean_assert(linedIntegralFrame);
	ocean_assert(width > 31u && height > 31u);
	ocean_assert(startIndex + range <= (unsigned int)(featurePoints->size()));

	for (unsigned int i = startIndex; i < startIndex + range; i++)
	{
		ORBFeature& feature = (*featurePoints)[i];

		const Vector2 centerPosition = feature.observation() + Vector2(Scalar(0.5), Scalar(0.5));

		feature.setOrientation(determineFeatureOrientation31(linedIntegralFrame, width, height, centerPosition));
	}
}

Scalar ORBFeatureOrientation::determineFeatureOrientation31(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, const Vector2& position)
{
	ocean_assert(linedIntegralFrame);
	ocean_assert(width >= 31u && height >= 31u);

	ocean_assert(position.x() > Scalar(0) && position.x() < Scalar(width) - Scalar(15));
	ocean_assert(position.y() > Scalar(0) && position.y() < Scalar(height) - Scalar(15));

	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	const Scalar m_10 =	FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(1, 0), CV::PC_CENTER, 1, 29)
		+ 2 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(2, 0), CV::PC_CENTER, 1, 29)
		+ 3 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(3, 0), CV::PC_CENTER, 1, 29)
		+ 4 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(4, 0), CV::PC_CENTER, 1, 29)
		+ 5 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(5, 0), CV::PC_CENTER, 1, 29)
		+ 6 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(6, 0), CV::PC_CENTER, 1, 27)
		+ 7 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(7, 0), CV::PC_CENTER, 1, 27)
		+ 8 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(8, 0), CV::PC_CENTER, 1, 25)
		+ 9 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(9, 0), CV::PC_CENTER, 1, 25)
		+ 10 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(10, 0), CV::PC_CENTER, 1, 23)
		+ 11 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(11, 0), CV::PC_CENTER, 1, 21)
		+ 12 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(12, 0), CV::PC_CENTER, 1, 19)
		+ 13 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(13, 0), CV::PC_CENTER, 1, 15)
		+ 14 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(14, 0), CV::PC_CENTER, 1, 11)
		+ 15 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(15, 0), CV::PC_CENTER, 1, 1)
		- FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-1, 0), CV::PC_CENTER, 1, 29)
		- 2 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-2, 0), CV::PC_CENTER, 1, 29)
		- 3 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-3, 0), CV::PC_CENTER, 1, 29)
		- 4 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-4, 0), CV::PC_CENTER, 1, 29)
		- 5 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-5, 0), CV::PC_CENTER, 1, 29)
		- 6 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-6, 0), CV::PC_CENTER, 1, 27)
		- 7 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-7, 0), CV::PC_CENTER, 1, 27)
		- 8 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-8, 0), CV::PC_CENTER, 1, 25)
		- 9 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-9, 0), CV::PC_CENTER, 1, 25)
		- 10 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-10, 0), CV::PC_CENTER, 1, 23)
		- 11 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-11, 0), CV::PC_CENTER, 1, 21)
		- 12 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-12, 0), CV::PC_CENTER, 1, 19)
		- 13 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-13, 0), CV::PC_CENTER, 1, 15)
		- 14 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-14, 0), CV::PC_CENTER, 1, 11)
		- 15 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(-15, 0), CV::PC_CENTER, 1, 1);

	const Scalar m_01 =	FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 1), CV::PC_CENTER, 29, 1)
		+ 2 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 2), CV::PC_CENTER, 29, 1)
		+ 3 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 3), CV::PC_CENTER, 29, 1)
		+ 4 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 4), CV::PC_CENTER, 29, 1)
		+ 5 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 5), CV::PC_CENTER, 29, 1)
		+ 6 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 6), CV::PC_CENTER, 27, 1)
		+ 7 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 7), CV::PC_CENTER, 27, 1)
		+ 8 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 8), CV::PC_CENTER, 25, 1)
		+ 9 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 9), CV::PC_CENTER, 25, 1)
		+ 10 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 10), CV::PC_CENTER, 23, 1)
		+ 11 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 11), CV::PC_CENTER, 21, 1)
		+ 12 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 12), CV::PC_CENTER, 19, 1)
		+ 13 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 13), CV::PC_CENTER, 15, 1)
		+ 14 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 14), CV::PC_CENTER, 11, 1)
		+ 15 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, 15), CV::PC_CENTER, 1, 1)
		- FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -1), CV::PC_CENTER, 29, 1)
		- 2 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -2), CV::PC_CENTER, 29, 1)
		- 3 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -3), CV::PC_CENTER, 29, 1)
		- 4 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -4), CV::PC_CENTER, 29, 1)
		- 5 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -5), CV::PC_CENTER, 29, 1)
		- 6 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -6), CV::PC_CENTER, 27, 1)
		- 7 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -7), CV::PC_CENTER, 27, 1)
		- 8 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -8), CV::PC_CENTER, 25, 1)
		- 9 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -9), CV::PC_CENTER, 25, 1)
		- 10 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -10), CV::PC_CENTER, 23, 1)
		- 11 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -11), CV::PC_CENTER, 21, 1)
		- 12 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -12), CV::PC_CENTER, 19, 1)
		- 13 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -13), CV::PC_CENTER, 15, 1)
		- 14 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -14), CV::PC_CENTER, 11, 1)
		- 15 * FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, position + Vector2(0, -15), CV::PC_CENTER, 1, 1);

	const Scalar angle = Ocean::Numeric::atan2(Scalar(m_01), Scalar(m_10));

	return Numeric::angleAdjustPositive(angle);
}

}

}

}
