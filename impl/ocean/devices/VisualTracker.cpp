/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

VisualTracker::VisualTracker(const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type)
{
	// nothing to do here
}

VisualTracker::~VisualTracker()
{
	// nothing to do here
}

void VisualTracker::setInput(Media::FrameMediumRefs&& frameMediums)
{
	for (const Media::FrameMediumRef& frameMedium : frameMediums)
	{
		if (frameMedium.isNull())
		{
			ocean_assert(false && "Invalid frame medium");
			return;
		}
	}

	const ScopedLock scopedLock(deviceLock);

	frameMediums_ = std::move(frameMediums);
}

}

}
