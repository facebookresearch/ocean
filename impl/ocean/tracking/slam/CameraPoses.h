/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_CAMERA_POSES_H
#define META_OCEAN_TRACKING_SLAM_CAMERA_POSES_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/CameraPose.h"
#include "ocean/tracking/slam/Mutex.h"

#include "ocean/math/Box3.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements a container for camera poses.
 * The object is thread-safe.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT CameraPoses
{
	public:

		/**
		 * Definition of an unordered map mapping frame indices to camera poses.
		 */
		using CameraPoseMap = std::unordered_map<Index32, SharedCameraPose>;

		/// Invalid frame index constant.
		static constexpr Index32 invalidFrameIndex_ = Index32(-1);

	public:

		/**
		 * Creates a new container object.
		 * Call nextFrame() to activate the container for the first time and set the current frame index to 0.
		 * @see nextFrame().
		 */
		CameraPoses();

		/**
		 * Returns the current frame index.
		 * @return The current frame index, with range [0, infinity)
		 */
		inline Index32 frameIndex() const;

		/**
		 * Returns the next frame index.
		 * This function can be called even if the container has not been activated yet.
		 * @return The next frame index, which is frameIndex() + 1, or 0 if the container has not been activated
		 */
		inline Index32 nextFrameIndex() const;

		/**
		 * Returns the index of the previous frame.
		 * @return The index of the previous frame, which is frameIndex() - 1, -1 if no previous frame exists
		 */
		inline Index32 previousFrameIndex() const;

		/**
		 * Returns the index of the last valid camera pose.
		 * @return The index of the last valid camera pose, with range [0, frameIndex()], -1 if no valid camera pose exists
		 */
		inline Index32 lastValidPoseFrameIndex() const;

		/**
		 * Returns the indices of all valid camera poses within a specified range.
		 * The function returns the indices of all valid camera poses within the range [max(0, lastFrameIndex - numberFrames + 1), lastFrameIndex].
		 * @param lastFrameIndex The last frame index for which the valid camera poses will be returned, with range [0, infinity)
		 * @param numberFrames The number of frames for which the valid camera poses will be returned, with range [1, infinity)
		 * @return The indices of all valid camera poses
		 */
		Indices32 validPoseFrameIndices(const Index32 lastFrameIndex, const Index32 numberFrames) const;

		/**
		 * Returns the camera pose for a specific frame index.
		 * @param frameIndex The index of the frame for which the camera pose will be returned, with range [0, infinity)
		 * @return The camera pose for the specified frame index, may be invalid
		 */
		inline SharedCameraPose pose(const Index32 frameIndex) const;

		/**
		 * Returns whether this container holds a valid camera pose for a specific frame index.
		 * @param frameIndex The index of the frame for which the camera pose will be returned, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool hasPose(const Index32 frameIndex) const;

		/**
		 * Returns whether this container holds a valid camera pose for a specific frame index.
		 * @param frameIndex The index of the frame for which the camera pose will be returned, with range [0, infinity)
		 * @param cameraPose The resulting camera pose for the specified frame index, must be initialized with nullptr
		 * @return True, if so
		 */
		bool hasPose(const Index32 frameIndex, SharedCameraPose& cameraPose) const;

		/**
		 * Returns the transformation between camera and world for a specific frame index.
		 * The default camera points towards the negative z-space with y-axis upwards.
		 * @param frameIndex The index of the frame for which the transformation will be returned, with range [0, infinity)
		 * @return The camera-to-world transformation, invalid if no pose exists for the specified frame
		 */
		inline HomogenousMatrix4 world_T_camera(const Index32 frameIndex) const;

		/**
		 * Returns the transformation between world and flipped camera for a specific frame index.
		 * The default flipped camera points towards the positive z-space with y-axis downwards.
		 * @param frameIndex The index of the frame for which the transformation will be returned, with range [0, infinity)
		 * @return The world-to-flipped-camera transformation, invalid if no pose exists for the specified frame
		 */
		inline HomogenousMatrix4 flippedCamera_T_world(const Index32 frameIndex) const;

		/**
		 * Sets or updates the camera pose for a specific frame index.
		 * @param frameIndex The index of the frame for which the camera pose will be updated, with range [0, frameIndex()]
		 * @param cameraPose The new camera pose to set, must be valid
		 * @param mapVersion The version of the map associated with this pose
		 */
		void setPose(const Index32 frameIndex, SharedCameraPose&& cameraPose, const Index32 mapVersion);

		/**
		 * Increases the frame index by one and makes the current camera pose the previous camera pose.
		 * Further, this function needs to be called once before the first camera pose can be accessed.
		 */
		void nextFrame();

		/**
		 * Removes all poses.
		 * The frame index will be untouched.
		 */
		void removePoses();

		/**
		 * Returns the 3D bounding box enclosing the positions/translations of all camera poses.
		 * @return The 3D bounding box
		 */
		Box3 boundingBox() const;

		/**
		 * Returns the number of camera poses stored in this container.
		 * The number of poses may be smaller than the frame index, as the container holds only valid camera poses.
		 * @return The container's size, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Returns whether this container holds no valid camera pose.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this container has been activated.
		 * The container is activated after the first call of nextFrame().
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Returns whether this container has been activated.
		 * The container is activated after the first call of nextFrame().
		 * This is a lock-free version of isValid().
		 * @return True, if so
		 */
		inline bool isValidLockFree() const;

	protected:

		/// The current frame index, with range [0, infinity), -1 before the first call of nextFrame().
		Index32 frameIndex_ = Index32(-1);

		/// The frame index of the last valid camera pose, with range [0, frameIndex()], -1 if no valid camera pose exists.
		Index32 lastValidPoseFrameIndex_ = Index32(-1);

		/// The map mapping frame indices to camera poses.
		CameraPoseMap cameraPoseMap_;

		/// The mutex of this object.
		mutable Mutex mutex_;
};

inline Index32 CameraPoses::frameIndex() const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	return frameIndex_;
}

inline Index32 CameraPoses::nextFrameIndex() const
{
	const ReadLock readLock(mutex_);

	ocean_assert(frameIndex_ != Index32(-1) || frameIndex_ + 1u == 0u); // needs to work even if object is invalid

	return Index32(frameIndex_ + 1u);
}

inline Index32 CameraPoses::previousFrameIndex() const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	if (frameIndex_ == Index32(-1) || frameIndex_ == 0u)
	{
		return Index32(-1);
	}

	return Index32(frameIndex_ - 1u);
}

inline Index32 CameraPoses::lastValidPoseFrameIndex() const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	return lastValidPoseFrameIndex_;
}

inline SharedCameraPose CameraPoses::pose(const Index32 frameIndex) const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	const CameraPoseMap::const_iterator iCameraPose = cameraPoseMap_.find(frameIndex);

	if (iCameraPose == cameraPoseMap_.cend())
	{
		return nullptr;
	}

	return iCameraPose->second;
}

inline bool CameraPoses::hasPose(const Index32 frameIndex) const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	return cameraPoseMap_.find(frameIndex) != cameraPoseMap_.cend();
}

inline HomogenousMatrix4 CameraPoses::world_T_camera(const Index32 frameIndex) const
{
	SharedCameraPose cameraPose;
	if (!hasPose(frameIndex, cameraPose))
	{
		return HomogenousMatrix4(false);
	}

	return cameraPose->world_T_camera();
}

inline HomogenousMatrix4 CameraPoses::flippedCamera_T_world(const Index32 frameIndex) const
{
	SharedCameraPose cameraPose;
	if (!hasPose(frameIndex, cameraPose))
	{
		return HomogenousMatrix4(false);
	}

	return cameraPose->flippedCamera_T_world();
}

inline size_t CameraPoses::size() const
{
	const ReadLock readLock(mutex_);

	ocean_assert(isValidLockFree());

	return cameraPoseMap_.size();
}

inline bool CameraPoses::isEmpty() const
{
	const ReadLock readLock(mutex_);

	return cameraPoseMap_.empty();
}

inline bool CameraPoses::isValid() const
{
	const ReadLock readLock(mutex_);

	return isValidLockFree();
}

inline bool CameraPoses::isValidLockFree() const
{
	return frameIndex_ != Index32(-1);
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_CAMERA_POSES_H
