/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/FramePyramidManager.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

void FramePyramidManager::ScopedPyramid::release()
{
	if (pyramid_ != nullptr)
	{
		ocean_assert(owner_ != nullptr);
		owner_->unlockPyramid(frameIndex_);

		owner_ = nullptr;
		pyramid_ = nullptr;
		frameIndex_ = Index32(-1);
	}
}

FramePyramidManager::ScopedPyramid& FramePyramidManager::ScopedPyramid::operator=(ScopedPyramid&& scopedPyramid) noexcept
{
	if (this != &scopedPyramid)
	{
		release();

		owner_ = scopedPyramid.owner_;
		pyramid_ = scopedPyramid.pyramid_;
		frameIndex_ = scopedPyramid.frameIndex_;

		scopedPyramid.owner_ = nullptr;
		scopedPyramid.pyramid_ = nullptr;
		scopedPyramid.frameIndex_ = Index32(-1);
	}

	return *this;
}

FramePyramidManager::ScopedPyramid FramePyramidManager::newPyramid(const Index32 frameIndex)
{
	ocean_assert(frameIndex != Index32(-1));

	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_DEBUG

	for (const SharedObject& object : usedObjects_)
	{
		ocean_assert(object);
		ocean_assert(object->frameIndex_ != frameIndex);
	}

#endif // OCEAN_DEBUG

	Object* object = nullptr;

	if (!freeObjects_.empty())
	{
		// we re-used an existing object
		usedObjects_.emplaceBack(std::move(freeObjects_.back()));
		object = usedObjects_.back().get();

		freeObjects_.popBack();
	}
	else
	{
		// we have to create a new object
		usedObjects_.emplaceBack(std::make_shared<Object>());
		object = usedObjects_.back().get();
	}

	ocean_assert(object->frameIndex_ == Index32(-1));
	object->frameIndex_ = frameIndex;

	ocean_assert(object->usageCounter_ == 0u);
	object->usageCounter_ = 1u;

	return ScopedPyramid(*this, object->framePyramid_, frameIndex);
}

FramePyramidManager::ScopedPyramid FramePyramidManager::existingPyramid(const Index32 frameIndex)
{
	ocean_assert(frameIndex != Index32(-1));

	const ScopedLock scopedLock(lock_);

	for (const SharedObject& object : usedObjects_)
	{
		ocean_assert(object);

		if (object->frameIndex_ == frameIndex)
		{
			ocean_assert(object->usageCounter_ >= 1u);
			++object->usageCounter_;

			return ScopedPyramid(*this, object->framePyramid_, frameIndex);
		}
	}

	Log::error() << "FramePyramidManager: The pyramid does not exist!";
	ocean_assert(false && "The pyramid does not exist!");

	return ScopedPyramid();
}

FramePyramidManager::ScopedPyramid FramePyramidManager::latestPyramid()
{
	const ScopedLock scopedLock(lock_);

	if (latestFrameIndex_ == Index32(-1))
	{
		return ScopedPyramid();
	}

	return existingPyramid(latestFrameIndex_);
}

void FramePyramidManager::updateLatest(const Index32 frameIndex)
{
	const ScopedLock scopedLock(lock_);

	if (latestFrameIndex_ != Index32(-1))
	{
		ocean_assert(frameIndex != latestFrameIndex_);

		unlockPyramid(latestFrameIndex_);
	}

	for (const SharedObject& object : usedObjects_)
	{
		ocean_assert(object);

		if (object->frameIndex_ == frameIndex)
		{
			ocean_assert(object->usageCounter_ >= 1u);
			++object->usageCounter_;

			latestFrameIndex_ = frameIndex;

			return;
		}
	}

	ocean_assert(false && "The pyramid does not exist!");
}

void FramePyramidManager::unlockPyramid(const Index32 frameIndex)
{
	ocean_assert(frameIndex != Index32(-1));

	const ScopedLock scopedLock(lock_);

	for (size_t nObject = 0; nObject < usedObjects_.size(); ++nObject)
	{
		SharedObject& object = usedObjects_[nObject];

		ocean_assert(object);

		if (object->frameIndex_ == frameIndex)
		{
			ocean_assert(object->usageCounter_ >= 1u);
			--object->usageCounter_;

			if (object->usageCounter_ == 0u)
			{
				object->frameIndex_ = Index32(-1);

				freeObjects_.emplaceBack(std::move(object));

				usedObjects_[nObject] = std::move(usedObjects_.back());
				usedObjects_.popBack();
			}

			return;
		}
	}

	ocean_assert(false && "The pyramid is not locked!");
}

unsigned int FramePyramidManager::idealPyramidLayers(const unsigned int width, const unsigned int height, const unsigned int patchSize, const float maximalTrackingDistance, const unsigned int coarseLayerRadius)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(patchSize >= 1u);
	ocean_assert(maximalTrackingDistance > 0.0f);
	ocean_assert(coarseLayerRadius >= 1u);

	const float diagonal = NumericF::sqrt(float(width * width) + float(height * height));

	const unsigned int trackingDistance = (unsigned int)(diagonal * maximalTrackingDistance + 0.5f);

	const unsigned int invalidLayerResolution = std::max(patchSize * 2u, 32u);

	const unsigned int pyramidLayers = CV::FramePyramid::idealLayers(width, height, invalidLayerResolution, invalidLayerResolution, 2u, trackingDistance, coarseLayerRadius);
	ocean_assert(pyramidLayers != 0u);

	const unsigned int actualTrackingDistance = coarseLayerRadius * (1u << (pyramidLayers - 1u));

	if (actualTrackingDistance < trackingDistance)
	{
		Log::warning() << "The actual tracking distance " << actualTrackingDistance << " is smaller than the maximal tracking distance " << trackingDistance;
		Log::warning() << "Patch size: " << patchSize << ", maximal tracking distance: " << String::toAString(maximalTrackingDistance * 100.0f, 1u) << "%, coarse layer radius: " << coarseLayerRadius;

		const unsigned int coarsestSizeFactor = CV::FramePyramid::sizeFactor(pyramidLayers - 1u);

		const unsigned int coarsestWidth = width / coarsestSizeFactor;
		const unsigned int coarsestHeight = height / coarsestSizeFactor;

		Log::warning() << "Resulting layers: " << pyramidLayers;
		Log::warning() << "Finest resolution " << width << "x" << height << ", coarsest resolution " << coarsestWidth << "x" << coarsestHeight;
	}

	return pyramidLayers;
}

}

}

}
