/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FRAME_COLLECTION_H
#define META_OCEAN_MEDIA_FRAME_COLLECTION_H

#include "ocean/media/Media.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RingMap.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Media
{

/**
 * This class implements a simple buffer holding several frames combined with their timestamps and optional camera calibrations.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FrameCollection
{
	protected:

		/**
		 * Definition of a pair combining a frame with a camera profile.
		 */
		using FrameCameraPair = std::pair<FrameRef, SharedAnyCamera>;

		/**
		 * Definition of a ring map mapping timestamps to frame pairs.
		 */
		using RingMap = RingMapT<Timestamp, FrameCameraPair, true /*thread-safe*/, true /*ordered keys*/>;

	public:

		/**
		 * Creates an empty frame collection able to hold one frame.
		 */
		FrameCollection() = default;

		/**
		 * Move constructor.
		 * @param frameCollection Frame collection to move
		 */
		FrameCollection(FrameCollection&& frameCollection) = default;

		/**
		 * Copy constructor.
		 * @param frameCollection Frame collection to copy
		 */
		FrameCollection(const FrameCollection& frameCollection) = default;

		/**
		 * Creates a new frame collection.
		 * @param capacity Number of maximal frames this object can store [1, infinity)
		 */
		explicit FrameCollection(const size_t capacity);

		/**
		 * Destructs a frame collection.
		 */
		~FrameCollection();

		/**
		 * Returns the capacity of this frame collection object.
		 * @return The frame collection object's capacity, with range [0, infinity)
		 */
		inline size_t capacity() const;

		/**
		 * Returns the most recent frame.
		 * @param anyCamera Optional resulting camera if known; nullptr if not of interest
		 * @return Most recent frame
		 */
		FrameRef recent(SharedAnyCamera* anyCamera = nullptr) const;

		/**
		 * Returns the frame with a specific timestamp.
		 * If no frame with the given timestamp exists, the most recent frame will be returned.
		 * @param timestamp The timestamp of the frame to return
		 * @param anyCamera Optional resulting camera if known; nullptr if not of interest
		 * @return Frame with the specific timestamp
		 */
		FrameRef get(const Timestamp timestamp, SharedAnyCamera* anyCamera = nullptr) const;

		/**
		 * Returns whether a frame with specific timestamp is currently stored inside the frame collection.
		 * @param timestamp The timestamp to be checked
		 * @return True, if so
		 */
		bool has(const Timestamp timestamp) const;

		/**
		 * Sets a new frame and overwrites the oldest frame.
		 * @param frame The frame to set, a copy will be created
		 * @param anyCamera Optional camera profile associated with the given frame, nullptr if unknown
		 * @return Reference to the stored frame, if succeeded
		 */
		FrameRef set(const Frame& frame, SharedAnyCamera anyCamera = nullptr);

		/**
		 * Sets a new frame and overwrites the oldest frame.
		 * @param frame The frame to set
		 * @param anyCamera Optional camera profile associated with the given frame, nullptr if unknown
		 * @return Reference to the stored frame, if succeeded
		 */
		FrameRef set(Frame&& frame, SharedAnyCamera anyCamera = nullptr);

		/**
		 * Sets or changes the capacity of this frame collection.
		 * In case the capacity is reduced, the oldest frames will be removed
		 * @param capacity The capacity to set, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool setCapacity(const size_t capacity);

		/**
		 * Returns whether the frame collection is empty.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Removes all frames from this frame collection.
		 * The capacity will be kept.
		 */
		void clear();

		/**
		 * Returns whether the frame collection is not empty.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Default move operator.
		 * @param frameCollection Frame collection to move
		 */
		FrameCollection& operator=(FrameCollection&& frameCollection) = default;

		/**
		 * Default assign operator.
		 * @param frameCollection Frame collection to assign
		 */
		FrameCollection& operator=(const FrameCollection& frameCollection) = default;

	protected:

		/// The ring map mapping timestamps to frame/camera pairs.
		RingMap ringMap_ = RingMap(1);
};

inline size_t FrameCollection::capacity() const
{
	return ringMap_.capacity();
}

inline bool FrameCollection::isNull() const
{
	return ringMap_.isEmpty();
}

inline FrameCollection::operator bool() const
{
	return !ringMap_.isEmpty();
}

}

}

#endif // META_OCEAN_MEDIA_FRAME_COLLECTION_H
