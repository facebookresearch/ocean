/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/NetworkResource.h"

namespace Ocean
{

namespace Network
{

NetworkResource::Manager::Manager()
{
	// nothing to do here
}

bool NetworkResource::Manager::increase()
{
	const ScopedLock scopedLock(lock_);

	return resourceCounter_++ == 0u;
}

bool NetworkResource::Manager::decrease()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(resourceCounter_ != 0u);
	return --resourceCounter_ == 0u;
}

NetworkResource::NetworkResource()
{

#ifdef _WINDOWS

	if (Manager::get().increase())
	{
		WSADATA wsaData;
		const int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		ocean_assert_and_suppress_unused(result == 0, result);
	}

#endif

}

NetworkResource::~NetworkResource()
{

#ifdef _WINDOWS

	if (Manager::get().decrease())
	{
		const int result = WSACleanup();
		ocean_assert_and_suppress_unused(result == 0, result);
	}

#endif

}

}

}
