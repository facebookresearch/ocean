/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_OBSERVATION_H
#define META_OCEAN_TRACKING_SLAM_OBSERVATION_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

// Forward declaration.
class Observation;

/**
 * Definition of a vector holding observations.
 * @ingroup trackingslam
 */
using Observations = std::vector<Observation>;

/**
 * This class implements an observation of a 3D feature point in a camera frame.
 * The observation is defined by the 2D position of the 3D feature point and the corresponding frame index.
 * @ingroup trackingslam
 */
class Observation
{
	public:

		/**
		 * Default constructor creating an invalid observation.
		 */
		Observation() = default;

		/**
		 * Creates a new observation.
		 * @param frameIndex The index of the camera frame in which the observation was made, with range [0, infinity)
		 * @param imagePoint The image point of the observation
		 */
		inline Observation(const Index32 frameIndex, const Vector2& imagePoint);

		/**
		 * Returns the index of the camera frame in which the observation was made.
		 * @return The observation's frame index
		 */
		inline Index32 frameIndex() const;

		/**
		 * Returns the image point of this observation.
		 * @return The observation's image point
		 */
		inline const Vector2& imagePoint() const;

	protected:

		/// The index of the camera frame in which the observation was made.
		Index32 frameIndex_ = Index32(-1);

		/// The image point of the observation.
		Vector2 imagePoint_;
};

inline Observation::Observation(const Index32 frameIndex, const Vector2& imagePoint) :
	frameIndex_(frameIndex),
	imagePoint_(imagePoint)
{
	// nothing to do here
}

inline Index32 Observation::frameIndex() const
{
	return frameIndex_;
}

inline const Vector2& Observation::imagePoint() const
{
	return imagePoint_;
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_OBSERVATION_H
