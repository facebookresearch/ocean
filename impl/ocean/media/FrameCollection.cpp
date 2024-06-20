/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/FrameCollection.h"

namespace Ocean
{

namespace Media
{

FrameCollection::FrameCollection(const size_t capacity) :
	ringMap_(max(size_t(1), capacity))
{
	ocean_assert(capacity >= 1);
}

FrameCollection::~FrameCollection()
{
	clear();
}

FrameRef FrameCollection::recent(SharedAnyCamera* anyCamera) const
{
	FrameCameraPair frameCameraPair;

	if (ringMap_.highestElement(frameCameraPair))
	{
#ifdef OCEAN_DEBUG
		if (frameCameraPair.first && frameCameraPair.second)
		{
			const FrameRef& localFrame = frameCameraPair.first;
			const SharedAnyCamera& localAnyCamera = frameCameraPair.second;

			ocean_assert(localFrame->width() == localAnyCamera->width());
			ocean_assert(localFrame->height() == localAnyCamera->height());
		}
#endif

		if (anyCamera != nullptr)
		{
			*anyCamera = frameCameraPair.second;
		}

		return frameCameraPair.first;
	}

	return FrameRef();
}

FrameRef FrameCollection::get(const Timestamp timestamp, SharedAnyCamera* anyCamera) const
{
	FrameCameraPair frameCameraPair;

	if (ringMap_.element<RingMap::AM_MATCH_OR_HIGHEST>(timestamp, frameCameraPair))
	{
		if (anyCamera != nullptr)
		{
			*anyCamera = frameCameraPair.second;

#ifdef OCEAN_DEBUG
			if (*anyCamera)
			{
				ocean_assert(frameCameraPair.first->width() == (*anyCamera)->width());
				ocean_assert(frameCameraPair.first->height() == (*anyCamera)->height());
			}
#endif
		}

		return frameCameraPair.first;
	}

	return FrameRef();
}

bool FrameCollection::has(const Timestamp timestamp) const
{
	return ringMap_.hasElement(timestamp);
}

FrameRef FrameCollection::set(const Frame& frame, SharedAnyCamera anyCamera)
{
	ocean_assert(!anyCamera || anyCamera->width() == frame.width());
	ocean_assert(!anyCamera || anyCamera->height() == frame.height());

	FrameRef frameRef(new Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));

	ringMap_.insertElement(frame.timestamp(), FrameCameraPair(frameRef, std::move(anyCamera)), true /*forceOverwrite*/);

	return frameRef;
}

FrameRef FrameCollection::set(Frame&& frame, SharedAnyCamera anyCamera)
{
	ocean_assert(!anyCamera || anyCamera->width() == frame.width());
	ocean_assert(!anyCamera || anyCamera->height() == frame.height());

	const Timestamp timestamp = frame.timestamp();

	FrameRef frameRef(new Frame(std::move(frame)));

	ringMap_.insertElement(timestamp, FrameCameraPair(frameRef, std::move(anyCamera)), true /*forceOverwrite*/);

	return frameRef;
}

bool FrameCollection::setCapacity(const size_t capacity)
{
	if (capacity == 0)
	{
		ocean_assert(false && "Invalid capacity!");
		return false;
	}

	ringMap_.setCapacity(capacity);

	return true;
}

void FrameCollection::clear()
{
	ringMap_.clear();
}

}

}
