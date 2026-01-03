/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_GRAVITIES_H
#define META_OCEAN_TRACKING_SLAM_GRAVITIES_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/Mutex.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements a container for gravity vectors associated with frame indices.
 * The object is thread-safe.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT Gravities
{
	public:

		/**
		 * Definition of an unordered map mapping frame indices to gravity vectors.
		 */
		using GravityMap = std::unordered_map<Index32, Vector3>;

	public:

		/**
		 * Sets the gravity vector for a specific frame index.
		 * @param frameIndex The index of the frame for which the gravity will be set, with range [0, infinity)
		 * @param gravity The gravity vector to set
		 * @return True, always
		 */
		inline bool setGravity(const Index32 frameIndex, const Vector3& gravity);

		/**
		 * Returns whether this container holds a gravity vector for a specific frame index.
		 * @param frameIndex The index of the frame for which the gravity will be checked, with range [0, infinity)
		 * @param gravity Optional pointer to receive the gravity vector if it exists, nullptr to skip
		 * @return True, if the gravity vector exists for the specified frame index
		 */
		inline bool hasGravity(const Index32 frameIndex, Vector3* gravity) const;

	protected:

		/// The map mapping frame indices to gravity vectors.
		GravityMap gravityMap_;

		/// The mutex of this object.
		mutable Mutex mutex_;
};

inline bool Gravities::setGravity(const Index32 frameIndex, const Vector3& gravity)
{
	const WriteLock writeLock(mutex_);

	gravityMap_[frameIndex] = gravity;

	return true;
}

inline bool Gravities::hasGravity(const Index32 frameIndex, Vector3* gravity) const
{
	const ReadLock readLock(mutex_);

	const GravityMap::const_iterator iGravity = gravityMap_.find(frameIndex);

	if (iGravity == gravityMap_.cend())
	{
		return false;
	}

	if (gravity != nullptr)
	{
		*gravity = iGravity->second;
	}

	return true;
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_GRAVITIES_H
