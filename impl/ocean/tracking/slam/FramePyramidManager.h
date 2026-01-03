/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_FRAME_PYRAMID_MANAGER_H
#define META_OCEAN_TRACKING_SLAM_FRAME_PYRAMID_MANAGER_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/base/Lock.h"
#include "ocean/base/StaticVector.h"

#include "ocean/cv/FramePyramid.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class manages a pool of frame pyramids for efficient reuse.
 * The manager provides thread-safe access to frame pyramids with automatic lifetime management.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT FramePyramidManager
{
	public:

		/**
		 * This class implements a scoped pyramid object providing automatic lifetime management.
		 * The pyramid is automatically released when the scoped object is destroyed.
		 */
		class ScopedPyramid
		{
			friend class FramePyramidManager;

			public:

				/**
				 * Creates an invalid scoped pyramid object.
				 */
				ScopedPyramid() = default;

				/**
				 * Move constructor.
				 * @param scopedPyramid The scoped pyramid object to move
				 */
				inline ScopedPyramid(ScopedPyramid&& scopedPyramid) noexcept;

				/**
				 * Destructs the scoped pyramid and releases the pyramid.
				 */
				inline ~ScopedPyramid();

				/**
				 * Returns the frame index of this pyramid.
				 * @return The frame index
				 */
				inline Index32 frameIndex() const;

				/**
				 * Returns a const reference to the pyramid.
				 * @return The pyramid object
				 */
				inline const CV::FramePyramid& pyramid() const;

				/**
				 * Returns the finest layer of the pyramid.
				 * @return The finest layer
				 */
				inline const Frame& finestLayer() const;

				/**
				 * Returns a reference to the pyramid.
				 * @return The pyramid object
				 */
				inline CV::FramePyramid& pyramid();

				/**
				 * Returns the finest layer of the pyramid.
				 * @return The finest layer
				 */
				inline Frame& finestLayer();

				/**
				 * Explicitly releases the pyramid before the scoped object is destroyed.
				 */
				void release();

				/**
				 * Returns whether this scoped pyramid is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether this scoped pyramid is valid.
				 * @return True, if so
				 */
				inline operator bool() const;

				/**
				 * Returns a const reference to the pyramid.
				 * @return The pyramid object
				 */
				inline const CV::FramePyramid& operator*() const;

				/**
				 * Returns a reference to the pyramid.
				 * @return The pyramid object
				 */
				inline CV::FramePyramid& operator*();

				/**
				 * Returns a const pointer to the pyramid.
				 * @return The pyramid object
				 */
				inline const CV::FramePyramid* operator->() const;

				/**
				 * Returns a pointer to the pyramid.
				 * @return The pyramid object
				 */
				inline CV::FramePyramid* operator->();

				/**
				 * Move assignment operator.
				 * @param scopedPyramid The scoped pyramid object to move
				 * @return Reference to this object
				 */
				ScopedPyramid& operator=(ScopedPyramid&& scopedPyramid) noexcept;

			protected:

				/**
				 * Creates a new scoped pyramid object.
				 * @param owner The owner manager
				 * @param pyramid The pyramid object
				 * @param frameIndex The frame index
				 */
				inline ScopedPyramid(FramePyramidManager& owner, CV::FramePyramid& pyramid, const Index32 frameIndex);

				/**
				 * Disabled copy constructor.
				 * @param scopedPyramid The scoped pyramid object to copy
				 */
				ScopedPyramid(const ScopedPyramid& scopedPyramid) = delete;

				/**
				 * Disabled copy assignment operator.
				 * @param scopedPyramid The scoped pyramid object to copy
				 * @return Reference to this object
				 */
				ScopedPyramid& operator=(const ScopedPyramid& scopedPyramid) = delete;

			protected:

				/// The owner manager.
				FramePyramidManager* owner_ = nullptr;

				/// The pyramid object.
				CV::FramePyramid* pyramid_ = nullptr;

				/// The frame index.
				Index32 frameIndex_ = Index32(-1);
		};

	protected:

		/**
		 * This class implements a pyramid object with usage counter.
		 */
		class Object
		{
			public:

				/// The frame index, -1 if unused.
				Index32 frameIndex_ = Index32(-1);

				/// The usage counter.
				unsigned int usageCounter_ = 0u;

				/// The actual frame pyramid.
				CV::FramePyramid framePyramid_;
		};

		/// Definition of a shared pointer to an object.
		using SharedObject = std::shared_ptr<Object>;

		/// Definition of a static vector holding object pointers.
		using Objects = StaticVector<SharedObject, 8>;

	public:

		/**
		 * Creates a new manager object.
		 */
		FramePyramidManager() = default;

		/**
		 * Creates a new pyramid for a given frame index.
		 * @param frameIndex The frame index for which a new pyramid will be created, must be valid
		 * @return The scoped pyramid object
		 */
		ScopedPyramid newPyramid(const Index32 frameIndex);

		/**
		 * Returns an existing pyramid for a given frame index.
		 * @param frameIndex The frame index for which the pyramid will be returned, must be valid
		 * @return The scoped pyramid object, invalid if the pyramid does not exist
		 */
		ScopedPyramid existingPyramid(const Index32 frameIndex);

		/**
		 * Returns the latest pyramid.
		 * @return The scoped pyramid object, invalid if no latest pyramid exists
		 */
		ScopedPyramid latestPyramid();

		/**
		 * Updates the latest pyramid to a given frame index.
		 * The pyramid for the given frame index must exist.
		 * @param frameIndex The frame index of the pyramid to be set as latest, must be valid
		 */
		void updateLatest(const Index32 frameIndex);

		/**
		 * Unlocks a pyramid for a given frame index.
		 * @param frameIndex The frame index of the pyramid to unlock, must be valid
		 */
		void unlockPyramid(const Index32 frameIndex);

		/**
		 * Returns the number of currently used pyramids.
		 * @return The number of pyramids
		 */
		inline size_t size() const;

		/**
		 * Determines the ideal number of pyramid layers for given parameters.
		 * @param width The width of the frame in pixels, with range [1, infinity)
		 * @param height The height of the frame in pixels, with range [1, infinity)
		 * @param patchSize The size of the patch in pixels, with range [1, infinity)
		 * @param maximalTrackingDistance The maximal tracking distance as fraction of the frame resolution, with range (0, infinity)
		 * @param coarseLayerRadius The search radius on the coarsest pyramid layer, with range [1, infinity)
		 * @return The ideal number of pyramid layers
		 */
		static unsigned int idealPyramidLayers(const unsigned int width, const unsigned int height, const unsigned int patchSize, const float maximalTrackingDistance, const unsigned int coarseLayerRadius);

	protected:

		/// The frame index of the latest pyramid, -1 if invalid.
		Index32 latestFrameIndex_ = Index32(-1);

		/// The vector of used pyramid objects.
		Objects usedObjects_;

		/// The vector of free pyramid objects.
		Objects freeObjects_;

		/// The lock object.
		mutable Lock lock_;
};

inline size_t FramePyramidManager::size() const
{
	const ScopedLock scopedLock(lock_);

	return usedObjects_.size();
}

inline FramePyramidManager::ScopedPyramid::ScopedPyramid(ScopedPyramid&& scopedPyramid) noexcept
{
	*this = std::move(scopedPyramid);
}

inline FramePyramidManager::ScopedPyramid::~ScopedPyramid()
{
	release();
}

inline Index32 FramePyramidManager::ScopedPyramid::frameIndex() const
{
	return frameIndex_;
}

inline const CV::FramePyramid& FramePyramidManager::ScopedPyramid::pyramid() const
{
	ocean_assert(pyramid_);
	return *pyramid_;
}

inline const Frame& FramePyramidManager::ScopedPyramid::finestLayer() const
{
	ocean_assert(pyramid_);
	return pyramid_->finestLayer();
}

inline CV::FramePyramid& FramePyramidManager::ScopedPyramid::pyramid()
{
	ocean_assert(pyramid_);
	return *pyramid_;
}

inline Frame& FramePyramidManager::ScopedPyramid::finestLayer()
{
	return pyramid_->finestLayer();
}

inline bool FramePyramidManager::ScopedPyramid::isValid() const
{
	return pyramid_ != nullptr;
}

inline FramePyramidManager::ScopedPyramid::operator bool() const
{
	return isValid();
}

inline const CV::FramePyramid& FramePyramidManager::ScopedPyramid::operator*() const
{
	ocean_assert(pyramid_);
	return *pyramid_;
}

inline CV::FramePyramid& FramePyramidManager::ScopedPyramid::operator*()
{
	ocean_assert(pyramid_);
	return *pyramid_;
}

inline const CV::FramePyramid* FramePyramidManager::ScopedPyramid::operator->() const
{
	ocean_assert(pyramid_);
	return pyramid_;
}

inline CV::FramePyramid* FramePyramidManager::ScopedPyramid::operator->()
{
	ocean_assert(pyramid_);
	return pyramid_;
}

inline FramePyramidManager::ScopedPyramid::ScopedPyramid(FramePyramidManager& owner, CV::FramePyramid& pyramid, const Index32 frameIndex) :
	owner_(&owner),
	pyramid_(&pyramid),
	frameIndex_(frameIndex)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_FRAME_PYRAMID_MANAGER_H
