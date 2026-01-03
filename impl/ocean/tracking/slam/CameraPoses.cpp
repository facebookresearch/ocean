/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/CameraPoses.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

CameraPoses::CameraPoses()
{
	cameraPoseMap_.reserve(512);
}

Indices32 CameraPoses::validPoseFrameIndices(const Index32 lastFrameIndex, const Index32 numberFrames) const
{
	ocean_assert(numberFrames != 0u);

	const ReadLock readLock(mutex_, "CameraPoses::hasPose()");

	ocean_assert(isValidLockFree());

	const Index32 firstFrameIndex = Index32(std::max(int64_t(0), int64_t(lastFrameIndex) - int64_t(numberFrames) + 1));

	if (firstFrameIndex > lastValidPoseFrameIndex_)
	{
		return Indices32();
	}

	const Index32 endFrameIndex = std::min(lastFrameIndex + 1u, lastValidPoseFrameIndex_ + 1u);

	Indices32 result;
	result.reserve(numberFrames);

	for (Index32 frameIndex = firstFrameIndex; frameIndex < endFrameIndex; ++frameIndex)
	{
		if (cameraPoseMap_.find(frameIndex) != cameraPoseMap_.cend())
		{
			result.push_back(frameIndex);
		}
	}

	ocean_assert(result.size() <= numberFrames);

	return result;
}

bool CameraPoses::hasPose(const Index32 frameIndex, SharedCameraPose& cameraPose) const
{
	const ReadLock readLock(mutex_, "CameraPoses::hasPose()");

	ocean_assert(isValidLockFree());
	ocean_assert(cameraPose == nullptr);

	const CameraPoseMap::const_iterator iCameraPose = cameraPoseMap_.find(frameIndex);

	if (iCameraPose == cameraPoseMap_.cend())
	{
		return false;
	}

	ocean_assert(iCameraPose->second != nullptr);

	cameraPose = iCameraPose->second;

	return true;
}

void CameraPoses::setPose(const Index32 frameIndex, SharedCameraPose&& cameraPose, const Index32 mapVersion)
{
	ocean_assert(cameraPose && cameraPose->isValid());

	const WriteLock writeLock(mutex_, "CameraPoses::setPose()");

	ocean_assert(isValidLockFree());

	ocean_assert(frameIndex <= frameIndex_);
	ocean_assert(mapVersion != Index32(-1));

	SharedCameraPose& newCameraPose = cameraPoseMap_[frameIndex];
	newCameraPose = std::move(cameraPose);
	newCameraPose->setMapVersion(mapVersion);

	if (lastValidPoseFrameIndex_ == Index32(-1) || frameIndex > lastValidPoseFrameIndex_)
	{
		lastValidPoseFrameIndex_ = frameIndex;
	}
}

void CameraPoses::nextFrame()
{
	const WriteLock writeLock(mutex_);

	++frameIndex_;
}

void CameraPoses::removePoses()
{
	const WriteLock writeLock(mutex_);

	lastValidPoseFrameIndex_ = Index32(-1);

	cameraPoseMap_.clear();
}

Box3 CameraPoses::boundingBox() const
{
	const ReadLock readLock(mutex_);

	Box3 box;

	for (CameraPoseMap::const_iterator iPose = cameraPoseMap_.cbegin(); iPose != cameraPoseMap_.cend(); ++iPose)
	{
		if (iPose->second)
		{
			const HomogenousMatrix4& world_T_camera = iPose->second->world_T_camera();
			ocean_assert(world_T_camera.isValid());

			box += world_T_camera.translation();
		}
	}

	return box;
}

}

}

}
