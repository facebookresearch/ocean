/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameProviderInterface.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace CV
{

FrameProviderInterface::~FrameProviderInterface()
{
	ocean_assert(frameCallbacks_.isEmpty());
	ocean_assert(frameNumberCallbacks_.isEmpty());
	ocean_assert(frameTypeCallbacks_.isEmpty());
}

bool FrameProviderInterface::synchronInitializationRequest(const double timeout, bool* abort)
{
	if (isInitialized())
	{
		return true;
	}

	const Timestamp startTimestamp(true);

	while ((!abort || !*abort) && startTimestamp + timeout > Timestamp(true))
	{
		if (isInitialized())
		{
			return true;
		}

		Thread::sleep(1);
	}

	return false;
}

void FrameProviderInterface::frameCacheRequest(const unsigned int /*index*/, const int /*range*/)
{
	// nothing to do here, provide an implementation in a derived class if desired
}

void FrameProviderInterface::registerFrameCallback(const FrameCallback& callback)
{
	frameCallbacks_.addCallback(callback);
}

void FrameProviderInterface::unregisterFrameCallback(const FrameCallback& callback)
{
	frameCallbacks_.removeCallback(callback);
}

void FrameProviderInterface::registerFrameNumberCallback(const FrameNumberCallback& callback)
{
	frameNumberCallbacks_.addCallback(callback);
}

void FrameProviderInterface::unregisterFrameNumberCallback(const FrameNumberCallback& callback)
{
	frameNumberCallbacks_.removeCallback(callback);
}

void FrameProviderInterface::registerFrameTypeCallback(const FrameTypeCallback& callback)
{
	frameTypeCallbacks_.addCallback(callback);
}

void FrameProviderInterface::unregisterFrameTypeCallback(const FrameTypeCallback& callback)
{
	frameTypeCallbacks_.removeCallback(callback);
}

void FrameProviderInterface::release()
{
	// nothing to do here
}

}

}
