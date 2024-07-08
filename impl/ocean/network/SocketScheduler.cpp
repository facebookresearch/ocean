/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/SocketScheduler.h"
#include "ocean/network/Socket.h"

#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Network
{

SocketScheduler::SocketScheduler() :
	Thread("SocketScheduler thread")
{
	startThread();
}

SocketScheduler::~SocketScheduler()
{
	stopThread();

	ocean_assert(activeSockets_.empty());
	ocean_assert(unregisterSockets_.empty());

	const Timestamp startTimestamp(true);
	while (isThreadActive() && startTimestamp + 5.0 > Timestamp(true))
	{
		sleep(1u);
	}

	ocean_assert(!isThreadActive());
}

void SocketScheduler::registerSocket(Socket& socket)
{
	const ScopedLock scopedLock(lock_);
	registerSockets_.insert(&socket);
}

void SocketScheduler::unregisterSocket(Socket& socket)
{
	const ScopedLock scopedLock(lock_);

	// as the thread starts immediately in the constructor we can expect all sockets to be unregistered if the thread is not active anymore
	if (!isThreadInvokedToStart() && !isThreadActive())
	{
		activeSockets_.clear();
		unregisterSockets_.clear();
		return;
	}

	ocean_assert(unregisterSockets_.find(&socket) == unregisterSockets_.end());

	// the socket is either not in the list of sockets to be registered (and thus the socket must be active),
	// or the socket is still in the list of sockets to be registered (and thus the socket is not yet active - and will never become active due to this unregister call)
	ocean_assert((registerSockets_.find(&socket) == registerSockets_.end() && activeSockets_.find(&socket) != activeSockets_.end())
					|| (registerSockets_.find(&socket) != registerSockets_.end() && activeSockets_.find(&socket) == activeSockets_.end()));

	unregisterSockets_.insert(&socket);
}

bool SocketScheduler::isSocketUnregistered(Socket& socket) const
{
	const ScopedLock scopedLock(lock_);

	// as the thread starts immediately in the constructor we can expect all sockets to be unregistered if the thread is not active anymore
	if (!isThreadInvokedToStart() && !isThreadActive())
	{
		return true;
	}

	ocean_assert(registerSockets_.find(&socket) == registerSockets_.end() || (activeSockets_.find(&socket) == activeSockets_.end()));
	ocean_assert(activeSockets_.find(&socket) == activeSockets_.end() || unregisterSockets_.find(&socket) != unregisterSockets_.end());

	if (registerSockets_.find(&socket) != registerSockets_.end() || activeSockets_.find(&socket) != activeSockets_.end())
	{
		return false;
	}

	return unregisterSockets_.find(&socket) == unregisterSockets_.end();
}

void SocketScheduler::threadRun()
{
	while (!shouldThreadStop())
	{
		TemporaryScopedLock firstTemporaryLock(lock_);
			const SocketSet activeSockets(activeSockets_);
		firstTemporaryLock.release();

		bool busy = false;
		for (SocketSet::const_iterator i = activeSockets.begin(); i != activeSockets.end(); ++i)
		{
			busy = (*i)->onScheduler() || busy;
		}

		TemporaryScopedLock secondTemporaryLock(lock_);

			for (SocketSet::const_iterator i = registerSockets_.begin(); i != registerSockets_.end(); ++i)
			{
				ocean_assert(activeSockets_.find(*i) == activeSockets_.end());
				activeSockets_.insert(*i);
			}
			registerSockets_.clear();

			for (SocketSet::const_iterator i = unregisterSockets_.begin(); i != unregisterSockets_.end(); ++i)
			{
				ocean_assert(activeSockets_.find(*i) != activeSockets_.end());
				activeSockets_.erase(*i);
			}
			unregisterSockets_.clear();

		secondTemporaryLock.release();

		// we sleep the thread only if we are not busy
		if (!busy)
		{
			sleep(1);
		}
	}
}

}

}
