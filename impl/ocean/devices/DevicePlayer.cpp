// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/DevicePlayer.h"

namespace Ocean
{

namespace Devices
{

DevicePlayer::~DevicePlayer()
{
	// nothing to do here
}

Media::FrameMediumRefs DevicePlayer::frameMediums()
{
	return Media::FrameMediumRefs();
}

bool DevicePlayer::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return !filename_.empty();
}

}

}
