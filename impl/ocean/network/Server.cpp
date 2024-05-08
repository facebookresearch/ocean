/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/Server.h"

namespace Ocean
{

namespace Network
{

Server::Server()
{
	// nothing to do here
}

Server::~Server()
{
	stop();
}

bool Server::start()
{
	const ScopedLock scopedLock(lock_);

	if (schedulerIsActive_)
	{
		return true;
	}

	if (buffer_.empty())
	{
		buffer_.resize(min(maximalMessageSize(), size_t(262144)));
	}

	schedulerIsActive_ = true;

	return true;
}

bool Server::stop()
{
	const ScopedLock scopedLock(lock_);

	schedulerIsActive_ = false;
	return true;
}

}

}
