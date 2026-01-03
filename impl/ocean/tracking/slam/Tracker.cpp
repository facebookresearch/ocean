/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/Tracker.h"

#include "ocean/cv/FramePyramid.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

bool Tracker::Configuration::isInsideExpectedFrameInterval(const Timestamp& previousFrameTimestamp, const Timestamp& currentFrameTimestamp) const
{
	ocean_assert(isValid());
	ocean_assert(previousFrameTimestamp.isValid() && currentFrameTimestamp.isValid());

	const double actualFrameInterval = double(currentFrameTimestamp - previousFrameTimestamp);

	const double expectedFrameInterval = NumericD::ratio(1.0, expectedFramesPerSecond_, 1.0 / 30.0);

	const double minimalFrameInterval = expectedFrameInterval * 0.5;
	const double maximalFrameInterval = expectedFrameInterval * 1.5;

	return minimalFrameInterval <= actualFrameInterval && actualFrameInterval <= maximalFrameInterval;
}

Tracker::TrackingParameters::TrackingParameters(const unsigned int width, const unsigned int height, const Tracker::Configuration& configuration)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(configuration.isValid());

	patchSize_ = configuration.patchSize_;

	const unsigned int invalidWidthOrHeight = patchSize_ * 2u;

	if (width <= invalidWidthOrHeight || height <= invalidWidthOrHeight)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	constexpr unsigned int minimalLayers = 1u;

	{
		// unguided

		constexpr unsigned int maximalLayers = (unsigned int)(-1);

		const unsigned int minimalCoarsestLayerRadius = 4u;
		const unsigned int maximalCoarsestLayerRadius = 8u;

		unsigned int idealLayers = 0u;
		unsigned int idealCoarsestLayerRadius = 0u;

		if (CV::FramePyramid::idealTrackingParameters(width, height, invalidWidthOrHeight, invalidWidthOrHeight, configuration.maximalTrackingDistanceUnguided_, minimalLayers, maximalLayers, minimalCoarsestLayerRadius, maximalCoarsestLayerRadius, idealLayers, idealCoarsestLayerRadius))
		{
			trackingParametersUnguided_ = TrackingParameterPair(idealLayers, idealCoarsestLayerRadius);
		}
	}

	{
		// guided with IMU

		for (unsigned int maximalLayers = 2u; maximalLayers <= 16u; ++maximalLayers)
		{
			const unsigned int minimalCoarsestLayerRadius = 2u;
			const unsigned int maximalCoarsestLayerRadius = 8u;

			unsigned int idealLayers = 0u;
			unsigned int idealCoarsestLayerRadius = 0u;

			if (CV::FramePyramid::idealTrackingParameters(width, height, invalidWidthOrHeight, invalidWidthOrHeight, configuration.maximalTrackingDistanceGuidedIMU_, minimalLayers, maximalLayers, minimalCoarsestLayerRadius, maximalCoarsestLayerRadius, idealLayers, idealCoarsestLayerRadius))
			{
				trackingParametersGuidedIMU_ = TrackingParameterPair(idealLayers, idealCoarsestLayerRadius);
				break;
			}
		}
	}

	{
		// guided with object point

		for (unsigned int maximalLayers = 2u; maximalLayers <= 16u; ++maximalLayers)
		{
			const unsigned int minimalCoarsestLayerRadius = 2u;
			const unsigned int maximalCoarsestLayerRadius = 8u;

			unsigned int idealLayers = 0u;
			unsigned int idealCoarsestLayerRadius = 0u;

			if (CV::FramePyramid::idealTrackingParameters(width, height, invalidWidthOrHeight, invalidWidthOrHeight, configuration.maximalTrackingDistanceGuidedObjectPoint_, minimalLayers, maximalLayers, minimalCoarsestLayerRadius, maximalCoarsestLayerRadius, idealLayers, idealCoarsestLayerRadius))
			{
				trackingParametersGuidedObjectPoint_ = TrackingParameterPair(idealLayers, idealCoarsestLayerRadius);
				break;
			}
		}
	}

#ifdef OCEAN_DEBUG

	Log::debug() << "Tracking parameters for image resolution " << width << "x" << height << ", and patch size: " << patchSize_;

	const unsigned int diagonal = (unsigned int)(NumericF::sqrt(float(width * width + height * height)) + 0.5f);

	const unsigned int necessaryTrackingDistanceUnguided = (unsigned int)(diagonal * configuration.maximalTrackingDistanceUnguided_ + 0.5);
	const unsigned int necessaryTrackingDistanceGuidedIMU = (unsigned int)(diagonal * configuration.maximalTrackingDistanceGuidedIMU_ + 0.5);
	const unsigned int necessaryTrackingDistanceGuidedObjectPoint = (unsigned int)(diagonal * configuration.maximalTrackingDistanceGuidedObjectPoint_ + 0.5);

	const unsigned int actualTrackingDistanceUnguided = trackingParametersUnguided_.coarsestLayerRadius_ * (1u << (trackingParametersUnguided_.layers_ - 1u));
	const unsigned int actualTrackingDistanceGuidedIMU = trackingParametersGuidedIMU_.coarsestLayerRadius_ * (1u << (trackingParametersGuidedIMU_.layers_ - 1u));
	const unsigned int actualTrackingDistanceGuidedObjectPoint = trackingParametersGuidedObjectPoint_.coarsestLayerRadius_ * (1u << (trackingParametersGuidedObjectPoint_.layers_ - 1u));

	Log::debug() << "Unguided: Layers: " << trackingParametersUnguided_.layers_ << ", Coarsest radius: " << trackingParametersUnguided_.coarsestLayerRadius_ << ", necessary tracking distance: " << necessaryTrackingDistanceUnguided << ", actual: " << actualTrackingDistanceUnguided;
	Log::debug() << "Guided IMU: Layers: " << trackingParametersGuidedIMU_.layers_ << ", Coarsest radius: " << trackingParametersGuidedIMU_.coarsestLayerRadius_<< ", necessary tracking distance: " << necessaryTrackingDistanceGuidedIMU << ", actual: " << actualTrackingDistanceGuidedIMU;
	Log::debug() << "Guided Pose: Layers: " << trackingParametersGuidedObjectPoint_.layers_ << ", Coarsest radius: " << trackingParametersGuidedObjectPoint_.coarsestLayerRadius_<< ", necessary tracking distance: " << necessaryTrackingDistanceGuidedObjectPoint << ", actual: " << actualTrackingDistanceGuidedObjectPoint;

#endif // OCEAN_DEBUG
}

const Tracker::TrackingParameterPair& Tracker::TrackingParameters::parameterPair(const HomogenousMatrix4& world_T_previousCamera, const Quaternion& previousCamera_Q_currentCamera, const Scalar strongMotionAngle) const
{
	ocean_assert(strongMotionAngle > Scalar(0) && strongMotionAngle < Numeric::pi());

	if (previousCamera_Q_currentCamera.isValid())
	{
		// we have access to the IMU

		const bool normalCameraMotion = previousCamera_Q_currentCamera.angle() <= strongMotionAngle;

		if (world_T_previousCamera.isValid())
		{
			// we have access to the previous camera pose and thus can make a prediction with projected 3D object points

			if (normalCameraMotion)
			{
				// tracking parameters for predictions based on 3D object points

				return trackingParametersGuidedObjectPoint_;
			}
			else
			{
				// tracking parameters for predictions based on re-projected previous image points

				return trackingParametersGuidedIMU_;
			}
		}
		else
		{
			// we do not have a previous camera pose, so we cannot make predictions based on projected 3D object points

			if (normalCameraMotion)
			{
				return trackingParametersGuidedIMU_;
			}
		}
	}

	return trackingParametersUnguided_;
}

}

}

}
