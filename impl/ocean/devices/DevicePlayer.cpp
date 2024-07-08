/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
